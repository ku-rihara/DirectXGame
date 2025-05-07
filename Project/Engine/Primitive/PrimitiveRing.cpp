#include "PrimitiveRing.h"
#include "3d/Mesh.h"
#include<numbers>

void PrimitiveRing::Init() {
    vertexNum_ = 32;
    IPrimitive::Init();
}

void PrimitiveRing::Create() {
  
  /*  const float kOuterRadius = 1.0f;
    const float kInnerRadius = 0.2f;
    const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(vertexNum_);

    for (uint32_t index = 0; index < vertexNum_; ++index) {
        float sin = std::sin(index * radianPerDivide);
        float cos = std::cos(index * radianPerDivide);
        float sinNext = std::sin((index + 1) * radianPerDivide);
        float cosNext = std::cos((index + 1) * radianPerDivide);
        float u       = float(index) / float(vertexNum_);
        float uNext   = float(index + 1) / float(vertexNum_);
    }*/

}
