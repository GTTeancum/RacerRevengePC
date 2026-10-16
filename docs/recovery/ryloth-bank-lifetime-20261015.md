# RA Bank destruction-bound world registration

Base: 5057e29. Source-only candidate; no build, syntax check, tests or runtime invocation.

## API and scope

`ra_cold_aggregate50LA::Owner::registerRuntimeQueryWorldOwners(actualBank, diagnostic)` follows successful fresh runtime Storage and actual query-owner binding. It registers exactly the 178 retained candidates' F8/copied40/copied50 lanes, followed by the existing Storage::registerMutableOwners implementation. It does not register the arena twice or seal the Bank. In the eventual RA caller this replaces only the candidate loop and Storage call in native_racer_owners50hn.inc; all other arena registration and actual per-racer constructor/resource-switch order remains required.

The appended stage/prefix records registration completion only. Camera/query preparation, attachment, complete registry extent and gameplay admission remain separate and unavailable here.

## Ownership and destruction

Bank alone strongly owns its private RaWorldLifetime pin. The concrete private BankPin strongly retains the same OwnershipBridge already owned by the aggregate. Bridge retains only a weak pin and the stable Bank address; it retains actual candidates/source/Storage/Follow as before. No bridge-to-Bank strong edge exists, and no public injection, replacement, callback, mutable world getter or retirement token is added.

The pin is installed before the first span registration. Its member is declared before all Bank spans/snapshots so it dies last. Bank destruction checks an RA-bound Bank is outside both visit and read-blocked scope. After all snapshots/span descriptors die, the concrete pin destructor verifies any actual Follow preparation attempt already reached its real closed state. It reads only its retained bridge, never the destroyed Bank. SourceCamera remains responsible for original bank0 Follow prepare/close; destruction does not perform that work.

Aggregate close and runtime close reject while the weak pin is live. Reconstructing Bank at the same address cannot revive the expired control block or permit a second registration: bridge registration is lifetime-once and retains its original identity. No stale Bank address is dereferenced. All borrowed arena, recovery, wreck and immutable resource objects must remain alive through Bank destruction and successful aggregate close. Destruction of a borrower cannot bypass these checks.

## Failure behavior

Preconditions reject sealed/busy/already-pinned Bank and replay or wrong bridge before span effects. Pin-allocation failure leaves no new spans. Once installed, the pin survives every candidate/Storage overlap or allocation failure until actual Bank destruction; partial spans are never removed or reset. Aggregate/bridge become terminal. Successful earlier effects remain owned. An early rejected close does not mutate the bridge or reset the pin and may be retried after genuine destruction and existing membership cleanup.

No prepared wreck aliases are cleared. Existing prepared-wreck world-borrow checks still reject aggregate close even after Bank destruction. A genuine joint terminal unbinding operation on the actual piece/query owner is pending; it must account for saved snapshots and cannot run while the Bank pin remains live. Full camera factory, typed per-slot attach and wreck retirement are intentionally not implemented.

## Compatibility / inclusion

Existing unbound Bank routes receive only an empty private shared_ptr and a conditional destructor guard; registration, snapshot and visit implementations are unchanged. Bank was already noncopyable and nonmovable. The bridge explicitly includes Bank's header; Bank only forward-declares the bridge, keeping the include graph acyclic. All definitions are inline in three existing main-TU-reachable headers, with no new build-list requirement.
