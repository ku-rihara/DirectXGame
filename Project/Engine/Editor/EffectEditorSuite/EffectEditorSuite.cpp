#include "EffectEditorSuite.h"
#include "EffectInlineEditRequest.h"

using namespace KetaEngine;
// editor
#include "Editor/CameraEditor/CameraEditor.h"
#include "Editor/DissolveEditor/DissolveEditor.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationEditor.h"
#include "Editor/PostEffectEditor/PostEffectEditor.h"
#include "Editor/RailEditor/RailEditor.h"
#include "Editor/RibbonTrailEditor/RibbonTrailEditor.h"
#include "Editor/ShakeEditor/ShakeEditor.h"
#include "Editor/SpriteEaseAnimation/SpriteEaseAnimationEditor.h"
#include "Editor/TimeScaleEditor/TimeScaleEditor.h"
#include "Particle/CPUParticle/Editor/ParticleEditor.h"
#include "Particle/GPUParticle/Editor/GPUParticleEditor.h"
// frame
#include "Frame/Frame.h"
// imgui
#include <imgui.h>

EffectEditorSuite::EffectEditorSuite()  = default;
EffectEditorSuite::~EffectEditorSuite() = default;

void EffectEditorSuite::Init() {

    // 生成
    objEaseAnimationEditor_    = std::make_unique<ObjEaseAnimationEditor>();
    spriteEaseAnimationEditor_ = std::make_unique<SpriteEaseAnimationEditor>();
    cameraEditor_              = std::make_unique<CameraEditor>();
    shakeEditor_               = std::make_unique<ShakeEditor>();
    railEditor_                = std::make_unique<RailEditor>();
    gpuParticleEditor_         = std::make_unique<GPUParticleEditor>();
    particleEditor_            = std::make_unique<ParticleEditor>();
    dissolveEditor_            = std::make_unique<DissolveEditor>();
    timeScaleEditor_           = std::make_unique<TimeScaleEditor>();
    ribbonTrailEditor_         = std::make_unique<RibbonTrailEditor>();
    postEffectEditor_          = std::make_unique<PostEffectEditor>();

    // 初期化
    objEaseAnimationEditor_->Init("ObjEaseAnimation");
    spriteEaseAnimationEditor_->Init("SpriteEaseAnimation");
    cameraEditor_->Init("CameraAnimation");
    shakeEditor_->Init("Shake");
    railEditor_->Init("Rail");
    gpuParticleEditor_->Init("GPUParticle");
    particleEditor_->Init("Particle");
    dissolveEditor_->Init("Dissolve");
    timeScaleEditor_->Init("TimeScale");
    ribbonTrailEditor_->Init("RibbonTrail");
    postEffectEditor_->Init("PostEffect");

    // SelectFileEditマップを初期化
    InitEditorSelectFileEditMap();
    InitEditorSaveLoadMaps();

    // インライン編集リクエストを受け取るコールバックを登録
    static const std::unordered_map<std::string, EffectEditorType> kTypeNameMap = {
        {"ObjEaseAnimation", EffectEditorType::ObjEaseAnimation},
        {"SpriteEaseAnimation", EffectEditorType::SpriteEaseAnimation},
        {"Camera", EffectEditorType::Camera},
        {"Shake", EffectEditorType::Shake},
        {"Rail", EffectEditorType::Rail},
        {"GPUParticle", EffectEditorType::GPUParticle},
        {"Particle", EffectEditorType::Particle},
        {"Dissolve", EffectEditorType::Dissolve},
        {"TimeScale", EffectEditorType::TimeScale},
        {"RibbonTrail", EffectEditorType::RibbonTrail},
        {"PostEffect", EffectEditorType::PostEffect},
    };
    EffectInlineEditRequest::SetCallback([this](const std::string& editorType, const std::string& name, const std::string& category) {
        auto it = kTypeNameMap.find(editorType);
        if (it != kTypeNameMap.end()) {
            OpenInlineEditor(it->second, name, category);
        }
    });
}

void EffectEditorSuite::Update() {
    objEaseAnimationEditor_->Update();
    spriteEaseAnimationEditor_->Update();
    cameraEditor_->Update();
    shakeEditor_->Update();
    railEditor_->Update();
    gpuParticleEditor_->Update();
    particleEditor_->Update();
    dissolveEditor_->Update();
    timeScaleEditor_->Update(Frame::DeltaTime());
    ribbonTrailEditor_->Update();
    postEffectEditor_->Update();
}

