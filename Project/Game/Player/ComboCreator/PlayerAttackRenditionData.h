#pragma once

#include "utility/FileSelector/FileSelector.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <string>

/// <summary>
/// プレイヤー攻撃演出データクラス
/// </summary>
class PlayerAttackRenditionData {
public:
    struct RenditionParam {
        std::string fileName;
        float startTiming;
        bool triggerByHit;
        FileSelector fileSelector;
    };

public:
    PlayerAttackRenditionData()  = default;
    ~PlayerAttackRenditionData() = default;

    //*-------------------------------- public Method --------------------------------*//


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
    const std::string folderPath_ = "Resources/GlobalParameter/";

    // 演出パラメータ
    RenditionParam cameraAction_;
    RenditionParam hitStopParam_;
    RenditionParam shakeAction_;
    RenditionParam postEffectParam_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const RenditionParam& GetCameraAction() const { return cameraAction_; }
    const RenditionParam& GetHitStopParam() const { return hitStopParam_; }
    const RenditionParam& GetShakeAction() const { return shakeAction_; }
    const RenditionParam& GetPostEffectParam() const { return postEffectParam_; }
};