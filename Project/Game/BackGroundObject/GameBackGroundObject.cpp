#include"GameBackGroundObject.h"
#include "Frame/Frame.h"

void GameBackGroundObject::Init(const std::string& filename) {
    BaseBackGroundObject::Init(filename);
    ObjectFromBlender_->EasingUpdateSelectGroup(0.0f, 0);
    ObjectFromBlender_->UpdateTransform();
}


void GameBackGroundObject::Update(float playSpeed) {
  
    ObjectFromBlender_->EasingUpdateSelectGroup(playSpeed, 0);
}

