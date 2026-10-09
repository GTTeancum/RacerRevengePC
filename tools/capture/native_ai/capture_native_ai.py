#!/usr/bin/env python3
"""STAGED, UNEXECUTED: one root-reserved direct-GC original-AI first-slice capture."""
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
from capture_clock_contract import verify_capture_clocks


REPO = Path('/workspace/scratch/74cd1d3d7a45/racerrevenge_solo_return/RacerRevengePC')
LEDGER = Path('/workspace/shared/rr-last-central-test.json')
COMMIT = re.compile(r'^live capture committed at tick (\d+)')
CAPTURE_STATE = re.compile(r'^capture_state wrong_way=([01]) tick=(\d+) host_menu=([01])$')
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
    claim['capture_scope'] = 'One direct GC process/session, genuine-AI slot 1, at most 12 seconds after ready; capture/quit only'
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


# This reviewed candidate is restricted to the parent's next eligible slot.
EARLIEST_AUTHORIZED_UTC = '2026-10-10T00:20:36+00:00'
FROZEN_SOURCE = Path('/workspace/shared/rr-native-ai-first-slice')
GUARD_SOURCE = Path('/workspace/shared/rr-native-ai-first-slice-manager-guard50iv/files/tests/native/terrain_interactive_drive50du.cpp')
GUARD_SOURCE_SHA256 = 'c9cdc1fc76f934dd2e48da1eb79f524c2b9d2b818598bb8fc1fb081ee6053933'


def validate_ui(item, identity):
    bools = ('menu', 'track_pending', 'loaded_track_incomplete', 'gc_incomplete_opt_in',
             'gc_mode2_admitted', 'audio_enabled', 'audio_pending', 'results_visible')
    integers = ('page', 'session', 'audio_bank_id', 'tick', 'mixed_frames', 'loaded_racer', 'selected_racer')
    if (not isinstance(item, dict) or item.get('reason') not in {'ready', 'capture'}
            or any(type(item.get(k)) is not bool for k in bools)
            or any(type(item.get(k)) is not int for k in integers)
            or item['session'] != 1 or item['audio_bank_id'] <= 0
            or item['tick'] < 0 or item['mixed_frames'] < 0
            or (item['loaded_racer'], item['selected_racer']) != (1, 1)
            or item['menu'] or item['page'] != 0 or item['track_pending']
            or item.get('loaded_track') != 'GC' or item.get('selected_track') != 'GC'
            or not item['loaded_track_incomplete'] or not item['gc_incomplete_opt_in']
            or not item['gc_mode2_admitted'] or not item['audio_enabled']
            or item['audio_pending'] or item['results_visible']):
        raise RuntimeError('Unexpected direct-GC, single-session, actual UI/audio identity')
    current = (item['session'], item['audio_bank_id'])
    if identity is not None and current != identity:
        raise RuntimeError('Native session/audio owner changed; no rebuild is authorized')
    return current


