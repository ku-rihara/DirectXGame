#include"StadiumLightEffect.h"
#include "Frame/Frame.h"

void StadiumLightEffect::Init(const std::string& filename) {
    BaseBackGroundObject::Init(filename);
}


void StadiumLightEffect::Update(const float& playSpeed) {
  
   BaseComboLevelBackObj::Update(playSpeed);
}

void StadiumLightEffect::Draw(const ViewProjection& viewProjection) {
    putObjForBlender_->DrawAll(viewProjection);
}