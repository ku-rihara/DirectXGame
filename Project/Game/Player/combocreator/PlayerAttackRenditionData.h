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
        MainHead,
        Count
    };

public:
    PlayerAttackRenditionData()  = default;
    ~PlayerAttackRenditionData() = default;

    //*-------------------------------- public Method --------------------------------*//

    void AdjustParam();
    void RegisterParams(KetaEngine::GlobalParameter* globalParam, const std::string& groupName);

private:
    //*-------------------------------- private Method --------------------------------*//
    void SelectRenditionFile(const char* label, const std::string& directory, std::pair<RenditionParam, KetaEngine::FileSelector>& param);
    void SelectObjAnimationFile(const char* label, const std::string& directory, std::pair<ObjAnimationParam, KetaEngine::FileSelector>& param);

    // オブジェクトアニメーションタイプに応じたフォルダパスを取得
    std::string GetObjAnimationFolderPath(ObjAnimationType type) const;

private:
    //*-------------------------------- Private variants--------------------------------*//
    std::string groupName_;
    const std::string folderPath_ = "Resources/GlobalParameter/";

    // 各オブジェクトアニメーションタイプのフォルダパス
    const std::string objAnimationFolderPath_Head_      = "Resources/GlobalParameter/ObjEaseAnimation/Player/Dates/";
    const std::string objAnimationFolderPath_RightHand_ = "Resources/GlobalParameter/ObjEaseAnimation/RightHand/Dates/";
    const std::string objAnimationFolderPath_LeftHand_  = "Resources/GlobalParameter/ObjEaseAnimation/LeftHand/Dates/";
    const std::string objAnimationFolderPath_MainHead_  = "Resources/GlobalParameter/ObjEaseAnimation/MainHead/Dates/";

    // 演出パラメータ配列
    std::array<std::pair<RenditionParam, KetaEngine::FileSelector>, static_cast<size_t>(Type::Count)> renditionParams_;

    // オブジェクトアニメーションパラメータ配列
    std::array<std::pair<ObjAnimationParam, KetaEngine::FileSelector>, static_cast<size_t>(ObjAnimationType::Count)> objAnimationParams_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const RenditionParam& GetRenditionParamFromIndex(int32_t index) const;
    const RenditionParam& GetRenditionParamFromType(const Type& type) const {
        return renditionParams_[static_cast<size_t>(type)].first;
    }

    const ObjAnimationParam& GetObjAnimationParamFromIndex(int32_t index) const {
        return objAnimationParams_[index].first;
    }
    const ObjAnimationParam& GetObjAnimationParamFromType(const ObjAnimationType& type) const {
        return objAnimationParams_[static_cast<size_t>(type)].first;
    }
};