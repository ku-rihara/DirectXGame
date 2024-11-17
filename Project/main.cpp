#include <Scene/GameScene.h>
#include <Scene/TitleScene.h>
#include "GrobalParamater/GlobalParameter.h"
#include "Frame/Frame.h"
#include <wrl.h>
#include "Colider/CollisionManager.h"
#include <memory> // std::unique_ptr用

const char kWindowTitle[] = "LE2A_11_クリハラ_ケイタ_CG3";
enum class Scene {
    kUnknown = 0,
    kTitle,
    kGame,
};
Scene scene = Scene::kUnknown;

// ゲームシーンとタイトルシーンをstd::unique_ptrで管理
std::unique_ptr<GameScene> gameScene = nullptr;
std::unique_ptr<TitleScene> titleScene = nullptr;

void ChangeScene();
void UpdateScene();
void DrawScene();

// ウィンドウアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Keta::Initialize(kWindowTitle, 1280, 720);

    // グローバル変数の読み込み
    GlobalParameter::GetInstance()->LoadFiles();

    /// コリジョンマネージャー
    CollisionManager* collisionManager = new CollisionManager();
    collisionManager->Init();

    /// 時間の初期化
    Frame::Init();

    // 最初のシーン初期化
    scene = Scene::kTitle;
    titleScene = std::make_unique<TitleScene>();
    titleScene->Init();

    gameScene = std::make_unique<GameScene>();
    gameScene->Init();

    // ウィンドウのxボタンが押されるまでループ
    while (Keta::ProcessMessage() == 0) {
        /// フレームの開始
        Keta::BeginFrame();
        /// ===================================================
        /// 更新
        /// ===================================================
        /// deltaTimeの更新
        Frame::Update();
        /// グローバル変数の更新
        GlobalParameter::GetInstance()->Update();

        /// 当たり判定
        collisionManager->Update();

        // シーン切り替え
        ChangeScene();
        /// ゲームシーンの毎フレーム処理
        UpdateScene();

        /// ===================================================
        /// 描画
        /// ===================================================
        DrawScene();
        /// フレームの終了
        Keta::EndFrame();
    }

    delete collisionManager;
    // ライブラリの終了
    Keta::Finalize();
    return 0;
}

void ChangeScene() {
    switch (scene) {
    case Scene::kTitle:
        if (titleScene->GetIsFinished()) {
            // 旧シーンのリソース解放はunique_ptrが自動で行う
            titleScene.reset();

            // シーン変更
            scene = Scene::kGame;

            // 新シーンの生成と初期化
            gameScene = std::make_unique<GameScene>();
            gameScene->Init();
        }
        break;
    case Scene::kGame:
        if (gameScene->GetIsFinished()) {
            // 旧シーンのリソース解放はunique_ptrが自動で行う
            gameScene.reset();

            // シーン変更
            scene = Scene::kTitle;

            // 新シーンの生成と初期化
            titleScene = std::make_unique<TitleScene>();
            titleScene->Init();
        }
        break;
    }
}

void UpdateScene() {
    switch (scene) {
    case Scene::kTitle:
        titleScene->Update();
        break;
    case Scene::kGame:
        gameScene->Update();
        break;
    }
}

void DrawScene() {
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = DirectXCommon::GetInstance()->GetCommandList();

    switch (scene) {
    case Scene::kTitle:
        ///*******
        /// モデル
        ///*******
        Model::PreDraw(commandList.Get());
        /// タイトルシーン描画
        titleScene->ModelDraw();
        ///*******
        /// パーティクル
        ///*******
        Model::PreDrawParticle(commandList.Get());
        titleScene->ParticleDraw();
        ///*******
        /// スプライト
        ///*******
        Sprite::PreDraw(commandList.Get());
        titleScene->SpriteDraw();
        break;
    case Scene::kGame:
        ///*******
        /// モデル
        ///*******
        Model::PreDraw(commandList.Get());
        /// ゲームシーン描画
        gameScene->ModelDraw();
        ///*******
        /// パーティクル
        ///*******
        Model::PreDrawParticle(commandList.Get());
        gameScene->ParticleDraw();
        ///*******
        /// スプライト
        ///*******
        Sprite::PreDraw(commandList.Get());
        gameScene->SpriteDraw();
        break;
    }
}
