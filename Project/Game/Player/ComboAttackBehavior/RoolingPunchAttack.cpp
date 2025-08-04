/// behavior
#include "RoolingPunchAttack.h"
#include "ComboAttackRoot.h"

/// objs
#include "Player/Player.h"

/// input
#include"input/Input.h"

/// frame
#include "Frame/Frame.h"

// 初期化
RoolingPunchAttack::RoolingPunchAttack(Player* player)
    : BaseComboAattackBehavior("RoolingPunchAttack", player) {

    ///---------------------------------------------------------
    /// 変数初期化
    ///---------------------------------------------------------
}

RoolingPunchAttack::~RoolingPunchAttack() {
}

// 更新
void RoolingPunchAttack::Update() {
    BaseComboAattackBehavior::PreOderNextComboForButton();
    if (isNextCombo_) {
        BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<ComboAttackRoot>(pPlayer_));
    }
    switch (order_) {

        ///----------------------------------------------------------------------
        /// 初回処理
        /// ---------------------------------------------------------------------
    case RoolingPunchAttack::Order::START:

        break;
        ///----------------------------------------------------------------------
        /// 回転攻撃
        /// ---------------------------------------------------------------------
    case RoolingPunchAttack::Order::ROOLING:
        break;
        ///----------------------------------------------------------------------
        /// 待機
        /// ---------------------------------------------------------------------
    case RoolingPunchAttack::Order::WAIT:
    
       /* waitTine_ += Frame::DeltaTime();

        /// コンボ途切れ
        if (waitTine_ >= pPlayerParameter_->GetNormalComboParm(THIRD).waitTime) {

            order_ = Order::END;
        }

        else {
            /// ボタンで次のコンボ
            BaseComboAattackBehavior::PreOderNextComboForButton();
            if (isNextCombo_) {
                BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<JumpRush>(pPlayer_));
            }
        }*/

        break;
        break;
        ///----------------------------------------------------------------------
        /// 終了処理
        /// ---------------------------------------------------------------------
    case RoolingPunchAttack::Order::END:
        break;
    default:
        break;
    }
}

void RoolingPunchAttack::Debug() {
}
