#!/bin/bash
set -e
cd "$(dirname "$0")/.."

echo "===================================================="
echo "         ForgeUI Windowed Demo Launcher             "
echo "===================================================="

if [ "$1" == "--build" ] || [ "$1" == "-b" ] || ([ ! -f "./build/examples/ForgeWindowedDemo" ] && [ ! -f "./build/ForgeWindowedDemo" ]); then
    echo "[INFO] Configuring & building ForgeWindowedDemo..."
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DFORGE_BUILD_WINDOWED_DEMO=ON
    cmake --build build --target ForgeWindowedDemo --config Release
fi

if [ -f "./build/examples/ForgeWindowedDemo" ]; then
    echo "[INFO] Launching ForgeWindowedDemo..."
    ./build/examples/ForgeWindowedDemo "$@"
elif [ -f "./build/examples/Release/ForgeWindowedDemo" ]; then
    echo "[INFO] Launching ForgeWindowedDemo..."
    ./build/examples/Release/ForgeWindowedDemo "$@"
elif [ -f "./build/ForgeWindowedDemo" ]; then
    echo "[INFO] Launching ForgeWindowedDemo..."
    ./build/ForgeWindowedDemo "$@"
elif [ -f "./build/Release/ForgeWindowedDemo" ]; then
    echo "[INFO] Launching ForgeWindowedDemo..."
    ./build/Release/ForgeWindowedDemo "$@"
else
    echo "[ERROR] Could not find ForgeWindowedDemo executable."
    exit 1
fi
