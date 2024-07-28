#pragma once
#include<wrl.h>
#include<xaudio2.h>
#include<fstream>
#include "SoundData.h"

class Audio{
private:
	//XAudio2のインスタンス
	Microsoft::WRL::ComPtr<IXAudio2>xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;
	std::vector<SoundData> soundDatas;
public:
	static Audio* GetInstance();
	void Init();
	SoundData SoundLoadWave(const char* filename);
	void SoundUnload(SoundData* soundData);
	void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);
	void Finalizer();
	IXAudio2* GetXAudio2()const { return xAudio2_.Get(); }
};

