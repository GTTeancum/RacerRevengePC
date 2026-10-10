#!/usr/bin/env bash
set -euo pipefail
repo="$(cd "${1:?usage: build_original_roster_tools.sh REPOSITORY OUTPUT}" && pwd)"
out="${2:?output directory required}"
mkdir -p "$out"
here="$(cd "$(dirname "$0")" && pwd)"
compiler="${CXX:-g++}"
flags=(-std=c++17 -O2 -ffp-contract=off -I"$repo/src" -I"$repo/third_party")
"$compiler" "${flags[@]}" "$here/import_original_pods.cpp" \
  "$repo/src/assets/import_decode48c.cpp" "$repo/src/assets/import_scene48d.cpp" \
  "$repo/src/assets/import_texture48e.cpp" "$repo/src/assets/scene_asset48d.cpp" \
  "$repo/src/assets/native_asset48b.cpp" "$repo/src/assets/pod_physics49n.cpp" \
  -lz -o "$out/import_original_pods"
"$compiler" "${flags[@]}" "$here/validate_roster_visuals.cpp" \
  "$repo/src/assets/scene_asset48d.cpp" "$repo/src/assets/native_asset48b.cpp" \
  -o "$out/validate_roster_visuals"
