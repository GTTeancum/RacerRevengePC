#!/usr/bin/env python3
"""48X authored player axis conditioning, ordered action reader and connected input pipeline."""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="player-input"
runner.EXTRA_INPUT_PATHS=("tools/test_native_player_input48x.py","cmake/NativePlayerInput48X.cmake","evidence/playerInput48X/recovery-manifest.json","evidence/playerInput48X/provenance-blocks.json","tools/gameplay/player_input_recover48x.py","tools/gameplay/player_input_provenance48x.py")
runner.SOURCES=("src/game/control_filter48b.cpp","src/game/control_shaping48v.cpp","src/game/control_binding48w.cpp","src/game/player_input48x.cpp")
runner.INCLUDES=("src/game",)
runner.TESTS={
 "player_input48x":{"source":"tests/native/player_input48x.cpp","fixture":None},
 "control_binding48w":{"source":"tests/native/control_binding48w.cpp","fixture":None},
 "control_shaping48v":{"source":"tests/native/control_shaping48v.cpp","fixture":None},
 "control_filter48b":{"source":"tests/native/control_filter48b.cpp","fixture":None},
}
if __name__=="__main__":sys.exit(runner.main())
