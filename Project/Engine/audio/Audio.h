#pragma once
#include<wrl.h>
#include<xaudio2.h>
#include "struct/SoundData.h"
#include<string>
#include<unordered_map>
#include <vector>
#include <chrono>

/// <summary>
///  オーディオクラス
/// </summary>
class Audio {
private:
	//XAudio2のインスタンス
	Microsoft::WRL::ComPtr<IXAudio2>xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;
	std::vector<SoundData> soundDatas_;
	std::unordered_map<std::string, int> soundIndexMap_;

	std::unordered_map<int, std::chrono::steady_clock::time_point> lastPlayTimes_;

	const std::string filePath_ = "Resources/Audio/";

public:
	static Audio* GetInstance();

	Audio() = default;
    ~Audio() = default;

	void Init();
	int LoadWave(const std::string& filename);
	void Unload(const int& soundId);
	void PlayWave(const int& soundId, const float& volume);
	void Finalize();

	IXAudio2* GetXAudio2()const { return xAudio2_.Get(); }
};

