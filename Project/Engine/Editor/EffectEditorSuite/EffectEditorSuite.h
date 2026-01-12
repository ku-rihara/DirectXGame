#pragma once

#include <memory>

namespace KetaEngine {

class ViewProjection;

class ObjEaseAnimationEditor;
class CameraEditor;
class ShakeEditor;
class RailEditor;
class GPUParticleEditor;
class ParticleEditor;
class DissolveEditor;
class TimeScaleEditor;

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
    std::unique_ptr<ShakeEditor> shakeEditor_;
    std::unique_ptr<RailEditor> railEditor_;
    std::unique_ptr<GPUParticleEditor> gpuParticleEditor_;
    std::unique_ptr<ParticleEditor> particleEditor_;
    std::unique_ptr<DissolveEditor> dissolveEditor_;
    std::unique_ptr<TimeScaleEditor> timeScaleEditor_;

public:
    CameraEditor* GetCameraEditor() const { return cameraEditor_.get(); }

    void SetViewProjection(ViewProjection* viewProjection);
    void SetCameraPreViewPos(const Vector3& pos);
};

}; // KetaEngine
