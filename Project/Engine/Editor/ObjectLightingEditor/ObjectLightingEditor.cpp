#include "ObjectLightingEditor.h"

using namespace KetaEngine;
#include "3D/Model.h"
#include "3D/Object3D/BaseObject3d.h"
#include <imgui.h>

ObjectLightingEditor* ObjectLightingEditor::GetInstance() {
    static ObjectLightingEditor instance;
    return &instance;
}

std::string ObjectLightingEditor::MakeGroupName(const std::string& modelName) {
    std::string sanitized = modelName;
    for (char& c : sanitized) {
        if (c == '/' || c == '\\' || c == '.') {
            c = '_';
        }
    }
    return "ObjLighting_" + sanitized;
}

std::string ObjectLightingEditor::MakeSceneFolder() const {
    return "Lighting/" + (currentSceneName_.empty() ? "Common" : currentSceneName_);
}

ObjectLightingEditor::Settings& ObjectLightingEditor::EnsureRegistered(const std::string& modelName, BaseObject3d* seedSource) {
    if (!globalParameter_) {
        globalParameter_ = GlobalParameter::GetInstance();
    }

    const std::string groupName = MakeGroupName(modelName);
    auto it                     = settings_.find(modelName);

    if (it == settings_.end()) {
        // 初回登録: 現在のマテリアル値をシードにする
        Settings seeded;
        if (seedSource != nullptr) {
            seeded.enableLighting        = seedSource->GetModelMaterial()->GetMaterialData()->enableLighting;
            seeded.environmentCoefficient = seedSource->GetModelMaterial()->GetMaterialData()->environmentCoefficient;
        }
        it = settings_.emplace(modelName, seeded).first;
    }

    // シーン遷移でGlobalParameterの登録がクリアされている場合は再登録する
    if (!globalParameter_->HasRegisters(groupName)) {
        globalParameter_->CreateGroup(groupName);

        // 現在のシーン用フォルダに保存済みのファイルがあれば読み込む
        globalParameter_->LoadFile(groupName, MakeSceneFolder());

        globalParameter_->Regist(groupName, "EnableLighting", &it->second.enableLighting);
        globalParameter_->Regist(groupName, "EnvironmentCoefficient", &it->second.environmentCoefficient);
        globalParameter_->SyncParamForGroup(groupName);
    }

    return it->second;
}

void ObjectLightingEditor::SyncObject(BaseObject3d* obj) {
    if (obj == nullptr || obj->GetModel() == nullptr || obj->GetModelMaterial() == nullptr) {
        return;
    }

    const std::string& modelName = obj->GetModel()->GetModelName();
    Settings& settings            = EnsureRegistered(modelName, obj);

    obj->GetModelMaterial()->GetMaterialData()->enableLighting        = settings.enableLighting;
    obj->GetModelMaterial()->GetMaterialData()->environmentCoefficient = settings.environmentCoefficient;
}

void ObjectLightingEditor::DebugImGui() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    ImGui::Text("Registered Models: %zu", settings_.size());
    ImGui::Separator();

    ImGui::BeginChild("ObjectLightingModelList", ImVec2(0, 120), true);
    for (auto& [modelName, settings] : settings_) {
        bool isSelected = (modelName == selectedModelName_);
        if (ImGui::Selectable(modelName.c_str(), isSelected)) {
            selectedModelName_ = modelName;
        }
    }
    ImGui::EndChild();

    auto it = settings_.find(selectedModelName_);
    if (it == settings_.end()) {
        return;
    }

    Settings& settings          = it->second;
    const std::string groupName = MakeGroupName(selectedModelName_);

    ImGui::PushID(groupName.c_str());
    ImGui::Text("%s", selectedModelName_.c_str());

    const char* lightingModes[] = {"No Lighting", "Lambert", "Half Lambert", "Specular Reflection",
        "PointLight", "SpotLight", "AreaLight", "Ambient"};
    ImGui::Combo("Lighting Mode", &settings.enableLighting, lightingModes, IM_ARRAYSIZE(lightingModes));
    ImGui::DragFloat("Environment Coefficient", &settings.environmentCoefficient, 0.01f);

    globalParameter_->ParamSaveForImGui(groupName, MakeSceneFolder());
    globalParameter_->ParamLoadForImGui(groupName, MakeSceneFolder());

    ImGui::PopID();
#endif // _DEBUG
}

void ObjectLightingEditor::OnSceneChanged(const std::string& sceneName) {
    settings_.clear();
    selectedModelName_.clear();
    currentSceneName_ = sceneName;
}
