#pragma once

// 3d
#include "3d/ViewProjection.h"
#include "MinMax.h"
// editor
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/RailManager.h"
#include "Pipeline/Particle/ParticlePipeline.h"
// int32_t
#include <cstdint>
// std
#include <string>
#include <vector>

namespace KetaEngine {

class Object3DAnimation;

/// <summary>
/// パーティクルパラメータ基底クラス
/// </summary>
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
        float maxTime;
        bool isScaleEase;
        int easeTypeInt;
        EaseType easeType;

        // スカラー値用
        float startValueF = 1.0f;
        FMinMax endValueF;

        // Vector3値用
        Vector3 startValueV3 = Vector3::OneVector();
        V3MinMax endValueV3;

        // Back系イージング用パラメータ
        float backRatio;

        // 現在のスケールオフセット
        Vector3 currentScaleOffset = Vector3::OneVector();
    };

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

    struct JointParent {
        std::string name;
        const Object3DAnimation* animation = nullptr;
    };

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
        EaseParm scaleEaseParm;
        UVParm uvParm;
        JointParent jointParent;
    };

public:
    ParticleParameter()          = default;
    virtual ~ParticleParameter() = default;

    virtual void Init(); //< 初期化
    void ParameterInit(); //< パラメータ初期化
    void InitAdaptTexture(); //< テクスチャ適用
    void RegisterParams(); //< パラメータのバインド
    void GetParams(); //< パラメータの取得
    virtual void EditorUpdate(); //< エディタ更新
    void ParticleChange(); //< パーティクル変更処理
    void ScaleParmEditor(); //< スケールパラメータエディタ
    void ImGuiTextureSelection(); //< テクスチャ選択ImGui

    /// <summary>
    /// テクスチャの適用
    /// </summary>
    /// <param name="textureName">テクスチャ名</param>
    void ApplyTexture(const std::string& textureName);

    /// <summary>
    /// ファイル選択表示
    /// </summary>
    /// <param name="header">ヘッダー文字列</param>
    /// <param name="filenames">ファイル名リスト</param>
    /// <param name="selectedIndex">選択中のインデックス</param>
    /// <param name="onApply">適用時のコールバック</param>
    void DisplayFileSelection(const std::string& header, const std::vector<std::string>& filenames, int& selectedIndex,
        const std::function<void(const std::string&)>& onApply);

public:
    Parameters parameters_;
    GroupParamaters groupParamaters_;
    GlobalParameter* globalParameter_;

    std::unique_ptr<RailManager> railManager_;

    int32_t billBordType_;
    int32_t blendMode_;
    bool isShot_;
    float intervalTime_;
    int32_t particleCount_;
    int32_t maxParticleNum_ = 1000;

    bool isMoveForRail_;
    bool isRailRoop_;
    float moveSpeed_;

    std::string particleName_;
    const std::string folderPath_      = "Particle";
    const std::string textureFilePath_ = "Resources/texture";
    std::string selectedTexturePath_;

    // Primitive変更用
    int32_t primitiveTypeInt_ = 0;
    bool useModel_            = false;
    std::string modelFilePath_;

public:
    // getter
    std::string GetParticleName() const { return particleName_; }
    int GetPreBillBordType() const { return billBordType_; }
    bool GetPreIsShot() const { return isShot_; }

    // setter
    void SetParameter(const Parameters& param) { parameters_ = param; }
    void SetGroupParameter(const GroupParamaters& groupParam) { groupParamaters_ = groupParam; }
    void SetPreBillBordType(const int& preBillBordType) { billBordType_ = preBillBordType; }
    void SetPreIsShot(bool preIsShot) { isShot_ = preIsShot; }
};

}; // KetaEngine
