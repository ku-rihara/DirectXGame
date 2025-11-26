#pragma once
#include "3d/Object3D.h"
#include "3d/ViewProjection.h"
#include "CameraAnimationData.h"
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// カメラアニメーションエディタクラス
/// </summary>
class CameraEditor : public BaseEffectEditor<CameraAnimationData> {
public:
    CameraEditor()  = default;
    ~CameraEditor() = default;

    // 初期化、更新
    void Init(const std::string& animationName, const bool& isUseCategory = false) override;
    void Update(const float& speedRate = 1.0f) override;
    void EditorUpdate() override;

    // 選択アニメーション再生
    void PlaySelectedAnimation()override;


    // ViewProjectionへの適用
    void ApplyToViewProjection();
    void ApplySelectedKeyFrameToViewProjection();

private:
    //*---------------------------- protected Methods ----------------------------*//

    // 純粋仮想関数の実装
    std::unique_ptr<CameraAnimationData> CreateEffectData() override;
    void RenderSpecificUI() override;
    std::string GetFolderPath() const override;

private:
    ViewProjection* viewProjection_ = nullptr;

    std::unique_ptr<Object3d> preViewCameraObj_ = nullptr;
    std::unique_ptr<Object3d> preViewFollowObj_ = nullptr;

    const std::string folderName_ = "CameraAnimation/AnimationData/";

    bool autoApplyToViewProjection_ = true;
    bool keyFramePreviewMode_       = false;
    bool isEditing_                 = false;
    bool isPreViewDraw_             = false;

public:
    const bool& GetIsEditing() const { return isEditing_; }
    const bool& GetAutoApplyToViewProjection() const { return autoApplyToViewProjection_; }
    const bool& GetSequenceElementPreviewMode() const { return keyFramePreviewMode_; }
    CameraAnimationData* GetSelectedAnimation();

    void SetAutoApplyToViewProjection(const bool& enable) { autoApplyToViewProjection_ = enable; }
    void SetSequenceElementPreviewMode(const bool& enable) { keyFramePreviewMode_ = enable; }
    void SetViewProjection(ViewProjection* vp);
};