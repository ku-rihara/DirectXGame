#include "EffectEditorSuite.h"
// editor
#include "Editor/CameraEditor/CameraEditor.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationEditor.h"
#include"Editor/ShakeEditor/ShakeEditor.h"


EffectEditorSuite::EffectEditorSuite() = default;
EffectEditorSuite::~EffectEditorSuite() = default;

void EffectEditorSuite::Init() {
    objEaseAnimationEditor_ = std::make_unique<ObjEaseAnimationEditor>();
    cameraEditor_           = std::make_unique<CameraEditor>();
    shakeEditor_            = std::make_unique<ShakeEditor>();

    // 初期化
    objEaseAnimationEditor_->Init("ObjEaseAnimation", true);
    cameraEditor_->Init("CameraAnimation");
    shakeEditor_->Init("Shake");
}


void EffectEditorSuite::Update() {

    objEaseAnimationEditor_->Update();
    cameraEditor_->Update();
    shakeEditor_->Update();
}

void EffectEditorSuite::EditorUpdate() {
    objEaseAnimationEditor_->EditorUpdate();
    cameraEditor_->EditorUpdate();
    shakeEditor_->EditorUpdate();
}

void EffectEditorSuite::SetViewProjection(ViewProjection* viewProjection) {
    cameraEditor_->SetViewProjection(viewProjection);
 }