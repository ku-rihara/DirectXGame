#include "Audio.h"

using namespace KetaEngine;
#include<fstream>
#include<assert.h>
#define ATTENUATION_TIME_MS 50
#define ATTENUATION_FACTOR 0.01f // 音量を0.5%にする
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

int Audio::LoadWave(const std::string& filename) {
    std::string fullPath = filePath_ + filename;
    // ファイルがすでに読み込まれている場合
    auto it = soundIndexMap_.find(fullPath);

    if (it != soundIndexMap_.end()) {
        return it->second; // 既存のインデックスを返す
    }

    // ファイルオープン------------------------
    std::ifstream file;
    file.open(fullPath, std::ios_base::binary);
    assert(file.is_open());

    // RIFFヘッダーの読み込み
    RiffHeader riff;
    file.read((char*)&riff, sizeof(riff));
    if (strncmp(riff.chunk.id, "RIFF", 4) != 0 || strncmp(riff.type, "WAVE", 4) != 0) {
        assert(0);
    }

    // Formatチャンクの読み込み
    FormatChunk format = {};
    file.read((char*)&format, sizeof(ChunkHeader));
    assert(strncmp(format.chunk.id, "fmt ", 4) == 0);
    assert(format.chunk.size <= sizeof(format.fmt));
    file.read((char*)&format.fmt, format.chunk.size);

    // Dataチャンクの読み込み (汎用チャンクスキップ対応)
    ChunkHeader chunk;
    char* pBuffer = nullptr;
    size_t dataSize = 0;

    while (file.read((char*)&chunk, sizeof(chunk))) {
        if (strncmp(chunk.id, "data", 4) == 0) {
            // 波形データの読み込み
            pBuffer = new char[chunk.size];
            file.read(pBuffer, chunk.size);
            dataSize = chunk.size;
            break;
        } else {
            // その他チャンクをスキップ
            file.seekg(chunk.size, std::ios_base::cur);
        }
    }
    assert(pBuffer != nullptr && dataSize > 0);
    file.close();

    // 読み込んだ音声データを保存
    SoundData soundData = {};
    soundData.wfex = format.fmt;
    soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
    soundData.bufferSize = int(dataSize);

    // soundDatas_に追加
    soundDatas_.push_back(soundData);
    int index = int(soundDatas_.size()) - 1;

    // ファイル名とインデックスをマップに保存
    soundIndexMap_[fullPath] = index;

    return index;
}


void Audio::PlayWave(const int& soundId, float volume) {
    if (soundId < 0 || soundId >= soundDatas_.size()) {
        return;
    }

    const SoundData& soundData = soundDatas_[soundId];

    HRESULT result;
    IXAudio2SourceVoice* pSourceVoice = nullptr;

    result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
    assert(SUCCEEDED(result));

    // **ボリューム調整**
    float adjustedVolume = volume; // デフォルトはそのまま
    auto now = std::chrono::steady_clock::now();

    if (lastPlayTimes_.find(soundId) != lastPlayTimes_.end()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPlayTimes_[soundId]).count();
        if (elapsed < ATTENUATION_TIME_MS) {
            // **直前に同じ音が鳴った場合、ボリュームを下げる**
            float factor = 1.0f - (1.0f - ATTENUATION_FACTOR) * (1.0f - float(elapsed) / ATTENUATION_TIME_MS);
            adjustedVolume *= factor; // 重なり具合に応じてボリュームを調整
        }
    }
    lastPlayTimes_[soundId] = now; // 最新の再生時間を記録

    result = pSourceVoice->SetVolume(adjustedVolume);
    assert(SUCCEEDED(result));

    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.pBuffer;
    buf.AudioBytes = soundData.bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    result = pSourceVoice->SubmitSourceBuffer(&buf);
    assert(SUCCEEDED(result));

    result = pSourceVoice->Start();
    assert(SUCCEEDED(result));
}

