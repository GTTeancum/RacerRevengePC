#!/usr/bin/env python3
"""49Q original engine-node animation and retained scene/body/input regressions."""
import sys
sys.dont_write_bytecode=True
import test_native_scene_projection49p as prior
runner=prior.runner
runner.__doc__=__doc__
runner.SUITE_NAME="engine-animation"
evidence="evidence/engineAnimation49Q"
runner.EXTRA_INPUT_PATHS=tuple(runner.EXTRA_INPUT_PATHS)+(
    "tools/test_native_engine_animation49q.py", "cmake/NativeEngineAnimation49Q.cmake",
    evidence+"/connected/supplied-controls49q.bin",
    evidence+"/connected/input-manifest.json", evidence+"/connected/exact-goldens49q.txt",
)
runner.SOURCES=("src/game/model_motion49q.cpp","src/game/engine_animation49q.cpp")+tuple(runner.SOURCES)
runner.TESTS={
    "model_motion49q":{"source":"tests/native/model_motion49q.cpp","fixture":None,"arguments":["."]},
    "engine_animation49q":{"source":"tests/native/engine_animation49q.cpp","fixture":None,"arguments":["."]},
    "engine_animation_diagnostic49q":{"source":"tests/native/engine_animation_diagnostic49q.cpp","fixture":None,"arguments":[]},
    **runner.TESTS,
}
if __name__=="__main__":sys.exit(runner.main())