void EffectEditorSuite::EditorUpdate() {
    objEaseAnimationEditor_->EditorUpdate();
    spriteEaseAnimationEditor_->EditorUpdate();
    cameraEditor_->EditorUpdate();
    dissolveEditor_->EditorUpdate();
    shakeEditor_->EditorUpdate();
    railEditor_->EditorUpdate();
    gpuParticleEditor_->EditorUpdate();
    particleEditor_->EditorUpdate();
    timeScaleEditor_->EditorUpdate();
    ribbonTrailEditor_->EditorUpdate();
    postEffectEditor_->EditorUpdate();

    // コンボエディターからのインライン編集ウィンドウ
    DrawInlineEditorWindow();
}

void EffectEditorSuite::InitEditorSelectFileEditMap() {
    editorSelectFileEditMap_ = {
        {EffectEditorType::ObjEaseAnimation, [this](const std::string& name, const std::string& category) {
             objEaseAnimationEditor_->SelectFileEdit(name, category);
         }},
        {EffectEditorType::SpriteEaseAnimation, [this](const std::string& name, const std::string& category) {
             spriteEaseAnimationEditor_->SelectFileEdit(name, category);
         }},
        {EffectEditorType::Camera, [this](const std::string& name, const std::string& category) {
             cameraEditor_->SelectFileEdit(name, category);
         }},
        {EffectEditorType::Shake, [this](const std::string& name, const std::string& category) {
             shakeEditor_->SelectFileEdit(name, category);
         }},
        {EffectEditorType::Rail, [this](const std::string& name, const std::string& category) {
             railEditor_->SelectFileEdit(name, category);
         }},
        {EffectEditorType::GPUParticle, [this](const std::string& name, const std::string& category) {
             gpuParticleEditor_->SelectFileEdit(name, category);
         }},
        {EffectEditorType::Particle, [this](const std::string& name, const std::string& category) {
             particleEditor_->SelectFileEdit(name, category);
         }},
        {EffectEditorType::Dissolve, [this](const std::string& name, const std::string& category) {
             dissolveEditor_->SelectFileEdit(name, category);
         }},
        {EffectEditorType::TimeScale, [this](const std::string& name, const std::string& category) {
             timeScaleEditor_->SelectFileEdit(name, category);
         }},
        {EffectEditorType::RibbonTrail, [this](const std::string& name, const std::string& category) {
             ribbonTrailEditor_->SelectFileEdit(name, category);
         }},
        {EffectEditorType::PostEffect, [this](const std::string& name, const std::string& category) {
             postEffectEditor_->SelectFileEdit(name, category);
         }}};
}

void EffectEditorSuite::PlayEffect(EffectEditorType type, const std::string& fileName, const std::string& categoryName) {
    if (fileName.empty() || fileName == "None") {
        return;
    }

    switch (type) {
    case EffectEditorType::Camera: {
        auto* effect = cameraEditor_->GetEffectByName(categoryName, fileName);
        if (effect) {
            effect->Play();
        }
        break;
    }
    case EffectEditorType::Shake: {
        auto* effect = shakeEditor_->GetEffectByName(categoryName, fileName);
        if (effect) {
            effect->Play();
        }
        break;
    }
    case EffectEditorType::TimeScale: {
        auto* effect = timeScaleEditor_->GetEffectByName(categoryName, fileName);
        if (effect) {
            effect->Play();
        }
        break;
    }
    case EffectEditorType::Particle: {
        auto* effect = particleEditor_->GetEffectByName(categoryName, fileName);
        if (effect) {
            effect->Play();
        }
        break;
    }
    case EffectEditorType::GPUParticle: {
        auto* effect = gpuParticleEditor_->GetEffectByName(categoryName, fileName);
        if (effect) {
            effect->Play();
        }
        break;
    }
    case EffectEditorType::ObjEaseAnimation: {
        auto* effect = objEaseAnimationEditor_->GetEffectByName(categoryName, fileName);
        if (effect) {
            effect->Play();
        }
        break;
    }
    case EffectEditorType::RibbonTrail: {
        auto* effect = ribbonTrailEditor_->GetEffectByName(categoryName, fileName);
        if (effect) {
            effect->Play();
        }
        break;
    }
    case EffectEditorType::PostEffect: {
        postEffectEditor_->PlayPostEffect(fileName, categoryName);
        break;
    }
    default:
        break;
    }
}

void EffectEditorSuite::SelectFileEdit(EffectEditorType type, const std::string& fileName, const std::string& categoryName) {
    auto it = editorSelectFileEditMap_.find(type);
    if (it != editorSelectFileEditMap_.end()) {
        it->second(fileName, categoryName);
    }
}

void EffectEditorSuite::OpenInlineEditor(EffectEditorType type, const std::string& fileName, const std::string& categoryName) {
    isInlineEditorOpen_       = true;
    inlineEditorType_         = type;
    inlineEditorFileName_     = fileName;
    inlineEditorCategoryName_ = categoryName;
}

