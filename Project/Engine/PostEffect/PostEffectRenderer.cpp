#include "PostEffectRenderer.h"

using namespace KetaEngine;
#include "Base/Dx/DirectXCommon.h"
#include "Base/Dx/DxRenderTarget.h"

#include "BoxFilter.h"
#include "Dissolve.h"
#include "GaussianFilter.h"
#include "GrayScale.h"
#include "LuminanceBasedOutline.h"
#include "NormalScreen.h"
#include "Outline.h"
#include "RadialBlur.h"
#include "RandomNoize.h"
#include "Vignette.h"

#include <algorithm>
#include <imgui.h>

PostEffectRenderer* PostEffectRenderer::GetInstance() {
    static PostEffectRenderer instance;
    return &instance;
}

void PostEffectRenderer::Init(DirectXCommon* dxCommon) {
    dxCommon_    = dxCommon;
    currentMode_ = PostEffectMode::NONE;

    // それぞれ生成
    effects_[static_cast<size_t>(PostEffectMode::NONE)]             = std::make_unique<NormalScreen>();
    effects_[static_cast<size_t>(PostEffectMode::GRAY)]             = std::make_unique<GrayScale>();
    effects_[static_cast<size_t>(PostEffectMode::VIGNETTE)]         = std::make_unique<Vignette>();
    effects_[static_cast<size_t>(PostEffectMode::GAUS)]             = std::make_unique<GaussianFilter>();
    effects_[static_cast<size_t>(PostEffectMode::BOXFILTER)]        = std::make_unique<BoxFilter>();
    effects_[static_cast<size_t>(PostEffectMode::RADIALBLUR)]       = std::make_unique<RadialBlur>();
    effects_[static_cast<size_t>(PostEffectMode::RANDOMNOISE)]      = std::make_unique<RandomNoize>();
    effects_[static_cast<size_t>(PostEffectMode::DISSOLVE)]         = std::make_unique<Dissolve>();
    effects_[static_cast<size_t>(PostEffectMode::OUTLINE)]          = std::make_unique<Outline>();
    effects_[static_cast<size_t>(PostEffectMode::LUMINANCEOUTLINE)] = std::make_unique<LuminanceBasedOutline>();

    // 初期化
    for (size_t i = 0; i < effects_.size(); ++i) {
        effects_[i]->Init(dxCommon_);
    }
}

