#pragma once
#include "3d/WorldTransform.h"
#include "Base/Material/ModelMaterial.h"
#include "MinMax.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cstdint>
#include <string>

namespace KetaEngine {

class Object3DAnimation;
class WorldTransform;

/// <summary>
/// パーティクル共通パラメータ
/// </summary>
namespace ParticleCommon {

// UVパラメータ
struct UVParam {
    float frameScrollSpeed;
    bool isScrollEachPixel;
    bool isLoop;
    bool isScroll;
    bool isFlipX;
    bool isFlipY;
    Vector2 pos;
    Vector3 rotate;
    int32_t numOfFrame;
};

// ジョイント親
struct JointParent {
    std::string name;
    const Object3DAnimation* animation = nullptr;
};

struct EaseParam {
    float maxTime       = 1.0f;
    bool isEase        = false;
    int32_t easeTypeInt = 0;
    float backRatio     = 0.0f;
};

// スケールイージングパラメータ
struct ScaleEaseParam {
    EaseParam baseParam;
    FMinMax endValueF;
    V3MinMax endValueV3;
};

// トランスレートイージングパラメータ
struct EasingParamV3 {
    EaseParam baseParam;
    V3MinMax endValue;
};

// カラーイージングパラメータ（RGB のみ）
struct ColorEaseParam {
    EaseParam baseParam;
    V3MinMax endValue;
};

// アルファモード
enum class AlphaMode {
    None,     // 消えない（常に1.0）
    LifeTime, // 時間経過で消える（デフォルト）
    Easing,   // イージングで消える
};

// アルファイージングパラメータ
struct AlphaEaseParam {
    EaseParam baseParam;
    FMinMax endValue; // 終了アルファ範囲
};

// エミット形状
enum class EmitShape {
    None,   ///< 通常（ベクトル指定）
    Sphere, ///< 球面から全方向に放射
};

// パーティクルパラメータ
struct Parameters {
    const WorldTransform* parentTransform = nullptr;
    const Vector3* followingPos_          = nullptr;
    Vector3 targetPos;
    Vector3 emitPos;
    Vector3 baseRotate;
    Vector4 baseColor;
    V3MinMax scaleDistV3;
    V3MinMax positionDist;
    V3MinMax directionDist;
    V3MinMax rotateDist;
    V3MinMax rotateSpeedDist;
    V3MinMax velocityDistV3;
    FMinMax scaleDist;
    FMinMax speedDist;
    V4MinMax colorDist;
    float lifeTime;
    float gravity;
    bool isFloatVelocity;
    bool isScalerScale;
    bool isRotateForDirection;
    ScaleEaseParam scaleEaseParam;
    EasingParamV3 translateEaseParam;
    EasingParamV3 rotateEaseParam;
    ColorEaseParam colorEaseParam;
    AlphaMode alphaMode = AlphaMode::LifeTime;
    AlphaEaseParam alphaEaseParam;
    UVParam uvParam;
    JointParent jointParent;
    // Shape
    EmitShape emitShape = EmitShape::None;
    FMinMax sphereRadius; // Sphere 形状のときの半径範囲
};

// グループパラメータ
struct GroupParameters {
    BlendMode blendMode;
    bool isBillboard;
    bool isShot;
    bool isAlphaNoMove;
    bool isScreenPos;
    BillboardType billboardType;
    AdaptRotate adaptRotate_;

    // 歪みエフェクト
    bool useDistortion        = false;
    float distortionStrength  = 0.05f;

    // 自己発光
    float emissiveIntensity = 1.0f;
};

} // namespace ParticleCommon

} // namespace KetaEngine