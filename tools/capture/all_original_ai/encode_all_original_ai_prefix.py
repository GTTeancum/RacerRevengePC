#!/usr/bin/env python3
"""STAGED, UNEXECUTED: artifact-only export of an existing all-eight-original-AI capture prefix."""
import argparse
import array
import hashlib
import json
import math
from pathlib import Path
import shutil
import subprocess
import sys
import wave


SCHEMA = 'rr-all-original-ai-capture-v1'
ENDPOINT_SECONDS = 0.001


def digest(path):
    result = hashlib.sha256()
    with path.open('rb') as source:
        for block in iter(lambda: source.read(1024 * 1024), b''):
            result.update(block)
    return result.hexdigest()


def finite(value):
    return type(value) in (int, float) and math.isfinite(value)


def source_file(take, name, suffix):
    if (not isinstance(name, str) or Path(name).name != name or Path(name).suffix != suffix
            or '\n' in name or '\r' in name):
        raise ValueError('Unsafe or unexpected source filename')
    path = take / name
    if not path.is_file() or path.resolve().parent != take:
        raise ValueError('Source file is missing or resolves outside the take: ' + name)
    return path


def concat_quote(path):
    # ffconcat uses shell-like single quoting, without invoking a shell.
    value = str(path)
    if '\n' in value or '\r' in value:
        raise ValueError('Newlines are not supported in source paths')
    return "'" + value.replace("'", "'\\''") + "'"


def write_report(output, report):
    (output / 'encode-receipt.json').write_text(json.dumps(report, indent=2) + '\n')


def select_prefix(take, receipt):
    if receipt.get('target_max_seconds_after_ready') != 12:
        raise ValueError('Expected the direct-GC 12-second ready-window contract')
    # A cold failure may legitimately finish with no ready event or images.
    # Preserve that failure as a report; it cannot become a JPEG or video.
    if receipt.get('frames') == []:
        return [], [], None
    ready = receipt.get('ready', {})
    origin = ready.get('observed_elapsed')
    if not finite(origin):
        raise ValueError('Actual images lack a valid direct-GC ready observation')
    if (ready.get('session') != 1 or ready.get('loaded_track') != 'GC'
            or ready.get('selected_track') != 'GC' or ready.get('menu') is not False
            or ready.get('page') != 0 or ready.get('loaded_racer') != 1
            or ready.get('selected_racer') != 1 or ready.get('track_pending') is not False
            or ready.get('gc_incomplete_opt_in') is not True
            or ready.get('gc_mode2_admitted') is not True):
        raise ValueError('Capture lacks actual direct-GC/session-1 ready identity')
    identity = (ready['session'], ready.get('audio_bank_id'))
    if type(identity[1]) is not int or identity[1] <= 0:
        raise ValueError('Missing actual native audio-bank identity')
    cutoff = origin + 12
    acceptance = receipt.get('acceptance', {})
    boundaries = acceptance.get('explicit_boundaries', [])
    for boundary in boundaries + acceptance.get('contract_violations', []):
        when = boundary.get('observed_elapsed')
        if not finite(when):
            raise ValueError('Invalid source-boundary or input-policy observation timestamp')
        cutoff = min(cutoff, when)
    stop = receipt.get('stop_requested_elapsed')
    if finite(stop):
        cutoff = min(cutoff, stop)
    frames = receipt.get('frames', [])
    if not isinstance(frames, list):
        raise ValueError('Invalid frame receipt')
    selected, excluded = [], []
    stopped = False
    seen = set()
    for index, frame in enumerate(frames):
        reason = None
        ack = frame.get('ack_elapsed')
        if stopped:
            reason = 'After first excluded image; never splice across an evidence hole'
        elif frame.get('classification') != 'fresh_scene':
            reason = 'Retained, unpaired, or otherwise non-fresh image'
        elif not finite(ack) or ack < origin:
            reason = 'Invalid or pre-ready acknowledgement timestamp'
        elif ack >= cutoff:
            reason = 'At or after first source boundary/input-policy violation, stop request, or ready-window cutoff'
        if reason:
            stopped = True
            excluded.append({'index': index, 'file': frame.get('file'), 'reason': reason})
            continue
        path = source_file(take, frame.get('file'), '.ppm')
        if frame['file'] in seen:
            raise ValueError('Source filename repeated in capture receipt')
        seen.add(frame['file'])
        if digest(path) != frame.get('sha256'):
            raise ValueError('Actual image hash differs from capture receipt: ' + frame['file'])
        state, ui = frame.get('capture_state', {}), frame.get('ui_state', {})
        tick = frame.get('tick')
        if (type(tick) is not int or tick < 0 or frame.get('rendered_tick') != tick
                or frame.get('native_tick') != tick or state.get('rendered_tick') != tick
                or ui.get('tick') != tick or state.get('host_menu_image') is not False
                or frame.get('retained_before_undertrack_pause') is not False
                or frame.get('tick_relation') != 'fresh_scene_equals_native'
                or (ui.get('session'), ui.get('audio_bank_id')) != identity
                or ui.get('reason') != 'capture' or ui.get('menu') is not False
                or ui.get('page') != 0 or ui.get('loaded_track') != 'GC'
                or ui.get('selected_track') != 'GC' or ui.get('track_pending') is not False
                or ui.get('loaded_racer') != 1 or ui.get('selected_racer') != 1
                or ui.get('gc_incomplete_opt_in') is not True or ui.get('gc_mode2_admitted') is not True):
            raise ValueError('Fresh image/render/native/session association changed: ' + frame['file'])
        if selected and (ack <= selected[-1]['ack_elapsed'] or tick < selected[-1]['tick']):
            raise ValueError('Actual source observation time or native tick regressed')
        with path.open('rb') as image:
            header = [image.readline() for _ in range(3)]
        if (header != [b'P6\n', b'640 480\n', b'255\n']
                or frame.get('width') != 640 or frame.get('height') != 480
                or path.stat().st_size != sum(map(len, header)) + 640 * 480 * 3):
            raise ValueError('Unexpected or incomplete actual PPM: ' + frame['file'])
        selected.append(frame)
    return selected, excluded, cutoff


