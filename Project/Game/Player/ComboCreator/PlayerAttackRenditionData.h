#pragma once

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
        float duration;
        bool triggerByHit;
    };

public:
    PlayerAttackRenditionData()  = default;
    ~PlayerAttackRenditionData() = default;

    //*-------------------------------- public Method --------------------------------*//

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="groupName">グループ名</param>
    void Init(const std::string& groupName);

    // パラメータバインド、調整
    void AdjustParam();
    void BindParams(GlobalParameter* globalParam, const std::string& groupName);

private:
    //*-------------------------------- private Method --------------------------------*//

    // 演出パラメータの選択UI
    void SelectRenditionFile(const char* label, const std::string& directory, RenditionParam& param);

private:
    //*-------------------------------- Private variants--------------------------------*//

    std::string groupName_;

    // 演出パラメータ
    RenditionParam cameraAction_;
    RenditionParam hitStopParam_;
    RenditionParam shakeAction_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const RenditionParam& GetCameraAction() const { return cameraAction_; }
    const RenditionParam& GetHitStopParam() const { return hitStopParam_; }
    const RenditionParam& GetShakeAction() const { return shakeAction_; }
};