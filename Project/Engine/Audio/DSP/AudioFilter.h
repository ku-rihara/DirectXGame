#pragma once

#include <memory>
#include <vector>

namespace KetaEngine {

class AudioPCM;

/// <summary>
/// 波形フィルターの基底クラス(1サンプルずつ処理する)
/// </summary>
class AudioFilter {
public:
    virtual ~AudioFilter() = default;

    // 内部状態(遅延器)をクリア
    virtual void Reset() = 0;

    // 1サンプル処理
    virtual double ProcessSample(double x) = 0;

    // 配列をその場で処理(内部状態は引き継ぐのでストリーム処理にも使える)
    void Process(std::vector<double>& samples);

    // 全チャンネルをその場で処理(チャンネル毎にResetしてから処理)
    void Process(AudioPCM& pcm);
};

/// <summary>
/// 複数フィルターの直列接続
/// </summary>
class AudioFilterChain : public AudioFilter {
public:
    void Reset() override;
    double ProcessSample(double x) override;

    // フィルター追加(追加順に処理)
    AudioFilter* Add(std::unique_ptr<AudioFilter> filter);
    void ClearFilters() { filters_.clear(); }

private:
    std::vector<std::unique_ptr<AudioFilter>> filters_;

public:
    size_t GetFilterCount() const { return filters_.size(); }
    AudioFilter* GetFilter(size_t index) const { return filters_[index].get(); }
};

/// <summary>
/// 複数フィルターの並列接続(各出力の和)
/// </summary>
class AudioFilterBank : public AudioFilter {
public:
    void Reset() override;
    double ProcessSample(double x) override;

    AudioFilter* Add(std::unique_ptr<AudioFilter> filter);
    void ClearFilters() { filters_.clear(); }

private:
    std::vector<std::unique_ptr<AudioFilter>> filters_;

public:
    size_t GetFilterCount() const { return filters_.size(); }
};

}; // KetaEngine
