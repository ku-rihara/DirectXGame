#include"GameBackGroundObject.h"
#include "Frame/Frame.h"

void GameBackGroundObject::Init(const std::string& filename) {
    BaseBackGroundObject::Init(filename);
}


void GameBackGroundObject::Update() {
  
    putObjForBlender_->EasingUpdateSelectGroup(Frame::DeltaTime(), 0);
}

void GameBackGroundObject::Draw(const ViewProjection& viewProjection) {
    putObjForBlender_->DrawAll(viewProjection);
}