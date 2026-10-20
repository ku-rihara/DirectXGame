#pragma once

#include "ISoundRegistry.h"
#include "PCMConverter.h"

namespace KetaEngine {

class Audio;

/// <summary>
/// ISoundRegistry の Audio(XAudio2) 実装
/// </summary>
class AudioSoundRegistry : public ISoundRegistry {
public:
    explicit AudioSoundRegistry(Audio& audio) : audio_(audio) {}

    bool Register(const std::string& soundName, const AudioPCM& pcm) override;
    bool TryGet(const std::string& soundName, AudioPCM& outPCM) const override;
    void Play(const std::string& soundName, float volume) override;
    std::vector<std::string> GetSoundNames() const override;

private:
    Audio& audio_;
    PCMConverter converter_;
};

}; // KetaEngine
