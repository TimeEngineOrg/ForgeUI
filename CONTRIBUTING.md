# Contributing to ForgeUI

Thank you for your interest in contributing to **ForgeUI**! We welcome contributions from developers across real-time graphics, creative tooling, CAD/CAM, embedded systems, audio software, and scientific computing.

This guide outlines our architectural invariants, coding standards, and workflow for submitting issues and pull requests.

---

## 🏛️ Core Architectural Invariants

ForgeUI is engineered for ultra-demanding interactive and real-time host environments. Every contribution must respect these invariants:

1. **Zero Heap Allocations on Hot Paths**:
   - Never call `malloc`, `free`, `new`, `delete`, or allocate with STL containers (`std::vector::push_back`, `std::string`, `std::map`) during per-frame layout, widget evaluation, or draw list generation.
   - All transient per-frame allocations must use the frame's `ForgeArena` bump allocator.
2. **Two-Pass Flexbox Hierarchy**:
   - Layout is computed in two distinct passes: **Pass 1 (Measure)** calculates intrinsic child bounds; **Pass 2 (Arrange)** resolves flex ratios, alignments, and absolute coordinates.
   - Avoid interleaving immediate draw calls into the measurement pass.
3. **Data-Oriented & Cache-Friendly**:
   - Layout state is stored in Structure-of-Arrays (SoA) to guarantee sequential memory access and vectorized computations.
4. **Backend State Isolation**:
   - Backend implementations (OpenGL, Vulkan, Metal, DirectX 11, Software) must back up and restore host application graphics state (VAOs, VBOs, active shaders, blend equations, scissor tests) cleanly without side effects.
5. **No Hidden Background Threads**:
   - Parallel tasks run strictly on user-provided worker pools via `ForgeJobSystem` or user job dispatchers.

---

## 🛠️ Development Setup & Prerequisites

- **CMake**: Version 3.20 or newer.
- **C++ Compiler**: Fully compliant C++20 compiler:
  - MSVC 2019 / 2022 (v142 or v143 toolset)
  - GCC 11 or newer
  - Clang 13 or newer
  - AppleClang 13 or newer
- **Platform Dependencies**:
  - **Windows**: Windows SDK (GDI and OpenGL 3.3+ drivers).
  - **Linux**: OpenGL development headers (`libgl1-mesa-dev`, `libxrandr-dev`, `libxinerama-dev`, `libxcursor-dev`, `libxi-dev`).
  - **macOS**: Cocoa / Metal frameworks (for macOS backend support).

---

## 🚀 Building & Testing

All automation and runner scripts are located in the `scripts/` directory:

### Windows (Command Prompt / PowerShell)
```cmd
:: Build library, demo, and execute the full test suite
scripts\build_and_test.bat

:: Launch the interactive hardware OpenGL demo
scripts\run_windowed_demo.bat

:: Run the head-to-head performance benchmark
scripts\run_benchmark.bat
```

### Linux & macOS (Bash)
```bash
# Build and run tests
./scripts/build_and_test.sh

# Launch the windowed demo
./scripts/run_windowed_demo.sh

# Run benchmarks
./scripts/run_benchmark.sh
```

### Direct CMake Workflow
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DFORGE_BUILD_TESTS=ON -DFORGE_BUILD_WINDOWED_DEMO=ON
cmake --build build --config Release --parallel
ctest --test-dir build --output-on-failure
```

---

## 📝 Coding Standards

- **Language Standard**: C++20 (`std::span`, concepts, `constexpr`, designated initializers where supported).
- **Naming Conventions**:
  - Types, structs, classes, and namespaces: `PascalCase` with `Forge` prefix for public API symbols (e.g., `ForgeContext`, `ForgeVec2`, `ForgeDrawList`).
  - Functions & Methods: `PascalCase` (e.g., `BeginFrame`, `LayoutFlexRow`, `AddRectFilled`).
  - Local variables & parameters: `camelCase` (e.g., `itemWidth`, `layoutNode`).
  - Member variables: `camelCase` or `m_camelCase`.
- **Memory Safety & Invariants**:
  - Assert preconditions and bounds in debug builds (`FORGE_ASSERT`).
  - Do not introduce circular header dependencies. Public headers live under `include/ForgeUI/` and private implementation details under `src/`.
- **Formatting**:
  - Follow `.editorconfig` (4 spaces, LF line endings, UTF-8).
  - Clean, concise code without redundant abstraction layers.

---

## 🧪 Testing Requirements

Every PR that adds or modifies functionality must include unit tests in `tests/`:
- Test files: `tests/test_arena.cpp`, `tests/test_layout.cpp`, `tests/test_widgets.cpp`, `tests/test_backends.cpp`, etc.
- All 32+ existing unit tests must pass without regressions.
- If introducing new widgets or layout modes, verify both clipped and non-clipped execution paths.

---

## 🔄 Pull Request Workflow

1. **Fork and branch**: Create a descriptive branch from `main`:
   ```bash
   git checkout -b feature/vulkan-sparse-binding
   # or
   git checkout -b fix/layout-flex-shrink-overflow
   ```
2. **Verify locally**: Run `scripts/build_and_test` and ensure 0 failures and 0 memory leaks.
3. **Commit cleanly**: Use clear, concise commit messages (e.g., `fix(layout): prevent division by zero in flex shrink calculation`).
4. **Submit PR**: Open a pull request against `main` on [TimeEngineOrg/ForgeUI](https://github.com/TimeEngineOrg/ForgeUI). Fill in the PR template detailing the change, benchmark or memory impact, and tests executed.

For architectural questions or collaboration, join our [GitHub Discussions](https://github.com/TimeEngineOrg/ForgeUI/discussions) or reach out to maintainer Shankharaj Datta (`shankharajdatta2004@gmail.com`). We review PRs actively and appreciate your dedication to high-performance systems engineering!
