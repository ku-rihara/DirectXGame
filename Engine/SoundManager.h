#pragma once
#include "Audio.h"
#include <vector>

class SoundManager {
private:
    Audio* audio_ = nullptr;
    std::vector<SoundData> soundDatas_;
public:
    static SoundManager* GetInstance();

    void Init();
    int SoundLoadWave(const char* filename);
    void SoundPlayWave(int soundId);
    void Finalize();

private:
    SoundManager() = default;
    ~SoundManager() = default;

   
};
