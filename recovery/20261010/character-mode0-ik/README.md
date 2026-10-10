# Frozen character mode-0 IK staging

Deliverable: native CPU reconstruction of solver28FFD0 and complete absent-axis
constructor path1568C8..156920, ready for parent review/integration only.
No active repository writes or commits were made. No build, syntax check, test,
project parser execution, sanitizer or gameplay validation was performed.

## Integrate

The manifest freezes the exact preimage of src/game/chariot_ik49t.cpp and all
three proposed files. Review preimage compatibility before applying; do not
replace independent later changes blindly. proposed.patch contains the same
edits. New files have no preimage.

- files/src/game/chariot_ik49t.cpp: adds native mode0 functions and extracts a
  supplied-axis constructor helper while retaining old mode1 axis values.
- files/src/game/character_mode0_ik50kg.h: stable initializer, direct solve,
  callback context and transactionally carried mode0 lazy-state API.
- files/docs/character-mode0-ik50kg.md: detailed source map, numerical contracts,
  known degenerate boundaries and unperformed verification.

No CMake change is needed: all new definitions reuse the existing compiled
chariot_ik49t.cpp numeric owner. Source labels identify the origin of arithmetic;
there is no instruction dispatcher, guest memory, vector-register interpreter,
emulator or translated-code runtime. Existing native scalar/vector/matrix
functions are ordinary C++ operations on caller-owned typed scene nodes.

Shared playback worker and UI lifecycle worker have the exact callback header.
Playback must use the staged-shared argument and commit it with its graph;
prewarming source lazy words at construction would change original ordering.
Caller passes the same persistent ModelMode0IkShared50KG on later advances.

For each real endpoint in source registration order, call
initializeModelMode0Ik50KG with the exact setup graph, one persistent
ChariotIkChain49T output, shared SourceLibmState49T, ChariotIkWorkspace49T scratch
and ChariotIkDiagnostic49T. Set ModelMode0IkContext50KG to the resulting record
array/count, and install modelMode0IkCallback50KG plus that context in playback.
The context pointer must remain stable for the callback lifetime.

Important setup detail: UI source samples entry at .001, captures matrices,
then constructs IK; raw PSG bind matrices are the wrong boundary. Default
constructor axis433400 is(+1,+0,+0,1), from its actual static initializer.
Use actual node ancestry, even where l_ik/r_ik labels cross apparent leg names.
No missing endpoint or failed constructor may be replaced with success.

## Evidence and limits

The source ELF hash and eight binary source ranges are frozen in manifest.json;
text listings and the bounded static byte decoder are under evidence/.
The constructor reuses sourceAcos49T and all constructor effects; the runtime
mode0 solve does not consume rest-axis coefficients or invoke acos. Runtime
rotates current elbow, then shoulder, and never applies290A00 wrist orientation.
It preserves translation lanes and existing scene-cache ownership.

Only mode10=0/debug40=0, finite admitted graph/arithmetic and the existing49T
root/norm boundary are represented. Collinear and zero runtime source paths
are retained without inventing a bend pole; success does not imply an
unreachable target was reached. Constructor degenerate products and debug
profiling remain explicit unsupported cases. See the staged document for all
admission/rollback details. Numerical parity and actual Rider integration are
unverified until the parent's authorized validation window.
