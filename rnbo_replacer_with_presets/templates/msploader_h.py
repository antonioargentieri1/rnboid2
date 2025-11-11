"""
MSPLoader.h Template - Updated with Buffer Support
"""


class MSPLoaderHeaderTemplate:
    @staticmethod
    def get_content():
        return '''//
// MSPLoader.h - RNBO Audio Engine with Buffer Support
//

#ifndef RNBO_TEST_MSPLOADER_H
#define RNBO_TEST_MSPLOADER_H

#include "rnbo/RNBO.h"
#include <oboe/Oboe.h>
#include <jni.h>
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

class MSPLoader : public oboe::AudioStreamDataCallback {
public:
    oboe::Result open();
    oboe::Result start();
    oboe::Result stop();
    oboe::Result close();

    class MyErrorCallback : public oboe::AudioStreamErrorCallback {
    public:
        MyErrorCallback(MSPLoader *parent) : mParent(parent) {}
        virtual ~MyErrorCallback() {}
        void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) override;
    private:
        MSPLoader *mParent;
    };
    
    // Initialize with AssetManager for buffer loading
    int init(std::function<int(std::string[], int, JavaVM*)> cDecPsRef, JavaVM* jvm, AAssetManager* assetManager);
    
    static constexpr int kChannelCount = 1;
    void updateParam(const char *paramName, float val);
    
private:
    RNBO::CoreObject rnboObject;
    std::shared_ptr<oboe::AudioStream> mStream;
    std::shared_ptr<MyErrorCallback> mErrorCallback;
    
    // Audio callback
    oboe::DataCallbackResult onAudioReady(
        oboe::AudioStream *oboeStream, 
        void *audioData, 
        int32_t numFrames
    ) override;
    
    RNBO::SampleValue** outputs;
    std::vector<std::string> paramNames;
    std::map<std::string, RNBO::ParameterInfo> paramInfoMap;
    
    void configureUI(std::function<int(std::string[], int, JavaVM*)> cDecPsRef, JavaVM* jvm);
};

#endif //RNBO_TEST_MSPLOADER_H
'''
