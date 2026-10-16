# Retained original gravity344 Entity

Additive source-only change after c0e750c, retaining the integrated draw and main340 work. No build, syntax check, test, sanitizer, converter, gameplay, guest execution, or emulator was run.

## Source and concrete ownership

211FF8 allocates0x60.21200C invokes282670(a1=1), which reaches141300 and installs3D2E60. Its represented suffix writes count30=capacity34=0, acceleration38 bits4200CCCD, force40 from the application's shared4431C0, and array50=null. Static initializer3CAA20..3CAA9C establishes4431C0=(+0,+0,+0,1).

212024 calls slot74=282750(1), writing count30=0/capacity34=1 and separately allocating one body pointer. The native owner creates that actual allocation with native pointer width. Its unused entry is deliberately not value-initialized because the source does not initialize it.212034 appends the gravity Entity with mask0.212048 invokes slot78=282790, writes the actual borrowed main340 pointer into array50[0], and increments count30. There is no body reference retain, scene operation, or extra slot30 call.

OriginalRacerGravityOwner50LA owns the Entity/reference, count/capacity and target array. Its acceleration38 is the SAME banked MainBodyState49K.selectedGravity used by selectGravity49K and the existing numerical force producer. Its force40.X/Z are the SAME MainBodyParameters49K.gravityForceX/Z. Constructor stores write those real locations directly; no second gravity scalar, body, force calculation, or numeric cache is introduced. W is copied from4431C0 into its previously unrepresented private4C lane and remains untouched by2827D0. The chariot's existing Fixture/Bank/slot/generation lifetime pins these pointers.

Like the existing retained main/model staging, constructor and target-array allocation occur before the handling transaction. Session publishes the same gravity344 alias, appends it after main340 and before the already retained joint360, then writes its target pointer through the real282790-equivalent call. The target is the retained body340 allocation, not a copied body or another racer's numeric snapshot. The source-owned borrowed pointer remains separate from the banked address shared by serial visits.

## Runtime boundary

The old native numerical2827D0 leaf computes force40.Y=(-mass)*acceleration and adds XYZ to body force180, but does not retain that transient Y cache. This patch explicitly leaves source40.Y unrepresented rather than inventing a stale duplicate. It does not advertise a complete Entity-backed2827D0 method or provide a new callback. Any future real callback requiring the retained Y cache must first join that write to canonical state; it cannot read a constructor-only zero here. Existing force mathematics and routing are unchanged. No source-ready flag is added.

## Lifetime and failures

The exact constructed prefix survives failure.282750 allocation failure leaves the real source count0/capacity1 prefix and null array for cold cleanup, without reading an unused slot or replaying construction. A Session failure after append and before target registration retains the appended Entity and its live array. Fifteen domain slots cover five animation Entities, four racer-prefix components, three scenes, main340, gravity344 and joint360.

Cleanup preflights the represented tail and then releases joint360 ->gravity344 ->main340 ->chariot.142590/104970 enters gravity destructor2826D0.2826F8/282704 frees the array, and282710 reaches1413E0. Array destruction never dereferences, releases or deletes the borrowed body. The body outlives the gravity allocation. Canonical numerical gravity, source stale links/counts and racer344 alias are not cleared by source teardown. Host unique_ptr release is host ownership bookkeeping, not a claimed source NULL store.

## Remaining source cut

Next is probe348 allocation212050(0x240), constructor1B0EE0 at212064, and binding1B11B0 at21207C with actual340/330/334. Existing numeric probe data and water providers do not establish its genuine Entity lifetime. Engine34C, aero350, collision690/694, spring354/detector358, auxiliary collision7A4/79C, distinct scene19C and secondary35C remain unjoined. Full virtualA0/2146C0 and late-strip admission remain blocked exactly as before. This patch changes no collision resources, original all-eight AI policy, input, course-driving behavior, or RA activation.

The packet provides exact preimages, changed-file hashes, and authenticated static words/range hashes for the constructor, target allocation/registration, destructor, caller, table and shared initializer. SLUS_202.68 SHA256 is c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. Opcode labels are static annotations only, with data tables identified as data.
