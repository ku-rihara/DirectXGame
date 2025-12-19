#include "SideRopeController.h"

void SideRopeController::Init() {

    // 生成と初期化
    for (int32_t i = 0; i < static_cast<int32_t>(SideRopeType::COUNT); ++i) {
        sideRopes_[i] = std::make_unique<SideRope>();
        sideRopes_[i]->Init(i);
    }
}

void SideRopeController::Update() {

    for (auto& sideRope : sideRopes_) {
        sideRope->Update();
    }
}

void SideRopeController::AdjustParam() {
    for (auto& sideRope : sideRopes_) {
        sideRope->AdjustParam();
    }
 }