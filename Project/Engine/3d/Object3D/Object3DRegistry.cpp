#include "Object3DRegistry.h"

using namespace KetaEngine;
// log
#include "../../Utility/Log/Log.h"
// 3D
#include "3D/Object3d/Object3d.h"
// Base
#include "Base/Dx/DirectXCommon.h"
// Editor
#include "Editor/ObjectLightingEditor/ObjectLightingEditor.h"
// Pipeline
#include "Pipeline/PipelineManager.h"
// ImGui
#include <imgui.h>
// std
#include <format>

///============================================================
/// オブジェクト登録
///============================================================
void Object3DRegistry::RegisterObject(Object3d* object) {
    if (object == nullptr) {
        return;
    }

    // 基底クラスの登録処理を呼び出す
    RegistryBase<Object3DRegistry, Object3d>::RegisterObject(object);

    // ログにモデル名とアドレスを出力
    Log::Info(std::format("[Object3DRegistry] Registered: {} (addr={:p})",
        object->GetModel()->GetModelName(), static_cast<void*>(object)));
}

///============================================================
/// 全オブジェクトの更新
///============================================================
void Object3DRegistry::UpdateAll() {

    ForEachAlive([](Object3d* obj) {

        // Editorのオブジェクトライティング情報を同期
        ObjectLightingEditor::GetInstance()->SyncObject(obj);

        // 自動更新を無効にしているオブジェクトはスキップ
        if (!obj->GetIsAutoUpdate()) {
            return;
        }

        // オブジェクトの更新
        obj->Update();
    });
}

///============================================================
/// 全オブジェクトの描画
///============================================================
void Object3DRegistry::DrawAll(const ViewProjection& viewProjection) {
    PipelineManager::GetInstance()->PreDraw(PipelineType::Object3D, DirectXCommon::GetInstance()->GetCommandList());

    ForEachAlive([&viewProjection](Object3d* obj) {
        obj->Draw(viewProjection);
    });
}

///============================================================
/// 全オブジェクトのシャドウ描画
///============================================================
void Object3DRegistry::DrawAllShadow(const ViewProjection& viewProjection) {
    ForEachAlive([&viewProjection](Object3d* obj) {
        obj->ShadowDraw(viewProjection);
    });
}

///============================================================
/// デバッグ表示
///============================================================
void Object3DRegistry::DebugImGui() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader("登録されたオブジェクト情報")) {
        ImGui::Text("登録された数: %zu", objects_.size());

        if (ImGui::CollapsingHeader("オブジェクトライティング")) {
            ObjectLightingEditor::GetInstance()->DebugImGui();
        }

        ImGui::Separator();

        int index = 0;
        for (Object3d* obj : objects_) {
            if (obj != nullptr) {
                ImGui::PushID(index);

                // オブジェクトのデバッグ情報を表示
                if (ImGui::CollapsingHeader(("Object " + obj->GetModel()->GetModelName()).c_str())) {
                    obj->DebugImGui();
                }

                ImGui::PopID();
                index++;
            }
        }
    }
#endif // _DEBUG
}
