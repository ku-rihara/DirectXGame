#pragma once

#include "BaseScene.h"
#include "2d/Sprite.h"
#include "KillCounter/KillCountUIController.h"
#include "UI/LevelUI/LevelUIController.h"

#include <memory>

/// <summary>
/// ゲームリザルトシーン
/// クリア情報（最大コンボ数など）を表示する
/// </summary>
class GameResultScene : public BaseScene {
public:
    GameResultScene()           = default;
    ~GameResultScene() override = default;

    void Init() override;
    void Update() override;
    void SkyBoxDraw() override {}
    void Debug() override;
    void ViewProjectionUpdate() override;
    void ViewProcess() override {}

private:
    void CheckEndInput();

private:
    std::unique_ptr<KetaEngine::Sprite> bgSprite_;

    std::unique_ptr<KillCountUIController> comboCountUI_;   // 最大コンボ数
    std::unique_ptr<KillCountUIController> killCountUI_;    // 総キル数
    LevelUIController levelUI_;                             // 到達レベル

    float alpha_         = 0.0f;
    bool isStartFadeOut_ = false;
};
