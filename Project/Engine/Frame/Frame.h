#include <chrono>

/// <summary>
/// フレームクラス
/// </summary>
class Frame {
private:
    /// ========================================================
    /// 静的メンバ変数
    /// ========================================================
    static std::chrono::steady_clock::time_point reference_;
    static std::chrono::high_resolution_clock::time_point lastTime_;
    static float deltaTime_;
    static float deltaTimeRate_;
    static float timeScale_;
public:
    /// ========================================================
    /// 静的メンバ関数
    /// ========================================================

    static void Init();
    static void Update();
    static float DeltaTime();
    static float DeltaTimeRate();
    static void FixFPS();   
    static void SetTimeScale(float scale); 
    static float GetTimeScale();          

};
