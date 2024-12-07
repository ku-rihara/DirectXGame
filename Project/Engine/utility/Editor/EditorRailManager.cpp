#include "EditorRailManager.h"
#include <imgui.h>
#include <cassert>

void EditorRailManager::Init(SrvManager* srvManager, uint32_t maxRails) {
    pSrvManager_ = srvManager;
    maxRails_ = maxRails;
    rails_.reserve(maxRails_);
    CreateStructuredBuffer();
}

void EditorRailManager::CreateStructuredBuffer() {
    // StructureBufferの作成
    auto device = DirectXCommon::GetInstance()->GetDevice();
    auto size = sizeof(Rail) * maxRails_;

    buffer_ = DirectXCommon::GetInstance()->CreateBufferResource(
        device, size);

    // GPU側リソースをマッピング
    buffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedBuffer_));

    // SRVを確保し作成
    srvIndex_ = pSrvManager_->Allocate();
    pSrvManager_->CreateSRVforStructuredBuffer(srvIndex_, buffer_.Get(), maxRails_, sizeof(Rail));
}

void EditorRailManager::Update(float deltaProgress) {
    for (size_t i = 0; i < rails_.size(); ++i) {
        // 進行度を更新（ループを考慮）
        rails_[i].progress += deltaProgress;
        if (rails_[i].progress > 1.0f) {
            rails_[i].progress -= 1.0f;
        }
        // マップ済みバッファにデータを送信
        mappedBuffer_[i] = rails_[i];
    }
}

void EditorRailManager::AddRail(const std::vector<Vector3>& controlPoints) {
    assert(rails_.size() < maxRails_ && "Error: Exceeded max rail count.");
    Rail newRail = { controlPoints, 0.0f };
    rails_.emplace_back(newRail);
}

void EditorRailManager::UpdateControlPoints(uint32_t railIndex, const std::vector<Vector3>& controlPoints) {
    assert(railIndex < rails_.size() && "Error: Invalid rail index.");
    rails_[railIndex].controlPoints = controlPoints;
    mappedBuffer_[railIndex] = rails_[railIndex];
}

void EditorRailManager::DrawImGui() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Rail Manager")) {
        for (size_t i = 0; i < rails_.size(); ++i) {
            ImGui::Text("Rail %d Progress: %.3f", i, rails_[i].progress);
        }
    }
#endif
}

const EditorRailManager::Rail& EditorRailManager::GetRail(uint32_t railIndex) const {
    assert(railIndex < rails_.size() && "Error: Invalid rail index.");
    return rails_[railIndex];
}
