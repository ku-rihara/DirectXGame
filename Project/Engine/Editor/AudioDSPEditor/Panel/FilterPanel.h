#pragma once

#include "Audio/DSP/Filter/FIRFilter.h"
#include "Audio/DSP/Filter/FilterProcessor.h"
#include "Audio/DSP/Filter/IIRFilter.h"
#include "IAudioDSPPanel.h"
#include <string>

namespace KetaEngine {

/// <summary>
/// 登録済みサウンドにFIR/IIRフィルターを掛けるパネル
/// </summary>
class FilterPanel : public IAudioDSPPanel {
public:
    const char* GetTitle() const override { return "フィルター (FIR / IIR)"; }
    void Draw(AudioDSPContext& context) override;

private:
    void DrawIIRParam(AudioDSPContext& context);
    void DrawFIRParam(AudioDSPContext& context);
    void Apply(AudioDSPContext& context);

private:
    enum class Kind {
        IIR,
        FIR,
    };

    std::string sourceName_;
    Kind kind_ = Kind::IIR;

    IIRFilter iirFilter_;
    FIRFilter firFilter_;
    IIRFilter::Param iirParam_;
    FIRFilter::Param firParam_;
    WindowFunction window_;
    FilterProcessor processor_;

    const std::string outputName_ = "DSP_Filtered.mp3";
};

}; // KetaEngine
