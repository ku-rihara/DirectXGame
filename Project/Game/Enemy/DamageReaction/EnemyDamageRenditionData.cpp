#include "EnemyDamageRenditionData.h"
#include <cassert>
#include <imgui.h>

void EnemyDamageRenditionData::Init(const std::string& groupName) {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();

    // グループ名を設定
    groupName_ = groupName + "_Rendition";

    folderPath_ += groupName;

    if (!globalParameter_->HasRegisters(groupName_)) {
        // 新規登録
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
    }
}

void EnemyDamageRenditionData::Reset() {
    // 必要に応じてリセット処理
}

void EnemyDamageRenditionData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void EnemyDamageRenditionData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void EnemyDamageRenditionData::RegisterParams() {
    // オブジェクトアニメーション
    globalParameter_->Regist(groupName_, "ObjAnim_FileName", &objAnimationParams_.first.fileName);
    globalParameter_->Regist(groupName_, "ObjAnim_StartTiming", &objAnimationParams_.first.startTiming);

    // パーティクルエフェクト
    globalParameter_->Regist(groupName_, "Particle_FileName", &particleEffectParams_.first.fileName);
    globalParameter_->Regist(groupName_, "Particle_StartTiming", &particleEffectParams_.first.startTiming);
}

void EnemyDamageRenditionData::AdjustParam() {
#ifdef _DEBUG
    ImGui::SeparatorText("Rendition Parameters");
    ImGui::PushID(groupName_.c_str());

    // オブジェクトアニメーションパラメータのUI
    ImGui::SeparatorText("Object Animation");
    SelectObjAnimationFile("Animation File", objAnimationParams_);
    ImGui::DragFloat("Anim Start Timing", &objAnimationParams_.first.startTiming, 0.01f);

    ImGui::Separator();

    // パーティクルエフェクトパラメータのUI
    ImGui::SeparatorText("Particle Effect");
    SelectParticleFile("Particle File", particleEffectParams_);
    ImGui::DragFloat("Particle Start Timing", &particleEffectParams_.first.startTiming, 0.01f);

    ImGui::PopID();
#endif
}

void EnemyDamageRenditionData::SelectObjAnimationFile(
    const char* label,
    std::pair<ObjAnimationParam, KetaEngine::FileSelector>& param) {
    param.second.SelectFile(label, objAnimationFolderPath_, param.first.fileName, "", true);
}

void EnemyDamageRenditionData::SelectParticleFile(
    const char* label,
    std::pair<ParticleEffectParam, KetaEngine::FileSelector>& param) {
    param.second.SelectFile(label, particleFolderPath_, param.first.fileName, "", true);
}