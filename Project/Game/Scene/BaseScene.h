#pragma once

// Base
#include "Base/Dx/DirectXCommon.h"
#include "Base/TextureManager.h"
// Transform
#include "3d/ViewProjection.h"
// Audio
#include "audio/Audio.h"
// Input
#include "input/Input.h"
// editor
#include "Editor/EffectEditorSuite/EffectEditorSuite.h"
// debug
#include "utility/Debug/DebugCamera.h"
// std
#include <functional>
#include <memory>
#include <unordered_map>

/// <summary>
/// シーン基底クラス
/// </summary>
class BaseScene {
public:
    // カメラ描画モード
    enum class CameraMode {
        NORMAL,
        EDITOR,
        DEBUG,
    };

public:
    BaseScene() = default;
    virtual ~BaseScene();

    // 初期化、更新、描画
    virtual void Init();
    virtual void Update();
    virtual void SkyBoxDraw() = 0;

    // デバッグ
    virtual void Debug();

    // viewProjection更新
    virtual void ViewProjectionUpdate();
    virtual void ViewProcess() = 0;

private:
    // Editor更新
    void UpdateEditors();

    // Init用ヘルパー
    void SetupViewProjection();       //< viewProjectionの初期化と各システムへの登録
    void RegisterCameraUpdaters();    //< カメラモードごとの更新処理を登録
    void RegisterCameraTransitions(); //< カメラモードごとの遷移条件を登録

protected:

    // エンジンシステムへの参照
    KetaEngine::DirectXCommon* dxCommon_        = nullptr;
    KetaEngine::Input* input_                   = nullptr;
    KetaEngine::Audio* audio_                   = nullptr;
    KetaEngine::TextureManager* textureManager_ = nullptr;

    // 演出エディターのまとまり
    std::unique_ptr<KetaEngine::EffectEditorSuite> effectEditorSuite_ = nullptr;

    // ViewProjection、デバッグカメラ、カメラモード
    KetaEngine::ViewProjection viewProjection_;
    std::unique_ptr<KetaEngine::DebugCamera> debugCamera_;
    CameraMode cameraMode_ = CameraMode::NORMAL;

    // カメラモードごとの更新処理テーブル
    std::unordered_map<CameraMode, std::function<void()>> cameraUpdaters_;

    // カメラモードごとの遷移条件テーブル
    std::unordered_map<CameraMode, std::function<CameraMode()>> cameraTransitions_;

public:
    ///========================================================
    /// getter method
    ///========================================================
    const KetaEngine::ViewProjection&
    GetViewProjection() const { return viewProjection_; }

    ///========================================================
    /// setter method
    ///========================================================
};