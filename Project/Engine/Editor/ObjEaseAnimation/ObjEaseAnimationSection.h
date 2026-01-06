#pragma once
#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/RailPlayer.h"
#include "utility/FileSelector/FileSelector.h"
#include "utility/TimeModeSelector/TimeModeSelector.h"
#include "Vector3.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>

/// <summary>
/// オブジェクトイージングアニメーションセクション
/// </summary>
namespace KetaEngine {

class ObjEaseAnimationSection {
public:
    enum class TransformType {
        Scale,
        Rotation,
        Translation,
        Count
    };

    enum class PlayState {
        STOPPED,
        WAITING,
        PLAYING,
        RETURN_WAITING,
        RETURNING
    };

    enum class TransformState {
        INACTIVE,
        WAITING,
        PLAYING,
        RETURN_WAITING,
        RETURNING,
        FINISHED
    };

    struct TransformParam {
        bool isActive          = false;
        bool isReturnToOrigin  = false;
        bool useRail           = false;
        bool isLookAtDirection = false;

        Vector3 endValue = Vector3::ZeroVector();

        // 通常再生パラメータ
        float maxTime    = 1.0f;
        int32_t easeType = 0;

        // 戻り動作パラメータ
        float returnMaxTime    = 1.0f;
        int32_t returnEaseType = 0;

        std::string railFileName;

        // 現在のオフセット値
        Vector3 currentOffset  = Vector3::ZeroVector();
        Vector3 startValue     = Vector3::ZeroVector();
        Vector3 previousOffset = Vector3::ZeroVector();

        // イージング
        Easing<Vector3> ease;
        Easing<Vector3> returnEase;

        // 個別の状態管理
        TransformState state    = TransformState::INACTIVE;
        float returnElapsedTime = 0.0f;
    };

    struct RailFileSelector {
        FileSelector selector;
    };

public:
    ObjEaseAnimationSection()  = default;
    ~ObjEaseAnimationSection() = default;

    // 初期化、更新
    void Init(const std::string& animationName, const std::string& categoryName, int32_t keyNumber);
    void Update(float speedRate = 1.0f);

    // リセット、終了判定
    void Reset();
    bool IsFinished() const;

    // データ読み込み、保存、編集
    void LoadData();
    void SaveData();
    void AdjustParam();

    // 開始処理
    void SetStartValues(const Vector3& scale, const Vector3& rotation, const Vector3& translation);
    void StartWaiting();

private:
    const char* GetSRTName(const TransformType& type) const;

    void RegisterParams();
    void GetParams();
    void AdaptEaseParam();
    void AdaptValueSetting();

    // 再生開始
    void StartPlay();

    // 各Transformの個別更新
    void UpdateTransform(TransformParam& param, TransformType type, float deltaTime);
    void UpdateTransformPlay(TransformParam& param, TransformType type, float deltaTime);
    void UpdateTransformReturn(TransformParam& param, TransformType type, float deltaTime);

    // 全体の状態チェック
    void UpdateOverallState();
    bool AreAllTransformsFinished() const;

    void ImGuiTransformParam(const char* label, TransformParam& param, const TransformType& type);

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_;
    int32_t currenTSequenceElementIndex_ = -1;

    // transformParam
    std::array<TransformParam, static_cast<size_t>(TransformType::Count)> transformParams_;

    // time
    float startTime_       = 0.0f;
    float returnStartTime_ = 0.0f;

    // 経過時間
    float elapsedTime_ = 0.0f;

    // rail
    std::unique_ptr<RailPlayer> railPlayer_;
    RailFileSelector railFileSelector_;

    // 再生状態
    PlayState playState_ = PlayState::STOPPED;

    TimeModeSelector timeModeSelector_;

public:
    float GetTimePoint() const { return startTime_; }
    const Vector3& GetCurrentScale() const { return transformParams_[static_cast<size_t>(TransformType::Scale)].currentOffset; }
    const Vector3& GetCurrentRotation() const { return transformParams_[static_cast<size_t>(TransformType::Rotation)].currentOffset; }
    const Vector3& GetCurrentTranslation() const { return transformParams_[static_cast<size_t>(TransformType::Translation)].currentOffset; }

    const Vector3& GetEditScale() const { return transformParams_[static_cast<size_t>(TransformType::Scale)].endValue; }
    const Vector3& GetEditRotation() const { return transformParams_[static_cast<size_t>(TransformType::Rotation)].endValue; }
    const Vector3& GetEditTranslation() const { return transformParams_[static_cast<size_t>(TransformType::Translation)].endValue; }

    RailPlayer* GetRailPlayer() { return railPlayer_.get(); }
    bool IsUsingRail() const;
    bool IsLookingAtDirection() const;
    Vector3 GetMovementDirection() const;
};

}; // KetaEngine
