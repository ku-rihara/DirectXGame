#include "ObjEaseAnimationEditor.h"
#include "Function/GetFile.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <Windows.h>

void ObjEaseAnimationEditor::Init() {
    AllLoadFile();
    InitPreviewObject();
}

void ObjEaseAnimationEditor::InitPreviewObject() {

    // デフォルトモデルでプレビューオブジェクトを作成
    previewObject_.reset(Object3d::CreateModel("DebugCube.obj"));
   
    if (previewObject_) {
        previewObject_->SetIsAutoUpdate(false);
        previewObject_->transform_.scale_       = previewBaseTransform_.scale;
        previewObject_->transform_.rotation_    = previewBaseTransform_.rotation;
        previewObject_->transform_.translation_ = previewBaseTransform_.translation;
    }
}

void ObjEaseAnimationEditor::AllLoadFile() {
    std::string baseFolderPath = "Resources/GlobalParameter/ObjEaseAnimation/";

    if (!std::filesystem::exists(baseFolderPath) || !std::filesystem::is_directory(baseFolderPath)) {
        return;
    }

    // 既存のカテゴリーをクリア
    categories_.clear();
    selectedCategoryIndex_ = -1;

    // カテゴリーフォルダを検索
    for (const auto& categoryEntry : std::filesystem::directory_iterator(baseFolderPath)) {
        if (categoryEntry.is_directory()) {
            std::string categoryName = categoryEntry.path().filename().string();
            LoadCategory(categoryName);
        }
    }
}

void ObjEaseAnimationEditor::LoadCategory(const std::string& categoryName) {
    std::string categoryPath = "Resources/GlobalParameter/ObjEaseAnimation/" + categoryName;

    if (!std::filesystem::exists(categoryPath) || !std::filesystem::is_directory(categoryPath)) {
        return;
    }

    Category newCategory;
    newCategory.name = categoryName;

    // カテゴリー内のアニメーションファイルを検索
    for (const auto& entry : std::filesystem::directory_iterator(categoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            std::string animationName = entry.path().stem().string();

            // アニメーションデータを作成してロード
            auto animation = std::make_unique<ObjEaseAnimationData>();
            animation->Init(animationName, categoryName);
            animation->LoadData();
            newCategory.animations.push_back(std::move(animation));
        }
    }

    categories_.push_back(std::move(newCategory));
}

void ObjEaseAnimationEditor::Update(const float& deltaTime) {
    // すべてのアニメーションを更新
    for (auto& category : categories_) {
        for (auto& animation : category.animations) {
            animation->Update(deltaTime);
        }
    }

    // プレビューオブジェクトを更新
    UpdatePreviewObject();
}

void ObjEaseAnimationEditor::UpdatePreviewObject() {
    if (!previewObject_ || !showPreview_) {
        return;
    }

    // ベース値をリセット
    previewObject_->transform_.scale_       = previewBaseTransform_.scale;
    previewObject_->transform_.rotation_    = previewBaseTransform_.rotation;
    previewObject_->transform_.translation_ = previewBaseTransform_.translation;

    // 選択中のアニメーションを適用
    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        auto& category = categories_[selectedCategoryIndex_];
        if (category.selectedAnimationIndex >= 0 && category.selectedAnimationIndex < static_cast<int>(category.animations.size())) {
            auto* animation = category.animations[category.selectedAnimationIndex].get();

            if (animation && animation->IsPlaying()) {
                // アニメーションのオフセット値を取得して適用
                Vector3 scaleOffset       = animation->GetCurrentScale();
                Vector3 rotationOffset    = animation->GetCurrentRotate();
                Vector3 translationOffset = animation->GetCurrentPos();

                previewObject_->transform_.scale_       = previewBaseTransform_.scale * scaleOffset;
                previewObject_->transform_.rotation_    = previewBaseTransform_.rotation + rotationOffset;
                previewObject_->transform_.translation_ = previewBaseTransform_.translation + translationOffset;
            }
        }
    }

    previewObject_->Update();
}

