#include"BaseBackGroundObject.h"

void BaseBackGroundObject::Init(const std::string& filename) {
	putObjForBlender_ = std::make_unique<PutObjForBlender>();
	putObjForBlender_->LoadJsonFile(filename);
	putObjForBlender_->PutObject();
}