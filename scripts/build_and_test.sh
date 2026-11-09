#!/bin/bash
set -e
cd "$(dirname "$0")/.."

echo "===================================================="
echo "          ForgeUI Build and Test Runner             "
echo "===================================================="

cmake -B build -DCMAKE_BUILD_TYPE=Release -DFORGE_BUILD_TESTS=ON -DFORGE_BUILD_WINDOWED_DEMO=ON
cmake --build build --config Release

echo ""
echo "================ Running Tests ================"
if [ -f "./build/tests/ForgeCoreTests" ]; then
    ./build/tests/ForgeCoreTests
elif [ -f "./build/tests/Release/ForgeCoreTests" ]; then
    ./build/tests/Release/ForgeCoreTests
else
    ctest --test-dir build --output-on-failure
fi
echo "================================================"
