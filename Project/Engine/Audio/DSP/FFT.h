#pragma once

#include <complex>
#include <vector>

namespace KetaEngine {

/// <summary>
/// 離散フーリエ変換 / 高速フーリエ変換
/// </summary>
class FFT {
public:
    using Complex = std::complex<double>;

    /// <summary>
    /// 高速フーリエ変換(その場で変換、サイズは2のべき乗)
    /// </summary>
    /// <param name="x">変換するデータ</param>
    /// <param name="isInverse">trueで逆変換(結果はNで割られる)</param>
    static void Transform(std::vector<Complex>& x, bool isInverse = false);

    // 実数データをFFT(サイズはdataのサイズ以上の2のべき乗に0埋め)
    static std::vector<Complex> Forward(const std::vector<double>& data);

    // 逆FFTして実部を返す
    static std::vector<double> InverseReal(std::vector<Complex> spectrum);

    // 振幅スペクトル(|X(k)|)
    static std::vector<double> Magnitude(const std::vector<Complex>& spectrum);

    // 素朴な離散フーリエ変換 O(N^2)(検証用)
    static std::vector<Complex> DFT(const std::vector<double>& data);

    static bool IsPowerOfTwo(size_t n) { return n != 0 && (n & (n - 1)) == 0; }
    static size_t NextPowerOfTwo(size_t n);
};

}; // KetaEngine
