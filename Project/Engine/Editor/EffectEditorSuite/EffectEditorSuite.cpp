#include "EffectEditorSuite.h"

using namespace KetaEngine;
// editor
#include "Editor/CameraEditor/CameraEditor.h"
#include "Editor/DissolveEditor/DissolveEditor.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationEditor.h"
#include "Editor/RailEditor/RailEditor.h"
#include "Editor/ShakeEditor/ShakeEditor.h"
#include "Editor/TimeScaleEditor/TimeScaleEditor.h"
#include "Particle/CPUParticle/Editor/ParticleEditor.h"
#include "Particle/GPUParticle/Editor/GPUParticleEditor.h"
// frame
#include "Frame/Frame.h"

EffectEditorSuite::EffectEditorSuite()  = default;
EffectEditorSuite::~EffectEditorSuite() = default;

void EffectEditorSuite::Init() {

    // 生成
    objEaseAnimationEditor_ = std::make_unique<ObjEaseAnimationEditor>();
    cameraEditor_           = std::make_unique<CameraEditor>();
    shakeEditor_            = std::make_unique<ShakeEditor>();
    railEditor_             = std::make_unique<RailEditor>();
    gpuParticleEditor_      = std::make_unique<GPUParticleEditor>();
    particleEditor_         = std::make_unique<ParticleEditor>();
    dissolveEditor_         = std::make_unique<DissolveEditor>();
    timeScaleEditor_        = std::make_unique<TimeScaleEditor>();

    // 初期化
    objEaseAnimationEditor_->Init("ObjEaseAnimation");
    cameraEditor_->Init("CameraAnimation");
    shakeEditor_->Init("Shake");
    railEditor_->Init("Rail");
    gpuParticleEditor_->Init("GPUParticle");
    particleEditor_->Init("Particle");
    dissolveEditor_->Init("Dissolve");
    timeScaleEditor_->Init("TimeScale");

    // SelectFileEditマップを初期化
    InitEditorSelectFileEditMap();
}

void EffectEditorSuite::Update() {
    objEaseAnimationEditor_->Update();
    cameraEditor_->Update();
    shakeEditor_->Update();
    railEditor_->Update();
    gpuParticleEditor_->Update();
    particleEditor_->Update();
    dissolveEditor_->Update();
    timeScaleEditor_->Update(Frame::DeltaTime());
}

void EffectEditorSuite::EditorUpdate() {
    objEaseAnimationEditor_->EditorUpdate();
    cameraEditor_->EditorUpdate();
    dissolveEditor_->EditorUpdate();
    shakeEditor_->EditorUpdate();
    railEditor_->EditorUpdate();
    gpuParticleEditor_->EditorUpdate();
    particleEditor_->EditorUpdate();
    timeScaleEditor_->EditorUpdate();
}

void EffectEditorSuite::InitEditorSelectFileEditMap() {
    editorSelectFileEditMap_ = {
        {EffectEditorType::ObjEaseAnimation, [this](const std::string& name, const std::string& category) {
             objEaseAnimationEditor_->SelectFileEdit(name, category);
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
         }}
    };
}

void EffectEditorSuite::SelectFileEdit(EffectEditorType type, const std::string& fileName, const std::string& categoryName) {
    auto it = editorSelectFileEditMap_.find(type);
    if (it != editorSelectFileEditMap_.end()) {
        it->second(fileName, categoryName);
    }
}

void EffectEditorSuite::SetViewProjection(ViewProjection* viewProjection) {
    cameraEditor_->SetViewProjection(viewProjection);
}

void EffectEditorSuite::SetCameraPreViewPos(const Vector3& pos) {
    cameraEditor_->SetPreviewObjPos(pos);
}