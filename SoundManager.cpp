#include "SoundManager.h"

SoundManager* SoundManager::GetInstance() {
	static SoundManager instance;
	return &instance;
}

void SoundManager::Init() {
	if (!audio_) {
		audio_ = Audio::GetInstance();
		audio_->Init();
	}
}
int SoundManager::SoundLoadWave(const char* filename) {

	SoundData soundData = audio_->SoundLoadWave(filename);
	soundDatas_.push_back(soundData);
	return int(soundDatas_.size()) - 1;
}

void SoundManager::SoundPlayWave(int soundId) {
	if (audio_ && soundId >= 0 && soundId < soundDatas_.size()) {
		audio_->SoundPlayWave(audio_->GetXAudio2(), soundDatas_[soundId]);
	}
}

void SoundManager::Finalize() {
	audio_->Finalizer();
	if (audio_) {
		for (auto& soundData : soundDatas_) {
			audio_->SoundUnload(&soundData);
		}
		soundDatas_.clear();
		audio_ = nullptr;
	}
}