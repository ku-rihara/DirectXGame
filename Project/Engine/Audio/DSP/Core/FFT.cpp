#include "FFT.h"

using namespace KetaEngine;
#include <algorithm>
#include <numbers>

FFT::FFT(size_t minSize) {
    Resize(minSize);
}

void FFT::Resize(size_t minSize) {
    // 2のべき乗に切り上げ
    size_t size = 2;
    while (size < minSize) {
        size <<= 1;
    }
    if (size == size_) {
        return;
    }
    size_ = size;

    // ビット反転表
    bitReverse_.assign(size_, 0);
    for (size_t i = 1, j = 0; i < size_; ++i) {
        size_t bit = size_ >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;
        bitReverse_[i] = j;
    }

    // 回転因子
    twiddles_.resize(size_ / 2);
    for (size_t k = 0; k < size_ / 2; ++k) {
        const double theta = -2.0 * std::numbers::pi * static_cast<double>(k) / static_cast<double>(size_);
        twiddles_[k]       = Complex(std::cos(theta), std::sin(theta));
    }
}

void FFT::Forward(std::vector<Complex>& x) const {
    Transform(x, false);
}

void FFT::Inverse(std::vector<Complex>& x) const {
    Transform(x, true);
}

std::vector<FFT::Complex> FFT::ForwardReal(const std::vector<double>& data) const {
    std::vector<Complex> x(size_, Complex{});
    const size_t count = (std::min)(data.size(), size_);
    for (size_t i = 0; i < count; ++i) {
        x[i] = Complex(data[i], 0.0);
    }
    Forward(x);
    return x;
}

std::vector<double> FFT::Magnitude(const std::vector<Complex>& spectrum) const {
    std::vector<double> result(spectrum.size());
    for (size_t i = 0; i < spectrum.size(); ++i) {
        result[i] = std::abs(spectrum[i]);
    }
    return result;
}

void FFT::Transform(std::vector<Complex>& x, bool isInverse) const {
    x.resize(size_, Complex{});

    // ビット反転並び替え
    for (size_t i = 0; i < size_; ++i) {
        if (i < bitReverse_[i]) {
            std::swap(x[i], x[bitReverse_[i]]);
        }
    }

    // バタフライ演算
    for (size_t len = 2; len <= size_; len <<= 1) {
        const size_t half = len / 2;
        const size_t step = size_ / len;
        for (size_t i = 0; i < size_; i += len) {
            for (size_t k = 0; k < half; ++k) {
                const Complex w = isInverse ? std::conj(twiddles_[k * step]) : twiddles_[k * step];
                const Complex a = x[i + k];
                const Complex b = x[i + k + half] * w;
                x[i + k]        = a + b;
                x[i + k + half] = a - b;
            }
        }
    }

    // 逆変換はNで割る
    if (isInverse) {
        for (auto& v : x) {
            v /= static_cast<double>(size_);
        }
    }
}
