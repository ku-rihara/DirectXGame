#pragma once
//チャンクヘッダ
struct ChunkHeader {
	char id[4];//チャンク毎のID
	int32_t size;//チャンクサイズ
};
//RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk;//RIFF
	char type[4];
};
//FNTチャンク
struct FormatChunk {
	ChunkHeader chunk;//"fmt"
	WAVEFORMATEX fmt;//波形フォーマット
};

struct SoundData {
	//波形フォーマット
	WAVEFORMATEX wfex;
	//バッファの先頭アドレス
	BYTE* pBuffer;
	//バッファのサイズ
	unsigned int bufferSize;

};
