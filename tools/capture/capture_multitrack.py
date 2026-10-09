#!/usr/bin/env python3
"""STAGED, NOT RUN: one reserved root runtime, actual TA -> incomplete GC UI exercise."""
import argparse
import datetime as dt
import fcntl
import hashlib
import json
import os
from pathlib import Path
import re
import statistics
import struct
import subprocess
import tempfile
import time
import wave


REPO = Path('/workspace/scratch/74cd1d3d7a45/racerrevenge_solo_return/RacerRevengePC')
LEDGER = Path('/workspace/shared/rr-last-central-test.json')
COMMIT = re.compile(r'^live capture committed at tick (\d+)')
RACE = re.compile(r'^race phase=(\d+) countdown=(\d+) seconds=([^ ]+)')
AUDIO_RECORDING = re.compile(r'^Audio recording session=(\d+) path=(.+)$')
ACTIVE = {'running', 'launching', 'reserved', 'building', 'ready', 'active'}


def utc():
    return dt.datetime.now(dt.timezone.utc)


def aware(value):
    result = dt.datetime.fromisoformat(value.replace('Z', '+00:00'))
    if result.tzinfo is None:
        raise ValueError('An explicit timezone is required')
    return result.astimezone(dt.timezone.utc)


def digest(path):
    h = hashlib.sha256()
    with path.open('rb') as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b''):
            h.update(block)
    return h.hexdigest()


def write_json(path, value):
    # A killed runner must leave either the previous complete receipt or the new
    # one, while append-only command/event/frame logs preserve the raw prefix.
    temporary = path.with_name(path.name + '.tmp')
    with temporary.open('w', encoding='utf-8') as handle:
        handle.write(json.dumps(value, indent=2) + '\n')
        handle.flush()
        os.fsync(handle.fileno())
    os.replace(temporary, path)


def replace_ledger(expected, value):
    # Called only with the shared flock held. Compare also catches a writer
    # that neglected that lock. The published slot is never rolled back.
    raw = (json.dumps(value, indent=2) + '\n').encode()
    fd, name = tempfile.mkstemp(prefix='.rr-video-slot-', dir=LEDGER.parent)
    try:
        with os.fdopen(fd, 'wb') as handle:
            handle.write(raw)
            handle.flush()
            os.fsync(handle.fileno())
        if LEDGER.read_bytes() != expected:
            raise RuntimeError('Central ledger changed; refusing to overwrite it')
        os.replace(name, LEDGER)
        return raw
    finally:
        if os.path.exists(name):
            os.unlink(name)


def admit(args, out):
    previous = LEDGER.read_bytes()  # Missing or malformed ledger fails closed.
    old = json.loads(previous)
    if old.get('owner') != 'root' or old.get('status') not in ACTIVE:
        raise RuntimeError('Only an already reserved, active root-owned central slot is admitted')
    started = aware(old['started_utc'])
    earliest = max(started + dt.timedelta(hours=1),
                   aware(old.get('next_earliest_utc',
                                 (started + dt.timedelta(hours=1)).isoformat())))
    now = utc()
    if started > now:
        raise RuntimeError('Central ledger has a future start timestamp')
    if (not args.root_owns_active_slot or old.get('runtime_launches') != 0
            or old['started_utc'] != args.expected_slot_start
            or old.get('source_commit') != args.source_commit or now >= earliest):
        raise RuntimeError('Requires exact recent root reservation/source commit, runtime_launches=0, and no prior runtime')
    claim = dict(old)
    claim['capture_scope'] = 'One process: TA menu/drive/pause then one cold GC selection and dense GC capture'
    out.mkdir(parents=True, exist_ok=False)
    (out / 'previous-test.json').write_bytes(previous)
    claim.update(status='launching', runtime_launches=1,
                 runtime_launches_note='Reserved before Popen; a failed launch consumes the slot',
                 source_commit=args.source_commit, binary=str(args.binary),
                 output_directory=str(out), capture_runner=str(Path(__file__).resolve()))
    current = replace_ledger(previous, claim)
    return claim, current


