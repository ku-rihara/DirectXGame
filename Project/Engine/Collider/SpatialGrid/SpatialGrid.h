#pragma once

#include "Line3D/Line3D.h"
#include "Vector3.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace KetaEngine {

class BaseCollider;

/// <summary>
/// 空間分割グリッド
/// </summary>
class SpatialGrid {
public:
    SpatialGrid();
    ~SpatialGrid();

    /// <summary>
    /// グリッドをクリア
    /// </summary>
    void Clear();

    /// <summary>
    /// コライダーをグリッドに登録
    /// </summary>
    /// <param name="collider">登録するコライダー</param>
    void Insert(BaseCollider* collider);

    /// <summary>
    /// 近隣のコライダーを取得
    /// </summary>
    /// <param name="collider">基準となるコライダー</param>
    /// <returns>近隣のコライダーリスト</returns>
    std::vector<BaseCollider*> GetNearbyColliders(BaseCollider* collider);

    /// <summary>
    /// グリッドをデバッグ描画
    /// </summary>
    void DrawGrid();

    /// <summary>
    /// パラメータ設定
    /// </summary>
    void SetParam(float cellSize, const Vector3& worldMin, const Vector3& worldMax);

private:
    /// <summary>
    /// 座標からグリッドインデックスを計算
    /// </summary>
    int GetGridIndex(const Vector3& position) const;

    /// <summary>
    /// 3Dグリッド座標を1Dインデックスに変換
    /// </summary>
    int ToIndex(int x, int y, int z) const;

    /// <summary>
    /// 座標からグリッド座標を計算
    /// </summary>
    void GetGridCoords(const Vector3& position, int& outX, int& outY, int& outZ) const;

private:
    float cellSize_; // セルのサイズ
    Vector3 worldMin_; // ワールド空間の最小座標
    Vector3 worldMax_; // ワールド空間の最大座標
    int gridWidth_; // グリッドの幅
    int gridHeight_; // グリッドの高さ
    int gridDepth_; // グリッドの奥行き

    std::unordered_map<int, std::vector<BaseCollider*>> grid_;

    // デバッグ描画用
    std::unique_ptr<Line3D> debugLine_;
   
public:
 
};

} // namespace KetaEngine