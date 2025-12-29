#pragma once
#include "Animation/Object3DAnimation.h"
#include "BaseObject/BaseObject.h"
#include "Behavior/BaseAudienceBehavior.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector2.h"
#include "Vector3.h"
#include <cstdint>
#include <memory>
#include <string>

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
class Audience : public BaseObject {
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

    // changeBehavior
    void ChangeBehavior(std::unique_ptr<BaseAudienceBehavior> behavior);
    void AppearByComboLevel(int32_t level);
    void DisAppearByComboLevel(int32_t level);

private:
    // オブジェクト生成
    void CreateObject();
    // パラメータ登録
    void RegisterParams();
    // RotateY適応
    void RotateYChangeBySeatSide(SeatSide seatSide);

private:
    // global parameter
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    std::string groupName_                        = "Audience";
    const std::string folderName_                 = "Audiences";

    // index
    int32_t seatsRowIndex_ = 0;
    int32_t seatSideIndex_ = 0;
    int32_t audienceIndex_ = 0;

    //
    int32_t seatRowNum_;
    int32_t appearComboLevel_;

    // enum
    SeatSide seatSide_ = SeatSide::LEFT;

    // animationObj
    std::unique_ptr<KetaEngine::Object3DAnimation> objAnimation_;
    float positionX_;

    float cheerAnimationTimer_;

    // behavior
    std::unique_ptr<BaseAudienceBehavior> behavior_;

public:
    KetaEngine::Object3DAnimation* GetObjAnimation() { return objAnimation_.get(); }
    int32_t GetSeatRowNum() const { return seatRowNum_; }
    AudienceRoot* GetAudienceRoot() const;

    void SetBaseScale(Vector3 scale);
    void SetCheerAnimationTimer(float timer) { cheerAnimationTimer_ = timer; }
};