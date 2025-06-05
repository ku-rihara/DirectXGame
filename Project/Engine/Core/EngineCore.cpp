#include "EngineCore.h"
/// 2d
#include "2d/ImGuiManager.h"

/// 3d
#include "3d/ModelManager.h"

/// base
#include "base/FullscreenRenderer.h"
#include "base/Object3DCommon.h"
#include "base/SkyBoxRenderer.h"
#include "base/SpriteCommon.h"
#include "base/SrvManager.h"
#include "base/TextureManager.h"
#include "base/WinApp.h"
#include "Dx/DirectXCommon.h"

/// audio,input
#include "audio/Audio.h"
#include "input/Input.h"

/// utility
#include "utility/ParticleEditor/ParticleCommon.h"
#include "utility/ParticleEditor/ParticleManager.h"

/// imgui,function
#include "function/Convert.h"
#include <imgui_impl_dx12.h>

/// std
#include <string>

///=======================================================================
/// 初期化
///========================================================================
void EngineCore::Initialize(const char* title, int width, int height) {
    // ゲームウィンドウの作成
    std::string windowTitle = std::string(title);
    auto&& titleString      = ConvertString(windowTitle);
    winApp_                 = std::make_unique<WinApp>();
    winApp_->MakeWindow(titleString.c_str(), width, height);

    // DirectX初期化
    directXCommon_ = DirectXCommon::GetInstance();
    directXCommon_->Init(winApp_.get(), width, height);
    directXCommon_->CreateGraphicPipelene();

    // srvManager
    srvManager_ = SrvManager::GetInstance();
    srvManager_->Init(directXCommon_);

    directXCommon_->CreateRnderSrvHandle();

    // TextureManager
    textureManager_ = TextureManager::GetInstance();
    textureManager_->Init(directXCommon_, srvManager_);

    // Object3DCommon
    object3DCommon_ = Object3DCommon::GetInstance();
    object3DCommon_->Init(directXCommon_);

    //
    skyBoxRenderer_ = SkyBoxRenderer::GetInstance();
    skyBoxRenderer_->Init(directXCommon_);

    // FullScreen Renderer
    copyImageRenderer_ = FullscreenRenderer::GetInstance();
    copyImageRenderer_->Init(directXCommon_);

    // ParticleCommon
    particleCommon_ = ParticleCommon::GetInstance();
    particleCommon_->Init(directXCommon_);
    ParticleManager::GetInstance()->Init(srvManager_);

    // SpriteCommon
    spriteCommon_ = SpriteCommon::GetInstance();
    spriteCommon_->Init(directXCommon_);

    // ModelManager
    modelManager_ = ModelManager::GetInstance();
    modelManager_->Initialize(directXCommon_);

    // ImGuiManager
    imguiManager_ = ImGuiManager::GetInstance();
    imguiManager_->Init(winApp_.get(), directXCommon_, srvManager_);

    // Input
    input_ = Input::GetInstance();
    input_->Init(winApp_->GetHInstaice(), winApp_->GetHwnd());

    // Audio
    audio_ = Audio::GetInstance();
    audio_->Init();
}

///=======================================================================
/// 　メッセージが無ければループする
///========================================================================
int EngineCore::ProcessMessage() {
    return winApp_->ProcessMessage();
}

///=======================================================================
/// フレーム開始処理
///========================================================================
void EngineCore::BeginFrame() {
#ifdef _DEBUG
    imguiManager_->Begin(); /// imGui begin
#endif
    input_->Update();
}

///=======================================================================
/// 　描画前処理
///========================================================================
void EngineCore::PreRenderTexture() {
    directXCommon_->PreRenderTexture();
    srvManager_->PreDraw();
}

void EngineCore::PreDraw() {
    directXCommon_->PreDraw();
    directXCommon_->DepthBarrierTransition();
}

///=======================================================================
/// 　フレーム終わり処理
///========================================================================
void EngineCore::EndFrame() {
#ifdef _DEBUG

    imguiManager_->preDrawa();
    imguiManager_->Draw();
#endif

    directXCommon_->PostDraw();
}

///=======================================================================
/// 解放
///========================================================================
void EngineCore::Finalize() {

    CoUninitialize();
    audio_->Finalize();
    textureManager_->Finalize();
    directXCommon_->Finalize();
    modelManager_->Finalize();

#ifdef _DEBUG
    imguiManager_->Finalizer();
#endif
}
