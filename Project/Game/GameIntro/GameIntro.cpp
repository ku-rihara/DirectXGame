#include "gameIntro.h"
#include "player/Player.h"
#include "FireInjectors/FireInjectors.h"
#include "BackGroundObject/GameBackGroundObject.h"

void GameIntro::Init() {
}

void GameIntro::Update() {
    (this->*spFuncTable_[static_cast<size_t>(step_)])();
}

void GameIntro::Wait() {
}
void GameIntro::ObjSpawn() {
}
void GameIntro::PlayerSpawn() {
}
void GameIntro::Finish() {
}

/// --------------------------------------------------------------------------------
// メンバ関数のポインタ配列
/// --------------------------------------------------------------------------------
void (GameIntro::* GameIntro::spFuncTable_[])(){
    &GameIntro::Wait,
    &GameIntro::ObjSpawn,
    &GameIntro::PlayerSpawn,
    &GameIntro::Finish,
};
