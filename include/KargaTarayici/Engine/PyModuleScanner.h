#pragma once

#include "KargaTarayici/Core/Types.h"
#include "KargaTarayici/Core/IMemoryReader.h"
#include "KargaTarayici/Strategies/RegisterDataFlowStrategy.h"
#include <string>
#include <vector>

namespace KargaTarayici::Engine {

struct PyMethodInfo {
    std::string methodName;
    Core::Address wrapperAddress{0};
    Core::Address instanceAddress{0};
    Core::Address cppMethodAddress{0};
};

struct PyModuleInfo {
    std::string moduleName;
    Core::Address tableAddress{0};
    std::vector<PyMethodInfo> methods{};
};

class PyModuleScanner {
private:
    Strategies::RegisterDataFlowStrategy dataFlowStrategy_{};

public:
    PyModuleScanner() = default;

    [[nodiscard]] std::vector<PyModuleInfo> ScanPythonModules(Core::Address moduleBase, 
                                                             Core::Size moduleSize, 
                                                             const Core::IMemoryReader& reader) const;

    [[nodiscard]] std::vector<PyMethodInfo> ParsePyMethodTable(Core::Address tableAddress, 
                                                              const Core::IMemoryReader& reader) const;
};

}
