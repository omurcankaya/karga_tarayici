#include "KargaTarayici/Utils/Logger.h"
#include <windows.h>
#include <fstream>
#include <iostream>
#include <chrono>
#include <format>
#include <mutex>

namespace KargaTarayici::Utils {

bool Logger::consoleEnabled_ = true;
bool Logger::fileEnabled_ = true;
std::string Logger::logFilePath_ = "KargaScanner.log";
static std::mutex g_logMutex;

void Logger::Initialize(bool enableConsole, bool enableFile, std::string_view filename) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    consoleEnabled_ = enableConsole;
    fileEnabled_ = enableFile;

    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    logFilePath_ = std::string(tempPath) + std::string(filename);

    if (consoleEnabled_) {
        if (AllocConsole()) {
            FILE* fDummy;
            freopen_s(&fDummy, "CONOUT$", "w", stdout);
            freopen_s(&fDummy, "CONERR$", "w", stderr);
        }
    }

    if (fileEnabled_) {
        std::ofstream logFile(logFilePath_, std::ios::trunc);
        if (logFile.is_open()) {
            logFile << "=== KargaScanner Logger Initialized ===\n";
        }
    }
}

void Logger::Shutdown() {
    std::lock_guard<std::mutex> lock(g_logMutex);
    if (consoleEnabled_) {
        FreeConsole();
    }
}

void Logger::Log(LogLevel level, std::string_view message) {
    std::lock_guard<std::mutex> lock(g_logMutex);

    const char* levelStr = "[INFO]";
    switch (level) {
    case LogLevel::Debug: levelStr = "[DEBUG]"; break;
    case LogLevel::Info: levelStr = "[INFO]"; break;
    case LogLevel::Warning: levelStr = "[WARN]"; break;
    case LogLevel::Error: levelStr = "[ERROR]"; break;
    }

    auto now = std::chrono::system_clock::now();
    std::string formattedLog = std::format("{} {} {}\n", levelStr, now, message);

    if (consoleEnabled_) {
        std::cout << formattedLog;
    }

    if (fileEnabled_) {
        std::ofstream logFile(logFilePath_, std::ios::app);
        if (logFile.is_open()) {
            logFile << formattedLog;
        }
    }
}

void Logger::Debug(std::string_view message) { Log(LogLevel::Debug, message); }
void Logger::Info(std::string_view message) { Log(LogLevel::Info, message); }
void Logger::Warning(std::string_view message) { Log(LogLevel::Warning, message); }
void Logger::Error(std::string_view message) { Log(LogLevel::Error, message); }

}
