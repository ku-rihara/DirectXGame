#pragma once
#include "Player/Player.h"
#include "Player/Behavior/TitleBehavior/BaseTitleBehavior.h"
#include <memory>

class TitlePlayer : public Player {
public:
    TitlePlayer()  = default;
    ~TitlePlayer() = default;

    void TitleUpdate();
    void SetTitleBehavior();

    BaseTitleBehavior* GetTitleBehavior() const { return titleBehavior_.get(); }

private:
    std::unique_ptr<BaseTitleBehavior> titleBehavior_;
};
