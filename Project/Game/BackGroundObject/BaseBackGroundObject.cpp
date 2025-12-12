#include"BaseBackGroundObject.h"

void BaseBackGroundObject::Init(const std::string& filename) {
    putObjForBlender_ = std::make_unique<KetaEngine::PutObjForBlender>();
	putObjForBlender_->LoadJsonFile(filename);
    putObjForBlender_->EasingAllReset();
}