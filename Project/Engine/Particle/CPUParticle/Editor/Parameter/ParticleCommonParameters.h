#pragma once
#include "3D/WorldTransform.h"
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

/// <summary>
/// UVパラメータ
/// </summary>
struct UVParam {
    float frameScrollSpeed  = 0.0f;
    bool isLoop             = false;
    bool isFlipX            = false;
    bool isFlipY            = false;
    Vector2 pos             = {};
    Vector2 scale           = {1.0f, 1.0f}; 
    Vector3 rotate          = {};
    int32_t numOfFrame      = 0;
};

/// <summary>
/// ジョイント親
/// </summary>
struct JointParent {
    std::string name;
    const Object3DAnimation* animation = nullptr;
};

/// <summary>
/// 前進・戻りフェーズを持つ汎用イージングパラメータ
/// </summary>
struct EaseParam {
    float maxTime              = 1.0f;
    bool isEase                = false;
    int32_t easeTypeInt        = 0;     // 前進フェーズのイージング種類
    float returnMaxTime        = 0.0f;  // 戻りフェーズの時間
    int32_t returnEaseTypeInt  = 0;     // 戻りフェーズのイージング種類
    bool isReturnToOrigin      = false; // true のとき前進後に起点へ戻るイージングを行う
};

/// <summary>
/// スケールイージングパラメータ
/// </summary>
struct ScaleEaseParam {
    EaseParam baseParam;
    FMinMax endValueF = {};
    V3MinMax endValueV3;
};

/// <summary>
/// トランスレートイージングパラメータ
/// </summary>
struct EasingParamV3 {
    EaseParam baseParam;
    V3MinMax endValue;
};

/// <summary>
/// カラーイージングパラメータ
/// </summary>
struct ColorEaseParam {
    EaseParam baseParam;
    V3MinMax endValue;
};

// アルファモード
enum class AlphaMode {
    None,     // 消えない
    LifeTime, // 時間経過で消える
    Easing,   // イージングで消える
};

/// <summary>
/// アルファイージングパラメータ
/// </summary>
struct AlphaEaseParam {
    EaseParam baseParam;
    FMinMax endValue = {}; // 終了アルファ範囲
};

/// <summary>
/// カラー+アルファ(RGBA)イージングパラメータ
/// </summary>
struct ColorAlphaEaseParam {
    EaseParam baseParam;
    V4MinMax endValue;
};

// エミット形状
enum class EmitShape {
    None,   ///< 通常
    Sphere, ///< 球面から全方向に放射
};

/// <summary>
/// パーティクルパラメータ
/// </summary>
struct Parameters {
    const WorldTransform* parentTransform = nullptr;
    const Vector3* followingPos_          = nullptr;
    Vector3 targetPos;
    Vector3 emitPos;
    Vector3 targetRotate;
    Vector3 baseRotate;
    Vector4 baseColor;
    V3MinMax scaleDistV3;
    V3MinMax positionDist;
    V3MinMax directionDist;
    V3MinMax rotateDist;
    V3MinMax rotateSpeedDist;
    V3MinMax velocityDistV3;
    FMinMax scaleDist              = {};
    FMinMax speedDist              = {};
    V4MinMax colorDist;
    float lifeTime = 0.0f;
    float gravity  = 0.0f;
    bool isFloatVelocity        = false;
    bool isScalerScale          = false;
    bool isRotateForDirection   = false;
    bool rotateForDirectionX    = true;
    bool rotateForDirectionY    = true;
    bool rotateForDirectionZ    = true;
    ScaleEaseParam scaleEaseParam;
    EasingParamV3 translateEaseParam;
    EasingParamV3 rotateEaseParam;
    ColorEaseParam colorEaseParam;
    ColorAlphaEaseParam colorAlphaEaseParam;
    AlphaMode alphaMode = AlphaMode::LifeTime;
    AlphaEaseParam alphaEaseParam;
    UVParam uvParam;
    JointParent jointParent;
    // Shape
    EmitShape emitShape = EmitShape::None;
    FMinMax sphereRadius = {};
    // ゴール誘導
    bool useGoalPosition = false;
    Vector3 goalOffset;       // エディタで設定するオフセット
    Vector3 goalTargetPos;    // ランタイムで設定するターゲット位置（JSON非保存）
};

/// <summary>
/// グループパラメータ
/// </summary>
struct GroupParameters {
    BlendMode blendMode         = BlendMode::None;
    bool isBillboard            = false;
    bool isShot                 = false;
    bool isAlphaNoMove          = false;
    bool isScreenPos            = false;
    BillboardType billboardType = BillboardType::XYZ;
    AdaptRotate adaptRotate_    = {};

    // 歪みエフェクト
    bool useDistortion        = false;
    float distortionStrength  = 0.05f;

    // 自己発光
    float emissiveIntensity = 1.0f;
};

} // namespace ParticleCommon

} // namespace KetaEngine