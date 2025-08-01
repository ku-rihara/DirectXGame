#include "Object3DRegistry.h"
#include "3d/Object3d.h"
#include "Pipeline/Object3DPiprline.h"
#include <imgui.h>
#include <cstdlib>

bool Object3DRegistry::isDestroyed_ = false;

///============================================================
/// デストラクタ
///============================================================
Object3DRegistry::~Object3DRegistry() {
    isDestroyed_ = true;
    objects_.clear();
}


///============================================================
/// シングルトンインスタンス取得
///============================================================
Object3DRegistry* Object3DRegistry::GetInstance() {
    static Object3DRegistry instance;
    static bool isAlive = true;

    // アプリケーション終了処理中かチェック
    if (!isAlive) {
        return nullptr;
    }

    // 初回呼び出し時にatexit登録
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
void Object3DRegistry::RegisterObject(Object3d* object) {
    if (object != nullptr) {
        objects_.insert(object);
    }
}

void Object3DRegistry::UnregisterObject(Object3d* object) {
    if (object != nullptr) {
        objects_.erase(object);
    }
}

///============================================================
/// 全オブジェクトの更新
///============================================================
void Object3DRegistry::UpdateAll() {

    auto objectsCopy = objects_;
    for (Object3d* obj : objectsCopy) {
        if (obj != nullptr && objects_.find(obj) != objects_.end()) {
            obj->Update();
        }
    }
}

///============================================================
/// 全オブジェクトの描画
///============================================================
void Object3DRegistry::DrawAll(const ViewProjection& viewProjection) {
    Object3DPiprline::GetInstance()->PreDraw(DirectXCommon::GetInstance()->GetCommandList());

    auto objectsCopy = objects_;
    for (Object3d* obj : objectsCopy) {
        if (obj != nullptr && objects_.find(obj) != objects_.end()) {
            obj->Draw(viewProjection);
        }
    }
}

///============================================================
/// 全オブジェクトのシャドウ描画
///============================================================
void Object3DRegistry::DrawAllShadow(const ViewProjection& viewProjection) {
    auto objectsCopy = objects_;
    for (Object3d* obj : objectsCopy) {
        if (obj != nullptr && objects_.find(obj) != objects_.end()) {
            obj->ShadowDraw(viewProjection);
        }
    }
}

///============================================================
/// 全オブジェクトクリア
///============================================================
void Object3DRegistry::Clear() {
    objects_.clear();
}

///============================================================
/// デバッグ表示
///============================================================
void Object3DRegistry::DebugImGui() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Object3D Registry")) {
        ImGui::Text("Registered Objects: %zu", objects_.size());

        ImGui::Separator();

        int index = 0;
        for (Object3d* obj : objects_) {
            if (obj != nullptr) {
                ImGui::PushID(index);

                if (ImGui::CollapsingHeader(("Object " + std::to_string(index)).c_str())) {
                    ImGui::Text("Object Address: %p", static_cast<void*>(obj));

                    // オブジェクトのデバッグ情報を表示
                    obj->DebugImgui();
                }

                ImGui::PopID();
                index++;
            }
        }
    }
#endif // _DEBUG
}