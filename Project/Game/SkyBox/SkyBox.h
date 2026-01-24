#pragma once
#include "3D/Primitive/PrimitiveBox.h"
#include "3d/WorldTransform.h"
#include <memory>
class SkyBox {
public:

private:
	// ワールド変換データ
    KetaEngine::WorldTransform transform_;
	// モデル
    std::unique_ptr<KetaEngine::PrimitiveBox> primitiveBox_ = nullptr;

	
public:
    SkyBox();
    ~SkyBox();
	void Init();
	void Update();
    void Draw(KetaEngine::ViewProjection& viewProjection);

	void Debug();
};