def main():
    from native_ai_evidence import NativeAiEvidence
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('binary', type=Path)
    parser.add_argument('fixture', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--root-authorized', action='store_true', required=True)
    parser.add_argument('--root-owns-active-slot', action='store_true', required=True)
    parser.add_argument('--source-commit', required=True)
    parser.add_argument('--expected-slot-start', required=True)
    parser.add_argument('--cwd', type=Path, default=REPO)
    parser.add_argument('--seconds', type=float, choices=(12.0,), default=12.0)
    parser.add_argument('--fps', type=float, choices=(20.0,), default=20.0)
    parser.add_argument('--source-file', type=Path, action='append', default=[])
    parser.add_argument('--env', action='append', default=[], metavar='RR_AUDIO_ROOT=PATH')
    args = parser.parse_args()
    if (utc() < aware(EARLIEST_AUTHORIZED_UTC)
            or aware(args.expected_slot_start) < aware(EARLIEST_AUTHORIZED_UTC)):
        parser.error('This candidate is not admitted before ' + EARLIEST_AUTHORIZED_UTC)
    if not re.fullmatch(r'[0-9a-fA-F]{40}', args.source_commit):
        parser.error('Requires the reviewed 40-character integrated source commit')
    args.cwd, args.binary = args.cwd.resolve(), args.binary.resolve()
    args.fixture = (args.fixture if args.fixture.is_absolute() else args.cwd / args.fixture).resolve()
    if not args.cwd.is_dir() or not args.binary.is_file() or not args.fixture.is_file():
        parser.error('Existing reviewed binary, fixture and cwd required; no build is performed')
    out = args.output.resolve()
    if out.exists():
        parser.error('Output must be a new directory; all prior evidence is preserved')
    here = Path(__file__).resolve().parent
    source_files = [Path(__file__).resolve(), here / 'native_ai_evidence.py',
                    here / 'capture_clock_contract.py', FROZEN_SOURCE / 'manifest.json', GUARD_SOURCE]
    if digest(GUARD_SOURCE) != GUARD_SOURCE_SHA256:
        parser.error('The separately reviewed single-session/manager guard source changed')
    frozen = json.loads((FROZEN_SOURCE / 'manifest.json').read_text())
    for entry in frozen['files']:
        path = (FROZEN_SOURCE / 'files' / entry['path']).resolve()
        if not path.is_relative_to((FROZEN_SOURCE / 'files').resolve()) or digest(path) != entry['sha256']:
            parser.error('Frozen runtime source identity changed: ' + entry['path'])
        source_files.append(path)
    for path in args.source_file:
        path = (path if path.is_absolute() else args.cwd / path).resolve()
        if path not in source_files:
            source_files.append(path)
    source_hashes = {str(path): digest(path) for path in source_files}
    binary_hash, fixture_hash = digest(args.binary), digest(args.fixture)
    overrides = {'RR_TRACK': 'GC', 'RR_RACE_MODE': '2', 'RR_GC_INCOMPLETE_WORLD': '1',
                 'RR_RACE': '1', 'RR_ORIGINAL_AI_FIRST_SLICE': '1', 'RR_START_MENU': '0',
                 'RR_AUDIO': '1', 'RR_AUDIO_DEVICE': '0', 'RR_AUDIO_WAV': str(out / 'actual-mix.wav'),
                 'RR_AUTODRIVE': '1', 'RR_AUTODRIVE_PACE': 'authored', 'RR_DISABLE_RACER_CONTACTS': '0'}
    for item in args.env:
        key, sep, value = item.partition('=')
        if not sep or key != 'RR_AUDIO_ROOT' or not value:
            parser.error('Only RR_AUDIO_ROOT may be overridden')
        overrides[key] = value
    env = {k: v for k, v in os.environ.items() if not k.startswith('RR_')}
    env.update(overrides)
    evidence = NativeAiEvidence()
    with LEDGER.with_suffix('.lock').open('a+b') as lock:
        fcntl.flock(lock.fileno(), fcntl.LOCK_EX | fcntl.LOCK_NB)
        claim, ledger_raw = admit(args, out)
        receipt = {
            'schema': 'rr-native-ai-first-slice-capture-v1',
            'source_commit_label': args.source_commit, 'slot': claim,
            'scope': 'One direct GC process/session; actual eight racers; genuine AI in slot 1; capture and quit only',
            'target_max_seconds_after_ready': args.seconds, 'target_capture_fps': args.fps,
            'ready_window_includes_countdown': True, 'cold_start_limit_seconds': 360,
            'window_bound_is_capture_and_quit_request_not_hard_process_kill': True,
            'milestone_evidence_cutoff_is_ready_plus_12_seconds': True,
            'capture_ack_limit_seconds': 2, 'quit_grace_seconds': 15,
            'timing_basis': 'Actual monotonic acknowledgement-observation times, no invented frame rate',
            'tick_contract': 'Fresh source frame/render/native tick agreement; retained or unpaired images never count as fresh gameplay',
            'env_overrides': overrides, 'cleared_inherited_rr_variables': sorted(k for k in os.environ if k.startswith('RR_')),
            'binary_sha256': binary_hash, 'fixture_sha256': fixture_hash,
            'source_hashes_before': source_hashes, 'cwd': str(args.cwd),
            'required_runtime_guard_sha256': GUARD_SOURCE_SHA256,
            'binary_source_linkage_requires_parent_build_receipt': True,
            'gc_complete_world_admitted': False, 'completed_lap_claim': False,
            'input_note': 'Slot 1 takes original-AI path. Human and other racers retain existing HOST controls. No held input, state injection, menu, reset or restart command.',
            'no_encoding_during_capture': True, 'frames': [], 'errors': [],
            'preserve_failed_prefix': True, 'capture_complete': False,
        }
        current_ppm, wav_path = out / 'current-committed.ppm', out / 'actual-mix.wav'
        argv = [str(args.binary), str(args.fixture), '--headless-live', str(current_ppm)]
        receipt['argv'] = argv
        start = time.monotonic()
        receipt['utc_started'] = utc().isoformat()
        process = None
        pending = None
        awaiting = None
        identity = None
        ready_at = None
        next_request = None
        carry = b''
        audio_recording = None
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
                if command not in {'capture', 'quit'}:
                    raise RuntimeError('Only capture and quit are permitted')
                at = time.monotonic()
                process.stdin.write((command + '\n').encode('ascii'))
                process.stdin.flush()
                commands.write(json.dumps({'elapsed': at - start, 'command': command}) + '\n')
                commands.flush()
                return at

            def checkpoint():
                receipt['acceptance'] = evidence.report(frames)
                write_json(out / 'capture-receipt.json', receipt)
                write_json(out / 'acceptance-report.json', receipt['acceptance'])

            def consume(line, observed):
                nonlocal pending, awaiting, identity, ready_at, next_request, audio_recording
                event = {'elapsed': observed - start, 'line': line}
                events.write(json.dumps(event) + '\n')
                events.flush()
                evidence.observe(line, event['elapsed'])
                audio_match = AUDIO_RECORDING.fullmatch(line)
                if audio_match:
                    if (audio_recording is not None or int(audio_match.group(1)) != 1
                            or Path(audio_match.group(2)).resolve() != wav_path):
                        raise RuntimeError('Unexpected or repeated native audio session')
                    audio_recording = event
                    receipt['audio_recording'] = event
                match = COMMIT.match(line)
                if match:
                    if pending is None or awaiting is not None:
                        raise RuntimeError('Unpaired image acknowledgement')
                    data = current_ppm.read_bytes()
                    parts = data.split(b'\n', 3)
                    if len(parts) != 4 or parts[0] != b'P6' or parts[2] != b'255':
                        raise RuntimeError('Invalid committed actual PPM')
                    width, height = map(int, parts[1].split())
                    if (width, height) != (640, 480) or len(parts[3]) != width * height * 3:
                        raise RuntimeError('Incomplete or unexpected actual image dimensions')
                    name = 'frame-%06d.ppm' % len(frames)
                    with (out / name).open('xb') as target:
                        target.write(data)
                    frame = {
                        'file': name, 'tick': int(match.group(1)), 'rendered_tick': int(match.group(1)),
                        'retained_before_undertrack_pause': 'retained before undertrack pause' in line,
                        'request_elapsed': pending - start, 'ack_elapsed': observed - start,
                        'ack_latency_seconds': observed - pending,
                        'audio_sample_frames': live_audio_frames(wav_path),
                        'audio_anchor_observed_elapsed': time.monotonic() - start,
                        'width': width, 'height': height, 'sha256': hashlib.sha256(data).hexdigest(),
                        'commit_log': line, 'classification': 'unpaired_actual_image',
                    }
                    if frames:
                        frame['video_time_seconds'] = frame['ack_elapsed'] - frames[0]['ack_elapsed']
                    else:
                        frame['video_time_seconds'] = 0.0
                    frames.append(frame)
                    frame_log.write(json.dumps({'event': 'image_committed', 'frame': frame}) + '\n')
                    frame_log.flush()
                    awaiting, pending = frame, None
                    next_request = max(next_request + 1 / args.fps, observed + 0.001)
                if line.startswith('capture_state '):
                    state = CAPTURE_STATE.fullmatch(line)
                    if state is None or awaiting is None or 'capture_state' in awaiting:
                        raise RuntimeError('Unpaired or malformed committed-image state')
                    awaiting['capture_state'] = {'rendered_tick': int(state.group(2)),
                        'host_menu_image': state.group(3) == '1', 'wrong_way': state.group(1) == '1',
                        'observed_elapsed': observed - start, 'line': line}
                if line.startswith('host_ui_state '):
                    item = json.loads(line[len('host_ui_state '):])
                    identity = validate_ui(item, identity)
                    if item['reason'] == 'ready':
                        if ready_at is not None or not evidence.ready_admitted() or audio_recording is None:
                            raise RuntimeError('Direct GC ready lacks exact cold/session/source admission')
                        ready_at = observed
                        evidence.observation_deadline_elapsed = observed - start + args.seconds
                        next_request = observed
                        receipt['ready'] = dict(item, observed_elapsed=observed - start)
                        checkpoint()
                    else:
                        if awaiting is None:
                            raise RuntimeError('UI capture without actual committed image')
                        awaiting['ui_state'] = dict(item, observed_elapsed=observed - start)
                        if awaiting['retained_before_undertrack_pause']:
                            # Preserve source-retained image but never count it as moving gameplay.
                            state = awaiting.get('capture_state', {})
                            if (state.get('rendered_tick') != awaiting['tick']
                                    or state.get('host_menu_image') is not False
                                    or awaiting['tick'] > item['tick']):
                                raise RuntimeError('Retained-image association failed')
                            awaiting['native_tick'] = item['tick']
                            awaiting['classification'] = 'retained_after_native_boundary'
                            evidence.boundaries.append({'kind': 'retained_native_boundary_image',
                                'line': awaiting['commit_log'], 'observed_elapsed': observed - start})
                        else:
                            verify_capture_clocks(awaiting, item)
                            awaiting['classification'] = 'fresh_scene'
                        frame_log.write(json.dumps({'event': 'image_paired', 'frame': awaiting}) + '\n')
                        frame_log.flush()
                        awaiting = None
                        if len(frames) % 20 == 0 or evidence.boundaries:
                            checkpoint()

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
                        try:
                            consume(raw.decode('utf-8', errors='replace').rstrip('\r'), time.monotonic())
                        except Exception as error:
                            # Keep consuming this already-read batch so a following
                            # exact source boundary is never lost from the report.
                            errors.append('Evidence association: ' + repr(error))
                    now = time.monotonic()
                    if evidence.boundaries:
                        stop_reason = 'explicit_native_boundary'
                        break
                    if errors:
                        stop_reason = 'capture_or_admission_error'
                        break
                    if process.poll() is not None:
                        stop_reason = 'runtime_exited'
                        break
                    if ready_at is None:
                        if now - start > receipt['cold_start_limit_seconds']:
                            raise TimeoutError('Direct GC cold-start bound exceeded')
                    elif now - ready_at >= args.seconds:
                        stop_reason = 'bounded_ready_window_reached'
                        break
                    if pending is not None and now - pending > 2:
                        raise TimeoutError('Capture acknowledgement absent; preserve prefix without reusing the last PPM')
                    if awaiting is not None and now - start - awaiting['ack_elapsed'] > 2:
                        raise TimeoutError('Image state/UI pairing absent')
                    if ready_at is not None and pending is None and awaiting is None and now >= next_request:
                        pending = send('capture')
                    time.sleep(0.001)
            except (Exception, KeyboardInterrupt) as error:
                errors.append(repr(error))
                stop_reason = 'capture_or_admission_error'
            finally:
                receipt['forced_cleanup'] = []
                receipt['pending_capture_at_stop'] = pending is not None
                receipt['unpaired_image_at_stop'] = awaiting['file'] if awaiting else None
                receipt['stop_requested_elapsed'] = time.monotonic() - start
                if ready_at is not None:
                    receipt['ready_to_stop_request_seconds'] = time.monotonic() - ready_at
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
                # Preserve final buffered native evidence, including errors emitted on exit.
                carry += reader.read()
                tail = carry.split(b'\n')
                if tail and not tail[-1]:
                    tail.pop()
                for raw in tail:
                    try:
                        consume(raw.decode('utf-8', errors='replace').rstrip('\r'), time.monotonic())
                    except Exception as error:
                        errors.append('Final evidence association: ' + repr(error))
                if process is not None and process.stdin:
                    process.stdin.close()
                receipt.update(exit_code=process.returncode if process else None,
                    stop_reason=stop_reason, utc_finished=utc().isoformat(),
                    process_wall_seconds=time.monotonic() - start)
        if len(frames) >= 2:
            gaps = [b['ack_elapsed'] - a['ack_elapsed'] for a, b in zip(frames, frames[1:])]
            receipt.update(measured_image_window_seconds=sum(gaps), maximum_gap_seconds=max(gaps),
                           measured_capture_fps=len(gaps) / sum(gaps) if sum(gaps) > 0 else None,
                           unique_pixel_frames=len({f['sha256'] for f in frames}))
        audio_finalized = False
        try:
            if wav_path.is_file():
                with wave.open(str(wav_path), 'rb') as audio:
                    receipt['audio'] = {'file': wav_path.name, 'sha256': digest(wav_path),
                        'frames': audio.getnframes(), 'sample_rate': audio.getframerate(),
                        'sample_width': audio.getsampwidth(), 'channels': audio.getnchannels(),
                        'bytes': wav_path.stat().st_size}
                    audio_finalized = ((audio.getnchannels(), audio.getsampwidth(), audio.getframerate()) == (2, 2, 48000)
                        and wav_path.stat().st_size == 44 + 4 * audio.getnframes())
                    if not audio_finalized:
                        receipt['audio_inspection_error'] = 'Native WAV format/header is not finalized; original left untouched'
            else:
                receipt['audio_inspection_error'] = 'No finalized native WAV; no synthesis or header repair'
        except Exception as error:
            receipt['audio_inspection_error'] = repr(error)
        try:
            receipt['source_hashes_after'] = {str(path): digest(path) for path in source_files}
            receipt['binary_sha256_after'], receipt['fixture_sha256_after'] = digest(args.binary), digest(args.fixture)
            if (receipt['source_hashes_after'] != source_hashes
                    or receipt['binary_sha256_after'] != binary_hash
                    or receipt['fixture_sha256_after'] != fixture_hash):
                errors.append('Source, binary or fixture identity changed during capture')
        except Exception as error:
            errors.append('Final source identity inspection: ' + repr(error))
        acceptance = evidence.report(frames)
        clean = bool(not errors and receipt['exit_code'] == 0 and not receipt['forced_cleanup'])
        acceptance.update(clean_quit=clean,
            bounded_window_reached=stop_reason == 'bounded_ready_window_reached',
            runtime_acceptance_passed=bool(clean and not evidence.boundaries
                and stop_reason == 'bounded_ready_window_reached'
                and acceptance['minimum_first_slice_milestones_observed']))
        acceptance['outcome'] = ('explicit_unsupported_stop' if evidence.boundaries else
            'first_slice_milestones_observed' if acceptance['runtime_acceptance_passed'] else
            'incomplete_or_missing_evidence')
        receipt['acceptance'] = acceptance
        receipt['artifact_status'] = {
            'images': len(frames),
            'fresh_paired_images': sum(f.get('classification') == 'fresh_scene' for f in frames),
            'retained_images': sum(f.get('classification') == 'retained_after_native_boundary' for f in frames),
            'unpaired_images': sum(f.get('classification') == 'unpaired_actual_image' for f in frames),
            'unacknowledged_request_remaining': pending is not None,
            'native_pcm_finalized': audio_finalized,
            'prefix_evidence_retained': True,
        }
        # Runtime milestones and complete image/audio artifacts are different claims.
        receipt['capture_complete'] = bool(acceptance['runtime_acceptance_passed']
            and pending is None and awaiting is None and audio_finalized
            and all(f.get('classification') == 'fresh_scene' for f in frames))
        write_json(out / 'capture-receipt.json', receipt)
        write_json(out / 'acceptance-report.json', acceptance)
        claim.update(status='completed' if acceptance['runtime_acceptance_passed'] else 'failed',
                     finished_utc=utc().isoformat(), runtime_launches=1,
                     capture_receipt=str(out / 'capture-receipt.json'),
                     first_slice_outcome=acceptance['outcome'], completed_lap_claim=False)
        replace_ledger(ledger_raw, claim)
    print(json.dumps({'receipt': str(out / 'capture-receipt.json'),
                      'acceptance': str(out / 'acceptance-report.json'),
                      'outcome': acceptance['outcome'], 'frames': len(frames), 'errors': errors}), flush=True)
    return 0 if acceptance['runtime_acceptance_passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