void ObjEaseAnimationEditor::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Object Ease Animation Manager")) {
        ImGui::PushID("ObjEaseAnimationManager");

        // プレビュー設定
        ImGui::SeparatorText("Preview Settings");
        ImGui::Checkbox("Show Preview", &showPreview_);

        if (showPreview_) {
            ImGui::InputText("Preview Model", previewModelNameBuffer_, IM_ARRAYSIZE(previewModelNameBuffer_));
         
            if (ImGui::Button("Change Model")) {
                ChangePreviewModel(previewModelNameBuffer_);
            }

            ImGui::DragFloat3("Base Scale", &previewBaseTransform_.scale.x, 0.01f);
            ImGui::DragFloat3("Base Rotation", &previewBaseTransform_.rotation.x, 0.01f);
            ImGui::DragFloat3("Base Translation", &previewBaseTransform_.translation.x, 0.1f);

            if (ImGui::Button("Reset Preview Transform")) {
                previewBaseTransform_.scale       = Vector3::OneVector();
                previewBaseTransform_.rotation    = Vector3::ZeroVector();
                previewBaseTransform_.translation = Vector3::ZeroVector();
            }
        }

        ImGui::Separator();

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
            std::string labelText = categories_[i].name + " (" + std::to_string(categories_[i].animations.size()) + " animations)";

            if (ImGui::Selectable(labelText.c_str(), isSelected)) {
                selectedCategoryIndex_ = i;
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        // 選択されたカテゴリーの編集
        if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
            auto& selectedCategory = categories_[selectedCategoryIndex_];

            ImGui::SeparatorText(("Category: " + selectedCategory.name).c_str());

            // アニメーション追加
            ImGui::InputText("New Animation Name", animationNameBuffer_, IM_ARRAYSIZE(animationNameBuffer_));
            if (ImGui::Button("Add Animation")) {
                if (strlen(animationNameBuffer_) > 0) {
                    AddAnimation(selectedCategoryIndex_, animationNameBuffer_);
                    animationNameBuffer_[0] = '\0';
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete Selected Animation")) {
                if (selectedCategory.selectedAnimationIndex >= 0 && selectedCategory.selectedAnimationIndex < static_cast<int>(selectedCategory.animations.size())) {
                    RemoveAnimation(selectedCategoryIndex_, selectedCategory.selectedAnimationIndex);
                }
            }

            ImGui::Separator();

            // アニメーションリスト表示
            ImGui::Text("Animations (%d):", static_cast<int>(selectedCategory.animations.size()));
            for (int j = 0; j < static_cast<int>(selectedCategory.animations.size()); j++) {
                ImGui::PushID(j);

                bool isAnimSelected       = (selectedCategory.selectedAnimationIndex == j);
                std::string animLabelText = selectedCategory.animations[j]->GetGroupName();

                // 再生中のアニメーションを強調表示
                if (selectedCategory.animations[j]->IsPlaying()) {
                    animLabelText += " [PLAYING]";
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                }

                if (ImGui::Selectable(animLabelText.c_str(), isAnimSelected)) {
                    selectedCategory.selectedAnimationIndex = j;
                }

                if (selectedCategory.animations[j]->IsPlaying()) {
                    ImGui::PopStyleColor();
                }

                ImGui::PopID();
            }

            ImGui::Separator();

            // 選択されたアニメーションの編集
            if (selectedCategory.selectedAnimationIndex >= 0 && selectedCategory.selectedAnimationIndex < static_cast<int>(selectedCategory.animations.size())) {
                selectedCategory.animations[selectedCategory.selectedAnimationIndex]->AdjustParam();
            }
        }

        ImGui::Separator();
        ImGui::SeparatorText("File Operations");

        // 個別セーブ/ロード
        if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
            auto& selectedCategory = categories_[selectedCategoryIndex_];

            if (selectedCategory.selectedAnimationIndex >= 0 && selectedCategory.selectedAnimationIndex < static_cast<int>(selectedCategory.animations.size())) {

                auto* selectedAnimation   = selectedCategory.animations[selectedCategory.selectedAnimationIndex].get();
                std::string animationName = selectedAnimation->GetGroupName();

                ImGui::SeparatorText("Selected Animation File Operations");

                // 個別Load
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
                if (ImGui::Button(("Load " + animationName).c_str())) {
                    selectedAnimation->LoadData();
                }
                ImGui::PopStyleColor(3);
                ImGui::SameLine();

                // 個別Save
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
                if (ImGui::Button(("Save " + animationName).c_str())) {
                    selectedAnimation->SaveData();
                    MessageBoxA(nullptr, (animationName + " saved successfully.").c_str(), "Animation Editor", 0);
                }
                ImGui::PopStyleColor(3);
            }

            ImGui::SeparatorText("Category File Operations");

            // カテゴリー単位のセーブ
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
            if (ImGui::Button(("Save Category: " + selectedCategory.name).c_str())) {
                SaveCategory(selectedCategoryIndex_);
                MessageBoxA(nullptr, ("Category " + selectedCategory.name + " saved successfully.").c_str(), "Animation Editor", 0);
            }
            ImGui::PopStyleColor(3);
        }

        ImGui::SeparatorText("All File Operations");

        // Load All
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
        if (ImGui::Button("Load All Categories")) {
            AllLoadFile();
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();

        // Save All
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
        if (ImGui::Button("Save All Categories")) {
            AllSaveFile();
            MessageBoxA(nullptr, "All categories saved successfully.", "Animation Editor", 0);
        }
        ImGui::PopStyleColor(3);

        ImGui::PopID();
    }