void EffectEditorSuite::DrawInlineEditorWindow() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (!isInlineEditorOpen_ || inlineEditorFileName_.empty()) {
        return;
    }

    std::string title = "[編集] " + inlineEditorFileName_ + "##ComboEditorInline";
    bool open         = true;

    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(title.c_str(), &open)) {
        ImGui::TextDisabled("カテゴリー: %s", inlineEditorCategoryName_.c_str());
        ImGui::Separator();

        // パラメータUI（AdjustParam）
        auto adjIt = editorSelectFileEditMap_.find(inlineEditorType_);
        if (adjIt != editorSelectFileEditMap_.end()) {
            adjIt->second(inlineEditorFileName_, inlineEditorCategoryName_);
        }

        ImGui::Separator();

        // Load ボタン
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
        if (ImGui::Button("Load")) {
            auto loadIt = editorLoadFileMap_.find(inlineEditorType_);
            if (loadIt != editorLoadFileMap_.end()) {
                loadIt->second(inlineEditorFileName_, inlineEditorCategoryName_);
            }
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        // Save ボタン
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
        if (ImGui::Button("Save")) {
            auto saveIt = editorSaveFileMap_.find(inlineEditorType_);
            if (saveIt != editorSaveFileMap_.end()) {
                saveIt->second(inlineEditorFileName_, inlineEditorCategoryName_);
            }
            MessageBoxA(nullptr, (inlineEditorFileName_ + " を保存しました。").c_str(), "エフェクト編集", 0);
        }
        ImGui::PopStyleColor(3);
    }
    ImGui::End();

    if (!open) {
        isInlineEditorOpen_ = false;
    }
#endif
}

void EffectEditorSuite::InitEditorSaveLoadMaps() {
    // Save マップ
    editorSaveFileMap_ = {
        {EffectEditorType::Camera, [this](const std::string& name, const std::string& category) {
             auto* effect = cameraEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->SaveData();
             }
         }},
        {EffectEditorType::Shake, [this](const std::string& name, const std::string& category) {
             auto* effect = shakeEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->SaveData();
             }
         }},
        {EffectEditorType::TimeScale, [this](const std::string& name, const std::string& category) {
             auto* effect = timeScaleEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->SaveData();
             }
         }},
        {EffectEditorType::Particle, [this](const std::string& name, const std::string& category) {
             auto* effect = particleEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->SaveData();
             }
         }},
        {EffectEditorType::GPUParticle, [this](const std::string& name, const std::string& category) {
             auto* effect = gpuParticleEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->SaveData();
             }
         }},
        {EffectEditorType::ObjEaseAnimation, [this](const std::string& name, const std::string& category) {
             auto* effect = objEaseAnimationEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->SaveData();
             }
         }},
        {EffectEditorType::RibbonTrail, [this](const std::string& name, const std::string& category) {
             auto* effect = ribbonTrailEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->SaveData();
             }
         }},
        {EffectEditorType::PostEffect, [this](const std::string& name, const std::string& category) {
             auto* effect = postEffectEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->SaveData();
             }
         }},
    };

    // Load マップ
    editorLoadFileMap_ = {
        {EffectEditorType::Camera, [this](const std::string& name, const std::string& category) {
             auto* effect = cameraEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->LoadData();
             }
         }},
        {EffectEditorType::Shake, [this](const std::string& name, const std::string& category) {
             auto* effect = shakeEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->LoadData();
             }
         }},
        {EffectEditorType::TimeScale, [this](const std::string& name, const std::string& category) {
             auto* effect = timeScaleEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->LoadData();
             }
         }},
        {EffectEditorType::Particle, [this](const std::string& name, const std::string& category) {
             auto* effect = particleEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->LoadData();
             }
         }},
        {EffectEditorType::GPUParticle, [this](const std::string& name, const std::string& category) {
             auto* effect = gpuParticleEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->LoadData();
             }
         }},
        {EffectEditorType::ObjEaseAnimation, [this](const std::string& name, const std::string& category) {
             auto* effect = objEaseAnimationEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->LoadData();
             }
         }},
        {EffectEditorType::RibbonTrail, [this](const std::string& name, const std::string& category) {
             auto* effect = ribbonTrailEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->LoadData();
             }
         }},
        {EffectEditorType::PostEffect, [this](const std::string& name, const std::string& category) {
             auto* effect = postEffectEditor_->GetEffectByName(category, name);
             if (effect) {
                 effect->LoadData();
             }
         }},
    };
}

void EffectEditorSuite::SetViewProjection(ViewProjection* viewProjection) {
    cameraEditor_->SetViewProjection(viewProjection);
}

void EffectEditorSuite::SetCameraPreViewPos(const Vector3& pos) {
    cameraEditor_->SetPreviewObjPos(pos);
}