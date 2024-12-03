#include"ParticleEmitter.h"
#include"ParticleManager.h"
/// file
#include <imgui.h>
#include <fstream>
#include <iostream>


///=====================================================
/// コンストラクタ
///=====================================================
ParticleEmitter::ParticleEmitter() {};

ParticleEmitter* ParticleEmitter::CreateParticle(
	const std::string name, const std::string modelFilePath,
	const std::string& extension, const int32_t& maxnum) {

	std::unique_ptr<ParticleEmitter>emitter = std::make_unique<ParticleEmitter>();

	/// グループ作成
	emitter->particleName_ = name;
	ParticleManager::GetInstance()->CreateParticleGroup(
		emitter->particleName_, modelFilePath, extension, maxnum);

    /// 初期化
    emitter->Init();

	return emitter.release();
}

///=====================================================
/// 初期化
///=====================================================
void ParticleEmitter::Init() {
    particleCount_ = 0;
    lifeTime_ = 0.0f;
    colorDist_.min = {0,0,0,0};
    colorDist_.max = { 0,0,0,0 };
    baseColor_ = { 0,0,0,0 };

}


///=====================================================
/// エミット
///=====================================================
void ParticleEmitter::Emit() {

	ParticleManager::GetInstance()->Emit(
		particleName_, basePos_, positionDist_, scaleDist_,
		velocityDist_, baseColor_, colorDist_, lifeTime_, particleCount_);
}


///=====================================================
/// Imguiでパラメータ調整
///=====================================================
void ParticleEmitter::EditorUpdate() {
    if (ImGui::Begin("Particle Emitter Editor")) {
  
        ImGui::DragFloat3("Base Position", &basePos_.x, 0.1f);
        ImGui::DragFloat3("Position  Min", &positionDist_.min.x, 0.1f);
        ImGui::DragFloat3("Position  Max", &positionDist_.max.x, 0.1f);
        ImGui::DragFloat3("Scale  Min", &scaleDist_.min.x, 0.1f);
        ImGui::DragFloat3("Scale  Max", &scaleDist_.max.x, 0.1f);
        ImGui::DragFloat3("Velocity  Min", &velocityDist_.min.x, 0.1f);
        ImGui::DragFloat3("Velocity  Max", &velocityDist_.max.x, 0.1f);
        ImGui::ColorEdit4("Base Color", &baseColor_.x);
        ImGui::ColorEdit4("Color  Min", &colorDist_.min.x);
        ImGui::ColorEdit4("Color  Max", &colorDist_.max.x);
        ImGui::DragFloat("Lifetime", &lifeTime_, 0.1f);
        ImGui::DragInt("Particle Count", &particleCount_, 1, 1, 100);

        if (ImGui::Button("Save")) {// セーブ
            SaveParameters(dyrectryPath + particleName_+".json");
        }
        if (ImGui::Button("Load")) {//ロード
            LoadParameters(dyrectryPath + particleName_+".json");
        }
    }
    ImGui::End();
}

///=====================================================
/// パラメータロード
///=====================================================
void ParticleEmitter::LoadParameters(const std::string& filepath) {
    std::ifstream inFile(filepath);
    if (!inFile) {
        assert("Particle File Not Find");
        return;
    }

    nlohmann::json jsonData;
    inFile >> jsonData;

    auto basePosArray = jsonData["basePos"];
    basePos_ = { basePosArray[0], basePosArray[1], basePosArray[2] };
    auto posDistMin = jsonData["positionDist"]["min"];
    positionDist_.min = { posDistMin[0], posDistMin[1], posDistMin[2] };
    auto posDistMax = jsonData["positionDist"]["max"];
    positionDist_.max = { posDistMax[0], posDistMax[1], posDistMax[2] };
    auto scaleDistMin = jsonData["scaleDist"]["min"];
    scaleDist_.min = { scaleDistMin[0], scaleDistMin[1], scaleDistMin[2] };
    auto scaleDistMax = jsonData["scaleDist"]["max"];
    scaleDist_.max = { scaleDistMax[0], scaleDistMax[1], scaleDistMax[2] };
    auto velDistMin = jsonData["velocityDist"]["min"];
    velocityDist_.min = { velDistMin[0], velDistMin[1], velDistMin[2] };
    auto velDistMax = jsonData["velocityDist"]["max"];
    velocityDist_.max = { velDistMax[0], velDistMax[1], velDistMax[2] };
    auto baseColor = jsonData["baseColor"];
    baseColor_ = { baseColor[0], baseColor[1], baseColor[2], baseColor[3] };
    auto colorDistMin = jsonData["colorDist"]["min"];
    colorDist_.min = { colorDistMin[0], colorDistMin[1], colorDistMin[2], colorDistMin[3] };
    auto colorDistMax = jsonData["colorDist"]["max"];
    colorDist_.max = { colorDistMax[0], colorDistMax[1], colorDistMax[2], colorDistMax[3] };
    lifeTime_ = jsonData["lifeTime"].get<float>();
    particleCount_ = jsonData["particleCount"].get<int>();
}

///=====================================================
/// パラメータセーブ
///=====================================================
void ParticleEmitter::SaveParameters(const std::string& filepath)const {
    nlohmann::json jsonData;

    // 基本データをJSON形式に変換
    jsonData["basePos"] = { basePos_.x, basePos_.y, basePos_.z };
    jsonData["positionDist"]["min"] = { positionDist_.min.x, positionDist_.min.y, positionDist_.min.z };
    jsonData["positionDist"]["max"] = { positionDist_.max.x, positionDist_.max.y, positionDist_.max.z };
    jsonData["scaleDist"]["min"] = { scaleDist_.min.x, scaleDist_.min.y, scaleDist_.min.z };
    jsonData["scaleDist"]["max"] = { scaleDist_.max.x, scaleDist_.max.y, scaleDist_.max.z };
    jsonData["velocityDist"]["min"] = { velocityDist_.min.x, velocityDist_.min.y, velocityDist_.min.z };
    jsonData["velocityDist"]["max"] = { velocityDist_.max.x, velocityDist_.max.y, velocityDist_.max.z };
    jsonData["baseColor"] = { baseColor_.x, baseColor_.y, baseColor_.z, baseColor_.w };
    jsonData["colorDist"]["min"] = { colorDist_.min.x, colorDist_.min.y, colorDist_.min.z, colorDist_.min.w };
    jsonData["colorDist"]["max"] = { colorDist_.max.x, colorDist_.max.y, colorDist_.max.z, colorDist_.max.w };
    jsonData["lifeTime"] = lifeTime_;
    jsonData["particleCount"] = particleCount_;

    // JSONデータをファイルに保存
    std::ofstream outFile(filepath);
    if (!outFile) {
        std::cerr << "Error: Could not open file for saving: " << filepath << std::endl;
        return;
    }
    outFile << jsonData.dump(4); // インデント4で保存
}