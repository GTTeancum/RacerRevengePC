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
import subprocess
import time

ROOT = Path(__file__).resolve().parents[2]


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
                        'main-rider-course-ta'], default='main-dud04-course-ga')
    parser.add_argument('--logs-only', action='store_true', help='Record stage and timing receipts without pixel readback or image files')
    parser.add_argument('--seconds', type=int, default=180)
    parser.add_argument('--profile-performance', action='store_true',
                        help='Enable existing CPU/render/display phase measurements')
    parser.add_argument('--profile-gpu-stages', action='store_true',
                        help='Serialize rendering stages for diagnosis; not production FPS measurement')
    parser.add_argument('--camera-trace', action='store_true',
                        help='Log actual attached-camera poses without changing camera behavior')
    parser.add_argument('--restart-once', action='store_true',
                        help='Exercise native race teardown/rebuild after first race evidence; not Pause-menu input')
    args = parser.parse_args()
    if os.name != 'nt' or not 10 <= args.seconds <= 180:
        parser.error('Windows required; watchdog must be 10..180 seconds')
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
               RR_NATIVE_FIXED_WINDOW='1')
    if args.logs_only:
        env['RR_EVIDENCE_LOGS_ONLY'] = '1'
    if args.camera_trace:
        env['RR_CAMERA_POSE_TRACE'] = '1'
    if args.profile_performance or args.profile_gpu_stages:
        env.update(RR_TRACE_PHASES='1', RR_TRACE_RENDER='1')
    if args.profile_gpu_stages:
        env['RR_TRACE_GPU_STAGES'] = '1'
    if args.restart_once:
        env['RR_UNATTENDED_RESTART_ONCE'] = '1'
    command = [str(staged), '--assets', str(assets)]
    result = dict(command=command, route=args.route, watchdogSeconds=args.seconds,
                  sourceBuildReceipt=str(build / 'build-receipt.json'),
                  executableSha256=digest(staged),
                  runtimeHashes={p.name: digest(output / p.name) for p in runtime},
                  nativeSaveDirectory=str(saves), audioCapturePath=str(output / 'audio.wav'),
                  visualVerified=False, audioVerified=False, hostInputUsed=False, logsOnly=args.logs_only,
                  cameraPoseTrace=args.camera_trace, nativeRestartOnce=args.restart_once,
                  performanceProfiling=args.profile_performance or args.profile_gpu_stages,
                  serializedGpuStageProfiling=args.profile_gpu_stages)
    (output / 'launch.json').write_text(json.dumps(result, indent=2) + '\n')
    # Inherited process error mode prevents OS crash-dialog waits. Application
    # dialogs, if any, remain bounded by termination of this exact child handle.
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
        track = 'GA' if args.route.endswith('-ga') else 'MA' if args.route.endswith('-ma') else 'TA'
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
            (args.logs_only or result['originalLoadingScreens']['captured']) and
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
            if args.logs_only:
                files = [first.get('receipt_file'), first.get('baseline_file'), second.get('receipt_file'), result.get('gameSummary', {}).get('baseline_file')]
            result['nativeRestartVerified'] = bool(restart.get('issued') and
                len(displays) == 3 and len(retained) == 2 and len(set(files)) == (4 if args.logs_only else 6) and
                all(path and inside(Path(path)).is_file() for path in files) and
                second.get('receipt_file' if args.logs_only else 'image_file') == str(capture / 'race-session-3' / ('track-1.json' if args.logs_only else 'track-1.ppm')))
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
        if args.logs_only:
            result['noImagesWritten'] = not any(p.suffix.lower() in {'.ppm', '.png', '.jpg', '.pending'} for p in output.rglob('*'))
            result['routeCompleted'] = result['routeCompleted'] and result['noImagesWritten']
        (output / 'execution.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))
    return 0 if result['routeCompleted'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