def live_audio_frames(path):
    if not path.exists():
        return None
    with path.open('rb', buffering=0) as handle:
        header = handle.read(44)
        size = os.fstat(handle.fileno()).st_size
    if len(header) < 44:
        return None
    if (header[:4] != b'RIFF' or header[8:16] != b'WAVEfmt '
            or header[36:40] != b'data'
            or struct.unpack_from('<IHHIIHH', header, 16)
            != (16, 1, 2, 48000, 192000, 4, 16)):
        raise RuntimeError('Unexpected live PCM format; audio anchor is not trustworthy')
    return (size - 44) // 4


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('binary', type=Path)
    parser.add_argument('fixture', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--root-authorized', action='store_true', required=True)
    parser.add_argument('--source-commit', required=True)
    parser.add_argument('--track-label', default='GC experimental incomplete world, mode 2; selected from TA through Main',
                        help='Output description only; actual track identity is checked from native telemetry')
    parser.add_argument('--cwd', type=Path, default=REPO)
    parser.add_argument('--seconds', type=float, default=20.0)
    parser.add_argument('--fps', type=float, default=20.0)
    parser.add_argument('--env', action='append', default=[], metavar='RR_KEY=VALUE')
    parser.add_argument('--source-file', type=Path, action='append', default=[],
                        help='Reviewed source file to hash before and after the sole runtime; repeatable')
    parser.add_argument('--root-owns-active-slot', action='store_true', required=True)
    parser.add_argument('--expected-slot-start', required=True)
    args = parser.parse_args()
    if args.seconds != 20 or args.fps != 20:
        parser.error('This bounded exercise requires exactly 20 seconds at 20 requested captures/second')
    if not re.fullmatch(r'[0-9a-fA-F]{40}', args.source_commit):
        parser.error('--source-commit must be the reviewed 40-character source commit')
    from multitrack_exercise import MultiTrackExercise
    menu = MultiTrackExercise()
    args.cwd = args.cwd.resolve()
    args.binary = args.binary.resolve()
    args.fixture = (args.fixture if args.fixture.is_absolute()
                    else args.cwd / args.fixture).resolve()
    if not args.binary.is_file() or not args.fixture.is_file() or not args.cwd.is_dir():
        parser.error('Existing binary, fixture, and working directory are required; no build is run')
    source_files = [Path(__file__).resolve(), Path(__file__).resolve().with_name('multitrack_exercise.py')]
    for path in args.source_file:
        resolved = (path if path.is_absolute() else args.cwd / path).resolve()
        if not resolved.is_file():
            parser.error('Source file does not exist: ' + str(resolved))
        if resolved not in source_files:
            source_files.append(resolved)
    source_hashes = {str(path): digest(path) for path in source_files}
    out = args.output.resolve()
    if out.exists():
        parser.error('Output must be a new directory')
    overrides = {'RR_AUTODRIVE': '1', 'RR_AUTODRIVE_PACE': 'authored',
                 'RR_AUDIO_DEVICE': '0', 'RR_AUDIO': '1', 'RR_RACE': '1',
                 'RR_AUDIO_WAV': str(out / 'actual-mix.wav'), 'RR_START_MENU': '1',
                 'RR_TRACK': 'TA', 'RR_RACE_MODE': '2', 'RR_GC_INCOMPLETE_WORLD': '1'}
    for item in args.env:
        key, sep, value = item.partition('=')
        if not sep or key != 'RR_AUDIO_ROOT' or not value:
            parser.error('Only an explicit RR_AUDIO_ROOT path override is permitted: ' + item)
        overrides[key] = value
    env = {k: v for k, v in os.environ.items() if not k.startswith('RR_')}
    env.update(overrides)
    binary_hash, fixture_hash = digest(args.binary), digest(args.fixture)
    # Root must not invoke this while another runtime is active. The held
    # central lock plus strict ledger admission guards cooperating callers.
    with LEDGER.with_suffix('.lock').open('a+b') as lock:
        fcntl.flock(lock.fileno(), fcntl.LOCK_EX | fcntl.LOCK_NB)
        claim, ledger_raw = admit(args, out)
        receipt = {'schema': 'rr-dense-gameplay-capture-v1',
                   'source_commit_label': args.source_commit, 'track_label': args.track_label,
                   'track_label_is_selection_or_verification': False,
                   'exercise': 'single-process-ta-to-incomplete-gc',
                   'video_track': 'GC', 'gc_complete_world_admitted': False,
                   'binary_sha256': binary_hash, 'fixture_sha256': fixture_hash,
                   'source_hashes_before': source_hashes,
                   'cwd': str(args.cwd), 'env_overrides': overrides,
                   'cleared_inherited_rr_variables': sorted(k for k in os.environ if k.startswith('RR_')),
                   'slot': claim, 'target_seconds': args.seconds, 'target_fps': args.fps,
                   'timing_basis': 'Monotonic acknowledgement-observation time, not invented frame rate',
                   'scope': ('One headless live process; prepared TA Start, real drive/Pause/Resume, '
                             'cancelled then confirmed Main, GC disclosure and one expected cold Start, '
                             '20-second actual GC capture, quit. No other restart or retry.'),
                   'input_note': 'HOST authored-pace course driver by default; not human gameplay or original AI',
                   'audio_note': 'Real native mixed PCM, no microphone/device playback claim',
                   'cold_start_limit_seconds': 360,
                   'capture_ack_limit_seconds': 2,
                   'no_encoding_during_capture': True, 'frames': [], 'errors': []}
        receipt['menu_exercise'] = menu.receipt if menu is not None else {'enabled': False}
        current_ppm, wav_path = out / 'current-committed.ppm', out / 'actual-mix.wav'
        argv = [str(args.binary), str(args.fixture), '--headless-live', str(current_ppm)]
        receipt['argv'] = argv
        start = time.monotonic()
        receipt['utc_started'] = utc().isoformat()
        process = None
        pending = None
        awaiting_ui_for = None
        menu_finalization_started = False
        latest_race = None
        first_ack = None
        next_request = None
        carry = b''
        session_headers = 0
        audio_recordings = {}
        stop_reason = 'not_launched'
        frames, errors = receipt['frames'], receipt['errors']
        write_json(out / 'capture-receipt.json', receipt)
        with ((out / 'stdout.log').open('xb', buffering=0) as stdout,
              (out / 'stderr.log').open('xb', buffering=0) as stderr,
              (out / 'stderr.log').open('rb', buffering=0) as reader,
              (out / 'events.jsonl').open('x') as events,
              (out / 'frames.jsonl').open('x') as frame_log,
              (out / 'commands.jsonl').open('x') as commands):

            def send(command):
                at = time.monotonic()
                process.stdin.write((command + '\n').encode('ascii'))
                process.stdin.flush()
                commands.write(json.dumps({'elapsed': at - start, 'command': command}) + '\n')
                commands.flush()
                return at

            def request_capture(purpose='video'):
                nonlocal pending
                if pending is not None or awaiting_ui_for is not None:
                    raise RuntimeError('Only one fully acknowledged capture may be outstanding')
                pending = {'requested_at': send('capture'), 'purpose': purpose}

            try:
                process = subprocess.Popen(argv, cwd=args.cwd, env=env,
                                           stdin=subprocess.PIPE, stdout=stdout, stderr=stderr)
                receipt['pid'] = process.pid
                claim.update(status='running', pid=process.pid)
                ledger_raw = replace_ledger(ledger_raw, claim)
                while True:
                    carry += reader.read()
                    lines = carry.split(b'\n')
                    carry = lines.pop()
                    for raw in lines:
                        observed = time.monotonic()
                        line = raw.decode('utf-8', errors='replace').rstrip('\r')
                        event = {'elapsed': observed - start, 'line': line}
                        events.write(json.dumps(event) + '\n')
                        if line.startswith('HOST RACE:'):
                            session_headers += 1
                            if menu is not None:
                                menu.admit_session_header(session_headers, line, observed - start)
                            elif session_headers > 1:
                                raise RuntimeError('Unexpected second race session; no restart is authorized')
                        audio_match = AUDIO_RECORDING.match(line)
                        if audio_match:
                            audio_session = int(audio_match.group(1))
                            audio_path = Path(audio_match.group(2)).resolve()
                            expected_audio = (out / 'actual-mix.wav' if audio_session == 1
                                              else out / ('actual-mix-session-%d.wav' % audio_session))
                            if (audio_session not in {1, 2} or audio_path != expected_audio
                                    or audio_session in audio_recordings):
                                raise RuntimeError('Unexpected/duplicate native audio recording destination')
                            audio_recordings[audio_session] = dict(event, path=str(audio_path))
                        race_match = RACE.match(line)
                        if race_match:
                            latest_race = dict(event, phase=int(race_match.group(1)),
                                               race_seconds=float(race_match.group(3)))
                        match = COMMIT.match(line)
                        if match:
                            if pending is None:
                                raise RuntimeError('Unrequested capture acknowledgement')
                            audio_at = time.monotonic()
                            audio_frames = live_audio_frames(wav_path)
                            data = current_ppm.read_bytes()
                            parts = data.split(b'\n', 3)
                            if len(parts) != 4 or parts[0] != b'P6' or parts[2] != b'255':
                                raise RuntimeError('Invalid committed PPM')
                            width, height = map(int, parts[1].split())
                            if width <= 0 or height <= 0 or len(parts[3]) != width * height * 3:
                                raise RuntimeError('Incomplete committed PPM')
                            purpose = pending['purpose']
                            name = ('frame-%06d.ppm' % len(frames) if purpose == 'video'
                                    else 'menu-%s.ppm' % purpose)
                            with (out / name).open('xb') as target:
                                target.write(data)
                            frame = {'file': name, 'tick': int(match.group(1)),
                                     'purpose': purpose,
                                     'request_elapsed': pending['requested_at'] - start,
                                     'ack_elapsed': observed - start,
                                     'ack_latency_seconds': observed - pending['requested_at'],
                                     'audio_sample_frames': audio_frames,
                                     'audio_anchor_observed_elapsed': audio_at - start,
                                     'width': width, 'height': height,
                                     'sha256': hashlib.sha256(data).hexdigest(),
                                     'latest_race': latest_race, 'commit_log': line}
                            if purpose == 'video':
                                if first_ack is None:
                                    first_ack = observed
                                frame['video_time_seconds'] = observed - first_ack
                                frames.append(frame)
                                frame_log.write(json.dumps(frame) + '\n')
                                frame_log.flush()
                                # No catch-up bursts. A late capture remains late in the evidence.
                                next_request = max(next_request + 1 / args.fps,
                                                   observed + 0.001)
                            else:
                                menu.capture_committed(purpose, frame)
                            if menu is not None:
                                awaiting_ui_for = frame
                            pending = None
                        if menu is not None:
                            ui_event = menu.observe(line, observed - start)
                            if ui_event is not None and ui_event['reason'] == 'ready':
                                active_audio_session = ui_event['session']
                                if active_audio_session not in audio_recordings:
                                    raise RuntimeError('Ready session lacks its actual Audio recording acknowledgement')
                                wav_path = Path(audio_recordings[active_audio_session]['path'])
                                receipt['active_audio_session'] = active_audio_session
                            if ui_event is not None and ui_event['reason'] == 'capture':
                                if awaiting_ui_for is None:
                                    raise RuntimeError('UI capture telemetry has no committed image')
                                awaiting_ui_for['ui_state'] = ui_event
                                if awaiting_ui_for['tick'] != ui_event['tick']:
                                    raise RuntimeError('Image acknowledgement and native UI tick differ')
                                awaiting_ui_for = None
                            if ui_event is not None and (ui_event['reason'] != 'capture'
                                    or len(frames) % 40 == 0):
                                # Keep the observed prefix even if the parent/host disappears.
                                receipt['native_session_header_count'] = session_headers
                                receipt['audio_recordings'] = audio_recordings
                                write_json(out / 'capture-receipt.json', receipt)
                                write_json(out / 'menu-exercise.json', menu.receipt)
                    events.flush()
                    now = time.monotonic()
                    video_window_complete = bool(frames and frames[-1]['video_time_seconds'] >= args.seconds)
                    if video_window_complete:
                        if menu is None:
                            stop_reason = 'requested_wall_time_window_captured'
                            break
                        if not menu_finalization_started and awaiting_ui_for is None:
                            menu.finish_video(now - start, send)
                            menu_finalization_started = True
                        if menu.done:
                            stop_reason = 'requested_wall_time_window_and_menu_exercise_captured'
                            break
                    if process.poll() is not None:
                        raise RuntimeError('Runtime exited before the requested video window')
                    if first_ack is None and now - start > receipt['cold_start_limit_seconds']:
                        raise TimeoutError('No racing capture within the declared cold-start/menu bound')
                    if pending is not None and now - pending['requested_at'] > 2:
                        raise TimeoutError('No capture acknowledgement within two seconds')
                    if awaiting_ui_for is not None and now - start - awaiting_ui_for['ack_elapsed'] > 2:
                        raise TimeoutError('No actual UI-state acknowledgement after committed image')
                    if (first_ack is not None and not menu_finalization_started
                            and now - first_ack > args.seconds + 3):
                        raise TimeoutError('Capture window stalled')
                    if menu is not None:
                        menu.step(now - start, latest_race, send, request_capture)
                    if not video_window_complete and (menu is None or menu.video_ready):
                        if pending is None and awaiting_ui_for is None and latest_race and latest_race['phase'] == 2:
                            if next_request is None:
                                next_request = now
                            if now >= next_request:
                                request_capture()
                        elif first_ack is not None and pending is None and awaiting_ui_for is None:
                            raise RuntimeError('Race left Racing phase during requested short clip')
                    time.sleep(0.001)
            except (Exception, KeyboardInterrupt) as error:
                errors.append(repr(error))
                stop_reason = 'capture_or_launch_error'
            finally:
                receipt['forced_cleanup'] = []
                if process is not None and process.poll() is None:
                    try:
                        send('quit')
                        receipt['quit_sent'] = True
                        process.wait(timeout=15)
                    except Exception as error:
                        errors.append('Clean stop: ' + repr(error))
                        if process.poll() is None:
                            receipt['forced_cleanup'].append('terminate_own_child')
                            process.terminate()
                            try:
                                process.wait(timeout=5)
                            except subprocess.TimeoutExpired:
                                receipt['forced_cleanup'].append('kill_own_child')
                                process.kill()
                                process.wait()
                if process is not None and process.stdin:
                    process.stdin.close()
                receipt.update(exit_code=process.returncode if process else None,
                               stop_reason=stop_reason, utc_finished=utc().isoformat(),
                               process_wall_seconds=time.monotonic() - start)
        if len(frames) >= 2:
            gaps = [b['ack_elapsed'] - a['ack_elapsed'] for a, b in zip(frames, frames[1:])]
            receipt.update(measured_video_seconds=sum(gaps),
                           measured_capture_fps=len(gaps) / sum(gaps),
                           median_gap_seconds=statistics.median(gaps),
                           maximum_gap_seconds=max(gaps),
                           unique_pixel_frames=len({f['sha256'] for f in frames}),
                           game_tick_delta=frames[-1]['tick'] - frames[0]['tick'])
            if (receipt['measured_video_seconds'] < 20
                    or receipt['measured_capture_fps'] < 12
                    or receipt['maximum_gap_seconds'] > 0.25
                    or receipt['unique_pixel_frames'] < len(frames) * 0.8
                    or receipt['game_tick_delta'] <= 0):
                errors.append('GC capture is sparse, stalled, or lacks unique actual moving frames; preserve it as a failed prefix')
        try:
            if wav_path.is_file():
                with wave.open(str(wav_path), 'rb') as audio:
                    receipt['audio'] = {'file': wav_path.name, 'sha256': digest(wav_path),
                                        'frames': audio.getnframes(), 'sample_rate': audio.getframerate(),
                                        'sample_width': audio.getsampwidth(), 'channels': audio.getnchannels(),
                                        'bytes': wav_path.stat().st_size}
        except Exception as error:
            receipt['audio_inspection_error'] = repr(error)
        receipt['audio_recordings'] = audio_recordings
        receipt['native_session_header_count'] = session_headers
        receipt['preserved_audio_files'] = []
        for audio_session, recording in audio_recordings.items():
            path = Path(recording['path'])
            try:
                receipt['preserved_audio_files'].append({'session': audio_session,
                    'file': path.name, 'bytes': path.stat().st_size, 'sha256': digest(path),
                    'selected_for_video': path == wav_path})
            except OSError as error:
                errors.append('Preserved audio identity check: ' + repr(error))
        try:
            receipt['source_hashes_after'] = {str(path): digest(path) for path in source_files}
            receipt['binary_sha256_after'] = digest(args.binary)
            receipt['fixture_sha256_after'] = digest(args.fixture)
            if (receipt['source_hashes_after'] != source_hashes
                    or receipt['binary_sha256_after'] != binary_hash
                    or receipt['fixture_sha256_after'] != fixture_hash):
                errors.append('Reviewed source, binary, or fixture changed during capture')
        except Exception as error:
            errors.append('Final source identity check: ' + repr(error))
        receipt['capture_complete'] = bool(not errors and receipt['exit_code'] == 0
            and stop_reason in {'requested_wall_time_window_captured',
                                'requested_wall_time_window_and_menu_exercise_captured'}
            and len(frames) >= 2 and menu.receipt['verified']
            and session_headers == 2 and set(audio_recordings) == {1, 2}
            and receipt.get('active_audio_session') == 2)
        if menu is not None:
            write_json(out / 'menu-exercise.json', menu.receipt)
        write_json(out / 'capture-receipt.json', receipt)
        claim.update(status='completed' if receipt['capture_complete'] else 'failed',
                     finished_utc=utc().isoformat(), runtime_launches=1,
                     capture_receipt=str(out / 'capture-receipt.json'))
        replace_ledger(ledger_raw, claim)
    print(json.dumps({'receipt': str(out / 'capture-receipt.json'),
                      'capture_complete': receipt['capture_complete'],
                      'frames': len(frames), 'errors': errors}), flush=True)
    return 0 if receipt['capture_complete'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
