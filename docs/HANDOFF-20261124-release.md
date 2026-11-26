# Racer Revenge PC maintainer handoff · 24 November 2026

> **November 25 follow-up:** [0.9.1](https://github.com/GTTeancum/RacerRevengePC/releases/tag/v0.9.1) supersedes the 0.9.0 download described below. Its release source is `911d9fc8062c9c14d2d98398a70f29099818dde8`. Use the 0.9.1 release notes for install/upgrade instructions and dependencies.

- [#1](https://github.com/GTTeancum/RacerRevengePC/issues/1): closed for the NVIDIA **race-load** shader crash. If that signature returns on 0.9.1, open a new issue with a log. The separate post-finish return-to-menu crash and low frame rate came from an RTX 2080 SUPER test of **binary-patched 0.9.0**; they are not confirmed fixed or reproduced on public 0.9.1. [Reporter details](https://github.com/GTTeancum/RacerRevengePC/issues/1#issuecomment-7163985862).
- [#3](https://github.com/GTTeancum/RacerRevengePC/issues/3): separate **window startup/VSync** problem. A community tester confirmed launch worked with the experimental 0.9.1 build. Do not treat that result as coverage of the post-finish crash or performance.

Start new development from GitHub `main`. The remainder of this note records the **0.9.0** Windows x64 release. A player can install and run it offline with their game inputs. [Player instructions](../README.md) · [Release downloads and trailer](https://github.com/GTTeancum/RacerRevengePC/releases/tag/v0.9.0)

## Which source matches the download?

The **v0.9.0** tag identifies the original release commit `f4e9ed9`. The installer on that release was replaced in place to make Pugwis immediately playable. Use **5410ecc** for the corrected installer source. The native game inside both installers is build **cr-181**. [Corrected installer source](https://github.com/GTTeancum/RacerRevengePC/tree/5410ecc72090b749ce2f741da07a76221e896a5b)

| Artifact | SHA-256 |
| --- | --- |
| Released Windows ZIP | `8119b46f34fc3b0a7dcd7a4b802aa5aa59e7e00e6e39cb556a622a2b18961b5e` |
| Installed game executable | `165c1f9608027926f07c3b839d3b9e49fdeaeb83bfe519a2a77e5a1161e6e656` |

## Installation and verification

| Mode | Player supplies | Clean install result |
| --- | --- | --- |
| Stock | USA Racer Revenge PS2 ISO or extracted disc root containing SLUS_202.68 | Original game and playable Pugwis. One active DLC folder. |
| Full | Same PS2 disc, Racer Arcade chip-set ZIP and USA Episode I Racer N64 ROM | Stock contents plus six Arcade pilots, two Arcade courses and eight N64 courses. Seventeen active DLC folders. |

Both optional games are required together. The GUI installer verifies the inputs, converts them locally using bundled tools and publishes the staged installation after success. Both modes install Pugwis in `DLC/demolition-pugwis` and keep a separate editable copy in `Templates/demolition-pugwis`. Saved video, keyboard and per-player controller settings apply on the next game launch. Players can start the installed executable directly.

The corrected public download passed SHA-256 verification. Fresh stock and full installs passed, with identical contents in the four corresponding active and template files. Pugwis was selected and raced from both installations. Each single-ROM combination was rejected in release testing. The release trailer shows native 4:3, 16:9 and 21:9 footage, Pugwis in character select and DLC races. The trailer export is 1080p. Separate game captures cover 3840×2160.

The eight N64 courses use 2.2 feet per original unit. Add-on tracks use the stock loading presentation. The shared racer code handles anti-gravity recovery. The player AI pilot remains opt-in via `--ai-pilot`. For remaining fidelity limits and the evidence from actual two-player races, consult the later dated entries in the engineering notes. [Oovo IV recovery](oovo-iv-recovery.md) · [N64 hazards](handoff-20261123/notes/n64-hazards-STATUS.txt) · [Versus verification](handoff-20261123/notes/two-player-plan.txt)

## Developer recovery

The public repository contains the Windows port, installer and offline converter sources. Some original-data-derived build fixtures and reference assets are private. A fresh source-only rebuild of cr-181 was not verified for this handoff. Keep the existing workstation repository and legally held inputs if moving development. [Launcher build instructions](../platform/windows/launcher/README.md) · [Windows build guide](../platform/windows/native/README-interactive50du.md) · [Local input exclusions](source-only-exclusions.md)

| Repository-relative workstation path | What to preserve |
| --- | --- |
| `work/scratch/release090-source` | Publication worktree. The root checkout and `.claude/worktrees/cr-main` contain divergent local history and edits. |
| `build/cr-181` | Validated game executable, receipt and linker response file. The response file references objects in earlier build folders. |
| `work/scratch/pugwis-installer090` | Corrected installer, verification script, release instructions and fresh stock/full install evidence. |
| `work/scratch/release090` | Prepared converter ZIP, embedded Python runtime, installation harness and original release packaging inputs. |
| `work/scratch/dlc/stock-install` and `work/scratch/dlc/roots/testers-cr21/DLC` | Local reference game data and package inputs, including Pugwis. |
| `work/scratch/trailer090` | Release trailer master, capture sources, editing and score scripts. |

Keep one common gameplay path across racers and courses. Keep project files, temporary files and native captures inside the repository. Use game-process input and native capture for testing. Inspect the actual rendered output. Clean up after each test and build round, keeping the objects referenced by the current linker response file and the private inputs needed for rebuilding. [Governing design rules](../CLAUDE.md) · [Historical engineering record and cleanup procedure](HANDOFF-20261123-codex.md)

## Account continuity

The GitHub owner is **GTTeancum**. A public clone and the release downloads can be used without this chat. Publishing updates requires GitHub write access. If the GitHub owner account itself is closing, arrange an ownership transfer while its owner can still authorize it.

The local offline handoff ZIP contains a Git bundle of public main and the release tag, the release attachments, checksums and restore instructions. A normal Git clone omits release attachments. Keep the handoff ZIP and the separately held private development inputs accessible to the next maintainer.
