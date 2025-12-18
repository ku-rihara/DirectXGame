#include "SideRopeController.h"

void SideRopeController::Init() {

    for (auto& sideRope : sideRopes_) {
        sideRope = std::make_unique<SideRope>();
        sideRope->Init();
    }
}

void SideRopeController::Update() {
}
