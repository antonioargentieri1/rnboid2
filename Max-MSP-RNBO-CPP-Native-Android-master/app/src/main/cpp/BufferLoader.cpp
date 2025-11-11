//
// BufferLoader.cpp - Audio Buffer Loading Implementation
//

#include "BufferLoader.h"
#include <cstring>
#include <cmath>

static const char* TAG = "BufferLoader";

// Callback to free buffer memory
void BufferLoader::freeBuffer(RNBO::ExternalDataId id, char* data) {
    __android_log_print(ANDROID_LOG_INFO, TAG, "Freeing buffer: %s", id);
    delete[] data;
}

// Byte order conversion helpers
uint32_t BufferLoader::swapEndian32(uint32_t value) {
    return ((value & 0xFF000000) >> 24) |
           ((value & 0x00FF0000) >> 8)  |
           ((value & 0x0000FF00) << 8)  |
           ((value & 0x000000FF) << 24);
}

uint16_t BufferLoader::swapEndian16(uint16_t value) {
    return ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
}

// Parse dependencies.json
bool BufferLoader::parseDependencies(
    AAssetManager* assetManager,
    std::vector<std::pair<std::string, std::string>>& buffers
) {
    AAsset* asset = AAssetManager_open(assetManager, "dependencies.json", AASSET_MODE_BUFFER);
    if (!asset) {
        __android_log_print(ANDROID_LOG_WARN, TAG, "dependencies.json not found - no buffers to load");
        return true; // Not an error, just no buffers
    }

    size_t length = AAsset_getLength(asset);
    std::vector<char> jsonData(length + 1);
    AAsset_read(asset, jsonData.data(), length);
    jsonData[length] = '\0';
    AAsset_close(asset);

    std::string json(jsonData.data());
    
    // Simple JSON parsing for dependencies.json format:
    // [{"id": "drums", "file": "media\\drumLoop.aif"}]
    
    size_t pos = 0;
    while ((pos = json.find("\"id\"", pos)) != std::string::npos) {
        // Find id value
        size_t idStart = json.find("\"", pos + 5);
        if (idStart == std::string::npos) break;
        idStart++;
        
        size_t idEnd = json.find("\"", idStart);
        if (idEnd == std::string::npos) break;
        
        std::string id = json.substr(idStart, idEnd - idStart);
        
        // Find file value
        size_t filePos = json.find("\"file\"", idEnd);
        if (filePos == std::string::npos) break;
        
        size_t fileStart = json.find("\"", filePos + 7);
        if (fileStart == std::string::npos) break;
        fileStart++;
        
        size_t fileEnd = json.find("\"", fileStart);
        if (fileEnd == std::string::npos) break;
        
        std::string file = json.substr(fileStart, fileEnd - fileStart);
        
        // Convert backslashes to forward slashes
        for (char& c : file) {
            if (c == '\\') c = '/';
        }
        
        buffers.push_back({id, file});
        __android_log_print(ANDROID_LOG_INFO, TAG, "Found buffer: %s -> %s", id.c_str(), file.c_str());
        
        pos = fileEnd;
    }
    
    return true;
}

