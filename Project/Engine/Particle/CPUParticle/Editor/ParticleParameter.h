#pragma once

// 3d
#include "3d/ViewProjection.h"
#include "MinMax.h"
// editor
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/RailManager.h"
#include "Parameter/ParticleCommonParameters.h"
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
    // 共通パラメータの型エイリアス
 
    using GroupParameters = ParticleCommon::GroupParameters;
    using Parameters      = ParticleCommon::Parameters;
    using EaseParm        = ParticleCommon::ScaleEaseParam;
    using UVParm          = ParticleCommon::UVParam;
    using JointParent     = ParticleCommon::JointParent;

public:
    ParticleParameter()          = default;
    virtual ~ParticleParameter() = default;

    virtual void Init();      //< 初期化
    void ParameterInit();     //< パラメータ初期化
    void InitAdaptTexture();  //< テクスチャ適用
    void RegisterParams();    //< パラメータのバインド
    void GetParams();         //< パラメータの取得
    virtual void EditorUpdate();  //< エディタ更新
    void ParticleChange();        //< パーティクル変更処理
    void ScaleParmEditor();       //< スケールパラメータエディタ
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
    GroupParameters groupParameters_;
    GlobalParameter* globalParameter_;

    std::unique_ptr<RailManager> railManager_;

    int32_t billboardType_;
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
    int GetPreBillboardType() const { return billboardType_; }
    bool GetPreIsShot() const { return isShot_; }

    // setter
    void SetParameter(const Parameters& param) { parameters_ = param; }
    void SetGroupParameter(const GroupParameters& groupParam) { groupParameters_ = groupParam; }
    void SetPreBillboardType(int preBillBordType) { billboardType_ = preBillBordType; }
    void SetPreIsShot(bool preIsShot) { isShot_ = preIsShot; }
};

}; // KetaEngine