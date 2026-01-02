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
        ParticleEffect,
        Count
    };

    enum class ObjAnimationType {
        Head,
        RightHand,
        LeftHand,
        MainHead,
        Count
    };

private:
    // 演出タイプ情報構造体
    struct RenditionTypeInfo {
        Type type;
        const char* name;
        const char* label;
        const char* dir;
    };

    // オブジェクトアニメーションタイプ情報構造体
    struct ObjAnimationTypeInfo {
        ObjAnimationType type;
        const char* name;
        const char* label;
    };

    // 静的な演出タイプ情報配列
    static constexpr RenditionTypeInfo kRenditionTypeInfos[] = {
        {Type::CameraAction, "CameraAction", "Camera Action", "CameraAnimation/AnimationData"},
        {Type::HitStop, "HitStop", "Hit Stop", "TimeScale"},
        {Type::ShakeAction, "ShakeAction", "Shake Action", "ShakeEditor"},
        {Type::PostEffect, "PostEffectParam", "PostEffect", "PostEffect"},
        {Type::ParticleEffect, "ParticleEffectParam", "ParticleEffect", "Particle/Player/Dates"},
    };

    // 静的なオブジェクトアニメーションタイプ情報配列
    static constexpr ObjAnimationTypeInfo kObjAnimationTypeInfos[] = {
        {ObjAnimationType::Head, "ObjAnim_Head", "Head Animation"},
        {ObjAnimationType::RightHand, "ObjAnim_RightHand", "Right Hand Animation"},
        {ObjAnimationType::LeftHand, "ObjAnim_LeftHand", "Left Hand Animation"},
        {ObjAnimationType::MainHead, "Main_Head", "Main Head Animation"},
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
    const std::array<std::string, 4> objAnimationFolderPaths_ = {
        "Resources/GlobalParameter/ObjEaseAnimation/Player/Dates/",
        "Resources/GlobalParameter/ObjEaseAnimation/RightHand/Dates/",
        "Resources/GlobalParameter/ObjEaseAnimation/LeftHand/Dates/",
        "Resources/GlobalParameter/ObjEaseAnimation/MainHead/Dates/"};

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