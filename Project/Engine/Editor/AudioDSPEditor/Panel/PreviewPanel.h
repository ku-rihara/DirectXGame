#pragma once

#include "Audio/DSP/Core/FFT.h"
#include "Audio/DSP/Core/WindowFunction.h"
#include "IAudioDSPPanel.h"
#include <cstdint>
#include <string>
#include <vector>

namespace KetaEngine {

class AudioPCM;

/// <summary>
/// 波形・スペクトルの表示と、エンコーダーでのファイル書き出し
/// </summary>
class PreviewPanel : public IAudioDSPPanel {
public:
    const char* GetTitle() const override { return "プレビュー / 書き出し"; }
    void Draw(AudioDSPContext& context) override;
    bool IsDefaultOpen() const override { return true; }

private:
    void Rebuild(AudioDSPContext& context);
    void BuildWave(const std::vector<double>& mono);
    void BuildSpectrum(const std::vector<double>& mono);
    void Export(AudioDSPContext& context);

private:
    FFT fft_{4096};
    WindowFunction window_{WindowFunction::Type::Hanning};

    uint32_t builtRevision_ = 0;
    std::vector<float> wave_;
    std::vector<float> spectrum_;
    double duration_ = 0.0;
    int sampleRate_  = 0;

    char exportName_[128] = "";

    const size_t maxFFTSize_          = 4096;
    const size_t wavePointCount_      = 1024;
    const std::string exportDirectory_ = "Resources/Audio/Voice/";
};

}; // KetaEngine
