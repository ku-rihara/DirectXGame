// ParticleEmitter.cpp

#include "ParticleEmitter.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include "Function/GetFile.h"
#include "ParticleCommon.h"
#include "ParticleManager.h"
#include "ParticleManager.h"

#include <format>
#include <imgui.h>

ParticleEmitter::ParticleEmitter() {
}

///=================================================================================
/// パーティクル作成
///=================================================================================
ParticleEmitter* ParticleEmitter::CreateParticle(const std::string& name, const std::string& modelFilePath,
    const std::string& extension, const int32_t& maxnum) {

    auto emitter           = std::make_unique<ParticleEmitter>();
    emitter->particleName_ = name;
    ParticleManager::GetInstance()->CreateParticleGroup(emitter->particleName_, modelFilePath, extension, maxnum);
    emitter->Init();
    return emitter.release();
}


///=================================================================================
/// パーティクル作成(Primitive)
///=================================================================================
ParticleEmitter* ParticleEmitter::CreateParticlePrimitive(const std::string& name, const PrimitiveType& primitiveType, const int32_t& maxnum) {

    auto emitter           = std::make_unique<ParticleEmitter>();
    emitter->particleName_ = name;
    ParticleManager::GetInstance()->CreatePrimitiveParticle(emitter->particleName_, primitiveType, maxnum);
    emitter->Init();
    return emitter.release();
}

/// =================================================================================
/// 初期化
/// =================================================================================
void ParticleEmitter::Init() {

    ParticleParameter::Init();

    // レールマネージャー
    railManager_ = std::make_unique<RailManager>();
    railManager_->Init(particleName_);

    /// 発生位置可視化オブジェ
    obj3d_.reset(Object3d::CreateModel("DebugCube", ".obj"));
    emitBoxTransform_.Init();
}

///=================================================================================
/// エミット
///=================================================================================
void ParticleEmitter::Emit() {

    // 　発生座標のパターン切り替え
    if (isMoveForRail_) {
        parameters_.emitPos = railManager_->GetWorldTransform().GetWorldPos();
    } else {
        parameters_.emitPos = parameters_.emitPos;
    }

    currentTime_ += Frame::DeltaTime(); // 時間加算

    if (currentTime_ >= intervalTime_ || groupParamaters_.isShot) { // 　間隔ごとに発動

        ParticleManager::GetInstance()->Emit(
            particleName_, parameters_, groupParamaters_, particleCount);
        currentTime_ = 0.0f; // 時間を戻す
    }
}

///=================================================================================
/// ImGui更新
///=================================================================================
void ParticleEmitter::Update() {
    // レール更新
    railManager_->Update(moveSpeed_);

    UpdateEmitTransform();

    SetValues();
}

