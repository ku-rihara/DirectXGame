#include "CollisionManager.h"

using namespace KetaEngine;

#include "Box.h"
#include "Collider/AABBCollider.h"
#include "Collider/OBBCollider.h"
#include <imgui.h>

// 静的メンバ変数の初期化
std::list<BaseCollider*> CollisionManager::baseColliders_;

void CollisionManager::Init() {
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    spatialGrid_ = std::make_unique<SpatialGrid>();
    spatialGrid_->SetParam(cellSize_, worldMin_, worldMax_);
}

void CollisionManager::AddCollider(BaseCollider* collider) {
    if (AABBCollider* aabbCollider = dynamic_cast<AABBCollider*>(collider)) {
        aabbCollider->Init();
        baseColliders_.push_back(collider);
    } else if (OBBCollider* obbCollider = dynamic_cast<OBBCollider*>(collider)) {
        obbCollider->Init();
        baseColliders_.push_back(collider);
    }
}

void CollisionManager::RemoveCollider(BaseCollider* collider) {
    baseColliders_.remove(collider); // リストから削除
}

void CollisionManager::Reset() {
    baseColliders_.clear();
    collisionStates_.clear(); // 状態リセット
}

void CollisionManager::Update() {

    UpdateWorldTransform();

    CheckAllCollisions();

    LineAllSet();
}

void CollisionManager::UpdateWorldTransform() {

    if (isNotAdaptCollision_) {
        return;
    }

    // 全てのコライダーについて行列更新をする
    for (BaseCollider* baseCollider : baseColliders_) {
        baseCollider->UpdateWorldTransform();
        baseCollider->ReverseNormalColor();
    }
}

void CollisionManager::LineAllSet() {
#ifdef _DEBUG

    if (spatialGrid_ && isGridVisible_) {
        spatialGrid_->DrawGrid();
    }

    // 非表示なら抜ける
    if (!isColliderVisible_) {
        return;
    }

    // 全てのコライダーを描画する
    for (BaseCollider* baseCollider : baseColliders_) {
        baseCollider->SetDebugCube();
    }
#endif // _DEBUG
}

void CollisionManager::CheckCollisionPair(BaseCollider* colliderA, BaseCollider* colliderB) {

    if (!colliderA->GetIsCollision() || !colliderB->GetIsCollision()) {
        return;
    }

    bool collisionDetected = false;

    // dynamic_cast を使用して AABB と OBB の判定
    if (auto* aabbA = dynamic_cast<AABBCollider*>(colliderA)) {
        if (auto* aabbB = dynamic_cast<AABBCollider*>(colliderB)) {
            collisionDetected = IsCollision(aabbA->GetAABB(), aabbB->GetAABB());
        } else if (auto* obbB = dynamic_cast<OBBCollider*>(colliderB)) {
            collisionDetected = IsCollision(obbB->GetOBB(), aabbA->GetAABB());
        }
    } else if (auto* obbA = dynamic_cast<OBBCollider*>(colliderA)) {
        if (auto* obbB = dynamic_cast<OBBCollider*>(colliderB)) {
            collisionDetected = IsCollision(obbA->GetOBB(), obbB->GetOBB());
        } else if (auto* aabbB = dynamic_cast<AABBCollider*>(colliderB)) {
            collisionDetected = IsCollision(obbA->GetOBB(), aabbB->GetAABB());
        }
    }

    // ペアをキーにして状態を管理
    auto pair = std::make_pair(colliderA, colliderB);
    if (collisionDetected) {
        if (collisionStates_[pair] == false) {
            HandleCollision(colliderA, colliderB); // 新しい衝突
        } else {
            // 既存の衝突継続
            colliderA->OnCollisionStay(colliderB);
            colliderB->OnCollisionStay(colliderA);
        }
        collisionStates_[pair] = true; // 衝突中に設定
    } else {
        if (collisionStates_[pair] == true) {
            HandleCollisionExit(colliderA, colliderB); // 衝突終了
        }
        collisionStates_[pair] = false; // 衝突なしに設定
    }
}

