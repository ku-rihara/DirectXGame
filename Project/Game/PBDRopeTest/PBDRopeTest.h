#pragma once
#include "3D/Object3d/Object3d.h"
#include "Physics/PBD/PBDSolver.h"
#include "Vector3.h"
#include <cstdint>
#include <memory>
#include <vector>

class Player;

/// <summary>
/// PBDロープの簡易テストオブジェクト
/// 両端を固定した粒子列を距離制約でつなぎ、各粒子にDebugSphereを追従させる
/// (テスト用に)プレイヤーの当たり判定ともぶつかる
/// </summary>
class PBDRopeTest {
public:
    PBDRopeTest()  = default;
    ~PBDRopeTest() = default;

    void Init();
    void Update();
    void DebugImGui();

    void SetPlayer(Player* player) { player_ = player; }

private:
    /// <summary>
    /// 現在のパラメータ(startPos_/endPos_/particleCount_)でロープを作り直す
    /// </summary>
    void BuildRope();

private:
    Player* player_ = nullptr;

    KetaEngine::PBDSolver solver_;
    std::vector<std::unique_ptr<KetaEngine::Object3d>> beadObjects_;

    // ロープの初期配置
    Vector3 startPos_       = Vector3(-5.0f, 5.0f, 0.0f);
    Vector3 endPos_         = Vector3(5.0f, 5.0f, 0.0f);
    uint32_t particleCount_ = 10;
    float beadScale_        = 0.3f;

    // ImGui編集用(ソルバー側の値のミラー)
    Vector3 gravity_           = Vector3(0.0f, -3.0f, 0.0f);
    uint32_t solverIterations_ = 4;
    float compliance_          = 0.0002f; // 0で完全に硬い。大きいほど伸び縮みする(柔らかい)
    float damping_             = 0.02f;   // 0で減衰なし。大きいほど揺れがすぐ収まる
};
