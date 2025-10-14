#pragma once

#include "3d/WorldTransform.h"

// data
#include "SkeletonData.h"
#include "SkinCluster.h"
#include "AnimationData.h"
#include "struct/ModelData.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class Model;
class ModelAnimation {
public:
    ModelAnimation()  = default;
    ~ModelAnimation() = default;

    /// アニメーションファイル読み込み
    Animation LoadAnimationFile(const std::string& fileName);

    /// スケルトン作成
    Skeleton CreateSkeleton(const Node& rootNode);

    /// ジョイント作成
    int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

    /// スキンクラスター作成
    SkinCluster CreateSkinCluster(ModelData& modelData, const Skeleton& skeleton, Model* model);

private:
    // 各Resource作成
    void CreatePaletteResource(SkinCluster& skinCluster, const Skeleton& skeleton);
    void CreateInputVertexSRV(SkinCluster& skinCluster, ModelData& modelData, Model* model);
    void CreateInfluenceResource(SkinCluster& skinCluster, ModelData& modelData);
    void CreateOutputVertexResourceAndUAV(SkinCluster& skinCluster, ModelData& modelData);
    void CreateSkinningInfoResource(SkinCluster& skinCluster, ModelData& modelData);

private:
    /// ============================================================
    /// private members
    /// ============================================================

    const std::string directoryPath_ = "Resources/Model/";

public:
    /// キーフレーム補間計算
    static Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe,const float& time);
    static Quaternion CalculateValueQuaternion(const std::vector<KeyframeQuaternion>& keyframe, const float& time);
};