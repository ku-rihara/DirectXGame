#pragma once
#include<vector>
#include"VertexData.h"
#include "materialData.h"
#include"Transform.h"
#include"Matrix4x4.h"
#include <cstdint>

struct Node {
    QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node>cihldren;
};

struct ModelData {
	std::vector<VertexData> vertices;
    std::vector<int32_t> indices;
	MaterialData material;
	Node rootNode;
};