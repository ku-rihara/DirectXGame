#pragma once
#include "3d/ViewProjection.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "utility/PutObjForBlender/PutObjForBlender.h"
#include <cstdint>
#include <memory>
#include <string>

class Combo;
class FireInjectors {
public:
    void Init();
    void Update();
    void Draw(const ViewProjection& viewProjection);

    void Spawn();
    void Launch();

    ///-------------------------------------------------------------------------------------
    /// Editor
    ///-------------------------------------------------------------------------------------
    void AdjustParam();
    void BindParams();

private:
    std::unique_ptr<PutObjForBlender> putObjForBlender_ = nullptr;

    GlobalParameter* globalParameter_; /// グローバルパラメータ
    const std::string groupName_ = "FireInjectors"; /// グループ名

    Combo* pCombo_;

    int32_t fireShotComboNum_;

      int prevComboCount_     = 0;
  
    bool isEasing_;

public:
    void SetCombo(Combo* combo);
};