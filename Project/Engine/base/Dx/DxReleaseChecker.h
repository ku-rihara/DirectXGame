#pragma once

namespace KetaEngine {

/// <summary>
/// DirectXリソースのリリースチェックを行うクラス
/// </summary>
class DxReleaseChecker {
public: 
    ~DxReleaseChecker(); //< デストラクタでリソースリークをチェック
    static void CheckLeaks(); //< 明示的にリソースリークをチェック
};

}; // KetaEngine
