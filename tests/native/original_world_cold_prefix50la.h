#pragma once
// Common rewrite (area C): retired. This header held the RA-only native 1A9EE0 world prefix
// (ra_pod_scene50LA / ra_cold_aggregate50LA inline members: 270CDC physics index, 271230
// collision objects, 27BB40 groups, the 3D0 trigger list, water 400). The one track load is
// common_track50CR::Load (common_track_load50cr.cpp) for every course; nothing here is reached.
// Kept as an empty header only because the frozen W0 header terrain_interactive_dependencies50du.h
// still includes it (w0-requests\round3-C.txt asks to drop that include; then delete this file).
