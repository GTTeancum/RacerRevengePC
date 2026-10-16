# Gasgano held preparer: cumulative rounds one, two and five review

## Verdict

No blocking source defect found under the stated contract: parent-controlled stable input/staging trees, a trusted Python environment, and a separately reviewed native importer. Round five addresses the original canonical-output and unauthenticated-helper-loading gaps. It is appropriate for source integration consideration, not a claim that preparation or runtime admission has succeeded.

The worker reports observing helper SHA-256 `5b984aeb41409b735e9820a7bab6365c5336f46250e28f2ebd615a179d002f71` in its Windows checkout. After this review, the parent reported independently verifying that the current helper bytes match that exact digest by a read-only hash calculation. This resolves the current pin-reconciliation concern; this reviewer did not repeat the check. A later differing helper, including different line endings, will correctly fail closed and require deliberate source review.

## Reviewed sources and review limits

Read the original new-preparer hunk in `WORKER.patch`, the round-two preparer delta, the round-five patch/report/receipt, and the current complete `tools/assets/original_roster/prepare_original_roster.py`. The reviewed cumulative behavior is the original preparer plus rounds two and five; round-five alone does not add a missing preparer. Rounds three/four's separate IPUM changes are not substitutes for these held preparer hunks.

No patch was applied or reconstructed into production. No tests, builds, syntax checks, imports, project execution, decoders, preparation, gameplay, or SLUS execution occurred. No independent hash or applicability check was run. Only source/report reads and this review-file write were performed.

## Canonical destination and input policy

`absolute_plain` rejects lexical `..` before resolution. Relative paths are anchored to the process working directory. Windows drive-relative, root-relative, UNC and device paths are rejected; the component policy also rejects trailing dots/spaces, alternate-stream colons, wildcards/control characters and the listed DOS device names. This intentionally restricts acceptable path spellings rather than attempting full Windows namespace support.

`canonical_existing` inspects the lexical chain from root to leaf with `lstat` before `resolve` can conceal redirects. Symlinks and Windows reparse attributes are rejected at each component. Ancestors must be directories and file leaves must be regular files. Strict resolution must equal the inspected path. This is materially stronger than the original `.absolute()` and file-only checks.

`fresh_output` requires an existing canonical parent and an absent output leaf. `lstat` catches existing files, directories, and dangling links instead of conflating dangling links with absence. Only FileNotFoundError is treated as absence. The later exclusive `mkdir` creates only the leaf, without parent creation or reuse.

Before creation, the output must be disjoint in both ancestry directions from every direct input-file directory, the helper directory, optional SLUS directory and optional UI source root. This intentionally rejects even a fresh output child inside a protected directory. It does not infer and protect a complete disc root when that root was not provided; the parent's staging location must still be chosen outside other original-data trees.

Output canonicality and device/inode identity are rechecked around helper execution and later publication boundaries. Report and selected speech reads use the regular-file policy, and the selected taunts filename must be flat and unanchored. These checks improve accidental/redirection safety without constituting a filesystem sandbox.

## Authenticated helper source contract

The helper file is canonicalized and authenticated against a constant before module creation or execution. `types.ModuleType` plus `exec(compile(helper_bytes, ...), helper.__dict__)` executes exactly the authenticated buffer. There is no import-loader reread or cached `.pyc` selection. The module is given its canonical `__file__`; its module name is not `__main__`, so the helper's guarded disc-scanning main is not called. The current helper has ordinary standard-library imports and the required `prepare_pod(importer, resource, out, identities)` and `norm` functions; no API mismatch is apparent.

The helper is reauthenticated after preparation. The caller-supplied importer digest is syntax-checked and its binary is authenticated initially, again immediately before the helper call, and after preparation. This remains parent-supplied trust in a reviewed binary, not independent correctness evidence.

The actual helper contract is broader than the wrapper's final speech-path check: `prepare_pod` invokes the importer by pathname, consumes `import.json`, reads report-selected source/native files, validates original RES/PHY/speech pins and component/material bindings, and writes `import.json` and `pod.json`. It does not itself canonicalize all report-derived child paths. Therefore the wrapper's final checks are not protection against a malicious importer/report. The claimed safety requires the reviewed importer and stable staging tree, exactly as the round-five report states.

## Residual race and failure boundaries

There are path-based check/use windows between `lstat`, `resolve`, reads, `mkdir`, subprocess invocation and writes. A concurrent actor can potentially replace ancestors, importer bytes or child entries after a check. Device/inode checks detect some replacements but are not an atomic handle-based guarantee. Standard-library import resolution is also not sandboxed by authenticating the helper buffer.

No code-level blocker follows under the stated controlled-tree contract. If the intended requirement is race-resistant execution in attacker-writable trees, this implementation is insufficient and would need a different handle-based or isolated execution design.

Failures after leaf reservation can leave an empty or partial staging directory. The program does not delete or reuse it. `candidate.json` is last, but its write is not atomic and an interrupted partial file is not a successful receipt. The parent must reject incomplete staging and retry in a different fresh root.

## Cumulative evidence and no-admission behavior

The original wrapper authenticates POD07 RES, UIACCEPT, announcer, PHY and speech identity through fixed pins and the trusted helper. Gasgano name, three component roles and PHY member ownership are checked. Unresolved texture bindings remain visible rather than being silently substituted.

Round two adds optional paired UI-source-root and SLUS inputs. It hashes fixed named UI/audio files and the exact SLUS bytes, then reads four static words; it does not execute SLUS, invoke a guest, or produce authenticated native IPUM pixels. The evidence object explicitly sets `native_ipum_output_authenticated` false. Providing neither optional input remains supported; providing only one fails before helper execution.

After preparation, the candidate schema remains distinct from a roster schema, and `course_admitted`, `rider_ui_ready` and `runtime_validated` all remain false. It does not update Course/voice/domain lists or install the candidate into a live roster. The helper's `original_asset_inputs_available` field describes its imported pod assets, not full racer/UI/runtime closure. The receipt's pending voice text refers to prior static first/count evidence; it must not be interpreted as runtime voice integration, particularly when optional input authentication was omitted.

The wrapper only calls the helper's bounded pod preparation entry point, not its disc-scanning roster main. No driving/AI code changes appear. Original in-game AI policy and parent-owned rig/COL/handling/audio/material/GPU/UI closure remain intact.

## Parent integration notes

1. Integrate the cumulative preparer source in order; do not apply only round five to a nonexistent file or unintentionally integrate unrelated round-two evidence hunks.
2. Reconcile the reported exact helper pin with the intended reviewed source before a future authorized run. Source inspection here established API compatibility, not byte equality.
3. Keep input and staging paths ordinary, canonical, disjoint and parent-controlled. Use a fresh output leaf with an existing parent.
4. Treat future output as a staged candidate only. Successful preparation does not establish actual renderer, audio, handling, collision, UI or Course admission.

The parent subsequently reported integrating the held rounds one/two/five preparer source and relocating original evidence under `recovery/20261014/gasgano/original-evidence`, with documentation references updated. Those integration actions are parent-reported, not an independently checked application result here.

No source-blocking issue found; future execution remains gated by authorization and the stated trusted-environment assumptions. The parent-reported exact current helper hash match resolves the outstanding identity concern for this checkout.
