#include "DistortionManager.h"

using namespace KetaEngine;
#include "3D/RibbonTrail/RibbonTrailManager.h"
#include "Base/Dx/DirectXCommon.h"
#include "Base/Dx/DxDepthBuffer.h"
#include "Base/Dx/DxRenderTarget.h"
#include "Particle/CPUParticle/ParticleManager.h"

///============================================================
/// シングルトンインスタンス取得
///============================================================
DistortionManager* DistortionManager::GetInstance() {
    static DistortionManager instance;
    return &instance;
}

///============================================================
/// 初期化
///============================================================
void DistortionManager::Init(DirectXCommon* dxCommon, uint32_t width, uint32_t height) {
    // 初期化
    distortionRT_.Init(dxCommon, width, height);
    distortionEffect_.Init(dxCommon);
    initialized_ = true;
}

///============================================================
/// 歪みパス開始
///============================================================
void DistortionManager::BeginPass() {
    hasAnyDistortion_ = false;
    passOpen_         = false;

    if (!initialized_) {
        return;
    }

    // 歪みを使っている側が1つも無ければパスを開かない
    bool hasRibbonDistortion   = RibbonTrailManager::GetInstance()->HasDistortionTrails();
    bool hasParticleDistortion = ParticleManager::GetInstance()->HasDistortionParticles();
    if (!hasRibbonDistortion && !hasParticleDistortion) {
        return;
    }

    // ポストエフェクト適用のためtrueにする
    hasAnyDistortion_ = true;

    // dxCommonの取得
    KetaEngine::DirectXCommon* dxCommon    = DirectXCommon::GetInstance();
    ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle  = dxCommon->GetDepthBuffer()->GetDsvHandle();

    // 歪みRTに切り替え・クリア
    distortionRT_.BeginDistortionPass(commandList, dsvHandle);
    passOpen_ = true;
}

///============================================================
/// 歪みパス終了
///============================================================
void DistortionManager::EndPass() {
    if (!passOpen_) {
        return;
    }
    passOpen_ = false;

    auto dxCommon    = DirectXCommon::GetInstance();
    auto commandList = dxCommon->GetCommandList();

    // 歪みRTをSRVとして遷移
    distortionRT_.EndDistortionPass(commandList);

    // メインRTを復元
    dxCommon->GetDxRenderTarget()->SetSceneRTAsRenderTarget(commandList);
}

///============================================================
/// 歪みポストエフェクト適用
///============================================================
void DistortionManager::ApplyEffect(ID3D12GraphicsCommandList* commandList) {
    if (!initialized_ || !hasAnyDistortion_) {
        return;
    }

    distortionEffect_.SetDistortionSRV(distortionRT_.GetGPUSrvHandle());
    distortionEffect_.SetDrawState(commandList);
    distortionEffect_.Draw(commandList);
}