void ParticleEmitter::EditorUpdate() {
#ifdef _DEBUG

    ImGui::Begin(particleName_.c_str());
    ImGui::PushID(particleName_.c_str());

    // Color
    if (ImGui::CollapsingHeader("Color")) {
        ImGui::SeparatorText("Base Color:");
        ImGui::ColorEdit4("Base", &parameters_.baseColor.x);

        ImGui::SeparatorText("Color Range:");
        ImGui::ColorEdit4("Max", &parameters_.colorDist.max.x);
        ImGui::ColorEdit4("Min", &parameters_.colorDist.min.x);
    }

    /// rail
    if (ImGui::CollapsingHeader("MoveForRail")) {
        ImGui::SeparatorText("Paramater");
        ImGui::Checkbox("isMoveForRail", &isMoveForRail_);
        ImGui::DragFloat("moveSpeed", &moveSpeed_, 0.001f);

        ImGui::SeparatorText("ControlPoints:");
        railManager_->ImGuiEdit();
    }

    // Position
    if (ImGui::CollapsingHeader("Position")) {
        ImGui::SeparatorText("Position Base:");
        ImGui::DragFloat3("Base", &parameters_.emitPos.x, 0.1f);

        ImGui::SeparatorText("Position Range:");
        ImGui::DragFloat3("Position Max", &parameters_.positionDist.max.x, 0.1f);
        ImGui::DragFloat3("Position Min", &parameters_.positionDist.min.x, 0.1f);
    }

    // Velocity
    if (ImGui::CollapsingHeader("Velocity")) {
        ImGui::SeparatorText("Velocity Range:");
        ImGui::DragFloat("Velocity Max", &parameters_.speedDist.max, 0.1f);
        ImGui::DragFloat("Velocity Min", &parameters_.speedDist.min, 0.1f);

        ImGui::SeparatorText("Direction Range:");
        ImGui::DragFloat3("Direction Max", &parameters_.directionDist.max.x, 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat3("Direction Min", &parameters_.directionDist.min.x, 0.01f, -1.0f, 1.0f);
    }

    // scale Parm
    ScaleParmEditor();

    // Rotate
    if (ImGui::CollapsingHeader("Rotate(Degree)")) {

        ImGui::DragFloat3("BaseRotate", &parameters_.baseRotate.x, 0.1f, 0, 360);
        ImGui::DragFloat3("Rotate Max", &parameters_.rotateDist.max.x, 0.1f, 0, 360);
        ImGui::DragFloat3("Rotate Min", &parameters_.rotateDist.min.x, 0.1f, 0, 360);
    }

    if (ImGui::CollapsingHeader("Rotate Speed(Degree)")) {
        ImGui::DragFloat3("Rotate Speed Max", &parameters_.rotateSpeedDist.max.x, 0.1f, 0, 720);
        ImGui::DragFloat3("Rotate Speed Min", &parameters_.rotateSpeedDist.min.x, 0.1f, 0, 720);
    }

    // その他のパラメータ
    if (ImGui::CollapsingHeader("etcParamater")) {
        ImGui::DragFloat("IntervalTime", &intervalTime_, 0.01f, 0.01f, 100.0f);
        ImGui::DragFloat("Gravity", &parameters_.gravity, 0.1f);
        ImGui::DragFloat("LifeTime", &parameters_.lifeTime, 0.01f);
        ImGui::SliderInt("Particle Count", &particleCount, 1, 100);
    }

    // チェックが有効なら、BillboardType の設定を表示
    if (ImGui::CollapsingHeader("BillBoard")) {

        // IsBillBoard のチェックボックス
        ImGui::Checkbox("IsBillBoard", &groupParamaters_.isBillBord);

        ImGui::SeparatorText("IsRotateAdapt");
        ImGui::Checkbox("IsX", &groupParamaters_.adaptRotate_.isX_);
        ImGui::Checkbox("IsY", &groupParamaters_.adaptRotate_.isY_);
        ImGui::Checkbox("IsZ", &groupParamaters_.adaptRotate_.isZ_);

        ImGui::SeparatorText("BillBordType");

        const char* items[] = {"XYZ", "X", "Y", "Z"}; // ビルボードの種類
        // ビルボードの種類を選択するコンボボックス
        if (ImGui::Combo("Billboard Type", &preBillBordType_, items, IM_ARRAYSIZE(items))) {
            // 選択した値を反映
            groupParamaters_.billBordType = static_cast<WorldTransform::BillboardType>(preBillBordType_);
        }
    }

    // その他のパラメータ
    if (ImGui::CollapsingHeader("Frag")) {

        // IsRotateforDirection のチェックボックス
        ImGui::Checkbox("IsRotateforDirection", &parameters_.isRotateforDirection);
        ImGui::Checkbox("IsShot", &preIsShot_);
        ImGui::Checkbox("isAlphaNoMove", &groupParamaters_.isAlphaNoMove);
    }

    ImGuiTextureSelection();

    // パーティクル切り替え
    ParticleChange();

    ParmSaveForImGui();
    ParmLoadForImGui();

    ImGui::PopID();
    ImGui::End();
#endif // _DEBUG
}

void ParticleEmitter::UpdateEmitTransform() {
    emitBoxTransform_.translation_ = parameters_.emitPos;
    // スケールを範囲の大きさで設定
    emitBoxTransform_.scale_ = {
        parameters_.positionDist.max.x - parameters_.positionDist.min.x,
        parameters_.positionDist.max.y - parameters_.positionDist.min.y,
        parameters_.positionDist.max.z - parameters_.positionDist.min.z};
    railManager_->SetScale(emitBoxTransform_.scale_);

    emitBoxTransform_.UpdateMatrix();
}

void ParticleEmitter::RailDraw(const ViewProjection& viewProjection) {
    viewProjection;
    /*railManager_->RailDraw(viewProjection);*/
}
void ParticleEmitter::DebugDraw(const ViewProjection& viewProjection) {
#ifdef _DEBUG

    if (isMoveForRail_) { // レールに沿うエミット位置
        railManager_->Draw(viewProjection);

    } else { // レールに沿わないエミット位置
        obj3d_->Draw(emitBoxTransform_, viewProjection);
    }
#endif // _DEBUG
}

// 共通部分を関数化
void ParticleEmitter::DisplayFileSelection(const std::string& header, const std::vector<std::string>& filenames, int& selectedIndex, const std::function<void(const std::string&)>& onApply) {
    if (!filenames.empty()) {
        std::vector<const char*> names;
        for (const auto& file : filenames) {
            names.push_back(file.c_str());
        }

        if (ImGui::CollapsingHeader(header.c_str())) {
            // リストボックスの表示
            ImGui::ListBox(header.c_str(), &selectedIndex, names.data(), static_cast<int>(names.size()));

            // 適用ボタン
            if (ImGui::Button(("Apply:" + header).c_str())) {
                onApply(filenames[selectedIndex]);
            }
        }
    } else {
        ImGui::Text("No %s files found.", header.c_str());
    }
}

// Particleファイル選択
void ParticleEmitter::ParticleChange() {
    static int selectedIndex           = 0; // 現在選択中のインデックス
    std::vector<std::string> filenames = ParticleManager::GetInstance()->GetParticleFiles();

    DisplayFileSelection("SelectParticle", filenames, selectedIndex, [this](const std::string& selectedFile) {
        ApplyGlobalParameter(selectedFile);
        globalParameter_->LoadFile(selectedFile, folderName_);
        ImGui::Text("Load Successful: %s", (folderName_ + selectedFile).c_str());
    });
}

/// texture選択
void ParticleEmitter::ImGuiTextureSelection() {
    static int selectedIndex           = 0; // 現在選択中のインデックス
    std::vector<std::string> filenames = GetFileNamesForDyrectry(textureFilePath_);

    DisplayFileSelection("SelectTexture", filenames, selectedIndex, [this](const std::string& selectedFile) {
        ApplyTexture(selectedFile);

        ImGui::Text("Texture Applied: %s", selectedFile.c_str());
    });
}
void ParticleEmitter::ScaleParmEditor() {
    // Scale
    if (ImGui::CollapsingHeader("Scale")) {
        ImGui::SeparatorText("Scale Mode");

        ImGui::Checkbox("IsScalerScale", &parameters_.isScalerScale);
        ImGui::Checkbox("IsEasingMode", &parameters_.scaleEaseParm.isScaleEase);

        if (parameters_.isScalerScale) {
            ImGui::SeparatorText("Scaler Range");
            ImGui::DragFloat("Scale Max", &parameters_.scaleDist.max, 0.1f);
            ImGui::DragFloat("Scale Min", &parameters_.scaleDist.min, 0.1f);
        } else {
            ImGui::SeparatorText("V3 Range");
            ImGui::DragFloat3("ScaleV3 Max", reinterpret_cast<float*>(&parameters_.scaleDistV3.max), 0.1f);
            ImGui::DragFloat3("ScaleV3 Min", reinterpret_cast<float*>(&parameters_.scaleDistV3.min), 0.1f);
        }

        // EaseParm
        if (parameters_.scaleEaseParm.isScaleEase) {

            if (parameters_.isScalerScale) {
                ImGui::SeparatorText("EaseRange Float");
                ImGui::DragFloat("EaseValue Min", &parameters_.scaleEaseParm.endValueF.min, 0.1f);
                ImGui::DragFloat("EaseValue Max", &parameters_.scaleEaseParm.endValueF.max, 0.1f);
            } else {
                ImGui::SeparatorText("EaseRange V3");
                ImGui::DragFloat3("EaseValueV3 Max", reinterpret_cast<float*>(&parameters_.scaleEaseParm.endValueV3.max), 0.1f);
                ImGui::DragFloat3("EaseValueV3 Min", reinterpret_cast<float*>(&parameters_.scaleEaseParm.endValueV3.min), 0.1f);
            }
            // イージングパラメータ
            ImGui::SeparatorText("Ease Paramater");
            ImGui::DragFloat("maxTime", &parameters_.scaleEaseParm.maxTime, 0.01f);
            ImGui::SeparatorText("EaseType");
            // イージング種類
            const char* easeItems[] = {"InSine", "OutSine", "OutBack", "OutQuint"};
            // ビルボードの種類を選択するコンボボックス
            if (ImGui::Combo("Easing Type", &parameters_.scaleEaseParm.easeTypeInt, easeItems, IM_ARRAYSIZE(easeItems))) {
                // 選択した値を反映
                parameters_.scaleEaseParm.easeType = static_cast<EaseType>(parameters_.scaleEaseParm.easeTypeInt);
            }
        }
    }
}

/// <summary>
/// setter
/// </summary>
/// <param name="parent"></param>

void ParticleEmitter::SetParentBasePos(WorldTransform* parent) {
    emitBoxTransform_.parent_ = parent;
}

void ParticleEmitter::SetBlendMode(const ParticleCommon::BlendMode& blendmode) {
    groupParamaters_.blendMode = blendmode;
}

void ParticleEmitter::SetBillBordType(const WorldTransform::BillboardType& billboardType) {
    groupParamaters_.billBordType = billboardType;
}

void ParticleEmitter::SetParentTransform(const WorldTransform* transform) {
    parameters_.parentTransform = transform;
}

void ParticleEmitter::SetFollowingPos(const Vector3* pos) {
    parameters_.followingPos_ = pos;
}

///=================================================================================
/// テクスチャ切り替え
///=================================================================================
void ParticleEmitter::SetTextureHandle(const uint32_t& handle) {
    ParticleManager::GetInstance()->SetTextureHandle(particleName_, handle);
    // SetSelectedTexturePath(handle); // 選択されたテクスチャハンドルを更新
}

/// override function

void ParticleEmitter::ParmSaveForImGui() {
    ParticleParameter::ParmSaveForImGui();
}

void ParticleEmitter::ParmLoadForImGui() {
    ParticleParameter::ParmLoadForImGui();
}
void ParticleEmitter::ApplyGlobalParameter(const std::string& particleName) {
    ParticleParameter::ApplyGlobalParameter(particleName);
}
void ParticleEmitter::ApplyTexture(const std::string& texturename) {
    ParticleParameter::ApplyTexture(texturename);
}

void ParticleEmitter::SetValues() {
    ParticleParameter::SetValues();
}
