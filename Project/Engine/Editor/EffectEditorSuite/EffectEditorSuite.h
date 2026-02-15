#pragma once

// math
#include "Vector3.h"
// std
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace KetaEngine {

class ViewProjection;

class ObjEaseAnimationEditor;
class SpriteEaseAnimationEditor;
class CameraEditor;
class ShakeEditor;
class RailEditor;
class GPUParticleEditor;
class ParticleEditor;
class DissolveEditor;
class TimeScaleEditor;

// エフェクトタイプ列挙型
enum class EffectEditorType {
    ObjEaseAnimation,
    SpriteEaseAnimation,
    Camera,
    Shake,
    Rail,
    GPUParticle,
    Particle,
    Dissolve,
    TimeScale
};

class EffectEditorSuite {
public:
    EffectEditorSuite();
    ~EffectEditorSuite();
    //*----------------------------- public Methods -----------------------------*//

    // 初期化、更新、エディター更新
    void Init();
    void Update();
    void EditorUpdate();

    // 統一されたSelectFileEdit呼び出し
    void SelectFileEdit(EffectEditorType type, const std::string& fileName, const std::string& categoryName);

private:
    // エディタへのアクセス用マップ
    using SelectFileEditFunc = std::function<void(const std::string&, const std::string&)>;
    std::unordered_map<EffectEditorType, SelectFileEditFunc> editorSelectFileEditMap_;

    // マップを初期化するヘルパー関数
    void InitEditorSelectFileEditMap();

private:
    std::unique_ptr<ObjEaseAnimationEditor> objEaseAnimationEditor_;
    std::unique_ptr<SpriteEaseAnimationEditor> spriteEaseAnimationEditor_;
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