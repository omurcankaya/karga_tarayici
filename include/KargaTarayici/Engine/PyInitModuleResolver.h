#pragma once

#include "KargaTarayici/Core/Types.h"
#include "KargaTarayici/Core/IMemoryReader.h"
#include <string>
#include <vector>

namespace KargaTarayici::Engine {

struct PyInitModule4CallSite {
    Core::Address callSiteAddress{0};
    Core::Address pyInitModule4Address{0};
    std::string moduleName;
    Core::Address tableAddress{0};
};

class PyInitModuleResolver {
public:
    PyInitModuleResolver() = default;

    [[nodiscard]] std::vector<PyInitModule4CallSite> DiscoverPyInitModule4Calls(Core::Address moduleBase, 
                                                                               Core::Size moduleSize, 
                                                                               const Core::IMemoryReader& reader) const;
};

}
