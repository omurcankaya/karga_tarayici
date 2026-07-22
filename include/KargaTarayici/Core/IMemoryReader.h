#pragma once

#include "KargaTarayici/Core/Types.h"
#include <windows.h>

namespace KargaTarayici::Core {

class IMemoryReader {
public:
    virtual ~IMemoryReader() = default;
    [[nodiscard]] virtual bool Read(Address address, void* buffer, Size size) const = 0;
    [[nodiscard]] virtual Buffer ReadBytes(Address address, Size size) const = 0;
};

class DirectMemoryReader : public IMemoryReader {
public:
    [[nodiscard]] bool Read(Address address, void* buffer, Size size) const override;
    [[nodiscard]] Buffer ReadBytes(Address address, Size size) const override;
};

class ProcessMemoryReader : public IMemoryReader {
private:
    HANDLE processHandle_{nullptr};

public:
    explicit ProcessMemoryReader(HANDLE processHandle) noexcept : processHandle_(processHandle) {}
    [[nodiscard]] bool Read(Address address, void* buffer, Size size) const override;
    [[nodiscard]] Buffer ReadBytes(Address address, Size size) const override;
};

}
