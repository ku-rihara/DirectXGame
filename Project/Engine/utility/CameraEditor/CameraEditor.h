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
    void Update();
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
    int selectedIndex_              = -1;
    ViewProjection* viewProjection_ = nullptr;

    std::unique_ptr<Object3d> preViewCameraObj_ = nullptr;
    std::unique_ptr<Object3d> preViewFollowObj_ = nullptr;

    // 入力用バッファ
    char nameBuffer_[128] = "";

    bool autoApplyToViewProjection_ = true; //<再生制御
    bool keyFramePreviewMode_       = false; //< KeyFrameプレビュー
    bool isEditing_                 = false;

public:
    //--------------------------------------------------------------------------------------
    // getter
    //--------------------------------------------------------------------------------------
    const bool& GetIsEditing() const { return isEditing_; }
    const bool& GetAutoApplyToViewProjection() const { return autoApplyToViewProjection_; }
    const bool& GetKeyFramePreviewMode() const { return keyFramePreviewMode_; }

    //--------------------------------------------------------------------------------------
    // setter
    //--------------------------------------------------------------------------------------
    void SetAutoApplyToViewProjection(const bool& enable) { autoApplyToViewProjection_ = enable; }
    void SetKeyFramePreviewMode(const bool& enable) { keyFramePreviewMode_ = enable; }
};