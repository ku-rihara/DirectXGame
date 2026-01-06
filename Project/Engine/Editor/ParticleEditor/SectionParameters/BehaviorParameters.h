#pragma once
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Pipeline/Particle/ParticlePipeline.h"
#include "Primitive/IPrimitive.h"
#include "utility/FileSelector/FileSelector.h"
#include "utility/TimeModeSelector/TimeModeSelector.h"
#include <cstdint>
#include <functional>
#include <string>

namespace KetaEngine {

/// <summary>
/// 動作関連パラメータ
/// </summary>
class BehaviorParameters {
public:
    // タイミングパラメータ
    struct TimingParam {
        float startTime     = 0.0f;
        float afterDuration = 0.0f;
    };

    BehaviorParameters()  = default;
    ~BehaviorParameters() = default;

    void RegisterParams(GlobalParameter* globalParam, const std::string& groupName);
    void AdaptParameters(GlobalParameter* globalParam, const std::string& groupName);
    void AdjustParam();

    // Getter
    const TimingParam& GetTimingParam() const { return timingParam_; }
    const TimeModeSelector& GetTimeModeSelector() const { return timeModeSelector_; }
    float GetIntervalTime() const { return intervalTime_; }
    float GetLifeTime() const { return lifeTime_; }
    int32_t GetParticleCount() const { return particleCount_; }
    int32_t GetMaxParticleNum() const { return maxParticleNum_; }
    int32_t GetPrimitiveTypeInt() const { return primitiveTypeInt_; }
    bool IsUseModel() const { return useModel_; }
    bool GetIsRailMove() const { return useRailMoveEmitter_; }
    bool IsShot() const { return isShot_; }
    bool IsBillBoard() const { return isBillBord_; }
    bool IsAlphaNoMove() const { return isAlphaNoMove_; }
    const std::string& GetRailFileName() const { return railFileParam_.first; }
    const std::string& GetModelFilePath() const { return modelFilePath_; }
    BlendMode GetBlendMode() const { return static_cast<BlendMode>(blendModeInt_); }
    BillboardType GetBillboardType() const { return static_cast<BillboardType>(billBordTypeInt_); }
    const AdaptRotate& GetAdaptRotate() const { return adaptRotate_; }

    // Setter
    void SetIntervalTime(float time) { intervalTime_ = time; }
    void SetLifeTime(float time) { lifeTime_ = time; }
    void SetParticleCount(int32_t count) { particleCount_ = count; }
    void SetMaxParticleNum(int32_t num) { maxParticleNum_ = num; }
    void SetIsShot(bool shot) { isShot_ = shot; }
    void SetBlendMode(BlendMode mode) { blendModeInt_ = static_cast<int32_t>(mode); }
    void SetBillboardType(BillboardType type) { billBordTypeInt_ = static_cast<int32_t>(type); }
    void SetStartTime(float time) { timingParam_.startTime = time; }
    void SetPrimitiveChangedCallback(std::function<void(PrimitiveType)> callback) {
        onPrimitiveChanged_ = callback;
    }

private:
    void SelectRailFile(const char* label, const std::string& directory, std::pair<std::string, FileSelector>* param);

    TimingParam timingParam_;
    TimeModeSelector timeModeSelector_;
    std::pair<std::string, FileSelector> railFileParam_;
    FileSelector modelFileSelector_;

    float intervalTime_       = 1.0f;
    float lifeTime_           = 1.0f;
    int32_t particleCount_    = 1;
    int32_t maxParticleNum_   = 1000;
    int32_t primitiveTypeInt_ = 0;
    int32_t billBordTypeInt_  = 0;
    int32_t blendModeInt_     = 0;

    bool isShot_             = false;
    bool useModel_           = false;
    bool useRailMoveEmitter_ = false;
    bool isBillBord_         = false;
    bool isAlphaNoMove_      = false;
    AdaptRotate adaptRotate_;

    std::string modelFilePath_;
    const std::string railFolderPath_ = "Resources/GlobalParameter/RailEditor/Dates";
    const std::string modelBasePath_  = "Resources/Model/";

    std::function<void(PrimitiveType)> onPrimitiveChanged_;
};

} // namespace KetaEngine