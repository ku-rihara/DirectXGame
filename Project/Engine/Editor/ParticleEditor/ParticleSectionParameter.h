#pragma once
#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "MinMax.h"
#include "Parameter/ParticleCommonParameters.h"
#include "Pipeline/Particle/ParticlePipeline.h"
#include "Primitive/IPrimitive.h"
#include "utility/FileSelector/FileSelector.h"
#include "utility/TimeModeSelector/TimeModeSelector.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cstdint>
#include <functional>
#include <string>
namespace KetaEngine {

class Object3DAnimation;
class WorldTransform;

/// <summary>
/// ParticleSection用パラメータ管理クラス
/// </summary>
class ParticleSectionParameter {
public:
    // 共通パラメータの型エイリアス
    using Parameters      = ParticleCommon::Parameters;
    using GroupParamaters = ParticleCommon::GroupParamaters;
    using ScaleEaseParm   = ParticleCommon::ScaleEaseParam;
    using UVParm          = ParticleCommon::UVParam;
    using JointParent     = ParticleCommon::JointParent;

    // タイミングパラメータ
    struct TimingParam {
        float startTime     = 0.0f;
        float afterDuration = 0.0f;
    };

    // エミッター位置制御タイプ
    enum class EmitterPositionMode {
        None = 0,
        ParentTransform,
        TargetPosition,
        FollowingPosition,
        ParentJoint
    };

public:
    ParticleSectionParameter()  = default;
    ~ParticleSectionParameter() = default;

    void RegisterParams(GlobalParameter* globalParam, const std::string& groupName);
    void AdaptParameters(GlobalParameter* globalParam, const std::string& groupName);

    void AdjustParam();
    void ImGuiTextureSelection();
    void ApplyTexture(const std::string& textureName);
    void InitAdaptTexture();

private:
    void SelectRailFile(const char* label, const std::string& directory, std::pair<std::string, FileSelector>* param);
    void ScaleParamEditor();
    void AdaptIntToType();

private:
    // Names
    std::string groupName_;
    const std::string railFolderPath_  = "Resources/GlobalParameter/RailEditor/Dates";
    const std::string textureFilePath_ = "Resources/texture/Particle";
    const std::string modelBasePath_   = "Resources/Model/";

    // Parameters
    Parameters parameters_;
    GroupParamaters groupParameters_;
    TimingParam timingParam_;
    TimeModeSelector timeModeSelector_;
    EmitterPositionMode emitPositionMode_ = EmitterPositionMode::None;

    // Rail,Model
    std::pair<std::string, FileSelector> railFileParam_;
    FileSelector modelFileSelector_;

    // TypeInt
    int32_t primitiveTypeInt_    = 0;
    int32_t billBordTypeInt_     = 0;
    int32_t blendModeInt_        = 0;
    int32_t emitPositionModeInt_ = 0;

    // frags
    bool isShot_             = false;
    bool useModel_           = false;
    bool useRailMoveEmitter_ = false;

    // time
    float intervalTime_ = 1.0f;

    // ParticleCount
    int32_t particleCount_  = 1;
    int32_t maxParticleNum_ = 1000;

    // filePath
    std::string modelFilePath_;
    std::string selectedTexturePath_;

    // テクスチャ変更時のコールバック
    std::function<void()> onTextureChanged_;
    std::function<void(PrimitiveType)> onPrimitiveChanged_;

public:
    // Getter
    const TimingParam& GetTimingParam() const { return timingParam_; }
    const TimeModeSelector& GetTimeModeSelector() const { return timeModeSelector_; }
    const GroupParamaters& GetGroupParameters() const { return groupParameters_; }
    const Parameters& GetParticleParameters() const { return parameters_; }
    float GetIntervalTime() const { return intervalTime_; }
    int32_t GetParticleCount() const { return particleCount_; }
    int32_t GetMaxParticleNum() const { return maxParticleNum_; }
    int32_t GetPrimitiveTypeInt() const { return primitiveTypeInt_; }
    bool IsUseModel() const { return useModel_; }
    bool GetIsRailMove() const { return useRailMoveEmitter_; }
    const std::string& GetRailFileName() const { return railFileParam_.first; }
    const std::string& GetModelFilePath() const { return modelFilePath_; }
    const std::string& GetSelectedTexturePath() const { return selectedTexturePath_; }

    // Setter
    void SetEmitPos(const Vector3& pos) { parameters_.emitPos = pos; }
    void SetIntervalTime(float time) { intervalTime_ = time; }
    void SetParticleCount(int32_t count) { particleCount_ = count; }
    void SetMaxParticleNum(int32_t num) { maxParticleNum_ = num; }
    void SetIsShot(bool shot);
    void SetBlendMode(BlendMode mode);
    void SetBillboardType(BillboardType type);
    void SetStartTime(float time) { timingParam_.startTime = time; }
    void SetParentTransform(const WorldTransform* transform);
    void SetParentJoint(const Object3DAnimation* animation, const std::string& jointName);
    void SetFollowingPos(const Vector3* pos);
    void SetTargetPosition(const Vector3& targetPos);
    void SetTextureChangedCallback(std::function<void()> callback);
    void SetPrimitiveChangedCallback(std::function<void(PrimitiveType)> callback) {
        onPrimitiveChanged_ = callback;
    }
};

} // namespace KetaEngine