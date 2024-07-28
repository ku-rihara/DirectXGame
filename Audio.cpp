#include "Audio.h"
#include<assert.h>

#pragma comment(lib,"xaudio2.lib")


Audio* Audio::GetInstance() {
	static Audio instance;
	return &instance;
}

void Audio::Init() {
	//XAudioエンジンのインスタンスを生成
	HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	// マスターボイスを作成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

int Audio::SoundLoadWave(const char* filename) {
	/*HRESULT result;*/
	//①ファイルオープン------------------------
	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());
	//②.wavデータ読み込み------------------------
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*) &data, sizeof(data));
	}
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}
	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	//waveファイルを閉じる
	file.close();
	//④読み込んだ音声データをreturn------------------------
	//returnする為の音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundDatas_.push_back(soundData);
	return int(soundDatas_.size())-1;
 }

void  Audio::SoundUnload(int soundId) {
	//バッファのメモリを解放
	delete[]soundDatas_[soundId].pBuffer;

	soundDatas_[soundId].pBuffer = 0;
	soundDatas_[soundId].bufferSize = 0;
	soundDatas_[soundId].wfex = {};
}

void Audio::SoundPlayWave(int soundId) {

	HRESULT result;
	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundDatas_[soundId].wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundDatas_[soundId].pBuffer;
	buf.AudioBytes = soundDatas_[soundId].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

void Audio::Finalizer() {
	xAudio2_.Reset();
}