#pragma once
#include<wrl.h>
#include<xaudio2.h>
#include "struct/SoundData.h"
#include<string>
#include<unordered_map>
#include <vector>
#include <chrono>
#include <unordered_map>
class Audio {
private:
	//XAudio2のインスタンス
	Microsoft::WRL::ComPtr<IXAudio2>xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;
	std::vector<SoundData> soundDatas_;
	std::unordered_map<std::string, int> soundIndexMap_;

	std::unordered_map<int, std::chrono::steady_clock::time_point> lastPlayTimes_; // 直前の再生時間を記録


public:
	static Audio* GetInstance();

	void Init();
	int LoadWave(const std::string& filename);
	void Unload(const int& soundId);
	void PlayWave(const int& soundId, const float& volume);
	void Finalize();

	IXAudio2* GetXAudio2()const { return xAudio2_.Get(); }
};


//SoundData SoundLoadWave(const char* filename);
//void SoundUnload(SoundData* soundData);
//void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);
//void Finalizer();