#include "AnimationRegistry.h"
#include "Object3DAnimation.h"
#include <algorithm>
#include <imgui.h>

bool AnimationRegistry::isDestroyed_ = false;

///============================================================
/// デストラクタ
///============================================================
AnimationRegistry::~AnimationRegistry() {
    isDestroyed_ = true;
    animations_.clear();
}

///============================================================
/// シングルトンインスタンス取得
///============================================================
AnimationRegistry* AnimationRegistry::GetInstance() {
    static AnimationRegistry instance;
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
/// 登録
///============================================================
void AnimationRegistry::RegisterAnimation(Object3DAnimation* animation) {
    if (animation != nullptr) {
        animations_.insert(animation);
    }
}

///============================================================
/// 登録解除
///============================================================
void AnimationRegistry::UnregisterAnimation(Object3DAnimation* animation) {
    if (animation != nullptr) {
        animations_.erase(animation);
    }
}

///============================================================
/// 更新
///============================================================
void AnimationRegistry::UpdateAll(const float& deltaTime) {

    for (Object3DAnimation* animation : animations_) {
        if (animation != nullptr && animations_.find(animation) != animations_.end()) {
            animation->Update(deltaTime);
        }
    }
}

///============================================================
/// 描画
///============================================================
void AnimationRegistry::DrawAll(const ViewProjection& viewProjection) {
    for (Object3DAnimation* animation : animations_) {
        if (animation != nullptr && animations_.find(animation) != animations_.end()) {
            animation->Draw(viewProjection);
        }
    }
}

///============================================================
/// シャドウ描画
///============================================================
void AnimationRegistry::DrawAllShadow(const ViewProjection& viewProjection) {
    for (Object3DAnimation* animation : animations_) {
        if (animation != nullptr && animations_.find(animation) != animations_.end()) {
            animation->DrawShadow(viewProjection);
        }
    }
}

///============================================================
/// デバッグ描画
///============================================================
void AnimationRegistry::DebugLineAllSet() {
    for (Object3DAnimation* animation : animations_) {
        if (animation != nullptr) {
            animation->DebugLineSet();
        }
    }
}

///============================================================
/// オブジェクトクリア
///============================================================
void AnimationRegistry::Clear() {
    animations_.clear();
}

///============================================================
/// デバッグ表示
///============================================================
void AnimationRegistry::DebugImGui() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Animation Registry")) {
        ImGui::Text("Registered Animations: %zu", animations_.size());

        ImGui::Separator();

        int index = 0;
        for (Object3DAnimation* animation : animations_) {
            if (animation != nullptr) {
                ImGui::PushID(index);

                if (ImGui::CollapsingHeader(("Animation " + std::to_string(index)).c_str())) {
                    ImGui::Text("Animation Address: %p", static_cast<void*>(animation));
                }

                ImGui::PopID();
                index++;
            }
        }
    }
#endif // _DEBUG
}