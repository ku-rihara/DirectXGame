#pragma once
#include "3d/Object3D.h"
#include "3d/ViewProjection.h"
#include "CameraAnimationData.h"
#include <memory>
#include <string>
#include <vector>

class CameraAnimation {
public:
    CameraAnimation()  = default;
    ~CameraAnimation() = default;

    /// 初期化・更新
    void Init(ViewProjection* vp);
    void Update(float deltaTime);
    void EditorUpdate();

    /// ファイル管理
    void AllLoadFile();
    void AllSaveFile();
    void AddAnimation(const std::string& animationName);

    /// 再生制御（統合機能）
    void Play(const std::string& animationName);
    void PlaySelectedAnimation();
    void PauseSelectedAnimation();
    void ResetSelectedAnimation();
    void StopAllAnimations();

    /// 状態取得
    bool IsSelectedAnimationPlaying() const;
    bool IsSelectedAnimationFinished() const;
    bool IsAnyAnimationPlaying() const;
    CameraAnimationData* GetSelectedAnimation();
    CameraAnimationData* GetAnimationByName(const std::string& name);

    /// ViewProjection制御
    void ApplyToViewProjection();
    void ApplySelectedKeyFrameToViewProjection();
    void SetViewProjection(ViewProjection* vp) { viewProjection_ = vp; }

private:
    /// 内部処理
    void UpdateAnimations(float deltaTime);
    void HandleAutoApply();

private:
    // アニメーションデータ
    std::vector<std::unique_ptr<CameraAnimationData>> animations_;
    int selectedIndex_ = -1;

    // ViewProjection関連
    ViewProjection* viewProjection_        = nullptr;
    std::unique_ptr<Object3d> debugObject_ = nullptr;

    // 再生制御用
    bool autoApplyToViewProjection_ = true;
    bool keyFramePreviewMode_       = false;

    // UI用バッファ
    char nameBuffer_[128] = "";

public:
    //--------------------------------------------------------------------------------------
    // getter
    //--------------------------------------------------------------------------------------
    bool GetAutoApplyToViewProjection() const { return autoApplyToViewProjection_; }
    bool GetKeyFramePreviewMode() const { return keyFramePreviewMode_; }
    int GetSelectedIndex() const { return selectedIndex_; }
    size_t GetAnimationCount() const { return animations_.size(); }

    //--------------------------------------------------------------------------------------
    // setter
    //--------------------------------------------------------------------------------------
    void SetAutoApplyToViewProjection(bool enable) { autoApplyToViewProjection_ = enable; }
    void SetKeyFramePreviewMode(bool enable) { keyFramePreviewMode_ = enable; }
    void SetSelectedIndex(int index);
};