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
class RibbonTrailEditor;

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
    TimeScale,
    RibbonTrail
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

    /// <summary>
    /// エディター経由でエフェクトを再生（プレビュー用）
    /// </summary>
    void PlayEffect(EffectEditorType type, const std::string& fileName, const std::string& categoryName);

    /// <summary>
    /// コンボエディターから特定ファイルのインライン編集ウィンドウを開く
    /// </summary>
    void OpenInlineEditor(EffectEditorType type, const std::string& fileName, const std::string& categoryName);

    /// <summary>
    /// インライン編集ウィンドウを描画（EditorUpdateから毎フレーム呼ばれる）
    /// </summary>
    void DrawInlineEditorWindow();

private:
    // エディタへのアクセス用マップ
    using EffectFileFunc = std::function<void(const std::string&, const std::string&)>;
    std::unordered_map<EffectEditorType, EffectFileFunc> editorSelectFileEditMap_;
    std::unordered_map<EffectEditorType, EffectFileFunc> editorSaveFileMap_;
    std::unordered_map<EffectEditorType, EffectFileFunc> editorLoadFileMap_;

    // マップを初期化するヘルパー関数
    void InitEditorSelectFileEditMap();
    void InitEditorSaveLoadMaps();

    // インラインエディター状態
    bool isInlineEditorOpen_            = false;
    EffectEditorType inlineEditorType_  = EffectEditorType::Camera;
    std::string inlineEditorFileName_;
    std::string inlineEditorCategoryName_;

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
    std::unique_ptr<RibbonTrailEditor> ribbonTrailEditor_;

public:
    CameraEditor* GetCameraEditor() const { return cameraEditor_.get(); }

    void SetViewProjection(ViewProjection* viewProjection);
    void SetCameraPreViewPos(const Vector3& pos);
};

}; // KetaEngine