#pragma once

#include "utility/FileSelector/FileSelector.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <array>
#include <cstdint>
#include <string>
#include <utility>

/// <summary>
/// プレイヤー攻撃演出データクラス
/// </summary>
class PlayerAttackRenditionData {
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

    enum class Type {
        CameraAction,
        HitStop,
        ShakeAction,
        PostEffect,
        Count
    };

    enum class ObjAnimationType {
        Head,
        RightHand,
        LeftHand,
        Count
    };

public:
    PlayerAttackRenditionData()  = default;
    ~PlayerAttackRenditionData() = default;

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
    const std::string objAnimationFolderPath_ = "Resources/GlobalParameter/ObjEaseAnimation/Player/";

    // 演出パラメータ配列
    std::array<std::pair<RenditionParam, FileSelector>, static_cast<size_t>(Type::Count)> renditionParams_;

    // オブジェクトアニメーションパラメータ配列
    std::array<std::pair<ObjAnimationParam, FileSelector>, static_cast<size_t>(ObjAnimationType::Count)> objAnimationParams_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const RenditionParam& GetRenditionParamFromIndex(const int32_t& index) const;
    const RenditionParam& GetRenditionParamFromType(const Type& type) const {
        return renditionParams_[static_cast<size_t>(type)].first;
    }

    const ObjAnimationParam& GetObjAnimationParamFromIndex(const int32_t& index) const {
        return objAnimationParams_[index].first;
    }
    const ObjAnimationParam& GetObjAnimationParamFromType(const ObjAnimationType& type) const {
        return objAnimationParams_[static_cast<size_t>(type)].first;
    }
};