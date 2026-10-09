#!/usr/bin/env python3
"""Native impact, damage and repair integration; not a full gameplay benchmark."""
from pathlib import Path
import sys

sys.dont_write_bytecode = True
import test_native_vehicle_frame49w as previous

runner = previous.runner
runner.__doc__ = __doc__
runner.SUITE_NAME = "impact-frame50a"
runner.SOURCES = (
    "src/game/static_impact49y.cpp",
    "src/assets/impact_authoring49y.cpp",
    "src/assets/repair_authoring50a.cpp",
    "src/game/impact_consumers49z.cpp",
    "src/game/impact_aftermath50a.cpp",
) + tuple(runner.SOURCES)
runner.TESTS = {
    **runner.TESTS,
    "vehicle_impact_repair50a": {
        "source": "tests/native/vehicle_impact_repair50a.cpp",
        "fixture": None,
        "arguments": [],
    },
    "repair_authoring50a": {
        "source": "tests/native/repair_authoring50a.cpp",
        "fixture": None,
        "arguments": ["work/repairAuthoring50A"],
    },
    "static_impact49y": {
        "source": "tests/native/static_impact49y.cpp",
        "fixture": "work/local49Y/static-impact/fixtures.bin",
        "binary": True,
        "arguments": ["work/local49Y/static-impact/fixtures.bin"],
    },
    "impact_authoring49y": {
        "source": "tests/native/impact_authoring49y.cpp",
        "fixture": None,
        "arguments": ["work/impactAuthoring49Y"],
    },
    "vehicle_impact49y": {
        "source": "tests/native/vehicle_impact49y.cpp",
        "fixture": None,
        "arguments": [],
    },
    "impact_consumers49z": {
        "source": "tests/native/impact_consumers49z.cpp",
        "fixture": "work/local49Z/impact-consumers/fixtures.bin",
        "binary": True,
        "arguments": ["work/local49Z/impact-consumers/fixtures.bin"],
    },
    "vehicle_impact_consumers49z": {
        "source": "tests/native/vehicle_impact_consumers49z.cpp",
        "fixture": None,
        "arguments": [],
    },
    "impact_aftermath50a": {
        "source": "tests/native/impact_aftermath50a.cpp",
        "fixture": "work/local50A/impact-aftermath/repair-fixtures.bin",
        "binary": True,
        "arguments": ["work/local50A/impact-aftermath/repair-fixtures.bin"],
    },
}
runner.EXTRA_INPUT_PATHS = tuple(dict.fromkeys((*runner.EXTRA_INPUT_PATHS,
    "tools/test_native_impact_frame50a.py",
    "evidence/frameImpact49Y/consumer-copy-only.bin",
    "evidence/frameImpact49Y/repair-copy-only.bin",
    "work/impactAuthoring49Y/ta.tun",
    "work/impactAuthoring49Y/Pod01.phy",
    "work/impactAuthoring49Y/decimal.tsv",
    "work/repairAuthoring50A/Pod01.phy",
    "work/repairAuthoring50A/decimal.tsv",
    "work/repairAuthoring50A/authored.tsv",
)))

if __name__ == "__main__":
    sys.exit(runner.main())
