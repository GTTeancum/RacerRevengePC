#!/usr/bin/env python3
"""Focused static query-to-response checks; explicit inputs, no gameplay claim."""
import sys
sys.dont_write_bytecode = True
import native_frame_runner49w as runner

runner.SUITE_NAME = "static-contact-response50e"
runner.SOURCES = (
    "src/game/static_contact_response50e.cpp",
    "src/game/static_type2_producer49x.cpp", "src/game/type2_resource49x.cpp",
    "src/game/type2_triangle_axes49x.cpp", "src/game/collision_query48h.cpp",
    "src/game/spring_callback48n.cpp", "src/game/spring_records48n.cpp",
    "src/assets/collision_mesh48i.cpp", "src/assets/collision_runtime48h.cpp",
    "src/assets/import_collision48g.cpp",
    "src/game/spring_model48k.cpp", "src/game/spring_contact48j.cpp",
    "src/game/spring_impulse48k.cpp", "src/game/spring_force48l.cpp",
    "src/assets/spring_authoring50f.cpp", "src/assets/impact_authoring49y.cpp",
    "src/assets/pod_physics49n.cpp", "src/game/authored_parameters49n.cpp",
)
runner.TESTS = {
    "static_contact_response50e": {
        "source": "tests/native/static_contact_response50e.cpp",
        "fixture": "evidence/springResponse48L/coordinator-fixtures.json",
        "arguments": ["evidence/springResponse48L/coordinator-fixtures.json"],
    },
    "static_contact_connected50e": {
        "source": "tests/native/static_contact_connected50e.cpp",
        "fixture": "evidence/staticContactResponse50E/actual-query-subset.json",
        "arguments": ["evidence/staticContactResponse50E/actual-query-subset.json"],
    },
}
runner.EXTRA_INPUT_PATHS = ("tools/check_static_contact_response50e.py",)
if __name__ == "__main__":
    sys.exit(runner.main())
