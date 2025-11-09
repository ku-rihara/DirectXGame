#pragma once
#include "Easing/Easing.h"
#include "Quaternion.h"
#include "utility/FileSelector/FileSelector.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "utility/RailEditor/RailPlayer.h"
#include "Vector3.h"
#include <cstdint>
#include <memory>
#include <string>

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

        Vector3 startValue = Vector3::OneVector();
        Vector3 endValue   = Vector3::OneVector();

        float maxTime    = 1.0f;
        int32_t easeType = 0;

        // 戻り用パラメータ
        float returnMaxTime    = 1.0f;
        int32_t returnEaseType = 0;

        // Rail用
        std::string railFileName;

        // 実行時データ
        Vector3 currentValue = Vector3::ZeroVector();
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

    void AdjustParam(); //< パラメータ調整
    void Play(); //< 再生
    void Stop(); //< 停止
    void Reset(); //< リセット
    void LoadData(); //< データロード
    void SaveData(); //< データセーブ
    bool IsPlaying() const; //< 再生中か
    bool IsFinished() const; //< 終了したか

private:
    void RegisterParams(); //< パラメータのバインド
    void LoadParams(); //< パラメータ取得
    void ResetParams(); //< パラメータリセット
    void UpdateTransforms(const float& deltaTime); //< Transform更新
    void UpdateReturn(const float& deltaTime); //< 戻り動作更新
    void StartReturn(); //< 戻り動作開始
    void CheckFinish(); //< 終了判定
    void ImGuiTransformParam(const char* label, TransformParam& param, TransformType type);

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string categoryName_;
    std::string folderPath_;

    TransformParam scaleParam_;
    TransformParam rotationParam_;
    TransformParam translationParam_;

    PlayState playState_ = PlayState::STOPPED;

    // Rail用
    std::unique_ptr<RailPlayer> railPlayer_;
    RailFileSelector railFileSelector_;

    // 元の値（戻り用）
    Vector3 originalScale_       = Vector3::OneVector();
    Vector3 originalRotation_    = Vector3::ZeroVector();
    Vector3 originalTranslation_ = Vector3::ZeroVector();

    bool showControls_ = true;

public:
    std::string GetGroupName() const { return groupName_; }
    std::string GetCategoryName() const { return categoryName_; }
    const Vector3& GetCurrentScale() const { return scaleParam_.currentValue; }
    const Vector3& GetCurrentRotation() const { return rotationParam_.currentValue; }
    const Vector3& GetCurrentTranslation() const { return translationParam_.currentValue; }
    RailPlayer* GetRailPlayer() { return railPlayer_.get(); }
    bool IsUsingRail() const { return translationParam_.useRail && translationParam_.isActive; }
};