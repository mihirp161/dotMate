#!/usr/bin/env bash
# Builds the WebAssembly module, runs the unit tests against it under Node,
# and copies the result into web/wasm/ (the folder that gets deployed).
#
# Needs CMake and Emscripten on PATH: with emsdk installed, run
#   source /path/to/emsdk/emsdk_env.sh
# first. (conda-forge's `emscripten` package works too.)
set -euo pipefail
cd "$(dirname "$0")/.."

emcmake cmake -S . -B build-wasm -DCMAKE_BUILD_TYPE=Release
cmake --build build-wasm -j
(cd build-wasm && ctest --output-on-failure)

mkdir -p web/wasm
cp build-wasm/dotmate.mjs build-wasm/dotmate.wasm web/wasm/
ls -l web/wasm
