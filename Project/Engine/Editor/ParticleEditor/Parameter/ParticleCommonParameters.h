#pragma once
#include "3d/WorldTransform.h"
#include "MinMax.h"
#include "Pipeline/Particle/ParticlePipeline.h"
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

// イージングタイプ
enum class EaseType {
    INSINE,
    OUTSINE,
    OUTBACK,
    OUTQUINT,
};

// UVパラメータ
struct UVParm {
    float frameScroolSpeed;
    bool isScroolEachPixel;
    bool isRoop;
    bool isScrool;
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

// スケールイージングパラメータ
struct ScaleEaseParam {
    float maxTime       = 1.0f;
    bool isScaleEase    = false;
    int32_t easeTypeInt = 0;
    EaseType easeType   = EaseType::INSINE;

    // スカラー値用
    float startValueF = 1.0f;
    FMinMax endValueF;

    // Vector3値用
    Vector3 startValueV3 = Vector3::OneVector();
    V3MinMax endValueV3;

    float backRatio = 0.0f;
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
    bool isRotateforDirection;
    ScaleEaseParam scaleEaseParm;
    UVParm uvParm;
    JointParent jointParent;
};

// グループパラメータ
struct GroupParamaters {
    BlendMode blendMode;
    bool isBillBord;
    bool isShot;
    bool isAlphaNoMove;
    BillboardType billBordType;
    AdaptRotate adaptRotate_;
};

} // namespace ParticleCommon

} // namespace KetaEngine