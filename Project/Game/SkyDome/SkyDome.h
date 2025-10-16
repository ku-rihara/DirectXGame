#pragma once
#include "3d/Object3d.h"
#include "3d/WorldTransform.h"

class SkyDome {
public:
    SkyDome();
    ~SkyDome();
    void Init();
    void Update();
    void Draw(ViewProjection& viewProjection);

private:
	// ワールド変換データ
	WorldTransform transform_;
	// モデル
	std::unique_ptr<Object3d> obj3D_ = nullptr;
};
