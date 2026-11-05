# Five settings from the native-adapted Wake selection

This packet continues the explicit native-adapted route only. Original Config and aggregate source PCs remain30492C, and the original Wake selection return remains absent. It starts from the reviewed completed native Wake load and stops at adapted2B9864, before packed render-state construction. No original EE heap/free completion, source selection return, Water completion or GPU readiness is established.

## Exact supported sequence

All five occurrences use the same live Wake section in the same Config. The caller ignores whether a key was found, so a genuinely missing key stores its literal default and continues. An unsupported found value stops native admission; it never becomes a missing/default result.

- MinForce: float call2B97D8, delay2B97DC destination383500, default bits0x3C23D70A. Same-App scalar lane30.
- AlphaReference: integer call2B97F4, delay2B97F8 destination383540, default50. Lane31.
- TexCoordScale: float call2B9818, delay2B981C destination383538, default bits0x3DCCCCCD. Lane32.
- FrothScale: float call2B983C, delay2B9840 destination443E20, default bits0x399D4952. Lane33.
- FrothStartSize: float call2B985C, delay2B9860 destination443E28, default bits0x40000000. Lane34.

No ELF initial value is substituted for a current word or caller default. In particular, file-backed MinForce initially has bits0x3BA3D70A, which differ from the getter's0x3C23D70A default. The five newly represented lanes start unknown until an admitted full overwrite. The trace establishes the cited addresses/call order, not all potential aliases.

## Explicit entry, rows and PC separation

Owner::readWakeSettingsNativeOnly requires WaterWakeLoadMode::nativeRetiredBackingOnly, completed native Wake loading, the same live Config/World/Application and owning thread, no withdrawn/closed native owner or outstanding aggregate borrows, and all original source cursors at30492C. It is one-shot and is not called by a normal source continuation. Repeated calls report alreadyAttempted with retained adapted PC and completed count, without replay.

NativeWakeGetters owns four new float occurrences and one new integer occurrence. Each has separate lookup/token/copy/conversion scratch and historical result state. No prior General getter result or row owner is reused as a Wake invocation. During the private synchronous getter scope only, field lookup selects live Wake rows, count, capacity and pointer metadata. It verifies the live table/data roles, uses normal token/folding/quote behavior, and rejects direct string-copy output on this numeric route. Outside that scope, the old source lookup still rejects retired General storage.

A scoped active flag routes every shared field/token/folding/decimal/integer helper PC update to the adapted getter cursor. The scope clears the flag on every return, including refusal. Config preflight rejects an active flag at any public boundary. Normal source callers retain their original pc_ sink; no transient source-PC advancement across30492C is introduced. The native loading cursor remains its historical158238 completion and retirement retains its historical157FE8 marker.

The reused getter value structures contain found/returned fields describing these native-adapted value computations only. They are not new original-source return receipts. wakeSelection_.returned remains absent throughout.

## Conversion and partial effects

Float conversion retains the previously reviewed bounded decimal path: optional sign, one or more integer digits, optional fractional digits, at most nine total digits and six fractional digits, no exponent, whitespace or suffix. It keeps current reentrancy/table admission and the original binary64-to-binary32 nearest-even/sticky ordering rather than host strtof. Unsupported syntax/unknown tables do not authorize an output store. Negative authored zero retains the established positive-zero result.

AlphaReference retains the actual ordered boolean probes: prefix T/F, full ON/OFF, prefix Y/N. False is a found value that stores0. Numeric fallback retains the admitted signed decimal subset, literal0's source base8 branch, separate current reentrancy/ctype/division-table gates, cutoff/cutlim and low32 result store. It is not a float cast, host atoi or an invented whole-word boolean parser.

Found float results write only at the158BC4 counterpart; missing float values at158BDC. Found integer results use the reached original store-PC counterpart, including boolean paths; missing AlphaReference writes50 at158B60. Earlier stores and histories survive a later lookup, conversion, global-owner or publication-capacity refusal. No rollback or fabricated source error/default is applied.

The existing source float/integer preflight bodies are factored into shared per-value validators. Old source loops retain their original section/precedence checks and defaults. Native validation supplies the live Wake count and new literal defaults, then separately enforces the five-occurrence order. Source-General histories remain bounded by their original sectionCounted_. Optional false/zero results remain engaged values, not unattempted states.

## Same-Application process globals and provenance

The existing SourceWaterProcessScalars owner expands from30 to35 lanes. These are current process words under the same Application, not per-Wake shadows or copied receipts. All existing thread/constructed/released/unknown-poison/publication-exhaustion gates remain. Full overwrites do not require inventing prior values. Provider withdrawal never resets current globals.

Every adapted store is followed, without fallible work, by nativeAdapted=true and one native publication increment in that same lane. Owner history records that publication. Later preflight checks current value and provenance when the publication is still the recorded one; newer publications retain the old history without pretending the old result is still current. Unknown process state continues to block future access rather than restoring defaults.

Explicit adapted read/write admission methods are restricted to new lanes30..34. Source-exact scalar reads, vector reads and source write-admission gates reject native-adapted provenance. Therefore these values cannot silently become source-exact receipts through a generic accessor. Existing startup lanes remain unchanged and untagged. A future joined producer/consumer must respect and explicitly handle this provenance; unsupported aliases must invalidate the shared owner.

Native Config/provider abort still validates all histories, withdraws stream custody and retains current process words. There is no source destructor/free, new stream read or new backing allocation in this five-getter slice.

## Next boundary and verification

At2B9864 the next family reads CURRENT383540, sign-extends and shifts64 bits, ORs0x5000D and stores a packed64-bit value into Water+2A0. It must not reuse the historical getter result as current authority. Later code also reads and multiplies CURRENT443B90 by16. Those operations are not implemented here.

Immediate predecessor: /tmp/racer-water-wake-load-20261105-0206, patcha48b2da015edba72933942da4c13eb5de1603bf09b870bf8d97e90eb4c23db8c, final manifeste50840d47395d74ba56a9f185ca5a1738f778251376564c71803c96ae6e659bd. All intervening Water packets remain unpublished. Published main remains e566993fc25f628ce5b4aa5def424c6003f53b75. The inherited narrow vector-header rebase requirement remains; preserve the pending checkpoint's vector-owner changes. This packet does not change that process-global header.

Verification is static source/ELF inspection, file-byte/Git-blob identity and strict text-patch reconstruction only. No compilation, syntax/type checks, builds, tests, probes, helper/model/target execution, emulation, runtime, gameplay or publication are performed or claimed.
