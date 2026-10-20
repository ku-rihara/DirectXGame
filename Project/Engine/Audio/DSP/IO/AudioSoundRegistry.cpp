#include "AudioSoundRegistry.h"

using namespace KetaEngine;
#include "Audio/Audio.h"
#include "Audio/DSP/Core/AudioPCM.h"
#include <algorithm>

bool AudioSoundRegistry::Register(const std::string& soundName, const AudioPCM& pcm) {
    if (pcm.IsEmpty()) {
        return false;
    }
    WAVEFORMATEX format{};
    std::vector<BYTE> bytes;
    converter_.Encode16(pcm, format, bytes);
    return audio_.RegisterSound(soundName, format, std::move(bytes)) >= 0;
}

bool AudioSoundRegistry::TryGet(const std::string& soundName, AudioPCM& outPCM) const {
    const Audio::SoundData* soundData = audio_.FindSoundData(soundName);
    if (!soundData) {
        return false;
    }
    return converter_.Decode(soundData->wfex, soundData->buffer, outPCM);
}

void AudioSoundRegistry::Play(const std::string& soundName, float volume) {
    if (audio_.FindSoundId(soundName) >= 0) {
        audio_.Play(soundName, volume);
    }
}

std::vector<std::string> AudioSoundRegistry::GetSoundNames() const {
    std::vector<std::string> names = audio_.GetAvailableSoundNames();
    std::sort(names.begin(), names.end());
    return names;
}
