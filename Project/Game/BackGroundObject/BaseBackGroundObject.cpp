#include"BaseBackGroundObject.h"

void BaseBackGroundObject::Init(const std::string& filename) {
    ObjectFromBlender_ = std::make_unique<KetaEngine::ObjectFromBlender>();
	ObjectFromBlender_->LoadJsonFile(filename);
    ObjectFromBlender_->EasingAllReset();
}