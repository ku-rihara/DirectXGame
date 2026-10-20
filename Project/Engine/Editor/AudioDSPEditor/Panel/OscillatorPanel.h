#pragma once

#include "Audio/DSP/Source/OscillatorSource.h"
#include "IAudioDSPPanel.h"
#include <string>

namespace KetaEngine {

/// <summary>
/// 基本波形の生成パネル
/// </summary>
class OscillatorPanel : public IAudioDSPPanel {
public:
    OscillatorPanel();

    const char* GetTitle() const override { return "波形生成"; }
    void Draw(AudioDSPContext& context) override;

private:
    OscillatorSource oscillator_;
    double duration_ = 1.5;  //< 長さ[秒]
    double fadeTime_ = 0.01; //< フェード[秒]

    const int sampleRate_         = 44100;
    const int channelCount_       = 2;
    const std::string outputName_ = "DSP_Wave.mp3";
};

}; // KetaEngine
