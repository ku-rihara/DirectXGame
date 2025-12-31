#pragma once
#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "MinMax.h"
#include "Pipeline/Particle/ParticlePipeline.h"
#include "utility/FileSelector/FileSelector.h"
#include "utility/TimeModeSelector/TimeModeSelector.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cstdint>
#include <string>

namespace KetaEngine {

class Object3DAnimation;
class WorldTransform;

/// <summary>
/// ParticleSection用パラメータ管理クラス
/// </summary>
class ParticleSectionParameter {
public:
    // グループパラメータ
    struct GroupParamaters {
        BlendMode blendMode;
        bool isBillBord;
        bool isShot;
        bool isAlphaNoMove;
        BillboardType billBordType;
        AdaptRotate adaptRotate_;
    };

    // スケールイージングパラメータ
    struct ScaleEaseParm {
        float maxTime       = 1.0f;
        bool isScaleEase    = false;
        int32_t easeTypeInt = 0;

        // スカラー値用
        float startValueF = 1.0f;
        FMinMax endValueF;

        // Vector3値用
        Vector3 startValueV3 = Vector3::OneVector();
        V3MinMax endValueV3;

        float backRatio            = 1.70158f;
        Vector3 currentScaleOffset = Vector3::OneVector();
    };

    // UVパラメータ
    struct UVParm {
        float frameScroolSpeed;
        bool isScroolEachPixel;
        bool isRoop;
        bool isScrool;
        bool isFlipX;
        bool isFlipY;
        Vector2 pos;
        Vector3 rotate;
        int32_t numOfFrame;
    };

    // ジョイント親
    struct JointParent {
        std::string name;
        const Object3DAnimation* animation = nullptr;
    };

    // パーティクルパラメータ
    struct Parameters {
        const WorldTransform* parentTransform = nullptr;
        const Vector3* followingPos_          = nullptr;
        Vector3 targetPos;
        Vector3 emitPos;
        Vector3 baseRotate;
        Vector4 baseColor;
        V3MinMax scaleDistV3;
        V3MinMax positionDist;
        V3MinMax directionDist;
        V3MinMax rotateDist;
        V3MinMax rotateSpeedDist;
        V3MinMax velocityDistV3;
        FMinMax scaleDist;
        FMinMax speedDist;
        V4MinMax colorDist;
        float lifeTime;
        float gravity;
        bool isFloatVelocity;
        bool isScalerScale;
        bool isRotateforDirection;
        ScaleEaseParm scaleEaseParm;
        UVParm uvParm;
        JointParent jointParent;
    };

    // タイミングパラメータ
    struct TimingParam {
        float startTime = 0.0f;
    };

    // Section固有パラメータ
    struct SectionParam {
        bool useScaleEasing = false;
        bool useRail        = false;
    };

public:
    ParticleSectionParameter()  = default;
    ~ParticleSectionParameter() = default;

    void RegisterParams(GlobalParameter* globalParam, const std::string& groupName);
    void GetParams(GlobalParameter* globalParam, const std::string& groupName);
    void AdjustParam();
    void ImGuiTextureSelection();
    void ApplyTexture(const std::string& textureName);
    void InitAdaptTexture();

private:
    void SelectRailFile(const char* label, const std::string& directory, std::pair<std::string, FileSelector>& param);
    void DisplayFileSelection(const std::string& header, const std::vector<std::string>& filenames, int& selectedIndex,
        const std::function<void(const std::string&)>& onApply);
    void ScaleParmEditor();

private:
    std::string groupName_;
    const std::string railFolderPath_  = "Resources/GlobalParameter/RailEditor/Dates";
    const std::string textureFilePath_ = "Resources/texture";
    const std::string modelBasePath_   = "Resources/Model/";

    Parameters parameters_;
    GroupParamaters groupParamaters_;
    TimingParam timingParam_;
    SectionParam sectionParam_;
    TimeModeSelector timeModeSelector_;

    std::pair<std::string, FileSelector> railFileParam_;
    FileSelector modelFileSelector_;

    int32_t billBordType_   = 0;
    int32_t blendMode_      = 0;
    bool isShot_            = false;
    float intervalTime_     = 1.0f;
    int32_t particleCount_  = 1;
    int32_t maxParticleNum_ = 1000;

    int32_t primitiveTypeInt_ = 0;
    bool useModel_            = false;
    std::string modelFilePath_;
    std::string selectedTexturePath_;

public:
    // Getter
    const SectionParam& GetSectionParam() const { return sectionParam_; }
    SectionParam& GetSectionParam() { return sectionParam_; }
    const TimingParam& GetTimingParam() const { return timingParam_; }
    TimingParam& GetTimingParam() { return timingParam_; }
    TimeModeSelector& GetTimeModeSelector() { return timeModeSelector_; }
    const TimeModeSelector& GetTimeModeSelector() const { return timeModeSelector_; }
    const std::string& GetRailFileName() const { return railFileParam_.first; }
    Parameters& GetParameters() { return parameters_; }
    const Parameters& GetParameters() const { return parameters_; }
    GroupParamaters& GetGroupParameters() { return groupParamaters_; }
    const GroupParamaters& GetGroupParameters() const { return groupParamaters_; }
    float GetIntervalTime() const { return intervalTime_; }
    int32_t GetParticleCount() const { return particleCount_; }
    int32_t GetMaxParticleNum() const { return maxParticleNum_; }
    bool IsUseModel() const { return useModel_; }
    int32_t GetPrimitiveTypeInt() const { return primitiveTypeInt_; }
    const std::string& GetModelFilePath() const { return modelFilePath_; }
    const std::string& GetSelectedTexturePath() const { return selectedTexturePath_; }
};

} // namespace KetaEngine