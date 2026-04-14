#include "ParticleFactory.h"

using namespace KetaEngine;

#include "Frame/Frame.h"
#include "MathFunction.h"
#include "random.h"

ParticleManager::Particle ParticleFactory::MakeParticle(
    const ParticleManager::Parameters& parameters,
    const ParticleManager::DissolveGroupParams* dissolveParams,
    const ViewProjection* viewProjection) {

    ParticleManager::Particle particle;

    /// Init
    particle.lifeTime_       = parameters.lifeTime;
    particle.currentTime_    = 0.0f;
    particle.worldTransform_ = std::make_unique<WorldTransform>();
    particle.worldTransform_->Init();

    ///------------------------------------------------------------------------
    /// ペアレント設定
    ///------------------------------------------------------------------------
    if (parameters.parentTransform) {
        particle.worldTransform_->parent_ = parameters.parentTransform;
    }
    if (parameters.jointParent.animation) {
        particle.worldTransform_->SetParentJoint(parameters.jointParent.animation, parameters.jointParent.name);
    }
    if (parameters.followingPos_) {
        particle.followPos = parameters.followingPos_;
    }

    ///------------------------------------------------------------------------
    /// 座標設定
    ///------------------------------------------------------------------------
    if (parameters.emitShape == ParticleCommon::EmitShape::Sphere) {
        // 球面上のランダム方向を生成
        Vector3 sphereDir;
        do {
            sphereDir = {
                Random::Range(-1.0f, 1.0f),
                Random::Range(-1.0f, 1.0f),
                Random::Range(-1.0f, 1.0f)};
        } while (sphereDir.Dot(sphereDir) < 0.0001f);
        sphereDir = sphereDir.Normalize();

        float radius = Random::Range(parameters.sphereRadius.min, parameters.sphereRadius.max);
        Vector3 sphereOffset = sphereDir * radius;

        particle.worldTransform_->translation_ = parameters.targetPos + parameters.emitPos + sphereOffset;
        particle.offSet                        = particle.worldTransform_->translation_;

        // 速度方向は球面の外向き
        float speed = Random::Range(parameters.speedDist.min, parameters.speedDist.max);
        particle.direction_      = sphereDir;
        particle.speed_          = speed;
        particle.speedV3         = sphereDir * speed;
        particle.isFloatVelocity = true;
    } else {
        Vector3 randomTranslate = {
            Random::Range(parameters.positionDist.min.x, parameters.positionDist.max.x),
            Random::Range(parameters.positionDist.min.y, parameters.positionDist.max.y),
            Random::Range(parameters.positionDist.min.z, parameters.positionDist.max.z)};

        particle.worldTransform_->translation_ = parameters.targetPos + parameters.emitPos + randomTranslate;
        particle.offSet                        = parameters.targetPos + parameters.emitPos + randomTranslate;

        ///------------------------------------------------------------------------
        /// 速度、向き設定
        ///------------------------------------------------------------------------
        if (parameters.isFloatVelocity) {
            Vector3 direction = {
                Random::Range(parameters.directionDist.min.x, parameters.directionDist.max.x),
                Random::Range(parameters.directionDist.min.y, parameters.directionDist.max.y),
                Random::Range(parameters.directionDist.min.z, parameters.directionDist.max.z)};
            direction                      = direction.Normalize();
            float speed                    = Random::Range(parameters.speedDist.min, parameters.speedDist.max);
            Matrix4x4 cameraRotationMatrix = MakeRotateMatrix(viewProjection->rotation_);
            particle.direction_            = TransformNormal(direction, cameraRotationMatrix);
            particle.speed_                = speed;
        } else {
            Vector3 velocity = {
                Random::Range(parameters.velocityDistV3.min.x, parameters.velocityDistV3.max.x),
                Random::Range(parameters.velocityDistV3.min.y, parameters.velocityDistV3.max.y),
                Random::Range(parameters.velocityDistV3.min.z, parameters.velocityDistV3.max.z)};
            Matrix4x4 cameraRotationMatrix = MakeRotateMatrix(viewProjection->rotation_);
            velocity                       = TransformNormal(velocity, cameraRotationMatrix);
            particle.direction_            = velocity;
            particle.speedV3               = velocity;
        }
        particle.isFloatVelocity = parameters.isFloatVelocity;
    }

    ///------------------------------------------------------------------------
    /// 回転設定
    ///------------------------------------------------------------------------
    if (parameters.isRotateForDirection) {
        particle.worldTransform_->rotation_ = DirectionToEulerAngles(particle.direction_, *viewProjection);
    } else {
        Vector3 rotate = {
            Random::Range(parameters.rotateDist.min.x, parameters.rotateDist.max.x),
            Random::Range(parameters.rotateDist.min.y, parameters.rotateDist.max.y),
            Random::Range(parameters.rotateDist.min.z, parameters.rotateDist.max.z)};
        particle.worldTransform_->rotation_ = ToRadian(parameters.baseRotate + rotate);
    }

    ///------------------------------------------------------------------------
    /// 回転スピード設定
    ///------------------------------------------------------------------------
    Vector3 rotateSpeed = {
        Random::Range(parameters.rotateSpeedDist.min.x, parameters.rotateSpeedDist.max.x),
        Random::Range(parameters.rotateSpeedDist.min.y, parameters.rotateSpeedDist.max.y),
        Random::Range(parameters.rotateSpeedDist.min.z, parameters.rotateSpeedDist.max.z)};
    particle.rotateSpeed_ = rotateSpeed;

    ///------------------------------------------------------------------------
    ///  スケール設定
    ///------------------------------------------------------------------------
    if (parameters.isScalerScale) {
        // スカラー値の場合
        float scale                      = Random::Range(parameters.scaleDist.min, parameters.scaleDist.max);
        particle.worldTransform_->scale_ = {scale, scale, scale};
        particle.scaleInfo.tempScaleV3   = particle.worldTransform_->scale_;

        float endScale                  = Random::Range(parameters.scaleEaseParam.endValueF.min, parameters.scaleEaseParam.endValueF.max);
        particle.scaleInfo.easeEndScale = {endScale, endScale, endScale};
    } else {
        // Vector3の場合
        Vector3 ScaleV3 = {
            Random::Range(parameters.scaleDistV3.min.x, parameters.scaleDistV3.max.x),
            Random::Range(parameters.scaleDistV3.min.y, parameters.scaleDistV3.max.y),
            Random::Range(parameters.scaleDistV3.min.z, parameters.scaleDistV3.max.z)};
        particle.worldTransform_->scale_ = ScaleV3;
        particle.scaleInfo.tempScaleV3   = ScaleV3;

        Vector3 endScaleV3 = {
            Random::Range(parameters.scaleEaseParam.endValueV3.min.x, parameters.scaleEaseParam.endValueV3.max.x),
            Random::Range(parameters.scaleEaseParam.endValueV3.min.y, parameters.scaleEaseParam.endValueV3.max.y),
            Random::Range(parameters.scaleEaseParam.endValueV3.min.z, parameters.scaleEaseParam.endValueV3.max.z)};
        particle.scaleInfo.easeEndScale = endScaleV3;
    }

    //  Easingパラメータを設定
    particle.scaleInfo.easeParam.baseParam.isEase  = parameters.scaleEaseParam.baseParam.isEase;
    particle.scaleInfo.easeParam.baseParam.maxTime = parameters.scaleEaseParam.baseParam.maxTime;

    //  Easingクラスのインスタンスを作成して設定
    if (parameters.scaleEaseParam.baseParam.isEase) {
        particle.scaleEasing   = std::make_unique<Easing<Vector3>>();
        particle.isAdaptEasing = true;

        // Easingパラメータを構築
        EasingParameter<Vector3> easingParam;
        easingParam.type       = static_cast<EasingType>(parameters.scaleEaseParam.baseParam.easeTypeInt);
        easingParam.startValue = particle.scaleInfo.tempScaleV3;
        easingParam.endValue   = particle.scaleInfo.easeEndScale;

        // isReturnToOrigin: maxTime=前進時間, returnMaxTime=後退時間, returnType=後退イージング種類
        // EasingクラスのSettingValue内で合計時間に変換される
        const float returnTime = parameters.scaleEaseParam.baseParam.returnMaxTime;
        easingParam.maxTime    = parameters.scaleEaseParam.baseParam.maxTime;
        if (parameters.scaleEaseParam.baseParam.isReturnToOrigin && returnTime > 0.0f) {
            easingParam.returnMaxTime = returnTime;
            easingParam.returnType    = static_cast<EasingType>(parameters.scaleEaseParam.baseParam.returnEaseTypeInt);
        } else {
            easingParam.returnMaxTime = 0.0f;
        }

        // Easingに設定
        particle.scaleEasing->SettingValue(easingParam);

        // WorldTransformのscaleに直接適用
        particle.scaleEasing->SetAdaptValue(&particle.worldTransform_->scale_);
    }

    ///------------------------------------------------------------------------
    ///  Translateイージング設定
    ///------------------------------------------------------------------------
    if (parameters.translateEaseParam.baseParam.isEase) {
        particle.translateEasing        = std::make_unique<Easing<Vector3>>();
        particle.isAdaptTranslateEasing = true;

        // 終了位置をランダムで決定
        Vector3 endTranslate = {
            Random::Range(parameters.translateEaseParam.endValue.min.x, parameters.translateEaseParam.endValue.max.x),
            Random::Range(parameters.translateEaseParam.endValue.min.y, parameters.translateEaseParam.endValue.max.y),
            Random::Range(parameters.translateEaseParam.endValue.min.z, parameters.translateEaseParam.endValue.max.z)};

        // 開始位置を保存
        particle.translateInfo.startPosition = particle.worldTransform_->translation_;
        particle.translateInfo.endPosition   = particle.worldTransform_->translation_ + endTranslate;

        // Easingパラメータを構築
        EasingParameter<Vector3> easingParam;
        easingParam.type       = static_cast<EasingType>(parameters.translateEaseParam.baseParam.easeTypeInt);
        easingParam.startValue = particle.translateInfo.startPosition;
        easingParam.endValue   = particle.translateInfo.endPosition;
        easingParam.maxTime    = parameters.translateEaseParam.baseParam.maxTime;

        const float translateReturnTime = parameters.translateEaseParam.baseParam.returnMaxTime;
        if (parameters.translateEaseParam.baseParam.isReturnToOrigin && translateReturnTime > 0.0f) {
            easingParam.returnMaxTime = translateReturnTime;
            easingParam.returnType    = static_cast<EasingType>(parameters.translateEaseParam.baseParam.returnEaseTypeInt);
        } else {
            easingParam.returnMaxTime = 0.0f;
            easingParam.finishType    = EasingFinishValueType::End;
        }

        // Easingに設定
        particle.translateEasing->SettingValue(easingParam);

        // WorldTransformのtranslationに直接適用
        particle.translateEasing->SetAdaptValue(&particle.worldTransform_->translation_);
    }

    ///------------------------------------------------------------------------
    ///  Rotateイージング設定
    ///------------------------------------------------------------------------
    if (parameters.rotateEaseParam.baseParam.isEase) {
        particle.rotateEasing        = std::make_unique<Easing<Vector3>>();
        particle.isAdaptRotateEasing = true;

        // 終了回転をランダムで決定
        Vector3 endRotate = {
            Random::Range(parameters.rotateEaseParam.endValue.min.x, parameters.rotateEaseParam.endValue.max.x),
            Random::Range(parameters.rotateEaseParam.endValue.min.y, parameters.rotateEaseParam.endValue.max.y),
            Random::Range(parameters.rotateEaseParam.endValue.min.z, parameters.rotateEaseParam.endValue.max.z)};

        // 度からラジアンに変換
        endRotate = ToRadian(endRotate);

        // 開始回転を保存
        particle.rotateInfo.startRotation = particle.worldTransform_->rotation_;
        particle.rotateInfo.endRotation   = particle.worldTransform_->rotation_ + endRotate;

        // Easingパラメータを構築
        EasingParameter<Vector3> easingParam;
        easingParam.type       = static_cast<EasingType>(parameters.rotateEaseParam.baseParam.easeTypeInt);
        easingParam.startValue = particle.rotateInfo.startRotation;
        easingParam.endValue   = particle.rotateInfo.endRotation;
        easingParam.maxTime    = parameters.rotateEaseParam.baseParam.maxTime;
        easingParam.finishType = EasingFinishValueType::End;

        // Easingに設定
        particle.rotateEasing->SettingValue(easingParam);

        // WorldTransformのrotationに直接適用
        particle.rotateEasing->SetAdaptValue(&particle.worldTransform_->rotation_);
    }

    ///------------------------------------------------------------------------
    /// 色設定
    ///------------------------------------------------------------------------
    Vector4 randomColor = {
        Random::Range(parameters.colorDist.min.x, parameters.colorDist.max.x),
        Random::Range(parameters.colorDist.min.y, parameters.colorDist.max.y),
        Random::Range(parameters.colorDist.min.z, parameters.colorDist.max.z),
        0.0f};
    particle.color_ = parameters.baseColor + randomColor;

    ///------------------------------------------------------------------------
    ///  カラーイージング設定（RGB のみ）
    ///------------------------------------------------------------------------
    if (parameters.colorEaseParam.baseParam.isEase) {
        particle.colorEasing        = std::make_unique<Easing<Vector3>>();
        particle.isAdaptColorEasing = true;

        Vector3 endColor = {
            Random::Range(parameters.colorEaseParam.endValue.min.x, parameters.colorEaseParam.endValue.max.x),
            Random::Range(parameters.colorEaseParam.endValue.min.y, parameters.colorEaseParam.endValue.max.y),
            Random::Range(parameters.colorEaseParam.endValue.min.z, parameters.colorEaseParam.endValue.max.z)};

        particle.colorInfo.startColor = {particle.color_.x, particle.color_.y, particle.color_.z};
        particle.colorInfo.endColor   = endColor;

        EasingParameter<Vector3> easingParam;
        easingParam.type       = static_cast<EasingType>(parameters.colorEaseParam.baseParam.easeTypeInt);
        easingParam.startValue = particle.colorInfo.startColor;
        easingParam.endValue   = particle.colorInfo.endColor;
        easingParam.maxTime    = parameters.colorEaseParam.baseParam.maxTime;
        easingParam.finishType = EasingFinishValueType::End;

        particle.colorEasing->SettingValue(easingParam);
        // color_.xyz に直接書き込む
        particle.colorEasing->SetAdaptValue(reinterpret_cast<Vector3*>(&particle.color_));
    }

    ///------------------------------------------------------------------------
    ///  アルファモード・イージング設定
    ///------------------------------------------------------------------------
    particle.alphaMode = parameters.alphaMode;

    if (parameters.alphaMode == ParticleCommon::AlphaMode::Easing) {
        particle.alphaEasing        = std::make_unique<Easing<float>>();
        particle.isAdaptAlphaEasing = true;

        float endAlpha = Random::Range(parameters.alphaEaseParam.endValue.min, parameters.alphaEaseParam.endValue.max);

        EasingParameter<float> easingParam;
        easingParam.type       = static_cast<EasingType>(parameters.alphaEaseParam.baseParam.easeTypeInt);
        easingParam.startValue = particle.color_.w;
        easingParam.endValue   = endAlpha;
        easingParam.maxTime    = parameters.alphaEaseParam.baseParam.maxTime;
        easingParam.finishType = EasingFinishValueType::End;

        particle.alphaEasing->SettingValue(easingParam);
        particle.alphaEasing->SetAdaptValue(&particle.color_.w);
    }

    ///------------------------------------------------------------------------
    /// UVTransform設定
    ///------------------------------------------------------------------------
    float frameWidth = 1.0f;
    if (parameters.uvParam.numOfFrame != 0) {
        frameWidth = 1.0f / float(parameters.uvParam.numOfFrame);
    }
    const float stopPosition = 1.0f - frameWidth;

    particle.uvInfo_.pos               = Vector3(parameters.uvParam.pos.x, parameters.uvParam.pos.y, 1.0f);
    particle.uvInfo_.rotate            = parameters.uvParam.rotate;
    particle.uvInfo_.scale             = Vector3(frameWidth * parameters.uvParam.scale.x, parameters.uvParam.scale.y, 1.0f);
    particle.uvInfo_.frameDistance_    = frameWidth;
    particle.uvInfo_.frameScrollSpeed  = parameters.uvParam.frameScrollSpeed;
    particle.uvInfo_.uvStopPos_        = stopPosition;
    particle.uvInfo_.uvScrollModeInt   = parameters.uvParam.uvScrollModeInt;
    particle.uvInfo_.isLoop            = parameters.uvParam.isLoop;
    particle.uvInfo_.isFlipX           = parameters.uvParam.isFlipX;
    particle.uvInfo_.isFlipY           = parameters.uvParam.isFlipY;
    particle.uvInfo_.currentScrollTime = 0.0f;

    ///------------------------------------------------------------------------
    /// 重力設定
    ///------------------------------------------------------------------------
    particle.gravity_ = parameters.gravity;

    ///------------------------------------------------------------------------
    /// Dissolveイージング設定
    ///------------------------------------------------------------------------
    if (dissolveParams && dissolveParams->isActive) {
        particle.dissolveOffsetTime     = dissolveParams->offsetTime;
        particle.dissolveThresholdData_ = std::make_unique<float>(dissolveParams->startThreshold);
        particle.dissolveEasing         = std::make_unique<Easing<float>>();
        particle.isAdaptDissolveEasing  = true;

        EasingParameter<float> dissolveEasingParam;
        dissolveEasingParam.type       = static_cast<EasingType>(dissolveParams->easeType);
        dissolveEasingParam.startValue = dissolveParams->startThreshold;
        dissolveEasingParam.endValue   = dissolveParams->endThreshold;
        dissolveEasingParam.maxTime    = dissolveParams->maxTime;
        dissolveEasingParam.finishType = EasingFinishValueType::End;

        particle.dissolveEasing->SettingValue(dissolveEasingParam);
        particle.dissolveEasing->SetAdaptValue(particle.dissolveThresholdData_.get());
    }

    return particle;
}
