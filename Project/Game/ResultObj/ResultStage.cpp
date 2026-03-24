#include "ResultStage.h"

void ResultStage::Init(const std::string& filename) {
    objectFromBlender_ = std::make_unique<KetaEngine::ObjectFromBlender>();
    objectFromBlender_->LoadJsonFile(filename);
 
}

void ResultStage::Update() {

}
