#include "EffectEditorSuite.h"
#include "Editor/CameraEditor/CameraEditor.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationEditor.h"


EffectEditorSuite::EffectEditorSuite() = default;
EffectEditorSuite::~EffectEditorSuite() = default;

void EffectEditorSuite::Init() {
    objEaseAnimationEditor_ = std::make_unique<ObjEaseAnimationEditor>();
    cameraEditor_           = std::make_unique<CameraEditor>();

    // 初期化
    objEaseAnimationEditor_->Init("ObjEaseAnimation", true);
    cameraEditor_->Init("CameraAnimation");
}


void EffectEditorSuite::Update(const float& deltaTime) {

    objEaseAnimationEditor_->Update(deltaTime);
    cameraEditor_->Update(deltaTime);
}

void EffectEditorSuite::EditorUpdate() {
    objEaseAnimationEditor_->EditorUpdate();
    cameraEditor_->EditorUpdate();
}