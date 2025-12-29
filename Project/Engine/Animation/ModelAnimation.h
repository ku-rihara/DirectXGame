#pragma once

#include "3d/WorldTransform.h"

// data
#include "SkeletonData.h"
#include "SkinCluster.h"
#include "AnimationData.h"
#include "struct/ModelData.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace KetaEngine {

class Model;

/// <summary>
/// モデルアニメーション管理クラス
/// </summary>
class ModelAnimation {
public:
    ModelAnimation()  = default;
    ~ModelAnimation() = default;

    /// <summary>
    /// アニメーションファイル読み込み
    /// </summary>
    /// <param name="fileName">ファイル名</param>
    /// <returns>読み込んだアニメーションデータ</returns>
    Animation LoadAnimationFile(const std::string& fileName);

    /// <summary>
    /// スケルトン作成
    /// </summary>
    /// <param name="rootNode">ルートノード</param>
    /// <returns>作成されたスケルトン</returns>
    Skeleton CreateSkeleton(const Node& rootNode);

    /// <summary>
    /// ジョイント作成
    /// </summary>
    /// <param name="node">ノード</param>
    /// <param name="parent">親のインデックス</param>
    /// <param name="joints">ジョイント配列</param>
    /// <returns>作成されたジョイントのインデックス</returns>
    int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

    /// <summary>
    /// スキンクラスター作成
    /// </summary>
    /// <param name="modelData">モデルデータ</param>
    /// <param name="skeleton">スケルトン</param>
    /// <param name="model">モデル</param>
    /// <returns>作成されたスキンクラスター</returns>
    SkinCluster CreateSkinCluster(ModelData& modelData, const Skeleton& skeleton, Model* model);

    /// <summary>
    /// キーフレーム補間計算(Vector3)
    /// </summary>
    /// <param name="keyframe">キーフレーム配列</param>
    /// <param name="time">時間</param>
    /// <returns>補間された値</returns>
    static Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time);

    /// <summary>
    /// キーフレーム補間計算(Quaternion)
    /// </summary>
    /// <param name="keyframe">キーフレーム配列</param>
    /// <param name="time">時間</param>
    /// <returns>補間された値</returns>
    static Quaternion CalculateValueQuaternion(const std::vector<KeyframeQuaternion>& keyframe, float time);

private:
    /// <summary>
    /// パレットリソース作成
    /// </summary>
    /// <param name="skinCluster">スキンクラスター</param>
    /// <param name="skeleton">スケルトン</param>
    void CreatePaletteResource(SkinCluster& skinCluster, const Skeleton& skeleton);

    /// <summary>
    /// 入力頂点SRV作成
    /// </summary>
    /// <param name="skinCluster">スキンクラスター</param>
    /// <param name="modelData">モデルデータ</param>
    /// <param name="model">モデル</param>
    void CreateInputVertexSRV(SkinCluster& skinCluster, ModelData& modelData, Model* model);

    /// <summary>
    /// インフルエンスリソース作成
    /// </summary>
    /// <param name="skinCluster">スキンクラスター</param>
    /// <param name="modelData">モデルデータ</param>
    void CreateInfluenceResource(SkinCluster& skinCluster, ModelData& modelData);

    /// <summary>
    /// 出力頂点リソースとUAV作成
    /// </summary>
    /// <param name="skinCluster">スキンクラスター</param>
    /// <param name="modelData">モデルデータ</param>
    void CreateOutputVertexResourceAndUAV(SkinCluster& skinCluster, ModelData& modelData);

    /// <summary>
    /// スキニング情報リソース作成
    /// </summary>
    /// <param name="skinCluster">スキンクラスター</param>
    /// <param name="modelData">モデルデータ</param>
    void CreateSkinningInfoResource(SkinCluster& skinCluster, ModelData& modelData);

private:
    /// ============================================================
    /// private members
    /// ============================================================

    const std::string directoryPath_ = "Resources/Model/";
};

}; // KetaEngine
