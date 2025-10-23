#pragma once

#include "Player/Parts/BasePlayerHand.h"

/// <summary>
/// プレイヤーの右手クラス
/// </summary>
class PlayerHandRight : public BasePlayerHand {
private:
private:
    ///===========================================
    /// private variant
    ///=============================================
    std::string threeComboName_;
    std::string forthComboName_;

    // 　移動用制御点
    std::unique_ptr<RailManager> threeComboRailManager_;
    std::unique_ptr<RailManager> stopPunchRailManager_;

    Vector3 combo3MovePos_;

public:
    PlayerHandRight()  = default;
    ~PlayerHandRight() = default;
    ///===========================================
    /// public method
    ///=============================================

    // 初期化、更新
    void Init() override;
    void Update() override;

    /// <summary>
    /// 3コンボ目のレール更新
    /// </summary>
    /// <param name="speed">速度</param>
    void RailThreeComboUpdate(const float& speed);

    /// <summary>
    /// 4コンボ目のレール更新
    /// </summary>
    /// <param name="speed">速度</param>
    void RailForthComboUpdate(const float& speed);

    void AdjustParam() override; //< パラメータの調整
    void SaveAndLoad() override; //< セーブ・ロード

    /// <summary>
    /// ディゾルブ効果の適用
    /// </summary>
    /// <param name="dissolve">ディゾルブ値</param>
    virtual void DissolveAdapt(const float& dissolve);

    ///===========================================
    /// getter method
    ///=============================================
    RailManager* GetThreeComboRailManager() { return threeComboRailManager_.get(); }
    RailManager* GetStopRailManager() { return stopPunchRailManager_.get(); }

    ///===========================================
    /// setter method
    ///=============================================
    void SetRailParent(WorldTransform* parent) override;
    void SetParent(WorldTransform* parent) override;
};