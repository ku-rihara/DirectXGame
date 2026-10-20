#pragma once

#include <vector>
#include <Windows.h>
#include <mmreg.h>

namespace KetaEngine {

class AudioPCM;

/// <summary>
/// AudioPCM と PCMバイト列(WAVEFORMATEX形式)の相互変換
/// </summary>
class PCMConverter {
public:
    /// <summary>
    /// バイト列からAudioPCMへ(8/16/24/32bit整数、32bit浮動小数に対応)
    /// </summary>
    /// <returns>対応していないフォーマットの場合はfalse</returns>
    bool Decode(const WAVEFORMATEX& format, const std::vector<BYTE>& buffer, AudioPCM& outPCM) const;

    // AudioPCMから16bitPCMのバイト列へ(クリッピングあり)
    void Encode16(const AudioPCM& pcm, WAVEFORMATEX& outFormat, std::vector<BYTE>& outBuffer) const;
};

}; // KetaEngine
