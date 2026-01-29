#pragma once
#include "Editor/BaseEffectEditor/BaseSequenceEffectData.h"
#include "GPUParticleSection.h"
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// GPUパーティクルデータクラス
/// </summary>
namespace KetaEngine {

class GPUParticleData : public BaseSequenceEffectData<GPUParticleSection> {
public:
    GPUParticleData()           = default;
    ~GPUParticleData() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectDataからのオーバーライド
    void Update(float speedRate = 1.0f) override;
    void Reset() override;
    void Play() override;

    // カテゴリー対応初期化
    void Init(const std::string& name, const std::string& categoryName) override;

    // 編集用メソッド
    void AdjustParam();

    // 全セクションの描画指示
    void Draw();

protected:
    //*----------------------------- protected Methods from Base -----------------------------*//
    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

    // BaseSequenceEffectDataからのオーバーライド
    void UpdateKeyFrameProgression() override;
    void AdvanceToNextSequenceElement() override;
    std::unique_ptr<GPUParticleSection> CreateKeyFrame(int32_t index) override;
    std::string GetSequenceElementFolderPath() const override;

private:
    //*---------------------------- private Methods ----------------------------*//
    void UpdateActiveSections(float speedRate);

private:
    //*---------------------------- private Variant ----------------------------*//
    const std::string baseFolderPath_ = "GPUParticle/";

    // 全体設定
    bool drawAllSections_ = true; // 全セクションを同時描画するか

public:
    //*----------------------------- getter Methods -----------------------------*//
    const std::string& GetCategoryName() const { return categoryName_; }
    bool GetDrawAllSections() const { return drawAllSections_; }
    GPUParticleSection* GetKeyFrame(int32_t index);
    //*----------------------------- setter Methods -----------------------------*//
    void SetDrawAllSections(bool value) { drawAllSections_ = value; }
};

}; // KetaEngine
