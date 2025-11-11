#include "PostEffectController.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <Windows.h>

void PostEffectController::Init() {
    renderer_ = PostEffectRenderer::GetInstance();
    AllLoadFile();
    isPostEffectActive_ = false;
    currentTimer_       = 0.0f;
    targetDuration_     = 0.0f;
    defaultMode_        = PostEffectMode::NONE;
}

void PostEffectController::Update(const float& deltaTime) {
    if (isPostEffectActive_) {
        currentTimer_ += deltaTime;

        // 時間が経過したらPostEffectを戻す
        if (currentTimer_ >= targetDuration_) {
            StopPostEffect();
        }
    }
}

void PostEffectController::PlayPostEffect(const std::string& postEffectName) {
    PostEffectData* postEffect = GetPostEffectByName(postEffectName);
    if (postEffect) {
        renderer_->SetPostEffectMode(postEffect->GetPostEffectMode());

        // 時間管理開始
        isPostEffectActive_ = true;
        currentTimer_       = 0.0f;
        targetDuration_     = postEffect->GetDuration();
    }
}

void PostEffectController::StopPostEffect() {
    renderer_->SetPostEffectMode(defaultMode_);
    isPostEffectActive_ = false;
    currentTimer_       = 0.0f;
    targetDuration_     = 0.0f;
}

void PostEffectController::SetPostEffectImmediate(PostEffectMode mode) {
    renderer_->SetPostEffectMode(mode);
}

PostEffectMode PostEffectController::GetCurrentPostEffect() const {
    return defaultMode_;
}

bool PostEffectController::IsPostEffectActive() const {
    return isPostEffectActive_;
}

void PostEffectController::AllLoadFile() {
    // PostEffectのフォルダ内のすべてのファイルを検索
    std::string folderPath = "Resources/GlobalParameter/PostEffect/";

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
        // 既存のPostEffectをクリア
        postEffects_.clear();
        selectedIndex_ = -1;

        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // 新規作成してロード
                auto postEffect = std::make_unique<PostEffectData>();
                postEffect->Init(fileName);
                postEffect->LoadData(); // PostEffectデータをロード
                postEffects_.push_back(std::move(postEffect));
            }
        }
    }
}

void PostEffectController::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("PostEffect Manager")) {

        // 全体制御
        if (ImGui::Button("Load All PostEffects")) {
            AllLoadFile();
            MessageBoxA(nullptr, "All PostEffects loaded successfully.", "PostEffect Player", 0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save All PostEffects")) {
            AllSaveFile();
            MessageBoxA(nullptr, "All PostEffects saved successfully.", "PostEffect Player", 0);
        }

        ImGui::Separator();

        // デフォルトモード設定
        const char* modeNames[] = {"None", "Gray", "Vignette", "Gaus", "BoxFilter", "RadialBlur", "RandomNoize", "Dissolve", "Outline", "LuminanceOutline"};
        int defaultModeIndex    = static_cast<int>(defaultMode_);
        if (ImGui::Combo("Default PostEffect Mode", &defaultModeIndex, modeNames, IM_ARRAYSIZE(modeNames))) {
            defaultMode_ = static_cast<PostEffectMode>(defaultModeIndex);
        }

        // 新規追加
        ImGui::InputText("New PostEffect Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
        if (ImGui::Button("Add PostEffect")) {
            if (strlen(nameBuffer_) > 0) {
                AddPostEffect(nameBuffer_);
                nameBuffer_[0] = '\0'; // クリア
            }
        }

        // 現在のPostEffect表示
        ImGui::Text("Current PostEffect: %d", static_cast<int>(GetCurrentPostEffect()));

        // タイマー情報表示
        if (IsPostEffectActive()) {
            float remainingTime = targetDuration_ - currentTimer_;
            ImGui::Text("Active Timer: %.2f / %.2f (%.2fs remaining)",
                currentTimer_, targetDuration_, remainingTime);
            ImGui::ProgressBar(currentTimer_ / targetDuration_, ImVec2(0.0f, 0.0f));
        } else {
            ImGui::Text("Status: Inactive");
        }

        // 即座制御
        int immediateValue = static_cast<int>(GetCurrentPostEffect());
        if (ImGui::Combo("Immediate PostEffect", &immediateValue, modeNames, IM_ARRAYSIZE(modeNames))) {
            SetPostEffectImmediate(static_cast<PostEffectMode>(immediateValue));
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            StopPostEffect();
        }

        ImGui::Separator();

        // PostEffectリスト表示
        ImGui::Text("PostEffects (%d):", static_cast<int>(postEffects_.size()));
        for (int i = 0; i < static_cast<int>(postEffects_.size()); i++) {
            ImGui::PushID(i);

            bool isSelected       = (selectedIndex_ == i);
            std::string labelText = postEffects_[i]->GetGroupName() + " (Mode: " + modeNames[static_cast<int>(postEffects_[i]->GetPostEffectMode())] + ", Duration: " + std::to_string(postEffects_[i]->GetDuration()) + "s)";

            if (ImGui::Selectable(labelText.c_str(), isSelected)) {
                selectedIndex_ = i;
            }

            ImGui::SameLine();
            if (ImGui::Button("Play")) {
                PlayPostEffect(postEffects_[i]->GetGroupName());
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        // 選択されたPostEffectの編集
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(postEffects_.size())) {
            postEffects_[selectedIndex_]->AdjustParam();
        }
    }
#endif
}

void PostEffectController::AddPostEffect(const std::string& postEffectName) {
    auto postEffect = std::make_unique<PostEffectData>();
    postEffect->Init(postEffectName);
    postEffects_.push_back(std::move(postEffect));
    selectedIndex_ = static_cast<int>(postEffects_.size()) - 1;
}

void PostEffectController::RemovePostEffect(const int& index) {
    if (index >= 0 && index < static_cast<int>(postEffects_.size())) {
        postEffects_.erase(postEffects_.begin() + index);

        // 選択インデックス調整
        if (selectedIndex_ >= index) {
            selectedIndex_--;
            if (selectedIndex_ < 0 && !postEffects_.empty()) {
                selectedIndex_ = 0;
            } else if (postEffects_.empty()) {
                selectedIndex_ = -1;
            }
        }
    }
}

void PostEffectController::AllSaveFile() {
    // すべてのPostEffectデータを保存
    for (auto& postEffect : postEffects_) {
        postEffect->SaveData();
    }
}

PostEffectData* PostEffectController::GetPostEffectByName(const std::string& name) {
    auto it = std::find_if(postEffects_.begin(), postEffects_.end(),
        [&name](const std::unique_ptr<PostEffectData>& postEffect) {
            return postEffect->GetGroupName() == name;
        });

    if (it != postEffects_.end()) {
        return it->get();
    }
    return nullptr;
}