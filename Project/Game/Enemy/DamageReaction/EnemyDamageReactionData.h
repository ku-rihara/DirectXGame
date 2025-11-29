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

    struct SlammedParam {
        int32_t boundNum    = 2;
        float bounceDamping = 0.5f;
    };

    struct TakeUpperParam {
        float floatingTime = 1.0f;
    };

    // リアクションパラメータ
    struct ReactionParameter {
        std::string triggerAttackName;
        int32_t intReactionState = 0;
        ReactionState reactionState;
        SlammedParam slammedParam;
        TakeUpperParam takeUpperParam;
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
    void RemoveRendition(const int32_t& index);
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
    const int32_t& GetSelectedRenditionIndex() const { return selectedRenditionIndex_; }

    void SetSelectedRenditionIndex(const int32_t& index);
};