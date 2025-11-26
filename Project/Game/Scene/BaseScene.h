#pragma once

// DirectX
#include "base/TextureManager.h"
#include "Dx/DirectXCommon.h"

// Transform
#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"

#include "2d/Sprite.h"
#include "audio/Audio.h"
#include "input/Input.h"

// editor
#include "Editor/EffectEditorSuite/EffectEditorSuite.h"
#include "Editor/RailEditor/RailEditor.h"
#include "Editor/ShakeEditor/ShakeEditor.h"

// debug
#include "utility/Debug/DebugCamera.h"

#include <memory>

/// <summary>
/// シーン基底クラス
/// </summary>
class BaseScene {
public:
    enum class CameraMode {
        NORMAL,
        EDITOR,
        DEBUG,
    };

public:
    BaseScene()          = default;
    virtual ~BaseScene() = default;

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
    void EditorClassUpdate();

protected:
    DirectXCommon* dxCommon_        = nullptr;
    Input* input_                   = nullptr;
    Audio* audio_                   = nullptr;
    TextureManager* textureManager_ = nullptr;

    std::unique_ptr<EffectEditorSuite> effectEditorSuite_ = nullptr;
   

    ViewProjection viewProjection_;

    std::unique_ptr<DebugCamera> debugCamera_;
    CameraMode cameraMode_ = CameraMode::NORMAL;


public:
    ///========================================================
    /// getter method
    ///========================================================
    const ViewProjection&
    GetViewProjection() const { return viewProjection_; }

    ///========================================================
    /// setter method
    ///========================================================
};