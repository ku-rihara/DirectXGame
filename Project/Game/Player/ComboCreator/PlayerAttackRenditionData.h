#pragma once

// Parameter
#include "Editor/ParameterEditor/GlobalParameter.h"
// utility
#include "Utility/FileSelector/FileSelector.h"
// std
#include <array>
#include <cstdint>
#include <json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

/// <summary>
/// プレイヤー攻撃演出データクラス
/// </summary>
class PlayerAttackRenditionData {
public:
    struct RenditionParam {
        std::string fileName;
        float startTiming    = 0.0f;
        bool isCameraReset   = false;
        float volume         = 1.0f;
        bool repeatOnDamage  = false;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(RenditionParam, fileName, startTiming, isCameraReset, volume, repeatOnDamage)
    };

    // オブジェクトアニメーションパラメータ
    struct ObjAnimationParam {
        std::string fileName;
        float       startTiming   = 0.0f;
        std::string trailFileName;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ObjAnimationParam, fileName, startTiming, trailFileName)
    };

    // 振動パラメータ
    struct VibrationParam {
        float startTiming   = 0.0f;
        float duration      = 0.3f;
        float intensity     = 0.0f;
        bool triggerByHit   = false;
        bool repeatOnDamage = false;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(VibrationParam, startTiming, duration, intensity, triggerByHit, repeatOnDamage)
    };

    enum class Type {
        CameraAction,
        HitStop,
        ShakeAction,
        PostEffect,
        ParticleEffect,
        RibbonTrailEffect,
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
        {Type::CameraAction,      "CameraAction",       "カメラアクション",       "CameraAnimation/AnimationData"},
        {Type::HitStop,           "HitStop",            "ヒットストップ",         "TimeScale"},
        {Type::ShakeAction,       "ShakeAction",        "シェイクアクション",     "ShakeEditor"},
        {Type::PostEffect,        "PostEffectParam",    "ポストエフェクト",       "PostEffect"},
        {Type::ParticleEffect,    "ParticleEffectParam","パーティクルエフェクト", "Particle/Player/Dates"},
        {Type::RibbonTrailEffect, "RibbonTrailEffect",  "リボントレイル",         "RibbonTrail/Player/Dates"},
        {Type::AudioAttack,       "Audio_Attack",       "攻撃音",                 "Audio"},
        {Type::AudioHit,          "Audio_Hit",          "ヒット音",               "Audio"},
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
    void RegisterParams(KetaEngine::GlobalParameter* globalParam, const std::string& groupName, const std::string& prefix = "");

private:
    //*-------------------------------- private Method --------------------------------*//

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

    // 演出パラメータリスト
    std::array<std::vector<RenditionParam>, static_cast<size_t>(Type::Count)> renditionLists_;
    std::array<std::vector<RenditionParam>, static_cast<size_t>(Type::Count)> renditionOnHitLists_;

    // オブジェクトアニメーションパラメータリスト
    std::array<std::vector<ObjAnimationParam>, static_cast<size_t>(ObjAnimationType::Count)> objAnimationLists_;

    // 振動パラメータリスト
    std::vector<VibrationParam> vibrationList_;

    // AdjustParam用のFileSelectorキャッシュ
    std::unordered_map<std::string, KetaEngine::FileSelector> fileSelectorMap_;

public:
    //*-------------------------------- Getter Method --------------------------------*//

    // 通常演出
    const std::vector<RenditionParam>& GetRenditionListFromIndex(int32_t index) const {
        return renditionLists_[index];
    }
    const std::vector<RenditionParam>& GetRenditionListFromType(const Type& type) const {
        return renditionLists_[static_cast<size_t>(type)];
    }
    void ClearRenditionList(const Type& type) { renditionLists_[static_cast<size_t>(type)].clear(); }
    void AddRenditionEntry(const Type& type, const RenditionParam& param) {
        renditionLists_[static_cast<size_t>(type)].push_back(param);
    }

    // ヒット時演出
    const std::vector<RenditionParam>& GetRenditionOnHitListFromIndex(int32_t index) const {
        return renditionOnHitLists_[index];
    }
    const std::vector<RenditionParam>& GetRenditionOnHitListFromType(const Type& type) const {
        return renditionOnHitLists_[static_cast<size_t>(type)];
    }
    void ClearRenditionOnHitList(const Type& type) { renditionOnHitLists_[static_cast<size_t>(type)].clear(); }
    void AddRenditionOnHitEntry(const Type& type, const RenditionParam& param) {
        renditionOnHitLists_[static_cast<size_t>(type)].push_back(param);
    }

    // オブジェクトアニメーション
    const std::vector<ObjAnimationParam>& GetObjAnimationListFromIndex(int32_t index) const {
        return objAnimationLists_[index];
    }
    const std::vector<ObjAnimationParam>& GetObjAnimationListFromType(const ObjAnimationType& type) const {
        return objAnimationLists_[static_cast<size_t>(type)];
    }
    void ClearObjAnimationList(const ObjAnimationType& type) { objAnimationLists_[static_cast<size_t>(type)].clear(); }
    void AddObjAnimationEntry(const ObjAnimationType& type, const ObjAnimationParam& param) {
        objAnimationLists_[static_cast<size_t>(type)].push_back(param);
    }

    // 振動
    const std::vector<VibrationParam>& GetVibrationList() const { return vibrationList_; }
    void ClearVibrationList() { vibrationList_.clear(); }
    void AddVibrationEntry(const VibrationParam& param) { vibrationList_.push_back(param); }
};
