//
// MSPLoader.cpp - VERSIONE OTTIMIZZATA ANTI-CLICK
// Fix per click ogni 4 secondi
// + Buffer Support via BufferLoader
//

#include <jni.h>
#include <string>
#include "android/log.h"
#include "MSPLoader.h"
#include "BufferLoader.h"

// Cache delle variabili per evitare allocazioni nel callback audio
static int cachedNumOutputChannels = 0;
static float invNumChannels = 1.0f;  // Pre-calcolata per evitare divisioni

oboe::Result MSPLoader::open() {
    oboe::AudioStreamBuilder builder;
    mErrorCallback = std::make_shared<MyErrorCallback>(this);

    builder.setChannelCount(kChannelCount)
            ->setSampleRate(44100)
            ->setDataCallback(this)
            ->setErrorCallback(mErrorCallback)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setFormat(oboe::AudioFormat::Float)
            ->setFramesPerCallback(512);  // Buffer fisso per consistenza

    oboe::Result result = builder.openStream(mStream);
    if (result == oboe::Result::OK && mStream) {
        assert(mStream->getChannelCount() == kChannelCount);
        assert(mStream->getFormat() == oboe::AudioFormat::Float);

        int32_t mFramesPerBurst = mStream->getFramesPerBurst();
        int32_t bufferSizeInFrames = mStream->getBufferSizeInFrames();
        int32_t mSampleRate = mStream->getSampleRate();

        // Buffer più grande per evitare underrun
        mStream->setBufferSizeInFrames(mFramesPerBurst * 6);

        __android_log_print(ANDROID_LOG_INFO, "MSPLoader",
                            "Stream opened: Frames per burst: %d BufferSize: %d SampleRate: %d",
                            mFramesPerBurst, bufferSizeInFrames, mSampleRate);
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "MSPLoader",
                            "Failed to open stream. Error: %s",
                            oboe::convertToText(result));
    }

    return result;
}

oboe::Result MSPLoader::start() {
    oboe::Result result = mStream->requestStart();
    if (result != oboe::Result::OK) {
        __android_log_print(ANDROID_LOG_ERROR, "MSPLoader",
                            "Error starting playback stream. Error: %s",
                            oboe::convertToText(result));
    }
    return result;
}

oboe::Result MSPLoader::stop() {
    oboe::Result result = mStream->stop();
    if (result != oboe::Result::OK) {
        __android_log_print(ANDROID_LOG_ERROR, "MSPLoader",
                            "Error stopping playback stream. Error: %s",
                            oboe::convertToText(result));
    }
    return result;
}

oboe::Result MSPLoader::close() {
    oboe::Result result = mStream->close();
    if (result != oboe::Result::OK) {
        __android_log_print(ANDROID_LOG_ERROR, "MSPLoader",
                            "Error closing playback stream. Error: %s",
                            oboe::convertToText(result));
    }
    return result;
}

void MSPLoader::configureUI(std::function<int(std::string[], int, JavaVM *)> cDecPsRef, JavaVM *jvm) {
    for (RNBO::ParameterIndex i = 0; i < rnboObject.getNumParameters(); ++i) {
        RNBO::ParameterInfo info;
        rnboObject.getParameterInfo(i, &info);
        if (info.visible) {
            std::string paramName = std::string(rnboObject.getParameterId(i));
            paramNames.push_back(paramName);
            paramInfoMap[paramName] = info;
        }
    }
    int numPars = paramNames.size();
    std::string *params = new std::string[numPars];
    for (unsigned int j = 0; j < numPars; j++) {
        params[j] = paramNames[j];
    }
    cDecPsRef(params, numPars, jvm);
}

