#pragma once

#include <windows.h>

namespace KargaTarayici::Core {

class WinHandle {
private:
    HANDLE handle_{INVALID_HANDLE_VALUE};

public:
    WinHandle() noexcept = default;
    explicit WinHandle(HANDLE handle) noexcept : handle_(handle) {}
    ~WinHandle() noexcept { Close(); }

    WinHandle(const WinHandle&) = delete;
    WinHandle& operator=(const WinHandle&) = delete;

    WinHandle(WinHandle&& other) noexcept : handle_(other.handle_) {
        other.handle_ = INVALID_HANDLE_VALUE;
    }

    WinHandle& operator=(WinHandle&& other) noexcept {
        if (this != &other) {
            Close();
            handle_ = other.handle_;
            other.handle_ = INVALID_HANDLE_VALUE;
        }
        return *this;
    }

    [[nodiscard]] bool IsValid() const noexcept {
        return handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE;
    }

    [[nodiscard]] HANDLE Get() const noexcept { return handle_; }

    void Close() noexcept {
        if (IsValid()) {
            CloseHandle(handle_);
            handle_ = INVALID_HANDLE_VALUE;
        }
    }
};

}
