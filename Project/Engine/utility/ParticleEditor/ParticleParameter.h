#pragma once

#include "MinMax.h"
#include "3d/ViewProjection.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "utility/RailEditor/RailManager.h"
#include "utility/ParticleEditor/ParticleCommon.h"
#include <string>
#include <vector>
#include <cstdint>

class ParticleParameter {
public:
    enum class EaseType {
        INSINE,
        OUTSINE,
        OUTBACK,
        OUTQUINT,
    };

    struct GroupParamaters {
        ParticleCommon::BlendMode blendMode;
        bool isBillBord;
        bool isShot;
        bool isAlphaNoMove;
        WorldTransform::BillboardType  billBordType;
        WorldTransform::AdaptRotate adaptRotate_;
    };

    struct EaseParm {
        bool isScaleEase;
        float maxTime;
        EaseType easeType;
        int easeTypeInt;
        FMinMax endValueF;
        V3MinMax endValueV3;
    };

    struct Parameters {
        const WorldTransform* parentTransform = nullptr;
        Vector3 targetPos;
        const Vector3* followingPos_ = nullptr;
        Vector3 emitPos;
        V3MinMax positionDist;
        bool isScalerScale;
        EaseParm scaleEaseParm;
        FMinMax scaleDist;
        V3MinMax scaleDistV3;
        FMinMax speedDist;
        V3MinMax directionDist;
        Vector4 baseColor;
        V4MinMax colorDist;
        float lifeTime;
        float gravity;
        Vector3 baseRotate;
        V3MinMax rotateDist;
        V3MinMax rotateSpeedDist;
        bool isRotateforDirection;
    };

protected:
    std::string particleName_;
    Parameters parameters_;
    GroupParamaters groupParamaters_;
    std::string folderName_ = "Particle";
    GlobalParameter* globalParameter_;
    int preBillBordType_;
    bool preIsShot_;
    float intervalTime_;
    int32_t particleCount;

public:
	ParticleParameter() = default;
    
    virtual ~ParticleParameter() = default;

    virtual void Init();

    //パラメータのリセット
    void ResetParameters();

   
    virtual  void ParmLoadForImGui(); // ロード
    virtual  void ParmSaveForImGui(); // セーブ
    void AddParmGroup();              // グループを追加
   virtual  void SetValues();                 // グループに値をセット
   virtual  void ApplyGlobalParameter(const std::string& particleName);  // ImGuiからパラメータを得る
  
   
    //getter
    std::string GetParticleName()const { return particleName_; }
    int GetPreBillBordType()const { return preBillBordType_; }
    bool GetPreIsShot()const { return preIsShot_; }

    //setter
    void SetParameter(const Parameters& param) { parameters_ = param; }
    void SetGroupParameter(const GroupParamaters& groupParam) { groupParamaters_ = groupParam; }
    void SetPreBillBordType(const int& preBillBordType) { preBillBordType_ = preBillBordType; }
    void SetPreIsShot(const bool& preIsShot) { preIsShot_ = preIsShot; }
};