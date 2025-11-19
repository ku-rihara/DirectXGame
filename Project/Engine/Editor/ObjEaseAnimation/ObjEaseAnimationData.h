#pragma once
#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/RailPlayer.h"
#include "Quaternion.h"
#include "utility/FileSelector/FileSelector.h"
#include "Vector3.h"
#include <cstdint>
#include <memory>
#include <string>

class WorldTransform;

/// <summary>
/// オブジェクトイージングアニメーションデータ
/// </summary>
class ObjEaseAnimationData {
public:
    enum class PlayState {
        STOPPED,
        PLAYING,
        RETURNING
    };

    enum class TransformType {
        Scale,
        Rotation,
        Translation,
        Count
    };

    struct TransformParam {
        bool isActive       = false;
        bool useRail        = false;
        bool returnToOrigin = false;

        // オフセット値として扱う
        Vector3 startValue = Vector3::ZeroVector();
        Vector3 endValue   = Vector3::ZeroVector();

        float maxTime    = 1.0f;
        int32_t easeType = 0;

        float returnMaxTime    = 1.0f;
        int32_t returnEaseType = 0;

        std::string railFileName;

        // 現在のオフセット値
        Vector3 currentOffset = Vector3::ZeroVector();
        Easing<Vector3> ease;
    };

    // Rail用ファイルセレクター
    struct RailFileSelector {
        FileSelector selector;
    };

public:
    ObjEaseAnimationData()  = default;
    ~ObjEaseAnimationData() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="animationName">アニメーション名</param>
    /// <param name="categoryName">カテゴリー名</param>
    void Init(const std::string& animationName, const std::string& categoryName);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(const float& deltaTime);

    // 再生制御
    void Play();
    void Stop();
    void Reset();

    // 編集、セーブ、ロード
    void AdjustParam();
    void LoadData();
    void SaveData();

    // 再生状態
    bool IsPlaying() const;
    bool IsFinished() const;
    bool IsUsingRail() const;

private:
    const char* GetSRTName(const TransformType& type) const;

    // パラメータ登録、取得、リセット
    void RegisterParams();
    void GetParams();
    void ResetParams();

    // 更新処理
    void UpdateTransforms(const float& deltaTime);
    void UpdateReturn(const float& deltaTime);

    void StartReturn(); //< 戻り動作開始
    void CheckFinish(); //< 終了判定

    // ImGui
    void ImGuiTransformParam(const char* label, TransformParam& param, const TransformType& type);

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string categoryName_;
    std::string folderPath_;

    std::array<TransformParam, static_cast<size_t>(TransformType::Count)> transformParams_;
    std::array<Vector3, static_cast<size_t>(TransformType::Count)> originalValues_;

    PlayState playState_ = PlayState::STOPPED;

    // Rail用
    std::unique_ptr<RailPlayer> railPlayer_;
    RailFileSelector railFileSelector_;

    bool showControls_ = true;

public:
    const std::string& GetGroupName() const { return groupName_; }
    const std::string& GetCategoryName() const { return categoryName_; }
    const Vector3& GetCurrentPos() const { return transformParams_[static_cast<size_t>(TransformType::Translation)].currentOffset; }
    const Vector3& GetCurrentScale() const { return transformParams_[static_cast<size_t>(TransformType::Scale)].currentOffset; }
    const Vector3& GetCurrentRotate() const { return transformParams_[static_cast<size_t>(TransformType::Rotation)].currentOffset; }
    RailPlayer* GetRailPlayer() { return railPlayer_.get(); }
};