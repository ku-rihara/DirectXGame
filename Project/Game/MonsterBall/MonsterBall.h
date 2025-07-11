#pragma once
#include "3d/Object3d.h"
#include "3d/WorldTransform.h"
#include "Animation/ModelAnimation.h"
#include "Easing.h"
#include "utility/ParticleEditor/ParticleEmitter.h"
class MonsterBall {
public:
private:
    // ワールド変換データ
    WorldTransform transform_;
    std::unique_ptr<Object3d> obj3D_;

    // モデル
    std::unique_ptr<ModelAnimation> modelAnimation_  = nullptr;
    std::unique_ptr<ModelAnimation> modelAnimation2_ = nullptr;
    Easing<Vector3> easing_;
    std::unique_ptr<ParticleEmitter> emitter_;

    ViewProjection* viewProjection_ = nullptr;
    std::unique_ptr<ParticleParameter> parameters_;

    Vector3 direction_;
    float objectiveAngle_ = 0.0f;
    float speed_;

public:
    MonsterBall();
    ~MonsterBall();
    void Init();
    void Update();
    void Draw(ViewProjection& viewProjection);

    void Debug();

    void ScaleEasing();

    void Move(const float& speed);
    void Jump(float& speed, const float& fallSpeedLimit, const float& gravity);
    void Fall(float& speed, const float& fallSpeedLimit, const float& gravity, const bool& isJump);
    void MoveToLimit();
    bool GetIsMoving();

private:
    Vector3 GetInputDirection();
    void FaceToTarget();
    void AdaptRotate();
};
