#include "EffectEditorSuite.h"

using namespace KetaEngine;
// editor
#include "Editor/CameraEditor/CameraEditor.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationEditor.h"
#include"Editor/ShakeEditor/ShakeEditor.h"
#include"Editor/RailEditor/RailEditor.h"
#include"Editor/GPUParticleEditor/GPUParticleEditor.h"
#include"Editor/ParticleEditor/ParticleEditor.h"


EffectEditorSuite::EffectEditorSuite() = default;
EffectEditorSuite::~EffectEditorSuite() = default;

void EffectEditorSuite::Init() {
    objEaseAnimationEditor_ = std::make_unique<ObjEaseAnimationEditor>();
    cameraEditor_           = std::make_unique<CameraEditor>();
    shakeEditor_            = std::make_unique<ShakeEditor>();
    railEditor_             = std::make_unique<RailEditor>();
    gpuParticleEditor_      = std::make_unique<GPUParticleEditor>();
    particleEditor_         = std::make_unique<ParticleEditor>();

    // 初期化:trueの場合カテゴリーシステムを使用
    objEaseAnimationEditor_->Init("ObjEaseAnimation", true);
    cameraEditor_->Init("CameraAnimation");
    shakeEditor_->Init("Shake");
    railEditor_->Init("Rail");
    gpuParticleEditor_->Init("GPUParticle", true);
    particleEditor_->Init("Particle", true);
}


void EffectEditorSuite::Update() {

    objEaseAnimationEditor_->Update();
    cameraEditor_->Update();
    shakeEditor_->Update();
    railEditor_->Update();
    gpuParticleEditor_->Update();
    particleEditor_->Update();
}

void EffectEditorSuite::EditorUpdate() {
    objEaseAnimationEditor_->EditorUpdate();
    cameraEditor_->EditorUpdate();
    shakeEditor_->EditorUpdate();
    railEditor_->EditorUpdate();
    gpuParticleEditor_->EditorUpdate();
    particleEditor_->EditorUpdate();
}

void EffectEditorSuite::SetViewProjection(ViewProjection* viewProjection) {
    cameraEditor_->SetViewProjection(viewProjection);
 }
