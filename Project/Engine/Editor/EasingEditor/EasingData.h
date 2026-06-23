#pragma once
#include "Editor/BaseEffectEditor/BaseEffectData.h"
#include "Editor/Easing/Easing.h"
#include "Editor/Easing/EasingParameterData.h"
#include "Vector2.h"
#include "Vector3.h"
#include <string>

namespace KetaEngine {

template <typename T>
class EasingData : public BaseEffectData {
public:
    EasingData()           = default;
    ~EasingData() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectDataからのオーバーライド
    void Init(const std::string& name, const std::string& categoryName) override;
    void Update(float speedRate = 1.0f) override;
    void Reset() override;
    void Play() override;

    // パラメータ調整
    void AdjustParam();

    static std::string GetStaticFolderName();

protected:
    //*---------------------------- protected Methods ----------------------------*//

    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

private:
    //*---------------------------- private Variant ----------------------------*//

    EasingParameter<T> param_;
    Easing<T> previewEasing_;
    T previewValue_ = {};

    // enum を int32_t として GlobalParameter に登録するための中継変数
    int32_t typeInt_       = 0;
    int32_t finishTypeInt_ = 0;
    int32_t returnTypeInt_ = 0;

public:
    //*----------------------------- getter Methods -----------------------------*//

    const EasingParameter<T>& GetParam() const { return param_; }
    const T& GetPreviewValue() const { return previewValue_; }
};

}; // KetaEngine
