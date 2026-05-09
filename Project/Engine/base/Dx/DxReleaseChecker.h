#pragma once

/// <summary>
/// DirectXリソースのリリースチェックを行うクラス
/// </summary>
namespace KetaEngine {

class DxReleaseChecker {
public: 
    ~DxReleaseChecker(); //< デストラクタでリソースリークをチェック
    static void CheckLeaks(); //< 明示的にリソースリークをチェック
};

}; // KetaEngine
