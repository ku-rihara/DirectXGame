#pragma once

#include "BackGroundObject/GameBackGroundObject.h"
#include "ComboLevel/ComboLevelObjHolder.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "utility/DitherOcclusion/DitherOcclusion.h"

#include <memory>
#include <string>
#include <vector>

class BackGroundObjectManager {
public:
    BackGroundObjectManager()  = default;
    ~BackGroundObjectManager() = default;

    // 初期化、更新、パラメータ編集
    void Init();
    void Update(float playSpeed);
    void AdjustParam();

private:
    void RegisterParams();
    void InitDitherOcclusion();

private:
    struct Entry {
        std::string filename;
        int32_t     comboLevel    = 0;
        bool        isPulseMaster = false;
    };

    // 背景オブジェクトの定義
    std::vector<Entry> entries_ = {
        { "ComboLevel1.json", 0, true  },
        { "ComboLevel2.json", 1, false },
    };

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_ = "BackGroundObjectManager";

    std::unique_ptr<GameBackGroundObject>      gameBackGroundObject_;
    std::unique_ptr<ComboLevelObjHolder>       comboLevelObjHolder_;
    std::unique_ptr<KetaEngine::DitherOcclusion> ditherOcclusion_;

public:
    ComboLevelObjHolder* GetComboLevelObjHolder() const { return comboLevelObjHolder_.get(); }
    GameBackGroundObject* GetGameBackGroundObject() const { return gameBackGroundObject_.get(); }
    KetaEngine::DitherOcclusion* GetDitherOcclusion() const { return ditherOcclusion_.get(); }
};
