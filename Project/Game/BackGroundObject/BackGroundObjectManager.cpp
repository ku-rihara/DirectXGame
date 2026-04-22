#include "BackGroundObjectManager.h"
#include <imgui.h>

void BackGroundObjectManager::Init() {

    // GlobalParameter
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // コンボレベルオブジェクト
    comboLevelObjHolder_ = std::make_unique<ComboLevelObjHolder>();
    for (const auto& entry : entries_) {
        comboLevelObjHolder_->Add(entry.filename, entry.comboLevel, entry.isPulseMaster);
    }

    // 背景オブジェクト
    gameBackGroundObject_ = std::make_unique<GameBackGroundObject>();
    gameBackGroundObject_->Init("game.json");

    // ディザオクルージョン
    ditherOcclusion_ = std::make_unique<KetaEngine::DitherOcclusion>();
    ditherOcclusion_->Init();
    InitDitherOcclusion();
}

void BackGroundObjectManager::Update(float playSpeed) {
    comboLevelObjHolder_->Update(playSpeed);
    gameBackGroundObject_->Update(playSpeed);
}

void BackGroundObjectManager::RegisterParams() {
    for (auto& entry : entries_) {
        globalParameter_->Regist(groupName_, entry.filename + "_ComboLevel", &entry.comboLevel);
    }
}

void BackGroundObjectManager::InitDitherOcclusion() {
    auto* levelData = gameBackGroundObject_->GetObjectFromBlender()->GetLevelData();
    if (!levelData) {
        return;
    }

    for (auto& objData : levelData->objects) {
        if (objData.fileName == "BackObj/Field.obj") {
            continue;
        }
        ditherOcclusion_->Add(objData.object3d->GetModelMaterial());
    }
}

void BackGroundObjectManager::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    ditherOcclusion_->AdjustParam();

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        for (auto& entry : entries_) {
            ImGui::InputInt(entry.filename.c_str(), &entry.comboLevel);
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif
}
