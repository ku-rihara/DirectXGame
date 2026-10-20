#include "AudioDSPContext.h"

using namespace KetaEngine;
#include "Audio/DSP/Core/AudioPCM.h"
#include "Audio/DSP/IO/ISoundRegistry.h"

bool AudioDSPContext::SubmitResult(const std::string& soundName, const AudioPCM& pcm) {
    if (!registry_.Register(soundName, pcm)) {
        status_ = "生成に失敗しました(長さ0): " + soundName;
        return false;
    }
    status_ = "生成しました: " + soundName;
    SelectPreview(soundName);
    return true;
}

void AudioDSPContext::Play(const std::string& soundName) {
    registry_.Play(soundName, playVolume_);
}

void AudioDSPContext::SelectPreview(const std::string& soundName) {
    previewName_ = soundName;
    ++previewRevision_;
}
