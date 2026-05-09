#pragma once
#include <string>

namespace KetaEngine {

/// <summary>
/// ログシステム。spdlogによるファイル出力
/// </summary>
class Log {
public:
    Log()  = delete;
    ~Log() = delete;

    static void Init();

    static void Info(const std::string& message);
    static void Warn(const std::string& message);
    static void Error(const std::string& message);
};

} // namespace KetaEngine
