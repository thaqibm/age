#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p web/dist
"${EMXX:-em++}" web/game.cpp -std=c++20 -O3 -flto --no-entry \
  -sMODULARIZE=1 -sEXPORT_ES6=1 -sENVIRONMENT=web,node -sFILESYSTEM=0 \
  -sINITIAL_MEMORY=16777216 -sALLOW_MEMORY_GROWTH=0 \
  -sEXPORTED_RUNTIME_METHODS=HEAPF32 \
  -o web/dist/age.js
cp web/index.html web/style.css web/app.js web/renderer.js web/dist/
