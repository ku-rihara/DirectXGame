#pragma once
#include "3d/Object3d.h"
#include "3d/WorldTransform.h"
#include "Animation/Object3DAnimation.h"
#include "Easing/Easing.h"
#include "Editor/ParticleEditor/ParticleEmitter.h"
class MonsterBall {
public:
private:
    // ワールド変換データ
    WorldTransform transform_;
    std::unique_ptr<Object3d> obj3D_;

    // モデル
    std::unique_ptr<Object3DAnimation> modelAnimation_  = nullptr;
    std::unique_ptr<Object3DAnimation> modelAnimation2_ = nullptr;
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

};
