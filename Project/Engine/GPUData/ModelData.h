#pragma once
#include<vector>
#include"VertexData.h"
#include "materialData.h"
#include"Transform.h"
#include"Matrix4x4.h"
#include <cstdint>
#include <map>

namespace KetaEngine {

/// <summary>
/// モデルの階層構造を表すノード
/// </summary>
struct Node {
    QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node>children;
};

/// <summary>
/// 1頂点に対するスキニングウェイト情報
/// </summary>
struct VertexWeightData {
    float weight;
    uint32_t vertexIndex;
};

/// <summary>
/// 1ジョイントが持つバインドポーズ逆行列と、影響を受ける頂点ウェイト一覧
/// </summary>
struct JointWeightData {
    Matrix4x4 inverseBindPoseMatrix;
    std::vector<VertexWeightData> vertexWeights;
};

/// <summary>
/// 読み込んだモデルの頂点・インデックス・マテリアル・スキニング・ノード階層をまとめて保持する構造体
/// </summary>
struct ModelData {
    std::map<std::string, JointWeightData> skinClusterData;
	std::vector<VertexData> vertices;
    std::vector<int32_t> indices;
	MaterialData material;
	Node rootNode;
};

}; // KetaEngine
