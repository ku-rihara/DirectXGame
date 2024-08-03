#include "WorldTransformManager.h"

WorldTransformManager&WorldTransformManager::GetInstance() {
	static WorldTransformManager instance;
	return instance;
}

void WorldTransformManager::Register(WorldTransform* worldTransform) {
	worldTransforms_.push_back(worldTransform);
}

void WorldTransformManager::Unregister(WorldTransform* worldTransform) {
	worldTransforms_.erase(std::remove(worldTransforms_.begin(),worldTransforms_.end(),worldTransform),worldTransforms_.end());
}

void WorldTransformManager::UpdateAll() {
	for (auto* worldtransform : worldTransforms_) {
		worldtransform->UpdateMatrix();
	}
}