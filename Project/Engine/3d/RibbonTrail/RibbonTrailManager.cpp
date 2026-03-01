#include "RibbonTrailManager.h"

using namespace KetaEngine;
#include "Base/Dx/DirectXCommon.h"
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
/// 全クリア
///============================================================
void RibbonTrailManager::Clear() {
    trails_.clear();
}
