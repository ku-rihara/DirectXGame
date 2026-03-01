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
    float   lifetime_     = 0.25f; // 各ポイントの生存時間（秒）
    int32_t maxPoints_    = 48;    // 保持する最大ポイント数（= トレイルの最大長さ）
    float   emitInterval_ = 0.005f; // ポイントを追加する間隔（秒）

    // ---- テクスチャ ----
    std::string texturePath_; // 空文字列 = デフォルト白テクスチャ（フルパスで保存）

    // テクスチャ選択（エディタ用）
    static constexpr const char* kTextureFolderPath = "Resources/EngineTexture";
    FileSelector                 textureSelector_;
    std::string                  textureStem_; // FileSelector が管理するステム名（一時変数）

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
    const std::string& GetTexturePath()   const { return texturePath_; }
};

} // KetaEngine
