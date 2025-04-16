#include "PrimitivePlane.h"
#include"3d/Mesh.h"


void PrimitivePlane::Init() {
    vertexNum_ = 4;
    IPrimitive::Init();
}

void PrimitivePlane::Create() {
    mesh_->SetVertexPositionData(0, Vector4());
}


