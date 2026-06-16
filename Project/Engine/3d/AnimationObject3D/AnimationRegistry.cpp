#include "AnimationRegistry.h"
#include <format>

using namespace KetaEngine;
#include "Base/Dx/DirectXCommon.h"
#include "Base/Dx/DxResourceBarrier.h"
#include "Object3DAnimation.h"
#include "Pipeline/CSPipelineManager.h"
#include "Pipeline/PipelineManager.h"
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
void AnimationRegistry::UpdateAll(float deltaTime) {

    for (Object3DAnimation* animation : animations_) {
        if (animation != nullptr && animation->IsActive()) {
            animation->Update(deltaTime);
        }
    }
}

///============================================================
/// スキニング
///============================================================
void AnimationRegistry::SkinningAll() {
    auto commandList = DirectXCommon::GetInstance()->GetCommandList();
    if (!commandList) {
        return;
    }

    // まとめてComputeShaderのパイプラインを設定
    CSPipelineManager::GetInstance()->PreDraw(CSPipelineType::Skinning, commandList);

    // 全敵のCSディスパッチを連続発行し、使用したリソースを収集
    std::vector<ID3D12Resource*> uavResources;
    for (Object3DAnimation* animation : animations_) {
        if (animation != nullptr && animation->IsActive() && animation->GetIsDraw()) {
            animation->CSSkinning();
            uavResources.push_back(animation->GetOutputVertexResource());
        }
    }

    // UAV barrierを全リソース分まとめて1回だけ発行
    if (!uavResources.empty()) {
        DxResourceBarrier* barrier = DirectXCommon::GetInstance()->GetResourceBarrier();
        barrier->UAVBarrierBatch(commandList, uavResources);
    }
}

///============================================================
/// 描画
///============================================================
void AnimationRegistry::DrawAll(const ViewProjection& viewProjection) {
    auto commandList = DirectXCommon::GetInstance()->GetCommandList();
    if (!commandList) {
        return;
    }

    PipelineManager::GetInstance()->PreDraw(PipelineType::SkinningObject3D, commandList);

    for (Object3DAnimation* animation : animations_) {
        if (animation != nullptr && animation->IsActive() && animation->GetIsDraw()) {
            // 個別のブレンドモードを設定
            PipelineManager::GetInstance()->PreBlendSet(PipelineType::SkinningObject3D, commandList, animation->GetBlendMode());
            animation->Draw(viewProjection);
        }
    }
}

///============================================================
/// シャドウ描画
///============================================================
void AnimationRegistry::DrawAllShadow(const ViewProjection& viewProjection) {
    for (Object3DAnimation* animation : animations_) {
        if (animation != nullptr && animation->IsActive()) {
            animation->DrawShadow(viewProjection);
        }
    }
}

///============================================================
/// デバッグ描画
///============================================================
void AnimationRegistry::DebugLineAllSet() {
    for (Object3DAnimation* animation : animations_) {
        if (animation != nullptr && animation->IsActive()) {
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
#if defined(_DEBUG) || defined(DEVELOPMENT)

    // 登録アニメーション一覧のヘッダ
    if (ImGui::CollapsingHeader("Animation Registry")) {
        ImGui::Text("登録アニメーションモデルの数: %zu", animations_.size());

        ImGui::Separator();

        // 登録中の各アニメーションを順に表示
        int index = 0;
        for (Object3DAnimation* animation : animations_) {
            if (animation != nullptr) {
                ImGui::PushID(index);

                // 折りたたみを開いたものだけ中身を描画
                if (ImGui::CollapsingHeader(std::format("Animation {}", index).c_str())) {
                    animation->DebugImGui();
                }

                ImGui::PopID();
                index++;
            }
        }
    }
#endif // _DEBUG
}
