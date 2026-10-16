# TimeStats Entity base and actual HUD parent

Static source/ELF data only. No execution or implementation in this check.

## Genuine TimeStats base

193100 calls141300 at19310C. The actual race caller supplies a1=1 at1A8BE4 before1A8BE8.141314 invokes1048F0:64-bit reference00=1.14132C sets category28=FF;141330..33C clears previous10,next14,firstChild18,parent1C.141340..3A8 writes flags29 low nibble3 (preserving upper bits).1413AC/B0 clears own mask20 and descendant mask24.1413B4..3C0 increments the ONE Application423F60 counter with source32-bit arithmetic. No name/type registry insertion occurs here.

193100 then constructs six ScreenText, six ScreenTexture, and extra ScreenText550; none is an Entity child. Derived race pointer30 is cleared at19317C and reset194CC0 runs. Loader193240 calls141C20 at193260;141C20 simply returns a0 and does not attach children or change masks. Remaining loader calls are resource/text operations, not Entity append calls. Bind193BB0 writes derived race30 at193BB8, copies mode8C→520 and lays out view1. It does NOT write Entity parent1C.

Own update194D00 finishes with Entity141ED0; demand194E20 calls141C30 at194F20; draw194FF0 calls141D70 at1950B8. Their descendant guards test mask24 bits2,1000,8 respectively. They walk firstChild18/next14, checking child flags/mask, and invoke actual child virtual methods; draw/demand/update traversals also perform renderer service before eligible child calls. With original empty firstChild18 and descendants24=0, these own child tails perform no dispatch. This bounded empty-subtree fact follows the inspected constructor, full loader, bind and TimeStats methods, not the mere absence of native children. A future native adapter must reject an unexpected child/mask rather than silently skip it. This is not a whole-program alias proof of arbitrary external code.

Destructor1931A0 destroys extra550, then six textures reverse, then six texts reverse, then calls1413E0 at193208.14140C..420 decrements actual423F60 then invokes104910, which has no child walk or reference decrement. The separate release104970 decrements reference00 and invokes virtual destructor+8 only when zero. Do not conflate base destructor with recursive release142590 or invent a detach in1413E0.

## Actual race attachment is later and is not detached

In1AAD40, constructor1891A0 creates a separate48-byte HUD Entity parent at1AAE04, with a1=1. Its virtual loader+30 at1AAE14 resolves141C20.1AAE28 appends this parent to actual race via141960 with maskFFFFFFFF. The same parent receives race+C8 at1AAE38, race+CC at1AAE48, **TimeStats race+D0 at1AAE58**, race+C4 at1AAE68, race+D4 at1AAE78, and further real HUD siblings later. All use maskFFFFFFFF. TimeStats's sibling/parent links are therefore nonnull in the completed original tree, although its own child list remains empty. The HUD parent is not retained in a newly invented race field; the real child topology owns it.

1891A0 calls141300 and sets vtable3CE9A0. Table words establish +30=141C20,+38=141ED0,+40=189240,+48=1892A0,+10=142590, destructor+8=1891E0.1891E0 calls1413E0. The source parent has meaningful specialized behavior:
-189240 returns without demand traversal if global382C00==0; otherwise calls141C30 at189280.
-1892A0 returns without drawing if382C00==0. If435600!=0, it calls renderer slot20(1),slot24 before child traversal.189328 calls141D70. With435600!=0 it again calls slot20(1),slot24 afterward; otherwise1893A8 calls slot20(0).
-1893D0 handles an input toggle of382C00 before ordinary142350 delegation.
Thus a direct TimeStats render call alone is not full parent scheduling parity. Do not drop these gates or substitute overlay visibility.

## Existing owners and smallest honest join

OriginalApplicationEntities50LA already owns stable global423F60 and attachment lock423F58 across races, plus Application root identities. Its objectCount423F60() reference is the correct counter. Its initial two-root domain is not a complete race/HUD forest. OriginalRacerEntityCold50LA demonstrates the existing represented-field initializeOriginalWreckComponent50LA and strict ordered cleanup model; it deliberately admits a bounded detached cold prefix, not a fake full race ancestry.

For a cold TimeStats base prefix, embed a real OriginalWreckComponent50LA in OriginalTimeStats50LA, initialize it once via existing initializeOriginalWreckComponent50LA with actual Application counter, true sourceA1 and stable authentic native owner/session identity, before ScreenText/ScreenTexture construction stages. Keep the Application entity owner alive through final base cleanup. Use entity141300 only for the represented base prefix or add an explicit concrete kind if dispatch needs it; do not claim its virtual behavior is a generic base. Retain partial prefix on later resource failure, and decrement the actual global exactly once after derived resources are destroyed. Ref00 release/destructor phase must be explicit and validated; no blanket counter reset/subtract.

A completed race join additionally needs the actual race Entity ancestor and the actual1891A0 HUD parent, real ordered141960 attachment with the actual lock, and eventual142590/104970 removal/release. No native1891A0/1AAD40 owner was found in current source. Implementing a substitute Application-root link, skipping siblings while claiming complete tree parity, or merely setting parent readiness would be false. A bounded subset must state exactly which real nodes it represents and preserve their actual source ordering/gates.

Current isolated candidate contains resource/CPU geometry ownership but lacks this base component/counter and parent join. It must not advertise complete193100, complete source-loader-to-race publication, or full update/draw lifecycle yet. Root should decide the bounded base and parent integration before promoting activation.

## New demand facet source review

OriginalStandardTextureResource50LA::demandTimeStats194E20 admits only194E58,194EB8,194F18; rejects aggregate, unconstructed/unreferenced/dead-context resources; then applies the exact finite-domain floor `if (!(10 <= demand10)) demand10=10`. The additional ordinary/nonnegative admission is the existing native numeric subset, not guest all-float parity. Source-proven no issue with this guard. The private owner must still verify the exact stats/font canonical lease and call in original stats,13ingame,16ingame order. The facet does not establish those identities by its PC argument alone, and does not replace the enclosing189240 parent gate.

## Implemented isolated detached-prefix extension

Following root's explicit integration decision, the candidate now embeds a genuine private EntityPrefix declared before derived resource members. Private Application construction receives the real shared OriginalApplicationEntities50LA and actual native session generation/object key. Existing initializeOriginalWreckComponent50LA initializes the real base and increments that shared counter before resource stages. The pinned Application entity owner survives final base cleanup.

Every resource/geometry stage validates the represented detached topology, reference1, exact owner/session identity, empty children/masks and source category/flags. No parent/sibling pointer is created. Destruction validates all these conditions before any derived resource destruction; unexpected refs/links fail-stop. It performs the represented104970 reference1→0 entry, releases derived resources, then the last-destroyed EntityPrefix decrements the actual Application counter once for1413E0. Constructor unwinding also cleans an actually initialized prefix. Native initialization failure changes no counter and remains terminal; successful prefix initialization is recorded as a real partial source effect even if a later stage rejects.

This resolves the prior candidate's missing own base prefix only. The future display is explicitly **native-host-scheduled**, and full original1AAD40/1891A0 parent forest/traversal remains unimplemented. Root owns this bounded host scheduling policy and actual race/view gates. The base kind denotes represented141300 fields, not a claim of generic-base virtual dispatch. No source-ready flag, invented hierarchy, blanket counter reset or renderer readiness was added.
