#pragma once
#include "../Parameter/ParticleCommonParameters.h"
#include "3D/Primitive/IPrimitive.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "utility/FileSelector/FileSelector.h"
#include "utility/TimeModeSelector/TimeModeSelector.h"
#include "Vector3.h"
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
    using GroupParameters = ParticleCommon::GroupParameters;
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
    ParticleSectionParameter() {
        parameters_.scaleDist.max   = 1.0f;
        parameters_.scaleDist.min   = 1.0f;
        parameters_.scaleDistV3.max = {1.0f, 1.0f, 1.0f};
        parameters_.scaleDistV3.min = {1.0f, 1.0f, 1.0f};
        parameters_.lifeTime        = 1.0f;
    }
    ~ParticleSectionParameter() = default;

    void RegisterParams(GlobalParameter* globalParam, const std::string& groupName);
    void AdaptParameters(GlobalParameter* globalParam, const std::string& groupName);

    void AdjustParam();
    void ImGuiTextureSelection();
    void InitAdaptTexture();

private:
    void SelectRailFile(const char* label, const std::string& directory, std::pair<std::string, FileSelector>* param);
    void ScaleParamEditor();
    void TranslateParamEditor();
    void RotateParamEditor();
    void AdaptIntToType();

private:
    // Names
    std::string groupName_;
    const std::string railFolderPath_     = "Resources/GlobalParameter/RailEditor/Dates";
    const std::string textureFilePath_    = "Resources/EngineTexture/Particle";
    const std::string noiseFolderPath_    = "Resources/EngineTexture/Noise";
    const std::string modelBasePath_      = "Resources/Model/";
    const std::string dissolveFolderPath_ = "Resources/GlobalParameter/DissolveEditor/Common/Dates";

    // Parameters
    Parameters parameters_;
    GroupParameters groupParameters_;
    TimingParam timingParam_;
    TimeModeSelector timeModeSelector_;
    EmitterPositionMode emitPositionMode_ = EmitterPositionMode::None;

    // Rail, Model
    std::pair<std::string, FileSelector> railFileParam_;
    FileSelector modelFileSelector_;

    // テクスチャ選択
    FileSelector textureFileSelector_;
    FileSelector distortionTextureFileSelector_;
    FileSelector dissolveFileSelector_;

    // TypeInt
    int32_t primitiveTypeInt_    = 0;
    int32_t billboardTypeInt_    = 0;
    int32_t blendModeInt_        = 0;
    int32_t emitPositionModeInt_ = 0;
    int32_t alphaModeInt_        = 1; // デフォルト: LifeTime
    int32_t emitShapeInt_        = 0; // EmitShape (0=None, 1=Sphere)

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
    std::string dissolveName_;
    std::string distortionTexturePath_; // 歪みノイズテクスチャパス

    // テクスチャ変更時のコールバック
    std::function<void()> onTextureChanged_;
    std::function<void(PrimitiveType)> onPrimitiveChanged_;

public:
    // Getter
    const TimingParam& GetTimingParam() const { return timingParam_; }
    const TimeModeSelector& GetTimeModeSelector() const { return timeModeSelector_; }
    const GroupParameters& GetGroupParameters() const { return groupParameters_; }
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
    const std::string& GetDissolveName() const { return dissolveName_; }
    const std::string& GetDistortionTexturePath() const { return distortionTexturePath_; }

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