#include "Log.h"

#include <chrono>
#include <filesystem>
#include <format>
#include <memory>

// spdlog
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/spdlog.h>

namespace KetaEngine {

namespace {
std::shared_ptr<spdlog::logger> s_logger;
} // namespace

void Log::Init() {
    // logsディレクトリを用意
    std::filesystem::create_directory("logs");

    // 現在時刻 (ローカル時間) でファイル名を決定
    auto now        = std::chrono::system_clock::now();
    auto nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    std::chrono::zoned_time localTime{std::chrono::current_zone(), nowSeconds};
    std::string dateString  = std::format("{:%Y%m%d_%H%M%S}", localTime);
    std::string logFilePath = "logs/" + dateString + ".log";

    // ファイルシンク + msvcシンク 
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath, true);
    auto msvcSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();

    s_logger = std::make_shared<spdlog::logger>(
        "engine",
        spdlog::sinks_init_list{fileSink, msvcSink});
    s_logger->set_level(spdlog::level::trace);
    s_logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
    s_logger->flush_on(spdlog::level::trace); // 全レベルで即フラッシュ
    spdlog::register_logger(s_logger);
}

void Log::Info(const std::string& message) {
    if (s_logger) {
        s_logger->info(message);
    }
}

void Log::Warn(const std::string& message) {
    if (s_logger) {
        s_logger->warn(message);
    }
}

void Log::Error(const std::string& message) {
    if (s_logger) {
        s_logger->error(message);
    }
}

void Log::Finalize() {
    spdlog::shutdown();
}

} // namespace KetaEngine
