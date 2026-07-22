# AGENTS.md - Developer & AI Agent Guidelines for Karga Tarayıcı

This document defines the strict development standards, architectural rules, code formatting guidelines, and workflow policies for all developers and AI coding agents working on **Karga Tarayıcı**.

---

## 1. Project Overview & Architecture

**Karga Tarayıcı** (`D:\dev\Karga_Tarayici`) is a modern, high-performance C++20 memory scanning, disassembling, and reverse engineering toolkit designed for dual-target execution:
1. **`KargaScannerApp.exe`**: Standalone GUI desktop memory scanner.
2. **`KargaScanner.dll`**: Injected DLL payload with zero external DLL dependencies.

### Key Architectural Principles
- **Top-Most External Overlay**: UI rendering for the injected DLL MUST use a dedicated Win32 transparent top-most overlay window (`WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED`) running DirectX 11 with Dear ImGui. **NEVER** hook in-game DirectX 8 / DirectX 9 / DirectX 11 render loops directly.
- **Disassembler Engine**: Heuristic assembly instruction analysis powered by **Zydis** (x86/x64 decoder).
- **Rule Storage**: JSON Schema rules (`nlohmann_json`) replacing legacy `.ztxt` call-index text files.
- **Memory Abstraction**: All memory access is abstracted via `IMemoryReader` (`DirectMemoryReader` for DLL, `ProcessMemoryReader` for EXE).

---

## 2. Strict Code Comment Policy

> [!IMPORTANT]
> The default policy is **NO COMMENTS**. Code must be self-documenting through clean structure, precise naming, and type safety.

### Allowed Comments
A comment is ONLY justified when it carries information that is *not recoverable from the code*, such as:
- Non-obvious external constraints: Windows/NT kernel behavior, undocumented OS structure layout, or hardware quirks.
- References saving research time (e.g., MSDN structure links or known OS bug workarounds).

### Strictly Forbidden Comments
- **Restatements of code**: `// increment the counter`, `// loop over modules`, `// call handler`.
- **Section banners / dividers**: `// --- Setup ---`, `// Step 3: Cleanup`, `// ####################`.
- **Conversational narration**: `// changed to fix X`, `// this is safe because ...`.
- **Docstring headers that restate function signature in prose**.

---

## 3. C++ Coding Standards & Best Practices

- **Language Standard**: Modern C++20 (`std::span`, `std::string_view`, `std::format`, concepts, RAII).
- **Compiler**: LLVM / Clang (`clang-cl` under Windows MSVC). Zero warnings permitted under `/W4 /WX` or `-Wall -Wextra -Werror`.
- **No C-Style Casts**: STRICTLY FORBIDDEN `(Type)var`. Use `static_cast`, `reinterpret_cast`, or `std::bit_cast`.
- **No `#define` Macros**: Use `constexpr`, `consteval`, or `inline` constants and functions.
- **Pointer Width & Architecture Safety**: The codebase must compile cleanly for both **x86** (32-bit) and **x64** (64-bit).
  - Never use `DWORD` for pointer arithmetic or memory addresses.
  - Always use architecture-aware types: `uintptr_t`, `size_t`, `ptrdiff_t`, or `DWORD_PTR`.
- **Resource Management (RAII)**: Never leave raw `HANDLE`, `HKEY`, or `HMODULE` types unmanaged. Always use `WinHandle` RAII wrappers.

---

## 4. Naming Conventions

- **Classes / Structs**: `PascalCase` (e.g., `DisassemblerEngine`, `RuleModel`).
- **Interfaces**: Prefix with `I` (e.g., `IMemoryReader`, `IScanStrategy`).
- **Member Variables**: Trailing underscore `_` (e.g., `processHandle_`, `vtableAddress_`).
- **Functions & Methods**: `PascalCase` (e.g., `ExecuteScan()`, `FindInstanceAddress()`).
- **Local Variables**: `camelCase` (e.g., `funcAddress`, `currentInstruction`).
- **Enums & Enum Members**: `PascalCase` for scoped enum classes (e.g., `enum class ScanDirection { Up, Down };`).

---

## 5. Build System & Binary Output Layout

All builds are orchestrated via modern CMake (3.25+). Binaries MUST be placed strictly in architecture and build-type subdirectories under `bin/`:

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

---

## 6. Test-Driven Development (TDD) Workflow

1. All new core features, instruction heuristics, and JSON rule parsers MUST be developed with corresponding unit tests using **GoogleTest** under `tests/`.
2. Run `ctest --output-on-failure` to verify test suite completion before committing changes.
