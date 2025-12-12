#pragma once

#include "Player/Parts/BasePlayerHand.h"

/// <summary>
/// プレイヤーの左手クラス
/// </summary>
class PlayerHandLeft : public BasePlayerHand {
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
    void RailForthComboUpdate(float speed);

    void AdjustParam() override; //< パラメータの調整
    void SaveAndLoad() override; //< セーブ・ロード

    /// <summary>
    /// ディゾルブ効果の適用
    /// </summary>
    /// <param name="dissolve">ディゾルブ値</param>
    void DissolveAdapt(float dissolve) override;

private:
    ///===========================================
    /// private variant
    ///=============================================

    std::string railGroupName_;
    std::unique_ptr<KetaEngine::RailManager> trustRailManager_;

public:
    ///===========================================
    /// getter method
    ///=============================================
    KetaEngine::RailManager* GetThrustRailManager() { return trustRailManager_.get(); }

    ///===========================================
    /// setter method
    ///=============================================
    void SetRailParent(KetaEngine::WorldTransform* parent) override;
    void SetParent(KetaEngine::WorldTransform* parent) override;
};