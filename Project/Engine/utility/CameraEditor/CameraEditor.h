#pragma once
#include "3d/ViewProjection.h"
#include "Easing/Easing.h"
#include "utility/EasingCreator/EasingParameterData.h"
#include "Vector3.h"
#include <functional>
#include <memory>
#include <json.hpp>
#include <string>
#include <vector>

/// キーフレームデータ構造体
struct CameraKeyFrame {
    float timePoint  = 0.0f;
    Vector3 position = {0.0f, 0.0f, 0.0f};
    Vector3 rotation = {0.0f, 0.0f, 0.0f};
    float fov        = 45.0f * 3.141592654f / 180.0f;

    // 次のキーフレームへの補間設定
    EasingType positionEasingType = EasingType::InOutSine;
    EasingType rotationEasingType = EasingType::InOutSine;
    EasingType fovEasingType      = EasingType::InOutSine;
};

/// カメラアニメーションデータ
struct CameraAnimation {
    std::string name = "DefaultAnimation";
    std::vector<CameraKeyFrame> keyFrames;
    float totalDuration = 0.0f;
    bool isLooping      = false;
};

/// カメラエディタークラス
class CameraEditor {
public:
    CameraEditor()  = default;
    ~CameraEditor() = default;

    /// 初期化
    void Init(ViewProjection* viewProjection);

    /// 更新
    void Update(float deltaTime);

    /// ImGui描画
    void DrawImGui();

    /// アニメーション制御
    void PlayAnimation(const std::string& animationName);
    void StopAnimation();
    void PauseAnimation();
    void ResumeAnimation();
    void SetAnimationTime(float time);

    /// キーフレーム操作
    void AddKeyFrame(float timePoint);
    void DeleteKeyFrame(int index);
    void UpdateKeyFrameFromCurrentCamera(int index);
    void ApplyKeyFrameToCamera(int index);

    /// アニメーション作成・編集
    void CreateNewAnimation(const std::string& name);
    void DuplicateAnimation(const std::string& sourceName, const std::string& newName);
    void DeleteAnimation(const std::string& name);

    /// JSON保存・読み込み
    void SaveAnimationsToJson(const std::string& filePath);
    void LoadAnimationsFromJson(const std::string& filePath);
    void ExportCurrentAnimationToJson(const std::string& filePath);
    void ImportAnimationFromJson(const std::string& filePath);

    /// 元のカメラパラメータ
    void SaveOriginalCameraParams();
    void RestoreOriginalCameraParams();

    /// ゲッター
    bool IsPlaying() const { return isPlaying_; }
    bool IsPaused() const { return isPaused_; }
    float GetCurrentAnimationTime() const { return currentTime_; }
    const std::string& GetCurrentAnimationName() const { return currentAnimationName_; }

    /// セッター
    void SetOnAnimationFinishCallback(const std::function<void()>& callback) { onAnimationFinishCallback_ = callback; }
    void SetOnKeyFrameReachedCallback(const std::function<void(int)>& callback) { onKeyFrameReachedCallback_ = callback; }

private:
    /// プライベートメソッド
    void UpdateAnimation(float deltaTime);
    void InterpolateCamera(float time);
    void SetupEasingForSegment(int fromKeyFrameIndex, int toKeyFrameIndex);
    int FindKeyFrameIndexAtTime(float time);
    void SortKeyFramesByTime();

    /// JSON関連
    nlohmann::json KeyFrameToJson(const CameraKeyFrame& keyFrame);
    CameraKeyFrame JsonToKeyFrame(const nlohmann::json& json);
    nlohmann::json AnimationToJson(const CameraAnimation& animation);
    CameraAnimation JsonToAnimation(const nlohmann::json& json);

    /// ImGui描画関連
    void DrawAnimationControls();
    void DrawKeyFrameList();
    void DrawKeyFrameEditor();
    void DrawFileOperations();
    void DrawCameraPreview();

private:
    /// カメラ関連
    ViewProjection* viewProjection_ = nullptr;
    ViewProjection originalCameraParams_;
    bool hasOriginalParams_ = false;

    /// アニメーション関連
    std::vector<CameraAnimation> animations_;
    std::string currentAnimationName_ = "";
    int currentAnimationIndex_        = -1;

    /// 再生制御
    bool isPlaying_      = false;
    bool isPaused_       = false;
    float currentTime_   = 0.0f;
    float playbackSpeed_ = 1.0f;

    /// イージング
    std::unique_ptr<Easing<Vector3>> positionEasing_;
    std::unique_ptr<Easing<Vector3>> rotationEasing_;
    std::unique_ptr<Easing<float>> fovEasing_;

    /// 現在の補間セグメント
    int currentFromKeyFrame_ = -1;
    int currentToKeyFrame_   = -1;

    /// ImGui用変数
    int selectedKeyFrameIndex_        = -1;
    char newAnimationNameBuffer_[256] = "NewAnimation";
    char filePathBuffer_[512]         = "Resources/CameraAnimations/";

    /// コールバック
    std::function<void()> onAnimationFinishCallback_;
    std::function<void(int)> onKeyFrameReachedCallback_;

    /// プレビュー用
    bool isPreviewMode_ = false;
    Vector3 previewPosition_;
    Vector3 previewRotation_;
    float previewFov_;
};