# Authentic Rider binder material and unique animation owner

Four NEW files under files/src/{assets,game}; no shared-file modifications.
Source-only candidate, NOT built, syntax-checked, tested, parsed/imported or
executed. All integration and validation belong to root. Static ELF decoding
is the only executable research performed in this increment.

## Concrete cold owner

OriginalUiRiderBinderAssets50KG pins authenticated Rider and APPSTART archives.
Its cold load invokes EXISTING original_ui_material50kg, original_ui_atm50kg,
and original_ui_screen_assets50kg; no duplicate PSM/PST/ATM parser or generated
texture exists. Those calls are staged source only and were not run here.

The original geometry constructor resolves a real DEFAULTMT associated owner:
1D3E10 virtual74 -> 23BEB0 -> 150450 -> material virtual0C at150524, with a3
loaded from3833E8 at150514. That is DefaultMT, not ScreenMT3833F0. The new owner
loads authentic APPSTART templates and explicitly selects defaultMt. Borrowed
material/template identities are pinned by both retained archives.

It reads authentic ui/textures/binder.psm (Rider logical offset2605056,
size2768, SHA256 ffa0e0631bbf4c4e1d15ccbd74651f7752560cb48da833459d7be773f6106249).
The unchanged material reader preserves the original Diffuse program and all
resolved GS/filter/group fields including the compatibility inputAlphaD alias.
The bounded binder contract checks one enabled map/pass, binder texture, UV0,
A0/B2/C0/D1 additive, textureMapping1, alphaBlending1, alphaTest0, zWriteDisabled1.
Other inherited state is preserved, not silently zeroed. Actual runtime435670
must be supplied as known; a runtime override incompatible with this bounded
binder profile rejects rather than fabricating normal mode.

TextureAnimation=unique, FPS16, reverse0, loopTo0 are checked against the actual
retained authored section. InitialFrame and Looping are omitted in binder.psm;
original shared constructor defaults are0 andtrue respectively.

It resolves authentic ui/textures/binder.atm (logical offset1007616,size51492,
SHA2567dc77ff532963c80df154c9d3d401004d472e431ceb92bab7b125968ed8740d4), requires
revision1/storage0/ten frames, and feeds each exact stored-PST offset+length
into OriginalUiScreenAssets50KG in original order. All original palette/mips/
raw alpha remain in the shared decoder's Texture48E. There is no binder.pst;
an unexpected same-name PST rejects rather than selecting a static substitute.
The owner-local 64-bit image IDs encode authenticated ATM logical offset and
1-based frame ordinal; they are keys only, not synthetic asset provenance.

Publication is atomic: the complete private storage publishes only after all
cold operations succeed. Failed loads leave the empty owner retryable; reload
is rejected. All exposed pointers require keeping the owner alive.

## Per-instance animation and source sequencing

OriginalUiRiderBinderAnimation50KG is noncopyable, one instance per actual
binderleftN/binderrightN helper. It shares immutable cold assets but owns its
own OriginalUiTextureAnimation50KG state. Two binders must not share a phase,
reset each draw, or share a global texture animation clock state.

Initialization uses the actual shared defaults and authenticated overrides:
initial0,FPS16,loopingtrue,reversefalse,loopTo0,count10. It does not sample an OS
clock. previousMilliseconds starts original sentinel0xFFFFFFFF.

Call advanceAfterGeometry at the original1D4240 point: after the endpoint/camera
geometry update and only if the original helper's material handle is valid.
Source1D4228 obtains the billboard material,1D4240 resolves298E60,1D4248 calls
29BFF0, and1D4254 continues child141C30. 29BFF0 visits each material pass virtual20.
Pass the existing sampled uint32 application-millisecond clock into the shared
original_ui_texture_animation50kg leaf. Do not derive millis from audio, wall
time, renderer counters, or a separately accumulated delta.

The existing shared leaf owns first-sample zero advancement, repeat-timestamp
suppression, source binary32 progression, uint32 wrap, loop behavior and frame
truncation. current() only binds the already selected frame; it never advances.
The class returns an immutable frame observation: actual Diffuse pass, common
image owner, exact Texture48E, original frame index and stored-PST range.
Failure preserves both animation state and the caller's output. Destruction
releases the actual instance; creating a new source helper initializes fresh
state. Reselecting without source destruction must not reset an existing clock.

## Renderer boundary and joining the frozen geometry

Use OriginalUiRiderBinderQuad50KG from ../binder unchanged. Its four world
vertices/UVs pair with the selected frame and exact material pass. Keep source
vertex ordering, actual transform/camera/light state, original material ordering
and original packet colors; this owner invents no normals, alpha, lighting or
screen projection.

IMPORTANT: prepareOriginalUiNativeMaterial50KG currently admits ScreenObject,
not Diffuse. Calling it on binder will reject correctly. Do NOT relabel Diffuse
as ScreenObject, DiffuseCompressed, or reuse a successful UI policy as renderer
proof. The racer-lighting owner confirmed this increment supplies numerical
lighting recovery only and has no live binder renderer interface. Therefore
native Diffuse instruction/material admission and actual draw submission remain
explicitly unsupported by this package. loaded()/initialized()/current() certify
only their named CPU ownership stages, never GPU or Rider-destination readiness.

Remaining parent integration:
1. Validate/build/test centrally and bind archive/runtime owners.
2. On authentic helper construction initialize a distinct animation instance.
3. At geometry update1D4240 advance with the source clock; continue child phase.
4. Retain observations through submission/drain, bind exact selected original
   frame, and add source-attested Diffuse rendering without changing its identity.
5. Tie lifetime to real optional named-node pair attachment/removal.

Evidence: local evidence/*.txt are static byte-to-text decodes. Broader creation
and geometry evidence remains ../preview/evidence/user-rotate-binder.txt;
authenticated raw PSM/ATM and provenance remain ../pod-assets/members and its
archive-member-identities.json. No frozen binder/dynamics/pod asset files changed.
