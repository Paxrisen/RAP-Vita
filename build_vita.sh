#!/bin/sh
# Build RAP-Vita (Raptor source port) for PS Vita. Needs VitaSDK incl. SDL2 (vdpm).
set -e
: "${VITASDK:?VITASDK not set}"
cmake -S . -B build-vita -DVITA_BUILD=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build-vita -j"$(nproc)"
echo "VPK: build-vita/Rap-Vita_v*.vpk"
