#pragma once
#include "2d/Sprite.h"
#include "Easing/Easing.h"
#include "Vector2.h"
#include <array>
#include <memory>

class ViewProjection;
class JumpAttackUI {
public:
    enum Type {
        X,
        Y,
        B,
        A,
        COUNT
    };

    enum class State {
        WAIT,
        OPEN,
        CLOSE,
    };

    struct BottomParam {
        std::unique_ptr<Sprite> sprite;
        Vector2 discreteDirection;
    };

    struct EasingParam {
        std::unique_ptr<Easing<float>> posEasing;
        std::unique_ptr<Easing<float>> rotateEasing;
        std::unique_ptr<Easing<Vector2>> scaleEasing;
        float posEaseValue;
        float rotateEaseValue;
        Vector2 scaleEaseValue;
    };

public:
    JumpAttackUI()  = default;
    ~JumpAttackUI() = default;

    // 初期化、更新、描画
    void Init();
    void Update(const Vector3& basePos, const ViewProjection& viewProjection);
    void Draw();

    // param Edit
    void BindParams();
    void AdjustParam();

private:
    void DirectionByBottom(const size_t& TypeNum);
    static void (JumpAttackUI::* spFuncTable_[])();

    void EasingInit();

    // state
    void Wait();
    void Open();
    void Close();

private:
    // global param
    GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_      = "JumpAttackUI";

    // スプライト
    std::array<BottomParam, static_cast<size_t>(Type::COUNT)> bottoms_;
    State state_ = State::WAIT;
    EasingParam easingParam_;
    

public:
};