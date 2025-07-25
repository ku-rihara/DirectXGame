#include"Box.h"
#include<algorithm>

//当たり判定
bool IsCollision(const AABB& aabb, const Vector3& point) {
    return (point.x >= aabb.min.x && point.x <= aabb.max.x) &&
        (point.y >= aabb.min.y && point.y <= aabb.max.y) &&
        (point.z >= aabb.min.z && point.z <= aabb.max.z);
}

// AABBとAABBの当たり判定
bool IsCollision(const AABB& aabbA, const AABB& aabbB) {
    // 各軸での判定
    return (aabbA.min.x <= aabbB.max.x && aabbA.max.x >= aabbB.min.x) &&
        (aabbA.min.y <= aabbB.max.y && aabbA.max.y >= aabbB.min.y) &&
        (aabbA.min.z <= aabbB.max.z && aabbA.max.z >= aabbB.min.z);
}

bool IsCollision(const OBB& obb1, const OBB& obb2) {
	Vector3 faceNormalAndCross[15];//面法線6本とクロス積9本
	int crossIndex = 6;//クロス積の配列は６からスタート
	for (int i = 0; i < 3; i++) {
		faceNormalAndCross[i] = obb1.orientations[i];
		faceNormalAndCross[i + 3] = obb2.orientations[i];
	}
	//クロス積
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			faceNormalAndCross[crossIndex] = Vector3::Cross(obb1.orientations[i], obb2.orientations[j]);
			if (crossIndex < 14) {
				crossIndex++;
			}
		}
	}
	for (int i = 0; i < 15; i++) {
		faceNormalAndCross[i] = Vector3::Normalize(faceNormalAndCross[i]);
		//全ての頂点を軸に対して射影する
		float projection1 = Vector3::Dot(obb1.center, faceNormalAndCross[i]);
		float projection2 = Vector3::Dot(obb2.center, faceNormalAndCross[i]);

		float halfSize1 = obb1.size.x * std::abs(Vector3::Dot(obb1.orientations[0], faceNormalAndCross[i])) +
			obb1.size.y * std::abs(Vector3::Dot(obb1.orientations[1], faceNormalAndCross[i])) +
			obb1.size.z * std::abs(Vector3::Dot(obb1.orientations[2], faceNormalAndCross[i]));

		float halfSize2 = obb2.size.x * std::abs(Vector3::Dot(obb2.orientations[0], faceNormalAndCross[i])) +
			obb2.size.y * std::abs(Vector3::Dot(obb2.orientations[1], faceNormalAndCross[i])) +
			obb2.size.z * std::abs(Vector3::Dot(obb2.orientations[2], faceNormalAndCross[i]));

		//射影した点のmax,minを求める
		float min1 = projection1 - halfSize1;
		float min2 = projection2 - halfSize2;
		float max1 = projection1 + halfSize1;
		float max2 = projection2 + halfSize2;
		float L1 = max1 - min1;
		float L2 = max2 - min2;
		float sumSpan = L1 + L2;//影の長さの合計
		float longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);//2つの影の両端の差分
		if (sumSpan < longSpan) {
			return false;
		}
	}

	return true;
}

// OBBとAABBの当たり判定
bool IsCollision(const OBB& obb, const AABB& aabb) {
    Vector3 faceNormalAndCross[15]; 
    int crossIndex = 6;             

    // OBBの軸
    for (int i = 0; i < 3; i++) {
        faceNormalAndCross[i] = obb.orientations[i];
    }
    // AABBの軸
    faceNormalAndCross[3] = Vector3(1, 0, 0);  // X軸
    faceNormalAndCross[4] = Vector3(0, 1, 0);  // Y軸
    faceNormalAndCross[5] = Vector3(0, 0, 1);  // Z軸

    // 各軸間のクロス積
    for (int i = 0; i < 3; i++) {
        for (int j = 3; j < 6; j++) {
            faceNormalAndCross[crossIndex] = Vector3::Cross(obb.orientations[i], faceNormalAndCross[j]);
            crossIndex++;
        }
    }

    // 各軸について投影し、衝突判定を行う
    for (int i = 0; i < 15; i++) {
        faceNormalAndCross[i] = Vector3::Normalize(faceNormalAndCross[i]);

        // AABBの投影
        float aabbProjection = 0.5f * (
            std::abs(Vector3::Dot(faceNormalAndCross[i], Vector3(1, 0, 0)) * (aabb.max.x - aabb.min.x)) +
            std::abs(Vector3::Dot(faceNormalAndCross[i], Vector3(0, 1, 0)) * (aabb.max.y - aabb.min.y)) +
            std::abs(Vector3::Dot(faceNormalAndCross[i], Vector3(0, 0, 1)) * (aabb.max.z - aabb.min.z))
            );

        // OBBの投影
        float obbProjection =
            obb.size.x * std::abs(Vector3::Dot(obb.orientations[0], faceNormalAndCross[i])) +
            obb.size.y * std::abs(Vector3::Dot(obb.orientations[1], faceNormalAndCross[i])) +
            obb.size.z * std::abs(Vector3::Dot(obb.orientations[2], faceNormalAndCross[i]));

        // 中心点の差分の投影
        float centerDistance = std::abs(Vector3::Dot(obb.center - ((aabb.max + aabb.min) * 0.5f), faceNormalAndCross[i]));

        if (centerDistance > (aabbProjection + obbProjection)) {
            return false;  // 衝突していない
        }
    }
    return true;  // 衝突している
}