// Decode AIFF format
bool BufferLoader::decodeAIFF(
    const uint8_t* fileData,
    size_t fileSize,
    float** outData,
    size_t* outNumSamples,
    int* outChannels,
    int* outSampleRate
) {
    if (fileSize < 12) return false;
    
    // Check FORM header
    if (memcmp(fileData, "FORM", 4) != 0) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Not a valid AIFF file: missing FORM header");
        return false;
    }
    
    // Check AIFF type
    if (memcmp(fileData + 8, "AIFF", 4) != 0 && memcmp(fileData + 8, "AIFC", 4) != 0) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Not a valid AIFF file: wrong type");
        return false;
    }
    
    size_t offset = 12;
    int channels = 0;
    int sampleRate = 0;
    int bitDepth = 0;
    size_t numFrames = 0;
    const uint8_t* audioData = nullptr;
    size_t audioDataSize = 0;
    
    // Parse chunks
    while (offset + 8 <= fileSize) {
        char chunkID[5] = {0};
        memcpy(chunkID, fileData + offset, 4);
        uint32_t chunkSize = swapEndian32(*(uint32_t*)(fileData + offset + 4));
        
        offset += 8;
        
        if (strcmp(chunkID, "COMM") == 0 && chunkSize >= 18) {
            // Common chunk
            channels = swapEndian16(*(uint16_t*)(fileData + offset));
            numFrames = swapEndian32(*(uint32_t*)(fileData + offset + 2));
            bitDepth = swapEndian16(*(uint16_t*)(fileData + offset + 6));
            
            // Parse sample rate (80-bit extended precision)
            const uint8_t* srData = fileData + offset + 8;
            uint16_t exponent = swapEndian16(*(uint16_t*)srData);
            uint32_t mantissa = swapEndian32(*(uint32_t*)(srData + 2));
            
            // Simplified conversion for common rates
            if (exponent == 0x400E) {
                sampleRate = 44100;
            } else if (exponent == 0x400D) {
                sampleRate = 22050;
            } else if (exponent == 0x400F) {
                sampleRate = 88200;
            } else {
                // Generic conversion
                int exp = (exponent & 0x7FFF) - 16383;
                sampleRate = (int)(mantissa * pow(2.0, exp - 31));
            }
            
            __android_log_print(ANDROID_LOG_INFO, TAG, 
                "AIFF: %d ch, %d Hz, %d bit, %zu frames",
                channels, sampleRate, bitDepth, numFrames);
        }
        else if (strcmp(chunkID, "SSND") == 0) {
            // Sound data chunk
            uint32_t ssndOffset = swapEndian32(*(uint32_t*)(fileData + offset));
            audioData = fileData + offset + 8 + ssndOffset;
            audioDataSize = chunkSize - 8 - ssndOffset;
        }
        
        offset += chunkSize;
        if (chunkSize % 2 == 1) offset++; // Pad byte
    }
    
    if (!audioData || channels == 0 || numFrames == 0) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Invalid AIFF: missing data");
        return false;
    }
    
    // Convert to float
    size_t totalSamples = numFrames * channels;
    float* floatData = new float[totalSamples];
    
    if (bitDepth == 16) {
        const int16_t* samples = (const int16_t*)audioData;
        for (size_t i = 0; i < totalSamples; i++) {
            int16_t sample = swapEndian16(samples[i]);
            floatData[i] = sample / 32768.0f;
        }
    }
    else if (bitDepth == 24) {
        for (size_t i = 0; i < totalSamples; i++) {
            int32_t sample = 0;
            const uint8_t* ptr = audioData + (i * 3);
            sample = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8);
            floatData[i] = sample / 2147483648.0f;
        }
    }
    else if (bitDepth == 32) {
        const int32_t* samples = (const int32_t*)audioData;
        for (size_t i = 0; i < totalSamples; i++) {
            int32_t sample = swapEndian32(samples[i]);
            floatData[i] = sample / 2147483648.0f;
        }
    }
    else {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Unsupported bit depth: %d", bitDepth);
        delete[] floatData;
        return false;
    }
    
    *outData = floatData;
    *outNumSamples = totalSamples;
    *outChannels = channels;
    *outSampleRate = sampleRate;
    
    return true;
}

