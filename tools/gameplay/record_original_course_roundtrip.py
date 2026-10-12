#!/usr/bin/env python3
"""One muted Main/Rider19/Course/Back observation; corrected launch, unexecuted.

Exactly three real host menu presses; never accepts caller-supplied stdin.
The original snapshot ran on 2026-10-12 and failed before Ready because it
explicitly enabled race-only selected lighting in Main mode. This revision
omits that override; the runtime chooses its own mode-appropriate default.
No successful menu observation is claimed. Root owns the centralized slot.
"""
import argparse
import datetime as dt
import hashlib
import json
import math
import os
from pathlib import Path
import re
import resource
import selectors
import signal
import stat
import subprocess
import sys
import time

REPO = Path('/workspace/shared/rr-recovery-git')
ASSETS = Path('/workspace/shared/recovery-20261010/native-assets-prepared-01')
STAGE = Path('/workspace/shared/recovery-20261012/course-validation-03')
CORPUS = Path('evidence/staticType2Producer49X/original-word/native-fixtures.json')
EARLIEST = dt.datetime.fromisoformat('2026-10-12T17:22:32.320233+00:00')
HEADER = b'P6\n640 448\n255\n'
IMAGE_BYTES = len(HEADER) + 640 * 448 * 3
MAX_FRAMES = 920
MAX_LOG = 32 * 1024 * 1024
MAX_LINE = 256 * 1024
MAX_JOURNAL = 16 * 1024 * 1024
WINDOW_NS = 30_000_000_000
# Each checkpoint has its own actual owner lifetime. GPU tokens and Rider
# source frame generations are NOT globally unique across a fresh owner.
ROUTES = ((4, 1, 0), (5, 1, 1), (6, 1, 2), (5, 1, 3))
ACTIONS = (('main_enter_rider19', 'Enter'),
           ('rider19_confirm_course', 'Enter'),
           ('course_back_rider19', 'Escape'))
DWELL_NS = (0, 1_000_000_000, 3_000_000_000, 1_000_000_000)
PREFIXES = {4: 'original_main_state ', 5: 'original_rider_state ',
            6: 'original_course_state '}
UI_FILES = ('APPSTART.RES', 'LOC.RES', 'UI/TEXTURES/OVERLAYS.RES',
            'UI/UIFONTS.RES', 'UI/UIMAIN/UIMAIN.RES', 'UI/UIBKG.RES',
            'UI/CONTPOP.RES', 'UI/UICOURSE/UICOURSE.RES',
            'UI/UIRIDER/UIRIDER.RES', 'UI/UIOPTION/UIOPTION.RES',
            'UI/UIPODTEX.RES', 'UI/PODPHYS.RES')


def utc():
    return dt.datetime.now(dt.timezone.utc).isoformat()


def require(condition, message):
    if not condition:
        raise ValueError(message)


def integer(row, name):
    value = row.get(name)
    require(type(value) is int and value >= 0, 'Invalid integer: ' + name)
    return value


def identity(path, maximum=1024 * 1024 * 1024, kernel_exe=False):
    require(kernel_exe or not path.is_symlink(), 'Symlink rejected: ' + str(path))
    before = path.stat()
    require(stat.S_ISREG(before.st_mode) and 0 < before.st_size <= maximum,
            'Missing, empty, nonregular or oversized file: ' + str(path))
    digest = hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            digest.update(block)
    after = path.stat()
    keys = ('st_dev', 'st_ino', 'st_size', 'st_mtime_ns', 'st_ctime_ns')
    require(all(getattr(before, k) == getattr(after, k) for k in keys),
            'File changed while hashing: ' + str(path))
    return {'path': str(path), 'sha256': digest.hexdigest(),
            'stat': {k: getattr(after, k) for k in keys}}


def save_json(path, value):
    payload = (json.dumps(value, indent=2, sort_keys=True) + '\n').encode()
    require(len(payload) <= 2 * 1024 * 1024, 'JSON evidence exceeded 2 MiB')
    temporary = path.with_name(path.name + '.part')
    with temporary.open('xb') as stream:
        stream.write(payload)
        stream.flush()
        os.fsync(stream.fileno())
    os.replace(temporary, path)


def child_limits():
    resource.setrlimit(resource.RLIMIT_FSIZE, (8 * 1024 * 1024,) * 2)
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))


