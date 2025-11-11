#pragma once
#include "PostEffect/PostEffectRenderer.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <string>

/// <summary>
/// ポストエフェクトデータ
/// </summary>
class PostEffectData {
public:
    PostEffectData()  = default;
    ~PostEffectData() = default;

    // 初期化
    void Init(const std::string& postEffectName);

    void AdjustParam(); //< パラメータ調整
    void LoadData(); //< データロード
    void SaveData(); //< データセーブ

private:
    void RegisterParams(); //< パラメータのバインド

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "PostEffect";

    int postEffectModeIndex_ = 0;
    float duration_          = 1.0f;

    bool showControls_ = true;

public:
    // getter
    std::string GetGroupName() const { return groupName_; }
    PostEffectMode GetPostEffectMode() const { return static_cast<PostEffectMode>(postEffectModeIndex_); }
    float GetDuration() const { return duration_; }

    // setter
    void SetPostEffectMode(PostEffectMode mode) { postEffectModeIndex_ = static_cast<int>(mode); }
    void SetDuration(float duration) { duration_ = duration; }
};