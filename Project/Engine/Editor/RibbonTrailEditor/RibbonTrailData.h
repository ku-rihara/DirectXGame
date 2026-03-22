#pragma once
#include "Editor/BaseEffectEditor/BaseEffectData.h"
#include "utility/FileSelector/FileSelector.h"
#include "Vector4.h"
#include <cstdint>
#include <string>

namespace KetaEngine {

/// <summary>
/// リボントレイルデータクラス
/// トレイルの外観パラメータを保持・保存する汎用データ
/// </summary>
class RibbonTrailData : public BaseEffectData {
public:
    RibbonTrailData()  = default;
    ~RibbonTrailData() = default;

    void Init(const std::string& name, const std::string& categoryName) override;
    void Update(float speedRate = 1.0f) override;
    void Play() override;
    void Reset() override;
    void AdjustParam();

protected:
    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

private:
    // ---- 色 ----
    Vector4 startColor_ = {1.0f, 0.8f, 0.3f, 1.0f}; // ヘッド（最新点）の色
    Vector4 endColor_   = {1.0f, 0.4f, 0.1f, 0.0f}; // テール（最古点）の色

    // ---- 幅 ----
    float startWidth_ = 0.12f; // ヘッドの幅
    float endWidth_   = 0.0f;  // テールの幅（先端が鋭く消える）

    // ---- 寿命・長さ ----
    float   lifetime_     = 0.25f;  // 各ポイントの生存時間（秒）
    int32_t maxPoints_    = 48;     // 保持する最大ポイント数
    float   emitInterval_ = 0.005f; // ポイントを追加する間隔（秒）

    // ---- テクスチャ ----
    std::string texturePath_;

    // テクスチャ選択（エディタ用）
    static constexpr const char* kTextureFolderPath = "Resources/EngineTexture";
    FileSelector                 textureSelector_;
    std::string                  textureStem_;

    // ---- 時空歪み ----
    bool        useDistortion_         = false;
    float       distortionStrength_    = 0.1f;
    std::string distortionTexturePath_;

    // 歪みテクスチャ選択（エディタ用）
    FileSelector distortionTextureSelector_;
    std::string  distortionTextureStem_;

    const std::string baseFolderPath_ = "RibbonTrail/";

public:
    // ---- Getter ----
    Vector4            GetStartColor()    const { return startColor_; }
    Vector4            GetEndColor()      const { return endColor_; }
    float              GetStartWidth()    const { return startWidth_; }
    float              GetEndWidth()      const { return endWidth_; }
    float              GetLifetime()      const { return lifetime_; }
    int32_t            GetMaxPoints()     const { return maxPoints_; }
    float              GetEmitInterval()  const { return emitInterval_; }
    const std::string& GetTexturePath()           const { return texturePath_; }
    bool               GetUseDistortion()         const { return useDistortion_; }
    float              GetDistortionStrength()     const { return distortionStrength_; }
    const std::string& GetDistortionTexturePath() const { return distortionTexturePath_; }
};

} // KetaEngine
