#pragma once

// 3d
#include "3d/ViewProjection.h"
#include "MinMax.h"
// editor
#include "utility/ParameterEditor/GlobalParameter.h"
#include "utility/ParticleEditor/ParticleCommon.h"
#include "utility/RailEditor/RailManager.h"
// int32_t
#include <cstdint>
// std
#include <string>
#include <vector>

class ParticleParameter {
public:
    enum class EaseType {
        INSINE,
        OUTSINE,
        OUTBACK,
        OUTQUINT, 
    };

    struct GroupParamaters {
        BlendMode blendMode;
        bool isBillBord;
        bool isShot;
        bool isAlphaNoMove;
        BillboardType billBordType;
        AdaptRotate adaptRotate_;
    };

    struct EaseParm {
        bool isScaleEase;
        float maxTime;
        EaseType easeType;
        int easeTypeInt;
        FMinMax endValueF;
        V3MinMax endValueV3;
    };

    struct UVParm {
        Vector2 pos;
        Vector3 rotate;
        int32_t numOfFrame;
        float frameScroolSpeed;
        bool isScroolEachPixel;
        bool isRoop;
        bool isScrool;
    };

    struct Parameters {
        const WorldTransform* parentTransform = nullptr;
        Vector3 targetPos;
        Vector3 emitPos;
        Vector3 baseRotate;
        Vector4 baseColor;
        V3MinMax scaleDistV3;
        V3MinMax positionDist;
        V3MinMax directionDist;
        V3MinMax rotateDist;
        V3MinMax rotateSpeedDist;
        FMinMax scaleDist;
        FMinMax speedDist;
        V4MinMax colorDist;
        float lifeTime;
        float gravity;
        bool isScalerScale;
        bool isRotateforDirection;
        EaseParm scaleEaseParm;
        UVParm uvParm;
        const Vector3* followingPos_ = nullptr;
    };

protected:
    /// parameters
    Parameters parameters_;
    GroupParamaters groupParamaters_;
    GlobalParameter* globalParameter_;

    ///* setting
    // type
    int32_t billBordType_;
    int32_t blendMode_;
    // isShot
    bool isShot_;
    // Time
    float intervalTime_;
    // Count
    int32_t particleCount_;

    // file
    std::string particleName_;
    const std::string folderName_      = "Particle";
    const std::string textureFilePath_ = "Resources/texture";
    std::string selectedTexturePath_;

public:
    ParticleParameter()          = default;
    virtual ~ParticleParameter() = default;

    // init
    virtual void Init();
    void ParameterInit();

    // Adapt Texture
    void AdaptTexture();

    // Parameter Edit
    virtual void ParmLoadForImGui();
    virtual void ParmSaveForImGui();
    virtual void AddParmGroup();
    virtual void SetValues();
    virtual void ApplyGlobalParameter(const std::string& particleName);
   
    /// =============================================================================
    /// getter method
    /// =============================================================================
    std::string GetParticleName() const { return particleName_; }
    int GetPreBillBordType() const { return billBordType_; }
    bool GetPreIsShot() const { return isShot_; }

    /// =============================================================================
    /// setter method
    /// =============================================================================
    void SetParameter(const Parameters& param) { parameters_ = param; }
    void SetGroupParameter(const GroupParamaters& groupParam) { groupParamaters_ = groupParam; }
    void SetPreBillBordType(const int& preBillBordType) { billBordType_ = preBillBordType; }
    void SetPreIsShot(const bool& preIsShot) { isShot_ = preIsShot; }
};