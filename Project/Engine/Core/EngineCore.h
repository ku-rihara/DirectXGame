#pragma once

#include <memory>

class WinApp;
class DirectXCommon;
class ImGuiManager;
class TextureManager;
class SpriteCommon;
class Object3DCommon;
class ModelManager;
class ParticleCommon;
class SrvManager;
class Audio;
class Input;
class FullscreenRenderer;
class SkyBoxRenderer;
class Line3DCommon;

class EngineCore {
public:

    EngineCore()  = default;
    ~EngineCore() = default;


    ///=======================================================================
    ///  public method
    ///========================================================================

    /// システム全体の初期化
     void Initialize(const char* title, int width = 1280, int height = 720);

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
     ImGuiManager* imguiManager_;
     TextureManager* textureManager_;
     SpriteCommon* spriteCommon_;
     Object3DCommon* object3DCommon_;
     ModelManager* modelManager_;
     ParticleCommon* particleCommon_;
     SrvManager* srvManager_;
     Audio* audio_;
     Input* input_;
     SkyBoxRenderer* skyBoxRenderer_;
     FullscreenRenderer* copyImageRenderer_;
     Line3DCommon* line3DCommon_;
};
