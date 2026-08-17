#include "PBDRopeTest.h"

#include "Frame/Frame.h"
#include "Player/Player.h"
#include <imgui.h>

void PBDRopeTest::Init() {
    solver_.SetGravity(gravity_);
    solver_.SetSolverIterations(solverIterations_);
    solver_.SetCompliance(compliance_);
    solver_.SetDamping(damping_);
    solver_.SetFloorHeight(0.0f); 

    BuildRope();
}

void PBDRopeTest::BuildRope() {
    solver_.Clear();
    beadObjects_.clear(); 

    // 両端固定でparticleCount_個の粒子を直線状に並べる
    for (uint32_t i = 0; i < particleCount_; ++i) {
        float t           = static_cast<float>(i) / static_cast<float>(particleCount_ - 1);
        Vector3 position = startPos_ + (endPos_ - startPos_) * t;

        // 先頭と末尾だけ固定点(invMass=0)にする
        bool isFixed  = (i == 0 || i == particleCount_ - 1);
        float invMass = isFixed ? 0.0f : 1.0f;
        solver_.AddParticle(position, invMass);
    }

    // 隣り合う粒子同士を距離制約でつなぐ
    for (uint32_t i = 0; i < particleCount_ - 1; ++i) {
        solver_.AddDistanceConstraint(i, i + 1);
    }

    // 粒子と同じ数だけ球モデルを生成し、粒子位置に追従させる
    beadObjects_.reserve(particleCount_);
    for (uint32_t i = 0; i < particleCount_; ++i) {
        std::unique_ptr<KetaEngine::Object3d> bead;
        bead.reset(KetaEngine::Object3d::CreateModel("TestObj/DebugSphere.obj"));
        bead->transform_.scale_ = Vector3(beadScale_, beadScale_, beadScale_);
        beadObjects_.push_back(std::move(bead));
    }
}

void PBDRopeTest::Update() {
    // プレイヤーのAABBを包む球を押し出し用の障害物として渡す
    if (player_ != nullptr) {
        const AABB& aabb   = player_->GetAABB();
        Vector3 halfExtent = (aabb.max - aabb.min) * 0.5f;

        KetaEngine::PBDSphereObstacle obstacle;
        obstacle.center = player_->GetCollisionPos();
        obstacle.radius = halfExtent.Length(); // AABBを内包する球の半径

        solver_.SetSphereObstacles({obstacle});
    }

    solver_.Update(KetaEngine::Frame::DeltaTime());

    // ソルバーが計算した粒子位置を、それぞれ対応する球モデルへ反映する
    const std::vector<KetaEngine::PBDParticle>& particles = solver_.GetParticles();
    for (uint32_t i = 0; i < particles.size(); ++i) {
        beadObjects_[i]->transform_.translation_ = particles[i].position;
    }
}

void PBDRopeTest::DebugImGui() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::TreeNode("PBDRopeTest")) {
        ImGui::Text("particles: %d", static_cast<int>(beadObjects_.size()));

        // ロープの基準位置・粒子数
        bool needRebuild = false;

        ImGui::DragFloat3("startPos", &startPos_.x, 0.1f);
        needRebuild |= ImGui::IsItemDeactivatedAfterEdit();

        ImGui::DragFloat3("endPos", &endPos_.x, 0.1f);
        needRebuild |= ImGui::IsItemDeactivatedAfterEdit();

        int particleCount = static_cast<int>(particleCount_);
        ImGui::DragInt("particleCount", &particleCount, 1, 2, 64);
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            particleCount_ = static_cast<uint32_t>(particleCount);
            needRebuild    = true;
        }

        if (needRebuild) {
            BuildRope();
        }

        if (ImGui::DragFloat3("gravity", &gravity_.x, 0.1f)) {
            solver_.SetGravity(gravity_);
        }

        int iterations = static_cast<int>(solverIterations_);
        if (ImGui::DragInt("solverIterations", &iterations, 1, 1, 20)) {
            solverIterations_ = static_cast<uint32_t>(iterations);
            solver_.SetSolverIterations(solverIterations_);
        }

        // compliance: 0=完全に硬い、大きいほど伸び縮みする
        if (ImGui::DragFloat("compliance", &compliance_, 0.00005f, 0.0f, 0.01f, "%.5f")) {
            solver_.SetCompliance(compliance_);
        }

        // damping: 0=減衰なし、大きいほど揺れがすぐ収まる
        if (ImGui::DragFloat("damping", &damping_, 0.005f, 0.0f, 0.5f)) {
            solver_.SetDamping(damping_);
        }

        ImGui::TreePop();
    }
#endif
}
