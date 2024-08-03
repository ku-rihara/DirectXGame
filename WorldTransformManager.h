#pragma once
#include <vector>
#include "WorldTransform.h"

class WorldTransformManager{
private:
    std::vector<WorldTransform*> worldTransforms_;
public:
    static WorldTransformManager& GetInstance();

    void Register(WorldTransform* worldTransform);
    void Unregister(WorldTransform* worldTransform);
    void UpdateAll();
private:
    WorldTransformManager() = default;
    ~WorldTransformManager() = default;
    WorldTransformManager(const WorldTransformManager&) = delete;
    WorldTransformManager& operator=(const WorldTransformManager&) = delete;

};

