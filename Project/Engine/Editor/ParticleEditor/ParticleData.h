#pragma once
#include "Editor/BaseEffectEditor/BaseSequenceEffectData.h"
#include "ParticleSection.h"
#include <memory>
#include <string>
#include <vector>

namespace KetaEngine {

class ParticleData : public BaseSequenceEffectData<ParticleSection> {
   
   
public:
    ParticleData()           = default;
    ~ParticleData() override = default;

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

protected:
    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

    // BaseSequenceEffectDataからのオーバーライド
    void UpdateKeyFrameProgression() override;
    void AdvanceToNexTSequenceElement() override;
    std::unique_ptr<ParticleSection> CreateKeyFrame(int32_t index) override;
    std::string GeTSequenceElementFolderPath() const override;

private:
    void UpdateActiveSections(float speedRate);

private:
    const std::string baseFolderPath_ = "Particle/";

public:
    const std::string& GetCategoryName() const { return categoryName_; }

    // セクション要素へのアクセス
    std::vector<std::unique_ptr<ParticleSection>>& GetSectionElements() {
        return sectionElements_;
    }
    const std::vector<std::unique_ptr<ParticleSection>>& GetSectionElements() const {
        return sectionElements_;
    }

  
};

}