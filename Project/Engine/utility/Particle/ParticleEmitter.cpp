#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include <imgui.h>


ParticleEmitter::ParticleEmitter() {}

ParticleEmitter* ParticleEmitter::CreateParticle(const std::string& name, const std::string& modelFilePath, 
    const std::string& extension, const int32_t& maxnum) {

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

    ////JSON関連****************************************************************
   
}

void ParticleEmitter::AddParmGroup() {
    globalParameter_ = GlobalParameter::GetInstance();
   const char* groupName = "CollisionManager";
   // グループを追加
   globalParameter_->CreateGroup(groupName);

   globalParameter_ = GlobalParameter::GetInstance();
   const char* groupName = "CollisionManager";

   // グループを追加
   globalParameter_->CreateGroup(groupName);

   // Position ツリーノード
   globalParameter_->AddTreeNode("Position");
   globalParameter_->AddItem(groupName, "BasePos", basePos_, GlobalParameter::DrawSettings::WidgetType::DragFloat3);
   globalParameter_->AddItem(groupName, "Position Max", positionDist_.max, GlobalParameter::DrawSettings::WidgetType::DragFloat3);
   globalParameter_->AddItem(groupName, "Position Min", positionDist_.min, GlobalParameter::DrawSettings::WidgetType::DragFloat3);
   globalParameter_->AddTreePoP();

   // Scale ツリーノード
   globalParameter_->AddTreeNode("Scale");
   globalParameter_->AddItem(groupName, "Scale Max", scaleDist_.max, GlobalParameter::DrawSettings::WidgetType::DragFloat3);
   globalParameter_->AddItem(groupName, "Scale Min", scaleDist_.min, GlobalParameter::DrawSettings::WidgetType::DragFloat3);
   globalParameter_->AddTreePoP();

   // Velocity ツリーノード
   globalParameter_->AddTreeNode("Velocity");
   globalParameter_->AddItem(groupName, "Velocity Max", velocityDist_.max, GlobalParameter::DrawSettings::WidgetType::DragFloat3);
   globalParameter_->AddItem(groupName, "Velocity Min", velocityDist_.min, GlobalParameter::DrawSettings::WidgetType::DragFloat3);
   globalParameter_->AddTreePoP();

   // Color ツリーノード
   globalParameter_->AddTreeNode("Color");
   globalParameter_->AddItem(groupName, "BaseColor", baseColor_, GlobalParameter::DrawSettings::WidgetType::ColorEdit4);
   globalParameter_->AddItem(groupName, "Color Max", colorDist_.max, GlobalParameter::DrawSettings::WidgetType::ColorEdit4);
   globalParameter_->AddItem(groupName, "Color Min", colorDist_.min, GlobalParameter::DrawSettings::WidgetType::ColorEdit4);
   globalParameter_->AddTreePoP();

   // その他のパラメータ
   globalParameter_->AddItem(groupName, "LifeTime", lifeTime_, GlobalParameter::DrawSettings::WidgetType::DragFloat);
   globalParameter_->AddItem(groupName, "Particle Count", particleCount_, GlobalParameter::DrawSettings::WidgetType::SliderInt);


}

void ParticleEmitter::Emit() {
    ParticleManager::GetInstance()->Emit(
        particleName_, basePos_, positionDist_, scaleDist_,
        velocityDist_, baseColor_, colorDist_, lifeTime_, particleCount_);
}

void ParticleEmitter::EditorUpdate() {
   /* if (ImGui::Begin("Particle Emitter Editor")) {
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
    ImGui::End();*/
}

