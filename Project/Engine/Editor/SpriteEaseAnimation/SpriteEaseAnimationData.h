#pragma once
#include "Editor/BaseEffectEditor/BaseSequenceEffectData.h"
#include "SpriteEaseAnimationSection.h"
#include "Vector2.h"
#include "Vector3.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>

namespace KetaEngine {

/// <summary>
/// スプライトイージングアニメーションデータ
/// </summary>
class SpriteEaseAnimationData : public BaseSequenceEffectData<SpriteEaseAnimationSection> {
public:
    enum class PropertyType {
        Scale,
        Position,
        Rotation,
        Color,
        Alpha,
        Count
    };

    static constexpr size_t kPropertyCount = static_cast<size_t>(PropertyType::Count);

public:
    SpriteEaseAnimationData()           = default;
    ~SpriteEaseAnimationData() override = default;

    //*----------------------------- public Methods -----------------------------*//

    void Update(float speedRate = 1.0f) override;
    void Reset() override;
    void Play() override;

    void Init(const std::string& animationName, const std::string& categoryName) override;

    void AdjustParam();

    //*----------------------------- protected Methods from Base -----------------------------*//
protected:
    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

    void UpdateKeyFrameProgression() override;
    void AdvanceToNextSequenceElement() override;
    std::unique_ptr<SpriteEaseAnimationSection> CreateKeyFrame(int32_t index) override;
    std::string GetSequenceElementFolderPath() const override;

private:
    //*---------------------------- private Methods ----------------------------*//
    const char* GetPropertyName(PropertyType type) const;
    void UpdateActiveSection(float speedRate);

    void UpdateIndependentProgression();
    void AdvancePropertyToNextSection(PropertyType type);
    bool AreAllPropertiesFinished() const;

private:
    //*---------------------------- private Variant ----------------------------*//

    const std::string baseFolderPath_ = "SpriteEaseAnimation/";

    // originParam
    Vector2 originalScale_    = Vector2::OneVector();
    Vector2 originalPosition_ = Vector2::ZeroVector();
    Vector3 originalRotation_ = Vector3::ZeroVector();
    Vector3 originalColor_    = {1.0f, 1.0f, 1.0f};
    float originalAlpha_      = 1.0f;

    // 独立進行用
    std::array<int32_t, kPropertyCount> activeSectionIndices_    = {0, 0, 0, 0, 0};
    std::array<bool, kPropertyCount> allSectionsFinished_        = {false, false, false, false, false};

public:
    //*----------------------------- getter Methods -----------------------------*//
    const std::string& GetCategoryName() const { return categoryName_; }

    const Vector2& GetOriginalScale() const { return originalScale_; }
    const Vector2& GetOriginalPosition() const { return originalPosition_; }
    const Vector3& GetOriginalRotation() const { return originalRotation_; }
    const Vector3& GetOriginalColor() const { return originalColor_; }
    float GetOriginalAlpha() const { return originalAlpha_; }

    Vector2 GetActiveScale() const;
    Vector2 GetActivePosition() const;
    Vector3 GetActiveRotation() const;
    Vector3 GetActiveColor() const;
    float GetActiveAlpha() const;

    bool IsPropertyActive(PropertyType type) const;

    void SetPreAnimationOffsets(const Vector2& scale, const Vector2& position,
                                const Vector3& rotation, const Vector3& color, float alpha);
};

}; // KetaEngine
