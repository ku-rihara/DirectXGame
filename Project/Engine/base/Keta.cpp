#include "Keta.h"
/// 2d
#include "2d/ImGuiManager.h"

/// 3d
#include "3d/ModelManager.h"

/// base
#include "TextureManager.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Object3DCommon.h"
#include "SpriteCommon.h"
#include "SrvManager.h"
#include"FullscreenRenderer.h"
#include"SkyBoxRenderer.h"

/// audio,input
#include "audio/Audio.h"
#include "input/Input.h"

/// utility
#include "utility/ParticleEditor/ParticleCommon.h"
#include "utility/ParticleEditor/ParticleManager.h"

/// imgui,function
#include <imgui_impl_dx12.h>
#include "function/Convert.h"

/// std
#include <string>

// 静的メンバ変数の定義
std::unique_ptr<WinApp> Keta::winApp_ = nullptr;
DirectXCommon* Keta::directXCommon_ = nullptr;
ImGuiManager* Keta::imguiManager_ = nullptr;
TextureManager* Keta::textureManager_ = nullptr;
SpriteCommon* Keta::spriteCommon_ = nullptr;
Object3DCommon* Keta::object3DCommon_ = nullptr;
ModelManager* Keta::modelManager_ = nullptr;
ParticleCommon* Keta::particleCommon_ = nullptr;
SrvManager* Keta::srvManager_ = nullptr;
FullscreenRenderer* Keta::copyImageRenderer_ = nullptr;
SkyBoxRenderer* Keta::skyBoxRenderer_        = nullptr;
Audio* Keta::audio_ = nullptr;
Input* Keta::input_ = nullptr;


 ///=======================================================================
 ///初期化
 ///========================================================================
void Keta::Initialize(const char* title, int width, int height) {
    // ゲームウィンドウの作成
    std::string windowTitle = std::string(title);
    auto&& titleString = ConvertString(windowTitle);
    winApp_ = std::make_unique<WinApp>();
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

    //FullScreen Renderer
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
///　メッセージが無ければループする
///========================================================================
int Keta::ProcessMessage() {
    return winApp_->ProcessMessage();
}

///=======================================================================
///フレーム開始処理
///========================================================================
void Keta::BeginFrame() {
#ifdef _DEBUG
    imguiManager_->Begin();  /// imGui begin
#endif
    input_->Update();
}

///=======================================================================
///　描画前処理
///========================================================================
void Keta::PreRenderTexture() {
    directXCommon_->PreRenderTexture();
    srvManager_->PreDraw(); 
}

void Keta::PreDraw() {
    directXCommon_->PreDraw();
    directXCommon_->DepthBarrierTransition();
}

///=======================================================================
///　フレーム終わり処理
///========================================================================
void Keta::EndFrame() {
#ifdef _DEBUG

	imguiManager_->preDrawa();
	imguiManager_->Draw();  
#endif

    directXCommon_->PostDraw();
}

///=======================================================================
///解放
///========================================================================
void Keta::Finalize() {

    CoUninitialize();
    audio_->Finalize();
    textureManager_->Finalize();
    directXCommon_->ReleaseObject();
    modelManager_->Finalize();

#ifdef _DEBUG
    imguiManager_->Finalizer();
#endif

}
