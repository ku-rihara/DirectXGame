#pragma once
#include<vector>
#include"VertexData.h"
#include "materialData.h"
#include"Transform.h"
#include"Matrix4x4.h"

struct Node {
    QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node>cihldren;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	Node rootNode;
};