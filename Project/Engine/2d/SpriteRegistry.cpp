#include "SpriteRegistry.h"

using namespace KetaEngine;
#include "2D/Sprite.h"
#include "Base/Dx/DirectXCommon.h"
#include "Pipeline/PipelineManager.h"
// std
#include <imgui.h>

///============================================================
/// 全オブジェクトの描画
///============================================================
void SpriteRegistry::DrawAll() {
    PipelineManager::GetInstance()->PreDraw(PipelineType::Sprite, DirectXCommon::GetInstance()->GetCommandList());

    std::vector<Sprite*> sortedSprites(objects_.begin(), objects_.end());
    // レイヤー番号順にソート
    std::sort(sortedSprites.begin(), sortedSprites.end(), [](Sprite* a, Sprite* b) {
        if (a == nullptr || b == nullptr)
            return false;
        return a->GetLayerNum() < b->GetLayerNum();
    });

    // ソート済みのスプライトを描画
    for (Sprite* obj : sortedSprites) {
        if (obj != nullptr && objects_.find(obj) != objects_.end()) {
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
    RegistryBase<SpriteRegistry, Sprite>::Clear();
    groupRepresentatives_.clear();
}

///============================================================
/// デバッグ表示
///============================================================
void SpriteRegistry::DebugImGui() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader("Sprite Registry")) {
        ImGui::Text("Registered Objects: %zu", objects_.size());

        ImGui::Separator();

        int index = 0;
        for (Sprite* obj : objects_) {
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
/// グループ名の重複チェック
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
/// 代表スプライト解除
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
