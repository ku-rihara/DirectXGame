#include "FFT.h"

using namespace KetaEngine;
#include <cassert>
#include <numbers>

void FFT::Transform(std::vector<Complex>& x, bool isInverse) {
    const size_t n = x.size();
    assert(IsPowerOfTwo(n) && "FFTのサイズは2のべき乗にしてください");
    if (n <= 1) {
        return;
    }

    // ビット反転並び替え
    for (size_t i = 1, j = 0; i < n; ++i) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;
        if (i < j) {
            std::swap(x[i], x[j]);
        }
    }

    // バタフライ演算
    const double sign = isInverse ? 1.0 : -1.0;
    for (size_t len = 2; len <= n; len <<= 1) {
        const double theta = sign * 2.0 * std::numbers::pi / static_cast<double>(len);
        const Complex wLen(std::cos(theta), std::sin(theta));
        for (size_t i = 0; i < n; i += len) {
            Complex w(1.0, 0.0);
            for (size_t k = 0; k < len / 2; ++k) {
                const Complex a = x[i + k];
                const Complex b = x[i + k + len / 2] * w;
                x[i + k]           = a + b;
                x[i + k + len / 2] = a - b;
                w *= wLen;
            }
        }
    }

    // 逆変換はNで割る
    if (isInverse) {
        for (auto& v : x) {
            v /= static_cast<double>(n);
        }
    }
}

std::vector<FFT::Complex> FFT::Forward(const std::vector<double>& data) {
    std::vector<Complex> x(NextPowerOfTwo(data.size()), Complex{});
    for (size_t i = 0; i < data.size(); ++i) {
        x[i] = Complex(data[i], 0.0);
    }
    Transform(x, false);
    return x;
}

std::vector<double> FFT::InverseReal(std::vector<Complex> spectrum) {
    Transform(spectrum, true);
    std::vector<double> result(spectrum.size());
    for (size_t i = 0; i < spectrum.size(); ++i) {
        result[i] = spectrum[i].real();
    }
    return result;
}

std::vector<double> FFT::Magnitude(const std::vector<Complex>& spectrum) {
    std::vector<double> result(spectrum.size());
    for (size_t i = 0; i < spectrum.size(); ++i) {
        result[i] = std::abs(spectrum[i]);
    }
    return result;
}

std::vector<FFT::Complex> FFT::DFT(const std::vector<double>& data) {
    const size_t n = data.size();
    std::vector<Complex> result(n, Complex{});
    for (size_t k = 0; k < n; ++k) {
        for (size_t i = 0; i < n; ++i) {
            const double theta = 2.0 * std::numbers::pi * static_cast<double>(k * i) / static_cast<double>(n);
            result[k] += data[i] * Complex(std::cos(theta), -std::sin(theta));
        }
    }
    return result;
}

size_t FFT::NextPowerOfTwo(size_t n) {
    size_t result = 1;
    while (result < n) {
        result <<= 1;
    }
    return result;
}
