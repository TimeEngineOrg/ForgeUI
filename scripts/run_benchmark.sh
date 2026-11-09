#!/bin/bash
set -e
cd "$(dirname "$0")/.."

echo "===================================================="
echo "     ForgeUI vs Dear ImGui Benchmark Runner"
echo "===================================================="

cmake -B build -DCMAKE_BUILD_TYPE=Release -DFORGE_BUILD_BENCHMARK=ON
cmake --build build --target ForgeBenchmark --config Release

if [ -f "./build/examples/ForgeBenchmark" ]; then
    ./build/examples/ForgeBenchmark
elif [ -f "./build/examples/Release/ForgeBenchmark" ]; then
    ./build/examples/Release/ForgeBenchmark
elif [ -f "./build/ForgeBenchmark" ]; then
    ./build/ForgeBenchmark
elif [ -f "./build/Release/ForgeBenchmark" ]; then
    ./build/Release/ForgeBenchmark
fi