void Audio::Unload(const int& soundId) {
    if (soundId >= 0 && soundId < soundDatas_.size()) {
        SoundData& soundData = soundDatas_[soundId];
        delete[] soundData.pBuffer;
        soundData.pBuffer = nullptr;
        soundData.bufferSize = 0;
        soundData.wfex = {};
    }
}

void Audio::Finalize() {
    for (auto& soundData : soundDatas_) {
        delete[] soundData.pBuffer;
        soundData.pBuffer = nullptr;
    }
    soundDatas_.clear();
    xAudio2_.Reset();
}
//
//Audio* Audio::GetInstance() {
//	static Audio instance;
//	return &instance;
//}
//
//void Audio::Init() {
//	//XAudioエンジンのインスタンスを生成
//	HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
//	// マスターボイスを作成
//	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
//}
//
//SoundData Audio::SoundLoadWave(const char* filename) {
//	/*HRESULT result;*/
//	//①ファイルオープン------------------------
//	//ファイル入力ストリームのインスタンス
//	std::ifstream file;
//	//.wavファイルをバイナリモードで開く
//	file.open(filename, std::ios_base::binary);
//	//ファイルオープン失敗を検出する
//	assert(file.is_open());
//	//②.wavデータ読み込み------------------------
//	//RIFFヘッダーの読み込み
//	RiffHeader riff;
//	file.read((char*)&riff, sizeof(riff));
//	//ファイルがRIFFかチェック
//	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
//		assert(0);
//	}
//	//タイプがWAVEかチェック
//	if (strncmp(riff.type, "WAVE", 4) != 0) {
//		assert(0);
//	}
//	//Formatチャンクの読み込み
//	FormatChunk format = {};
//	//チャンクヘッダーの確認
//	file.read((char*)&format, sizeof(ChunkHeader));
//	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
//		assert(0);
//	}
//	//チャンク本体の読み込み
//	assert(format.chunk.size <= sizeof(format.fmt));
//	file.read((char*)&format.fmt, format.chunk.size);
//	//Dataチャンクの読み込み
//	ChunkHeader data;
//	file.read((char*)&data, sizeof(data));
//	//JUNKチャンクを検出した場合
//	if (strncmp(data.id, "JUNK", 4) == 0) {
//		//読み取り位置をJUNKチャンクの終わりまで進める
//		file.seekg(data.size, std::ios_base::cur);
//		//再読み込み
//		file.read((char*) &data, sizeof(data));
//	}
//	if (strncmp(data.id, "data", 4) != 0) {
//		assert(0);
//	}
//	//Dataチャンクのデータ部(波形データ)の読み込み
//	char* pBuffer = new char[data.size];
//	file.read(pBuffer, data.size);
//	//waveファイルを閉じる
//	file.close();
//	//④読み込んだ音声データをreturn------------------------
//	//returnする為の音声データ
//	SoundData soundData = {};
//
//	soundData.wfex = format.fmt;
//	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
//	soundData.bufferSize = data.size;
//	return soundData;
// }
//
//void  Audio::SoundUnload(SoundData* soundData) {
//	//バッファのメモリを解放
//	delete[]soundData->pBuffer;
//
//	soundData->pBuffer = 0;
//	soundData->bufferSize = 0;
//	soundData->wfex = {};
//}
//
//void Audio::SoundPlayWave(IXAudio2* xAudio2,const SoundData&soundData) {
//
//	HRESULT result;
//	//波形フォーマットを元にSourceVoiceの生成
//	IXAudio2SourceVoice* pSourceVoice = nullptr;
//	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
//	assert(SUCCEEDED(result));
//
//	//再生する波形データの設定
//	XAUDIO2_BUFFER buf{};
//	buf.pAudioData = soundData.pBuffer;
//	buf.AudioBytes = soundData.bufferSize;
//	buf.Flags = XAUDIO2_END_OF_STREAM;
//
//	//波形データの再生
//	result = pSourceVoice->SubmitSourceBuffer(&buf);
//	result = pSourceVoice->Start();
//}
//
//void Audio::Finalizer() {	
//	xAudio2_.Reset();
//}
