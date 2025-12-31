#include "ParticleManager.h"

using namespace KetaEngine;
#include "3d/ModelManager.h"
#include "Animation/ModelAnimation.h"
#include "base/TextureManager.h"
#include "Pipeline/PipelineManager.h"
// frame
#include "Frame/Frame.h"
// Function
#include "Easing/EasingFunction.h"
#include "Function/GetFile.h"
#include "MathFunction.h"
#include "random.h"
// Primitive
#include "Primitive/PrimitiveCylinder.h"
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
            (*it).scaleEasing->Update(Frame::DeltaTimeRate());
          /*  (*it).easeTime                = std::min((*it).easeTime, (*it).scaleInfo.easeParam.maxTime);*/
            (*it).worldTransform_->scale_ = (*it).scaleInfo.easingAdaptParam;

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
                    // 通常:方向ベクトル × スカラー速度
                    it->worldTransform_->translation_ += it->direction_ * it->speed_ * Frame::DeltaTime();
                } else {
                    // ベクトル速度そのまま適用
                    it->worldTransform_->translation_ += it->speedV3 * Frame::DeltaTime();
                }
            }

            ///------------------------------------------------------------------------
            /// UV更新
            ///------------------------------------------------------------------------
            if (it->uvInfo_.isScrool) {
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
            commandList->SetGraphicsRootDescriptorTable(0, pSrvManager_->GetGPUDescriptorHandle(group.srvIndex));

            // テクスチャハンドルの設定
            commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(group.textureHandle));

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
    if (uvInfo.isScroolEachPixel) {
        // 毎フレーム、速度に応じて移動
        uvInfo.pos.x += uvInfo.frameScroolSpeed * deltaTime;

        if (!uvInfo.isRoop) {
            // 停止位置を上限にする
            uvInfo.pos.x = std::min(uvInfo.pos.x, uvInfo.uvStopPos_);
        }
    } else {
        // コマ送り制御
        uvInfo.currentScroolTime += deltaTime;

        // フレームごとの更新タイミングに達したら
        if (uvInfo.currentScroolTime >= uvInfo.frameScroolSpeed) {
            uvInfo.currentScroolTime = 0.0f; // リセット
            uvInfo.pos.x += uvInfo.frameDistance_;

            if (!uvInfo.isRoop) {
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
    particleGroups_[name].material.CreateMaterialResource(DirectXCommon::GetInstance());
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
ParticleManager::Particle ParticleManager::MakeParticle(const Parameters& paramaters) {

    Particle particle;

    /// Init
    particle.lifeTime_       = paramaters.lifeTime;
    particle.currentTime_    = 0.0f;
    particle.worldTransform_ = std::make_unique<WorldTransform>();
    particle.worldTransform_->Init();

    ///------------------------------------------------------------------------
    /// ペアレント設定
    ///------------------------------------------------------------------------
    if (paramaters.parentTransform) {
        particle.worldTransform_->parent_ = paramaters.parentTransform;
    }
    if (paramaters.jointParent.animation) {
        particle.worldTransform_->SetParentJoint(paramaters.jointParent.animation, paramaters.jointParent.name);
    }
    if (paramaters.followingPos_) {
        particle.followPos = paramaters.followingPos_;
    }

    ///------------------------------------------------------------------------
    /// 座標設定
    ///------------------------------------------------------------------------
    Vector3 randomTranslate = {
        Random::Range(paramaters.positionDist.min.x, paramaters.positionDist.max.x),
        Random::Range(paramaters.positionDist.min.y, paramaters.positionDist.max.y),
        Random::Range(paramaters.positionDist.min.z, paramaters.positionDist.max.z)};

    particle.worldTransform_->translation_ = paramaters.targetPos + paramaters.emitPos + randomTranslate;
    particle.offSet                        = paramaters.targetPos + paramaters.emitPos + randomTranslate;

    ///------------------------------------------------------------------------
    /// 速度、向き設定
    ///------------------------------------------------------------------------
    if (paramaters.isFloatVelocity) {
        Vector3 direction = {
            Random::Range(paramaters.directionDist.min.x, paramaters.directionDist.max.x),
            Random::Range(paramaters.directionDist.min.y, paramaters.directionDist.max.y),
            Random::Range(paramaters.directionDist.min.z, paramaters.directionDist.max.z)};
        direction                      = direction.Normalize();
        float speed                    = Random::Range(paramaters.speedDist.min, paramaters.speedDist.max);
        Matrix4x4 cameraRotationMatrix = MakeRotateMatrix(viewProjection_->rotation_);
        particle.direction_            = TransformNormal(direction, cameraRotationMatrix);
        particle.speed_                = speed;
    } else {
        Vector3 velocity = {
            Random::Range(paramaters.velocityDistV3.min.x, paramaters.velocityDistV3.max.x),
            Random::Range(paramaters.velocityDistV3.min.y, paramaters.velocityDistV3.max.y),
            Random::Range(paramaters.velocityDistV3.min.z, paramaters.velocityDistV3.max.z)};
        Matrix4x4 cameraRotationMatrix = MakeRotateMatrix(viewProjection_->rotation_);
        velocity                       = TransformNormal(velocity, cameraRotationMatrix);
        particle.direction_            = velocity;
        particle.speedV3               = velocity;
    }
    particle.isFloatVelocity = paramaters.isFloatVelocity;

    ///------------------------------------------------------------------------
    /// 回転設定
    ///------------------------------------------------------------------------
    if (paramaters.isRotateforDirection) {
        particle.worldTransform_->rotation_ = DirectionToEulerAngles(particle.direction_, *viewProjection_);
    } else {
        Vector3 rotate = {
            Random::Range(paramaters.rotateDist.min.x, paramaters.rotateDist.max.x),
            Random::Range(paramaters.rotateDist.min.y, paramaters.rotateDist.max.y),
            Random::Range(paramaters.rotateDist.min.z, paramaters.rotateDist.max.z)};
        particle.worldTransform_->rotation_ = ToRadian(paramaters.baseRotate + rotate);
    }

    ///------------------------------------------------------------------------
    /// 回転スピード設定
    ///------------------------------------------------------------------------
    Vector3 rotateSpeed = {
        Random::Range(paramaters.rotateSpeedDist.min.x, paramaters.rotateSpeedDist.max.x),
        Random::Range(paramaters.rotateSpeedDist.min.y, paramaters.rotateSpeedDist.max.y),
        Random::Range(paramaters.rotateSpeedDist.min.z, paramaters.rotateSpeedDist.max.z)};
    particle.rotateSpeed_ = rotateSpeed;

    ///------------------------------------------------------------------------
    ///  スケール設定 
    ///------------------------------------------------------------------------
    if (paramaters.isScalerScale) {
        // スカラー値の場合
        float scale                      = Random::Range(paramaters.scaleDist.min, paramaters.scaleDist.max);
        particle.worldTransform_->scale_ = {scale, scale, scale};
        particle.scaleInfo.tempScaleV3   = particle.worldTransform_->scale_;

        float endscale                  = Random::Range(paramaters.scaleEaseParm.endValueF.min, paramaters.scaleEaseParm.endValueF.max);
        particle.scaleInfo.easeEndScale = {endscale, endscale, endscale};
    } else {
        // Vector3の場合
        Vector3 ScaleV3 = {
            Random::Range(paramaters.scaleDistV3.min.x, paramaters.scaleDistV3.max.x),
            Random::Range(paramaters.scaleDistV3.min.y, paramaters.scaleDistV3.max.y),
            Random::Range(paramaters.scaleDistV3.min.z, paramaters.scaleDistV3.max.z)};
        particle.worldTransform_->scale_ = ScaleV3;
        particle.scaleInfo.tempScaleV3   = ScaleV3;

        Vector3 endScaleV3 = {
            Random::Range(paramaters.scaleEaseParm.endValueV3.min.x, paramaters.scaleEaseParm.endValueV3.max.x),
            Random::Range(paramaters.scaleEaseParm.endValueV3.min.y, paramaters.scaleEaseParm.endValueV3.max.y),
            Random::Range(paramaters.scaleEaseParm.endValueV3.min.z, paramaters.scaleEaseParm.endValueV3.max.z)};
        particle.scaleInfo.easeEndScale = endScaleV3;
    }

    //  Easingパラメータを設定
    particle.scaleInfo.easeParam.isScaleEase = paramaters.scaleEaseParm.isScaleEase;
    particle.scaleInfo.easeParam.maxTime     = paramaters.scaleEaseParm.maxTime;


    //  Easingクラスのインスタンスを作成して設定
    if (paramaters.scaleEaseParm.isScaleEase) {
        particle.scaleEasing = std::make_unique<Easing<Vector3>>();

        // Easingパラメータを構築
        EasingParameter<Vector3> easingParam;
        easingParam.type       = static_cast<EasingType>(paramaters.scaleEaseParm.easeTypeInt);
        easingParam.startValue = particle.scaleInfo.tempScaleV3;
        easingParam.endValue   = particle.scaleInfo.easeEndScale;
        easingParam.maxTime    = paramaters.scaleEaseParm.maxTime;
        easingParam.backRatio  = paramaters.scaleEaseParm.backRatio;
        easingParam.finishType = EasingFinishValueType::End;

        // Easingに設定
        particle.scaleEasing->SettingValue(easingParam);

        //  WorldTransformのscaleに直接適用
        particle.scaleEasing->SetAdaptValue(&particle.scaleInfo.easingAdaptParam);
    }

    ///------------------------------------------------------------------------
    /// 色設定
    ///------------------------------------------------------------------------
    Vector4 randomColor = {
        Random::Range(paramaters.colorDist.min.x, paramaters.colorDist.max.x),
        Random::Range(paramaters.colorDist.min.y, paramaters.colorDist.max.y),
        Random::Range(paramaters.colorDist.min.z, paramaters.colorDist.max.z),
        0.0f};
    particle.color_ = paramaters.baseColor + randomColor;

    ///------------------------------------------------------------------------
    /// UVTransform設定
    ///------------------------------------------------------------------------
    float frameWidth = 1.0f;
    if (paramaters.uvParm.numOfFrame != 0) {
        frameWidth = 1.0f / float(paramaters.uvParm.numOfFrame);
    }
    const float stopPosition = 1.0f - frameWidth;

    particle.uvInfo_.pos               = Vector3(paramaters.uvParm.pos.x, paramaters.uvParm.pos.y, 1.0f);
    particle.uvInfo_.rotate            = paramaters.uvParm.rotate;
    particle.uvInfo_.scale             = Vector3(frameWidth, 1.0f, 1.0f);
    particle.uvInfo_.frameDistance_    = frameWidth;
    particle.uvInfo_.frameScroolSpeed  = paramaters.uvParm.frameScrollSpeed;
    particle.uvInfo_.uvStopPos_        = stopPosition;
    particle.uvInfo_.isScroolEachPixel = paramaters.uvParm.isScrollEachPixel;
    particle.uvInfo_.isRoop            = paramaters.uvParm.isLoop;
    particle.uvInfo_.isScrool          = paramaters.uvParm.isScroll;
    particle.uvInfo_.isFlipX           = paramaters.uvParm.isFlipX;
    particle.uvInfo_.isFlipY           = paramaters.uvParm.isFlipY;
    particle.uvInfo_.currentScroolTime = 0.0f;

    ///------------------------------------------------------------------------
    /// 重力設定
    ///------------------------------------------------------------------------
    particle.gravity_ = paramaters.gravity;

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