// Decode WAV format (RIFF WAVE)
bool BufferLoader::decodeWAV(
    const uint8_t* fileData,
    size_t fileSize,
    float** outData,
    size_t* outNumSamples,
    int* outChannels,
    int* outSampleRate
) {
    if (fileSize < 44) return false;  // Minimum WAV header size

    // Check RIFF header
    if (memcmp(fileData, "RIFF", 4) != 0) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Not a valid WAV file: missing RIFF header");
        return false;
    }

    // Check WAVE format
    if (memcmp(fileData + 8, "WAVE", 4) != 0) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Not a valid WAV file: wrong format");
        return false;
    }

    size_t offset = 12;
    int channels = 0;
    int sampleRate = 0;
    int bitDepth = 0;
    int audioFormat = 0;
    const uint8_t* audioData = nullptr;
    size_t audioDataSize = 0;

    // Parse chunks
    while (offset + 8 <= fileSize) {
        char chunkID[5] = {0};
        memcpy(chunkID, fileData + offset, 4);
        uint32_t chunkSize = *(uint32_t*)(fileData + offset + 4);  // Little-endian (no swap needed)

        offset += 8;

        if (strcmp(chunkID, "fmt ") == 0 && chunkSize >= 16) {
            // Format chunk (little-endian)
            audioFormat = *(uint16_t*)(fileData + offset);      // 1 = PCM
            channels = *(uint16_t*)(fileData + offset + 2);
            sampleRate = *(uint32_t*)(fileData + offset + 4);
            // Skip byte rate (4 bytes) and block align (2 bytes)
            bitDepth = *(uint16_t*)(fileData + offset + 14);

            __android_log_print(ANDROID_LOG_INFO, TAG,
                "WAV: %d ch, %d Hz, %d bit, format=%d",
                channels, sampleRate, bitDepth, audioFormat);

            if (audioFormat != 1) {  // Only PCM supported
                __android_log_print(ANDROID_LOG_ERROR, TAG, "Unsupported WAV format: %d (only PCM supported)", audioFormat);
                return false;
            }
        }
        else if (strcmp(chunkID, "data") == 0) {
            // Data chunk
            audioData = fileData + offset;
            audioDataSize = chunkSize;
        }

        offset += chunkSize;
        if (chunkSize % 2 == 1) offset++;  // Pad byte
    }

    if (!audioData || channels == 0 || sampleRate == 0) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Invalid WAV: missing data or format");
        return false;
    }

    // Calculate number of samples
    size_t bytesPerSample = bitDepth / 8;
    size_t totalSamples = audioDataSize / bytesPerSample;

    // Convert to float
    float* floatData = new float[totalSamples];

    if (bitDepth == 16) {
        const int16_t* samples = (const int16_t*)audioData;
        for (size_t i = 0; i < totalSamples; i++) {
            // Little-endian, no swap needed on most platforms
            floatData[i] = samples[i] / 32768.0f;
        }
    }
    else if (bitDepth == 24) {
        for (size_t i = 0; i < totalSamples; i++) {
            int32_t sample = 0;
            const uint8_t* ptr = audioData + (i * 3);
            // Little-endian: LSB first
            sample = (ptr[2] << 24) | (ptr[1] << 16) | (ptr[0] << 8);
            floatData[i] = sample / 2147483648.0f;
        }
    }
    else if (bitDepth == 32) {
        const int32_t* samples = (const int32_t*)audioData;
        for (size_t i = 0; i < totalSamples; i++) {
            // Little-endian, no swap needed on most platforms
            floatData[i] = samples[i] / 2147483648.0f;
        }
    }
    else if (bitDepth == 8) {
        // 8-bit WAV is unsigned (0-255)
        const uint8_t* samples = (const uint8_t*)audioData;
        for (size_t i = 0; i < totalSamples; i++) {
            floatData[i] = (samples[i] - 128) / 128.0f;
        }
    }
    else {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Unsupported bit depth: %d", bitDepth);
        delete[] floatData;
        return false;
    }

    *outData = floatData;
    *outNumSamples = totalSamples;
    *outChannels = channels;
    *outSampleRate = sampleRate;

    return true;
}

// Load audio file from assets
bool BufferLoader::loadAudioFile(
    AAssetManager* assetManager,
    const std::string& filename,
    float** outData,
    size_t* outNumSamples,
    int* outChannels,
    int* outSampleRate
) {
    __android_log_print(ANDROID_LOG_INFO, TAG, "Loading: %s", filename.c_str());
    
    AAsset* asset = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_BUFFER);
    if (!asset) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to open: %s", filename.c_str());
        return false;
    }
    
    size_t length = AAsset_getLength(asset);
    std::vector<uint8_t> fileData(length);
    AAsset_read(asset, fileData.data(), length);
    AAsset_close(asset);
    
    __android_log_print(ANDROID_LOG_INFO, TAG, "File size: %zu bytes", length);
    
    // Decode based on file extension
    std::string ext = filename.substr(filename.find_last_of('.') + 1);

    if (ext == "aif" || ext == "aiff" || ext == "AIF" || ext == "AIFF") {
        return decodeAIFF(fileData.data(), length, outData, outNumSamples, outChannels, outSampleRate);
    }
    else if (ext == "wav" || ext == "WAV") {
        return decodeWAV(fileData.data(), length, outData, outNumSamples, outChannels, outSampleRate);
    }

    __android_log_print(ANDROID_LOG_ERROR, TAG, "Unsupported format: %s (supported: .aif, .aiff, .wav)", ext.c_str());
    return false;
}

