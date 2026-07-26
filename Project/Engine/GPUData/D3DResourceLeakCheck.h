#pragma once

#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxgidebug.h>
#include <wrl.h>

namespace KetaEngine {

/// <summary>
/// スコープを抜ける際にD3D12/DXGIリソースのリークをチェックする構造体
/// </summary>
struct  D3DResourceLeakChecker {
	
	~D3DResourceLeakChecker() {
		//リリースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);

		}
	}
};

}; // KetaEngine