int MSPLoader::init(std::function<int(std::string[], int, JavaVM *)> cDecPsRef, JavaVM *jvm, AAssetManager* assetManager) {
    rnboObject = RNBO::CoreObject();
    rnboObject.prepareToProcess(44100, 512);

    // CACHE numero canali per non chiamarlo nel callback audio
    cachedNumOutputChannels = rnboObject.getNumOutputChannels();
    
    // Pre-calcola inverso per evitare divisioni
    if (cachedNumOutputChannels > 1) {
        invNumChannels = 1.0f / cachedNumOutputChannels;
    } else {
        invNumChannels = 1.0f;
    }

    outputs = new RNBO::SampleValue *[cachedNumOutputChannels];
    for (int i = 0; i < cachedNumOutputChannels; i++) {
        outputs[i] = new RNBO::SampleValue[512];
    }

    // LOAD AUDIO BUFFERS from assets
    if (assetManager) {
        __android_log_print(ANDROID_LOG_INFO, "MSPLoader", "Loading audio buffers...");
        int buffersLoaded = BufferLoader::loadBuffers(rnboObject, assetManager);
        __android_log_print(ANDROID_LOG_INFO, "MSPLoader", "Buffers loaded: %d", buffersLoaded);
    } else {
        __android_log_print(ANDROID_LOG_WARN, "MSPLoader", "No AssetManager - buffers not loaded");
    }

    configureUI(cDecPsRef, jvm);

    __android_log_print(ANDROID_LOG_INFO, "MSPLoader", 
                        "RNBO initialized: %d output channels, invNumChannels=%f", 
                        cachedNumOutputChannels, invNumChannels);
    return 0;
}

void MSPLoader::updateParam(const char *paramName, float val) {
    RNBO::ParameterIndex parameterIndex = rnboObject.getParameterIndexForID(paramName);
    if (parameterIndex != -1) {
        rnboObject.setParameterValue(parameterIndex, val);
        __android_log_print(ANDROID_LOG_DEBUG, "MSPLoader",
                            "Parameter %s updated to %f", paramName, val);
    }
}

bool MSPLoader::loadBuffer(const char *bufferId, const char *filePath) {
    __android_log_print(ANDROID_LOG_INFO, "MSPLoader",
                        "Loading buffer '%s' from: %s", bufferId, filePath);
    return BufferLoader::loadBufferFromFile(rnboObject, bufferId, filePath);
}

void MSPLoader::MyErrorCallback::onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) {
    __android_log_print(ANDROID_LOG_INFO, "MSPLoader", "%s stream Error after close: %s",
                        oboe::convertToText(oboeStream->getDirection()),
                        oboe::convertToText(error));
    if (mParent->open() == oboe::Result::OK) {
        mParent->start();
    }
}

oboe::DataCallbackResult
MSPLoader::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    // ============================================
    // OTTIMIZZAZIONE CRITICA: Nessuna allocazione,
    // nessuna chiamata virtuale, nessuna divisione
    // ============================================
    
    // Input vuoto statico (allocato una sola volta)
    static RNBO::SampleValue* emptyInputs[1] = { nullptr };
    
    // Process RNBO usando cache
    rnboObject.process(emptyInputs, 0, outputs, cachedNumOutputChannels, numFrames);
    
    float *fOutput = (float *) audioData;
    
    // Ottimizzazione: branch fuori dal loop
    if (cachedNumOutputChannels == 1) {
        // MONO: copia diretta, zero overhead
        for (int i = 0; i < numFrames; i++) {
            *fOutput++ = outputs[0][i];
        }
    } else {
        // MULTI-CHANNEL: mix con moltiplicazione invece di divisione
        for (int i = 0; i < numFrames; i++) {
            float mixedSample = 0.0f;
            
            // Loop unrolled per stereo (caso più comune)
            if (cachedNumOutputChannels == 2) {
                mixedSample = (outputs[0][i] + outputs[1][i]) * 0.5f;
            } else {
                // Caso generico
                for (int ch = 0; ch < cachedNumOutputChannels; ch++) {
                    mixedSample += outputs[ch][i];
                }
                mixedSample *= invNumChannels;  // Moltiplicazione invece di divisione
            }
            
            *fOutput++ = mixedSample;
        }
    }

    return oboe::DataCallbackResult::Continue;
}
