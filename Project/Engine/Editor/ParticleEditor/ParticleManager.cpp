#include "ParticleManager.h"

using namespace KetaEngine;
#include "3d/ModelManager.h"
#include "base/TextureManager.h"
// pipeline
#include"Pipeline/Particle/ParticlePipeline.h"
#include "Pipeline/PipelineManager.h"
// frame
#include "Frame/Frame.h"
// Function
#include "Function/GetFile.h"
#include "MathFunction.h"
#include "random.h"
// Primitive
#include "Primitive/PrimitiveCylinder.h"
#include"Primitive/PrimitiveSphere.h"
#include"Primitive/PrimitiveBox.h"
#include "Primitive/PrimitivePlane.h"
#include "Primitive/PrimitiveRing.h"
// editor
#include"Editor/ParameterEditor/GlobalParameter.h"
// std
#include <cassert>
#include <string>

ParticleManager* ParticleManager::GetInstance() {
    static ParticleManager instance;
    return &instance;
}

///============================================================
/// 　初期化
///============================================================
void ParticleManager::Init(SrvManager* srvManager) {
    pSrvManager_ = srvManager;

    SetAllParticleFile();
}

///============================================================
/// 更新
///============================================================
void ParticleManager::Update() {
    // 各粒子グループを周る
    for (auto& groupPair : particleGroups_) {
        ParticleGroup& group           = groupPair.second;
        std::list<Particle>& particles = group.particles;

        /// 粒子一つ一つの更新
        for (auto it = particles.begin(); it != particles.end();) {

            ///------------------------------------------------------------------------
            /// 加速フィールド
            ///------------------------------------------------------------------------
            if (accelerationField_.isAdaption && IsCollision(accelerationField_.area, it->worldTransform_->translation_)) {
                it->velocity_ += accelerationField_.acceleration * Frame::DeltaTimeRate();
            }

            ///------------------------------------------------------------------------
            /// スケール変更
            ///------------------------------------------------------------------------
            if ((*it).isAdaptEasing) {
                (*it).scaleEasing->Update(Frame::DeltaTimeRate());
            }
            ///------------------------------------------------------------------------
            /// 回転させる
            ///------------------------------------------------------------------------
            it->worldTransform_->rotation_.x += it->rotateSpeed_.x * Frame::DeltaTimeRate();
            it->worldTransform_->rotation_.y += it->rotateSpeed_.y * Frame::DeltaTimeRate();
            it->worldTransform_->rotation_.z += it->rotateSpeed_.z * Frame::DeltaTimeRate();

            ///------------------------------------------------------------------------
            /// 重力の適用
            ///------------------------------------------------------------------------
            it->velocity_.y += it->gravity_ * Frame::DeltaTime();

            ///------------------------------------------------------------------------
            /// 変位更新
            ///------------------------------------------------------------------------
            if (it->followPos) {
                it->worldTransform_->translation_ = *it->followPos + it->offSet;
            } else {
                it->worldTransform_->translation_.y += it->velocity_.y * Frame::DeltaTime();

                if (it->isFloatVelocity) {
                    // 方向ベクトル × スカラー速度
                    it->worldTransform_->translation_ += it->direction_ * it->speed_ * Frame::DeltaTime();
                } else {
                    // ベクトル速度そのまま適用
                    it->worldTransform_->translation_ += it->speedV3 * Frame::DeltaTime();
                }
            }

            ///------------------------------------------------------------------------
            /// UV更新
            ///------------------------------------------------------------------------
            if (it->uvInfo_.isScroll) {
                UpdateUV(it->uvInfo_, Frame::DeltaTime());
            }
            ///------------------------------------------------------------------------
            /// ビルボードまたは通常の行列更新
            ///------------------------------------------------------------------------

            if (group.param.isBillBord) {
                it->worldTransform_->BillboardUpdateMatrix(*viewProjection_, group.param.billBordType, group.param.adaptRotate_);
            } else {
                it->worldTransform_->UpdateMatrix();
            }

            // 時間を進める
            it->currentTime_ += Frame::DeltaTime();
            ++it;
        }
    }
}
///============================================================
/// 描画
///============================================================
void ParticleManager::Draw(const ViewProjection& viewProjection) {
    /// commandList取得
    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

    for (auto& groupPair : particleGroups_) {
        ParticleGroup& group           = groupPair.second;
        std::string name               = groupPair.first;
        std::list<Particle>& particles = group.particles;
        ParticleFprGPU* instancingData = group.instancingData;

        uint32_t instanceIndex = 0;

        // 各粒子のインスタンシングデータを設定
        for (auto it = particles.begin(); it != particles.end();) {
            if (it->currentTime_ >= it->lifeTime_) {
                it = particles.erase(it);
                continue;
            }

            if (instanceIndex > particleGroups_[name].currentNum) {
                return;
            }

            // WVP適応
            instancingData[instanceIndex].World                 = it->worldTransform_->matWorld_;
            instancingData[instanceIndex].WVP                   = it->worldTransform_->matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
            instancingData[instanceIndex].WorldInverseTranspose = Inverse(Transpose(it->worldTransform_->matWorld_));

            /// Alpha適応
            AlphaAdapt(instancingData[instanceIndex], *it, group);

            ///==========================================================================================
            //  UVTransform
            ///==========================================================================================
            instancingData[instanceIndex].UVTransform = MakeAffineMatrix(it->uvInfo_.scale, it->uvInfo_.rotate, it->uvInfo_.pos);

            if (it->uvInfo_.isFlipX) {
                instancingData[instanceIndex].isFlipX = true;
            }
            if (it->uvInfo_.isFlipY) {
                instancingData[instanceIndex].isFlipY = true;
            }

            ++instanceIndex;
            ++it;
        }

        if (instanceIndex > 0) {
            PipelineManager::GetInstance()->PreDraw(PipelineType::Particle, commandList);
            PipelineManager::GetInstance()->PreBlendSet(PipelineType::Particle, commandList, group.param.blendMode);

            // マテリアルのリソースを設定
            group.material.SetCommandList(commandList);
            commandList->SetGraphicsRootDescriptorTable(static_cast<UINT>(ParticleRootParameter::ParticleData), pSrvManager_->GetGPUDescriptorHandle(group.srvIndex));

            // テクスチャハンドルの設定
            commandList->SetGraphicsRootDescriptorTable(static_cast<UINT>(ParticleRootParameter::Texture), TextureManager::GetInstance()->GetTextureHandle(group.textureHandle));

            // モデル描画
            if (group.model) {
                group.model->DrawInstancing(instanceIndex);
                // メッシュ描画
            } else if (group.primitive_->GetMesh()) {
                group.primitive_->GetMesh()->DrawInstancing(instanceIndex);
            }
        }
    }
}