// Main entry point: load all buffers
int BufferLoader::loadBuffers(RNBO::CoreObject& rnboObject, AAssetManager* assetManager) {
    __android_log_print(ANDROID_LOG_INFO, TAG, "=== Buffer Loading Started ===");
    
    if (!assetManager) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "AssetManager is null!");
        return 0;
    }
    
    // Parse dependencies.json
    std::vector<std::pair<std::string, std::string>> buffers;
    if (!parseDependencies(assetManager, buffers)) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to parse dependencies.json");
        return 0;
    }
    
    if (buffers.empty()) {
        __android_log_print(ANDROID_LOG_INFO, TAG, "No buffers to load");
        return 0;
    }
    
    int loadedCount = 0;
    
    // Load each buffer
    for (const auto& buffer : buffers) {
        const std::string& id = buffer.first;
        const std::string& file = buffer.second;
        
        float* audioData = nullptr;
        size_t numSamples = 0;
        int channels = 0;
        int sampleRate = 0;
        
        if (!loadAudioFile(assetManager, file, &audioData, &numSamples, &channels, &sampleRate)) {
            __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to load: %s", file.c_str());
            continue;
        }
        
        // Create buffer type
        RNBO::Float32AudioBuffer bufferType(channels, sampleRate);
        
        // Set external data
        rnboObject.setExternalData(
            id.c_str(),
            (char*)audioData,
            numSamples * sizeof(float),
            bufferType,
            &freeBuffer
        );
        
        __android_log_print(ANDROID_LOG_INFO, TAG,
            "[BUFFER] Loaded buffer '%s': %zu samples, %d ch, %d Hz",
            id.c_str(), numSamples, channels, sampleRate);
        
        loadedCount++;
    }
    
    __android_log_print(ANDROID_LOG_INFO, TAG,
        "=== Buffer Loading Complete: %d/%zu loaded ===",
        loadedCount, buffers.size());

    return loadedCount;
}

// Load audio file from external file path
bool BufferLoader::loadAudioFileFromPath(
    const std::string& filePath,
    float** outData,
    size_t* outNumSamples,
    int* outChannels,
    int* outSampleRate
) {
    __android_log_print(ANDROID_LOG_INFO, TAG, "Loading audio from file: %s", filePath.c_str());

    // Open file
    FILE* file = fopen(filePath.c_str(), "rb");
    if (!file) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to open file: %s", filePath.c_str());
        return false;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read file data
    std::vector<uint8_t> fileData(length);
    size_t read = fread(fileData.data(), 1, length, file);
    fclose(file);

    if (read != length) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to read file completely");
        return false;
    }

    __android_log_print(ANDROID_LOG_INFO, TAG, "File size: %zu bytes", length);

    // Decode based on file extension
    std::string ext = filePath.substr(filePath.find_last_of('.') + 1);

    if (ext == "aif" || ext == "aiff" || ext == "AIF" || ext == "AIFF") {
        return decodeAIFF(fileData.data(), length, outData, outNumSamples, outChannels, outSampleRate);
    }
    else if (ext == "wav" || ext == "WAV") {
        return decodeWAV(fileData.data(), length, outData, outNumSamples, outChannels, outSampleRate);
    }

    __android_log_print(ANDROID_LOG_ERROR, TAG, "Unsupported format: %s (supported: .aif, .aiff, .wav)", ext.c_str());
    return false;
}

// Load a single buffer from external file
bool BufferLoader::loadBufferFromFile(
    RNBO::CoreObject& rnboObject,
    const std::string& bufferId,
    const std::string& filePath
) {
    __android_log_print(ANDROID_LOG_INFO, TAG, "Loading buffer '%s' from: %s", bufferId.c_str(), filePath.c_str());

    float* audioData = nullptr;
    size_t numSamples = 0;
    int channels = 0;
    int sampleRate = 0;

    if (!loadAudioFileFromPath(filePath, &audioData, &numSamples, &channels, &sampleRate)) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to load audio file: %s", filePath.c_str());
        return false;
    }

    // Create buffer type
    RNBO::Float32AudioBuffer bufferType(channels, sampleRate);

    // Set external data (replaces existing buffer if any)
    rnboObject.setExternalData(
        bufferId.c_str(),
        (char*)audioData,
        numSamples * sizeof(float),
        bufferType,
        &freeBuffer
    );

    __android_log_print(ANDROID_LOG_INFO, TAG,
        "[BUFFER] Dynamically loaded buffer '%s': %zu samples, %d ch, %d Hz",
        bufferId.c_str(), numSamples, channels, sampleRate);

    return true;
}
