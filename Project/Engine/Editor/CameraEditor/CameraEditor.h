#pragma once
#include "3d/Object3D.h"
#include "3d/ViewProjection.h"
#include "CameraAnimationData.h"
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// カメラアニメーションエディタクラス
/// </summary>
class CameraEditor {
public:
    CameraEditor()  = default;
    ~CameraEditor() = default;

    // 初期化、更新
    void Init(ViewProjection* vp);
    void Update();      
    void EditorUpdate();

    /// <summary>
    /// アニメーション追加
    /// </summary>
    /// <param name="animationName">アニメーション名</param>
    void AddAnimation(const std::string& animationName);

    CameraAnimationData* GetSelectedAnimation();

     // 選択アニメーション再生、一時停止、リセット
    void PlaySelectedAnimation();            
    void PauseSelectedAnimation();           
    void ResetSelectedAnimation();           

    // 選択アニメーション状態取得
    bool IsSelectedAnimationPlaying()  const; 
    bool IsSelectedAnimationFinished() const; 

    // ViewProjectionへの適用
    void ApplyToViewProjection();                 
    void ApplySelectedKeyFrameToViewProjection(); 

private:
    void SetViewProjection(ViewProjection* vp);
    void AllLoadFile(); //< 全ファイル読み込み
    void AllSaveFile(); //< 全ファイル保存
private:
    std::vector<std::unique_ptr<CameraAnimationData>> animations_;
    int selectedIndex_              = -1;
    ViewProjection* viewProjection_ = nullptr;

    std::unique_ptr<Object3d> preViewCameraObj_ = nullptr;
    std::unique_ptr<Object3d> preViewFollowObj_ = nullptr;

    char nameBuffer_[128] = "";

    bool autoApplyToViewProjection_ = true;
    bool keyFramePreviewMode_       = false;
    bool isEditing_                 = false;
    bool isPreViewDraw_             = false;

public:
    const bool& GetIsEditing() const { return isEditing_; }
    const bool& GetAutoApplyToViewProjection() const { return autoApplyToViewProjection_; }
    const bool& GetKeyFramePreviewMode() const { return keyFramePreviewMode_; }

    void SetAutoApplyToViewProjection(const bool& enable) { autoApplyToViewProjection_ = enable; }
    void SetKeyFramePreviewMode(const bool& enable) { keyFramePreviewMode_ = enable; }
};