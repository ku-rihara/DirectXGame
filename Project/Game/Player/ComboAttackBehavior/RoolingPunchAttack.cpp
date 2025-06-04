/// behavior
#include "RoolingPunchAttack.h"
#include "RightJobPunch.h"

/// objs
#include "Player/Player.h"

/// input
#include "JoyState/JoyState.h"

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
