#pragma once
#include "Primitive/PrimitiveBox.h"
#include "3d/WorldTransform.h"
#include <memory>
class SkyBox {
public:

private:
	// ワールド変換データ
	WorldTransform transform_;
	// モデル
    std::unique_ptr<PrimitiveBox> primitiveBox_ = nullptr;

	
public:
    SkyBox();
    ~SkyBox();
	void Init();
	void Update();
	void Draw(ViewProjection& viewProjection);

	void Debug();
};
