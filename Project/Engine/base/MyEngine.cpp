#include "MyEngine.h"
/// 2d
#include "2d/ImGuiManager.h"

/// 3d
#include "3d/ModelManager.h"

/// base
#include "TextureManager.h"
#include "WinApp.h"
#include"Dx/DirectXCommon.h"
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
std::unique_ptr<WinApp> MyEngine::winApp_ = nullptr;
DirectXCommon* MyEngine::directXCommon_ = nullptr;
ImGuiManager* MyEngine::imguiManager_ = nullptr;
TextureManager* MyEngine::textureManager_ = nullptr;
SpriteCommon* MyEngine::spriteCommon_ = nullptr;
Object3DCommon* MyEngine::object3DCommon_ = nullptr;
ModelManager* MyEngine::modelManager_ = nullptr;
ParticleCommon* MyEngine::particleCommon_ = nullptr;
SrvManager* MyEngine::srvManager_ = nullptr;
FullscreenRenderer* MyEngine::copyImageRenderer_ = nullptr;
SkyBoxRenderer* MyEngine::skyBoxRenderer_        = nullptr;
Audio* MyEngine::audio_ = nullptr;
Input* MyEngine::input_ = nullptr;


 ///=======================================================================
 ///初期化
 ///========================================================================
void MyEngine::Initialize(const char* title, int width, int height) {
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

     // ImGuiManager
    imguiManager_ = ImGuiManager::GetInstance();
    imguiManager_->Init(winApp_.get(), directXCommon_, srvManager_);


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
int MyEngine::ProcessMessage() {
    return winApp_->ProcessMessage();
}

///=======================================================================
///フレーム開始処理
///========================================================================
void MyEngine::BeginFrame() {
#ifdef _DEBUG
    imguiManager_->Begin();  /// imGui begin
#endif
    input_->Update();
}

///=======================================================================
///　描画前処理
///========================================================================
void MyEngine::PreRenderTexture() {
    directXCommon_->PreRenderTexture();
    srvManager_->PreDraw(); 
}

void MyEngine::PreDraw() {
    directXCommon_->PreDraw();
    directXCommon_->DepthBarrierTransition();
}

///=======================================================================
///　フレーム終わり処理
///========================================================================
void MyEngine::EndFrame() {
#ifdef _DEBUG

	imguiManager_->preDrawa();
	imguiManager_->Draw();  
#endif

    directXCommon_->PostDraw();
}

///=======================================================================
///解放
///========================================================================
void MyEngine::Finalize() {

    CoUninitialize();
    audio_->Finalize();
    textureManager_->Finalize();
    directXCommon_->Finalize();
    modelManager_->Finalize();

#ifdef _DEBUG
    imguiManager_->Finalizer();
#endif

}
