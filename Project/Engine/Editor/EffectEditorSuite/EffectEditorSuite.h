#pragma once

#include <memory>

class ViewProjection;
class ObjEaseAnimationEditor;
class CameraEditor;
class EffectEditorSuite {
public:
    EffectEditorSuite();
    ~EffectEditorSuite();
    //*----------------------------- public Methods -----------------------------*//

    // 初期化、更新、エディター更新
    void Init();
    void Update();
    void EditorUpdate();

private:
    std::unique_ptr<ObjEaseAnimationEditor> objEaseAnimationEditor_;
    std::unique_ptr<CameraEditor> cameraEditor_;

public:
    CameraEditor* GetCameraEditor() const { return cameraEditor_.get(); }
    void SetViewProjection(ViewProjection* viewProjection);
};