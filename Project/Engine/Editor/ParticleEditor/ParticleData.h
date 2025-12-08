#pragma once
#include "Editor/BaseEffectEditor/BaseSequenceEffectData.h"
#include "ParticleSection.h"
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// パーティクルデータクラス
/// </summary>
class ParticleData : public BaseSequenceEffectData<ParticleSection> {
public:
    ParticleData()           = default;
    ~ParticleData() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectDataからのオーバーライド
    void Update(float speedRate = 1.0f) override;
    void Reset() override;
    void Play() override;

    // カテゴリー対応初期化
    void InitWithCategory(const std::string& name, const std::string& categoryName) override;

    // 編集用メソッド
    void AdjustParam();
    void LoadSequenceElements() override;
    void SaveSequenceElements() override;

    // 全セクションの描画指示
    void Draw();

protected:
    //*----------------------------- protected Methods from Base -----------------------------*//
    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

    // BaseSequenceEffectDataからのオーバーライド
    void UpdateKeyFrameProgression() override;
    void AdvanceToNexTSequenceElement() override;
    std::unique_ptr<ParticleSection> CreateKeyFrame(int32_t index) override;
    std::string GeTSequenceElementFolderPath() const override;

private:
    //*---------------------------- private Methods ----------------------------*//
    void UpdateActiveSections(float speedRate);

private:
    //*---------------------------- private Variant ----------------------------*//
    const std::string baseFolderPath_ = "Particle/";

    // 全体設定
    bool drawAllSections_ = true; 

public:
    //*----------------------------- getter Methods -----------------------------*//
    const std::string& GetCategoryName() const { return categoryName_; }
    bool GetDrawAllSections() const { return drawAllSections_; }

    // セクション要素へのアクセス
    std::vector<std::unique_ptr<ParticleSection>>& GetSectionElements() {
        return sectionElements_;
    }
    const std::vector<std::unique_ptr<ParticleSection>>& GetSectionElements() const {
        return sectionElements_;
    }

    //*----------------------------- setter Methods -----------------------------*//
    void SetDrawAllSections(bool value) { drawAllSections_ = value; }
};