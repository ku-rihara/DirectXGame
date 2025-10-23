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

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="vp">ビュープロジェクション</param>
    void Init(ViewProjection* vp);

    void Update(); //< 更新
    void EditorUpdate(); //< エディタ更新

    void AllLoadFile(); //< 全ファイル読み込み
    void AllSaveFile(); //< 全ファイル保存

    /// <summary>
    /// アニメーション追加
    /// </summary>
    /// <param name="animationName">アニメーション名</param>
    void AddAnimation(const std::string& animationName);

    CameraAnimationData* GetSelectedAnimation();

    void PlaySelectedAnimation();             //< 選択中のアニメーション再生
    void PauseSelectedAnimation();            //< 選択中のアニメーション一時停止
    void ResetSelectedAnimation();            //< 選択中のアニメーションリセット
    bool IsSelectedAnimationPlaying() const;  //< 選択中のアニメーション再生中か
    bool IsSelectedAnimationFinished() const; //< 選択中のアニメーション終了したか

    void ApplyToViewProjection();                 //< ViewProjectionに適用
    void ApplySelectedKeyFrameToViewProjection(); //< 選択中のキーフレームをViewProjectionに適用

private:
    void SetViewProjection(ViewProjection* vp);

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