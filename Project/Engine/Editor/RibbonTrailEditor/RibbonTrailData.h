#pragma once
#include "Editor/BaseEffectEditor/BaseEffectData.h"
#include "utility/FileSelector/FileSelector.h"
#include "Vector2.h"
#include "Vector4.h"
#include <cstdint>
#include <string>

namespace KetaEngine {

/// トレイルの追従モード
enum class TrailFollowMode {
    Follow,       ///< 対象に追従しながらポイントを追加し続ける（デフォルト）
    StayInPlace,  ///< 自動追従しない。ポイントはその場に留まり消えていく
    Arc,          ///< 弧モード。SetArc() で形状を一括生成する
};

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
    Vector4 startColor_ = {1.0f, 0.8f, 0.3f, 1.0f};
    Vector4 endColor_   = {1.0f, 0.4f, 0.1f, 0.0f};

    // ---- 幅 ----
    float startWidth_ = 0.12f;
    float endWidth_   = 0.0f;

    // ---- 寿命・長さ ----
    float   lifetime_     = 0.25f;
    int32_t maxPoints_    = 48;
    float   emitInterval_ = 0.005f;

    // ---- UVスクロール ----
    Vector2 uvScrollSpeed_           = Vector2::ZeroVector();
    Vector2 distortionUVScrollSpeed_ = Vector2::ZeroVector();

    // ---- テクスチャ ----
    std::string texturePath_;

    const std::string kTextureFolderPath    = "Resources/EngineTexture/Trail";
    const std::string kDistortionFolderPath = "Resources/EngineTexture/Noise";
    FileSelector textureSelector_;

    // ---- 追従モード ----
    TrailFollowMode followMode_    = TrailFollowMode::Follow;
    int32_t         followModeInt_ = 0;

    // ---- 時空歪み ----
    bool        useDistortion_         = false;
    float       distortionStrength_    = 0.1f;
    std::string distortionTexturePath_;
    FileSelector distortionTextureSelector_;

    // ---- 弧モードパラメータ ----
    float   arcStartAngleDeg_ = 0.0f;    ///< 弧の開始角度（度数法）
    float   arcEndAngleDeg_   = 180.0f;  ///< 弧の終了角度（度数法）
    float   arcRadius_        = 1.5f;    ///< 弧の半径
    int32_t arcDirectionInt_  = 0;       ///< 0=反時計回り, 1=時計回り
    int32_t arcPlaneInt_      = 0;       ///< 0=XZ, 1=XY, 2=YZ
    float   arcLifetime_      = 0.5f;    ///< 弧ポイントの生存時間

    const std::string baseFolderPath_ = "RibbonTrail/";

public:
    // ---- Getter ----
    Vector4            GetStartColor()              const { return startColor_; }
    Vector4            GetEndColor()                const { return endColor_; }
    float              GetStartWidth()              const { return startWidth_; }
    float              GetEndWidth()                const { return endWidth_; }
    float              GetLifetime()                const { return lifetime_; }
    int32_t            GetMaxPoints()               const { return maxPoints_; }
    float              GetEmitInterval()            const { return emitInterval_; }
    Vector2            GetUVScrollSpeed()           const { return uvScrollSpeed_; }
    Vector2            GetDistortionUVScrollSpeed() const { return distortionUVScrollSpeed_; }
    const std::string& GetTexturePath()             const { return texturePath_; }
    TrailFollowMode    GetFollowMode()              const { return followMode_; }
    bool               GetUseDistortion()           const { return useDistortion_; }
    float              GetDistortionStrength()      const { return distortionStrength_; }
    const std::string& GetDistortionTexturePath()   const { return distortionTexturePath_; }

    // 弧モード用 Getter
    float   GetArcStartAngleDeg() const { return arcStartAngleDeg_; }
    float   GetArcEndAngleDeg()   const { return arcEndAngleDeg_; }
    float   GetArcRadius()        const { return arcRadius_; }
    int32_t GetArcDirectionInt()  const { return arcDirectionInt_; }
    int32_t GetArcPlaneInt()      const { return arcPlaneInt_; }
    float   GetArcLifetime()      const { return arcLifetime_; }
};

} // KetaEngine
