# Retained original main340 body Entity

Source-only change based on central 0e22f22. No build, syntax check, test, sanitizer, converter, guest execution, emulator, gameplay run, or activation was performed.

## Concrete recovered cut

The source calls allocation2E8D90(0x330) at211F6C, then167920(a1=1) at211F80.167920 reaches164ED0 ->141300, installs PhysicsRigidBody dispatch3CE000, writes axis locks, invokes slotAC=167A50 ->slotA8=167AD0, and clears320.211F88 publishes340,211F98 invokes literal identity slot30=141C20,211FA8 appends mask2.211FB8 sets320=1.211FC8 invokes slot74=1652E0 with the actual retained scene190; this stores a borrowed pointer without retaining or changing the scene.211FE0/FE8/FF0/FFC write all four limits as bits501502F9.

OriginalRacerMainBodyOwner50LA owns this real native Entity/reference allocation and otherwise-unrepresented source fields, while borrowing the SAME Fixture.state.bodies.main, Fixture.parameters.bodies.main, canonical mainSupplement/mainExtra, and main full-vector lanes. No second main or secondary numerical body is allocated or copied for construction. The retained chariot Owner pins the exact Fixture, Bank, slot and generation already used by model/joint ownership. Every canonical numeric address is identity-only outside its bank visit.

The application's actual shared body-origin433AD0 and reset433B80 values come from startup3C9190/3C9290, each (0,0,0,1). File-backed quaternion382B60 contains four +0 words, not an identity quaternion. Original14D7F0 zeroes all sixteen matrix words: both constructor100 and inverse140 are zero, including every extra inverse lane. The constructor applies all represented original stores directly to the canonical numeric owner. It does not clear arbitrary fixture structures, controls, phases, collision state, or scene matrices.

The native retained-resource pipeline stages body construction and its scene/limit suffix during Rig.install, before the existing handling transaction. Session later publishes the same body340 alias and appends its actual Entity, before the retained joint360. This is an explicitly staged native projection of the admitted calls, not a claim that the complete source constructor ran in one contiguous invocation. The represented source child sequence is chariot animation ->340 ->360; intervening missing source siblings remain unrepresented, not synthesized.

The existing successful authored handling transaction already computes source21488C/165340 inertia100 and canonical inverse140. Immediately before its no-fail final state/parameter/tuning publication, the transaction copies that SAME matrix to the retained body100. Failure before commit preserves the constructor's body100 and actual live parameters. No independent inverse or extra physics binder is created. This narrow numerical join does not complete virtualA0/2146C0 or its other Entity consumers.

## Lifetime and failures

The owner retains an allocated/constructed/scene-bound/appended prefix, not a source-ready flag. Object counts increment through the same application's141300 and decrement through represented1413E0. Allocation/construction/handling/publication failures retain the actual prefix for explicit cleanup. Successful source publication does not allocate or construct the body again.

The component domain capacity grows from13 to14 for five animations, four racer-prefix components, three scene allocations, one main body, and one joint. Only the chariot role owns a main body. Session append order is mask2 body then maskFFFFFFFF joint. Cleanup preflights the whole represented tail before mutation, then retires joint, body, chariot. Body release represents142590 ->104970 ->1679D0 ->165260 ->1413E0; original28C remains null on this bounded cold route. This auxiliary pointer is distinct from the fixed inline20C/210 contacts. Body release does not zero the banked numeric body, dereference or release scene288, clear stale source links, or recompute ancestor masks. Earlier model/scene and racer-owned208/20C ordering is unchanged.

## Exact remaining boundary

The next unowned main construction is allocation211FF8 and gravity344 constructor282670 at21200C, followed by probe348/1B0EE0, engine34C/2E7510, aero350/16CCD0, main collision690 and694/11B190, spring354/295A40, detector358/295290, and auxiliary collision7A4/79C. Existing Fixture/Bank numeric data and native collision resources remain unchanged and do not count as those genuine Entity constructors.

Secondary35C cannot simply borrow an index and call it constructed:2124E8 allocates a separate0x1B0 SceneObject19C,2124FC calls2893A0, and212530 appends its Entity8 mask2 before21254C constructs35C. That distinct retained19C allocation is still missing. Secondary35C/368, intervening siblings, and full virtualA0/2146C0/late-strip admission remain open. This patch neither claims their completion nor bypasses any cold gates.

Original AI for all eight racers, input ownership, course driving policy, existing collision owners, and RA activation are unchanged. The no-retained-owner legacy Rig template route is unchanged.

## Static evidence

Authenticated data-only SLUS_202.68 SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. The packet includes exact words and range hashes for141300/1413E0,141C20,142590,14D7F0,164ED0/165260/1652E0/165340,167920/1679D0/167A50/167AD0, the caller's body/sibling/secondary prefixes,21488C's setter call, both body vtables, shared startup constants and file quaternion. Opcode annotations are static labels only; vtables and constants are explicitly data. All review was source reading.
