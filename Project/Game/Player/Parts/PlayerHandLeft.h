#pragma once

#include "Player/Parts/BasePlayerHand.h"

/// <summary>
/// プレイヤーの左手クラス
/// </summary>
class PlayerHandLeft : public BasePlayerHand {
private:
private:
    ///===========================================
    /// private variant
    ///=============================================

    std::string railGroupName_;
    std::unique_ptr<RailManager> trustRailManager_;

public:
    PlayerHandLeft()  = default;
    ~PlayerHandLeft() = default;
    ///===========================================
    /// public method
    ///=============================================

    // 初期化、更新
    void Init() override;
    void Update() override;

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
    void DissolveAdapt(const float& dissolve) override;

    ///===========================================
    /// getter method
    ///=============================================
    RailManager* GetThrustRailManager() { return trustRailManager_.get(); }

    ///===========================================
    /// setter method
    ///=============================================
    void SetRailParent(WorldTransform* parent) override;
    void SetParent(WorldTransform* parent) override;
};