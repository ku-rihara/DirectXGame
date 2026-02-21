#pragma once
#include "Editor/BaseEffectEditor/BaseSequenceEffectData.h"
#include "ObjEaseAnimationSection.h"
#include "Vector3.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>

namespace KetaEngine {

class WorldTransform;
class RailPlayer;

/// <summary>
/// オブジェクトイージングアニメーションデータ
/// </summary>
class ObjEaseAnimationData : public BaseSequenceEffectData<ObjEaseAnimationSection> {
public:
    enum class TransformType {
        Scale,
        Rotation,
        Translation,
        Count
    };

public:
    ObjEaseAnimationData()           = default;
    ~ObjEaseAnimationData() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectDataからのオーバーライド
    void Update(float speedRate = 1.0f) override;
    void Reset() override;
    void Play() override;

    // カテゴリー対応初期化
    void Init(const std::string& animationName, const std::string& categoryName)override;

    // 編集用メソッド
    void AdjustParam();
 
    //*----------------------------- protected Methods from Base -----------------------------*//
protected:
    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

    // BaseSequenceEffectDataからのオーバーライド
    void UpdateKeyFrameProgression() override;
    void AdvanceToNextSequenceElement() override;
    std::unique_ptr<ObjEaseAnimationSection> CreateKeyFrame(int32_t index) override;
    std::string GetSequenceElementFolderPath() const override;

private:
    //*---------------------------- private Methods ----------------------------*//
    std::string GetSRTName(const TransformType& type) const;
    void CreateOrLoadSections(const std::vector<std::pair<int32_t, std::string>>& KeyFrameFiles);
    void UpdateActiveSection(float speedRate);

    // SRT独立進行用メソッド
    void UpdateIndependentSRTProgression();
    void AdvanceTransformToNextSection(TransformType type);
    bool AreAllSRTFinished() const;

private:
    //*---------------------------- private Variant ----------------------------*//

    const std::string baseFolderPath_ = "ObjEaseAnimation/";

    // originParam
    std::array<Vector3, static_cast<size_t>(TransformType::Count)> originalValues_;

    // SRT独立進行用 - 各TransformTypeごとのアクティブセクションインデックス
    std::array<int32_t, static_cast<size_t>(TransformType::Count)> activeSectionIndices_ = {0, 0, 0};
    // 各TransformTypeが全セクション完了したかどうか
    std::array<bool, static_cast<size_t>(TransformType::Count)> srtAllSectionsFinished_ = {false, false, false};

    bool isLoop_ = false; //< ループ再生フラグ

public:
    //*----------------------------- getter Methods -----------------------------*//
    const std::string& GetCategoryName() const { return categoryName_; }
    const Vector3& GetOriginalValue(const TransformType& type) const {
        return originalValues_[static_cast<size_t>(type)];
    }
    Vector3 GetActiveKeyFrameValue(const TransformType& type) const;
    bool GetIsUseRailActiveKeyFrame() const;
    RailPlayer* GetCurrentRailPlayer() const;

    // アニメーション開始前のオフセット値を設定
    void SetPreAnimationOffsets(const Vector3& scale, const Vector3& rotation, const Vector3& translation);

    //*----------------------------- setter Methods -----------------------------*//
    void SetLoop(bool isLoop) { isLoop_ = isLoop; }
    bool GetIsLoop() const { return isLoop_; }
};

}; // KetaEngine
