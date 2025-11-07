#pragma once

#include "utility/FileSelector/FileSelector.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <array>
#include <string>
#include <utility>
#include <cstdint>

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

    enum class Type {
        CameraAction,
        HitStop,
        ShakeAction,
        PostEffect,
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

private:
    //*-------------------------------- Private variants--------------------------------*//
    std::string groupName_;
    const std::string folderPath_ = "Resources/GlobalParameter/";

    // 演出パラメータ配列
    std::array<std::pair<RenditionParam, FileSelector>, static_cast<size_t>(Type::Count)> renditionParams_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const RenditionParam& GetRenditionParamFromIndex(const int32_t& index) const;
    const RenditionParam& GetRenditionParamFromType(const Type& type) const {
        return renditionParams_[static_cast<size_t>(type)].first;
    }
};
