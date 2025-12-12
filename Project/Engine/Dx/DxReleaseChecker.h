#pragma once

/// <summary>
/// DirectXリソースのリリースチェックを行うクラス
/// </summary>
namespace KetaEngine {

class DxReleaseChecker {
public: 
    ~DxReleaseChecker(); //< デストラクタでリソースリークをチェック
};

}; // KetaEngine
