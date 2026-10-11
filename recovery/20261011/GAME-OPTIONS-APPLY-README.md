# Actual Game Options Accept: source-only candidate

Frozen October 11, 2026 against repository HEAD `5b190924c37372c5853f603bdbeb5795cdbdd3ef`.
The active tree was read only. No build, test, syntax check, project parser,
decoder, game, chip/guest emulator or guest instruction execution was run.
Root owns integration and every executable check.

## Delivered

`files/` contains eight exact replacement files and one new header.
`preimages/` contains the eight exact original files, and `changes.patch` is
the complete textual patch. No translation unit or build-list addition is needed.

- New `src/game/original_ui_game_options_apply50kg.h`
- `src/game/original_ui_route_decisions50kg.h`
- `platform/linux/native/original_ui_main_audio50kg.{h,cpp}`
- `platform/linux/native/original_ui_options_session50kg.{h,cpp}`
- `platform/linux/native/original_ui_frontend50kg.{h,cpp}`
- `tests/native/original_ui_main_entry50kg.h`

This opens the real screen12/page4 Accept route when its actual enabled audio
consumer is present. It is a process-owned profile commit plus the exact188740
sound-mode effect. It does not implement disk saves, memory cards, gameplay
handoff, two-player viewports, controller rumble or full positional surround.

## Exact transaction

The source page input1F50F0 event4 calls1F66F0 at1F5134. The native retained
proposal carries all nine typed profile destinations; the actual commit is
explicit source-order code, never a generic interpreter of that array:

1.1F672C replaces only profile11050 bit0 from local auto-save.
2.1F6744 writes soundMode1101C.
3.1F6760/1F677C/1F6798/1F67B4 write SFX/music/voice/announcer11020/24/28/2C.
4.1F67EC replaces only11050 bit1 from local vertical split.
5.1F6824 replaces only11050 bit2 from local vibration.
6.1F6830 tail-calls188740 through the actual frontend transport and shared
   `OriginalSoundMode50KG` installed on its real native mixer.
7.1F5158 stores the actual attached group1..8 into6E8.
8. The already admitted real manager publishes its same-screen page1 request,
   retaining the existing lifecycle7/next-tick transition behavior.
9. Immediate `notifyRouteAccepted` verifies the exact profile, applied mode
   revision, selection and actual manager request before SELECT1 at1F5188.
10.1F51A0 reaches267BE0. Admitted shared transportE00=-1 has the original
    no-work early return. No SDL drain, PCM clock step or synthetic IOP flush
    replaces it. An active original pending transport remains unsupported.

All unrelated flag bits are preserved. Defaults remains local. Cancel remains
its separate one-word6E8 store and BACK5 route, leaving settings/mode untouched.
Mode setters preserve active PCM voices, playback coordinates, current gains,
category ownership and output queues; later centered play/controls consume the
new mode. The newly started SELECT is therefore after the mode application.
There is no fabricated audible difference between centered stereo/surround.

## Concrete admission and no partial rejection

The application now passes the same retained `soundMode` pointer into frontend
cold input and the Options session. This is the pointer that the existing
AudioOutput binds to its dedicated actual mixer and checks at startup.

Accept's session preflight requires:

- valid real shared profile and loaded local settings;
- authoritative idle screen12/page4, actual open input aperture and attached group;
- enabled, current, unpaused real audio transport;
- exact shared sound owner identity on its actual epoch-checked mixer;
- sound fields matching the currently committed profile and a usable revision;
- concrete SELECT clip/voice ownership and centered routing capability;
- no queued update, stale proposal, uncommitted prior action or count overflow.

It captures the full profile, all local settings and exact sound revision.
The coordinator retains the original unmodified proposal, verifies its exact
source shape, admits the real prepared page1, promotes only its private decision
copy, and invokes the actual transition owner. The manager calls the concrete
commit only after destination admission and has no fallible work between its
successful callback and request publication. The commit is private to the
session and accessible only to the concrete frontend coordinator. A caller
holding a genuine proposal cannot manually promote its flags and invoke the
profile transaction without the manager path. The commit revalidates both the
original proposal and promoted copy, complete profile/local snapshot and audio
capability. No external callback or allocation occurs during profile/audio/group
writes. The sound setter can reject only before changing its own fields; a
strictly defensive profile restore covers that otherwise unreachable rejection.
No failed callback leaves a partially changed profile, mode, remembered group
or manager request.

A one-use committed marker prevents replay. After commit, notification compares
all nine destinations plus resulting full profile and actual sound revision.
The pending proposal is consumed before its sole SELECT dispatch. Input,
update, render and transition notifications invalidate pending proposals.
Missing/mismatched/muted/paused/stale audio remains unavailable before mutation;
no readiness callback or Boolean promise grants Accept.

The new `canPlayGameOptionsEffect` is read-only. It checks the transport's actual
clip, slot and ownership and the same centered mode projection used by its PCM
mixer. The play method also uses this preflight before stopping its old event.
All hot work is bounded, allocation-free native CPU logic; existing GPU,
source-time, audio scheduling and same-manager navigation are unchanged.

## Profile-only settings and existing later consumers

1F66F0 contains no volume setter, save request, viewport change or controller
feedback call. These settings are committed to the one application profile for
later source consumers. A missing gameplay destination does not invalidate this
source-complete frontend transaction. The other consumers and their exact current
limitations are mapped in `APPLICATION-CONSUMERS.md`.

The original frontend has its own effects group100/100 and music group40/100
at1B78D0/1B7944. Applying the four race-profile percentages to those frontend
voices would invent an immediate effect, so this patch deliberately retains
those source frontend levels.

## Diagnostics and root-owned verification

Reports now include committed profile mode, all four volumes and packed flags
on every page, the application sound revision, and the current Options session's
successful Accept count/last applied revision. The application revision survives
screen/session re-entry; the Options session count is scoped to that session.
The existing live local settings remain separately reported on page4.

No compile/runtime success is claimed. Root should verify:

- all three modes and every settings field commit with unrelated flag bits kept;
- Accept after Defaults, unchanged Accept, and all eight remembered groups;
- re-entry reloads committed settings; Cancel after edits leaves profile and
  sound revision unchanged; Defaults alone also leaves both unchanged;
- muted/missing/foreign owner, paused/stale transport, invalid values, foreign
  SELECT voice, malformed/forged/replayed decisions and intervening work reject
  before any profile/mode/request mutation;
- actual mode application precedes the new SELECT; old voices and PCM coordinates,
  group gains and device queues are not reset by the mode write;
- one same-manager page4->1 request, no GPU bank/publication or screen-switch
  change, unchanged scene/fire/camera owner and monotonic audio/source clocks;
- existing Main/menu directions, menu entry, Cancel and screen Back still work.

## Static evidence

`evidence/game-options-render-phases.txt` contains1F66F0..1F6834;
`game-options-entry-input-update.txt` contains exact event4/6 source order;
`profile-settings-apply.txt` contains188740's three mode branches;
`pending-transport-267BE0-267CB0.txt` proves the bounded no-work flush;
`ui-audio-create.words.txt` proves the distinct frontend group levels.
All were copied from existing authenticated recovery evidence, not regenerated
by executing a project decoder or guest code.
