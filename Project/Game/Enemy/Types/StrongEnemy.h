#pragma once

#include "BaseEnemy.h"

/// <summary>
/// 強敵クラス
/// </summary>
class StrongEnemy : public BaseEnemy {
public:
    struct StrongParameter {
        float tauntRange = 15.0f;
    };

public:
    StrongEnemy()           = default;
    ~StrongEnemy() override = default;

    void Init(const Vector3& spownPos) override;
    void Update() override;
    void SpawnRenditionInit() override;
   
    void StartTaunt();
    void StopTaunt();

private:
    bool isTaunting_ = false;
    StrongParameter strongParam_;

public:
    const StrongParameter& GetStrongParameter() const { return strongParam_; }
    void SetStrongParameter(const StrongParameter& param) { strongParam_ = param; }
};