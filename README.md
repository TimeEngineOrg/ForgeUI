# ForgeUI

> **Fast Orchestrated Responsive Graphic-layout Engine**
> A lightweight, data-oriented, immediate-mode C++20 GUI library for game engines and high-throughput tools.

---

## ⚡ Overview

ForgeUI combines the beloved ergonomics of **Immediate-Mode APIs** with the microsecond layout performance of **Linear Data-Oriented Memory** and **GPU-accelerated SDF rendering**.

* **Zero External Dependencies:** Built-in linear bump arenas and custom data structures (zero STL allocation churn in hot paths).
* **Two-Pass Flexbox Solver:** Microsecond auto-layout inspired by modern linear solvers (Clay-style).
* **GPU Signed Distance Fields (SDF):** Anti-aliased rounded rectangles, borders, and analytic Gaussian drop shadows with minimal state changes.
* **Dual-Tier Font Architecture:** Pre-baked static MSDF base atlas for crisp vector UI + fast dynamic rasterization for extended Unicode/CJK.
* **Cross-Platform & Engine-Agnostic:** Pluggable backend architecture supporting OpenGL 3.3+, Vulkan 1.2+, DirectX 11, Metal, and pure CPU scanline rasterization.

---

## 📄 License

ForgeUI is licensed under the [MIT License](LICENSE).
