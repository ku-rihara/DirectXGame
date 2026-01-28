#pragma once

// paremeter
#include "Editor/ParameterEditor/GlobalParameter.h"
// utility
#include "utility/FileSelector/FileSelector.h"
// std
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
        bool isCameraReset = false;
        float volume       = 1.0f; 
    };

    // オブジェクトアニメーションパラメータ
    struct ObjAnimationParam {
        std::string fileName;
        float startTiming = 0.0f;
    };

    // 振動パラメータ
    struct VibrationParam {
        float startTiming;
        float duration;
        float intensity;
        bool triggerByHit = false;
    };

    enum class Type {
        CameraAction,
        HitStop,
        ShakeAction,
        PostEffect,
        ParticleEffect,
        AudioAttack,
        AudioHit,
        Count
    };

    enum class ObjAnimationType {
        Head,
        RightHand,
        LeftHand,
        MainHead,
        Count
    };

    // 演出タイプ
    struct RenditionTypeInfo {
        Type type;
        const char* name;
        const char* label;
        const char* dir;
    };

    // オブジェクトアニメーションタイプ
    struct ObjAnimationTypeInfo {
        ObjAnimationType type;
        const char* name;
        const char* label;
    };

    // 静的な演出タイプ配列
    static constexpr RenditionTypeInfo kRenditionTypeInfos[] = {
        {Type::CameraAction, "CameraAction", "カメラアクション", "CameraAnimation/AnimationData"},
        {Type::HitStop, "HitStop", "ヒットストップ", "TimeScale"},
        {Type::ShakeAction, "ShakeAction", "シェイクアクション", "ShakeEditor"},
        {Type::PostEffect, "PostEffectParam", "ポストエフェクト", "PostEffect"},
        {Type::ParticleEffect, "ParticleEffectParam", "パーティクルエフェクト", "Particle/Player/Dates"},
        {Type::AudioAttack, "Audio_Attack", "攻撃音", "Audio"},
        {Type::AudioHit, "Audio_Hit", "ヒット音", "Audio"},
    };

    // 静的なオブジェクトアニメーションタイプ配列
    static constexpr ObjAnimationTypeInfo kObjAnimationTypeInfos[] = {
        {ObjAnimationType::Head, "ObjAnim_Head", "頭アニメーション"},
        {ObjAnimationType::RightHand, "ObjAnim_RightHand", "右手アニメーション"},
        {ObjAnimationType::LeftHand, "ObjAnim_LeftHand", "左手アニメーション"},
        {ObjAnimationType::MainHead, "Main_Head", "メイン頭アニメーション"},
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
    const std::string folderPath_      = "Resources/GlobalParameter/";
    const std::string audioFolderPath_ = "Resources/Audio/";

    // 各オブジェクトアニメーションタイプのフォルダパス
    const std::array<std::string, 4> objAnimationFolderPaths_ = {
        "Resources/GlobalParameter/ObjEaseAnimation/Player/Dates/",
        "Resources/GlobalParameter/ObjEaseAnimation/RightHand/Dates/",
        "Resources/GlobalParameter/ObjEaseAnimation/LeftHand/Dates/",
        "Resources/GlobalParameter/ObjEaseAnimation/MainHead/Dates/"};

    // 演出パラメータ配列(通常とヒット時)
    std::array<std::pair<RenditionParam, KetaEngine::FileSelector>, static_cast<size_t>(Type::Count)> renditionParams_;
    std::array<std::pair<RenditionParam, KetaEngine::FileSelector>, static_cast<size_t>(Type::Count)> renditionParamsOnHit_;

    // オブジェクトアニメーションパラメータ配列
    std::array<std::pair<ObjAnimationParam, KetaEngine::FileSelector>, static_cast<size_t>(ObjAnimationType::Count)> objAnimationParams_;

    // 振動パラメータ
    VibrationParam vibrationParam_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const RenditionParam& GetRenditionParamFromIndex(int32_t index) const {
        return GetRenditionParamFromType(static_cast<Type>(index));
    }

    const RenditionParam& GetRenditionParamFromType(const Type& type) const {
        return renditionParams_[static_cast<size_t>(type)].first;
    }

    // ヒット時用のゲッター
    const RenditionParam& GetRenditionParamOnHitFromIndex(int32_t index) const {
        return GetRenditionParamOnHitFromType(static_cast<Type>(index));
    }

    const RenditionParam& GetRenditionParamOnHitFromType(const Type& type) const {
        return renditionParamsOnHit_[static_cast<size_t>(type)].first;
    }

    const ObjAnimationParam& GetObjAnimationParamFromIndex(int32_t index) const {
        return objAnimationParams_[index].first;
    }
    const ObjAnimationParam& GetObjAnimationParamFromType(const ObjAnimationType& type) const {
        return objAnimationParams_[static_cast<size_t>(type)].first;
    }

    // 振動パラメータの取得
    const VibrationParam& GetVibrationParam() const {
        return vibrationParam_;
    }
};