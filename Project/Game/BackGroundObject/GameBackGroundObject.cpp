#include"GameBackGroundObject.h"
#include "Frame/Frame.h"

void GameBackGroundObject::Init(const std::string& filename) {
    BaseBackGroundObject::Init(filename);
}


void GameBackGroundObject::Update(const float&playSpeed) {
  
    putObjForBlender_->EasingUpdateSelectGroup(playSpeed, 0);
}

void GameBackGroundObject::Draw(const ViewProjection& viewProjection) {
    putObjForBlender_->DrawAll(viewProjection);
}