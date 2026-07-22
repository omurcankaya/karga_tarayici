# Karga Tarayıcı Modernization Architecture Plan

## Overview
This document defines the architectural specification and implementation plan for **Karga Tarayıcı** (`D:\dev\Karga_Tarayici`). The project is a modern, high-performance C++20 memory scanning and reverse engineering framework supporting both standalone desktop execution (`KargaScannerApp.exe`) and in-process DLL injection (`KargaScanner.dll`).

---

## Technical Standards & Build Specifications

### 1. External Top-Most Overlay Architecture (Zero Render Engine Dependency)
- **Overlay Window**: Rather than hooking game-specific DirectX 8 / DirectX 9 / DirectX 11 / OpenGL render loops (which risk crashes, device lost states, and anti-cheat detections), `KargaScanner.dll` creates its own **Top-Most Transparent Win32 Overlay Window** (`WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED`).
- **Modern Renderer**: The overlay window natively runs **DirectX 11** with Dear ImGui, tracking the target game window coordinates dynamically via `GetWindowRect`.
- **Target Independence**: Operates with 100% decoupling from the target application's graphics API, rendering pipeline, or DirectX version.

### 2. Dual Execution Targets
- **`KargaScannerCore.lib`**: Static core library containing the disassembler engine (Zydis), JSON schema rule parser, pattern scanner, and strategy execution pipeline.
- **`KargaScannerApp.exe`**: Standalone GUI desktop scanner using Dear ImGui + DirectX11, communicating with target processes via `IMemoryReader` (`ReadProcessMemory`).
- **`KargaScanner.dll`**: Injected DLL payload with zero external DLL dependencies (statically linked C runtime `/MT`), accessing memory directly via pointers and presenting UI via the External Top-Most Overlay.

### 3. Integrated Live Assembly Disassembler View (Interactive UI Feature)
- **Interactive Result Inspector**: Clicking any item in the Module & Address Result Tree panel automatically opens a dedicated **Live Assembly Inspector View**.
- **Zydis-Powered Inspection**: Uses the statically linked Zydis engine to disassemble and present the target function's disassembly instructions starting from the resolved address.
- **Visual Feedback**: Displays memory addresses, instruction hex bytes, disassembled assembly mnemonics, operands, and color-coded `CALL`/`JMP` instructions.

### 4. Output Directory Structure
Binaries are compiled into architecture and build-configuration isolated output folders:
```
bin/
├── x86/
│   ├── Debug/
│   │   ├── KargaScannerApp.exe
│   │   ├── KargaScanner.dll
│   │   └── KargaScannerTests.exe
│   └── Release/
└── x64/
    ├── Debug/
    └── Release/
```

### 5. Compiler & Toolchain
- **Compiler**: LLVM / Clang (`clang-cl`).
- **Standard**: C++20.
- **Warnings**: Zero warnings allowed under `/W4 /WX` or `-Wall -Wextra -Werror`.

---

## Directory Schema

```
D:\dev\Karga_Tarayici\
├── CMakeLists.txt
├── AGENTS.md
├── implementation_plan.md
├── cmake/
│   ├── Dependencies.cmake
│   ├── CompilerFlags.cmake
│   └── OutputDirectories.cmake
├── bin/
├── include/
│   └── KargaTarayici/
│       ├── Core/
│       │   ├── Types.h
│       │   ├── WinHandle.h
│       │   └── IMemoryReader.h
│       ├── Engine/
│       │   ├── DisassemblerEngine.h
│       │   ├── PatternScanner.h
│       │   └── ScanPipeline.h
│       ├── Strategies/
│       │   ├── IScanStrategy.h
│       │   ├── InstanceRegisterStrategy.h
│       │   └── MethodCallStrategy.h
│       ├── Rules/
│       │   ├── RuleModel.h
│       │   └── JsonRuleParser.h
│       └── UI/
│           ├── OverlayWindow.h
│           ├── UIContext.h
│           ├── LogWindow.h
│           ├── ScannerWindow.h
│           └── AssemblyInspectorWindow.h
├── src/
│   ├── Core/
│   │   ├── WinHandle.cpp
│   │   └── MemoryReader.cpp
│   ├── Engine/
│   │   ├── DisassemblerEngine.cpp
│   │   ├── PatternScanner.cpp
│   │   └── ScanPipeline.cpp
│   ├── Strategies/
│   │   ├── InstanceRegisterStrategy.cpp
│   │   └── MethodCallStrategy.cpp
│   ├── Rules/
│   │   └── JsonRuleParser.cpp
│   ├── UI/
│   │   ├── OverlayWindow.cpp
│   │   ├── UIContext.cpp
│   │   ├── LogWindow.cpp
│   │   ├── ScannerWindow.cpp
│   │   └── AssemblyInspectorWindow.cpp
│   ├── AppMain.cpp
│   └── DllMain.cpp
├── tests/
│   ├── CMakeLists.txt
│   ├── Core/
│   │   └── TestMemoryReader.cpp
│   ├── Engine/
│   │   └── TestDisassemblerEngine.cpp
│   └── Rules/
│       └── TestJsonRuleParser.cpp
└── config/
    └── default_rules.json
```

---

## Verification Plan

### Automated Testing
- Execute `ctest` inside build output directory to run all unit tests.
- Validate `TestDisassemblerEngine` against mock instruction byte sequences.

### Manual Verification
- Compile under both x86 and x64 configurations using Clang-CL.
- Verify that clicking any scanned item in the UI automatically populates the Assembly Inspector view with live decoded disassembler lines.
