#pragma once

#include "Easing/Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include <cstdint>
#include <string>
#include <vector>

/// <summary>
/// プレイヤー攻撃演出データクラス
/// </summary>
class PlayerAttackRenditionData {
public:
    struct RenditionParam {
        std::string fileName;
        float startTiming;
        bool triggerByHit;
    };

public:
    PlayerAttackRenditionData()  = default;
    ~PlayerAttackRenditionData() = default;

    //*-------------------------------- public Method --------------------------------*//

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="attackName">攻撃名</param>
    void Init(const std::string& attackName);

    // パラメータバインド、調節
    void AdjustParam();
    void BindParams();

    // データロード、セーブ
    void LoadData();
    void SaveData();

private:
    //*-------------------------------- private Method --------------------------------*//

    // 次の攻撃の選択
    void SelectNextAttack();

private:
    //*-------------------------------- Private variants--------------------------------*//

    GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string folderPath_ = "AttackRenditionData";

    RenditionParam cameraAction_;
    RenditionParam hitStopParam_;
    RenditionParam shakeAction_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const std::string& GetGroupName() const { return groupName_; }
    const RenditionParam& GetCameraAction() { return cameraAction_; }
    const RenditionParam& GetHitStopParam() { return hitStopParam_; }
    const RenditionParam& GetShakeAction() { return shakeAction_; }
};