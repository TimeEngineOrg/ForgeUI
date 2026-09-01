# ForgeUI

> **Fast Orchestrated Responsive Graphic-layout Engine**
> A lightweight, data-oriented, immediate-mode C++20 GUI library for game engines and high-throughput tools.

---

## ⚡ Overview

ForgeUI combines the ergonomics of **Immediate-Mode APIs** with the microsecond layout performance of **Linear Data-Oriented Memory (SoA)** and **GPU-accelerated SDF rendering**.

* **Zero External Dependencies:** Built-in linear bump arenas and custom data structures (zero STL allocation churn in hot paths).
* **Two-Pass Flexbox Solver:** Microsecond auto-layout inspired by modern linear solvers (Measure + Arrange passes in contiguous memory).
* **GPU Signed Distance Fields (SDF):** Anti-aliased rounded rectangles, borders, and analytic Gaussian drop shadows with minimal state changes.
* **Dual-Tier Font Architecture:** Pre-baked static MSDF base atlas for crisp vector UI + fast dynamic rasterization for extended Unicode/CJK.
* **Multithreaded Job System:** Work-stealing `ParallelFor` and parallel prefix sums for high-throughput vertex and index batching.
* **Cross-Platform & Engine-Agnostic:** Pluggable backend architecture supporting OpenGL 3.3+, Vulkan 1.2+, DirectX 11, Metal, and pure CPU scanline rasterization.

---

## 📊 Benchmarks & Architecture Comparison

### Architectural Comparison

| Feature | Dear ImGui | ForgeUI |
| :--- | :--- | :--- |
| Architecture | Object-oriented / AoS | Data-oriented / SoA |
| Layout model | Cursor based | Flexbox (measure + arrange) |
| Memory model | Heap reallocations | Per-frame bump arena (zero heap) |
| Threading | Single-threaded | Work-stealing job system |
| Memory Allocation | User-defined callbacks (`malloc`/`free`) | Contiguous bump arena (custom buffer support) |
| Core Storage | Custom `ImVector` wrappers | Zero-STL cache-aligned SoA |

---

### Performance Benchmark (50,000 Elements / Frame)

*Workload: 50,000 interactive button elements per frame measured over 30 frames.*

| Metric | **Dear ImGui (v1.91.8)** | **ForgeUI** |
| :--- | :--- | :--- |
| **Frame Time** | 1.82 ms | **3.33 ms** |
| **Element Throughput** | 27.36 M elem/s | **15.01 M elem/s** |
| **Layout Complexity** | 1-Pass Cursor Advancement | **2-Pass Flexbox Hierarchy (Measure + Arrange)** |
| **Hot-Path Heap Allocations** | Dynamic (`ImVector` growth) | **0 bytes (Contiguous Bump Arena)** |
| **Vertex & Index Generation** | Single-threaded | **Scalable via `ForgeJobSystem`** |

---

## 🚀 Running the Benchmarks

You can run the head-to-head comparison on your own machine. ImGui is fetched on-demand during CMake configuration and is **never committed** to the repository.

### Windows (Batch)
```cmd
run_benchmark.bat
```

### Linux / macOS (Bash)
```bash
chmod +x run_benchmark.sh
./run_benchmark.sh
```

### Manual CMake
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DFORGE_BUILD_BENCHMARK=ON
cmake --build build --target ForgeBenchmark --config Release
./build/ForgeBenchmark
```

---

## 🛠️ Building & Testing

### Building Core & Tests
```cmd
build_and_test.bat
```

### Running the Test Suite (28 Tests)
```bash
./build/tests/ForgeCoreTests
```

---

## 📄 License

ForgeUI is licensed under the [MIT License](LICENSE).
