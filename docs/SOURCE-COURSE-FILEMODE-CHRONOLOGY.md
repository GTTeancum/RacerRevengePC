# Course FileManager mode chronology correction

Source-only correction, 2026-11-04. This packet changes documentation only. It does not add a mode writer, manufacture a failure return, alter Application/Video/dispatcher source, or advance music. No build, syntax check, test, probe, runtime or emulation was performed.

## Corrected finding

The earlier music-open packet mislabeled187118/1872B8 as UICourse cleanup. They belong to rider-profile startup1870B0, called at17F97C, loading bank `UI\podphys.res` and the26 `pods\pod%02d\pod%02d.phy` ability files. The archive and caller are different from UICourse. Retained registration success controls its temporary mode change and restoration;1872A0 bank-close return does not control1872B8. Native authored-profile parsing does not enact that original FileManager history.

This correction supersedes the erroneous attribution in the preceding music packet's FILEMANAGER-AUDIT.md, SOURCE-MUSIC-CONFIG-OPEN.md, NEXT-BOUNDARIES.md and related prose. The prior artifact is retained unchanged for provenance. Its implementation's actual-current-mode branching remains valid: it neither sets170 nor assumes either branch merely from file existence.

## Actual UICourse mode writer

- Course1D0140 calls generic manager loader1CCBA0 at1D0150, with t0=0 at1D0154. The loader retains t0 in s1 and initializes s0=0.
- Successful bank12CEA0 at1CCC44 sets s0=1 and manager2A8=1. Only then1CCC64 calls132A20(1), storing FileManager170=0.
- Normal completion must traverse original lights, fog, widgets, fog attachment, Page factory,1CDA70 and conditional global callback. It destroys Config298 at1CCE4C, nulls298, calls manager v78 and vA4. When retained s1=0 and s0!=0,1CCE94 calls132A20(0), storing170=1. If s0=0, the source takes vAC instead.
- A genuine source failure branch enters1CCEC0. It destroys Config, nulls298, then for s1=0 calls forced bank12E5E0 at1CCEE0 and writes2A8=0. Only retained s0!=0 permits restore1CCEF8. A native bounded refusal is not the required source return0.

The restoration belongs to loader completion or its genuine failure suffix, not to Course departure.

## Genuine departure and destruction

Flags processing at1465D0 first needs actual Video v24/v20 and App17DCE0. App2D8 is the dispatcher, not the Course manager.17DCE0 detaches it, calls its virtual10 and nulls App2D8 only after return. Entity142590 releases real children/siblings, then104970 decrements actual references and invokes virtual08 only at zero. It cannot force externally held children to zero.

Course's final vtable3CFBA0 has v10=142590 and v08=1D00D0. Its destructor calls1CCAA0, which releases SearchList2A0 and Fog190, calls Light148110, frees auxiliary table29C, formats the bank path, closes the bank with12E5E0 at1CCB50, then calls base2A3AD0. None of this restores FileManager170. The base Video+44 write is a different field. Dispatcher1B71A0 has its own subsequent retirement; it is not interchangeable with Course lifetime.

## Current source boundary

The live CPU Course Session is not OriginalUiCourseOwnedLifecycle50KG. Storage::retireSource has no Course branch; Session has no teardownSource; collector drain releases frame work only. Native resource cleanup cannot certify the original dispatcher/manager tree release.

The source-owned Course lifecycle is detached and has no current issuer. It retains actual constructor/Config/search/light/fog/widget/Page components, but the deepest Page Control1 routing component stops at2A71D4 while reading Widget9/Soultree+5C, whose source value is unknown. No Control1/Page/factory/load return exists. No native receipt may substitute for those source predecessors.

No represented genuine failure currently reaches1CCEC0 either. The authenticated Course archive contains the exact `UI\UICourse\ui.dat`; its narrow absent-file classification cannot be fabricated for this cohort. Current open failures stop at1CCCAC. Lights/fog allocation and parser bounds stop before unrepresented operations, not as genuine caller return0. The router unknown-field stop is also not return0.

Therefore this packet deliberately implements no mode restoration. The next relevant source work is the genuine Widget9+5C producer/Control1/Page path and remaining loader returns. A separate destruction implementation would still require real published owner topology, references and forced-bank semantics, and would not itself establish1CCE94.

## Evidence and integration

Both independent audits retain exact function/caller evidence and source ownership findings. SOURCE-WORDS.txt and manifest record original ELF ranges, hash and exact dependency snapshots. The corrected music document uses the prior reviewed music-open afterimage as its exact preimage. No code or shared source was edited, and no successful source runtime occurrence is claimed.
