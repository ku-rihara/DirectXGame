#pragma once
#include "PostEffect/PostEffectRenderer.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <string>

class PostEffectData {
public:
    PostEffectData()  = default;
    ~PostEffectData() = default;

    /// 初期化
    void Init(const std::string& postEffectName);

    /// ImGuiでの調整
    void AdjustParam();

    /// データのロード・セーブ
    void LoadData();
    void SaveData();

private:
    /// パラメータのバインド
    void BindParams();

private:
    // GlobalParameter
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "PostEffect";

    // PostEffectパラメータ
    int postEffectModeIndex_ = 0;
    float duration_          = 1.0f;

    // UI表示制御
    bool showControls_ = true;

public:
    // ゲッター・セッター
    std::string GetGroupName() const { return groupName_; }
    PostEffectMode GetPostEffectMode() const { return static_cast<PostEffectMode>(postEffectModeIndex_); }
    float GetDuration() const { return duration_; }

    void SetPostEffectMode(PostEffectMode mode) { postEffectModeIndex_ = static_cast<int>(mode); }
    void SetDuration(float duration) { duration_ = duration; }
};