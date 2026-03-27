#pragma once

#include "utility/ObjectFromBlender/ObjectFromBlender.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "3d/ViewProjection.h"
#include "Vector3.h"
// std
#include <memory>
#include <string>

/// <summary>
/// リザルト画面のステージ
/// ObjectFromBlender でオブジェクトを配置する
/// カメラ位置・回転の調整も担当
/// </summary>
class ResultStage {
public:
    ResultStage()  = default;
    ~ResultStage() = default;

    void Init(const std::string& filename);
    void Update();
    void AdjustParam();

    void SetViewProjection(KetaEngine::ViewProjection* vp) { viewProjection_ = vp; }

private:
    void RegisterParams();

    std::unique_ptr<KetaEngine::ObjectFromBlender> objectFromBlender_;

    KetaEngine::ViewProjection*  viewProjection_  = nullptr;
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_ = "ResultStage";

    Vector3 cameraPos_ = {0.f, -6.2f, -109.f};
    Vector3 cameraRot_ = {0.f, 0.f,   0.f};
};
