#pragma once
#include<vector>
#include"VertexData.h"
#include "materialData.h"
#include"Transform.h"
#include"Matrix4x4.h"
#include <cstdint>
#include <map>

namespace KetaEngine {

struct Node {
    QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node>children;
};

struct VertexWeightData {
    float weight;
    uint32_t vertexIndex;
};

struct JointWeightData {
    Matrix4x4 inverseBindPoseMatrix;
    std::vector<VertexWeightData> vertexWeights;
};

struct ModelData {
    std::map<std::string, JointWeightData> skinClusterData;
	std::vector<VertexData> vertices;
    std::vector<int32_t> indices;
	MaterialData material;
	Node rootNode;
};

}; // KetaEngine
