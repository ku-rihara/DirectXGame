#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include <imgui.h>


ParticleEmitter::ParticleEmitter() {}

ParticleEmitter* ParticleEmitter::CreateParticle(const std::string& name, const std::string& modelFilePath, const std::string& extension, const int32_t& maxnum) {
    auto emitter = std::make_unique<ParticleEmitter>();
    emitter->particleName_ = name;
    ParticleManager::GetInstance()->CreateParticleGroup(emitter->particleName_, modelFilePath, extension, maxnum);
    emitter->Init();
    return emitter.release();
}

void ParticleEmitter::Init() {
    particleCount_ = 0;
    lifeTime_ = 0.0f;
    baseColor_ = { 0, 0, 0, 0 };
    colorDist_.min = { 0, 0, 0, 0 };
    colorDist_.max = { 0, 0, 0, 0 };
}

void ParticleEmitter::Emit() {
    ParticleManager::GetInstance()->Emit(
        particleName_, basePos_, positionDist_, scaleDist_,
        velocityDist_, baseColor_, colorDist_, lifeTime_, particleCount_);
}

void ParticleEmitter::EditorUpdate() {
    if (ImGui::Begin("Particle Emitter Editor")) {
        ImGui::DragFloat3("Base Position", &basePos_.x, 0.1f);
        ImGui::DragFloat3("Position  Min", &positionDist_.min.x, 0.1f);
        ImGui::DragFloat3("Position  Max", &positionDist_.max.x, 0.1f);
        ImGui::DragFloat3("Scale  Min", &scaleDist_.min.x, 0.1f);
        ImGui::DragFloat3("Scale  Max", &scaleDist_.max.x, 0.1f);
        ImGui::DragFloat3("Velocity  Min", &velocityDist_.min.x, 0.1f);
        ImGui::DragFloat3("Velocity  Max", &velocityDist_.max.x, 0.1f);
        ImGui::ColorEdit4("Base Color", &baseColor_.x);
        ImGui::ColorEdit4("Color  Min", &colorDist_.min.x);
        ImGui::ColorEdit4("Color  Max", &colorDist_.max.x);
        ImGui::DragFloat("Lifetime", &lifeTime_, 0.1f);
        ImGui::DragInt("Particle Count", &particleCount_, 1, 1, 100);


    }
    ImGui::End();
}

