# Stage9: original Tournament portrait/background and unique marker subsection

Incremental after364b60d. Root trophy catalog-only separator fix1d74e95 must
remain applied; this patch deliberately does not duplicate that root edit.
No builds, tests, syntax checks, launches, gameplay or guest execution.

Ownership: only original_ui_tournament_career_resources50kg.{h,cpp},
src/assets/original_ui_material50kg.{h,cpp}, and this evidence directory.
No frontend, route header, profile or result integration edits.

Original source evidence:
- Existing tournament-entry.txt1EF014..1EF088: selected career formats
  %02d_flrg at3AF720, loads ScreenTexture1000 at1EF034, positions296,100.
- 1F177C..1F1930: selected %02d_bkg at3AF710; four ScreenTextures
  3D80+60*i, loader1F17AC, explicit depth15,width320,height224.
- 1F1934..1F1D38:319/223 endpoints multiplied by actual atlas reciprocals;
  positions(0,0),(320,0),(0,224),(320,224), mirrored right/bottom UVs.
- 1EFED4..1EFEE0: ScreenTextureFA0 loads medstar at1EFEDC.
- 1F10C4..1F139C: progression marker x41+37*n for0..3,
  215+37*(n-4) for4..7,386+37*(n-8) for8..12,y301;
  completed progress13 uses641,449. Existing progress>13 stays rejected.
- Existing tournament-popup-draw.txt1F2720..1F2898: actual popup29&1
  suppresses custom subsection; backgrounds1F273C,portrait1F2758,
  stats1F2778,medals,marker1F2884,numeric text.
- tournament-portrait-background-marker.txt is an offline data transcription
  from the authenticated disc UITOURN.RES, not a runtime asset replacement.
  It contains representative authored19_bkg.psm, exact medstar.psm text,
  and all nine ATM frame extents: revision1,selector0,46344bytes,
  each frame5148bytes,kind1,mip1,64x64,offset12+5148*n.
- medstar authored TextureAnimation=unique,FPS15,reverse0,loopTo0.
  Omitted initialFrame0 and looping1 use recovered29B950 defaults.
  Existing native29A*/29B* animation arithmetic supplies sentinel and repeat
  timestamp handling; no original instructions or project executable run.

Resource changes:
load now requires a retained authenticated Backgrounds archive.
Images0..12 unchanged;13 selected portrait;14 selected background;15..23
actual medstar ATM frames. Material bindings are per-image24 entries;
marker bindings all name ONE original medstar material and share ONE retained
unique animation controller, not nine registry/material instances.
Background PSM is authored in Tournament; its PST is in Backgrounds.
Portrait absent-PSM fallback has exact sourceCaller1EF034 and canonical
catalog whitelist. Notch catalog-only paths corrected to backslashes;
provenance names remain forward slashes. Stage8 count wording corrected.

Packets are borrowed, including materialName in the copied marker packet.
Caller MUST retain this resource owner through actual renderer retirement.
sampleMarker requires a known actual application clock, respects caller-owned
pause gating, rejects popup/stale generation before changing state, and
updates transactionally on leaf success. It is for an accepted frame only;
it supplies NO complete-frame commit/rollback or submission receipt.
The frontend must remain closed until a real session owns these boundaries.

Remaining:1F24B0 material-update hook, original widget/controller tree and
ordinary/top draw ordering, complete native pod material submission,
aperture/prompts, popup/save handling, registry/full-frame sealing and retained
submission/retirement. No Tournament race admission or profile/result writeback.
