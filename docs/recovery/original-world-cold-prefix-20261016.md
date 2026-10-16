# Same-scene world constructor prefix and native failure rollback

Source-only at24272d1. No build, tests, syntax checks or decoder execution.

The existing ra_pod_scene50LA::Owner now owns a private counted world prefix,
not a replacement RA world, visibility registry or collision graph. Its
Application-aware aggregate entry requires the actual decoded source and
rejects already reached particle/model/scene construction and live borrows.
The SAME audio group owner must have exactly six original group constructors
and loads, the same Application/session, current mixer epoch and creating
thread. Native list storage and Entity authority capacity are preflighted.

Reached order:26FC90->141300 increments/enrolls world first;30=1 and the
fresh native scene's UseEnvironmentNode35C=0 precede the SAME World4D8
2350D0 constructor/load. Only after that is the derived500 list published
with capacity10/growth100000/count0, then actual process434320 is cleared
before524=3, matching1B28D0/D4.
The owning native source/lighting/query resources are unchanged. No guest
allocation layout, untouched flags or unknown fields are fabricated.
The bool result certifies this bounded constructor prefix, not270550 field
coverage, a loaded world, full race construction or scheduling readiness.

## Why normal empty-world destruction is not admitted

270550 never initializes3CC or3D4;26FC90 and1B2870 do not either.26FD80
nevertheless consumes both. Their actual loader producers are271230
(store3CC at27126C, conditional null271340) and270CC4..CE8 (construct
27C010, publish3D4, allocate three zone arrays through27C110). Normal
cleanup also reaches LightManager148110 and context103DD0. No missing
resource is turned into a successful null branch.

## Explicit native failure cleanup

rollbackNativeWorldConstructionFailure is separate from original142590,
1B2920 and26FD80. It admits only the unchanged detached constructor prefix,
no reached scene topology/particles/environment method, and zero list count.
A genuinely acquired World4D8 group retires through its independently
implemented empty typed release. Only this prefix's native backing and
canonical Entity enrollment/reference/count then retire. No unknown source
field is read, no original world callback is claimed, and the constructor's
real process-word clear is not rolled back. Failure preserves acquired owners.
If the seventh group failed before acquiring a count, only the actual world
prefix is retired. The actual caller must keep the audio/mixer alive through
this explicit operation. Nonempty loaded-world native rollback is not added.

Existing aggregate.close refuses a surviving counted world prefix before any
resource mutation. The explicit host rollback must precede normal aggregate
close on this bounded failure route. Existing legacy nonempty aggregate
teardown and ordering are unchanged. Destructor fail-stop preserves the
requirement to retain unsupported partial owners rather than silently drop
counts. Successful loaded-world retirement remains gated on actual3CC/3D4,
global cleanup and full topology/resource joins. No runtime calls are added.
