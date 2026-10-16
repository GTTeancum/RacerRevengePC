# Canonical wreck model append and authored position

Source-only bounded increment againstccb609f. No builds, tests, syntax checks,
sanitizers, project decoder executions or gameplay were performed. Static
inspection used existing recovered native leaves and raw read-only ELF words.

## Implemented phase

`appendFirstModel165CF0(sessionGeneration,racerKey)` accepts only the actual
first occurrence that completed fresh resource/packet loading and stopped at
165CF0. It appends the canonical ModelSceneOwner Entity to the actual piece
with mask0, through the existing141960 native component operation. PSG child
rows remain a separate hierarchy and are not appended as Entity children.

165CF8..165D80 compares piece50 X, Y, Z against the actual433B10 constructor
vector, with short-circuit comparison and no W comparison. On equality,28B180
copies the current canonical model's local F0/F4/F8 into piece50 XYZ. It does
not refresh world100 or replace piece50 W. The inventory's earlier numeric
preview is not used as a substitute for this reached source read.

165D8C calls28AB90 with actual433B10. The continuation uses
setLocalTranslation49P, preserving localFC and every basis word while performing
real28C4F0/ready-gated289A20 invalidation. It does not refresh the world cache.
Source append and any authored50 copy survive a later rejection. The admitted
continuation is one-shot; no repeated reference, child-list or position writes.

## Exact remaining setup boundary

The method returns `pieceSetupRequired` at165E90, the callee entered by165DA8.
D0/gravity are deliberately still unreached because165E90 first stores two
unrepresented source inputs to pieceB4/B8. Their actual source words are known;
their scalar/pointer semantics and native owner identities are not yet proved.
No fabricated zero or opaque pointer stand-in is installed.

Read-only source chain:

-212A6C stores saved s0 in outgoing stack18;212A70 loads incoming stack1170
 and212A78 stores it in outgoing stack20 before22BFD0 at212A90.
-22BFD0 reserves320 bytes.22C160 loads old caller stack18 through338(sp)
 for the eventual t3 input;22C170 loads old caller stack20 through340(sp)
 and forwards it as the first stack argument to2CB5A0 at22C1A0.
-2CB5A0 forwards those inputs to165C30 at2CB5FC.165C30 retains its t3 in
 s0 and forwards it to165E90 t0, while forwarding its first stack argument
 through t1 at165D94.
-165E90 retains t0/t1 as s4/s3, then stores B4 at165EC8 and B8 at165ED0.

141C20 at165EBC is the recovered identity-return no-op, with no naming or
registration side effects. It is not the blocker. The B4/B8 input semantics
must be recovered before honestly advancing this setup prefix.

Once those inputs are represented, the already recovered next steps are:
167920 body construction at165EE0, actual D0 publication165EE8, literal
slot30 initialization, model bind1652E0, body320=1,1653C0(0), and141960 body
append with maskFFFFFFFF at165F3C. Then282670 gravity construction165F58,
capacity1 and actual own-D0 target, and mask0 append165F94. The authentic
4431C0 force vector and body constructor/reset globals remain required.

## Ownership and retirement

This patch does not promote the prefix to the old completed pre-E0 status.
The D4/D0 retirement helper remains closed because neither component exists.
The appended model owns real material/resource/cache/row-table/packet facets;
its240450/26E460/2896D0 retirement still must be joined in source order.
Neither query backing cancellation nor a null-resource shortcut releases it.
The first piece remains unappended to its collection, and no later occurrence,
query publication, effect callback, death, KO or active driver path is enabled.
