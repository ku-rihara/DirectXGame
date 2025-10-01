#include "OBBCollider.h"
#include"Collider/CollisionManager.h"

OBBCollider::OBBCollider() {
    // 自動登録する
    CollisionManager::AddCollider(this);
}

OBBCollider::~OBBCollider() {
    // 自動登録する
    CollisionManager::RemoveCollider(this);
}


void OBBCollider::Init() {

    debugLine_.Init(24); 
    cTransform_.Init();
    obb_.size = { 1.5f,1.5f,1.5f };
}

void OBBCollider::UpdateWorldTransform() {
 
    // OBBの中心をワールド座標に更新
    cTransform_.translation_ = GetCollisionPos();
    cTransform_.rotation_ = GetRotate();
    cTransform_.scale_ = obb_.size;

     obb_.center = GetCollisionPos();

    // 回転ベクトルを更新
    obb_.orientations[0] = cTransform_.GetRightVector();    // X軸方向ベクトル
    obb_.orientations[1] = cTransform_.GetUpVector();       // Y軸方向ベクトル
    obb_.orientations[2] = cTransform_.GetForwardVector();  // Z軸方向ベクトル
  
    // ワールド変換行列の更新
    cTransform_.UpdateMatrix();
}

void OBBCollider::DrawDebugCube(const ViewProjection& viewProjection) {
    DrawOBBLine(obb_, lineColor_);
    debugLine_.Draw(viewProjection);
}

Vector3 OBBCollider::GetCollisionPos() const {
    return BaseCollider::GetCollisionPos();
}

Vector3 OBBCollider::GetRotate() const {
    Vector3 rotate;
    rotate = cTransform_.rotation_;
    return rotate;
}

void OBBCollider::DrawOBBLine(const OBB& obb, const Vector4& color) {
    const Vector3& c = obb.center;
    const Vector3& u = obb.orientations[0]; // X方向
    const Vector3& v = obb.orientations[1]; // Y方向
    const Vector3& w = obb.orientations[2]; // Z方向

    const Vector3& s = obb.size * 0.5f; // 半サイズ

    // 頂点を8つ求める（ローカル→ワールド）
    Vector3 verts[8];
    verts[0] = c + (-u * s.x) + (-v * s.y) + (-w * s.z);
    verts[1] = c + (u * s.x) + (-v * s.y) + (-w * s.z);
    verts[2] = c + (u * s.x) + (v * s.y) + (-w * s.z);
    verts[3] = c + (-u * s.x) + (v * s.y) + (-w * s.z);
    verts[4] = c + (-u * s.x) + (-v * s.y) + (w * s.z);
    verts[5] = c + (u * s.x) + (-v * s.y) + (w * s.z);
    verts[6] = c + (u * s.x) + (v * s.y) + (w * s.z);
    verts[7] = c + (-u * s.x) + (v * s.y) + (w * s.z);

    // インデックステーブル（12本の線）
    int indices[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // 底面
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // 上面
        {0, 4}, {1, 5}, {2, 6}, {3, 7} // 側面
    };

    for (int i = 0; i < 12; ++i) {
        debugLine_.SetLine(verts[indices[i][0]], verts[indices[i][1]], color);
    }
}
