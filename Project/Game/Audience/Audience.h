#pragma once
#include "Animation/Object3DAnimation.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Behavior/BaseAudienceBehavior.h"
#include "Vector2.h"
#include "Vector3.h"
#include <cstdint>
#include <memory>
#include <string>

enum class SeatsRow {
    FRONT,
    MIDDLE,
    BACK,
    COUNT,
};

enum class SeatSide {
    LEFT,
    RIGHT,
    FRONT,
    BACK,
    COUNT,
};

class AudienceRoot;

/// <summary>
/// 観客
/// </summary>
class Audience {
public:
    Audience()  = default;
    ~Audience() = default;

    // 初期化、更新
    void Init(int32_t index);
    void Update();

    // parameter
    void AdjustParam();

    // adaptPos
    void AdaptPosition(const Vector2& ZXBasePos);

    // change Behabior
     void ChangeBehavior(std::unique_ptr<BaseAudienceBehavior> behavior);

private:

    // パラメータ登録
    void RegisterParams();

    // RotateY適応
    void RotateYChangeBySeatSide(SeatSide seatSide);
    
     // changeBehavior
    void AppearByComboLevel(int32_t level);
    void CloseByComboLevel(int32_t level);

private:
    // global parameter
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    std::string groupName_                        = "Audience";
    const std::string folderName_                 = "Application/Audiences";

    // index
    int32_t seatsRowIndex_ = 0;
    int32_t seatSideIndex_ = 0;
    int32_t audienceIndex_ = 0;

    // 
    int32_t appearComboLevel_;

    // enum
    SeatsRow seatsRow_ = SeatsRow::FRONT;
    SeatSide seatSide_ = SeatSide::LEFT;

    // animationObj
    std::unique_ptr<KetaEngine::Object3DAnimation> objAnimation_;
    KetaEngine::WorldTransform baseTransform_;
    float positionX_;

    // behavior
    std::unique_ptr<BaseAudienceBehavior> behavior_;

public:
    AudienceRoot* GetAudienceRoot()const;

    SeatsRow GetSeatsRow() const { return seatsRow_; }
};