void ParticleManager::UpdateUV(UVInfo& uvInfo, float deltaTime) {
    if (uvInfo.isScrollEachPixel) {
        // 毎フレーム、速度に応じて移動
        uvInfo.pos.x += uvInfo.frameScrollSpeed * deltaTime;

        if (!uvInfo.isLoop) {
            // 停止位置を上限にする
            uvInfo.pos.x = std::min(uvInfo.pos.x, uvInfo.uvStopPos_);
        }
    } else {
        // コマ送り制御
        uvInfo.currentScrollTime += deltaTime;

        // フレームごとの更新タイミングに達したら
        if (uvInfo.currentScrollTime >= uvInfo.frameScrollSpeed) {
            uvInfo.currentScrollTime = 0.0f; // リセット
            uvInfo.pos.x += uvInfo.frameDistance_;

            if (!uvInfo.isLoop) {
                // 停止位置に達したらストップ
                uvInfo.pos.x = std::min(uvInfo.pos.x, uvInfo.uvStopPos_);
            }
        }
    }
}

///============================================================
/// グループ作成
///============================================================
void ParticleManager::CreateParticleGroup(
    const std::string name, const std::string modelFilePath,
    uint32_t maxnum) {
    if (particleGroups_.contains(name)) {
        return;
    }

    // グループ追加
    particleGroups_[name] = ParticleGroup();

    /// モデル
    ModelManager::GetInstance()->LoadModel(modelFilePath);
    SetModel(name, modelFilePath);

    /// リソース作成
    CreateInstancingResource(name, maxnum); // インスタンシング
    CreateMaterialResource(name); // マテリアル

    particleGroups_[name].instanceNum = 0;
}

