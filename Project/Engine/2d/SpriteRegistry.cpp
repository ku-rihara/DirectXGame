#include "SpriteRegistry.h"
#include "2d/Sprite.h"
#include "Dx/DirectXCommon.h"
#include "Pipeline/PipelineManager.h"
#include <cstdlib>
#include <imgui.h>

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

    auto objectsCopy = sprites_;

    for (Sprite* obj : objectsCopy) {
        if (obj != nullptr && sprites_.find(obj) != sprites_.end()) {
            obj->Draw();
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