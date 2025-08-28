#pragma once
#include "3d/Object3D.h"
#include "3d/ViewProjection.h"
#include "CameraAnimationData.h"
#include <memory>
#include <string>
#include <vector>

class CameraEditor {

public:
    CameraEditor()  = default;
    ~CameraEditor() = default;

    void Init(ViewProjection* vp);
    void Update(float deltaTime);
    void EditorUpdate();

    void AllLoadFile();
    void AllSaveFile();
    void AddAnimation(const std::string& animationName);

    CameraAnimationData* GetSelectedAnimation();

    // 再生機能の追加
    void PlaySelectedAnimation();
    void PauseSelectedAnimation();
    void ResetSelectedAnimation();
    bool IsSelectedAnimationPlaying() const;
    bool IsSelectedAnimationFinished() const;

    // ViewProjection適応
    void ApplyToViewProjection();

    // KeyFrameプレビュー機能
    void ApplySelectedKeyFrameToViewProjection();

private:
    void SetViewProjection(ViewProjection* vp);

private:
    // Animationデバッグ
    std::vector<std::unique_ptr<CameraAnimationData>> animations_;
    int selectedIndex_                     = -1;
    ViewProjection* viewProjection_        = nullptr;
    std::unique_ptr<Object3d> debugObject_ = nullptr;

    // 入力用バッファ
    char nameBuffer_[128] = "";
    // 再生制御用
    bool autoApplyToViewProjection_ = true;
    // KeyFrameプレビュー用
    bool keyFramePreviewMode_ = false;

public:
    //--------------------------------------------------------------------------------------
    // getter
    //--------------------------------------------------------------------------------------
    bool GetAutoApplyToViewProjection() const { return autoApplyToViewProjection_; }
    bool GetKeyFramePreviewMode() const { return keyFramePreviewMode_; }

    //--------------------------------------------------------------------------------------
    // setter
    //--------------------------------------------------------------------------------------
    void SetAutoApplyToViewProjection(bool enable) { autoApplyToViewProjection_ = enable; }
    void SetKeyFramePreviewMode(bool enable) { keyFramePreviewMode_ = enable; }
};