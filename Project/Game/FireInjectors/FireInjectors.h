#pragma once
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "utility/PutObjForBlender/PutObjForBlender.h"
#include <cstdint>
#include <memory>
#include <string>

class Combo;

/// <summary>
/// 炎噴射の演出装置クラス
/// </summary>
class FireInjectors {
public:

    // 初期化、更新
    void Init();
    void Update();

    void Spawn();  //< 生成
    void Launch(); //< 発射

    void AdjustParam();    //< パラメータ調整
    void RegisterParams(); //< パラメータバインド

private:
    std::unique_ptr<KetaEngine::PutObjForBlender> putObjForBlender_ = nullptr;
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "FireInjectors";
    Combo* pCombo_;
    int32_t fireShotComboNum_;
    int prevComboCount_ = 0;
    bool isEasing_;

public:
    // setter
    void SetCombo(Combo* combo);
};