#endif
}

void ObjEaseAnimationEditor::AddCategory(const std::string& categoryName) {
    // 既存チェック
    auto it = std::find_if(categories_.begin(), categories_.end(),
        [&categoryName](const Category& cat) {
            return cat.name == categoryName;
        });

    if (it != categories_.end()) {
        return; // 既に存在する
    }

    // 新規カテゴリー作成
    Category newCategory;
    newCategory.name = categoryName;
    categories_.push_back(std::move(newCategory));
    selectedCategoryIndex_ = static_cast<int32_t>(categories_.size()) - 1;

    // フォルダ作成
    std::string folderPath = "Resources/GlobalParameter/ObjEaseAnimation/" + categoryName;
    std::filesystem::create_directories(folderPath);
}

void ObjEaseAnimationEditor::RemoveCategory(const int32_t& index) {
    if (index >= 0 && index < static_cast<int>(categories_.size())) {
        categories_.erase(categories_.begin() + index);

        // 選択インデックス調整
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

void ObjEaseAnimationEditor::AddAnimation(const int32_t& categoryIndex, const std::string& animationName) {
    if (categoryIndex < 0 || categoryIndex >= static_cast<int>(categories_.size())) {
        return;
    }

    auto& category = categories_[categoryIndex];

    // 既存チェック
    auto it = std::find_if(category.animations.begin(), category.animations.end(),
        [&animationName](const std::unique_ptr<ObjEaseAnimationData>& anim) {
            return anim->GetGroupName() == animationName;
        });

    if (it != category.animations.end()) {
        return; // 既に存在する
    }

    // 新規アニメーション作成
    auto animation = std::make_unique<ObjEaseAnimationData>();
    animation->Init(animationName, category.name);
    category.animations.push_back(std::move(animation));
    category.selectedAnimationIndex = static_cast<int32_t>(category.animations.size()) - 1;
}

void ObjEaseAnimationEditor::RemoveAnimation(const int32_t& categoryIndex, const int32_t& animationIndex) {
    if (categoryIndex < 0 || categoryIndex >= static_cast<int>(categories_.size())) {
        return;
    }

    auto& category = categories_[categoryIndex];

    if (animationIndex >= 0 && animationIndex < static_cast<int>(category.animations.size())) {
        category.animations.erase(category.animations.begin() + animationIndex);

        // 選択インデックス調整
        if (category.selectedAnimationIndex >= animationIndex) {
            category.selectedAnimationIndex--;
            if (category.selectedAnimationIndex < 0 && !category.animations.empty()) {
                category.selectedAnimationIndex = 0;
            } else if (category.animations.empty()) {
                category.selectedAnimationIndex = -1;
            }
        }
    }
}

void ObjEaseAnimationEditor::AllSaveFile() {
    for (int32_t i = 0; i < static_cast<int>(categories_.size()); ++i) {
        SaveCategory(i);
    }
}

void ObjEaseAnimationEditor::SaveCategory(const int32_t& categoryIndex) {
    if (categoryIndex < 0 || categoryIndex >= static_cast<int>(categories_.size())) {
        return;
    }

    auto& category = categories_[categoryIndex];

    // カテゴリー内の全アニメーションを保存
    for (auto& animation : category.animations) {
        animation->SaveData();
    }
}

ObjEaseAnimationData* ObjEaseAnimationEditor::GetAnimationByName(const std::string& categoryName, const std::string& animationName) {
    // カテゴリーを検索
    auto catIt = std::find_if(categories_.begin(), categories_.end(),
        [&categoryName](const Category& cat) {
            return cat.name == categoryName;
        });

    if (catIt == categories_.end()) {
        return nullptr;
    }

    // アニメーションを検索
    auto animIt = std::find_if(catIt->animations.begin(), catIt->animations.end(),
        [&animationName](const std::unique_ptr<ObjEaseAnimationData>& anim) {
            return anim->GetGroupName() == animationName;
        });

    if (animIt != catIt->animations.end()) {
        return animIt->get();
    }

    return nullptr;
}

void ObjEaseAnimationEditor::ChangePreviewModel(const std::string& modelName) {
    previewObject_->SetModelByName(modelName);
}
