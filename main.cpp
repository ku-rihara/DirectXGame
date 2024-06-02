#include"Keta.h"
#include"Transform.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include "externals/imgui/imgui.h"
#include"Vector2.h"
#include<random>

const char kWindowTitle[] = "CG2";

float Clamp(float n, float min, float max);
float easeOutSine(float EasingTime, float start, float end);
float InOutQuart(float t, float totaltime, float min, float max);

struct Emitter {
	Vector3 pos;
	Vector3 velocity;
	Vector3 direction;
	float  scale;
	float theta;
	int lifeTime;
};
//windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ライブラリの初期化
	Keta::Initialize(kWindowTitle, 1280, 720);
	DirectXCommon* dxcommon;
	dxcommon = DirectXCommon::GetInstance();
	//乱数
	std::random_device random;
	std::mt19937 gen(random());
	//黄色の三角形
	std::uniform_real_distribution<>distribX(-2.3f, 2.3f);//Xの乱数
	std::uniform_real_distribution<>distribY(-6.0f, 0.0f);//Yの乱数
	std::uniform_real_distribution<>distribZ(0.0f, 8.0f);//Zの乱数
	std::uniform_real_distribution<>distribSpeed(0.004f, 0.01f);//スピードの乱数
	std::uniform_real_distribution<>distribScale(0.1f, 0.4f);//スケールの乱数
	//背景三角形
	std::uniform_real_distribution<>distribBackScale(0.2f, 0.6f);//スケールの乱数
	std::uniform_real_distribution<>distribBackX(-1.0f, 1.0f);//向きの乱数
	std::uniform_real_distribution<>distribBackY(-1.0f, 1.0f);//向きの乱数
	std::uniform_real_distribution<>distribRotateX(0.0f, 2.0f*3.1419f);//回転乱数
	std::uniform_real_distribution<>distribRotateY(0.0f, 2.0f * 3.1419f);//回転乱数
	std::uniform_real_distribution<>distribRotateZ(0.0f, 2.0f * 3.1419f);//回転乱数
	std::uniform_real_distribution<>distribBackSpeed(0.01f, 0.02f);//スピードの乱数
	//std::uniform_real_distribution<>distribBackZ(-1.0f, 1.0f);//向きの乱数

	//イージング
	float easeTime = 0.0f;
	float rotateY = 0.0f;
	float alphaEaseTime[triangleNum]{};

	//パーティクル関連
	Emitter fromtEmitter{};
	Emitter backEmitter{};
	int collTime = 0;
	bool is1Lap[triangleNum]{};
	//三角形
	Transform triangleTramsform[triangleNum]{};
	Vector3 triangleDirection[triangleNum]{};
	Vector3 velocity[triangleNum]{};
	bool isStart[triangleNum]{};
	float alpha[triangleNum]{};
	//行列
	Matrix4x4 worldMatrix[triangleNum]{};
	Matrix4x4 worldViewProjectionMatrix[triangleNum]{};
	Transform cameraTransform;

	//映像に使わない三角形
	Vector4 color{1,1,1,1.0f};
	for (int i = triangleNum - 2; i < triangleNum; i++) {
		triangleTramsform[i].scale = { 1,1,1 };
		triangleTramsform[triangleNum - 2].rotate = { 0.0f,-0.758f,0.008f };
		triangleTramsform[triangleNum - 1].rotate = { 0.0f,0.758f,0.008f };
		triangleTramsform[triangleNum - 2].translate = {};
		triangleTramsform[triangleNum - 1].translate = {0,0.3f,0 };
	}
	cameraTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,-5.0f} };

	//ウィンドウのxボタンが押されるまでループ
	while (Keta::ProcessMessage() == 0) {
		//フレームの開始
		Keta::BeginFrame();
		//カメラ行列
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kWindowWidth) / float(WinApp::kWindowHeight), 0.1f, 100.0f);

		if (!dxcommon->GetIsVideo()) {//ビデオモードじゃないとき
#ifdef _DEBUG
			ImGui::Begin("Window");
			ImGui::ColorEdit4("color", (float*)&color);
			if (ImGui::TreeNode("Triangle1")){
				ImGui::DragFloat3("Scale", &triangleTramsform[triangleNum - 2].scale.x, 0.01f);
				ImGui::DragFloat3("Rotate", &triangleTramsform[triangleNum - 2].rotate.x, 0.01f);
				ImGui::DragFloat3("Translate", &triangleTramsform[triangleNum - 2].translate.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Triangle2")) {
				ImGui::DragFloat3("Scale", &triangleTramsform[triangleNum - 1].scale.x, 0.01f);
				ImGui::DragFloat3("Rotate", &triangleTramsform[triangleNum - 1].rotate.x, 0.01f);
				ImGui::DragFloat3("Translate", &triangleTramsform[triangleNum - 1].translate.x, 0.01f);
				ImGui::TreePop();
			}
			ImGui::End();
#endif
			
		}
		else {
			//映像に必要ない三角形は消す
			for (int i = triangleNum - 2; i < triangleNum; i++) {
				triangleTramsform[i].scale = { 0,0,0 };		
			}
			
			//回転イージング
			easeTime += 0.01f;
			rotateY = InOutQuart(easeTime, 0.8f, 0, 2.0f * 3.14f);
			if (easeTime >= 0.8f) {
				easeTime = 0;
			}

			//パーティクル処理------------------------------------------------------------------------------------------------------------------------------------
			//発生処理(黄色Triangle)
			for (int i = 0; i < frontTriangleNum; i++) {

				if (isStart[i] == false) {

					//発生位置
					fromtEmitter.pos.x = float(distribX(gen));
					if (is1Lap[i] == false) {
						fromtEmitter.pos.y = float(distribY(gen));
						is1Lap[i] = true;
					}
					else {
						fromtEmitter.pos.y = -3.5f;//2周目以降は-2
					}
					fromtEmitter.pos.z = float(distribZ(gen));
					//散る向き
					fromtEmitter.direction = Normnalize({ 0.0f,1.0f,0.0f });
					//散る速度
					fromtEmitter.velocity = { 0.0f,float(distribSpeed(gen)),0.0f };
					//発生時のスケール
					fromtEmitter.scale = float(distribScale(gen));

					//代入**********************************+**************
					triangleTramsform[i].translate = fromtEmitter.pos;
					triangleTramsform[i].scale = { fromtEmitter.scale,fromtEmitter.scale,fromtEmitter.scale };
					triangleDirection[i] = fromtEmitter.direction;
					velocity[i] = fromtEmitter.velocity;
					alpha[i] = 1.0f;
					alphaEaseTime[i] = 0.0f;
					isStart[i] = true;
				}
			}
			collTime++;
			//発生処理(背景Triangle)
			for (int i = frontTriangleNum; i < triangleNum-2; i++) {
				if (isStart[i] == false) {
					
					//発生位置
					backEmitter.pos = { 0,0,3 };
					//スケール
					backEmitter.scale = float(distribBackScale(gen));
					//向き
					backEmitter.direction = Normnalize({ float(distribBackX(gen)),float(distribBackY(gen)) ,-0.0f });
					//スピード
					backEmitter.velocity = { float(distribBackSpeed(gen)),float(distribBackSpeed(gen)) ,float(distribBackSpeed(gen)) };
					//代入**********************************+**************
					triangleTramsform[i].translate = backEmitter.pos;
					triangleTramsform[i].rotate = { float(distribRotateX(gen)),float(distribRotateY(gen)) ,float(distribRotateZ(gen)) };
					triangleTramsform[i].scale = { backEmitter.scale,backEmitter.scale,backEmitter.scale };
					velocity[i] = backEmitter.velocity;
					triangleDirection[i] = backEmitter.direction;
					alpha[i] = 0.2f;
					alphaEaseTime[i] = 0.0f;
					//一定間隔ごとに発射
					if (collTime >= 2) {
						isStart[i] = true;
						collTime = 0;					
					}
					break;
				}

			}
			//発生後処理(黄色Triangle)
			for (int i = 0; i < frontTriangleNum; i++) {
				if (isStart[i] == true) {
					//移動
					triangleTramsform[i].translate.x += velocity[i].x * triangleDirection[i].x;
					triangleTramsform[i].translate.y += velocity[i].y * triangleDirection[i].y;
					triangleTramsform[i].translate.z += velocity[i].z * triangleDirection[i].z;
					//回転のイージング
					triangleTramsform[i].rotate.y = rotateY;

					if (triangleTramsform[i].translate.y >= -0.3f) {
						//透明度のイージング
						alphaEaseTime[i] += 0.005f;
						alphaEaseTime[i] = Clamp(alphaEaseTime[i], 0, 1);

						alpha[i] = easeOutSine(alphaEaseTime[i], 1, 0);
						if (alphaEaseTime[i] >= 1.0f) {
							isStart[i] = false;
						}

					}
				}
				dxcommon->SetColor(i, Vector4{ 1.0f,1.0f,0.0f,alpha[i] });//色指定
			}

			//発生後処理(背景Triangle)
			for (int i = frontTriangleNum; i < triangleNum-2; i++) {
				if (isStart[i] == true) {
					//移動
					triangleTramsform[i].translate.x += velocity[i].x * triangleDirection[i].x;
					triangleTramsform[i].translate.y += velocity[i].y * triangleDirection[i].y;
					triangleTramsform[i].translate.z += velocity[i].z * triangleDirection[i].z;
					triangleTramsform[i].rotate.x += 0.01f;
					triangleTramsform[i].rotate.y += 0.01f;
					triangleTramsform[i].rotate.z += 0.01f;

					//一定範囲外にいったら消滅する
					if (triangleTramsform[i].translate.x < -1.5f || triangleTramsform[i].translate.x > 1.5f ||
						triangleTramsform[i].translate.y < -1.5f || triangleTramsform[i].translate.y > 1.5f) {
						//透明度のイージング
						alphaEaseTime[i] += 0.005f;
						alphaEaseTime[i] = Clamp(alphaEaseTime[i], 0, 1);

						alpha[i] = easeOutSine(alphaEaseTime[i], 0.2f, 0);
						if (alphaEaseTime[i] >= 1.0f) {
							isStart[i] = false;
						}
					}

				}
				dxcommon->SetColor(i, Vector4{ 0.0f, 0.08f, 0.08f,alpha[i] });//色指定
			}	
		}
			//----------------------------------------------------------------------------------------------------------------------------------------------------
		//三角形のスクリーン変換
		for (int i = 0; i < triangleNum; i++) {
			//三角形
			worldMatrix[i] = MakeAffineMatrix(triangleTramsform[i].scale, triangleTramsform[i].rotate, triangleTramsform[i].translate);
			worldViewProjectionMatrix[i] = Multiply(worldMatrix[i], Multiply(viewMatrix, projectionMatrix));
			dxcommon->SetwvpDate(i, worldViewProjectionMatrix[i]);
		}
		for (int i = triangleNum - 2; i < triangleNum; i++) {
			dxcommon->SetColor(i, color);//色指定
		}
		

		//フレームの終了
		Keta::EndFrame();
	}

	//ライブラリの終了
	Keta::Finalize();
	return 0;
}

float Clamp(float n, float min, float max) {
	if (n > max) {
		return max;
	}
	if (n < min) {
		return min;
	}
	return n;
}

float easeOutSine(float EasingTime, float start, float end) {
	float result;

	result = sinf((EasingTime * float(3.14259)) / 2);

	return (1 - result) * start + result * end;
}
float InOutQuart(float t, float totaltime, float min, float max) {
	if (t <= 0.0f) return min;
	if (t >= totaltime) return max;

	t /= totaltime * 0.5f;
	float deltaValue = max - min;

	if (t < 1.0f) {
		return min + deltaValue * 0.5f * t * t * t * t;
	}
	else {
		t -= 2.0f;
		return min + deltaValue * (-0.5f * (t * t * t * t - 2.0f));
	}
}