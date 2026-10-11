# Native B9D reader and tail component check

`tests/native/vehicle_b9d_native_check.cpp` checks all 256 B9C byte values
against both B9D bit-zero states in the canonical reader and damage tail:
512 reader cases and 512 tail cases. These counts describe source cases;
this addition has not yet been compiled or executed.

The test uses the existing native `runtime_startup50fl::Fixture` and its
authored PSG/VUT/MOT/PHY/COL inputs. No guest machine, original-word oracle,
renderer, body substep, race driver or generated reference snapshot is used.
The fixture's development context, two preserving-cut assertions and explicit
absent effect/probe owners are bounded component assumptions. They do not
establish that every intervening original stage or full cold constructor ran.

The reader expectation is B9C masked with `0xF8`, independently preserving
B9D. The healthy-damage tail expects either no transition at `0x2261A4` or
the real unsupported destruction boundary `0x225D10`, slot `0x88`, target
`0x2261B0`. Rejected calls must preserve the whole canonical frame. Expectations
are explicit literals rather than another invocation of the decision leaf.
The ordinary C++ allocation watch covers 1,031 wrapped frame calls, excluding
fixture construction and snapshots; it is not an allocator-wide audit.

## One focused build and run

After reserving the shared validation slot, run:

```sh
python3 tools/native_b9d/build_run_native_b9d.py \
  --repo-root "$REPO_ROOT" --assets-root "$NATIVE_ASSET_ROOT" \
  --output-dir "$FRESH_OUTPUT_DIRECTORY" --source-label "$VERIFIED_COMMIT" \
  --cxx /usr/bin/g++ --build-timeout 900 --test-timeout 120
```

The POSIX GCC/Clang convenience runner compiles the explicit 62-production-unit
manifest directly, then runs this one test once only on build success. The
whole `vehicle_frame49w.cpp` translation unit requires native body, terrain,
registration and late-tail dependencies even though this check does not invoke
them. No dead stripping, LTO, unresolved-symbol stubs, imported legacy runner,
compiler probe, retry or extra test is used. The 14 plain-data asset paths are
listed separately. There is no RES decoder/zlib dependency in this closure.
This runner is not a Windows/MSVC build or evidence of Windows validation.

The output directory must be new and outside the repository. Source/include,
asset, compiler and output hashes accompany commands, logs, timing and results.
Timeouts terminate the process group and require bounded cleanup before log
hashing. Input changes or unconfirmed cleanup block a successful identity.
The runner never reserves or expands the shared hourly test allowance.

## Remaining scope

This does not replace the broader historical optional-probe fixture, whose
recorded inputs and original-word adapters are separate. Dynamic-impact writer
publication, ordinary-reset lifetime, checkpoint reinspection, real all-AI cold
construction, noise-excluding banking and completed destruction remain outside
this check. It provides no gameplay, UI or visual proof.
