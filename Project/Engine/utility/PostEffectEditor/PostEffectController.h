#pragma once
#include "PostEffect/PostEffectRenderer.h"
#include "PostEffectData.h"
#include <memory>
#include <string>
#include <vector>

class PostEffectController {
public:
    PostEffectController()  = default;
    ~PostEffectController() = default;

    /// 初期化・更新
    void Init();
    void Update(float deltaTime);

    /// PostEffect制御
    void PlayPostEffect(const std::string& postEffectName);
    void StopPostEffect();
    void SetPostEffectImmediate(PostEffectMode mode);

    /// 状態取得
    PostEffectMode GetCurrentPostEffect() const;
    bool IsPostEffectActive() const;

    /// エディタ機能
    void EditorUpdate();
    void AddPostEffect(const std::string& postEffectName);
    void RemovePostEffect(int index);
    void AllLoadFile();
    void AllSaveFile();

    /// 取得
    PostEffectData* GetPostEffectByName(const std::string& name);
    int GetPostEffectCount() const { return static_cast<int>(postEffects_.size()); }

private:
    std::vector<std::unique_ptr<PostEffectData>> postEffects_;
    PostEffectRenderer* renderer_ = nullptr;
    int selectedIndex_            = -1;

    // 入力用バッファ
    char nameBuffer_[128] = "";

    // 時間管理
    bool isPostEffectActive_    = false;
    float currentTimer_         = 0.0f;
    float targetDuration_       = 0.0f;
    PostEffectMode defaultMode_ = PostEffectMode::NONE;
};
