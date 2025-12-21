#include "SpatialGrid.h"
#include "Collider/AABBCollider.h"
#include "Collider/BaseCollider.h"
#include "Collider/OBBCollider.h"
#include <algorithm>
#include <cmath>

using namespace KetaEngine;

void SpatialGrid::Clear() {
    grid_.clear();
}

void SpatialGrid::Insert(BaseCollider* collider) {
    if (!collider)
        return;

    Vector3 pos  = collider->GetCollisionPos();
    Vector3 size = {1.0f, 1.0f, 1.0f};

    // AABBまたはOBBの場合、実際のサイズを取得
    if (auto* aabb = dynamic_cast<AABBCollider*>(collider)) {
        size = aabb->GetCollisonScale();
    } else if (auto* obb = dynamic_cast<OBBCollider*>(collider)) {
        size = obb->GetOBB().size;
    }

    // コライダーが占有する範囲を計算
    Vector3 min = pos - size;
    Vector3 max = pos + size;

    int minX, minY, minZ, maxX, maxY, maxZ;
    GetGridCoords(min, minX, minY, minZ);
    GetGridCoords(max, maxX, maxY, maxZ);

    // 範囲内の全てのセルに登録
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            for (int z = minZ; z <= maxZ; ++z) {
                int index = ToIndex(x, y, z);
                grid_[index].push_back(collider);
            }
        }
    }
}

std::vector<BaseCollider*> SpatialGrid::GetNearbyColliders(BaseCollider* collider) {
    if (!collider)
        return {};

    std::vector<BaseCollider*> nearby;
    Vector3 pos = collider->GetCollisionPos();

    // 周囲9x9x9セル
    int centerX, centerY, centerZ;
    GetGridCoords(pos, centerX, centerY, centerZ);

    // 重複を避けるためのセット
    std::unordered_map<BaseCollider*, bool> added;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                int x = centerX + dx;
                int y = centerY + dy;
                int z = centerZ + dz;

                // グリッド範囲外チェック
                if (x < 0 || x >= gridWidth_ || y < 0 || y >= gridHeight_ || z < 0 || z >= gridDepth_) {
                    continue;
                }

                int index = ToIndex(x, y, z);
                auto it   = grid_.find(index);
                if (it != grid_.end()) {
                    for (BaseCollider* other : it->second) {
                        // 自分自身は除外
                        if (other != collider && !added[other]) {
                            nearby.push_back(other);
                            added[other] = true;
                        }
                    }
                }
            }
        }
    }

    return nearby;
}

int SpatialGrid::GetGridIndex(const Vector3& position) const {
    int x, y, z;
    GetGridCoords(position, x, y, z);
    return ToIndex(x, y, z);
}

void SpatialGrid::GetGridCoords(const Vector3& position, int& outX, int& outY, int& outZ) const {
    Vector3 localPos = position - worldMin_;

    outX = static_cast<int>(localPos.x / cellSize_);
    outY = static_cast<int>(localPos.y / cellSize_);
    outZ = static_cast<int>(localPos.z / cellSize_);

    // 範囲内にクランプ
    outX = (std::max)(0, (std::min)(outX, gridWidth_ - 1));
    outY = (std::max)(0, (std::min)(outY, gridHeight_ - 1));
    outZ = (std::max)(0, (std::min)(outZ, gridDepth_ - 1));
}

int SpatialGrid::ToIndex(int x, int y, int z) const {
    return x + y * gridWidth_ + z * gridWidth_ * gridHeight_;
}

void SpatialGrid::SetParam(float cellSize, const Vector3& worldMin, const Vector3& worldMax) {

    cellSize_ = cellSize;
    worldMin_ = worldMin;
    worldMax_ = worldMax;

    // グリッドの分割数を計算
    Vector3 worldSize = worldMax_ - worldMin_;

    if (cellSize_ == 0.0f) {
        return;
    }

    gridWidth_  = static_cast<int>(std::ceil(worldSize.x / cellSize_));
    gridHeight_ = static_cast<int>(std::ceil(worldSize.y / cellSize_));
    gridDepth_  = static_cast<int>(std::ceil(worldSize.z / cellSize_));
}