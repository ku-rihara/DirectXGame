#include "SpriteRegistry.h"

using namespace KetaEngine;
#include "2d/Sprite.h"
#include "Base/Dx/DirectXCommon.h"
#include "Pipeline/PipelineManager.h"
#include <algorithm>
#include <cstdlib>
#include <imgui.h>
#include <vector>

bool SpriteRegistry::isDestroyed_ = false;

///============================================================
/// デストラクタ
///============================================================
SpriteRegistry::~SpriteRegistry() {
    isDestroyed_ = true;
    sprites_.clear();
}

///============================================================
/// シングルトンインスタンス取得
///============================================================
SpriteRegistry* SpriteRegistry::GetInstance() {
    static SpriteRegistry instance;
    static bool isAlive = true;

    // アプリケーション終了処理中かチェック
    if (!isAlive) {
        return nullptr;
    }

    // 初回呼び出し時にAtexit登録
    static bool registered = false;
    if (!registered) {
        std::atexit([]() { isAlive = false; });
        registered = true;
    }

    return &instance;
}

///============================================================
/// オブジェクト登録
///============================================================
void SpriteRegistry::RegisterObject(Sprite* object) {
    if (object != nullptr) {
        sprites_.insert(object);
    }
}

void SpriteRegistry::UnregisterObject(Sprite* object) {
    if (object != nullptr) {
        sprites_.erase(object);
    }
}

///============================================================
/// 全オブジェクトの描画
///============================================================
void SpriteRegistry::DrawAll() {
    PipelineManager::GetInstance()->PreDraw(PipelineType::Sprite, DirectXCommon::GetInstance()->GetCommandList());

    std::vector<Sprite*> sortedSprites(sprites_.begin(), sprites_.end());
    // レイヤー番号順にソート
    std::sort(sortedSprites.begin(), sortedSprites.end(), [](Sprite* a, Sprite* b) {
        if (a == nullptr || b == nullptr)
            return false;
        return a->GetLayerNum() < b->GetLayerNum(); 
    });

    // ソート済みのスプライトを描画
    for (Sprite* obj : sortedSprites) {
        if (obj != nullptr && sprites_.find(obj) != sprites_.end()) {
            if (obj->GetIsDraw()) {
                obj->Draw();
            }
        }
    }
}

///============================================================
/// 全オブジェクトクリア
///============================================================
void SpriteRegistry::Clear() {
    sprites_.clear();
}

///============================================================
/// デバッグ表示
///============================================================
void SpriteRegistry::DebugImGui() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Sprite Registry")) {
        ImGui::Text("Registered Objects: %zu", sprites_.size());

        ImGui::Separator();

        int index = 0;
        for (Sprite* obj : sprites_) {
            if (obj != nullptr) {
                ImGui::PushID(index);

                // オブジェクトのデバッグ情報を表示
                obj->AdjustParam();

                ImGui::PopID();
                index++;
            }
        }
    }
#endif // _DEBUG
}

///============================================================
/// グループ名の重複チェック（代表が登録済みか）
///============================================================
bool SpriteRegistry::HasGroupName(const std::string& groupName) const {
    return groupRepresentatives_.count(groupName) > 0;
}

///============================================================
/// 代表スプライト登録
///============================================================
void SpriteRegistry::RegisterRepresentative(const std::string& groupName, Sprite* sprite) {
    groupRepresentatives_[groupName] = sprite;
}

///============================================================
/// 代表スプライト解除（フォロワーの参照もクリア）
///============================================================
void SpriteRegistry::UnregisterRepresentative(Sprite* sprite) {
    for (auto it = groupRepresentatives_.begin(); it != groupRepresentatives_.end(); ++it) {
        if (it->second == sprite) {
            groupRepresentatives_.erase(it);
            return;
        }
    }
}

///============================================================
/// 代表スプライト取得
///============================================================
Sprite* SpriteRegistry::GetRepresentative(const std::string& groupName) const {
    auto it = groupRepresentatives_.find(groupName);
    return (it != groupRepresentatives_.end()) ? it->second : nullptr;
}