void ParticleManager::CreatePrimitiveParticle(const std::string& name, PrimitiveType type, uint32_t maxnum) {
    if (particleGroups_.contains(name)) {
        return;
    }

    // グループを追加
    particleGroups_[name] = ParticleGroup();

    // createPrimitive
    switch (type) {
    case PrimitiveType::Plane:
        particleGroups_[name].primitive_ = std::make_unique<PrimitivePlane>();
        break;
    case PrimitiveType::Sphere:
        particleGroups_[name].primitive_ = std::make_unique<PrimitiveSphere>();
        break;
    case PrimitiveType::Box:
        particleGroups_[name].primitive_ = std::make_unique<PrimitiveBox>();
        break;
    case PrimitiveType::Ring:
        particleGroups_[name].primitive_ = std::make_unique<PrimitiveRing>();
        break;
    case PrimitiveType::Cylinder:
        particleGroups_[name].primitive_ = std::make_unique<PrimitiveCylinder>();
        break;
    }

    // プリミティブの初期化と作成
    particleGroups_[name].primitive_->Init();

    // インスタンシングリソースとマテリアルリソースを作成
    CreateInstancingResource(name, maxnum);
    CreateMaterialResource(name);

    particleGroups_[name].instanceNum = 0;
}

///============================================================
/// テクスチャセット
///============================================================
void ParticleManager::SetTextureHandle(const std::string name, uint32_t handle) {
    particleGroups_[name].textureHandle = handle;
}

///============================================================
/// モデルセット
///============================================================

void ParticleManager::SetModel(const std::string& name, const std::string& modelName) {

    // モデルを検索してセット
    ModelManager::GetInstance()->LoadModel(modelName);
    particleGroups_[name].model         = (ModelManager::GetInstance()->FindModel(modelName));
    particleGroups_[name].textureHandle = TextureManager::GetInstance()->LoadTexture(
        particleGroups_[name].model->GetModelData().material.textureFilePath);
}

void ParticleManager::CreateMaterialResource(const std::string& name) {
    particleGroups_[name].material.Init(DirectXCommon::GetInstance());
}

///============================================================
/// インスタンシングリソース作成
///============================================================
void ParticleManager::CreateInstancingResource(const std::string& name, uint32_t instanceNum) {

    particleGroups_[name].instanceNum = instanceNum;
    particleGroups_[name].currentNum  = instanceNum;

    // Instancing用のTransformationMatrixリソースを作る
    particleGroups_[name].instancingResource = DirectXCommon::GetInstance()->CreateBufferResource(
        DirectXCommon::GetInstance()->GetDevice(), sizeof(ParticleFprGPU) * particleGroups_[name].instanceNum);

    particleGroups_[name].instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroups_[name].instancingData));

    // インスタンシングデータリセット
    ResetInstancingData(name);

    // SRV確保
    particleGroups_[name].srvIndex = pSrvManager_->Allocate();

    // SRVの作成
    pSrvManager_->CreateForStructuredBuffer(
        particleGroups_[name].srvIndex,
        particleGroups_[name].instancingResource.Get(),
        instanceNum,
        sizeof(ParticleFprGPU));
}

