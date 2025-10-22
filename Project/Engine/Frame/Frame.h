#include <chrono>

/// <summary>
/// フレームレート管理クラス
/// </summary>
class Frame {
private:
    static std::chrono::steady_clock::time_point reference_;
    static std::chrono::high_resolution_clock::time_point lastTime_;
    static float deltaTime_;
    static float deltaTimeRate_;
    static float timeScale_;

public:
    Frame()  = default;
    ~Frame() = default;

    static void Init(); //< フレーム管理の初期化
    static void Update(); //< フレーム更新
    static float DeltaTime(); //< デルタタイムを取得
    static float DeltaTimeRate(); //< タイムスケール適用後のデルタタイムを取得
    static void FixFPS(); //< FPSを60に固定

    /// <summary>
    /// タイムスケールを設定
    /// </summary>
    /// <param name="scale">スケール値</param>
    static void SetTimeScale(const float& scale);

    static float GetTimeScale(); //< タイムスケールを取得
    static void ResetDeltaTime(); //< デルタタイムをリセット
};