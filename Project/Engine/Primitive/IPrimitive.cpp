#include"IPrimitive.h"
#include"3d/Mesh.h"
#include"base/DirectXCommon.h"

void IPrimitive::Init() {

    mesh_ = std::make_unique<Mesh>();
    mesh_->Init(DirectXCommon::GetInstance(), vertexNum_);
}