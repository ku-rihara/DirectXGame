#include "EffectEditorSuite.h"

using namespace KetaEngine;
// editor
#include "Editor/CameraEditor/CameraEditor.h"
#include "Editor/DissolveEditor/DissolveEditor.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationEditor.h"
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

    // SelectFileEditマップを初期化
    InitEditorSelectFileEditMap();
    InitEditorSaveLoadMaps();
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
         }}
    };
}

void EffectEditorSuite::SelectFileEdit(EffectEditorType type, const std::string& fileName, const std::string& categoryName) {
    auto it = editorSelectFileEditMap_.find(type);
    if (it != editorSelectFileEditMap_.end()) {
        it->second(fileName, categoryName);
    }
}

void EffectEditorSuite::OpenInlineEditor(EffectEditorType type, const std::string& fileName, const std::string& categoryName) {
    isInlineEditorOpen_      = true;
    inlineEditorType_        = type;
    inlineEditorFileName_    = fileName;
    inlineEditorCategoryName_ = categoryName;
}

void EffectEditorSuite::DrawInlineEditorWindow() {
#ifdef _DEBUG
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
        {EffectEditorType::Camera, [this](const std::string& name, const std::string& cat) {
             auto* e = cameraEditor_->GetEffectByName(cat, name);
             if (e) e->SaveData();
         }},
        {EffectEditorType::Shake, [this](const std::string& name, const std::string& cat) {
             auto* e = shakeEditor_->GetEffectByName(cat, name);
             if (e) e->SaveData();
         }},
        {EffectEditorType::TimeScale, [this](const std::string& name, const std::string& cat) {
             auto* e = timeScaleEditor_->GetEffectByName(cat, name);
             if (e) e->SaveData();
         }},
        {EffectEditorType::Particle, [this](const std::string& name, const std::string& cat) {
             auto* e = particleEditor_->GetEffectByName(cat, name);
             if (e) e->SaveData();
         }},
        {EffectEditorType::GPUParticle, [this](const std::string& name, const std::string& cat) {
             auto* e = gpuParticleEditor_->GetEffectByName(cat, name);
             if (e) e->SaveData();
         }},
        {EffectEditorType::ObjEaseAnimation, [this](const std::string& name, const std::string& cat) {
             auto* e = objEaseAnimationEditor_->GetEffectByName(cat, name);
             if (e) e->SaveData();
         }},
        {EffectEditorType::RibbonTrail, [this](const std::string& name, const std::string& cat) {
             auto* e = ribbonTrailEditor_->GetEffectByName(cat, name);
             if (e) e->SaveData();
         }},
    };

    // Load マップ
    editorLoadFileMap_ = {
        {EffectEditorType::Camera, [this](const std::string& name, const std::string& cat) {
             auto* e = cameraEditor_->GetEffectByName(cat, name);
             if (e) e->LoadData();
         }},
        {EffectEditorType::Shake, [this](const std::string& name, const std::string& cat) {
             auto* e = shakeEditor_->GetEffectByName(cat, name);
             if (e) e->LoadData();
         }},
        {EffectEditorType::TimeScale, [this](const std::string& name, const std::string& cat) {
             auto* e = timeScaleEditor_->GetEffectByName(cat, name);
             if (e) e->LoadData();
         }},
        {EffectEditorType::Particle, [this](const std::string& name, const std::string& cat) {
             auto* e = particleEditor_->GetEffectByName(cat, name);
             if (e) e->LoadData();
         }},
        {EffectEditorType::GPUParticle, [this](const std::string& name, const std::string& cat) {
             auto* e = gpuParticleEditor_->GetEffectByName(cat, name);
             if (e) e->LoadData();
         }},
        {EffectEditorType::ObjEaseAnimation, [this](const std::string& name, const std::string& cat) {
             auto* e = objEaseAnimationEditor_->GetEffectByName(cat, name);
             if (e) e->LoadData();
         }},
        {EffectEditorType::RibbonTrail, [this](const std::string& name, const std::string& cat) {
             auto* e = ribbonTrailEditor_->GetEffectByName(cat, name);
             if (e) e->LoadData();
         }},
    };
}

void EffectEditorSuite::SetViewProjection(ViewProjection* viewProjection) {
    cameraEditor_->SetViewProjection(viewProjection);
}

void EffectEditorSuite::SetCameraPreViewPos(const Vector3& pos) {
    cameraEditor_->SetPreviewObjPos(pos);
}