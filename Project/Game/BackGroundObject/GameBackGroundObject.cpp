#include"GameBackGroundObject.h"

void GameBackGroundObject::Init(const std::string& filename) {
    BaseBackGroundObject::Init(filename);
}


void GameBackGroundObject::Update() {
  
}

void GameBackGroundObject::Draw(const ViewProjection& viewProjection) {
    putObjForBlender_->DrawAll(viewProjection);
}