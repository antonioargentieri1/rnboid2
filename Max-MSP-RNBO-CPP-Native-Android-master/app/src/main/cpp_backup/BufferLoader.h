//
// BufferLoader.h - Audio Buffer Loading System
// Loads audio files from Android assets into RNBO buffers
// Supports: AIFF (.aif, .aiff) and WAV (.wav) formats
//

#ifndef RNBO_BUFFER_LOADER_H
#define RNBO_BUFFER_LOADER_H

#include <RNBO.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <memory>

class BufferLoader {
public:
    /**
     * Load all buffers defined in dependencies.json
     * @param rnboObject The RNBO CoreObject to load buffers into
     * @param assetManager Android AssetManager for file access
     * @return Number of buffers successfully loaded
     */
    static int loadBuffers(RNBO::CoreObject& rnboObject, AAssetManager* assetManager);

private:
    /**
     * Load a single audio file from assets
     * @param assetManager Android AssetManager
     * @param filename File path in assets (e.g., "media/drumLoop.aif")
     * @param outData Pointer to receive allocated audio data
     * @param outNumSamples Total number of samples (channels * frames)
     * @param outChannels Number of audio channels
     * @param outSampleRate Sample rate in Hz
     * @return true if successful
     */
    static bool loadAudioFile(
        AAssetManager* assetManager,
        const std::string& filename,
        float** outData,
        size_t* outNumSamples,
        int* outChannels,
        int* outSampleRate
    );

    /**
     * Decode AIFF audio file format
     */
    static bool decodeAIFF(
        const uint8_t* fileData,
        size_t fileSize,
        float** outData,
        size_t* outNumSamples,
        int* outChannels,
        int* outSampleRate
    );

    /**
     * Decode WAV audio file format (RIFF WAVE, PCM only)
     */
    static bool decodeWAV(
        const uint8_t* fileData,
        size_t fileSize,
        float** outData,
        size_t* outNumSamples,
        int* outChannels,
        int* outSampleRate
    );

    /**
     * Parse dependencies.json from assets
     */
    static bool parseDependencies(
        AAssetManager* assetManager,
        std::vector<std::pair<std::string, std::string>>& buffers
    );

    /**
     * Callback to free buffer memory when RNBO is done
     */
    static void freeBuffer(RNBO::ExternalDataId id, char* data);

    /**
     * Convert big-endian to native byte order
     */
    static uint32_t swapEndian32(uint32_t value);
    static uint16_t swapEndian16(uint16_t value);
};

#endif // RNBO_BUFFER_LOADER_H