///======================================================================
/// パーティクル作成
///======================================================================
ParticleManager::Particle ParticleManager::MakeParticle(const Parameters& parameters) {

    Particle particle;

    /// Init
    particle.lifeTime_       = parameters.lifeTime;
    particle.currentTime_    = 0.0f;
    particle.worldTransform_ = std::make_unique<WorldTransform>();
    particle.worldTransform_->Init();

    ///------------------------------------------------------------------------
    /// ペアレント設定
    ///------------------------------------------------------------------------
    if (parameters.parentTransform) {
        particle.worldTransform_->parent_ = parameters.parentTransform;
    }
    if (parameters.jointParent.animation) {
        particle.worldTransform_->SetParentJoint(parameters.jointParent.animation, parameters.jointParent.name);
    }
    if (parameters.followingPos_) {
        particle.followPos = parameters.followingPos_;
    }

    ///------------------------------------------------------------------------
    /// 座標設定
    ///------------------------------------------------------------------------
    Vector3 randomTranslate = {
        Random::Range(parameters.positionDist.min.x, parameters.positionDist.max.x),
        Random::Range(parameters.positionDist.min.y, parameters.positionDist.max.y),
        Random::Range(parameters.positionDist.min.z, parameters.positionDist.max.z)};

    particle.worldTransform_->translation_ = parameters.targetPos + parameters.emitPos + randomTranslate;
    particle.offSet                        = parameters.targetPos + parameters.emitPos + randomTranslate;

    ///------------------------------------------------------------------------
    /// 速度、向き設定
    ///------------------------------------------------------------------------
    if (parameters.isFloatVelocity) {
        Vector3 direction = {
            Random::Range(parameters.directionDist.min.x, parameters.directionDist.max.x),
            Random::Range(parameters.directionDist.min.y, parameters.directionDist.max.y),
            Random::Range(parameters.directionDist.min.z, parameters.directionDist.max.z)};
        direction                      = direction.Normalize();
        float speed                    = Random::Range(parameters.speedDist.min, parameters.speedDist.max);
        Matrix4x4 cameraRotationMatrix = MakeRotateMatrix(viewProjection_->rotation_);
        particle.direction_            = TransformNormal(direction, cameraRotationMatrix);
        particle.speed_                = speed;
    } else {
        Vector3 velocity = {
            Random::Range(parameters.velocityDistV3.min.x, parameters.velocityDistV3.max.x),
            Random::Range(parameters.velocityDistV3.min.y, parameters.velocityDistV3.max.y),
            Random::Range(parameters.velocityDistV3.min.z, parameters.velocityDistV3.max.z)};
        Matrix4x4 cameraRotationMatrix = MakeRotateMatrix(viewProjection_->rotation_);
        velocity                       = TransformNormal(velocity, cameraRotationMatrix);
        particle.direction_            = velocity;
        particle.speedV3               = velocity;
    }
    particle.isFloatVelocity = parameters.isFloatVelocity;

    ///------------------------------------------------------------------------
    /// 回転設定
    ///------------------------------------------------------------------------
    if (parameters.isRotateForDirection) {
        particle.worldTransform_->rotation_ = DirectionToEulerAngles(particle.direction_, *viewProjection_);
    } else {
        Vector3 rotate = {
            Random::Range(parameters.rotateDist.min.x, parameters.rotateDist.max.x),
            Random::Range(parameters.rotateDist.min.y, parameters.rotateDist.max.y),
            Random::Range(parameters.rotateDist.min.z, parameters.rotateDist.max.z)};
        particle.worldTransform_->rotation_ = ToRadian(parameters.baseRotate + rotate);
    }

    ///------------------------------------------------------------------------
    /// 回転スピード設定
    ///------------------------------------------------------------------------
    Vector3 rotateSpeed = {
        Random::Range(parameters.rotateSpeedDist.min.x, parameters.rotateSpeedDist.max.x),
        Random::Range(parameters.rotateSpeedDist.min.y, parameters.rotateSpeedDist.max.y),
        Random::Range(parameters.rotateSpeedDist.min.z, parameters.rotateSpeedDist.max.z)};
    particle.rotateSpeed_ = rotateSpeed;

    ///------------------------------------------------------------------------
    ///  スケール設定 
    ///------------------------------------------------------------------------
    if (parameters.isScalerScale) {
        // スカラー値の場合
        float scale                      = Random::Range(parameters.scaleDist.min, parameters.scaleDist.max);
        particle.worldTransform_->scale_ = {scale, scale, scale};
        particle.scaleInfo.tempScaleV3   = particle.worldTransform_->scale_;

        float endScale                  = Random::Range(parameters.scaleEaseParam.endValueF.min, parameters.scaleEaseParam.endValueF.max);
        particle.scaleInfo.easeEndScale = {endScale, endScale, endScale};
    } else {
        // Vector3の場合
        Vector3 ScaleV3 = {
            Random::Range(parameters.scaleDistV3.min.x, parameters.scaleDistV3.max.x),
            Random::Range(parameters.scaleDistV3.min.y, parameters.scaleDistV3.max.y),
            Random::Range(parameters.scaleDistV3.min.z, parameters.scaleDistV3.max.z)};
        particle.worldTransform_->scale_ = ScaleV3;
        particle.scaleInfo.tempScaleV3   = ScaleV3;

        Vector3 endScaleV3 = {
            Random::Range(parameters.scaleEaseParam.endValueV3.min.x, parameters.scaleEaseParam.endValueV3.max.x),
            Random::Range(parameters.scaleEaseParam.endValueV3.min.y, parameters.scaleEaseParam.endValueV3.max.y),
            Random::Range(parameters.scaleEaseParam.endValueV3.min.z, parameters.scaleEaseParam.endValueV3.max.z)};
        particle.scaleInfo.easeEndScale = endScaleV3;
    }

    //  Easingパラメータを設定
    particle.scaleInfo.easeParam.baseParam.isEase = parameters.scaleEaseParam.baseParam.isEase;
    particle.scaleInfo.easeParam.baseParam.maxTime = parameters.scaleEaseParam.baseParam.maxTime;


    //  Easingクラスのインスタンスを作成して設定
    if (parameters.scaleEaseParam.baseParam.isEase) {
        particle.scaleEasing   = std::make_unique<Easing<Vector3>>();
        particle.isAdaptEasing = true;

        // Easingパラメータを構築
        EasingParameter<Vector3> easingParam;
        easingParam.type       = static_cast<EasingType>(parameters.scaleEaseParam.baseParam.easeTypeInt);
        easingParam.startValue = particle.scaleInfo.tempScaleV3;
        easingParam.endValue   = particle.scaleInfo.easeEndScale;
        easingParam.maxTime    = parameters.scaleEaseParam.baseParam.maxTime;
        easingParam.backRatio  = parameters.scaleEaseParam.baseParam.backRatio;
        if (easingParam.backRatio == 0.0f) {
            easingParam.finishType = EasingFinishValueType::End;
        } else {
            easingParam.finishType = EasingFinishValueType::Start;
        }

        // Easingに設定
        particle.scaleEasing->SettingValue(easingParam);

        // WorldTransformのscaleに直接適用
        particle.scaleEasing->SetAdaptValue(&particle.worldTransform_->scale_);
    }

    ///------------------------------------------------------------------------
    /// 色設定
    ///------------------------------------------------------------------------
    Vector4 randomColor = {
        Random::Range(parameters.colorDist.min.x, parameters.colorDist.max.x),
        Random::Range(parameters.colorDist.min.y, parameters.colorDist.max.y),
        Random::Range(parameters.colorDist.min.z, parameters.colorDist.max.z),
        0.0f};
    particle.color_ = parameters.baseColor + randomColor;

    ///------------------------------------------------------------------------
    /// UVTransform設定
    ///------------------------------------------------------------------------
    float frameWidth = 1.0f;
    if (parameters.uvParam.numOfFrame != 0) {
        frameWidth = 1.0f / float(parameters.uvParam.numOfFrame);
    }
    const float stopPosition = 1.0f - frameWidth;

    particle.uvInfo_.pos               = Vector3(parameters.uvParam.pos.x, parameters.uvParam.pos.y, 1.0f);
    particle.uvInfo_.rotate            = parameters.uvParam.rotate;
    particle.uvInfo_.scale             = Vector3(frameWidth, 1.0f, 1.0f);
    particle.uvInfo_.frameDistance_    = frameWidth;
    particle.uvInfo_.frameScrollSpeed  = parameters.uvParam.frameScrollSpeed;
    particle.uvInfo_.uvStopPos_        = stopPosition;
    particle.uvInfo_.isScrollEachPixel = parameters.uvParam.isScrollEachPixel;
    particle.uvInfo_.isLoop            = parameters.uvParam.isLoop;
    particle.uvInfo_.isScroll          = parameters.uvParam.isScroll;
    particle.uvInfo_.isFlipX           = parameters.uvParam.isFlipX;
    particle.uvInfo_.isFlipY           = parameters.uvParam.isFlipY;
    particle.uvInfo_.currentScrollTime = 0.0f;

    ///------------------------------------------------------------------------
    /// 重力設定
    ///------------------------------------------------------------------------
    particle.gravity_ = parameters.gravity;

    return particle;
}

