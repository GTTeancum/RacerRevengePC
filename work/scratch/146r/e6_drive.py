from edlib import edit
edit('tests/native/terrain_interactive_drive50du.cpp', [
('''        effectiveTrack==rr::platform::HostTrack50IM::MA||effectiveTrack==rr::platform::HostTrack50IM::SA||''',
 '''        effectiveTrack==rr::platform::HostTrack50IM::MA||effectiveTrack==rr::platform::HostTrack50IM::SA||
        // Common route (146r): a registry course reached by the consumed Course launch.
        effectiveTrack==rr::platform::HostTrack50IM::Course||'''),
])
