#pragma once

#include "ResultUIItem.h"
#include "2d/Sprite.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// ゲームリザルトUI
/// 「生き残った時間」→「敵を倒した数」→「到達したレベル」を順番にアニメ表示
/// </summary>
class GameResultUI {
public:
    GameResultUI()  = default;
    ~GameResultUI() = default;

    void Init();
    void Update();
    void AdjustParam();
    void RegisterParams();

private:
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_ = "ResultUI";

    ResultUIItem::Config survivalCfg_; ///< ① 生き残った時間
    ResultUIItem::Config killCfg_;     ///< ② 敵を倒した数
    ResultUIItem::Config levelCfg_;    ///< ③ 到達したレベル

    std::array<ResultUIItem, 3> items_;
    int32_t currentItem_ = 0;

    std::unique_ptr<KetaEngine::Sprite> sprite_;
};