void CollisionManager::HandleCollision(BaseCollider* colliderA, BaseCollider* colliderB) {
    //  OnCollisionEnter
    colliderA->OnCollisionEnter(colliderB);
    colliderB->OnCollisionEnter(colliderA);

    // その後、継続処理も実行
    colliderA->OnCollisionStay(colliderB);
    colliderB->OnCollisionStay(colliderA);

    colliderA->SetColliding(true);
    colliderB->SetColliding(true);
}

// 衝突終了処理
void CollisionManager::HandleCollisionExit(BaseCollider* colliderA, BaseCollider* colliderB) {
    if (colliderA->GetIsColliding()) {
        colliderA->OnCollisionExit(colliderB);
        colliderA->SetColliding(false);
    }
    if (colliderB->GetIsColliding()) {
        colliderB->OnCollisionExit(colliderA);
        colliderB->SetColliding(false);
    }
}

void CollisionManager::CheckAllCollisions() {

    // グリッドをクリアして再構築
    spatialGrid_->Clear();

    if (isNotAdaptCollision_) {
        return;
    }

    if (baseColliders_.size() < 2) {
        return;
    }

    // 全コライダーをグリッドに登録
    for (BaseCollider* collider : baseColliders_) {
        spatialGrid_->Insert(collider);
    }

    // 各コライダーについて、近隣のコライダーとのみ判定
    std::unordered_map<std::pair<BaseCollider*, BaseCollider*>, bool, PairHash> checkedPairs;

    for (BaseCollider* colliderA : baseColliders_) {
        // 近隣のコライダーを取得
        std::vector<BaseCollider*> nearbyColliders = spatialGrid_->GetNearbyColliders(colliderA);

        for (BaseCollider* colliderB : nearbyColliders) {
            // ペアの重複チェック（A-BとB-Aは同じ）
            auto pair1 = std::make_pair(colliderA, colliderB);
            auto pair2 = std::make_pair(colliderB, colliderA);

            if (checkedPairs[pair1] || checkedPairs[pair2]) {
                continue;
            }

            checkedPairs[pair1] = true;
            checkedPairs[pair2] = true;

            // 衝突判定
            CheckCollisionPair(colliderA, colliderB);
        }
    }
}

void CollisionManager::RegisterParams() {
    globalParameter_->Regist<bool>(groupName_, "isColliderVisible", &isColliderVisible_);
    globalParameter_->Regist<bool>(groupName_, "isGridVisible", &isGridVisible_);
    globalParameter_->Regist<Vector3>(groupName_, "worldMin", &worldMin_);
    globalParameter_->Regist<Vector3>(groupName_, "worldMax", &worldMax_);
    globalParameter_->Regist<float>(groupName_, "cellSize", &cellSize_);
}

void CollisionManager::AdjustParam() {
#ifdef _DEBUG

    ImGui::Begin("Collision");

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::Checkbox("IsColliderVisible", &isColliderVisible_);
        ImGui::Checkbox("isGridVisible", &isGridVisible_);
        ImGui::Checkbox("isNotAdaptCollision", &isNotAdaptCollision_);
        ImGui::Separator();
        ImGui::DragFloat3("WorldMin", &worldMin_.x, 0.1f);
        ImGui::DragFloat3("WorldMax", &worldMax_.x, 0.1f);
        ImGui::DragFloat("CellSize", &cellSize_, 0.1f);

        if (ImGui::Button("Apply Spatial Grid Params")) {
            spatialGrid_->SetParam(cellSize_, worldMin_, worldMax_);
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_, folderPath_);
        globalParameter_->ParamLoadForImGui(groupName_, folderPath_);

        ImGui::PopID();
    }

    ImGui::End();
#endif // _DEBUG
}