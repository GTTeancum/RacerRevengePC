#!/usr/bin/env python3
"""Artifact-only encoding of an existing dense capture; never launches the game."""
import argparse
import array
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys
import wave


def digest(path):
    h = hashlib.sha256()
    with path.open('rb') as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b''):
            h.update(block)
    return h.hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('take', type=Path)
    parser.add_argument('--root-requested-encode', action='store_true', required=True)
    parser.add_argument('--require-audio', action='store_true')
    parser.add_argument('--output', type=Path, required=True,
                        help='New artifact directory; the failed export is never overwritten')
    args = parser.parse_args()
    take = args.take.resolve()
    receipt = json.loads((take / 'capture-receipt.json').read_text())
    if receipt.get('schema') != 'rr-dense-gameplay-capture-v1' or not receipt.get('capture_complete'):
        parser.error('Requires a successfully completed dense real capture from this runner')
    frames = receipt['frames']
    if len(frames) < 2:
        parser.error('No complete frame interval')
    gaps = [b['ack_elapsed'] - a['ack_elapsed'] for a, b in zip(frames, frames[1:])]
    duration = sum(gaps)
    if (not 15 <= duration <= 30.5 or min(gaps) <= 0
            or len(gaps) / duration < 12 or max(gaps) > 0.25):
        parser.error('Sparse/incomplete capture refused: need 15–30.5 s, >=12 measured fps, no >250 ms gaps')
    if len({f['sha256'] for f in frames}) < len(frames) * 0.8:
        parser.error('Fewer than 80% unique actual pixel frames; do not present this as moving gameplay')
    if any(b['tick'] < a['tick'] for a, b in zip(frames, frames[1:])):
        parser.error('Tick regression suggests another session or invalid evidence')
    if frames[-1]['tick'] <= frames[0]['tick']:
        parser.error('No simulation advancement observed')
    size = (frames[0]['width'], frames[0]['height'])
    for frame in frames:
        path = take / frame['file']
        if (Path(frame['file']).name != frame['file'] or path.suffix != '.ppm'
                or (frame['width'], frame['height']) != size or digest(path) != frame['sha256']):
            parser.error('Frame identity/dimensions changed: ' + frame['file'])
    ffmpeg, ffprobe = shutil.which('ffmpeg'), shutil.which('ffprobe')
    if not ffmpeg or not ffprobe:
        parser.error('Existing ffmpeg and ffprobe are required; do not install or rerun gameplay')
    output = args.output.resolve()
    if output.exists():
        parser.error('Output directory already exists; preserve it and choose a new destination')
    audio_report = {'included': False}
    audio_bytes = None
    try:
        audio = receipt['audio']
        if Path(audio['file']).name != audio['file'] or Path(audio['file']).suffix != '.wav':
            raise ValueError('Invalid actual mixed WAV filename')
        wav_path = take / audio['file']
        if digest(wav_path) != audio['sha256']:
            raise ValueError('Original mixed WAV hash changed')
        with wave.open(str(wav_path), 'rb') as source:
            if (source.getnchannels(), source.getsampwidth(), source.getframerate()) != (2, 2, 48000):
                raise ValueError('Expected real 48 kHz stereo signed 16-bit PCM')
            if wav_path.stat().st_size != 44 + 4 * source.getnframes():
                raise ValueError('WAV is not fully finalized; no header repair is permitted')
            anchors = [f['audio_sample_frames'] for f in frames]
            if any(a is None for a in anchors) or any(b < a for a, b in zip(anchors, anchors[1:])):
                raise ValueError('Missing or nonmonotonic real audio anchors')
            if anchors[-1] > source.getnframes() or anchors[-1] <= anchors[0]:
                raise ValueError('Mixed WAV does not cover the video anchors')
            skews = [(sample - anchors[0]) / 48000 - (frame['ack_elapsed'] - frames[0]['ack_elapsed'])
                     for sample, frame in zip(anchors, frames)]
            audio_report.update(maximum_anchor_skew_seconds=max(abs(s) for s in skews),
                                final_duration_skew_seconds=skews[-1],
                                start_sample=anchors[0], end_sample=anchors[-1],
                                source_sha256=audio['sha256'])
            if max(abs(s) for s in skews) > 0.080 or abs(skews[-1]) > 0.050:
                raise ValueError('Simulation-timed PCM diverges from wall video; retained separately, never stretched')
            source.setpos(anchors[0])
            audio_bytes = source.readframes(anchors[-1] - anchors[0])
        samples = array.array('h')
        samples.frombytes(audio_bytes)
        if sys.byteorder != 'little':
            samples.byteswap()
        peak = max(abs(s) for s in samples)
        if peak == 0:
            raise ValueError('Mixed PCM interval is silent')
        audio_report.update(included=True, peak_absolute_sample=peak,
                            edit='Exact PCM sample-range trim; no synthesis, normalization, resampling, or stretching')
    except (KeyError, OSError, ValueError, wave.Error) as error:
        audio_bytes = None
        audio_report['omission_reason'] = str(error)
        if args.require_audio:
            parser.error('Real synchronized audio required: ' + str(error))
    output.mkdir()
    if audio_bytes is not None:
        with wave.open(str(output / 'video-mix.wav'), 'wb') as target:
            target.setnchannels(2)
            target.setsampwidth(2)
            target.setframerate(48000)
            target.writeframes(audio_bytes)
    # A 1 ms input time base preserves measured cadence. The final actual
    # captured frame marks the endpoint and remains visible for one millisecond.
    # No fps filter, motion interpolation, speed-up, or invented intermediate frames.
    # Disable B-frame reordering so the MP4 decoding and presentation timeline
    # have the same endpoint. The prior reordered export had correct PTS but
    # reported a container duration shorter than its final presentation time.
    concat = ['ffconcat version 1.0']
    for frame, gap in zip(frames[:-1], gaps):
        concat += ["file '%s'" % str(take / frame['file']).replace("'", "'\\''"),
                   'option framerate 1000', 'duration %.9f' % gap]
    concat += ["file '%s'" % str(take / frames[-1]['file']).replace("'", "'\\''"),
               'option framerate 1000']
    (output / 'frames.ffconcat').write_text('\n'.join(concat) + '\n')
    mp4 = output / 'gameplay.mp4'
    command = [ffmpeg, '-hide_banner', '-nostdin', '-n', '-f', 'concat', '-safe', '0',
               '-i', 'frames.ffconcat']
    if audio_bytes is not None:
        command += ['-i', 'video-mix.wav', '-map', '0:v:0', '-map', '1:a:0', '-c:a', 'aac', '-b:a', '192k']
    else:
        command += ['-map', '0:v:0', '-an']
    command += ['-fps_mode', 'vfr', '-enc_time_base', '1:1000',
                '-c:v', 'libx264', '-bf', '0', '-crf', '18', '-preset', 'medium', '-pix_fmt', 'yuv420p',
                '-video_track_timescale', '1000000', '-movflags', '+faststart', 'gameplay.mp4']
    report = {'capture_receipt': str(take / 'capture-receipt.json'),
              'timing_basis': receipt['timing_basis'], 'measured_video_seconds': duration,
              'measured_capture_fps': len(gaps) / duration, 'maximum_gap_seconds': max(gaps),
              'captured_frames': len(frames), 'visible_source_frames': len(frames),
              'endpoint_frame_duration_seconds': 0.001,
              'timestamp_quantization_seconds': 0.001,
              'timing_fix': 'No B-frame reordering; presentation and decoding endpoint agree',
              'artifact_only': True, 'new_game_runs': 0,
              'source_speed_adjustment': False, 'invented_or_repeated_source_frames': 0,
              'track_label': receipt['track_label'], 'audio': audio_report, 'ffmpeg_argv': command,
              'verified': False, 'status': 'encoding'}
    report_path = output / 'encode-receipt.json'
    report_path.write_text(json.dumps(report, indent=2) + '\n')
    with (output / 'ffmpeg.log').open('xb') as log:
        subprocess.run(command, cwd=output, stdout=log, stderr=log, check=True)
    probe = subprocess.run([ffprobe, '-v', 'error', '-show_format', '-show_streams',
                            '-show_frames', '-of', 'json', str(mp4)],
                           capture_output=True, text=True, check=True)
    (output / 'ffprobe.json').write_text(probe.stdout)
    info = json.loads(probe.stdout)
    video = next(s for s in info['streams'] if s['codec_type'] == 'video')
    video_frames = [f for f in info['frames'] if f.get('media_type') == 'video']
    actual_pts = [float(f['best_effort_timestamp_time']) for f in video_frames]
    expected_pts = [f['ack_elapsed'] - frames[0]['ack_elapsed'] for f in frames]
    problems = []
    if len(actual_pts) != len(expected_pts):
        problems.append('Encoded frame count differs from actual visible captures')
    elif max(abs(a - b) for a, b in zip(actual_pts, expected_pts)) > 0.003:
        problems.append('Encoded frame times differ by more than 3 ms from measured cadence')
    actual_duration = float(video.get('duration', info['format']['duration']))
    format_duration = float(info['format']['duration'])
    if video.get('has_b_frames', 0) != 0:
        problems.append('Unexpected B-frame reordering')
    if actual_pts and actual_duration < actual_pts[-1]:
        problems.append('Container ends before the final real source frame')
    if abs(format_duration - actual_duration) > 0.001:
        problems.append('Container and stream duration disagree')
    if abs(actual_duration - (duration + 0.001)) > 0.005:
        problems.append('Encoded duration does not match the measured window')
    if bool(any(s['codec_type'] == 'audio' for s in info['streams'])) != audio_report['included']:
        problems.append('Audio stream presence differs from the evidence')
    # JPEG comes from one real source frame, not a composited or generated image.
    preview = frames[(len(frames) - 1) // 2]
    with (output / 'jpeg.log').open('xb') as log:
        subprocess.run([ffmpeg, '-hide_banner', '-nostdin', '-n', '-i', str(take / preview['file']),
                        '-frames:v', '1', '-q:v', '2', str(output / 'preview.jpg')],
                       stdout=log, stderr=log, check=True)
    report.update(verified=not problems, status='verified' if not problems else 'verification_failed',
                  problems=problems, encoded_duration_seconds=actual_duration,
                  encoded_video_frames=len(actual_pts), video_sha256=digest(mp4),
                  maximum_frame_time_error_seconds=max(abs(a - b) for a, b in zip(actual_pts, expected_pts)),
                  encoded_format_duration_seconds=format_duration,
                  last_frame_pts_seconds=actual_pts[-1],
                  encoded_endpoint_frame_duration_seconds=actual_duration-actual_pts[-1],
                  preview_source=preview['file'], preview_source_time=preview['video_time_seconds'],
                  preview_sha256=digest(output / 'preview.jpg'))
    report_path.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({'mp4': str(mp4), 'jpeg': str(output / 'preview.jpg'),
                      'receipt': str(report_path), 'verified': not problems, 'audio': audio_report}))
    return 0 if not problems else 1


if __name__ == '__main__':
    raise SystemExit(main())
