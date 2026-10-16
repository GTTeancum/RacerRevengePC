# Same-owner Video44 and renderer2C stores

Static source recovery closes two scalar-state gaps without activating a race
constructor, load, destructor or Pause callback.

## Video443A60 word44

259E00 saves its actual this pointer at stack5C;259F54 reloads that same pointer
into t3 and259F58 writes zero at44. Actual race load1A764C..58 resolves global
Video443A60, sets v1=1 and stores word44=1. Race destructor1A6868..70 stores zero.
Other real transitions17CD5C and17DDE8 also store1. Reader25BBF8..BC08 branches
on zero/nonzero. This field is therefore represented as the actual scalar word,
not a resource pointer or native render-queue identity.

The existing Application-owned Video now privately stores sourceMode44_=0.
Private beginRaceDisplay1A7658 and clearRaceDisplay1A6870 implement just those
reached stores. Existing Application friendship is the only intended future
entry. No load method runs implicitly when a native RA scene becomes available.

## Renderer442F70 word2C

Renderer243A80 invokes base26F870, whose26F880 initializes2C=0. Race load starts
at1A6AA0;1A6AC0 sets v1=1,1A6ACC/D0 resolves the actual renderer, and1A6AD8
stores that one in the delay slot of141C20. Race destructor1A6874..7C clears2C.
Reader243FC0 loads2C and selects default mask2 when zero or3 otherwise before
calling25C0A0. It is not current scene pointer0 or queue ordinal44.

The same RaceDisplayLifetime50LA that already owns the actual native renderer
and original dispatch fields now privately owns sourceClearMode2C_=0, plus the
exact beginRaceClear1A6AD8 and clearRaceClear1A687C stores. Application already
has private access. No public fabricated state or new owner is introduced.

## Integration boundaries

Future genuine load must write renderer2C at its early1A6AD8 boundary and Video44
at its later1A7658 boundary. Destructor must clear Video44 first, renderer2C next,
then the still-missing process global3E1C10 before later conditional releases
and1AB4F0. These operations cannot be collapsed into a single active-default
constructor or inferred from current native RA readiness. Other source writers
and display consumers are not claimed integrated by these private leaves.

This closes represented scalar ownership only. The complete genuine race owner,
texture-manager13C transfer/cache allocation lifetime, global3E1C10, and exact
parent/input/scheduler activation still require their own concrete joins.
The existing renderer reset and two real destructor drains remain independent.

Source-only inspection and edits. No builds/tests/syntax checks, project decoder
execution, gameplay or guest execution performed.
