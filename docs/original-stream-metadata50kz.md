# Original authored stream metadata

## Scope and status

`OriginalStreamMetadata50KZ` is a source-only candidate immutable asset owner
for the original stream library. It owns 1,743 rows in TXT ordinal order and
retains the JUK authoring independently of native PCM. It does not implement a
runtime provider, sample handle, mutable registry, sound/group lifetime,
voice, pitch scaling, sample decoding or playback.

This candidate was written against repository revision
`c840b434c6def9eaa68d1552c6116e59c6aeb437`, without repository changes or a branch.
The implementation and regression have **not been compiled or executed** by
the author. No syntax, sanitizer, compiler, project-script or guest-execution
probe was performed. Static source and original text/byte inspection alone
supports the specific observations below; exhaustive 1,743-row association
verification is prospective and must not be reported as already passed.

## Authentication and immutable ownership

Preparation accepts only an actual loaded `OriginalUiArchive50KG` whose
identity is the catalog's `appStart`. The archive owner authenticates original
compressed bytes and its logical members before publishing them. This leaf
requires the exact catalog identities and logical extents of:

- APPSTART member 4, `audio\Streams.txt`, logical offset 147456, 55,535 bytes,
  SHA256 `f231c4a527bd6fc5b1ad39337736fa6e072f39cec0313080d0cb5c07c0bb8aa6`.
- APPSTART member 5, `audio\streams.juk`, logical offset 221184, 518,223 bytes,
  SHA256 `0cee7d838db0bc3fbfa03513f759ede05c746a0a66f1adf8f5c12d2c402eee7b`.

There is no leaf admission API for loose bytes, caller-supplied member views,
or asserted hashes. In particular, the retained loose STREAMS files used for
static inspection are not an implementation input path. All row strings and
field objects are copied, so archive and temporary parser lifetimes cannot
invalidate published rows. The leaf is noncopyable, nonmoving and load-once.
Preparation is transactional; errors leave no partial row publication and an
empty owner remains retryable. Repeated preparation preserves published
storage and returns `alreadyPrepared`. Const lookups allocate nothing.

## Correct original block labels and association

The source investigation report used `Song1`/`song1` as shorthand. Direct
original JUK byte inspection corrects that literal spelling: its first block
label at member offset `0x13` is `song 1`, followed by `song 10`, `song 100`,
`song 1000`, and so on. The separating ASCII space is real. The parser and
regression use exact `song N`, never the report's unspaced shorthand.

Original backend setup `268690` supplies source registry ordinal N-1 while
iterating numerically over song N. The leaf preserves this relationship:

1. Read member 4 tokens in original ResourceList order using the existing
   source delimiter profile (space, CR, LF), with exactly 1,743 unique
   case-folded full names.
2. Use existing `decodeParameters48F` for authenticated member 5. Its strict
   BINARY1 decoder preserves source labels, strings and offsets and checks a
   reversible serialization. Require exactly 1,744 blocks, one `songs` count
   block and complete unique canonical `song 1` through `song 1743` coverage.
3. Require the single `numberofsongs` value to equal 1743, and require each
   song's five original fields: duration, filename, pitch, samplerate, size.
4. Associate numeric song N with TXT ordinal N-1 and require its actual
   Filename to match that row's source ASCII-case-insensitive full key.
   A mismatch fails atomically with row/member/offset diagnostics. No
   filename reordering, guessed prefix or separator rewriting conceals one.

The first TXT token and first JUK Filename were independently read as
`speech\Dax\Tracks\Coruscnt\DAXCRA23.VAG`. Selected later TXT ordinals were
located through original text reads and JUK text offsets through retained
original byte evidence. Exhaustive agreement awaits the prospective test.

The leaf retains three separate names:

- `authoredName`: exact original TXT token spelling.
- `sourceName`: ASCII lowercase with original separators, for source lookup.
- `pcmRequestName`: lowercase slash-normalized relative `.vag` key conforming
  to the existing AudioBank50HC key shape, solely for a later explicit caller.

