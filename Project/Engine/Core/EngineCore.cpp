#include "EngineCore.h"
/// 2d
#include "2d/ImGuiManager.h"
/// 3d
#include "3d/ModelManager.h"
/// base
#include "base/DsvManager.h"
#include "base/RtvManager.h"
#include "base/SrvManager.h"
#include "base/TextureManager.h"
#include "Dx/DirectXCommon.h"
#include "Lighrt/Light.h"
#include "Pipeline/CSPipelineManager.h"
#include "Pipeline/PipelineManager.h"
#include "PostEffect/PostEffectRenderer.h"
#include "ShadowMap/ShadowMap.h"
/// audio,input
#include "audio/Audio.h"
#include "input/Input.h"
/// particle
#include "GPUParticle/GPUParticleManager.h"
#include "Editor/ParticleEditor/ParticleManager.h"
/// imGui,function
#include "function/Convert.h"
#include <imgui_impl_dx12.h>
/// std
#include <string>

///=======================================================================
/// 初期化
///========================================================================
void EngineCore::Initialize(const char* title, const int& width, const int& height) {
    // ゲームウィンドウの作成
    std::string windowTitle = std::string(title);
    auto&& titleString      = ConvertString(windowTitle);

    winApp_ = std::make_unique<WinApp>();
    winApp_->MakeWindow(titleString.c_str(), width, height);

    // DirectX初期化
    directXCommon_ = DirectXCommon::GetInstance();
    directXCommon_->Init(winApp_.get(), width, height);

    // rtvManager
    rtvManager_ = RtvManager::GetInstance();
    rtvManager_->Init(directXCommon_);

    // srvManager
    srvManager_ = SrvManager::GetInstance();
    srvManager_->Init(directXCommon_);

    dsvManager_ = DsvManager::GetInstance();
    dsvManager_->Init(directXCommon_);

    directXCommon_->InitRenderingResources();

    // TextureManager
    textureManager_ = TextureManager::GetInstance();
    textureManager_->Init(directXCommon_, srvManager_);

    // graphicPipeline
    pipelineManager_ = PipelineManager::GetInstance();
    pipelineManager_->Init(directXCommon_);

    // CsPipeline
    csPipelineManager_ = CSPipelineManager::GetInstance();
    csPipelineManager_->Init(directXCommon_);

    shadowMap_ = ShadowMap::GetInstance();
    shadowMap_->Init(directXCommon_);

    // OffScreen Renderer
    PostEffectRenderer_ = PostEffectRenderer::GetInstance();
    PostEffectRenderer_->Init(directXCommon_);

    // ParticlePipeline
    particleManager_ = ParticleManager::GetInstance();
    particleManager_->Init(srvManager_);

    // GPU Particle
    gpuParticleManager_ = GPUParticleManager::GetInstance();
    gpuParticleManager_->Init(srvManager_);

    // ModelManager
    modelManager_ = ModelManager::GetInstance();
    modelManager_->Initialize(directXCommon_);

    light_ = Light::GetInstance();
    light_->Init(directXCommon_);

    // ImGuiManager
    imGuiManager_ = ImGuiManager::GetInstance();
    imGuiManager_->Init(winApp_.get(), directXCommon_, srvManager_);

    // Input
    input_ = Input::GetInstance();
    input_->Init(winApp_->GetHInstance(), winApp_->GetHwnd());

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
    imGuiManager_->Begin();
#endif
    input_->Update();
    light_->Update();
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

    imGuiManager_->preDraw();
    imGuiManager_->Draw();
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
    imGuiManager_->Finalizer();
#endif
}
