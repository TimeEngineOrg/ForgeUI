#!/bin/bash
set -e

echo "===================================================="
echo "     ForgeUI vs Dear ImGui Benchmark Runner"
echo "===================================================="

cmake -B build -DCMAKE_BUILD_TYPE=Release -DFORGE_BUILD_BENCHMARK=ON
cmake --build build --target ForgeBenchmark --config Release

./build/ForgeBenchmark
