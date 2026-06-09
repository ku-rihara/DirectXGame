#include "AttackBomb.h"
#include "Frame/Frame.h"
#include "math/MathFunction.h"

void AttackBomb::Init(const Vector3& startPos, const Vector3& targetPos, const Param& param) {
    startPos_    = startPos;
    targetPos_   = targetPos;
    position_    = startPos;
    param_       = param;
    elapsedTime_ = 0.0f;
    isLanded_    = false;
    arcOffset_   = 0.0f;

    // イージングの初期化
    InitEasing(param_.arcHeight);

    // 3Dモデルの生成
    obj3d_.reset(KetaEngine::Object3d::CreateModel("Items/RottenBanana.obj"));
    obj3d_->transform_.Init();
    obj3d_->transform_.translation_ = startPos;
    obj3d_->transform_.scale_       = param_.scale;

    // トレイルの初期化
    trailPlayer_.Init();
    trailPlayer_.SetFollowPosition(&position_);
    trailPlayer_.Play("BombTrail", "AttackBomb");

    // Particle初期化
    particlePlayer_.Init();
    particlePlayer_.SetParentTransform(&obj3d_->transform_);
    particlePlayer_.Play("BomMoveEffect", "AttackBomb");
}

void AttackBomb::Update() {

    // トレイル更新
    trailPlayer_.Update();
    // パーティクル更新
    particlePlayer_.Update();

    if (isLanded_) {
        return;
    }

    // 経過時間とイージングの更新
    float dt = KetaEngine::Frame::DeltaTimeRate();
    arcEasing_.Update(dt);
    elapsedTime_ += dt;

    // 飛行時間を超えたら着地
    if (arcEasing_.IsFinished()) {
        position_ = targetPos_;
        isLanded_ = true;
        trailPlayer_.StopEmit();
        if (onLanded_) {
            onLanded_();
        }
        return;
    }

    // 放物線軌道の計算
    float t   = elapsedTime_ / param_.flightTime;
    position_ = Lerp(startPos_, targetPos_, t);
    position_.y += arcOffset_;

    // 3Dモデルの位置・回転を更新
    obj3d_->transform_.translation_ = position_;
    obj3d_->transform_.rotation_.z += param_.rotateZSpeed * dt;
}

void AttackBomb::StopTrail() {
    trailPlayer_.StopEmit();
}

void AttackBomb::InitEasing(float arcHeight) {
    KetaEngine::EasingParameter<float> param;
    param.type          = EasingType::OutSine;
    param.returnType    = EasingType::InSine;
    param.startValue    = 0.0f;
    param.endValue      = arcHeight;
    param.maxTime       = param_.flightTime / 2.0f;
    param.returnMaxTime = param_.flightTime / 2.0f;
    arcEasing_.SettingValue(param);
    arcEasing_.SetAdaptValue(&arcOffset_);
}