class Recorder:
    def __init__(self, args, manifest):
        self.args, self.manifest, self.out = args, manifest, args.output
        self.proc = None
        self.selector = selectors.DefaultSelector()
        self.raw = (self.out / 'game.log').open('xb', buffering=0)
        self.journal = (self.out / 'frames.jsonl').open('x', encoding='utf-8')
        self.events = (self.out / 'events.jsonl').open('x', encoding='utf-8')
        self.raw_bytes = self.journal_bytes = self.event_bytes = self.stdin_bytes = 0
        self.buffer = b''
        self.eof = False
        self.launch_ns = self.ready_ns = self.quit_ns = self.term_ns = self.kill_ns = None
        self.last_request_ns = self.navigation_deadline = None
        self.pending = self.last_frame = None
        self.requests = self.acks = self.frames = self.presses = self.releases = 0
        self.held = None
        self.reason = None
        self.problems = []
        self.signals = []
        self.ready = self.closed = self.quit_state = None
        self.checkpoints = {}
        self.owner_tokens = {}
        self.rider_generations = {}
        self.attached_lifetimes = set()
        self.activations = []
        self.terminal_receipts = []
        self.interrupted = None

    def event(self, kind, **fields):
        payload = json.dumps({'kind': kind, 'monotonic_ns': time.monotonic_ns(), **fields}) + '\n'
        size = len(payload.encode())
        require(self.event_bytes + size <= MAX_JOURNAL, 'Event journal bound exceeded')
        self.events.write(payload)
        self.events.flush()
        self.event_bytes += size

    def fail(self, message):
        if len(self.problems) < 32:
            self.problems.append(str(message)[:4096])
        if self.reason is None or self.reason == 'sequence_complete':
            self.reason = 'failed'

    @staticmethod
    def route(row):
        return tuple(integer(row, k) for k in ('screen', 'page', 'switches'))

    @staticmethod
    def open_input(row):
        return (row.get('input_shown') is True and row.get('aperture_input_enabled') is True
                and row.get('aperture_state') == 0 and row.get('lifecycle') == 0
                and row.get('phase') == 0
                and (row['screen'] != 5 or
                     (row.get('input_blocked') is False and row.get('panel_y') == 0)))

    def check_state(self, row, terminal=False):
        require(isinstance(row, dict), 'State must be an object')
        route = self.route(row)
        require(route in ROUTES, 'Unexpected menu route or owner lifetime')
        require(integer(row, 'world_ticks') == 0 and integer(row, 'mixed_frames') == 0,
                'Gameplay tick/audio activity appeared')
        require(row.get('audio_muted') is True, 'Actual UI audio owner was not muted')
        require(integer(row, 'unavailable_destinations') == 0, 'A requested destination was unavailable')
        # Explicit mute is allowed to suppress authored effects. It owns no PCM
        # output; its accepted-audio clock remains zero and is NOT a dwell clock.
        if row['screen'] in (4, 5):
            for key in ('audio_mixed_frames', 'audio_source_mixed_frames', 'audio_captured_frames',
                        'audio_accepted_nanoseconds', 'audio_clock_microseconds',
                        'audio_device_consumed_frames', 'audio_device_queued_frames', 'audio_failed',
                        'audio_submitted', 'audio_remainder_nanoseconds', 'audio_rate',
                        'audio_device_underrun_frames', 'audio_device_overflow_frames'):
                require(integer(row, key) == 0, 'Muted output unexpectedly active: ' + key)
            require(row.get('audio_device_status') == 'not_requested', 'Muted UI device was requested')
            if row['screen'] == 5:
                require(integer(row, 'audio_music_song') == 0, 'Muted Rider music transport exists')
        for key in ('input_events', 'updates', 'renders', 'submitted', 'source_milliseconds',
                    'gpu_publication', 'rider_profile_generation', 'light_manager_generation'):
            integer(row, key)
        require(row['input_events'] == self.presses, 'Input count differs from real issued presses')
        allowed = {ROUTES[self.presses]}
        if self.presses:
            allowed.add(ROUTES[self.presses - 1])
        require(route in allowed, 'Route does not belong to the current transition')
        requested = (integer(row, 'requested_screen'), integer(row, 'requested_page'))
        require(requested in {r[:2] for r in allowed}, 'Unexpected requested destination')
        require(all(row.get(k) is True for k in
                    ('rider_profile_initialized', 'rider_profile_common_link')),
                'Original process profile ownership missing')
        require(row['gpu_publication'] > 0 and row['light_manager_generation'] == row['switches'] + 1
                and integer(row, 'light_cache_state') == 1,
                'Fresh GPU/light owner lifetime or tracked cache absence missing')
        switch = row['switches']
        token = (row['gpu_publication'], row['light_manager_generation'])
        require(switch not in self.owner_tokens or self.owner_tokens[switch] == token,
                'GPU/light identity changed within one screen lifetime')
        self.owner_tokens[switch] = token
        if self.last_frame:
            for key in ('updates', 'renders', 'submitted', 'input_events', 'switches'):
                require(row[key] >= self.last_frame['state'][key], 'Counter regressed: ' + key)
        if row['screen'] == 5:
            require(row.get('selected_slot') == 1 and row.get('selected_rider') == 19,
                    'Rider19 selection changed')
        if row['screen'] == 6:
            require(row.get('presentation') == 'native_host_rgb', 'Course presentation label missing')
            require([row.get(k) for k in ('course_slot', 'subtype_slot', 'laps', 'entrants')]
                    == [1, 1, 3, 8] and row.get('popup_enabled') is False,
                    'Course left the fixed no-popup route')
        if not terminal and row.get('reason') == 'capture':
            if row['screen'] == 5:
                require(row.get('accepted_source_frame') is True,
                        'Rider capture lacks actual accepted source submission')
                # A legal initialEntry frame precedes openingAC's model attach.
                # Never discard that frame or claim its model is already ready.
                for key in ('quads', 'draws', 'source_triangles', 'bank_generation', 'registry_generation'):
                    integer(row, key)
                for key in ('source_frame_generation', 'source_target_generation'):
                    require(integer(row, key) > 0, 'Rider render evidence missing: ' + key)
                generations = (row['source_frame_generation'], row['source_target_generation'])
                prior = self.rider_generations.get(switch)
                require(prior is None or all(a > b for a, b in zip(generations, prior)),
                        'Rider source frame/target generation did not advance within its owner')
                self.rider_generations[switch] = generations
                if switch in self.attached_lifetimes:
                    require(row.get('selection_attached') is True and row['source_triangles'] > 0,
                            'Settled Rider lost its actual model before retirement')
            elif row['screen'] == 6:
                require(row.get('accepted_native_frame') is True,
                        'Course capture lacks accepted native submission')
                require(integer(row, 'quads') > 0 and integer(row, 'draws') > 0,
                        'Course geometry evidence missing')

    def checkpoint(self, row):
        count = row['input_events']
        if self.route(row) != ROUTES[count] or not self.open_input(row):
            return False
        require((row['requested_screen'], row['requested_page']) == ROUTES[count][:2],
                'Open route has a different pending destination')
        # Confirm commits once, then mandatory Course entry commits packed1125E
        # once more. Back commits Course memory before a fresh Rider activation.
        require(row['rider_profile_generation'] == (1, 1, 3, 4)[count],
                'Unexpected actual profile commit generation')
        require(row.get('main_profile_mode') == (4 if count == 0 else 0),
                'Unexpected actual shared profile mode')
        if count == 0:
            require(row.get('selected_group') == 1 and row.get('selected_slot') == 1
                    and row.get('saved_slot') == 1, 'Fresh Main selection not established')
        else:
            require(row.get('rider_profile_readable') is True
                    and row.get('rider_profile_remembered') == [19, 19, 19, 19]
                    and row.get('rider_profile_mode') == 0
                    and (integer(row, 'main_profile_player_bits') & 0x60) == 0x20,
                    'Fresh mode0 one-human Rider profile not established')
        if row['screen'] == 5:
            require(row.get('prompt_mask') == 3, 'Settled Rider prompts differ')
            require(row.get('selection_attached') is True and row.get('accepted_source_frame') is True,
                    'Settled Rider model not attached/accepted')
            require(all(integer(row, key) > 0 for key in
                        ('quads', 'draws', 'source_triangles', 'bank_generation', 'registry_generation')),
                    'Settled Rider lacks actual model geometry/ownership')
            self.attached_lifetimes.add(row['switches'])
        return True

    def send(self, command, edge=None):
        require(self.proc is not None, 'No owned process')
        require(command in ('capture', 'quit'), 'Forbidden stdin command')
        now = time.monotonic_ns()
        if command == 'capture':
            require(self.pending is None and self.ready_ns is not None and self.reason is None,
                    'Capture not admitted')
            require(self.requests < MAX_FRAMES, 'Frame bound exceeded')
        lines = []
        if edge == 'down':
            require(self.presses == self.releases and self.held is None and self.presses < 3,
                    'Repeated or overlapping press')
            require(self.last_frame is not None and self.checkpoint(self.last_frame['state'])
                    and now - self.last_frame['received_ns'] <= 500_000_000,
                    'No fresh acknowledged input-open source gate')
            key = ACTIONS[self.presses][1]
            # The only Course action is Escape; there is no race-launch command.
            require((self.last_frame['state']['screen'], key) in
                    ((4, 'Enter'), (5, 'Enter'), (6, 'Escape')), 'Forbidden route/key pair')
            lines.append('down ' + key)
        elif edge == 'up':
            require(self.held in ('Enter', 'Escape') and self.presses == self.releases + 1,
                    'Release without one held key')
            key = self.held
            lines.append('up ' + key)
        else:
            require(edge is None, 'Forbidden key edge')
            key = None
        lines.append(command)
        payload = ('\n'.join(lines) + '\n').encode('ascii')
        require(self.stdin_bytes + len(payload) <= 16384, 'Stdin byte bound exceeded')
        written = os.write(self.proc.stdin.fileno(), payload)
        self.stdin_bytes += written
        require(written == len(payload), 'Partial stdin write; NEVER retry its menu input')
        if edge == 'down':
            self.held = key
            self.presses += 1
            self.navigation_deadline = now + int(self.args.navigation_timeout * 1e9)
        if command == 'capture':
            self.requests += 1
            self.last_request_ns = now
            self.pending = {'request': self.requests, 'sent_ns': now, 'edge': edge, 'key': key,
                            'input_count': self.presses,
                            'deadline_ns': now + int(self.args.ack_timeout * 1e9)}
        self.event('stdin', lines=lines, bytes=written)

    def preserve(self, state, now):
        require(self.pending is not None and 'ack' in self.pending, 'State without atomic capture ACK')
        ack = self.pending['ack']
        require(state['submitted'] == ack['native_frame'], 'Image and source frame differ')
        common = ('screen', 'page', 'switches', 'requested_screen', 'requested_page',
                  'lifecycle', 'phase', 'aperture_state', 'aperture_input_enabled', 'gpu_publication')
        require(all(state[k] == ack[k] for k in common), 'Image/state route or ownership mismatch')
        require(ack['accepted_update_sequence'] == state['updates'], 'Image/update sequence mismatch')
        require(state['input_events'] == self.pending['input_count'], 'Image/key-edge input count mismatch')
        if state['screen'] == 5:
            matching = ('selected_slot', 'selected_rider', 'selection_attached', 'accepted_source_frame',
                        'source_frame_generation', 'source_target_generation', 'source_triangles')
            require(ack.get('selected_group') is None and all(state[k] == ack[k] for k in matching),
                    'Rider image/source owner mismatch')
            require(state['quads'] == ack['source_quads'] and state['draws'] == ack['source_draws'],
                    'Rider draw evidence mismatch')
        elif state['screen'] == 6:
            matching = ('presentation', 'course_slot', 'subtype_slot', 'laps', 'entrants',
                        'popup_enabled', 'accepted_native_frame')
            require(all(state[k] == ack[k] for k in matching)
                    and state['quads'] == ack['native_quads'] and state['draws'] == ack['native_draws']
                    and ack.get('selected_group') == 1 and ack.get('selected_slot') == 1,
                    'Course native image/source owner mismatch')
        else:
            require(all(state[k] == ack[k] for k in ('selected_group', 'selected_slot', 'saved_group')),
                    'Main image/navigation mismatch')
        if self.last_frame:
            require(ack['native_frame'] > self.last_frame['ack']['native_frame']
                    and ack['accepted_update_sequence'] > self.last_frame['ack']['accepted_update_sequence'],
                    'Every capture must contain a fresh real update and render')
        live = self.out / 'live.ppm'
        require(not live.is_symlink() and stat.S_ISREG(live.stat().st_mode), 'Nonregular live image')
        before = live.stat()
        with live.open('rb') as stream:
            data = stream.read(IMAGE_BYTES + 1)
        after = live.stat()
        require(all(getattr(before, k) == getattr(after, k) for k in
                    ('st_dev', 'st_ino', 'st_size', 'st_mtime_ns', 'st_ctime_ns')),
                'Acknowledged image changed while reading')
        require(len(data) == IMAGE_BYTES and data.startswith(HEADER), 'Incomplete 640x448 PPM')
        number = self.frames + 1
        destination = self.out / 'frames' / ('frame-%06d.ppm' % number)
        with destination.open('xb') as stream:
            stream.write(data)
            stream.flush()
            os.fsync(stream.fileno())
        record = {**self.pending, 'state': state, 'received_ns': now, 'frame': number,
                  'path': str(destination.relative_to(self.out)), 'sha256': hashlib.sha256(data).hexdigest(),
                  'seconds_since_ready': (now - self.ready_ns) / 1e9,
                  'timing': 'Actual host receipt time and source_milliseconds; no synthetic presentation time'}
        payload = json.dumps(record, sort_keys=True) + '\n'
        size = len(payload.encode())
        require(self.journal_bytes + size <= MAX_JOURNAL, 'Frame journal bound exceeded')
        self.journal.write(payload)
        self.journal.flush()
        os.fsync(self.journal.fileno())
        self.journal_bytes += size
        self.frames = number
        self.last_frame = record
        self.pending = None
        require(now <= record['deadline_ns'], 'Capture completed beyond its original deadline')
        if record['edge'] == 'up':
            self.releases += 1
            self.held = None
        if self.checkpoint(state):
            self.navigation_deadline = None
            key = str(state['input_events'])
            if key not in self.checkpoints:
                self.checkpoints[key] = {'frame': number, 'received_ns': now, 'state': state}
                self.event('input_open_checkpoint', checkpoint=key, frame=number, route=self.route(state))

    def line(self, text, now):
        if text.startswith(('original_main_closed ',)) or re.match(
                r'^original_(?:main|rider|course)_state .*"reason"\s*:\s*"quit"', text):
            if len(self.terminal_receipts) < 4:
                self.terminal_receipts.append({'received_ns': now, 'text': text[:8192]})
        if self.reason is None and self.pending is not None and 'ack' in self.pending:
            require(text.startswith(PREFIXES[self.pending['ack']['screen']]),
                    'Capture ACK was not immediately followed by its owner state')
        if text.startswith(('interactive drive:', 'stdin input error ', 'original_ui_destination unavailable',
                            'Development session restarting:', 'Starting a fresh authored-grid session.',
                            'Development drive stopped:', 'live capture committed at tick ',
                            'capture_state wrong_way=', 'ui_state ', 'ORIGINAL AI FLEET ')):
            raise ValueError('Unexpected failure/gameplay/host-menu receipt: ' + text[:2048])
        if text.startswith('ORIGINAL AI ') and 'boundary:' in text:
            raise ValueError('Unexpected original AI boundary: ' + text[:2048])
        if text.startswith('original_ui_screen_activated '):
            value = int(text.split()[-1])
            require(len(self.activations) < 3 and value == (5, 6, 5)[len(self.activations)],
                    'Unexpected destination activation order')
            self.activations.append(value)
            self.event('screen_activated', screen=value)
        elif text.startswith('original_main_ready '):
            row = json.loads(text.split(' ', 1)[1])
            require(self.ready is None and row == {'native_frame': 1, 'width': 640, 'height': 448},
                    'Unexpected first actual Main submission')
            require(now - self.launch_ns <= int(self.args.startup_timeout * 1e9), 'Late readiness')
            self.ready, self.ready_ns = row, now
            self.navigation_deadline = now + int(self.args.navigation_timeout * 1e9)
        elif text.startswith('original_main_capture '):
            row = json.loads(text.split(' ', 1)[1])
            require(self.pending is not None and 'ack' not in self.pending, 'Unsolicited capture ACK')
            require(integer(row, 'sequence') == self.pending['request'] == self.acks + 1,
                    'Noncontiguous capture ACK')
            require(self.route(row) in ROUTES and integer(row, 'native_frame') > 0,
                    'Unexpected capture route/frame')
            require(row.get('width') == 640 and row.get('height') == 448
                    and row.get('file') == str(self.out / 'live.ppm'), 'Wrong image target')
            for key in ('audio_mixed_frames', 'audio_captured_frames', 'audio_clock_microseconds',
                        'audio_accepted_nanoseconds', 'audio_rate'):
                require(integer(row, key) == 0, 'Muted capture output active: ' + key)
            self.pending['ack'] = row
            self.pending['ack_received_ns'] = now
            self.acks += 1
        elif text.startswith(tuple(PREFIXES.values())):
            row = json.loads(text.split(' ', 1)[1])
            require(row.get('screen') in PREFIXES and text.startswith(PREFIXES[row['screen']]),
                    'State prefix/owner mismatch')
            reason = row.get('reason')
            require(reason in ('bound', 'ready', 'capture', 'quit'), 'Unknown state reason')
            self.check_state(row, terminal=reason == 'quit')
            if reason == 'capture':
                self.preserve(row, now)
            elif reason == 'quit':
                require(self.quit_ns is not None and self.quit_state is None, 'Unexpected/repeated quit state')
                self.quit_state = row
        elif text.startswith('original_main_closed '):
            row = json.loads(text.split(' ', 1)[1])
            require(self.quit_ns is not None and self.closed is None, 'Unexpected/repeated cleanup receipt')
            require(row.get('cleanup_complete') is True and integer(row, 'world_ticks') == 0
                    and integer(row, 'window_swaps') == 0, 'Cleanup or zero-gameplay evidence failed')
            require(integer(row, 'captures') == self.acks and integer(row, 'native_frames') > 0,
                    'Cleanup capture accounting mismatch')
            if self.last_frame:
                require(row['native_frames'] >= self.last_frame['ack']['native_frame'], 'Final frame regressed')
            for key in ('audio_mixed_frames', 'audio_source_mixed_frames', 'audio_captured_frames',
                        'audio_clock_microseconds', 'audio_accepted_nanoseconds', 'audio_device_consumed_frames',
                        'audio_rate', 'audio_device_underrun_frames', 'audio_device_overflow_frames'):
                require(integer(row, key) == 0, 'Muted cleanup output active: ' + key)
            require(row.get('audio_device_status') == 'not_requested', 'Muted cleanup device state differs')
            self.closed = row

    def drain(self, delay):
        for key, _ in self.selector.select(delay):
            data = os.read(key.fileobj.fileno(), 65536)
            if not data:
                self.eof = True
                self.selector.unregister(key.fileobj)
                continue
            available = max(0, MAX_LOG - self.raw_bytes)
            self.raw.write(data[:available])
            self.raw_bytes += min(len(data), available)
            if len(data) > available:
                self.fail('Raw log reached 32 MiB; retained prefix only')
                continue
            self.buffer += data
            while b'\n' in self.buffer:
                line, self.buffer = self.buffer.split(b'\n', 1)
                try:
                    require(len(line) <= MAX_LINE, 'Oversized log line')
                    self.line(line.decode('utf-8', errors='replace'), time.monotonic_ns())
                except (ValueError, TypeError, KeyError, OSError, OverflowError, RecursionError) as error:
                    self.fail('Rejected receipt: ' + str(error))
            if len(self.buffer) > MAX_LINE:
                self.fail('Oversized unterminated log line')
                self.buffer = b''

    def signal_child(self, number):
        self.signals.append(number)
        try:
            os.killpg(self.proc.pid, number)
        except ProcessLookupError:
            pass

    def cleanup(self, now):
        if self.pending and now < self.pending['deadline_ns']:
            return
        if self.quit_ns is None:
            self.quit_ns = now
            try:
                self.send('quit')
            except (OSError, ValueError) as error:
                self.fail('Quit write failed: ' + str(error))
        elif self.term_ns is None and now - self.quit_ns >= int(self.args.quit_timeout * 1e9):
            self.term_ns = now
            self.signal_child(signal.SIGTERM)
        elif self.term_ns is not None and self.kill_ns is None and now - self.term_ns >= 2_000_000_000:
            self.kill_ns = now
            self.signal_child(signal.SIGKILL)

    def next_capture(self, now):
        edge = None
        if self.held is not None:
            edge = 'up'
        elif self.last_frame and self.presses == self.releases and self.checkpoint(self.last_frame['state']):
            checkpoint = self.checkpoints.get(str(self.presses))
            if (checkpoint and self.last_frame['received_ns'] - checkpoint['received_ns']
                    >= DWELL_NS[self.presses]):
                # The first and last retained checkpoints must span real source
                # time, not an audio clock, which is deliberately zero in mute.
                delta_ms = (self.last_frame['state']['source_milliseconds']
                            - checkpoint['state']['source_milliseconds']) & 0xffffffff
                if delta_ms >= DWELL_NS[self.presses] // 1_000_000:
                    if self.presses == 3:
                        self.reason = 'sequence_complete'
                        return
                    edge = 'down'
        self.send('capture', edge=edge)

    def run(self):
        old = {n: signal.signal(n, lambda number, frame: setattr(self, 'interrupted', number))
               for n in (signal.SIGINT, signal.SIGTERM)}
        returncode = None
        try:
            self.launch_ns = time.monotonic_ns()
            self.proc = subprocess.Popen(self.manifest['argv'], cwd=str(ASSETS),
                                         env=self.manifest['environment'], stdin=subprocess.PIPE,
                                         stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                         start_new_session=True, close_fds=True, bufsize=0,
                                         preexec_fn=child_limits)
            os.set_blocking(self.proc.stdin.fileno(), False)
            os.set_blocking(self.proc.stdout.fileno(), False)
            self.selector.register(self.proc.stdout, selectors.EVENT_READ)
            # Only this owned process's kernel executable link is exempt from
            # the ordinary input-symlink policy; hash the running inode itself.
            require(Path(os.readlink('/proc/%d/exe' % self.proc.pid)) == self.args.executable,
                    'Owned process executable path differs from reviewed binary')
            running = identity(Path('/proc/%d/exe' % self.proc.pid), kernel_exe=True)
            require(running['sha256'] == self.manifest['executable']['sha256'],
                    'Running native executable differs from reviewed hash')
            self.event('running_executable', identity=running)
            exited = None
            while True:
                self.drain(0.005)
                now = time.monotonic_ns()
                returncode = self.proc.poll()
                if returncode is not None:
                    exited = exited or now
                    if self.reason is None:
                        self.fail('Unexpected process exit')
                    if self.eof or now - exited >= 2_000_000_000:
                        break
                    continue
                if self.interrupted is not None:
                    self.fail('Recorder interrupted by signal %d' % self.interrupted)
                if self.eof:
                    self.fail('Native stdout closed while process remained live')
                if self.ready_ns is None and now - self.launch_ns >= int(self.args.startup_timeout * 1e9):
                    self.fail('Startup timeout')
                if self.reason is None and self.ready_ns is not None and now - self.ready_ns >= WINDOW_NS:
                    self.fail('Thirty-second observation window elapsed')
                if self.navigation_deadline and now >= self.navigation_deadline and self.reason is None:
                    self.fail('Navigation did not reach the next acknowledged gate')
                if self.pending and now >= self.pending['deadline_ns']:
                    self.fail('Capture acknowledgment timeout')
                if self.reason:
                    self.cleanup(now)
                    if self.kill_ns and now - self.kill_ns >= 2_000_000_000:
                        self.fail('Owned child unreaped after SIGKILL')
                        break
                elif self.ready_ns is not None and self.pending is None:
                    if self.last_request_ns is None or now - self.last_request_ns >= int(1e9 / self.args.capture_hz):
                        self.next_capture(now)
        except Exception as error:
            self.fail(type(error).__name__ + ': ' + str(error))
        finally:
            if self.proc is not None and self.proc.poll() is None:
                deadline = time.monotonic_ns() + int((self.args.ack_timeout + self.args.quit_timeout + 6) * 1e9)
                while self.proc.poll() is None and time.monotonic_ns() < deadline:
                    try:
                        self.drain(0.01)
                        self.cleanup(time.monotonic_ns())
                    except Exception as error:
                        self.fail('Cleanup: ' + str(error))
                        self.signal_child(signal.SIGKILL)
                        break
                if self.proc.poll() is None:
                    self.signal_child(signal.SIGKILL)
                try:
                    returncode = self.proc.wait(timeout=2)
                except subprocess.TimeoutExpired:
                    self.fail('Owned process could not be reaped')
            if self.proc is not None:
                # Exception cleanup can reap before the final pipe drain. Keep
                # its real terminal receipts even though the sequence failed.
                drain_deadline = time.monotonic_ns() + 2_000_000_000
                while not self.eof and self.proc.poll() is not None and time.monotonic_ns() < drain_deadline:
                    try:
                        self.drain(0.01)
                    except Exception as error:
                        self.fail('Final stdout drain: ' + str(error))
                        break
                for stream in (self.proc.stdin, self.proc.stdout):
                    if stream:
                        stream.close()
            self.selector.close()
            for n, handler in old.items():
                signal.signal(n, handler)
            self.raw.close()
            self.journal.close()
            self.events.close()
        ending = None
        try:
            ending = identity(self.args.executable)
            require(ending['sha256'] == self.manifest['executable']['sha256'], 'Executable changed during observation')
        except (OSError, ValueError) as error:
            self.fail(str(error))
        complete = (self.reason == 'sequence_complete' and not self.problems and returncode == 0
                    and not self.signals and self.closed is not None and self.quit_state is not None
                    and self.presses == self.releases == 3 and self.held is None
                    and self.pending is None and self.requests == self.acks == self.frames
                    and set(self.checkpoints) == {'0', '1', '2', '3'}
                    and self.activations == [5, 6, 5]
                    and self.route(self.quit_state) == ROUTES[3])
        summary = {'created_utc': utc(), 'outcome': 'observed' if complete else 'failed',
                   'stop_reason': self.reason, 'problems': self.problems, 'returncode': returncode,
                   'launch_ns': self.launch_ns, 'ready_ns': self.ready_ns, 'quit_ns': self.quit_ns,
                   'frames': self.frames, 'requests': self.requests, 'matched_acks': self.acks,
                   'presses': self.presses, 'releases': self.releases, 'held_at_stop': self.held,
                   'checkpoints': self.checkpoints, 'screen_activations': self.activations,
                   'forced_signals': self.signals, 'native_quit_state': self.quit_state,
                   'native_closed': self.closed, 'terminal_receipts': self.terminal_receipts,
                   'last_frame': self.last_frame, 'pending_capture': self.pending,
                   'ending_executable': ending, 'audio_policy': 'explicit mute; no PCM or playback claim',
                   'limits': [
                       'This observes native integrated Main4/1 -> Rider5/1 initial19 -> Course6/1 -> fresh Rider5/1 only.',
                       'Exactly Enter, Enter, Escape; zero driving/controller packets; world_ticks must stay zero.',
                       'Course is native_host_rgb. Accepted frames do not establish original GS ordering or retail pixel parity.',
                       'Muted effects can still advance the real shared RNG; no seed, pose or input is synthesized.',
                       'Capture rate is a ceiling/target. Actual timing is retained per frame; duplicates are preserved.',
                       'Release capture is an ordered host-consumption barrier, not a separate exposed key-up receipt.',
                       'Destination activation traverses mandatory renderer retirement; no independent retirement ACK exists.',
                       'No video encoding, frame interpolation, audio fabrication, restart, retry, or second launch.',
                       'Visual layout, Tatooine colors, animation, and return quality need review of retained acknowledged frames.'
                   ]}
        save_json(self.out / 'summary.json', summary)
        print(json.dumps({'outcome': summary['outcome'], 'output': str(self.out), 'frames': self.frames}), flush=True)
        return 0 if complete else 2


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--expected-exe-sha256', required=True)
    parser.add_argument('--expected-entry-sha256', required=True)
    parser.add_argument('--source-commit', required=True)
    parser.add_argument('--build-provenance', type=Path, required=True)
    parser.add_argument('--ui-root', type=Path, required=True)
    parser.add_argument('--rider-ipum-cache', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--not-before', required=True)
    parser.add_argument('--root-reviewed-one-check', action='store_true')
    parser.add_argument('--capture-hz', type=float, default=10)
    parser.add_argument('--startup-timeout', type=float, default=180)
    parser.add_argument('--ack-timeout', type=float, default=5)
    parser.add_argument('--navigation-timeout', type=float, default=12)
    parser.add_argument('--quit-timeout', type=float, default=10)
    args = parser.parse_args()
    require(args.root_reviewed_one_check, 'Root review/centralized slot authorization missing')
    for field in ('expected_exe_sha256', 'expected_entry_sha256'):
        require(re.fullmatch(r'[0-9a-fA-F]{64}', getattr(args, field)), 'Invalid SHA256: ' + field)
    require(re.fullmatch(r'[0-9a-fA-F]{40}', args.source_commit), 'Invalid frozen source commit')
    for field, maximum in (('capture_hz', 30), ('startup_timeout', 300), ('ack_timeout', 10),
                           ('navigation_timeout', 20), ('quit_timeout', 15)):
        require(math.isfinite(getattr(args, field)) and 0 < getattr(args, field) <= maximum,
                'Invalid bounded setting: ' + field)
    gate = dt.datetime.fromisoformat(args.not_before.replace('Z', '+00:00'))
    require(gate.tzinfo is not None and gate >= EARLIEST and dt.datetime.now(dt.timezone.utc) >= gate,
            'Centralized not-before gate unavailable; no waiting or launch')
    require(not args.executable.is_symlink(), 'Executable symlink rejected')
    args.executable = args.executable.resolve(strict=True)
    executable = identity(args.executable)
    require(executable['sha256'] == args.expected_exe_sha256.lower()
            and os.access(args.executable, os.X_OK), 'Reviewed executable identity/permission mismatch')
    with args.executable.open('rb') as stream:
        require(stream.read(4) == b'\x7fELF', 'Native ELF required; wrappers rejected')
    header = identity(REPO / 'tests/native/original_ui_main_entry50kg.h')
    require(header['sha256'] == args.expected_entry_sha256.lower(), 'Reviewed report header changed')
    for path in (args.ui_root, args.rider_ipum_cache):
        require(not path.is_symlink() and path.is_dir(), 'Explicit nonsymlink input directory required')
    args.ui_root = args.ui_root.resolve(strict=True)
    args.rider_ipum_cache = args.rider_ipum_cache.resolve(strict=True)
    ui = [identity(args.ui_root / path, 16 * 1024 * 1024) for path in UI_FILES]
    cache = [identity(args.rider_ipum_cache / name, 16 * 1024 * 1024)
             for name in ['19c_1-mip%d.rrtex' % n for n in range(5)] + ['manifest.json']]
    corpus = identity(ASSETS / CORPUS, 64 * 1024 * 1024)
    audio_root = ASSETS / 'work/native/project/disc/AUDIO'
    require(audio_root.is_dir(), 'Cold all-AI native audio input root is missing')
    provenance = identity(args.build_provenance, 64 * 1024 * 1024)
    args.output = args.output.absolute()
    require(args.output.parent.resolve() == STAGE.resolve() and not args.output.exists()
            and not args.output.is_symlink(), 'Output must be a new direct child of the fixed stage')
    args.output.mkdir(mode=0o700)
    for name in ('frames', 'home', 'tmp', 'cache'):
        (args.output / name).mkdir(mode=0o700)
    environment = {
        'PATH': '/usr/bin:/bin', 'LANG': 'C', 'LC_ALL': 'C', 'TZ': 'UTC',
        'HOME': str(args.output / 'home'), 'TMPDIR': str(args.output / 'tmp'),
        'XDG_CACHE_HOME': str(args.output / 'cache'),
        'RR_TRACK': 'GC', 'RR_GC_INCOMPLETE_WORLD': '1', 'RR_RACE': '1',
        'RR_RACE_MODE': '2', 'RR_START_MENU': '0', 'RR_ORIGINAL_AI_ALL_RACERS': '1',
        # The cold all-AI world requires genuine audio providers. Original UI
        # takes input/update ownership before world ticks and prevents that
        # world bank from opening output; UI audio itself is explicitly muted.
        'RR_AUTODRIVE': '0', 'RR_DISABLE_RACER_CONTACTS': '0', 'RR_AUDIO': '1',
        'RR_AUDIO_ROOT': str(audio_root),
        'RR_AUDIO_DEVICE': '0', 'RR_AUTHORED_WORLD_FOG': '1',
        # Omit race-only selected-light overrides: Main has its own authentic
        # UI light owner. Fresh gameplay's default-on lighting is unchanged.
        'RR_ORIGINAL_MAIN_MENU': '1', 'RR_ORIGINAL_UI_ROOT': str(args.ui_root),
        'RR_ORIGINAL_MAIN_AUDIO': 'muted', 'RR_ORIGINAL_RIDER_IPUM_CACHE': str(args.rider_ipum_cache),
    }
    source_paths = ('tests/native/terrain_interactive_drive50du.cpp',
                    'tests/native/original_ui_main_entry50kg.h',
                    'platform/linux/native/original_ui_frontend50kg.cpp',
                    'platform/linux/native/original_ui_rider_session50kg.cpp',
                    'platform/linux/native/original_ui_course_session50kg.cpp',
                    'platform/linux/native/original_ui_rider_profile50kg.cpp',
                    'src/assets/original_ui_ipum_texture_cache50kg_generated.inc',
                    'tools/prepare_interactive_build50dv.py')
    manifest = {
        'created_utc': utc(), 'authoring_status': 'Corrected Main-mode launch contract; this revision has not been executed',
        'root_reported_frozen_source_commit': args.source_commit.lower(), 'executable': executable,
        'report_header': header, 'recorder': identity(Path(__file__).resolve()), 'corpus': corpus,
        'root_build_provenance': provenance, 'source_files': [identity(REPO / p) for p in source_paths],
        'source_warning': 'Hashes/claimed commit do not independently establish binary build provenance',
        'ui_archives': ui, 'rider_cache': cache, 'native_authentication_required': True,
        'cwd': str(ASSETS), 'environment': environment,
        'argv': [str(args.executable), str(CORPUS), '--headless-live', str(args.output / 'live.ppm')],
        'actions': ACTIONS, 'routes': ROUTES, 'dwell_seconds': [n / 1e9 for n in DWELL_NS],
        'capture_hz_target': args.capture_hz, 'window_seconds': 30, 'not_before': gate.isoformat(),
        'timeouts': {'startup': args.startup_timeout, 'ack': args.ack_timeout,
                     'navigation': args.navigation_timeout, 'quit': args.quit_timeout},
        'bounds': {'frames': MAX_FRAMES, 'image_bytes_each': IMAGE_BYTES, 'raw_log': MAX_LOG,
                   'frame_journal': MAX_JOURNAL, 'event_journal': MAX_JOURNAL, 'stdin': 16384,
                   'child_file_rlimit': 8 * 1024 * 1024, 'core_dumps': 0,
                   'scope': 'Named evidence is bounded; native HOME/TMP/cache ancillary files are not aggregate-quota limited'},
        'centralized_gate': 'Root must own shared ledger/lock. This script never assigns or advances a shared slot.',
        'one_launch_attempt': True,
    }
    save_json(args.output / 'launch.json', manifest)
    return Recorder(args, manifest).run()


if __name__ == '__main__':
    sys.exit(main())
