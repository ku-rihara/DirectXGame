#include "BaseEffectEditor.h"

using namespace KetaEngine;
// editor
#include "Editor/CameraEditor/CameraAnimationData.h"
#include "Editor/DissolveEditor/DissolveData.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationData.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/RailData.h"
#include "Editor/ShakeEditor/ShakeData.h"
#include "Editor/TimeScaleEditor/TimeScaleData.h"
#include "Particle/CPUParticle/Editor/ParticleData.h"
#include "Particle/GPUParticle/Editor/GPUParticleData.h"
// std
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <Windows.h>

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::Init(const std::string& typeName) {
    // 「Resources/GlobalParameter/ファイル名」までの
    effecrtFolderName_ = GetFolderName();
    effectTypeName_ = typeName;

    AllLoadFile();

    // デフォルトカテゴリーが存在しない場合は作成
    if (categories_.empty()) {
        AddCategory("Common");
    }

    isEditing_ = false;
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::Update(float speedRate) {
    // 全カテゴリーの全エフェクトを更新
    for (auto& category : categories_) {
        for (auto& effect : category.effects) {
            effect->Update(speedRate);
        }
    }
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader((effectTypeName_ + " Editor").c_str())) {
        isEditing_ = true;

        ImGui::PushID(effectTypeName_.c_str());

        // 派生クラス固有のUI
        RenderSpecificUI();

        ImGui::Separator();

        // カテゴリーモードUI
        RenderCategoryUI();

        ImGui::Separator();

        // ファイル操作
        RenderFileOperations();

        ImGui::PopID();
    } else {
        isEditing_ = false;
    }
#endif
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::RenderCategoryUI() {
    ImGui::SeparatorText("Category Management");

    // 新規カテゴリー追加
    ImGui::InputText("New Category Name", categoryNameBuffer_, IM_ARRAYSIZE(categoryNameBuffer_));
    if (ImGui::Button("Add Category")) {
        if (strlen(categoryNameBuffer_) > 0) {
            AddCategory(categoryNameBuffer_);
            categoryNameBuffer_[0] = '\0';
        }
    }

    ImGui::Separator();

    // カテゴリーリスト表示
    ImGui::Text("Categories (%d):", static_cast<int>(categories_.size()));
    for (int i = 0; i < static_cast<int>(categories_.size()); i++) {
        ImGui::PushID(i);

        bool isSelected       = (selectedCategoryIndex_ == i);
        std::string labelText = categories_[i].name + " (" + std::to_string(categories_[i].effects.size()) + " effects)";

        if (ImGui::Selectable(labelText.c_str(), isSelected)) {
            selectedCategoryIndex_ = i;
        }

        ImGui::PopID();
    }

    ImGui::Separator();

    // 選択されたカテゴリーの編集
    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        RenderCategoryEffectListUI();
    }
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::RenderCategoryEffectListUI() {
    auto& selectedCategory = categories_[selectedCategoryIndex_];

    ImGui::SeparatorText(("Category: " + selectedCategory.name).c_str());

    // エフェクト追加
    ImGui::InputText("New Effect Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
    if (ImGui::Button("Add Effect")) {
        if (strlen(nameBuffer_) > 0) {
            AddEffectToCategory(selectedCategoryIndex_, nameBuffer_);
            nameBuffer_[0] = '\0';
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete Selected Effect")) {
        if (selectedCategory.selectedEffectIndex >= 0 && selectedCategory.selectedEffectIndex < static_cast<int>(selectedCategory.effects.size())) {
            RemoveEffectFromCategory(selectedCategoryIndex_, selectedCategory.selectedEffectIndex);
        }
    }

    ImGui::Separator();

    // エフェクトリスト表示
    ImGui::Text("Effects (%d):", static_cast<int>(selectedCategory.effects.size()));
    for (int j = 0; j < static_cast<int>(selectedCategory.effects.size()); j++) {
        ImGui::PushID(j);

        bool isEffectSelected       = (selectedCategory.selectedEffectIndex == j);
        std::string effectLabelText = selectedCategory.effects[j]->GetGroupName();

        if (selectedCategory.effects[j]->IsPlaying()) {
            effectLabelText += " [PLAYING]";
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        }

        if (ImGui::Selectable(effectLabelText.c_str(), isEffectSelected)) {
            selectedCategory.selectedEffectIndex = j;
        }

        if (selectedCategory.effects[j]->IsPlaying()) {
            ImGui::PopStyleColor();
        }

        ImGui::PopID();
    }

    ImGui::Separator();

    // 選択されたエフェクトの編集
    if (selectedCategory.selectedEffectIndex >= 0 && selectedCategory.selectedEffectIndex < static_cast<int>(selectedCategory.effects.size())) {
        selectedCategory.effects[selectedCategory.selectedEffectIndex]->AdjustParam();
    }
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::RenderFileOperations() {
    ImGui::Text("File Operations:");

    // カテゴリーモードのファイル操作
    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        auto& selectedCategory = categories_[selectedCategoryIndex_];

        if (selectedCategory.selectedEffectIndex >= 0 && selectedCategory.selectedEffectIndex < static_cast<int>(selectedCategory.effects.size())) {

            ImGui::SeparatorText("Selected Effect File Operations");

            auto* selectedEffect   = selectedCategory.effects[selectedCategory.selectedEffectIndex].get();
            std::string effectName = selectedEffect->GetGroupName();

            // 個別Load
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
            if (ImGui::Button(("Load " + effectName).c_str())) {
                selectedEffect->LoadData();
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();

            // 個別Save
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
            if (ImGui::Button(("Save " + effectName).c_str())) {
                selectedEffect->SaveData();
                MessageBoxA(nullptr, (effectName + " saved successfully.").c_str(),
                    "Effect Editor", 0);
            }
            ImGui::PopStyleColor(3);
        }

        ImGui::SeparatorText("Category File Operations");

        // カテゴリー単位のSave
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
        if (ImGui::Button(("Save Category: " + selectedCategory.name).c_str())) {
            SaveCategory(selectedCategoryIndex_);
            MessageBoxA(nullptr,
                ("Category '" + selectedCategory.name + "' saved successfully.").c_str(),
                "Effect Editor", 0);
        }
        ImGui::PopStyleColor(3);
    }

    ImGui::SeparatorText("All File Operations");

    // Load All ボタン
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
    if (ImGui::Button("Load All Effects")) {
        AllLoadFile();
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();

    // Save All ボタン
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
    if (ImGui::Button("Save All Effects")) {
        AllSaveFile();
        MessageBoxA(nullptr, "All effects saved successfully.", "Effect Editor", 0);
    }
    ImGui::PopStyleColor(3);
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::RenderPlayBack() {
    ImGui::Separator();

    if (ImGui::Button("Play Selected")) {
        PlaySelectedAnimation();
    }
    ImGui::SameLine();

    if (ImGui::Button("Pause")) {
        PauseSelectedAnimation();
    }
    ImGui::SameLine();

    if (ImGui::Button("Reset")) {
        ResetSelectedAnimation();
    }

    // 状態表示
    ImGui::Spacing();
    if (IsSelectedAnimationPlaying()) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Playing");
    } else if (IsSelectedAnimationFinished()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Finished");
        ResetSelectedAnimation();
    } else {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Stopped");
    }
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::AddCategory(const std::string& categoryName) {
    auto it = std::find_if(categories_.begin(), categories_.end(),
        [&categoryName](const Category& cat) {
            return cat.name == categoryName;
        });

    if (it != categories_.end()) {
        return;
    }

    Category newCategory;
    newCategory.name = categoryName;
    categories_.push_back(std::move(newCategory));
    selectedCategoryIndex_ = static_cast<int32_t>(categories_.size()) - 1;

    // フォルダ作成
    std::string folderPath = GlobalParameter::GetInstance()->GetDirectoryPath() + effecrtFolderName_ + categoryName;
    std::filesystem::create_directories(folderPath);
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::RemoveCategory(int32_t index) {
    if (index >= 0 && index < static_cast<int>(categories_.size())) {
        categories_.erase(categories_.begin() + index);

        if (selectedCategoryIndex_ >= index) {
            selectedCategoryIndex_--;
            if (selectedCategoryIndex_ < 0 && !categories_.empty()) {
                selectedCategoryIndex_ = 0;
            } else if (categories_.empty()) {
                selectedCategoryIndex_ = -1;
            }
        }
    }
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::AddEffectToCategory(int32_t categoryIndex, const std::string& effectName) {
    if (categoryIndex < 0 || categoryIndex >= static_cast<int>(categories_.size())) {
        return;
    }

    auto& category = categories_[categoryIndex];

    auto it = std::find_if(category.effects.begin(), category.effects.end(),
        [&effectName](const std::unique_ptr<TEffectData>& effect) {
            return effect->GetGroupName() == effectName;
        });

    if (it != category.effects.end()) {
        return;
    }

    auto effect = CreateEffectData();
    effect->Init(effectName, category.name);

    category.effects.push_back(std::move(effect));
    category.selectedEffectIndex = static_cast<int32_t>(category.effects.size()) - 1;
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::RemoveEffectFromCategory(int32_t categoryIndex, int32_t effectIndex) {
    if (categoryIndex < 0 || categoryIndex >= static_cast<int>(categories_.size())) {
        return;
    }

    auto& category = categories_[categoryIndex];

    if (effectIndex >= 0 && effectIndex < static_cast<int>(category.effects.size())) {
        category.effects.erase(category.effects.begin() + effectIndex);

        if (category.selectedEffectIndex >= effectIndex) {
            category.selectedEffectIndex--;
            if (category.selectedEffectIndex < 0 && !category.effects.empty()) {
                category.selectedEffectIndex = 0;
            } else if (category.effects.empty()) {
                category.selectedEffectIndex = -1;
            }
        }
    }
}

template <typename TEffectData>
TEffectData* BaseEffectEditor<TEffectData>::GetEffectByName(const std::string& categoryName, const std::string& effectName) {
    auto catIt = std::find_if(categories_.begin(), categories_.end(),
        [&categoryName](const Category& cat) {
            return cat.name == categoryName;
        });

    if (catIt == categories_.end()) {
        return nullptr;
    }

    auto effectIt = std::find_if(catIt->effects.begin(), catIt->effects.end(),
        [&effectName](const std::unique_ptr<TEffectData>& effect) {
            return effect->GetGroupName() == effectName;
        });

    if (effectIt != catIt->effects.end()) {
        return effectIt->get();
    }

    return nullptr;
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::AllLoadFile() {
  
    std::string basePath = GlobalParameter::GetInstance()->GetDirectoryPath() + effecrtFolderName_;

    if (!std::filesystem::exists(basePath) || !std::filesystem::is_directory(basePath)) {
        std::filesystem::create_directories(basePath);
    }

    categories_.clear();
    selectedCategoryIndex_ = -1;

    // カテゴリーフォルダを検索
    for (const auto& categoryEntry : std::filesystem::directory_iterator(basePath)) {
        if (categoryEntry.is_directory()) {
            std::string categoryName = categoryEntry.path().filename().string();
            LoadCategory(categoryName);
        }
    }

    // カテゴリーが1つもない場合はデフォルトカテゴリーを作成
    if (categories_.empty()) {
        AddCategory("Common");
    }
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::AllSaveFile() {
    // カテゴリーモードのセーブ
    for (int32_t i = 0; i < static_cast<int32_t>(categories_.size()); i++) {
        SaveCategory(i);
    }
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::SaveCategory(int32_t categoryIndex) {
    if (categoryIndex < 0 || categoryIndex >= static_cast<int>(categories_.size())) {
        return;
    }

    auto& category = categories_[categoryIndex];

    for (auto& effect : category.effects) {
        effect->SaveData();
    }
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::LoadCategory(const std::string& categoryName) {
   const std::string& categoryPath = GlobalParameter::GetInstance()->GetDirectoryPath() + effecrtFolderName_ + categoryName + "/" + datesFolderName_;

    if (!std::filesystem::exists(categoryPath) || !std::filesystem::is_directory(categoryPath)) {
        return;
    }

    Category newCategory;
    newCategory.name = categoryName;

    for (const auto& entry : std::filesystem::directory_iterator(categoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            std::string effectName = entry.path().stem().string();

            auto effect = CreateEffectData();
            effect->Init(effectName, categoryName);
            effect->LoadData();
            newCategory.effects.push_back(std::move(effect));
        }
    }

    categories_.push_back(std::move(newCategory));
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::PauseSelectedAnimation() {
    auto* selectedAnim = GetSelectedEffect();
    if (selectedAnim) {
        selectedAnim->Pause();
    }
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::PlaySelectedAnimation() {
    auto* selectedAnim = GetSelectedEffect();
    if (selectedAnim) {
        selectedAnim->Play();
    }
}

template <typename TEffectData>
void BaseEffectEditor<TEffectData>::ResetSelectedAnimation() {
    auto* selectedAnim = GetSelectedEffect();
    if (selectedAnim) {
        selectedAnim->Reset();
    }
}

template <typename TEffectData>
bool BaseEffectEditor<TEffectData>::IsSelectedAnimationPlaying() const {
    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        auto& category = categories_[selectedCategoryIndex_];
        if (category.selectedEffectIndex >= 0 && category.selectedEffectIndex < static_cast<int>(category.effects.size())) {
            return category.effects[category.selectedEffectIndex]->IsPlaying();
        }
    }
    return false;
}

template <typename TEffectData>
bool BaseEffectEditor<TEffectData>::IsSelectedAnimationFinished() const {
    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        auto& category = categories_[selectedCategoryIndex_];
        if (category.selectedEffectIndex >= 0 && category.selectedEffectIndex < static_cast<int>(category.effects.size())) {
            return category.effects[category.selectedEffectIndex]->IsFinished();
        }
    }
    return false;
}

template <typename TEffectData>
TEffectData* BaseEffectEditor<TEffectData>::GetSelectedEffect() {
    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        auto& category = categories_[selectedCategoryIndex_];
        if (category.selectedEffectIndex >= 0 && category.selectedEffectIndex < static_cast<int>(category.effects.size())) {
            return category.effects[category.selectedEffectIndex].get();
        }
    }
    return nullptr;
}

template class BaseEffectEditor<CameraAnimationData>;
template class BaseEffectEditor<ObjEaseAnimationData>;
template class BaseEffectEditor<RailData>;
template class BaseEffectEditor<ShakeData>;
template class BaseEffectEditor<TimeScaleData>;
template class BaseEffectEditor<GPUParticleData>;
template class BaseEffectEditor<ParticleData>;
template class BaseEffectEditor<DissolveData>;