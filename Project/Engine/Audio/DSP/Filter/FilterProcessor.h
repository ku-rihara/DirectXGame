#pragma once

#include <vector>

namespace KetaEngine {

class IAudioFilter;
class AudioPCM;

/// <summary>
/// フィルターを波形データへ適用する
/// </summary>
class FilterProcessor {
public:
    // 配列をその場で処理(フィルターの内部状態は引き継ぐのでストリーム処理にも使える)
    void Apply(IAudioFilter& filter, std::vector<double>& samples) const;

    // 全チャンネルをその場で処理(チャンネル毎にResetしてから処理)
    void Apply(IAudioFilter& filter, AudioPCM& pcm) const;
};

}; // KetaEngine
