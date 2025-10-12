#pragma once

#include <memory>
#include "base/WinApp.h"
class DirectXCommon;
class WinApp;
class ImGuiManager;
class TextureManager;
class ModelManager;
class SrvManager;
class Audio;
class Input;
class PostEffectRenderer;
class RtvManager;
class Light;
class DsvManager;
class ShadowMap;
class PipelineManager;
class CSPipelineManager;
class ParticleManager;
class GPUParticleManager;

class EngineCore {
public:

    EngineCore()  = default;
    ~EngineCore() = default;


    ///=======================================================================
    ///  public method
    ///========================================================================

    /// システム全体の初期化
    void Initialize(const char* title, const int& width = 1280, const int& height = 720);

    // Windowsのメッセージを処理
     int ProcessMessage();

    // フレーム開始処理
     void BeginFrame();

     void PreRenderTexture();
     void PreDraw();

    // フレーム終了処理
     void EndFrame();

    /// システム全体の終了
     void Finalize();

     private:
     ///=======================================================================
     /// static private variant
     ///=======================================================================
     std::unique_ptr<WinApp> winApp_;
     DirectXCommon* directXCommon_;
     ImGuiManager* imGuiManager_;
     TextureManager* textureManager_;
     ModelManager* modelManager_;
     SrvManager* srvManager_;
     Audio* audio_;
     Input* input_;
     RtvManager* rtvManager_;
     PostEffectRenderer* PostEffectRenderer_;
     Light* light_;
     DsvManager* dsvManager_;
     ShadowMap* shadowMap_;
     PipelineManager* pipelineManager_;
     CSPipelineManager* csPipelineManager_;
     ParticleManager* particleManager_;
     GPUParticleManager* gpuParticleManager_;
};
