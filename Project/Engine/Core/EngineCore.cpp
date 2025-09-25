#include "EngineCore.h"
/// 2d
#include "2d/ImGuiManager.h"
/// 3d
#include "3d/ModelManager.h"
/// base
#include "base/DsvManager.h"
#include "base/RtvManager.h"
#include "base/SkyBoxRenderer.h"
#include "base/SrvManager.h"
#include "base/TextureManager.h"
#include "Dx/DirectXCommon.h"
#include "Lighrt/Light.h"
#include "Pipeline/Line3D/Line3DPipeline.h"
#include "Pipeline/Object3D/Object3DPiprline.h"
#include "Pipeline/Skinning/SkinningObject3DPipeline.h"
#include "Pipeline/Sprite/SpritePipeline.h"
#include "PostEffect/PostEffectRenderer.h"
#include"ShadowMap/ShadowMap.h"
/// audio,input
#include "audio/Audio.h"
#include "input/Input.h"
/// utility
#include "Pipeline/Particle/ParticlePipeline.h"
#include "utility/ParticleEditor/ParticleManager.h"
/// imGui,function
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

    // Object3DPipeline
    Object3DPiprline_ = Object3DPiprline::GetInstance();
    Object3DPiprline_->Init(directXCommon_);

    skinningObject3DPipeline_ = SkinningObject3DPipeline::GetInstance();
    skinningObject3DPipeline_->Init(directXCommon_);

    // skybox
    skyBoxRenderer_ = SkyBoxRenderer::GetInstance();
    skyBoxRenderer_->Init(directXCommon_);

    shadowMap_ = ShadowMap::GetInstance();
    shadowMap_->Init(directXCommon_);

    // OffScreen Renderer
    PostEffectRenderer_ = PostEffectRenderer::GetInstance();
    PostEffectRenderer_->Init(directXCommon_);

    // ParticlePipeline
    ParticlePipeline_ = ParticlePipeline::GetInstance();
    ParticlePipeline_->Init(directXCommon_);
    ParticleManager::GetInstance()->Init(srvManager_);

    // SpritePipeline
    SpritePipeline_ = SpritePipeline::GetInstance();
    SpritePipeline_->Init(directXCommon_);

    Line3DPipeline_ = Line3DPipeline::GetInstance();
    Line3DPipeline_->Init(directXCommon_);

    // ModelManager
    modelManager_ = ModelManager::GetInstance();
    modelManager_->Initialize(directXCommon_);

    light_ = Light::GetInstance();
    light_->Init(directXCommon_);

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
    imguiManager_->Begin();
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

    imguiManager_->preDraw();
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
