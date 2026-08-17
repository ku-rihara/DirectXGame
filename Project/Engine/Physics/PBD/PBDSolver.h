#pragma once
#include "PBDDistanceConstraint.h"
#include "PBDParticle.h"
#include "PBDSphereObstacle.h"
#include <cstdint>
#include <vector>

namespace KetaEngine {

/// <summary>
/// 粒子の位置を更新するPBDソルバー
/// </summary>
class PBDSolver {
public:
    /// <summary>
    /// 粒子を追加する
    /// </summary>
    /// <param name="position">初期位置</param>
    /// <param name="invMass">質量の逆数</param>
    /// <returns>追加された粒子のインデックス</returns>
    uint32_t AddParticle(const Vector3& position, float invMass = 1.0f);

    /// <summary>
    /// 距離制約を追加する
    /// </summary>
    /// <param name="particleA">粒子Aのインデックス</param>
    /// <param name="particleB">粒子Bのインデックス</param>
    /// <param name="restLength">保ちたい距離</param>
    /// <returns>追加された制約のインデックス</returns>
    uint32_t AddDistanceConstraint(uint32_t particleA, uint32_t particleB, float restLength = -1.0f);

    /// <summary>
    /// 球状の障害物リストを設定する
    /// </summary>
    /// <param name="obstacles">障害物リスト</param>
    void SetSphereObstacles(const std::vector<PBDSphereObstacle>& obstacles) { sphereObstacles_ = obstacles; }

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="deltaTime">経過時間</param>
    void Update(float deltaTime);

    /// <summary>
    /// 粒子・制約・障害物を全てクリアする
    /// </summary>
    void Clear();

private:
    void PredictPositions(float deltaTime); //< 外力を反映した予測位置を計算
    void SolveConstraints(float deltaTime); //< 制約を反復解決する
    void SolveDistanceConstraints(float deltaTime); //< 距離制約を解決
    void SolveFloorConstraint(); //< 床制約を解決
    void SolveSphereObstacleConstraints(); //< 球障害物との衝突を解決
    void UpdateVelocityAndPosition(float deltaTime); //< 予測位置から速度・位置を確定

private:
    std::vector<PBDParticle> particles_;
    std::vector<PBDDistanceConstraint> distanceConstraints_;
    std::vector<PBDSphereObstacle> sphereObstacles_; // 毎フレーム外部からセットされる

    Vector3 gravity_        = Vector3(0.0f, -9.8f, 0.0f);
    float floorHeight_      = 0.0f;
    uint32_t solverIterations_ = 4; // 制約解決の反復回数

    // 剛性の逆数。0なら完全に硬い、大きいほど柔らかく伸び縮みする
    float compliance_ = 0.0f;
    // 速度減衰係数。0で減衰なし、1に近いほど揺れがすぐ収まる
    float damping_    = 0.0f;

public:

    // Getter
    const std::vector<PBDParticle>& GetParticles() const { return particles_; }
    const std::vector<PBDDistanceConstraint>& GetDistanceConstraints() const { return distanceConstraints_; }

    // Setter
    void SetGravity(const Vector3& gravity) { gravity_ = gravity; }
    void SetFloorHeight(float floorHeight) { floorHeight_ = floorHeight; }
    void SetSolverIterations(uint32_t iterations) { solverIterations_ = iterations; }
    void SetCompliance(float compliance) { compliance_ = compliance; }
    void SetDamping(float damping) { damping_ = damping; }
};

}; // namespace KetaEngine
