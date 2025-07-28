#pragma once

/// base
#include "BaseScene.h"

// testobj
#include "Ground/Ground.h"
#include "MonsterBall/MonsterBall.h"
#include "Plane/Plane.h"
#include "SkyBox/SkyBox.h"
// emitter
#include "utility/ParticleEditor/ParticleEmitter.h"

#include "SkyBox/SkyBox.h"

#include "utility/PutObjForBlender/PutObjForBlender.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public BaseScene {
private:
    ///========================================================
    /// Private variants
    ///========================================================

    std::unique_ptr<Ground> ground_                    = nullptr;
    std::unique_ptr<MonsterBall> monsterBall_          = nullptr;
    std::unique_ptr<Plane> plane_                      = nullptr;
    std::unique_ptr<SkyBox> skuBox_                    = nullptr;
    std::unique_ptr<PutObjForBlender> putObjForBlender = nullptr;

public:
    ///========================================================
    /// Constralt destract
    ///========================================================
    GameScene();
    ~GameScene() override;

    ///========================================================
    /// private method
    ///========================================================

    /// 初期化、更新、描画
    void Init() override;
    void Update() override;
    void ModelDraw() override;
    void SpriteDraw() override;
    void SkyBoxDraw() override;
    void DrawShadow() override;


    void Debug() override; /// debug
    void ViewProjectionUpdate() override;
    void ViewProssess() override;

    /*const ViewProjection& GetViewProjection()const { return viewProjection_; }*/
};