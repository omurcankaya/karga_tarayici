cmake_minimum_required(VERSION 3.25)

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

project(KargaTarayici VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(OutputDirectories)
include(CompilerFlags)
include(Dependencies)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(CORE_SOURCES
    src/Core/WinHandle.cpp
    src/Core/MemoryReader.cpp
    src/Engine/DisassemblerEngine.cpp
    src/Engine/PatternScanner.cpp
    src/Engine/ScanPipeline.cpp
    src/Engine/PyModuleScanner.cpp
    src/Strategies/InstanceRegisterStrategy.cpp
    src/Strategies/MethodCallStrategy.cpp
    src/Strategies/RegisterDataFlowStrategy.cpp
    src/Strategies/StringXrefStrategy.cpp
    src/Rules/JsonRuleParser.cpp
    src/UI/RenderEngineD3D11.cpp
    src/UI/UIContext.cpp
    src/UI/ScannerWindow.cpp
    src/UI/LogWindow.cpp
    src/UI/AssemblyInspectorWindow.cpp
    src/UI/OverlayWindow.cpp
    src/Utils/Logger.cpp
)

add_library(KargaScannerCore STATIC ${CORE_SOURCES})
target_link_libraries(KargaScannerCore PUBLIC Zydis nlohmann_json::nlohmann_json imgui::imgui d3d11.lib dxgi.lib)

add_executable(KargaScannerApp src/AppMain.cpp)
target_link_libraries(KargaScannerApp PRIVATE KargaScannerCore imgui::imgui d3d11.lib dxgi.lib)

add_library(KargaScanner SHARED src/DllMain.cpp)
target_link_libraries(KargaScanner PRIVATE KargaScannerCore imgui::imgui d3d11.lib dxgi.lib)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
