# Actual Application28C race publication

Source-only native ownership facet; no builds/tests or runtime activation.

## Source order

Retained full static words show17D678 invokes1A65C0,17D680 stores its exact
returned owner into Application28C,17D684 reloads that slot, and17D690 dispatches
slot30. Only AFTER that load returns does17D6A0 attach the race's actual sibling
chain to Application10. The alternate constructor route17C17C publishes before
its load as well.17C6A0 initializes28C to null. On retirement17D8E8 detaches,
17D8FC invokes final release, and17D904 clears28C afterward.

## Same-owner native lifetime

`Application::publishConsumedRace28C` accepts only the shared pointer already
minted by this Application's successful consumed-launch race constructor. It
checks the actual same Application Entity authority, profile, constructor suffix,
canonical enrollment and detached pre-load state. No new count or duplicate
Entity is created. The private strong slot is the actual28C owner; `race28C()`
provides a read-only identity borrow. Publication is one-shot and does not imply
successful load, root attachment or scheduler readiness.

`beginConsumedRaceLoad1A6AA0` now requires this actual published owner before
its first source effects. The shared slot pins the same partially loaded race
through any later failure. A second constructor/publication cannot replace it.

The existing explicit `closeNativeConsumedRace` retains a local strong pin,
performs real resource/group/world/FIFO/count retirement, and clears28C only
when the actual reference is zero, topology detached and acquired backing gone.
Rejected or partially completed cleanup retains28C. A repeated successful close
is accepted while the caller retains that retired owner. No source1A6840/full
original destructor completion is asserted by this native cleanup.

The race retains ApplicationEntities and the profile, not the Application object.
Its display retains only a raw Application identity. Thus the strong28C slot
introduces no owning cycle back to Application. Application destruction requires
this explicit slot cleanup before member destruction; silently dropping a live
race could destroy its borrowed context in the wrong order. Native driver
staging must publish before early load and close after its actual GPU/runtime
borrows are retired. No driver call or root attachment is added here.
