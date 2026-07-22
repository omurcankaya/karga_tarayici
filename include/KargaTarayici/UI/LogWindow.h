#pragma once

#include "KargaTarayici/Core/Types.h"
#include <string>
#include <vector>

namespace KargaTarayici::UI {

enum class LogLevel {
    Info,
    Warning,
    Error,
    Instance,
    MethodCall
};

struct LogEntry {
    std::string timestamp;
    std::string message;
    LogLevel level{LogLevel::Info};
};

class LogWindow {
private:
    std::vector<LogEntry> logs_;

public:
    LogWindow() = default;

    void AddLog(LogLevel level, std::string_view message);
    void Render();
};

}
