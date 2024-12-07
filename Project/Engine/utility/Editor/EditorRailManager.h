#pragma once
#include <vector>
#include <string>
#include <optional>
#include <d3d12.h>
#include "base/DirectXCommon.h"
#include "base/SrvManager.h"

class EditorRailManager {
public:
	struct Rail {
		std::vector<Vector3> controlPoints; // レールの制御点
		float progress;                    // レール上の進行度（0.0 ～ 1.0）
	};
private:
	void CreateStructuredBuffer(); // StructureBufferの作成

	std::vector<Rail> rails_;        // レールのデータ
	uint32_t maxRails_;              // 最大レール数
	SrvManager* pSrvManager_;        // SRVマネージャ
	ComPtr<ID3D12Resource> buffer_;  // GPU側のStructureBuffer
	Rail* mappedBuffer_;             // マップされたバッファ
	uint32_t srvIndex_;              // SRVのインデックス


	void Init(SrvManager* srvManager, uint32_t maxRails);    // 初期化
	void Update(float deltaProgress);                       // レールの進行度を更新
	void AddRail(const std::vector<Vector3>& controlPoints); // レールを追加
	void UpdateControlPoints(uint32_t railIndex, const std::vector<Vector3>& controlPoints); // 制御点を更新

	void DrawImGui(); // デバッグ用描画
	const Rail& GetRail(uint32_t railIndex) const;          // レールの取得


};
