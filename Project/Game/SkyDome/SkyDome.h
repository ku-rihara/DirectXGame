#pragma once
#include "3D/Object3d/Object3d.h"
#include "3D/WorldTransform.h"

class SkyDome {
public:
    SkyDome();
    ~SkyDome();
    void Init();
    void Update();
  
private:
	// ワールド変換データ
    KetaEngine::WorldTransform transform_;
	// モデル
    std::unique_ptr<KetaEngine::Object3d> obj3D_ = nullptr;
};
