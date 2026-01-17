#include <chrono>
#include <cstdint>

/// <summary>
/// フレームレート管理クラス
/// </summary>
namespace KetaEngine {

class Frame {
public:
    Frame()  = default;
    ~Frame() = default;

    static void Init(); //< フレーム管理の初期化
    static void Update(); //< フレーム更新
    static float DeltaTime(); //< デルタタイムを取得
    static float DeltaTimeRate(); //< タイムスケール適用後のデルタタイムを取得
    static void FixFPS(); //< FPSを60に固定
    static void ResetDeltaTime(); //< デルタタイムをリセット

    /// <summary>
    /// 秒からフレームに変換
    /// </summary>
    static int32_t TimeToFrame(float seconds);

    /// <summary>
    /// フレームから秒に変換
    /// </summary>
    static float FrameToTime(int32_t frame);

private:
    static std::chrono::steady_clock::time_point reference_;
    static std::chrono::high_resolution_clock::time_point lastTime_;
    static float deltaTime_;
    static float deltaTimeRate_;
    static float timeScale_;

    static const float kFPS; //< フレームレート固定値

public:
    static float GetTimeScale();
    float GetTimeScaleInstance() const {return timeScale_;}

    static void SetTimeScale(float scale);
};

}; // KetaEngine
