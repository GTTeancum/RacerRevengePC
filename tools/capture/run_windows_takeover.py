"""Stage a verified build and exercise its existing in-process UI/AI route.

No host input or desktop capture. Images come from the game's framebuffer.
The receipt is execution evidence only; images/audio still need inspection.
"""
import argparse
import ctypes
import hashlib
import json
import os
import re
from pathlib import Path
import shutil
import struct
import subprocess
import time
import zlib

ROOT = Path(__file__).resolve().parents[2]
# A git worktree under <repo>\.claude\worktrees\<name> runs against the main
# repository's build, evidence and scratch folders (one shared tree of outputs).
if ROOT.parent.name == 'worktrees' and ROOT.parent.parent.name == '.claude':
    ROOT = ROOT.parents[2]

# The 23 original racer ids (no pods 5, 6, 13).
ORIGINAL_RACERS = [r for r in range(1, 27) if r not in (5, 6, 13)]
# Free Play routes start from a fresh profile (Main slot 1, Rider19), so a
# seeded racer applies to the tournament route only.
PLAYER_RACER_ROUTES = ('main-tournament-course-ta',)
# Add-on riders join the free-play (mode 0) Rider carousel only; the plain
# free-play route starts from a fresh profile on Rider19 and races TA.
ADDON_RIDER_ROUTES = ('main-rider-course-ta',)


# Tournament course order (career progress219C) -> course code.
TOURNAMENT_ORDER = ('TA', 'SA', 'MA', 'GA', 'RA', 'TB', 'MB', 'GB', 'SC', 'MC', 'GC', 'RC', 'TC')


def seeded_tournament_track(data):
    """Course code the seed sends a tournament run to: the Tournament-remembered
    racer's (6FC) own career progress. None if the file is not readable."""
    try:
        u32 = lambda at: struct.unpack_from('<I', data, at)[0]
        if u32(0) != 0x42505252 or u32(4) != 1:
            return None
        unlock = 16 + 12
        remembered = unlock + 27 * 4 * 2
        careers = remembered + 4 * 4 + 7 * 4
        career_size = 16 + 27 * (6 + 13 + 13) * 4
        racer = u32(remembered + 4 * 3)
        progress = u32(careers + racer * career_size + 12)
        return TOURNAMENT_ORDER[progress] if 0 <= progress < len(TOURNAMENT_ORDER) else None
    except (struct.error, IndexError):
        return None


def seed_player_racer(data, racer):
    """Return progress-v1.bin bytes in which `racer` is unlocked, remembered
    for Free Play (6F0) and Tournament (6FC), and has reached the same
    tournament course as the seed's Anakin19 career (progress byte and that
    career's own diagonal medals). Layout: saveNativeBetaProgress in
    platform/linux/native/original_ui_rider_profile50kg.cpp. Only fields the
    game's own unlock grant and Rider Confirm write are touched (unlock10FAC,
    remembered); hidden0730 stays as the game left it. The physics identity
    word is kept, so the seed must be a file the game itself wrote."""
    d = bytearray(data)
    u32 = lambda at: struct.unpack_from('<I', d, at)[0]
    put = lambda at, v: struct.pack_into('<I', d, at, v)
    if u32(0) != 0x42505252 or u32(4) != 1 or u32(8) != len(d) - 16 or \
            u32(12) != zlib.crc32(bytes(d[16:])) & 0xffffffff:
        raise ValueError('Seed save is not an intact progress-v1 file')
    unlock = 16 + 12
    remembered = unlock + 27 * 4 * 2
    careers = remembered + 4 * 4 + 7 * 4
    pod_size = (6 + 13 + 13) * 4
    career_size = 16 + 27 * pod_size
    career = lambda c: careers + c * career_size
    medal = lambda c, p, o: career(c) + 16 + p * pod_size + 24 + o * 4
    put(unlock + 4 * racer, 1)
    put(remembered + 4 * 0, racer)  # 6F0, mode0 Free Play
    put(remembered + 4 * 3, racer)  # 6FC, mode2 Tournament
    if racer != 19:
        put(career(racer) + 12, u32(career(19) + 12))  # progress219C
        for o in range(13):
            put(medal(racer, racer, o), u32(medal(19, 19, o)))
    put(8, len(d) - 16)
    put(12, zlib.crc32(bytes(d[16:])) & 0xffffffff)
    return bytes(d)