void PostEffectRenderer::Draw(ID3D12GraphicsCommandList* commandList) {
    auto* rt      = dxCommon_->GetDxRenderTarget();
    const size_t N = effectStack_.size();

    if (N == 0) {
        // パススルー：シーンRTをそのままバックバッファへ
        auto* effect = effects_[static_cast<size_t>(PostEffectMode::NONE)].get();
        effect->SetInputSRV(rt->GetRenderTextureGPUSrvHandle());
        effect->SetDrawState(commandList);
        effect->Draw(commandList);
        return;
    }

    if (N == 1) {
        // 1パス：シーンRT → エフェクト → ピンポン（GameView用SRV）→ NormalScreen → バックバッファ
        auto* effect = effects_[static_cast<size_t>(effectStack_[0])].get();
        effect->SetInputSRV(rt->GetRenderTextureGPUSrvHandle());
        rt->SetPingPongAsRenderTarget(commandList);
        effect->SetDrawState(commandList);
        effect->Draw(commandList);

        // ピンポンをSRVに遷移
        rt->TransitionPingPongTo(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // ピンポン → バックバッファへコピー
        auto* normalScreen = effects_[static_cast<size_t>(PostEffectMode::NONE)].get();
        normalScreen->SetInputSRV(rt->GetPingPongGPUSrvHandle());
        rt->SetBackBufferAsRenderTarget(commandList);
        normalScreen->SetDrawState(commandList);
        normalScreen->Draw(commandList);
        return;
    }

    for (size_t i = 0; i < N; ++i) {
        const bool isLast = (i == N - 1);
        auto* effect = effects_[static_cast<size_t>(effectStack_[i])].get();

        // --- 入力SRV設定 ---
        // pass0: シーンRT
        // pass1: ピンポンRT 
        // pass2: シーンRT 
        if (i == 0) {
            effect->SetInputSRV(rt->GetRenderTextureGPUSrvHandle());
        } else if (i % 2 == 1) {
            effect->SetInputSRV(rt->GetPingPongGPUSrvHandle());
        } else {
            effect->SetInputSRV(rt->GetRenderTextureGPUSrvHandle());
        }

        // --- 出力RT設定 ---
        if (isLast) {
            // N>1 の場合のみ OMSetRenderTargets を戻す
            if (N > 1) {
                rt->SetBackBufferAsRenderTarget(commandList);
            }
        } else if (i % 2 == 0) {
            // 偶数パスの出力：ピンポンRT
            rt->SetPingPongAsRenderTarget(commandList);
        } else {
            // 奇数パスの出力：シーンRT（再利用）
            rt->SetSceneRTAsRenderTarget(commandList);
        }

        effect->SetDrawState(commandList);
        effect->Draw(commandList);

        // --- 出力RTをSRVに遷移 ---
        if (!isLast) {
            if (i % 2 == 0) {
                rt->TransitionPingPongTo(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            } else {
                rt->TransitionSceneRTTo(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            }
        }
    }
}

void PostEffectRenderer::DrawImGui() {
#ifdef _DEBUG
    static const char* kEffectNames[] = {
        "None", "Gray", "Vignette", "Gaus", "BoxFilter",
        "RadialBlur", "RandomNoize", "Dissolve", "Outline", "LuminanceBasedOutline"};

    if (ImGui::Begin("PostEffects")) {
        ImGui::SeparatorText("エフェクトスタック (上から順に適用)");

        // チェックボックスで各エフェクトを有効/無効
        for (int i = 1; i < static_cast<int>(PostEffectMode::COUNT); ++i) {
            auto mode    = static_cast<PostEffectMode>(i);
            bool enabled = IsEffectEnabled(mode);
            ImGui::PushID(i);
            if (ImGui::Checkbox(kEffectNames[i], &enabled)) {
                if (enabled) {
                    EnableEffect(mode);
                } else {
                    DisableEffect(mode);
                }
            }
            ImGui::PopID();
        }

        // 現在のスタック順を表示
        ImGui::SeparatorText("現在のスタック順");
        if (effectStack_.empty()) {
            ImGui::TextDisabled("(エフェクトなし)");
        } else {
            for (size_t i = 0; i < effectStack_.size(); ++i) {
                ImGui::Text("%zu: %s", i + 1, kEffectNames[static_cast<int>(effectStack_[i])]);
            }
        }

        ImGui::Separator();

        // 有効なエフェクトのパラメータ調整
        for (auto mode : effectStack_) {
            effects_[static_cast<size_t>(mode)]->DebugParamImGui();
        }
    }
    ImGui::End();
#endif
}

void PostEffectRenderer::EnableEffect(PostEffectMode mode) {
    if (mode == PostEffectMode::NONE) {
        return;
    }
    // 既に存在する場合は追加しない
    for (auto m : effectStack_) {
        if (m == mode) {
            return;
        }
    }
    effectStack_.push_back(mode);
}

void PostEffectRenderer::DisableEffect(PostEffectMode mode) {
    effectStack_.erase(
        std::remove(effectStack_.begin(), effectStack_.end(), mode),
        effectStack_.end());
}

void PostEffectRenderer::ClearEffectStack() {
    effectStack_.clear();
}

bool PostEffectRenderer::IsEffectEnabled(PostEffectMode mode) const {
    for (auto m : effectStack_) {
        if (m == mode) {
            return true;
        }
    }
    return false;
}

void PostEffectRenderer::SetViewProjection(const ViewProjection* viewProjection) {
    viewProjection_ = viewProjection;
    for (size_t i = 0; i < effects_.size(); ++i) {
        effects_[i]->SetViewProjection(viewProjection_);
    }
}

D3D12_GPU_DESCRIPTOR_HANDLE PostEffectRenderer::GetPostProcessedSRVHandle() const {
    auto* rt = dxCommon_->GetDxRenderTarget();
    if (!effectStack_.empty()) {
        // N==1 時はピンポンにエフェクト済み出力が残る
        return rt->GetPingPongGPUSrvHandle();
    }
    return rt->GetRenderTextureGPUSrvHandle();
}

// 指定したモードの効果を取得
BasePostEffect* PostEffectRenderer::GetEffect(PostEffectMode mode) {
    size_t index = static_cast<size_t>(mode);
    if (index >= effects_.size()) {
        return nullptr;
    }
    return effects_[index].get();
}

