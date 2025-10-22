#pragma once

/// <summary>
/// DirectXリソースのリリースチェックを行うクラス
/// </summary>
class DxReleaseChecker {
public: 
    ~DxReleaseChecker(); //< デストラクタでリソースリークをチェック
};