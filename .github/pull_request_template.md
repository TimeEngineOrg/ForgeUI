## Summary of Changes
Provide a clear and concise summary of the changes proposed in this pull request.

## Motivation & Context
Why is this change required? What problem or issue does it solve?
Fixes #(issue_number)

## Type of Change
- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Performance optimization (improves latency, throughput, or memory footprint)
- [ ] Documentation update

## Systems & Architectural Invariants Audit
- [ ] **Zero Heap Allocations on Hot Paths**: Confirmed that no dynamic memory allocations (`malloc`, `new`, `std::vector::push_back`, etc.) occur per-frame.
- [ ] **Arena Memory**: Transient memory correctly utilizes the frame's `ForgeArena`.
- [ ] **Layout Invariant**: Respects 2-pass flexbox hierarchy (Pass 1 Measure, Pass 2 Arrange).
- [ ] **State Isolation**: Backend state (OpenGL, Vulkan, Metal, DX11) is saved and restored without polluting the host context.

## Testing & Verification
- [ ] Existing unit tests pass (`scripts/build_and_test.bat` / `./scripts/build_and_test.sh`).
- [ ] New unit tests added covering edge cases (e.g. clipping, empty containers, extreme bounds).
- [ ] Tested on target backends / operating systems:
  - [ ] Windows (MSVC)
  - [ ] Linux (GCC / Clang)
  - [ ] macOS (AppleClang)

## Screenshots or Benchmark Diffs (if applicable)
Add before/after screenshots, frame latency numbers, or benchmark outputs.