The JUK's exact independent filename spelling is `authoredFilename.value`.
No `audio\` prefix is added. `find` uses full source names and ASCII case
folding only, so normalized PCM names, basenames, prefix matches and missing
extensions do not become source lookup aliases. `findForStreamRequest` also
requires the caller's actual authored stream flag. It does not certify a
caller-supplied flag or supply a PCM-name-to-source adapter.

This strict directory-key API does not reconstruct the original mutable
preload-request adapter. Original `2684E0` calls `266C50` at `268508`; after
the `strlen` call at `266C6C`, `266C78..266CA0` replaces exactly one final LF
(10) or CR (13) with NUL before comparison through `308F00` at `266CD0`.
The leaf accepts already-normalized source-directory keys and deliberately
rejects trailing CR, LF and CRLF. A later authentic preload consumer would
need its own explicitly reviewed request-normalization boundary; this leaf
neither mutates requests nor claims to reproduce that complete preload path.

## Source fields, offsets and numerical boundary

Each row owns all five `ParameterField48F` objects. Their `name` and `value`
retain exact original text; `entry_offset`, `value_offset`, and `value_bytes`
retain member-5 field provenance, including the value's terminating NUL.
The row also retains block-name/table-entry/block offsets, original block
size, and its separate member-4 TXT token offset. These are logical-member
relative offsets, not compressed-archive offsets. `songCountField` retains
the source `numberofsongs` field and its provenance too.

`authoredDuration.value` is the original decimal string. The separate
`admittedDurationSeconds` is a finite positive host IEEE binary32 admitted
with locale-independent `std::from_chars` directly to float, under the
bounded fixed unsigned-decimal profile. It is not milliseconds, not divided
by 1,000, and not regenerated from sample bytes or native frames.

**Original conversion bit parity is unverified.** Original `158B80` calls
`302E10` and `310440` before the single-precision store. Preserving the decimal
and its store boundary is not proof that the host converter reproduces those
original conversion routines on every input. The API name and tests retain
that distinction rather than labeling host float bits as recovered bits.

`originalPitch`, `originalSampleRate` and `originalSize` are separately named
uint32 values admitted from their respective positive integer source text.
Pitch is additionally bounded to 65535 because the later original stream
backing copy narrows it to a halfword. The leaf retains a uint32 source pitch;
it does not make it a float multiplier or infer it from sample rate.
No rate-to-pitch or size-to-duration formula is present.

Representative prospective expectations from static original evidence:

| TXT ordinal | Name suffix | Duration text | Pitch | Rate | Size | JUK Filename / Duration offsets |
|---:|---|---|---:|---:|---:|---|
| 0 | DAXCRA23.VAG | 13.055238 | 1881 | 22050 | 164496 | 0x9273 / 0x92a8 |
| 6 | DAXTAA01.VAG | 14.933333 | 1881 | 22050 | 188160 | 0x682b8 / 0x682ed |
| 1309 | pod13\BENRBA01.VAG | 1.222857 | 3763 | 44100 | 30816 | 0x20335 / 0x2035b |
| 1657 | Pod01\AN1RDL01.VAG | 0.632381 | 1881 | 22050 | 7968 | 0x39c1d / 0x39c42 |
| 1692 | ui_main.vag | 17.420952 | 1881 | 22050 | 219504 | 0x3c5ae / 0x3c5d8 |
| 1735 | combat01.vag | 20.389841 | 1881 | 22050 | 256912 | 0x3f9b1 / 0x3f9dd |

The original `numberofsongs` literal `1743` starts at JUK offset `0x7e84a`.
The last TXT token is `Music\Race\Practice\practc1.vag`, ordinal 1742.

AN1RDL01's authored `0.632381` remains separate from the previously documented
native finite endpoint 13,916 / 22,050 seconds. The regression compares these
quantities without loading or decoding any sample. Metadata may exist for
stereo assets unsupported by the current bounded PCM decoder: metadata
readiness never asserts PCM readiness.

## Prospective focused regression

`tests/native/original_stream_metadata50kz_check.cpp` accepts one path to the
exact retained APPSTART.RES. It is intended to check:

- Empty/unloaded and failed-authentication rejection, retry, load-once status.
- Every original TXT ordinal and token offset, every numeric spaced song
  association, and every Filename comparison without reordering.
- An independent raw-byte walk of the actual BINARY name and field tables,
  checking all five texts and offsets for every row, plus the count field.
- The original lexical physical order versus numeric semantic row order.
- All actual admitted source integer values and documented host float
  admission, without an original-conversion bit-equivalence assertion.
- Six independently recorded representative row/value/offset combinations,
  including both 22,050/1881 and 44,100/3763 authoring and AN1RDL01's duration.
- All 1,743 stable row addresses and owned fields after archive/input death.
- Stream-flag gating, case-fold lookup, range failures, embedded NULs and
  rejection of PCM-separator, basename, prefix and extension fallbacks, plus
  trailing CR/LF/CRLF rejection under the strict directory-key contract.

A later authorized centralized build/run should link this test and these
production units: `original_stream_metadata50kz.cpp`,
`original_ui_archive50kg.cpp`, `import_world48f.cpp`, `import_decode48c.cpp`,
plus zlib, using the existing native strict-FP C++20 flags. The candidate is
not registered in any build/source list and enables no runtime path. Test
creation is not a passing result; root owns any permitted execution slot.

## Boundaries still open

This leaf does not recover the absent original nonstream RPC provider's
commands 15/16 or their input values. It does not resolve ordinary 267-row
AudioStr samples or GC material positive metadata. It does not turn an
ordinal into a source sample ID, fill sound offsets, clear pending rows,
allocate a voice or claim original IOP or hardware output equivalence.

The existing mixer already advances by source sample rate divided by output
rate. Applying source pitch 1881/4096 there would introduce a second rate
scale. This candidate supplies no such scaling or any other playback change.
Authentic stream authoring can inform a later explicitly reviewed stream
logical-lifetime consumer; that integration is outside this leaf.
