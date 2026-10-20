#pragma once

namespace KetaEngine {

/// <summary>
/// 波形フィルターのインターフェース(1サンプルずつ処理する)
/// </summary>
class IAudioFilter {
public:
    virtual ~IAudioFilter() = default;

    // 内部状態(遅延器)をクリア
    virtual void Reset() = 0;

    // 1サンプル処理
    virtual double ProcessSample(double x) = 0;
};

}; // KetaEngine
