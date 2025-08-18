#pragma once
#include "3d/ViewProjection.h"
#include "CameraKeyFrame.h"
#include "Quaternion.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class CameraEditor {
public:
    enum class PlayState {
        STOPPED,
        PLAYING,
        PAUSED
    };

    struct InitialSettings {
        Vector3 position       = {0.0f, 0.0f, -10.0f};
        Vector3 rotation       = {0.0f, 0.0f, 0.0f};
        float fov              = 45.0f * 3.141592654f / 180.0f;
        float returnDuration   = 1.0f; // 初期値に戻る時間
        int32_t returnEaseType = 0; // 初期値に戻るイージング
    };

public:
    CameraEditor()  = default;
    ~CameraEditor() = default;

    /// 初期化
    void Init(const std::string& animationName);

    /// 更新
    void Update(float deltaTime);

    /// ImGuiでの調整
    void AdjustParam();

    /// ViewProjectionへの適応
    void ApplyToViewProjection(ViewProjection& viewProjection);

    /// キーフレーム操作
    void AddKeyFrame();
    void RemoveKeyFrame(int32_t index);
    void ClearAllKeyFrames();

    /// 再生制御
    void Play();
    void Stop();
    void Pause();
    void Reset();

    /// 設定の保存・読み込み
    void SaveAnimation();
    void LoadAnimation();

    /// 初期設定の保存・読み込み
    void SaveInitialSettings();
    void LoadInitialSettings();

private:
    /// パラメータのバインド
    void BindParams();

    /// キーフレーム間の補間計算
    void UpdateInterpolation();

    /// 初期値への復帰処理
    void UpdateReturnToInitial(float deltaTime);

    /// 現在の補間値を計算
    void CalculateInterpolatedValues();

    /// キーフレームのソート（時間順）
    void SortKeyFramesByTime();

    /// 現在時刻に対応するキーフレームインデックスを取得
    std::pair<int32_t, int32_t> GetSurroundingKeyFrames() const;

    /// Quaternion変換
    Quaternion EulerToQuaternion(const Vector3& euler);
    Vector3 QuaternionToEuler(const Quaternion& quaternion);

private:
    // アニメーション名
    std::string animationName_;

    // GlobalParameter
    GlobalParameter* globalParameter_;
    std::string groupName_;

    // キーフレーム
    std::vector<std::unique_ptr<CameraKeyFrame>> keyFrames_;
    int32_t selectedKeyFrameIndex_ = -1;

    // 再生状態
    PlayState playState_      = PlayState::STOPPED;
    float currentTime_        = 0.0f;
    bool loopAnimation_       = false;
    bool autoReturnToInitial_ = true;

    // 初期設定
    InitialSettings initialSettings_;

    // 初期値復帰用
    bool isReturningToInitial_ = false;
    float returnTimer_         = 0.0f;
    Vector3 returnStartPosition_;
    Vector3 returnStartRotation_;
    float returnStartFov_;
    Easing<Vector3> returnPositionEase_;
    Easing<Vector3> returnRotationEase_;
    Easing<float> returnFovEase_;

    // 現在の補間値
    Vector3 currentPosition_;
    Vector3 currentRotation_;
    float currentFov_;

    // UI用パラメータ
    float playbackSpeed_        = 1.0f;
    bool showKeyFrameList_      = true;
    bool showAnimationControls_ = true;
    bool showInitialSettings_   = false;

    // キーフレーム追加用の時間
    float newKeyFrameTime_ = 0.0f;

public:
    /// アニメーション制御
    void SetCurrentTime(float time);
    float GetCurrentTimer() const { return currentTime_; }
    float GetTotalDuration() const;
    bool IsPlaying() const { return playState_ == PlayState::PLAYING; }
    bool IsFinished() const;
};