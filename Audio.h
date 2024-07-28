#pragma once
#include<wrl.h>
#include<xaudio2.h>
#include<fstream>
#include <vector>
#include "SoundData.h"

class Audio{
private:
	//XAudio2のインスタンス
	Microsoft::WRL::ComPtr<IXAudio2>xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;
	std::vector<SoundData> soundDatas_;
public:
	static Audio* GetInstance();
	void Init();
	int SoundLoadWave(const char* filename);
	void SoundUnload(int soundId);
	void SoundPlayWave(int soundId);
	void Finalizer();

	std::vector<SoundData>GetSoundDatas() { return soundDatas_; }
	IXAudio2* GetXAudio2()const { return xAudio2_.Get(); }
};

