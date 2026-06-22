#pragma once
// Player
#include "Player/Player.h"
// Behavior
#include "Player/Behavior/TitleBehavior/BaseTitleBehavior.h"
// std
#include <memory>

class TitlePlayer : public Player {
public:
    TitlePlayer()  = default;
    ~TitlePlayer() = default;

    void TitleUpdate();

private:
    std::unique_ptr<BaseTitleBehavior> titleBehavior_;

public:
    BaseTitleBehavior* GetTitleBehavior() const { return titleBehavior_.get(); }

    void SetTitleBehavior();
};
