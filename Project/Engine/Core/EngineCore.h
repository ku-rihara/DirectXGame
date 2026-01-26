#pragma once

#include "Base/WinApp.h"
#include <memory>
namespace KetaEngine {

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

/// <summary>
///  エンジンコアクラス
/// </summary>
class EngineCore {
public:
    EngineCore()  = default;
    ~EngineCore() = default;

    ///=======================================================================
    ///  public method
    ///========================================================================

    /// <summary>
    /// システム全体の初期化
    /// </summary>
    /// <param name="title">ウィンドウタイトル</param>
    /// <param name="width">ウィンドウ幅</param>
    /// <param name="height">ウィンドウ高さ</param>
    void Initialize(const char* title, int width, int height);

    int ProcessMessage();     //< Windowsのメッセージを処理
    void BeginFrame();        //< フレーム開始処理
    void PreRenderTexture();  //< レンダーテクスチャ描画前処理
    void PreDraw();           //< 描画前処理
    void EndFrame();          //< フレーム終了処理
    void Finalize();          //< システム全体の終了

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

}; // KetaEngine
