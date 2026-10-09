#!/usr/bin/env python3
"""49B authentic compact world metadata, measured requested heap and input regressions.

Uses the existing serial native harness without executing any collision suites.
Historical import-world tests are unchanged; their absent standalone SCB/JSON
references are not silently replaced. This new test directly checks all retained
RRWORLD1 fixtures, embedded original production decoding and byte identities.
"""
import sys
sys.dont_write_bytecode = True
import test_native_spring48k as runner

runner.__doc__ = __doc__
runner.SUITE_NAME = "world-index"
FIXTURE_BASE = "evidence/nativePc48K-build-20261003-174328-460"
AUTHENTIC = {
    "ga": (749005, "e5694b2826524f0ea41b7e841975c87ac47289d1e11fda56e6c47483ee09efbf"),
    "gb": (1010359, "8b95e8003ce99fcf396f4481007459258720ac68c734108b901ab0ab09406029"),
    "gc": (940917, "23ead718a49238f3d889491f3b15680f25151d9404031ae108628d17b971d3dc"),
    "ma": (1039858, "49cddec08077f82f9c516786911b627dab7dfc27a984626704f0e2415a20cecd"),
    "mb": (1425025, "05731aa72728f9f43b7c379b7e2dff468e10af97cebd4e618b027b52f440e714"),
    "ra": (1342915, "3f3811b6304fa56d4edaccdc0347fe6693a3179f0c0ad98fcc0b3b8571b5f49a"),
    "rc": (1224624, "51ef4b7630711ec82acda41b6ef59c7062712fa18ee430850b94c724d4b2fa76"),
    "sa": (2005224, "0dfc54e6d556a04d5c36a816ad7119d07db8e6de12f2a5e44983868d2b66e72f"),
    "sc": (4152477, "ec222ab62692178887195073d9d760cc2abd5f307e3bb34c3f1b96074b282fb1"),
    "ta": (328110, "90f2457c51d4298edbb695e1ae48e4bc860ed49e148e05b4658c6b6c5be105ba"),
    "tb": (1133506, "6ff86a6816cfddf41ce59042953c264fd8877a8d2215ccf143f5a69d8701a0d5"),
    "tc": (593192, "2a78521244823534e52f95fd57f0f053e77380a7ccb361300f3f8bcbff597f52"),
}
_original_fixture_error = runner.fixture_error


def fixture_error(path, spec):
    error = _original_fixture_error(path, spec)
    if error or path is None or path.suffix != ".rrworld":
        return error
    key = path.name.split("-", 1)[0]
    size, sha = AUTHENTIC[key]
    if runner.fingerprint(path) != {"bytes": size, "sha256": sha}:
        return f"BLOCKED: authentic fixture identity changed: {path}"
    return None


runner.fixture_error = fixture_error
runner.EXTRA_INPUT_PATHS = (
    "tools/test_native_world_index49b.py", "cmake/NativeWorldIndex49B.cmake",
    "platform/windows/native/import_service48f.h",
    "platform/windows/native/import_service48f.cpp", "cmake/NativePc49B.cmake",
    "evidence/playerEvent49A/recovery-manifest.json",
    "evidence/playerEvent49A/original-disassembly.txt",
    "tools/gameplay/player_event_recover49a.py",
)
runner.SOURCES = (
    "src/assets/import_world48f.cpp", "src/assets/world_asset48f.cpp",
    "src/assets/world_index49b.cpp",
    "src/game/control_filter48b.cpp", "src/game/control_shaping48v.cpp",
    "src/game/control_binding48w.cpp", "src/game/player_input48x.cpp",
    "src/game/device_held48y.cpp", "src/game/event_dispatch48z.cpp",
    "src/game/player_events49a.cpp",
)
runner.INCLUDES = ("src/assets", "src/game")
runner.TESTS = {
    "world_index49b": {
        "source": "tests/native/world_index49b.cpp", "fixture": None,
        "extra_fixtures": [
            {"fixture": f"{FIXTURE_BASE}/{architecture}-world-fixtures/{name}-tracks_{name}.rrworld", "binary": True}
            for architecture in ("x64", "x86") for name in AUTHENTIC
        ],
    },
    "player_events49a": {"source": "tests/native/player_events49a.cpp", "fixture": None},
    "event_dispatch48z": {"source": "tests/native/event_dispatch48z.cpp", "fixture": None},
    "device_held48y": {"source": "tests/native/device_held48y.cpp", "fixture": None},
    "player_input48x": {"source": "tests/native/player_input48x.cpp", "fixture": None},
    "control_binding48w": {"source": "tests/native/control_binding48w.cpp", "fixture": None},
    "control_shaping48v": {"source": "tests/native/control_shaping48v.cpp", "fixture": None},
    "control_filter48b": {"source": "tests/native/control_filter48b.cpp", "fixture": None},
}
if __name__ == "__main__":
    sys.exit(runner.main())
