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

        if (ImGui::Button("Save")) {
            editor_.Save(dyrectryPath + particleName_ + ".json", [this]() { return Serialize(); });
        }
        if (ImGui::Button("Load")) {
            editor_.Load(dyrectryPath + particleName_ + ".json", [this](const json& data) { Deserialize(data); });
        }
    }
    ImGui::End();
}

ParticleEmitter::json ParticleEmitter::Serialize() const {
    json data;
    data["basePos"] = { basePos_.x, basePos_.y, basePos_.z };
    data["positionDist"]["min"] = { positionDist_.min.x, positionDist_.min.y, positionDist_.min.z };
    data["positionDist"]["max"] = { positionDist_.max.x, positionDist_.max.y, positionDist_.max.z };
    data["scaleDist"]["min"] = { scaleDist_.min.x, scaleDist_.min.y, scaleDist_.min.z };
    data["scaleDist"]["max"] = { scaleDist_.max.x, scaleDist_.max.y, scaleDist_.max.z };
    data["velocityDist"]["min"] = { velocityDist_.min.x, velocityDist_.min.y, velocityDist_.min.z };
    data["velocityDist"]["max"] = { velocityDist_.max.x, velocityDist_.max.y, velocityDist_.max.z };
    data["baseColor"] = { baseColor_.x, baseColor_.y, baseColor_.z, baseColor_.w };
    data["colorDist"]["min"] = { colorDist_.min.x, colorDist_.min.y, colorDist_.min.z, colorDist_.min.w };
    data["colorDist"]["max"] = { colorDist_.max.x, colorDist_.max.y, colorDist_.max.z, colorDist_.max.w };
    data["lifeTime"] = lifeTime_;
    data["particleCount"] = particleCount_;
    return data;
}

void ParticleEmitter::Deserialize(const json& data) {
    basePos_ = { data["basePos"][0], data["basePos"][1], data["basePos"][2] };
    positionDist_.min = { data["positionDist"]["min"][0], data["positionDist"]["min"][1], data["positionDist"]["min"][2] };
    positionDist_.max = { data["positionDist"]["max"][0], data["positionDist"]["max"][1], data["positionDist"]["max"][2] };
    scaleDist_.min = { data["scaleDist"]["min"][0], data["scaleDist"]["min"][1], data["scaleDist"]["min"][2] };
    scaleDist_.max = { data["scaleDist"]["max"][0], data["scaleDist"]["max"][1], data["scaleDist"]["max"][2] };
    velocityDist_.min = { data["velocityDist"]["min"][0], data["velocityDist"]["min"][1], data["velocityDist"]["min"][2] };
    velocityDist_.max = { data["velocityDist"]["max"][0], data["velocityDist"]["max"][1], data["velocityDist"]["max"][2] };
    baseColor_ = { data["baseColor"][0], data["baseColor"][1], data["baseColor"][2], data["baseColor"][3] };
    colorDist_.min = { data["colorDist"]["min"][0], data["colorDist"]["min"][1], data["colorDist"]["min"][2], data["colorDist"]["min"][3] };
    colorDist_.max = { data["colorDist"]["max"][0], data["colorDist"]["max"][1], data["colorDist"]["max"][2], data["colorDist"]["max"][3] };
    lifeTime_ = data["lifeTime"];
    particleCount_ = data["particleCount"];
}
