#include "KargaTarayici/Core/IMemoryReader.h"
#include <windows.h>
#include <cstring>

namespace KargaTarayici::Core {

bool DirectMemoryReader::Read(Address address, void* buffer, Size size) const {
    if (address == 0 || buffer == nullptr || size == 0) {
        return false;
    }

    __try {
        auto src = reinterpret_cast<const void*>(address);
        std::memcpy(buffer, src, size);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

Buffer DirectMemoryReader::ReadBytes(Address address, Size size) const {
    Buffer buffer(size, 0);
    if (!Read(address, buffer.data(), size)) {
        return {};
    }
    return buffer;
}

bool ProcessMemoryReader::Read(Address address, void* buffer, Size size) const {
    if (processHandle_ == nullptr || processHandle_ == INVALID_HANDLE_VALUE || address == 0 || buffer == nullptr) {
        return false;
    }
    SIZE_T bytesRead = 0;
    BOOL result = ReadProcessMemory(processHandle_, reinterpret_cast<LPCVOID>(address), buffer, size, &bytesRead);
    return result != FALSE && bytesRead == size;
}

Buffer ProcessMemoryReader::ReadBytes(Address address, Size size) const {
    Buffer buffer(size, 0);
    if (!Read(address, buffer.data(), size)) {
        return {};
    }
    return buffer;
}

}
