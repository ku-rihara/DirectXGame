#pragma once

#include <cstddef>
#include <vector>

namespace KetaEngine {

/// <summary>
/// 波形加工用のPCMデータ(-1.0～1.0に正規化したdouble)
/// フォーマット変換やファイル入出力は持たない純粋なデータクラス
/// </summary>
class AudioPCM {
public:
    AudioPCM() = default;

    /// <summary>
    /// 指定サイズで確保(無音で初期化)
    /// </summary>
    /// <param name="sampleRate">標本化周波数</param>
    /// <param name="channelCount">チャンネル数</param>
    /// <param name="length">1チャンネルあたりのサンプル数</param>
    AudioPCM(int sampleRate, int channelCount, size_t length);

    // 確保し直し(無音で初期化)
    void Allocate(int sampleRate, int channelCount, size_t length);

    // サンプル数変更(増えた分は無音)
    void Resize(size_t length);

    // 全サンプルを0にする
    void Clear();

    // 全チャンネルに同じ波形を設定(長さも合わせる)
    void FillAllChannels(const std::vector<double>& wave);

    // 全チャンネルにゲインを掛ける
    void ApplyGain(double gain);

    // 最大振幅がpeakになるよう正規化
    void Normalize(double peak = 0.9);

    // 先頭・末尾にフェードイン/アウトを掛ける
    void ApplyFade(double fadeInSec, double fadeOutSec);

    // otherを指定位置へ加算合成(チャンネル数が違う場合はotherのチャンネルを繰り返し使う)
    void MixFrom(const AudioPCM& other, size_t destOffset, double gain = 1.0);

    // 全チャンネルの平均でモノラル化した波形
    std::vector<double> GetMonoMix() const;

private:
    int sampleRate_ = 44100;
    std::vector<std::vector<double>> channels_; //< [チャンネル][サンプル]

public:
    //*----------------------------- getter Methods -----------------------------*//

    int GetSampleRate() const { return sampleRate_; }
    int GetChannelCount() const { return static_cast<int>(channels_.size()); }
    size_t GetLength() const { return channels_.empty() ? 0 : channels_[0].size(); }
    double GetDuration() const { return sampleRate_ > 0 ? static_cast<double>(GetLength()) / sampleRate_ : 0.0; }
    bool IsEmpty() const { return GetLength() == 0; }

    std::vector<double>& GetChannel(int channel) { return channels_[channel]; }
    const std::vector<double>& GetChannel(int channel) const { return channels_[channel]; }
};

}; // KetaEngine
