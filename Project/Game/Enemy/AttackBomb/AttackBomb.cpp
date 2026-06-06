#include "AttackBomb.h"
#include "Frame/Frame.h"
#include "math/MathFunction.h"

void AttackBomb::Init(const Vector3& startPos, const Vector3& targetPos, float flightTime, float arcHeight) {
    startPos_    = startPos;
    targetPos_   = targetPos;
    position_    = startPos;
    flightTime_  = flightTime;
    elapsedTime_ = 0.0f;
    isLanded_    = false;
    arcOffset_   = 0.0f;

    // イージングの初期化
    InitEasing(arcHeight);

    // 3Dモデルの生成
    obj3d_.reset(KetaEngine::Object3d::CreateModel("TestObj/axis.obj"));
    obj3d_->transform_.Init();
    obj3d_->transform_.translation_ = startPos;
}

void AttackBomb::Update() {
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
        if (onLanded_) {
            onLanded_();
        }
        return;
    }

    // 放物線軌道の計算
    float t   = elapsedTime_ / flightTime_;
    position_ = Lerp(startPos_, targetPos_, t);
    position_.y += arcOffset_;

    // 3Dモデルの位置を更新
    obj3d_->transform_.translation_ = position_;
}

void AttackBomb::InitEasing(float arcHeight) {
    KetaEngine::EasingParameter<float> param;
    param.type          = EasingType::OutSine;
    param.returnType    = EasingType::InSine;
    param.startValue    = 0.0f;
    param.endValue      = arcHeight;
    param.maxTime       = flightTime_ / 2.0f;
    param.returnMaxTime = flightTime_ / 2.0f;
    arcEasing_.SettingValue(param);
    arcEasing_.SetAdaptValue(&arcOffset_);
}
