#pragma once

#include<memory>

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

class Keta {

private:
  
    ///=======================================================================
    /// static private variant
    ///=======================================================================
    static std::unique_ptr<WinApp> winApp_;
    static DirectXCommon* directXCommon_;
    static ImGuiManager* imguiManager_;
    static TextureManager* textureManager_;
    static SpriteCommon* spriteCommon_;
    static Object3DCommon* object3DCommon_;
    static ModelManager* modelManager_;
    static ParticleCommon* particleCommon_;
    static SrvManager* srvManager_;
    static Audio* audio_;
    static Input* input_;
	static FullscreenRenderer* copyImageRenderer_;

public:

    ///=======================================================================
   /// static public method
   ///========================================================================

    /// システム全体の初期化
    static void Initialize(const char* title, int width = 1280, int height = 720);

    // Windowsのメッセージを処理
    static int ProcessMessage();

    // フレーム開始処理
    static void BeginFrame();

    static void PreRenderTexture();
    static void PreDraw();

    // フレーム終了処理
    static void EndFrame();

    /// システム全体の終了
    static void Finalize();


};
