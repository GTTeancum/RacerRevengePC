# Synchronous selected-light snapshot borrow

UNTESTED; source-reviewed only. No build, test or benchmark was performed.

The submit function already owns validated local selected-light value snapshots
for the player and bounded opponent set. The renderer previously copied those
packets into Storage again and cleared the duplicates after synchronous drawing.
Storage now borrows const pointers to those local values for the same transaction.

A local scope guard clears the player pointer and the captured opponent-count
range on every exit before the local snapshots are destroyed. Capturing the
count matters because normal cleanup sets the Storage opponent count to zero.
All existing admission and local snapshot validation remain unchanged. Both
consumers immediately use the selected component to populate GL scalar uniforms;
no GL API retains a host pointer. The backend transaction is synchronous and
renderer reentry is forbidden by its existing contract.

This avoids up to nine additional packet copies and zero-fills, without changing
source-owner lifetime, illumination values or any draw order. The benefit is
unmeasured and is small relative to the previously observed driver cost.
