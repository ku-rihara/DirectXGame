#pragma once

#include "ResultUIItem.h"
#include "ResultRankSprite.h"
#include "2d/Sprite.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <array>
#include <memory>
#include <string>

/// <summary>
/// ゲームリザルトUI
/// 3段階のアニメーションで、生き残った時間、敵を倒した数、ランクの順で表示する
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
    const std::string groupName_     = "ResultUI";
    const std::string rankGroupName_ = "RankThreshold";

    ResultUIItem::Config    survivalCfg_; ///<  生き残った時間
    ResultUIItem::Config    killCfg_;     ///<  敵を倒した数
    ResultRankSprite::Config rankCfg_;    ///<  ランク

    int32_t rankScoreForA_; ///< このスコア以上でAランク
    int32_t rankScoreForS_; ///< このスコア以上でSランク

    std::array<ResultUIItem, 2> items_;
    ResultRankSprite            rankSprite_;
    int32_t currentItem_ = 0;

    std::unique_ptr<KetaEngine::Sprite> sprite_;
};