///======================================================================
/// エミット
///======================================================================
void ParticleManager::Emit(
    std::string name, const Parameters& paramaters, const GroupParamaters& groupParamaters, int32_t count) {

    // パーティクルグループが存在するか確認
    assert(particleGroups_.find(name) != particleGroups_.end() && "Error: Not Find ParticleGroup");

    // 指定されたパーティクルグループを取得
    ParticleGroup& particleGroup = particleGroups_[name];
    particleGroup.param          = groupParamaters;

    // 生成、グループ追加
    std::list<Particle> particles;
    for (uint32_t i = 0; i < uint32_t(count); ++i) {
        particles.emplace_back(MakeParticle(paramaters));
    }

    // グループに追加
    particleGroup.particles.splice(particleGroup.particles.end(), particles);
}

/// Reset
void ParticleManager::ResetAllParticles() {
    for (auto& groupPair : particleGroups_) {
        ParticleGroup& group = groupPair.second;

        // 全てのパーティクルを消去
        group.particles.clear();

        // インスタンシングデータをリセット
        for (uint32_t index = 0; index < group.instanceNum; ++index) {
            group.instancingData[index].WVP         = MakeIdentity4x4();
            group.instancingData[index].World       = MakeIdentity4x4();
            group.instancingData[index].color       = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            group.instancingData[index].UVTransform = MakeIdentity4x4();
        }
    }
}

void ParticleManager::ResetInstancingData(const std::string& name) {
    for (uint32_t index = 0; index < particleGroups_[name].instanceNum; ++index) {
        particleGroups_[name].instancingData[index].WVP         = MakeIdentity4x4();
        particleGroups_[name].instancingData[index].World       = MakeIdentity4x4();
        particleGroups_[name].instancingData[index].color       = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        particleGroups_[name].instancingData[index].UVTransform = MakeIdentity4x4();
    }
}

///=================================================================================================
/// parm Adapt
///=================================================================================================
void ParticleManager::AlphaAdapt(ParticleFprGPU& data, const Particle& parm, const ParticleGroup& group) {
    data.color = parm.color_;
    if (group.param.isAlphaNoMove) {
        data.color.w = 1.0f;
        return;
    }
    data.color.w = 1.0f - (parm.currentTime_ / parm.lifeTime_);
}


void ParticleManager::SetViewProjection(const ViewProjection* view) {

    viewProjection_ = view;
}

void ParticleManager::SetAllParticleFile() {
    particleFiles_ = GetFileNamesForDirectory(GlobalParameter::GetInstance()->GetDirectoryPath() + ParticleFolderName_);
}