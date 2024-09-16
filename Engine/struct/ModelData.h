#pragma once
#include<vector>
#include"VertexData.h"
#include "materialData.h"
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};