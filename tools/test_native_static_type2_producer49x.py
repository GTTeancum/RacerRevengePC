#!/usr/bin/env python3
"""After selective overlay: five bounded49X suites plus inherited69 baseline suites.
Use check_static_type2_recovered49x.py for a disk-bounded focused recovery gate.
"""
from pathlib import Path
import sys
sys.dont_write_bytecode=True
import test_native_vehicle_frame49w as previous
runner=previous.runner
runner.SUITE_NAME='static-type2-producer'
runner.__doc__=__doc__
runner.SOURCES=tuple(dict.fromkeys((*runner.SOURCES,
 'src/game/static_type2_producer49x.cpp','src/game/type2_resource49x.cpp','src/game/type2_triangle_axes49x.cpp',
 'src/game/collision_query48h.cpp','src/game/spring_callback48n.cpp','src/game/spring_records48n.cpp',
 'src/assets/collision_mesh48i.cpp','src/assets/collision_runtime48h.cpp')))
e='evidence/staticType2Producer49X'
runner.TESTS={
 'static_type2_opaque49x':{'source':'tests/native/static_type2_producer49x.cpp','fixture':e+'/original-word/opaque-snan-fixtures.json','arguments':[e+'/original-word/opaque-snan-fixtures.json']},
 'static_type2_early_reject49x':{'source':'tests/native/static_type2_producer49x.cpp','fixture':e+'/admission/first-reject-fixture.json','arguments':[e+'/admission/first-reject-fixture.json']},
 'static_type2_admission49x':{'source':'tests/native/static_type2_admission49x.cpp','fixture':None,'arguments':[]},
 'static_type2_producer49x':{'source':'tests/native/static_type2_producer49x.cpp','fixture':e+'/original-word/native-fixtures.json','arguments':[e+'/original-word/native-fixtures.json']},
 'type2_resource49x':{'source':'tests/type2_resource49x_test.cpp','fixture':e+'/resource/fixtures.json','arguments':[e+'/resource/fixtures.json','.']},
 **runner.TESTS}
root=Path(__file__).resolve().parents[1]
new=['tools/test_native_static_type2_producer49x.py','tools/check_static_type2_recovered49x.py','cmake/NativeStaticType2Producer49X.cmake']
for sub in ('original-word','axes','mode0','resource','admission'):
 new.extend(str(p.relative_to(root)) for p in (root/e/sub).rglob('*') if p.is_file() and '__pycache__' not in p.parts)
runner.EXTRA_INPUT_PATHS=tuple(dict.fromkeys((*runner.EXTRA_INPUT_PATHS,*new)))
if __name__=='__main__':sys.exit(runner.main())
