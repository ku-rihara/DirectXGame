#include "PBDSolver.h"

using namespace KetaEngine;

uint32_t PBDSolver::AddParticle(const Vector3& position, float invMass) {
    // パラメータセット
    PBDParticle particle;
    particle.position          = position;
    particle.predictedPosition = position;
    particle.velocity          = Vector3::ZeroVector();
    particle.invMass           = invMass;

    // リストに追加してインデックスを返す
    particles_.push_back(particle);
    return static_cast<uint32_t>(particles_.size() - 1);
}

uint32_t PBDSolver::AddDistanceConstraint(uint32_t particleA, uint32_t particleB, float restLength) {
    if (restLength < 0.0f) {
        // 負の値が渡されたら、現在の2粒子間距離をそのまま初期の長さとして使う
        restLength = (particles_[particleA].position - particles_[particleB].position).Length();
    }

    // 制約を作成してリストに追加
    PBDDistanceConstraint constraint;
    constraint.particleA  = particleA;
    constraint.particleB  = particleB;
    constraint.restLength = restLength;
    
    distanceConstraints_.push_back(constraint);
    return static_cast<uint32_t>(distanceConstraints_.size() - 1);
}

void PBDSolver::Clear() {
    particles_.clear();
    distanceConstraints_.clear();
    sphereObstacles_.clear();
}

void PBDSolver::Update(float deltaTime) {
    if (deltaTime <= 0.0f) {
        return;
    }

    // 1. 外力を反映した予測位置を計算
    PredictPositions(deltaTime);
    // 2. 制約を解決
    SolveConstraints(deltaTime);
    // 3. 予測位置から速度・位置を確定
    UpdateVelocityAndPosition(deltaTime);
}

void PBDSolver::PredictPositions(float deltaTime) {

    for (PBDParticle& particle : particles_) {
        if (particle.invMass == 0.0f) {
            // 固定点は外力を受けず動かない
            particle.predictedPosition = particle.position;
            continue;
        }

        // 重力を速度に反映してから、揺れが収まっていくように減衰させる
        particle.velocity += gravity_ * deltaTime;
        particle.velocity *= (1.0f - damping_);

        // 予測位置を計算
        particle.predictedPosition = particle.position + particle.velocity * deltaTime;
    }
}

void PBDSolver::SolveConstraints(float deltaTime) {

    // constraint.lambdaを毎フレームの最初にリセットする
    for (PBDDistanceConstraint& constraint : distanceConstraints_) {
        constraint.lambda = 0.0f;
    }

    // 全ての制約を複数回まわして少しずつ収束させる
    for (uint32_t i = 0; i < solverIterations_; ++i) {
        SolveDistanceConstraints(deltaTime);
        SolveSphereObstacleConstraints();
        SolveFloorConstraint();
    }
}

void PBDSolver::SolveDistanceConstraints(float deltaTime) {
    //compliance_をタイムステップでスケーリングした値
    float alphaTilde = compliance_ / (deltaTime * deltaTime);

    
    for (PBDDistanceConstraint& constraint : distanceConstraints_) {

        // 2粒子の参照を取得
        PBDParticle& particleA = particles_[constraint.particleA];
        PBDParticle& particleB = particles_[constraint.particleB];

        // 両方の粒子が固定点なら、動かす必要はない
        float invMassSum = particleA.invMass + particleB.invMass;
        if (invMassSum == 0.0f) {
            continue; 
        }

        // 2粒子間の距離と方向を計算
        Vector3 delta   = particleA.predictedPosition - particleB.predictedPosition;
        float distance = delta.Length();
        if (distance == 0.0f) {
            continue;
        }

        // 2粒子間の距離がrestLengthからどれだけずれているかを計算
        Vector3 normal         = delta / distance;
        float constraintValue = distance - constraint.restLength; 

        // コンプライアンス込みで補正量を求める
        float deltaLambda = (-constraintValue - alphaTilde * constraint.lambda) / (invMassSum + alphaTilde);
        constraint.lambda += deltaLambda;

        // 2粒子の予測位置を補正
        Vector3 correction = normal * deltaLambda;

        // 質量の逆数に応じて補正量を分配
        particleA.predictedPosition += correction * particleA.invMass;
        particleB.predictedPosition -= correction * particleB.invMass;
    }
}

void PBDSolver::SolveSphereObstacleConstraints() {

    // 障害物に粒子がめり込んでいたら、表面まで押し出す
    for (const PBDSphereObstacle& obstacle : sphereObstacles_) {
        for (PBDParticle& particle : particles_) {
            if (particle.invMass == 0.0f) {
                continue; // 固定点は押し出さない
            }

            Vector3 diff   = particle.predictedPosition - obstacle.center;
            float distance = diff.Length();
            if (distance >= obstacle.radius || distance == 0.0f) {
                continue;
            }

            // 球の中心から粒子への方向に、半径ぶんだけ押し出す
            Vector3 normal              = diff / distance;
            particle.predictedPosition = obstacle.center + normal * obstacle.radius;
        }
    }
}

void PBDSolver::SolveFloorConstraint() {
    //  予測位置が床より下に潜ったら床の高さまで押し戻す
    for (PBDParticle& particle : particles_) {
        if (particle.predictedPosition.y < floorHeight_) {
            particle.predictedPosition.y = floorHeight_;
        }
    }
}

void PBDSolver::UpdateVelocityAndPosition(float deltaTime) {
    for (PBDParticle& particle : particles_) {
        //  位置を直接動かしてから、その移動量を deltaTime で割って速度を逆算する
        particle.velocity = (particle.predictedPosition - particle.position) / deltaTime;
        particle.position = particle.predictedPosition;
    }
}
