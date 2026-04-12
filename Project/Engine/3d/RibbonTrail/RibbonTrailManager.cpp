#include "RibbonTrailManager.h"

using namespace KetaEngine;
#include "Base/Dx/DirectXCommon.h"
#include "Base/Dx/DxDepthBuffer.h"
#include "Base/Dx/DxRenderTarget.h"
#include "Frame/Frame.h"
#include "Pipeline/PipelineManager.h"
#include "RibbonTrail.h"
#include <cstdlib>

bool RibbonTrailManager::isDestroyed_ = false;

///============================================================
/// デストラクタ
///============================================================
RibbonTrailManager::~RibbonTrailManager() {
    isDestroyed_ = true;
    trails_.clear();
}

///============================================================
/// シングルトンインスタンス取得
///============================================================
RibbonTrailManager* RibbonTrailManager::GetInstance() {
    static RibbonTrailManager instance;
    static bool               isAlive = true;

    if (!isAlive) {
        return nullptr;
    }

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
void RibbonTrailManager::Register(RibbonTrail* trail) {
    if (trail != nullptr) {
        trails_.insert(trail);
    }
}

///============================================================
/// 登録解除
///============================================================
void RibbonTrailManager::Unregister(RibbonTrail* trail) {
    if (trail != nullptr) {
        trails_.erase(trail);
    }
}

///============================================================
/// 全トレイルの更新と描画
///============================================================
void RibbonTrailManager::DrawAll(const ViewProjection& viewProj) {
    if (trails_.empty()) {
        return;
    }

    float deltaTime  = KetaEngine::Frame::DeltaTime();
    auto  commandList = DirectXCommon::GetInstance()->GetCommandList();

    // RibbonTrail用パイプラインを一度だけ設定
    PipelineManager::GetInstance()->PreDraw(PipelineType::RibbonTrail, commandList);

    auto trailsCopy = trails_;
    for (RibbonTrail* trail : trailsCopy) {
        if (trail != nullptr && trails_.find(trail) != trails_.end()) {
            trail->Update(deltaTime);
            trail->Draw(viewProj);
        }
    }

    // Object3D用パイプラインに戻す
    PipelineManager::GetInstance()->PreDraw(PipelineType::Object3D, commandList);
}

///============================================================
/// 時空歪みシステム初期化
///============================================================
void RibbonTrailManager::InitDistortion(DirectXCommon* dxCommon, uint32_t width, uint32_t height) {
    distortionRT_.Init(dxCommon, width, height);
    distortionEffect_.Init(dxCommon);
    distortionInitialized_ = true;
}

///============================================================
/// 時空歪みパス描画
///============================================================
void RibbonTrailManager::DrawDistortionPass(const ViewProjection& viewProj, bool hasExternalDistortion) {
    hasAnyDistortion_   = false;
    distortionPassOpen_ = false;

    if (!distortionInitialized_) {
        return;
    }

    // 歪み有効なトレイルがあるか確認
    for (RibbonTrail* trail : trails_) {
        if (trail && trail->IsDistortionEnabled()) {
            hasAnyDistortion_ = true;
            break;
        }
    }

    // リボンもパーティクルも歪みなし → パスを開かない
    if (!hasAnyDistortion_ && !hasExternalDistortion) {
        return;
    }
    hasAnyDistortion_ = true; // ポストエフェクト適用のためtrueにする

    auto  dxCommon    = DirectXCommon::GetInstance();
    auto  commandList = dxCommon->GetCommandList();
    auto  dsvHandle   = dxCommon->GetDepthBuffer()->GetDsvHandle();

    // 歪みRTに切り替え・クリア
    distortionRT_.BeginDistortionPass(commandList, dsvHandle);
    distortionPassOpen_ = true;

    // 歪み有効なトレイルがあれば描画
    if (!trails_.empty()) {
        PipelineManager::GetInstance()->PreDraw(PipelineType::DistortionRibbon, commandList);

        auto trailsCopy = trails_;
        for (RibbonTrail* trail : trailsCopy) {
            if (trail && trails_.find(trail) != trails_.end()) {
                trail->DrawDistortion(viewProj);
            }
        }
    }
    // RTは開いたまま（CloseDistortionPassで閉じる）
}

///============================================================
/// 時空歪みパス終了
///============================================================
void RibbonTrailManager::CloseDistortionPass() {
    if (!distortionPassOpen_) {
        return;
    }
    distortionPassOpen_ = false;

    auto  dxCommon    = DirectXCommon::GetInstance();
    auto  commandList = dxCommon->GetCommandList();

    // 歪みRTをSRVとして遷移
    distortionRT_.EndDistortionPass(commandList);

    // メインRTを復元（後続のスプライト描画のため）
    dxCommon->GetDxRenderTarget()->SetSceneRTAsRenderTarget(commandList);
}

///============================================================
/// 時空歪みポストエフェクト適用
///============================================================
void RibbonTrailManager::ApplyDistortionEffect(ID3D12GraphicsCommandList* commandList) {
    if (!distortionInitialized_ || !hasAnyDistortion_) {
        return;
    }

    distortionEffect_.SetDistortionSRV(distortionRT_.GetGPUSrvHandle());
    distortionEffect_.SetDrawState(commandList);
    distortionEffect_.Draw(commandList);
}

///============================================================
/// 全クリア
///============================================================
void RibbonTrailManager::Clear() {
    trails_.clear();
}
