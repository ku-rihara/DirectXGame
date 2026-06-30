#include"GameBackGroundObject.h"

void GameBackGroundObject::Init(const std::string& filename) {
    BaseBackGroundObject::Init(filename);
}


void GameBackGroundObject::Update(float playSpeed) {
  
    ObjectFromBlender_->EasingUpdateSelectGroup(playSpeed, 0);
}

