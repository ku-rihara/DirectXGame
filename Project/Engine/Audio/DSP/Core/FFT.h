#pragma once

#include <complex>
#include <vector>

namespace KetaEngine {

/// <summary>
/// 高速フーリエ変換(サイズ毎にビット反転表と回転因子を事前計算して使い回す)
/// </summary>
class FFT {
public:
    using Complex = std::complex<double>;

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="minSize">変換サイズ(2のべき乗に切り上げられる)</param>
    explicit FFT(size_t minSize = 1024);

    // 変換サイズを変更(2のべき乗に切り上げ)
    void Resize(size_t minSize);

    // 順変換(xはGetSize()に揃えられる)
    void Forward(std::vector<Complex>& x) const;

    // 逆変換(結果はNで割られる)
    void Inverse(std::vector<Complex>& x) const;

    // 実数データを0埋めして順変換
    std::vector<Complex> ForwardReal(const std::vector<double>& data) const;

    // 振幅スペクトル |X(k)|
    std::vector<double> Magnitude(const std::vector<Complex>& spectrum) const;

private:
    void Transform(std::vector<Complex>& x, bool isInverse) const;

private:
    size_t size_ = 0;
    std::vector<size_t> bitReverse_; //< ビット反転並び替え表
    std::vector<Complex> twiddles_;  //< 回転因子 exp(-2πik/N) (k < N/2)

public:
    size_t GetSize() const { return size_; }
};

}; // KetaEngine
