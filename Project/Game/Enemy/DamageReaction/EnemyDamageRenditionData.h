#pragma once

#include "Editor/ParameterEditor/GlobalParameter.h"
#include "utility/FileSelector/FileSelector.h"
#include <array>
#include <cstdint>
#include <string>
#include <utility>

/// <summary>
/// プレイヤー攻撃演出データクラス
/// </summary>
class EnemyDamageRenditionData {
public:
    struct RenditionParam {
        std::string fileName;
        float startTiming = 0.0f;
        float currentTime_;
        bool triggerByHit = false;
    };

    // オブジェクトアニメーションパラメータ
    struct ObjAnimationParam {
        std::string fileName;
        float startTiming = 0.0f;
    };

public:
    EnemyDamageRenditionData()  = default;
    ~EnemyDamageRenditionData() = default;

    //*-------------------------------- public Method --------------------------------*//

    void AdjustParam();
    void BindParams(GlobalParameter* globalParam, const std::string& groupName);

private:
    //*-------------------------------- private Method --------------------------------*//
    void SelectRenditionFile(const char* label, const std::string& directory, std::pair<RenditionParam, FileSelector>& param);
    void SelectObjAnimationFile(const char* label, std::pair<ObjAnimationParam, FileSelector>& param);

private:
    //*-------------------------------- Private variants--------------------------------*//
    std::string groupName_;
    const std::string folderPath_             = "Resources/GlobalParameter/";
    const std::string objAnimationFolderPath_ = "Resources/GlobalParameter/ObjEaseAnimation/Enemy/";

  
    // オブジェクトアニメーションパラメータ配列
    std::pair<ObjAnimationParam, FileSelector> objAnimationParams_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const RenditionParam& GetRenditionParamFromIndex(const int32_t& index) const;

    const ObjAnimationParam& GetObjAnimationParamFromIndex() const {return objAnimationParams_.first;}
    const ObjAnimationParam& GetObjAnimationParamFromType() const {return objAnimationParams_.first;
    }
};