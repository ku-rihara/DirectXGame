#include "Object3DRegistry.h"
#include "3d/Object3d.h"
#include <algorithm>
#include <imgui.h>

///============================================================
/// シングルトンインスタンス取得
///============================================================
Object3DRegistry* Object3DRegistry::GetInstance() {
    static Object3DRegistry instance;
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

///============================================================
/// 全オブジェクトの更新
///============================================================
void Object3DRegistry::UpdateAll() {
  
}

///============================================================
/// 全オブジェクトの描画
///============================================================
void Object3DRegistry::DrawAll(const ViewProjection& viewProjection) {
    for (Object3d* obj : objects_) {
        if (obj != nullptr) {
            obj->Draw(viewProjection);
        }
    }
}

///============================================================
/// 全オブジェクトのシャドウ描画
///============================================================
void Object3DRegistry::DrawAllShadow(const ViewProjection& viewProjection) {
    for (Object3d* obj : objects_) {
        if (obj != nullptr) {
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