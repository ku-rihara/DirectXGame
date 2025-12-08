#pragma once

#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "EnemyDamageRenditionData.h"
#include "utility/FileSelector/FileSelector.h"
#include "Vector3.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// ダメージリアクションデータクラス
/// </summary>
class EnemyDamageReactionData {
public:
    enum class ReactionState {
        Normal,
        Slammed,
        TakeUpper,
    };

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
        float rotateSpeed;
    };

    struct TakeUpperParam {
        float floatingTime;
        float fallSpeedLimit;
        float gravity;
        float rotateSpeed;
    };

    // リアクションパラメータ
    struct ReactionParameter {
        std::string triggerAttackName;
        int32_t intReactionState = 0;
        ReactionState reactionState;
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

    // パラメータバインド、調節
    void AdjustParam();
    void RegisterParams();

    // データロード、セーブ
    void LoadData();
    void SaveData();

    // 演出データ追加、削除、クリア、初期化
    void AddRendition();
    void RemoveRendition(int32_t index);
    void ClearRenditions();
    void InitRenditions();

    // 保存、読み込み
    void SaveAllRenditions();
    void LoadRenditions();

private:
    //*-------------------------------- private Method --------------------------------*//

    // 次の攻撃の選択
    void SelectTriggerAttack();
    void CreateOrLoadRendition(const std::vector<std::pair<int32_t, std::string>>& renditionFiles);

private:
    //*-------------------------------- Private variants--------------------------------*//

    // グローバルパラメータ
    GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string folderPath_          = "EnemyDamageReaction/ReactionDates";
    const std::string renditionFolderPath_ = "Resources/GlobalParameter/EnemyDamageReaction/RenditionDates/";

    // 演出データリスト
    std::vector<std::unique_ptr<EnemyDamageRenditionData>> renditions_;
    int32_t selectedRenditionIndex_ = -1;
    bool showRenditionList_         = true;

    FileSelector fileSelector_;

    // リアクションパラメータ
    ReactionParameter reactionParam_;
    std::vector<std::string> reactionFileNames_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const std::string& GetGroupName() const { return groupName_; }
    const ReactionParameter& GetReactionParam() const { return reactionParam_; }
    ReactionState GetReactionState() const { return static_cast<ReactionState>(reactionParam_.intReactionState); }

    // 演出データ取得
    const std::vector<std::unique_ptr<EnemyDamageRenditionData>>& GetAllRenditions() const { return renditions_; }
    int32_t GetRenditionCount() const { return static_cast<int32_t>(renditions_.size()); }
    const EnemyDamageRenditionData* GetSelectedRendition() const;
    int32_t GetSelectedRenditionIndex() const { return selectedRenditionIndex_; }

    void SetSelectedRenditionIndex(int32_t index);
};