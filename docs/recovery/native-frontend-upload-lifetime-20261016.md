# Actual frontend upload lifetime

Base38b733e. Frontend::prepare owns a typed native upload lifetime tied to its
actual Frontend object, same Application Video, and same Application texture
manager. This is separate from the actual race display lifetime; no frontend
renderer is relabeled as a race renderer.

The existing cold.textureRequest receives a sealed binding after checking the
actual Application Video pointer and matching renderer/texture context callbacks.
The coordinator's real upload at original_ui_frontend50kg.cpp copies this cold
request before MaterialTextureGl::create. Initial Main and later page/rider/course
image replacements therefore use the actual upload ledger. Other callers' cold
requests retain existing optional behavior.

Bindings weakly borrow the typed frontend lifetime. An actual upload scope pins
it while registered in the SAME manager and checks its Frontend member identity,
actual Application Video and current host. closeUi refuses an active transfer,
then performs the existing coordinator.close. Failure retains the lifetime;
only successful/no-change checked close marks it retired and drops the member.
Stale copied requests cannot revive it across serial frontend sessions.

The frontend lifetime pins Video, but does not own its Frontend/coordinator or
renderer, and no owning cycle is introduced. Its member is declared before the
coordinator, so it survives the latter's ordinary member destruction. Actual
transfers remain synchronous/nonreentrant. Partial upload failure ends the
client transfer while unresolved persistent GPU names remain in their existing
checked owner; no claim of GPU-name retirement follows from scope completion.

The race and standalone frontend routes now both propagate genuine bindings to
their MaterialTextureGl uploads. This patch alone adds no aggregate2543F0 call,
no transfer-cache success stub, and no deletion of persistent decoded/GL images.
No build, test, syntax check, gameplay or decoder execution was performed.
