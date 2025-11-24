#pragma once
#include "Editor/BaseEffectEditor/BaseSequenceEffectData.h"
#include "ObjEaseAnimationSection.h"
#include "Vector3.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>

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
    void Update(const float& speedRate = 1.0f) override;
    void Reset() override;

    // カテゴリー対応初期化
    void InitWithCategory(const std::string& animationName, const std::string& categoryName)override;

    // 編集用メソッド
    void AdjustParam();
    void LoadSequenceElements() override;
    void SaveSequenceElements() override;

    //*----------------------------- protected Methods from Base -----------------------------*//
protected:
    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

    // BaseSequenceEffectDataからのオーバーライド
    void UpdateKeyFrameProgression() override;
    void AdvanceToNexTSequenceElement() override;
    std::unique_ptr<ObjEaseAnimationSection> CreateKeyFrame(const int32_t& index) override;
    std::string GeTSequenceElementFolderPath() const override;

private:
    //*---------------------------- private Methods ----------------------------*//
    const char* GetSRTName(const TransformType& type) const;
    void CreateOrLoadSections(const std::vector<std::pair<int32_t, std::string>>& KeyFrameFiles);

private:
    //*---------------------------- private Variant ----------------------------*//
   
    const std::string baseFolderPath_ = "ObjEaseAnimation/";

    // originParam
    std::array<Vector3, static_cast<size_t>(TransformType::Count)> originalValues_;

public:
    //*----------------------------- getter Methods -----------------------------*//
    const std::string& GetCategoryName() const { return categoryName_; }
    const Vector3& GetOriginalValue(const TransformType& type) const {
        return originalValues_[static_cast<size_t>(type)];
    }
    Vector3 GetActiveKeyFrameValue(const TransformType& type) const;
    bool GetIsUseRailActiveKeyFrame() const;
    RailPlayer* GetCurrentRailPlayer() const;
};