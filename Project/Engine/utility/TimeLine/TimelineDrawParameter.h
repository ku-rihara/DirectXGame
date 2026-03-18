#pragma once
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <cstdint>
#include <string>

namespace KetaEngine {

/// <summary>
/// タイムライン描画パラメータ
/// </summary>
class TimelineDrawParameter {
public:
    TimelineDrawParameter()  = default;
    ~TimelineDrawParameter() = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init(const std::string& name);
    void DrawImGui();
   
private:
    //*---------------------------- private Methods ----------------------------*//

    void RegisterParams();
    void GetParams();

private:
    //*---------------------------- private Variant ----------------------------*//

    GlobalParameter* globalParameter_ = nullptr;
    std::string groupName_;
    std::string folderPath_;

    int32_t endFrame_  = 300;
    float headerWidth_ = 150.0f;
    float trackHeight_ = 30.0f;
    float rulerHeight_ = 25.0f;

public:
    //*----------------------------- getter Methods -----------------------------*//

    int32_t GetEndFrame() const { return endFrame_; }
    float GetHeaderWidth() const { return headerWidth_; }
    float GetTrackHeight() const { return trackHeight_; }
    float GetRulerHeight() const { return rulerHeight_; }

    //*----------------------------- setter Methods -----------------------------*//

    void SetEndFrame(int32_t v) { endFrame_ = v; }
};

}; // KetaEngine
