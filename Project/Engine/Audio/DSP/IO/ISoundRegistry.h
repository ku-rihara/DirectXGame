#pragma once

#include <string>
#include <vector>

namespace KetaEngine {

class AudioPCM;

/// <summary>
/// 波形データを再生可能なサウンドとして登録・取得するインターフェース
/// (DSP側は再生エンジンの実装を知らずに済む)
/// </summary>
class ISoundRegistry {
public:
    virtual ~ISoundRegistry() = default;

    // 波形を名前付きで登録(同名は差し替え)
    virtual bool Register(const std::string& soundName, const AudioPCM& pcm) = 0;

    // 登録済みサウンドを波形として取得
    virtual bool TryGet(const std::string& soundName, AudioPCM& outPCM) const = 0;

    virtual void Play(const std::string& soundName, float volume) = 0;

    // 登録済みサウンド名一覧
    virtual std::vector<std::string> GetSoundNames() const = 0;
};

}; // KetaEngine
