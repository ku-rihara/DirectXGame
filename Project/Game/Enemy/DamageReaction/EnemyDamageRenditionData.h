#pragma once

#include "Editor/ParameterEditor/GlobalParameter.h"
#include "utility/FileSelector/FileSelector.h"
#include <array>
#include <cstdint>
#include <string>
#include <utility>

/// <summary>
/// 敵のダメージ演出データクラス
/// </summary>
class EnemyDamageRenditionData {
public:
    // オブジェクトアニメーションパラメータ
    struct ObjAnimationParam {
        std::string fileName;
        float startTiming = 0.0f;
    };

public:
    EnemyDamageRenditionData()  = default;
    ~EnemyDamageRenditionData() = default;

    //*-------------------------------- public Method --------------------------------*//

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="reactionName">リアクション名</param>
    /// <param name="renditionIndex">演出インデックス</param>
    void Init(const std::string& groupName, int32_t renditionIndex);

    void AdjustParam();
    void RegisterParams();
    void LoadData();
    void SaveData();
    void Reset();

private:
    //*-------------------------------- private Method --------------------------------*//
    void SelectObjAnimationFile(const char* label, std::pair<ObjAnimationParam, KetaEngine::FileSelector>& param);

private:
    //*-------------------------------- Private variants--------------------------------*//
    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_;
    int32_t currentRenditionIndex_            = -1;
    std::string folderPath_                   = "EnemyDamageReaction/RenditionDates/";
    const std::string objAnimationFolderPath_ = "Resources/GlobalParameter/ObjEaseAnimation/Enemy/Dates/";

    // オブジェクトアニメーションパラメータ
    std::pair<ObjAnimationParam, KetaEngine::FileSelector> objAnimationParams_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const std::string& GetGroupName() const { return groupName_; }
    const ObjAnimationParam& GetObjAnimationParam() const { return objAnimationParams_.first; }
};