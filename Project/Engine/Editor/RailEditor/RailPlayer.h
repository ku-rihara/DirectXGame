#pragma once
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "RailData.h"
#include "Vector3.h"
#include <memory>
#include <string>

/// <summary>
/// レールプレイヤー
/// </summary>
namespace KetaEngine {

class RailPlayer : public BaseEffectPlayer {
public:
    RailPlayer()           = default;
    ~RailPlayer() override = default;

    // 初期化、更新、再生
    void Init() override;
    void Update(float speedRate = 1.0f) override;
    void Play(const std::string& railName, const std::string& categoryName = "Common") override;

    void UpdateWithDirection(float speedRate, const RailData::PositionMode& mode = RailData::PositionMode::WORLD, const Vector3& direction = {1.0f, 1.0f, 1.0f});

private:
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    Vector3 currentPosition_ = {0.0f, 0.0f, 0.0f};
    

public:
    const Vector3& GetCurrentPosition() const { return currentPosition_; }
    RailData* GetRailData() { return dynamic_cast<RailData*>(effectData_.get()); }
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }

    void SetParent(WorldTransform* parent);
};

}; // KetaEngine