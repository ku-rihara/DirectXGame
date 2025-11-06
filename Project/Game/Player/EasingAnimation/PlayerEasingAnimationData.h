//#pragma once
//
//#include "Easing/Easing.h"
//#include "PlayerEasingAnimationData.h"
//#include "utility/FileSelector/FileSelector.h"
//#include "utility/ParameterEditor/GlobalParameter.h"
//#include "Vector3.h"
//#include <cstdint>
//#include <string>
//#include <vector>
//
///// <summary>
///// プレイヤーイージングアニメーションデータクラス
///// </summary>
//class PlayerEasingAnimationData {
//public:
//
//    struct HandParam {
//    };
//
//    // アニメーションパラメータ
//    struct AnimationParameters {
//
//    };
//
//public:
//    PlayerEasingAnimationData()  = default;
//    ~PlayerEasingAnimationData() = default;
//
//    //*-------------------------------- public Method --------------------------------*//
//
//    /// <summary>
//    /// 初期化
//    /// </summary>
//    /// <param name="attackName">攻撃名</param>
//    void Init(const std::string& attackName);
//
//    // パラメータバインド、調節
//    void AdjustParam();
//    void RegisterParams();
//
//    // データロード、セーブ
//    void LoadData();
//    void SaveData();
//
//private:
//    //*-------------------------------- private Method --------------------------------*//
//
//    // 次の攻撃の選択
//    void SelectNextAttack();
//
//private:
//    //*-------------------------------- Private variants--------------------------------*//
//
//    GlobalParameter* globalParameter_;
//    std::string groupName_;
//    const std::string folderPath_ = "PlayerEasingAnimation";
//
//   /* PlayerEasingAnimationData renditionData_;*/
//    FileSelector fileSelector_;
//
//    // 攻撃パラメータ
//    AttackParameter attackParam_;
//    bool needsRefresh_ = true;
//    std::vector<std::string> attackFileNames_;
//
//    int32_t tempCondition_;
//
//public:
//    //*-------------------------------- Getter Method --------------------------------*//
//    const std::string& GetGroupName() const { return groupName_; }
//    AttackParameter& GetAttackParam() { return attackParam_; }
//};