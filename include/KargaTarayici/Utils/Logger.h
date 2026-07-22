#pragma once

#include <string>
#include <string_view>

namespace KargaTarayici::Utils {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
private:
    static bool consoleEnabled_;
    static bool fileEnabled_;
    static std::string logFilePath_;

public:
    static void Initialize(bool enableConsole = true, bool enableFile = true, std::string_view filename = "KargaScanner.log");
    static void Shutdown();

    static void Log(LogLevel level, std::string_view message);
    static void Debug(std::string_view message);
    static void Info(std::string_view message);
    static void Warning(std::string_view message);
    static void Error(std::string_view message);
};

}
