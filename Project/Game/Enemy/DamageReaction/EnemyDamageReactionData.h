#pragma once

#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "EnemyDamageRenditionData.h"
#include "utility/FileSelector/FileSelector.h"
#include "Vector3.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// ダメージリアクションデータクラス
/// </summary>
class EnemyDamageReactionData {
public:
    // Normal状態のパラメータ
    struct NormalParam {
        float knockBackTime;
        float knockBackDamping;
    };

    // バウンド共通パラメータ
    struct BoundParam {
        int32_t boundNum;
        float bounceDamping;
        float initialBounceRate;
    };

    struct SlammedParam {
        float gravity;
        Vector3 rotateSpeed;
    };

    struct TakeUpperParam {
        float floatingTime;
        float fallSpeedLimit;
        float gravity;
        Vector3 rotateSpeed;
    };

    // 敵タイプ数
    static constexpr int kEnemyTypeCount = 2;

    // リアクションパラメータ
    struct ReactionParameter {
        std::string triggerAttackName;
        std::array<std::string, kEnemyTypeCount> damageAnimationNames; 
        NormalParam normalParam;
        BoundParam boundParam;
        SlammedParam slammedParam;
        TakeUpperParam takeUpperParam;
        float damageCollingTime;
        bool isPriorityReaction;
    };

public:
    EnemyDamageReactionData()  = default;
    ~EnemyDamageReactionData() = default;

    //*-------------------------------- public Method --------------------------------*//

    // 初期化
    void Init(const std::string& reactionName);

private:

public:

    // パラメータバインド、調節
    void AdjustParam(const std::vector<std::string>& availableAnimations);
    void RegisterParams();

    // データロード、セーブ
    void LoadData();
    void SaveData();

    // 演出データ初期化
    void InitRendition();

    // 保存、読み込み
    void SaveRendition();
    void LoadRendition();

private:
    //*-------------------------------- private Method --------------------------------*//

    // 次の攻撃の選択
    void SelectTriggerAttack();

private:
    //*-------------------------------- Private variants--------------------------------*//

    // グローバルパラメータ
    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string folderPath_          = "EnemyDamageReaction/ReactionDates";
    const std::string renditionFolderPath_ = "Resources/GlobalParameter/EnemyDamageReaction/RenditionDates/";

    // 演出データ
    std::unique_ptr<EnemyDamageRenditionData> rendition_;

    KetaEngine::FileSelector fileSelector_;

    // リアクションパラメータ
    ReactionParameter reactionParam_;
    std::vector<std::string> reactionFileNames_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const std::string& GetGroupName() const { return groupName_; }
    const ReactionParameter& GetReactionParam() const { return reactionParam_; }

    // 演出データ取得
    const EnemyDamageRenditionData* GetRendition() const { return rendition_.get(); }
    bool HasRendition() const { return rendition_ != nullptr; }
};