def select_audio(take, receipt, frames):
    report = {'included': False, 'codec_if_included': 'AAC 192 kb/s from exact native PCM sample trim',
              'maximum_allowed_anchor_skew_seconds': 0.080,
              'maximum_allowed_final_skew_seconds': 0.050}
    try:
        if len(frames) < 2:
            raise ValueError('No measured video interval')
        audio = receipt['audio']
        path = source_file(take, audio['file'], '.wav')
        if digest(path) != audio['sha256']:
            raise ValueError('Original mixed WAV hash changed')
        with wave.open(str(path), 'rb') as source:
            if (source.getnchannels(), source.getsampwidth(), source.getframerate()) != (2, 2, 48000):
                raise ValueError('Expected native 48 kHz stereo signed 16-bit PCM')
            if path.stat().st_size != 44 + 4 * source.getnframes():
                raise ValueError('Native WAV is not fully finalized; never repair its header')
            anchors = [frame.get('audio_sample_frames') for frame in frames]
            if any(type(a) is not int or a < 0 for a in anchors):
                raise ValueError('Missing actual integer PCM anchors')
            if any(b < a for a, b in zip(anchors, anchors[1:])):
                raise ValueError('Native PCM anchors regressed')
            if anchors[-1] > source.getnframes() or anchors[-1] <= anchors[0]:
                raise ValueError('Finalized native PCM does not cover a nonempty anchored video interval')
            skews = [(sample - anchors[0]) / 48000 - (frame['ack_elapsed'] - frames[0]['ack_elapsed'])
                     for sample, frame in zip(anchors, frames)]
            report.update(source_file=audio['file'], source_sha256=audio['sha256'],
                          start_sample=anchors[0], end_sample=anchors[-1],
                          maximum_anchor_skew_seconds=max(abs(value) for value in skews),
                          final_duration_skew_seconds=skews[-1],
                          exact_trim_duration_seconds=(anchors[-1] - anchors[0]) / 48000)
            if max(abs(value) for value in skews) > 0.080 or abs(skews[-1]) > 0.050:
                raise ValueError('Native PCM diverges from wall video; omit it rather than stretch/resample/synthesize')
            source.setpos(anchors[0])
            data = source.readframes(anchors[-1] - anchors[0])
            if len(data) != (anchors[-1] - anchors[0]) * 4:
                raise ValueError('Native PCM sample range could not be read completely')
        samples = array.array('h')
        samples.frombytes(data)
        if sys.byteorder != 'little':
            samples.byteswap()
        peak = max(abs(sample) for sample in samples)
        if peak == 0:
            raise ValueError('Actual anchored PCM is silent')
        report.update(included=True, peak_absolute_sample=peak,
                      edit='Exact anchored sample-range trim; AAC encoding only, no synthesis, normalization, resampling or stretching')
        return report, data
    except (KeyError, OSError, TypeError, ValueError, wave.Error) as error:
        report['omission_reason'] = str(error)
        return report, None


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('take', type=Path)
    parser.add_argument('--root-requested-encode', action='store_true', required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--require-audio', action='store_true')
    args = parser.parse_args()
    take, output = args.take.resolve(), args.output.resolve()
    receipt_path = take / 'capture-receipt.json'
    receipt = json.loads(receipt_path.read_text())
    receipt_hash = digest(receipt_path)
    if receipt.get('schema') != SCHEMA or not receipt.get('utc_finished'):
        parser.error('Requires a finished direct-GC all-eight-original-AI capture receipt')
    expected = {'RR_TRACK': 'GC', 'RR_RACE_MODE': '2', 'RR_GC_INCOMPLETE_WORLD': '1',
                'RR_RACE': '1', 'RR_ORIGINAL_AI_ALL_RACERS': '1', 'RR_AUTODRIVE': '0', 'RR_START_MENU': '0'}
    if any(receipt.get('env_overrides', {}).get(key) != value for key, value in expected.items()):
        parser.error('Source receipt does not match the reviewed all-eight-AI/no-HOST-input configuration')
    if (receipt.get('driving_policy') != 'all_eight_original_ai_only'
            or receipt.get('human_controller_count') != 0 or receipt.get('camera_slot') != 0
            or receipt.get('expected_camera_racer60') != 1):
        parser.error('Source receipt lacks the explicit all-eight ownership/camera-identity contract')
    if output.exists():
        parser.error('Output must be a new directory; preserve all source and earlier exports')
    try:
        frames, excluded, cutoff = select_prefix(take, receipt)
    except (KeyError, OSError, TypeError, ValueError) as error:
        parser.error(str(error))
    acceptance = receipt.get('acceptance', {})
    boundaries = acceptance.get('explicit_boundaries', [])
    source_stopped = bool(boundaries or acceptance.get('unsupported_stop_observed'))
    violations = acceptance.get('contract_violations', [])
    ownership_proven = acceptance.get('bounded_all_ai_ownership_acceptance_passed') is True
    if violations:
        label = 'ownership-violation-prefix'
    elif source_stopped:
        label = 'source-boundary-prefix'
    elif (acceptance.get('runtime_acceptance_passed') is True and receipt.get('capture_complete') is True
          and not receipt.get('errors') and not excluded and len(frames) >= 2):
        label = 'bounded-capture'
    else:
        label = 'incomplete-prefix'
    title = 'GC requested all-original-AI ' + label.replace('-', ' ')
    disclosure = (title + '. Actual captured images; incomplete GC world. '
                  'Configuration requests genuine original AI in all eight racers including camera slot 0, with no HOST/simulated driving input. '
                  'No completed lap, full original AI, or complete GC claim.')
    if ownership_proven:
        disclosure += ' The source receipt establishes all-eight AI ownership and native progress up to its reported stop.'
    else:
        disclosure += ' All-eight ownership/progress is not established by this artifact or its source receipt.'
    if violations:
        disclosure += ' An all-AI/input-policy violation was recorded; only earlier eligible images may be exported.'
    if source_stopped:
        disclosure += ' A real source boundary stopped this take; no continuation was fabricated.'
    if label == 'incomplete-prefix':
        disclosure += ' This is an incomplete captured prefix; runtime milestone results are reported separately.'
    duration = frames[-1]['ack_elapsed'] - frames[0]['ack_elapsed'] if len(frames) >= 2 else 0.0
    gaps = [b['ack_elapsed'] - a['ack_elapsed'] for a, b in zip(frames, frames[1:])]
    audio_report, audio_bytes = select_audio(take, receipt, frames)
    if args.require_audio and audio_bytes is None:
        parser.error('Synchronized real audio required: ' + audio_report['omission_reason'])
    warnings = []
    if len(frames) >= 2:
        if len(gaps) / duration < 12 or max(gaps) > 0.25:
            warnings.append('Sparse sampled-image prefix; recorded gaps are retained, not filled with invented frames')
        if frames[-1]['tick'] <= frames[0]['tick']:
            warnings.append('No native tick advancement in selected images; this may show countdown/setup only')
    if audio_bytes is None:
        disclosure += ' Silent export: ' + audio_report['omission_reason'] + '.'
    report = {
        'schema': 'rr-all-original-ai-prefix-export-v1', 'artifact_only': True,
        'capture_receipt': str(receipt_path), 'capture_receipt_sha256': receipt_hash,
        'source_capture_schema': SCHEMA, 'source_capture_complete': receipt.get('capture_complete'),
        'source_runtime_acceptance_passed': acceptance.get('runtime_acceptance_passed'),
        'source_bounded_all_ai_ownership_acceptance_passed': ownership_proven,
        'source_all_eight_driving_evidence_satisfied': acceptance.get('all_eight_driving_evidence_satisfied'),
        'source_per_racer_observations': acceptance.get('per_racer', []),
        'source_contract_violations': violations,
        'source_driving_policy': receipt.get('driving_policy'),
        'source_integrated_commit': receipt.get('source_commit_label'),
        'source_outcome': acceptance.get('outcome'), 'source_stop_reason': receipt.get('stop_reason'),
        'source_exit_code': receipt.get('exit_code'), 'source_errors': receipt.get('errors', []),
        'source_artifact_status': receipt.get('artifact_status', {}),
        'source_milestones': acceptance.get('milestones', {}), 'explicit_boundaries': boundaries,
        'label': label, 'disclosure': disclosure, 'warnings': warnings,
        'completed_lap_claim': False, 'full_original_ai_or_full_gc_claim': False,
        'captured_images_total': len(receipt.get('frames', [])), 'selected_source_frames': len(frames),
        'selected_files': [frame['file'] for frame in frames], 'excluded_images': excluded,
        'selection': 'Contiguous original-order fresh paired prefix strictly before first source boundary/input-policy violation, stop request and ready+12 s; never splice',
        'selection_cutoff_elapsed': cutoff,
        'first_selected_ack_elapsed': frames[0]['ack_elapsed'] if frames else None,
        'last_selected_ack_elapsed': frames[-1]['ack_elapsed'] if frames else None,
        'measured_video_seconds': duration,
        'measured_capture_fps': len(gaps) / duration if duration > 0 else None,
        'maximum_gap_seconds': max(gaps) if gaps else None,
        'source_timing_basis': receipt.get('timing_basis'), 'timestamp_quantization_seconds': 0.001,
        'endpoint_frame_duration_seconds': ENDPOINT_SECONDS,
        'source_speed_adjustment': False, 'invented_or_added_source_frames': 0,
        'source_image_duplicates_retained_as_captured': True,
        'audio': audio_report, 'verified': False, 'status': 'preparing',
        'new_game_runs': 0, 'runtime_acceptance_not_reassessed_by_encoder': True,
    }
    output.mkdir(parents=True, exist_ok=False)
    write_report(output, report)
    (output / 'DISCLOSURE.txt').write_text(disclosure + '\n' + '\n'.join(warnings) + '\n')
    if not frames:
        report.update(status='no_eligible_actual_image', problems=['No safe source image to export'])
        write_report(output, report)
        print(json.dumps({'receipt': str(output / 'encode-receipt.json'), 'status': report['status']}))
        return 2
    ffmpeg, ffprobe = shutil.which('ffmpeg'), shutil.which('ffprobe')
    if not ffmpeg or not ffprobe:
        report.update(status='blocked_missing_existing_ffmpeg_or_ffprobe')
        write_report(output, report)
        parser.error('Existing ffmpeg/ffprobe required; do not install or relaunch the game')
    try:
        preview_index = (len(frames) - 1) // 2
        preview = frames[preview_index]
        jpeg = output / ('gc-all-original-ai-' + label + '.jpg')
        with (output / 'jpeg.log').open('xb') as log:
            subprocess.run([ffmpeg, '-hide_banner', '-nostdin', '-n', '-i', str(take / preview['file']),
                            '-frames:v', '1', '-q:v', '2', str(jpeg)], stdout=log, stderr=log, check=True)
        report.update(preview=str(jpeg), preview_source=preview['file'],
                      preview_source_ack_elapsed=preview['ack_elapsed'], preview_sha256=digest(jpeg))
        if len(frames) < 2:
            report.update(status='jpeg_only_no_measured_video_interval', problems=['Only one eligible image; no fabricated MP4 duration'])
            write_report(output, report)
            print(json.dumps({'jpeg': str(jpeg), 'receipt': str(output / 'encode-receipt.json'), 'status': report['status']}))
            return 2
        expected_pts = [frame['ack_elapsed'] - frames[0]['ack_elapsed'] for frame in frames]
        rounded_pts = [round(value * 1000) for value in expected_pts]
        if any(b <= a for a, b in zip(rounded_pts, rounded_pts[1:])):
            raise ValueError('Two actual observations collide at the 1 ms encoder timebase; do not alter timing or drop frames')
        # Write differences of rounded absolute PTS: no accumulation of tiny
        # per-gap truncation errors in the concat demuxer.
        concat = ['ffconcat version 1.0']
        for index, frame in enumerate(frames):
            concat += ['file ' + concat_quote(take / frame['file']), 'option framerate 1000']
            if index + 1 < len(frames):
                concat.append('duration %.3f' % ((rounded_pts[index + 1] - rounded_pts[index]) / 1000))
        (output / 'frames.ffconcat').write_text('\n'.join(concat) + '\n')
        if audio_bytes is not None:
            with wave.open(str(output / 'video-mix.wav'), 'wb') as target:
                target.setnchannels(2)
                target.setsampwidth(2)
                target.setframerate(48000)
                target.writeframes(audio_bytes)
            report['audio']['exact_trim_wav_sha256'] = digest(output / 'video-mix.wav')
        mp4 = output / ('gc-all-original-ai-' + label + '.mp4')
        command = [ffmpeg, '-hide_banner', '-nostdin', '-n', '-f', 'concat', '-safe', '0', '-i', 'frames.ffconcat']
        if audio_bytes is not None:
            command += ['-i', 'video-mix.wav', '-map', '0:v:0', '-map', '1:a:0', '-c:a', 'aac', '-b:a', '192k']
        else:
            command += ['-map', '0:v:0', '-an']
        command += ['-fps_mode', 'vfr', '-enc_time_base', '1:1000', '-c:v', 'libx264', '-bf', '0',
                    '-crf', '18', '-preset', 'medium', '-pix_fmt', 'yuv420p',
                    '-video_track_timescale', '1000000', '-movflags', '+faststart',
                    '-metadata', 'title=' + title, '-metadata', 'comment=' + disclosure, mp4.name]
        report.update(status='encoding', ffmpeg_argv=command)
        write_report(output, report)
        with (output / 'ffmpeg.log').open('xb') as log:
            subprocess.run(command, cwd=output, stdout=log, stderr=log, check=True)
        probe = subprocess.run([ffprobe, '-v', 'error', '-show_format', '-show_streams', '-show_frames',
                                '-of', 'json', str(mp4)], capture_output=True, text=True, check=True)
        (output / 'ffprobe.json').write_text(probe.stdout)
        info = json.loads(probe.stdout)
        video = next(stream for stream in info['streams'] if stream['codec_type'] == 'video')
        actual = [float(frame['best_effort_timestamp_time']) for frame in info['frames'] if frame.get('media_type') == 'video']
        video_duration, container_duration = float(video['duration']), float(info['format']['duration'])
        audio_streams = [stream for stream in info['streams'] if stream['codec_type'] == 'audio']
        problems = []
        timing_error = max((abs(a - b) for a, b in zip(actual, expected_pts)), default=None)
        if len(actual) != len(frames):
            problems.append('Encoded frame count differs from selected real images')
        if timing_error is None or timing_error > 0.0011:
            problems.append('Encoded presentation times differ from measured source times')
        if abs(video_duration - (duration + ENDPOINT_SECONDS)) > 0.002:
            problems.append('Video-track duration differs from actual window plus disclosed 1 ms endpoint')
        if video.get('has_b_frames', 0) or not actual or video_duration < actual[-1]:
            problems.append('Reordering or premature video endpoint')
        if bool(audio_streams) != audio_report['included']:
            problems.append('Audio stream presence differs from anchored-PCM evidence')
        # AAC may add a small packet tail. Do not shorten/rescale source images
        # or audio to disguise it: report video and container durations separately.
        allowed_tail = 0.075 if audio_report['included'] else 0.001
        if container_duration < video_duration - 0.001 or container_duration - video_duration > allowed_tail:
            problems.append('Unexpected mux duration outside disclosed source-anchor/AAC tail allowance')
        if audio_streams and abs(float(audio_streams[0]['duration']) - audio_report['exact_trim_duration_seconds']) > 0.025:
            problems.append('Encoded audio duration differs unexpectedly from exact native sample trim')
        if digest(receipt_path) != receipt_hash or any(digest(take / frame['file']) != frame['sha256'] for frame in frames):
            problems.append('Source receipt or selected image changed during export')
        if audio_report['included'] and digest(take / audio_report['source_file']) != audio_report['source_sha256']:
            problems.append('Source native PCM changed during export')
        report.update(status='artifact_verified' if not problems else 'artifact_verification_failed',
                      verified=not problems, problems=problems, mp4=str(mp4), video_sha256=digest(mp4),
                      encoded_video_frames=len(actual), maximum_frame_time_error_seconds=timing_error,
                      encoded_video_duration_seconds=video_duration, encoded_container_duration_seconds=container_duration,
                      mux_tail_seconds_after_video=container_duration - video_duration,
                      encoded_audio_duration_seconds=float(audio_streams[0]['duration']) if audio_streams else None,
                      encoded_final_video_frame_duration_seconds=video_duration - actual[-1] if actual else None)
        write_report(output, report)
        print(json.dumps({'mp4': str(mp4), 'jpeg': str(jpeg), 'receipt': str(output / 'encode-receipt.json'),
                          'disclosure': str(output / 'DISCLOSURE.txt'), 'artifact_verified': not problems,
                          'source_label': label, 'audio': audio_report}))
        return 0 if not problems else 1
    except Exception as error:
        report.update(status='artifact_preparation_failed', verified=False, error=repr(error))
        write_report(output, report)
        raise


if __name__ == '__main__':
    raise SystemExit(main())