def seed_dlc_hall(pod_key):
    """DLC save side file (schema rrpc-dlc-progress/1, the layout DlcProgress50DL
    ::serialise writes, src/assets/dlc_progress50dl.cpp) holding one add-on pod
    record on stock TA, faster than any stock row: lap 1.00 s, race 5.00 s.
    Empty slots are null; knockouts stay empty. The stock save is not touched."""
    entry = lambda t: {'time': t, 'name': 'EYECHECK', 'racer': pod_key}
    return {'schema': 'rrpc-dlc-progress/1', 'courses': {},
            'stock_course_addon_pod_records': {'TA': {'lap': [entry(1.0), None, None],
                                                      'race': [entry(5.0), None, None],
                                                      'knockouts': [None, None, None]}},
            'pods': {pod_key: {'unlocked': True}}, 'careers': {}}


def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def inside(path):
    path = path.resolve()
    if not path.is_relative_to(ROOT):
        raise ValueError(f'Project output must remain in repository: {path}')
    return path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--assets', type=Path, required=True)
    parser.add_argument('--sdl', type=Path, required=True)
    parser.add_argument('--route', choices=['main-dud04-course-ga', 'main-ben02-course-ga',
                        'main-alder10-course-ma', 'main-ody11-course-ma',
                        'main-rider-course-ta', 'main-tournament-course-ta',
                        'main-mawhonic09-course-ra', 'none'], default='main-dud04-course-ga')
    parser.add_argument('--logs-only', action='store_true', default=True, help='Record stage and timing receipts without pixel readback or image files')
    parser.add_argument('--seconds', type=int, default=180)
    parser.add_argument('--profile-performance', action='store_true',
                        help='Enable existing CPU/render/display phase measurements')
    parser.add_argument('--profile-gpu-stages', action='store_true',
                        help='Serialize rendering stages for diagnosis; not production FPS measurement')
    parser.add_argument('--camera-trace', action='store_true',
                        help='Log actual attached-camera poses without changing camera behavior')
    parser.add_argument('--restart-once', action='store_true',
                        help='Exercise native race teardown/rebuild after first race evidence; not Pause-menu input')
    parser.add_argument('--renderer-switch', action='append', default=[],
                        choices=['RR_EXACT_PS2_SPECULAR', 'RR_GPU_FINISH_EACH_FRAME', 'RR_ORIGINAL_ASPECT',
                                 'RR_TRACE_ORIGINAL_UI_STAGES', 'RR_TRACE_PHASES',
                                 'RR_GARAGE_PROOF_SHADOW', 'RR_GARAGE_KERNEL_SHADOW',
                                 'RR_GARAGE_LEGACY_INDEX', 'RR_GARAGE_LEGACY_FINISH',
                                 'RR_GARAGE_LEGACY_PARALLEL', 'RR_GARAGE_LEGACY_BLEND',
                                 'RR_GARAGE_COVERAGE_PER_TRIANGLE', 'RR_GARAGE_COVERAGE_VERIFY',
                                 'RR_CPU_POD_VERTICES', 'RR_TEST_PAIR_ROUTE_COMPARE', 'RR_COVERAGE_PAIR_EXACT', 'RR_GPU_POD_VERTEX_CHECK',
                                'RR_COLLISION_ADMISSION_CHECK',
                                 'RR_SHADOW_TRACE'],
                        help='Set one renderer comparison switch to 1 (repeatable); recorded in execution.json')
    parser.add_argument('--track', default='TA', help='Direct-start track when --route none')
    parser.add_argument('--all-ai', action='store_true',
                        help='Original AI drives every pod, camera on pod 0 (spectator)')
    parser.add_argument('--race-full', action='store_true',
                        help='Keep racing until the watchdog budget instead of stopping after FPS evidence')
    parser.add_argument('--steer-invert', action='store_true',
                        help='Invert the test steering toward the next gate')
    parser.add_argument('--input-device', choices=['keyboard', 'controller'],
                        help='Force the human input device (controller uses the real connected pad)')
    parser.add_argument('--pad-hold-a', action='store_true',
                        help='Hold pad A through the green light (focused window), release 2 s after green')
    parser.add_argument('--stop-start', action='store_true',
                        help='With --hold-throttle: 4 s throttle / 3 s coast, repeating')
    parser.add_argument('--maximized', action='store_true',
                        help='Open the (resizable, --window) test window maximized')
    parser.add_argument('--no-fxaa', action='store_true', help='Turn FXAA off for this run')
    parser.add_argument('--destroy', help='Destroy racers during the race: slot@seconds[,slot@seconds] (0 = player)')
    parser.add_argument('--env', action='append', default=[],
                        help='Test-only: pass RR_NAME=value to the game (repeatable; other RR_ variables are not inherited)')
    parser.add_argument('--undertrack', help='Test-only: drop racers below the track: slot@seconds[,slot@seconds] (0 = player)')
    parser.add_argument('--camera-cycle', action='store_true',
                        help='Tap the view key (C) 1 s after green and every 15 s (reaches the follow camera)')
    parser.add_argument('--results-confirm', type=float, default=0.0,
                        help='Tap Enter every N seconds once the Results replay is running (0 = off)')
    parser.add_argument('--ieee-subnormals', action='store_true',
                        help='Reproduction only: disable the PS2 no-subnormal float mode')
    parser.add_argument('--window', help='Open a resizable window at WIDTHxHEIGHT (widescreen tests)')
    parser.add_argument('--hold-throttle', action='store_true',
                        help='Hold the human keyboard throttle once the race is green (launch test)')
    parser.add_argument('--garage-close-frame', type=int, default=-1,
                        help='Test only: close the window at native frame N of a race-return Garage (0 = before its first frame)')
    parser.add_argument('--repair', action='store_true',
                        help='With --hold-throttle: hold the repair key (R) 2 s of every 6 s')
    parser.add_argument('--unstick', action='store_true',
                        help='With --hold-throttle: tap the manual reset key (T) when the pod is wedged for 3 s')
    parser.add_argument('--seed-save', type=Path,
                        help='Test only: copy this prepared native progress file (progress-v1.bin format) into the run save directory before launch')
    parser.add_argument('--player-racer', type=int, choices=ORIGINAL_RACERS,
                        help='Test only: unlock and remember this original racer in the seeded save '
                             '(requires --seed-save; main-tournament-course-ta only). The game '
                             'loads it through its own Rider screen; the route only checks it opened on it')
    # PORT EXTENSION, NOT ORIGINAL (DLC phase 2): add-on riders and courses.
    parser.add_argument('--addon-rider', metavar='PACKAGE/ITEM',
                        help='Test only (DLC): on --route main-rider-course-ta, press Rider Next until this '
                             'admitted add-on rider is highlighted, then Confirm (sets RR_TEST_ADDON_RIDER). '
                             'Needs the package in the DLC root, e.g. --env RR_DLC_ROOT=<folder>\\DLC')
    parser.add_argument('--eyecheck-dlc-hall', metavar='PACKAGE/ITEM',
                        help='Eye check (DLC Hall merge): seed saves\\dlc-progress-v1.json with a TA record of this '
                             'admitted add-on pod (lap 1.00 s, race 5.00 s, name EYECHECK; faster than any stock TA '
                             'row), then drive Main -> Options -> Hall of Fame (Options slot 4) -> TA in process '
                             '(route main-options-hall-ta, no manual input), write that Hall frame as an image '
                             '(logs-only off for this run) and hold there until --seconds runs out (use 90). '
                             'Needs --env RR_DLC_ROOT=<folder with the package>. Name the output eyecheck-dlc-hall-...')
    parser.add_argument('--hold-rider', action='store_true',
                        help='Eye check: stop on the Rider screen (no Confirm) and write that Rider frame as an image '
                             '(turns logs-only off for this run). With --addon-rider it holds with that add-on '
                             'highlighted (RR_TEST_ADDON_RIDER_HOLD=1); without it, on the stock rider the route lands '
                             'on (RR_TEST_RIDER_HOLD=1; --route main-rider-course-ta lands on Rider19). The run then '
                             'idles until its wall budget; use a short --seconds (90)')
    parser.add_argument('--addon-course', metavar='PACKAGE/ITEM',
                        help='Test only (DLC): add-on course (sets RR_TEST_ADDON_COURSE). Not implemented in this '
                             'build: the variable is passed through and nothing reads it yet')
    parser.add_argument('--gpu-vertex-check', type=int, default=0,
                        help='Diagnostic: compare GPU pod vertices with the CPU reference every N rendered frames')
    parser.add_argument('--keep-binaries', action='store_true',
                        help='Keep the staged program/DLL copies and audio recording after a logs-only run '
                             '(by default they are deleted when the run ends; logs are always kept)')
    args = parser.parse_args()
    if os.name != 'nt' or not 10 <= args.seconds <= 1800:
        parser.error('Windows required; watchdog must be 10..1800 seconds (chained-course runs need up to 30 min)')
    if args.player_racer is not None:
        if not args.seed_save:
            parser.error('--player-racer needs --seed-save: a progress file the game itself wrote')
        if args.route not in PLAYER_RACER_ROUTES:
            parser.error('--player-racer works only with --route ' + ' / '.join(PLAYER_RACER_ROUTES))
    addon_key = re.compile(r'[a-z0-9][a-z0-9-]{1,46}/[a-z0-9][a-z0-9-]{1,46}')
    if args.addon_rider is not None:
        if not addon_key.fullmatch(args.addon_rider):
            parser.error('--addon-rider expects <package-id>/<item-id>, e.g. arcade-ark-roose/a2r-ark')
        if args.route not in ADDON_RIDER_ROUTES:
            parser.error('--addon-rider works only with --route ' + ' / '.join(ADDON_RIDER_ROUTES))
        if args.player_racer is not None:
            parser.error('--addon-rider and --player-racer cannot be combined')
    if args.hold_rider and args.route not in ADDON_RIDER_ROUTES:
        parser.error('--hold-rider works only with --route ' + ' / '.join(ADDON_RIDER_ROUTES))
    if args.eyecheck_dlc_hall is not None:
        if not addon_key.fullmatch(args.eyecheck_dlc_hall):
            parser.error('--eyecheck-dlc-hall expects <package-id>/<item-id>, e.g. arcade-ark-roose/a2r-ark')
        if args.addon_rider is not None or args.player_racer is not None:
            parser.error('--eyecheck-dlc-hall runs its own Main/Options/Hall route; it cannot be combined with other test routes')
        args.route = 'main-options-hall-ta'   # in-process HallRoute (no manual input)
        if not any(item.startswith('RR_DLC_ROOT=') for item in args.env):
            parser.error('--eyecheck-dlc-hall needs --env RR_DLC_ROOT=<folder with the package>')
    if args.addon_course is not None:
        if not addon_key.fullmatch(args.addon_course):
            parser.error('--addon-course expects <package-id>/<item-id>, e.g. n64-vengeance/nv')
        print('addon_course: --addon-course is not implemented in this build; RR_TEST_ADDON_COURSE is '
              'passed through and the run uses the route\'s own stock course', flush=True)
    build, output = inside(args.build), inside(args.output)
    assets, sdl = args.assets.resolve(), args.sdl.resolve()
    receipt = json.loads((build / 'build-receipt.json').read_text(encoding='utf-8-sig'))
    if receipt['status'] != 'built-not-run':
        raise ValueError('A successful complete build receipt is required')
    executable = inside(Path(receipt['executable']))
    if digest(executable).lower() != receipt['executableSha256'].lower():
        raise ValueError('Executable differs from the build receipt')
    for name in ['SLUS_202.68', 'APPSTART.RES', 'LOC.RES']:
        if not (assets / name).is_file():
            raise ValueError(f'Missing original asset: {name}')
    runtime = [sdl, assets / 'avcodec-61.dll', assets / 'avutil-59.dll',
               assets / 'libwinpthread-1.dll']
    if sdl.name != 'SDL2.dll' or not all(p.is_file() for p in runtime):
        raise ValueError('SDL2 and the original installation codec DLLs are required')
    output.mkdir(parents=True, exist_ok=False)
    capture = output / 'capture'
    capture.mkdir()
    temporary = output / 'temp'
    temporary.mkdir()
    saves = output / 'saves'
    saves.mkdir()
    if args.seed_save:
        seed = args.seed_save.resolve()
        if not seed.is_file():
            raise ValueError(f'Missing seed save: {seed}')
        if args.player_racer is None:
            shutil.copy2(seed, saves / 'progress-v1.bin')
        else:
            (saves / 'progress-v1.bin').write_bytes(seed_player_racer(seed.read_bytes(), args.player_racer))
    if args.eyecheck_dlc_hall is not None:
        (saves / 'dlc-progress-v1.json').write_text(json.dumps(seed_dlc_hall(args.eyecheck_dlc_hall), indent=2) + '\n')
    staged = output / 'RacerRevengePC.exe'
    shutil.copy2(executable, staged)
    for dll in runtime:
        shutil.copy2(dll, output / dll.name)
    env = {k: v for k, v in os.environ.items()
           if not k.upper().startswith(('RR_', 'SDL_', 'MESA_', 'LIBGL_'))}
    env.update(TEMP=str(temporary), TMP=str(temporary),
               RR_NATIVE_SAVE_DIR=str(saves), RR_AUDIO_WAV=str(output / 'audio.wav'),
               RR_WINDOW_CAPTURE_DIR=str(capture), RR_WINDOW_CAPTURE_AUTO='1',
               RR_UNATTENDED_CAPTURE='1', RR_UNATTENDED_UI_ROUTE=args.route,
               RR_UNATTENDED_SECONDS=str(max(1, args.seconds - 5)),
               RR_ORIGINAL_AI_ALL_RACERS='0', RR_TRACK_FPS_BASELINE='1',
               RR_NATIVE_FIXED_WINDOW='1',
               RR_TEST_MUTE_OUTPUT='1',  # Tests are silent; player builds are not.
               RR_VIDEO_MAXIMIZED='0')  # Never cover the desktop unless asked.
    if args.eyecheck_dlc_hall is not None:
        # In-process route Main -> Options -> Hall TA, then hold (PORT EXTENSION,
        # tests/native/original_ui_capture_navigation50la.h HallRoute).
        env['RR_UNATTENDED_UI_ROUTE'] = 'main-options-hall-ta'
    # A --hold-rider / --eyecheck-dlc-hall run writes its held frame as an image (eye check).
    logs_only = args.logs_only and not args.hold_rider and args.eyecheck_dlc_hall is None
    if logs_only:
        env['RR_EVIDENCE_LOGS_ONLY'] = '1'
    if args.camera_trace:
        env['RR_CAMERA_POSE_TRACE'] = '1'
    # Test-only (PC port, not original): every injected input below is
    # in-process (RR_TEST_* read by the game), so the run must not depend on
    # the game window being in front. RR_TEST_IGNORE_FOCUS stops focus loss
    # from pausing or releasing those inputs; a real minimise/close still does.
    injected = (args.hold_throttle or args.steer_invert or args.camera_cycle or args.results_confirm or
                args.destroy or args.undertrack or getattr(args, 'pad_hold_a', False) or args.player_racer is not None)
    if injected:
        env['RR_TEST_IGNORE_FOCUS'] = '1'
    if args.hold_throttle:
        env['RR_TEST_HOLD_THROTTLE'] = '1'
    if args.repair:
        env['RR_TEST_REPAIR'] = '1'
    if args.unstick:
        env['RR_TEST_UNSTICK'] = '1'
    if args.stop_start:
        env['RR_TEST_STOP_START'] = '1'
    if args.maximized:
        env['RR_VIDEO_MAXIMIZED'] = '1'
    if args.no_fxaa:
        env['RR_VIDEO_FXAA'] = '0'
    if args.destroy:
        env['RR_TEST_DESTROY'] = args.destroy
    if args.undertrack:
        env['RR_TEST_UNDERTRACK'] = args.undertrack
    for item in args.env:
        key, sep, value = item.partition('=')
        if not sep or not key.startswith('RR_'):
            parser.error('--env expects RR_NAME=value')
        env[key] = value
    if args.results_confirm:
        env['RR_TEST_RESULTS_CONFIRM'] = str(args.results_confirm)
    if args.camera_cycle:
        env['RR_TEST_CAMERA_CYCLE'] = '1'
    if args.ieee_subnormals:
        env['RR_TEST_IEEE_SUBNORMALS'] = '1'
    if args.garage_close_frame >= 0:
        env['RR_TEST_CLOSE_GARAGE_AT_FRAME'] = str(args.garage_close_frame)
    if args.gpu_vertex_check > 0:
        env['RR_GPU_POD_VERTEX_CHECK'] = str(args.gpu_vertex_check)
    if args.pad_hold_a:
        env['RR_TEST_PAD_HOLD_A'] = '1'
    if args.window:
        env['RR_TEST_WINDOW_SIZE'] = args.window
        env['RR_NATIVE_FIXED_WINDOW'] = '0'
    if args.input_device:
        env['RR_HUMAN_INPUT_DEVICE'] = args.input_device
    if args.player_racer is not None:
        env['RR_TEST_PLAYER_RACER'] = str(args.player_racer)
    if args.addon_rider is not None:
        env['RR_TEST_ADDON_RIDER'] = args.addon_rider
    if args.hold_rider:
        env['RR_TEST_ADDON_RIDER_HOLD' if args.addon_rider is not None else 'RR_TEST_RIDER_HOLD'] = '1'
        env['RR_EVIDENCE_LOGS_ONLY'] = '0'   # this run writes the held Rider frame as an image
    if args.addon_course is not None:
        env['RR_TEST_ADDON_COURSE'] = args.addon_course
    if args.steer_invert:
        env['RR_TEST_STEER_INVERT'] = '1'
    if args.race_full:
        env['RR_TEST_RACE_FULL_BUDGET'] = '1'
    if args.all_ai:
        env['RR_ORIGINAL_AI_ALL_RACERS'] = '1'
    if args.route == 'main-mawhonic09-course-ra':
        # Normal-mode (mode 0) RA route: its explicit Mawhonic/Ryloth test
        # profile requires all eight original AI racers (see Run-Ryloth-FPS.cmd).
        env.update(RR_ORIGINAL_TEST_PROFILE='mawhonic-ryloth', RR_ORIGINAL_AI_ALL_RACERS='1',
                   RR_ORIGINAL_RIDER_PREVIEWS='3,7,9,23,24')
    if args.route == 'none':
        env.pop('RR_UNATTENDED_UI_ROUTE', None)
        env['RR_TRACK'] = args.track
    if args.profile_performance or args.profile_gpu_stages:
        env.update(RR_TRACE_PHASES='1', RR_TRACE_RENDER='1')
    if args.profile_gpu_stages:
        env['RR_TRACE_GPU_STAGES'] = '1'
    if args.restart_once:
        env['RR_UNATTENDED_RESTART_ONCE'] = '1'
    for switch in args.renderer_switch:
        env[switch] = '1'
    command = [str(staged), '--assets', str(assets)]
    result = dict(command=command, route=args.route, playerRacer=args.player_racer, watchdogSeconds=args.seconds,
                  sourceBuildReceipt=str(build / 'build-receipt.json'),
                  executableSha256=digest(staged),
                  runtimeHashes={p.name: digest(output / p.name) for p in runtime},
                  nativeSaveDirectory=str(saves), audioCapturePath=str(output / 'audio.wav'),
                  visualVerified=False, audioVerified=False, hostInputUsed=False, logsOnly=logs_only,
                  addonRider=args.addon_rider, addonRiderHold=args.hold_rider, addonCourse=args.addon_course,
                  eyecheckDlcHall=args.eyecheck_dlc_hall,
                  cameraPoseTrace=args.camera_trace, nativeRestartOnce=args.restart_once, rendererSwitches=sorted(set(args.renderer_switch)),
                  performanceProfiling=args.profile_performance or args.profile_gpu_stages,
                  serializedGpuStageProfiling=args.profile_gpu_stages)
    (output / 'launch.json').write_text(json.dumps(result, indent=2) + '\n')
    # Inherited process error mode prevents OS crash-dialog waits. Application
    # dialogs, if any, remain bounded by termination of this exact child handle.
    # User rule: one instance for this session, one for the arcade session.
    # Never start while another of OUR RacerRevengePC.exe runs (any copy not
    # under the work/scratch/racer-arcade folder, i.e. main-tree builds and evidence
    # outputs). Arcade runs and the user's silo/play are ignored. Wait, never
    # kill; the watchdog starts after the wait.
    arcade = str(ROOT / 'work' / 'scratch' / 'racer-arcade').lower()
    waited = 0.0
    while True:
        listing = subprocess.run(['powershell', '-NoProfile', '-Command',
            "Get-CimInstance Win32_Process -Filter \"Name='RacerRevengePC.exe'\" | ForEach-Object { $_.ExecutablePath }"],
            capture_output=True, text=True, errors='replace').stdout
        ours = [line.strip() for line in listing.splitlines()
                if line.strip() and not line.strip().lower().startswith(arcade)]
        if not ours:
            break
        if waited == 0.0:
            print('one_instance_wait: another RacerRevengePC.exe of this session is running; waiting', flush=True)
        time.sleep(15.0); waited += 15.0
    result['oneInstanceWaitSeconds'] = waited
    prior_mode = ctypes.windll.kernel32.SetErrorMode(0x0001 | 0x0002 | 0x8000)
    started = time.monotonic()
    child = None
    try:
        with (output / 'stdout.log').open('wb') as stdout, (output / 'stderr.log').open('wb') as stderr:
            child = subprocess.Popen(command, cwd=ROOT, env=env, stdout=stdout,
                                     stderr=stderr, creationflags=subprocess.CREATE_NO_WINDOW)
            result['pid'] = child.pid
            try:
                result['exitCode'] = child.wait(timeout=args.seconds)
                result['watchdogExpired'] = False
            except subprocess.TimeoutExpired:
                child.kill()
                result['exitCode'] = child.wait()
                result['watchdogExpired'] = True
    finally:
        if child is not None and child.poll() is None:
            child.kill()
            child.wait()
        ctypes.windll.kernel32.SetErrorMode(prior_mode)
        result['seconds'] = time.monotonic() - started
        result['childStopped'] = child is not None and child.poll() is not None
        summary = capture / 'window-evidence-summary.json'
        if summary.is_file():
            result['gameSummary'] = json.loads(summary.read_text())
        log = output / 'RacerRevenge.log'
        log_text = log.read_text(errors='replace') if log.is_file() else ''
        displays = re.findall(r'native_player_display generation=(\d+) sdl_id=(\d+) video=(\S+) host=(\S+)', log_text)
        retained = re.findall(r'native_player_window phase=retained sdl_id=(\d+) video=(\S+) host=(\S+)', log_text)
        lifecycle = re.findall(r'native_window_lifetime phase=(created|destroyed) sdl_id=(\d+) window=(\S+)', log_text)
        # The first Display is constructed before its window, hence SDL ID 0.
        owners = {tuple(row[2:]) for row in displays}
        live_ids = {row[1] for row in displays if row[1] != '0'}
        result['windowContinuity'] = dict(displays=displays, retained=retained, lifecycle=lifecycle,
            verified=bool(len(displays) >= 2 and len(owners) == 1 and retained and
                          len(lifecycle) == 2 and lifecycle[0][0] == 'created' and
                          lifecycle[1][0] == 'destroyed' and lifecycle[0][1] != '0' and
                          lifecycle[0][1:] == lifecycle[1][1:] and
                          live_ids == {lifecycle[0][1]} and
                          all(row[0] == lifecycle[0][1] and tuple(row[1:]) in owners for row in retained)))
        track = ('GA' if args.route.endswith('-ga') else 'MA' if args.route.endswith('-ma') else
                 'RA' if args.route.endswith('-ra') else 'TA')
        # A seeded tournament run goes to the course the seed's career reached.
        if args.route == 'main-tournament-course-ta' and args.seed_save:
            track = seeded_tournament_track((saves / 'progress-v1.bin').read_bytes()) or track
        result['expectedTrack'] = track
        loading_sessions = [(1, 'UI'), (2, track)] + ([(3, track)] if args.restart_once else [])
        loading_files = [capture / f'loading-session-{session}' / f'loading-{code}-{frame}.ppm'
                         for session, code in loading_sessions for frame in (1, 2)]
        loading_assets = re.findall(r'original_loading_assets background=(\S+) layout=(\S+) background_sha256=([0-9a-f]{64})', log_text)
        result['originalLoadingScreens'] = dict(assets=loading_assets,
            captures=[str(p) for p in loading_files],
            captured=all(p.is_file() and p.stat().st_size > 32 for p in loading_files))
        result['routeCompleted'] = bool(result.get('exitCode') == 0 and
            not result.get('watchdogExpired', True) and
            result['windowContinuity']['verified'] and
            (logs_only or result['originalLoadingScreens']['captured']) and
            {row[0] for row in loading_assets} == {'LD_UI.PST', f'LD_{track}.PST'} and
            result.get('gameSummary', {}).get('completed') and
            result.get('gameSummary', {}).get('cleanup_complete'))
        if args.restart_once:
            restart = result.get('gameSummary', {}).get('native_restart_test', {})
            first = restart.get('first_race') or {}
            second = result.get('gameSummary', {}).get('track', {})
            files = [first.get('image_file'), first.get('receipt_file'), first.get('baseline_file'),
                     second.get('image_file'), second.get('receipt_file'),
                     result.get('gameSummary', {}).get('baseline_file')]
            if logs_only:
                files = [first.get('receipt_file'), first.get('baseline_file'), second.get('receipt_file'), result.get('gameSummary', {}).get('baseline_file')]
            result['nativeRestartVerified'] = bool(restart.get('issued') and
                len(displays) == 3 and len(retained) == 2 and len(set(files)) == (4 if logs_only else 6) and
                all(path and inside(Path(path)).is_file() for path in files) and
                second.get('receipt_file' if logs_only else 'image_file') == str(capture / 'race-session-3' / ('track-1.json' if logs_only else 'track-1.ppm')))
            if result['nativeRestartVerified']:
                before = json.loads(Path(first['receipt_file']).read_text())
                after = json.loads(Path(second['receipt_file']).read_text())
                result['nativeRestartVerified'] = bool(
                    before.get('session') == 2 and after.get('session') == 3 and
                    before.get('course_selection') == after.get('course_selection') and
                    before.get('selected_rider') == after.get('selected_rider') and
                    all(row.get('track') == track and row.get('race_phase') == 2 and
                        row.get('settled') and row.get('window_swap_submitted') and
                        row.get('world_ticks', 0) > 0 and 0 < row.get('race_seconds', 0) < 2
                        for row in (before, after)))
            result['routeCompleted'] = result['routeCompleted'] and result['nativeRestartVerified']
        if logs_only:
            result['noImagesWritten'] = not any(p.suffix.lower() in {'.ppm', '.png', '.jpg', '.pending'} for p in output.rglob('*'))
            result['routeCompleted'] = result['routeCompleted'] and result['noImagesWritten']
        if args.eyecheck_dlc_hall is not None:
            # Held on the Hall TA page until the wall budget: complete when the
            # Hall frame was written and the hold was reached.
            texts = log_text
            for name in ('stderr.log', 'stdout.log'):
                extra = output / name
                if extra.is_file():
                    texts += extra.read_text(errors='replace')
            hall_images = sorted(str(p) for p in capture.rglob('hall-*.ppm'))
            result['dlcHallEyecheck'] = dict(
                key=args.eyecheck_dlc_hall, hallImages=hall_images,
                holdReached='hall_route hold screen=11 course=0 planet=0' in texts)
            result['routeCompleted'] = bool(hall_images and result['dlcHallEyecheck']['holdReached'])
        if args.hold_rider:
            # A held run never confirms and ends at its wall budget, so the race
            # receipts above are absent. It is complete when the Rider frame with
            # the add-on highlighted was written and the hold was reached.
            texts = log_text
            for name in ('stderr.log', 'stdout.log'):
                extra = output / name
                if extra.is_file():
                    texts += extra.read_text(errors='replace')
            rider_images = sorted(str(p) for p in capture.rglob('rider-*.ppm'))
            result['addonRiderHold'] = dict(
                key=args.addon_rider, riderImages=rider_images,
                holdReached=(f'addon_rider_route hold key={args.addon_rider} ' in texts) if args.addon_rider
                            else ('rider_route hold rider=' in texts),
                carousel=re.findall(r'dlc_rider_carousel [^\r\n]*', texts),
                draw=re.findall(r'dlc_rider_draw [^\r\n]*', texts),
                model=re.findall(r'dlc_rider_model[ _][^\r\n]*', texts))
            result['routeCompleted'] = bool(rider_images and result['addonRiderHold']['holdReached'])
        (output / 'execution.json').write_text(json.dumps(result, indent=2) + '\n')
        # Disk hygiene (user request 2026-11-13): a logs-only run keeps its logs and
        # receipts; the staged program, DLL copies and audio recording (~65 MB per
        # run) are removed once the child has stopped.
        if args.logs_only and not args.keep_binaries and result.get('childStopped'):
            for name in [staged.name, *(dll.name for dll in runtime), 'audio.wav']:
                try:
                    (output / name).unlink(missing_ok=True)
                except OSError:
                    pass
            for extra in output.glob('audio-*.wav'):
                try:
                    extra.unlink()
                except OSError:
                    pass
    print(json.dumps(result, indent=2))
    return 0 if result['routeCompleted'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
