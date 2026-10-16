#!/usr/bin/env python3
"""Bounded original Game Options Cancel/Accept capture via host keys. UNTESTED source."""
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
import struct
import subprocess
import sys
import time

STAGE = Path('/workspace/shared/recovery-20261010/ui-navigation-recording')
REPO = Path('/workspace/shared/rr-recovery-git')
ASSETS = Path('/workspace/shared/recovery-20261010/native-assets-prepared-01')
UI_ROOT = Path('/workspace/shared/recovery-20261010/ui-inputs/game-files')
AUDIO_ROOT = Path('/workspace/shared/recovery-20261010/asset-inputs/extracted/RacerRevenge-original-AUDIO/AUDIO')
CORPUS = Path('evidence/staticType2Producer49X/original-word/native-fixtures.json')
EARLIEST = dt.datetime(2026, 10, 11, 3, 17, 10, 251731, tzinfo=dt.timezone.utc)
PPM_HEADER = b'P6\n640 448\n255\n'
PPM_BYTES = len(PPM_HEADER) + 640 * 448 * 3
MAX_LOG = 32 * 1024 * 1024
MAX_EVENTS = 64 * 1024 * 1024
MAX_FRAME_LOG = 16 * 1024 * 1024
MAX_LINE = 256 * 1024
MAX_CAPTURES = 1220
MAX_STDIN_BYTES = 16384
WINDOW_NS = 40_000_000_000
SEQUENCE_TAIL_NS = 1_000_000_000
AUDIO_RATE = 48000
MAX_WAV_FRAMES = AUDIO_RATE * 70
MAX_WAV_BYTES = 44 + MAX_WAV_FRAMES * 4
# Gates are assertions about observed source state, never state writes. All
# presses and releases are separated by committed-image plus state receipts.
# Exactly twenty fresh original-UI presses. Each action requires the previous
# fully open checkpoint, including settings/profile/revision and GPU identity.
ACTIONS = (
    ('main_down_1', 'Down'), ('main_down_2', 'Down'),
    ('main_down_3', 'Down'), ('main_confirm_options', 'Enter'),
    ('options_down_1', 'Down'), ('options_down_2', 'Down'),
    ('enter_game_options_first', 'Enter'), ('cancel_edit_sound_group', 'Down'),
    ('cancel_edit_sound_right', 'Right'), ('cancel_edit_sfx_group', 'Down'),
    ('cancel_edit_sfx_left', 'Left'), ('cancel_dirty_game_options', 'Escape'),
    ('reenter_after_cancel', 'Enter'), ('accept_edit_sfx_left', 'Left'),
    ('accept_edit_sound_group', 'Up'), ('accept_edit_sound_right', 'Right'),
    ('accept_dirty_game_options', 'Enter'), ('reenter_after_accept', 'Enter'),
    ('leave_verified_game_options', 'Escape'), ('options_back_main', 'Escape'),
)
ALLOWED_COMMANDS = ('capture', 'quit', 'down Down', 'up Down',
                    'down Up', 'up Up', 'down Left', 'up Left', 'down Right', 'up Right',
                    'down Enter', 'up Enter', 'down Escape', 'up Escape')
DIAGNOSTIC_FIELDS = ('reason', 'screen', 'page', 'switches', 'updates', 'renders', 'submitted',
                     'selected_group', 'selected_slot', 'saved_slot', 'saved_group',
                     'requested_screen', 'requested_page', 'gpu_publication',
                     'game_profile_sound_mode', 'game_profile_volumes', 'game_profile_flags',
                     'game_local_sound_mode', 'game_local_volumes', 'game_options_accepts',
                     'game_sound_apply_revision', 'application_sound_revision',
                     'input_events', 'unavailable_destinations',
                     'lifecycle', 'aperture_state', 'aperture_top', 'aperture_bottom',
                     'input_shown', 'aperture_input_enabled', 'source_rng', 'fire_serial',
                     'source_milliseconds', 'world_ticks', 'mixed_frames', 'audio_muted', 'audio_failed',
                     'audio_source_mixed_frames', 'audio_mixed_frames', 'audio_captured_frames',
                     'audio_accepted_nanoseconds', 'audio_clock_microseconds', 'audio_device_status')
ENV = {
    'RR_TRACK': 'GC', 'RR_GC_INCOMPLETE_WORLD': '1', 'RR_RACE': '1',
    'RR_RACE_MODE': '2', 'RR_START_MENU': '0', 'RR_ORIGINAL_AI_ALL_RACERS': '1',
    'RR_AUTODRIVE': '0', 'RR_DISABLE_RACER_CONTACTS': '0', 'RR_AUDIO': '1',
    'RR_AUDIO_DEVICE': '0', 'RR_AUTHORED_WORLD_FOG': '0',
    'RR_ORIGINAL_MAIN_MENU': '1', 'RR_ORIGINAL_UI_ROOT': str(UI_ROOT),
    'RR_ORIGINAL_MAIN_AUDIO': 'enabled', 'RR_ORIGINAL_MAIN_AUDIO_ROOT': str(AUDIO_ROOT),
}


def checkpoint(screen, page, switches, group, slot, saved_slot, saved_group,
               profile_mode=1, profile_sfx=50, local_mode=None, local_sfx=None):
    expected = {'screen': screen, 'page': page, 'switches': switches,
                'selected_group': group, 'selected_slot': slot, 'saved_slot': saved_slot,
                'saved_group': saved_group, 'game_profile_sound_mode': profile_mode,
                'game_profile_volumes': [profile_sfx, 100, 80, 80], 'game_profile_flags': 5,
                'application_sound_revision': 2 if profile_mode == 2 else 1,
                'game_options_accepts': 1 if screen == 12 and profile_mode == 2 else 0,
                'game_sound_apply_revision': 2 if screen == 12 and profile_mode == 2 else 0}
    if page == 4:
        expected.update(game_local_sound_mode=local_mode, game_local_volumes=[local_sfx, 100, 80, 80],
                        game_local_auto_save=True, game_local_vertical_split=False, game_local_vibration=True)
    return expected


# Entry 0 is the fresh opened Main; entry N is the opened state after press N.
# Main4/1 and Options12/{1,4} are the only admitted routes. Same-screen page
# changes retain switch1 and its real GPU token. A final fresh Main is switch2.
CHECKPOINTS = (
    checkpoint(4, 1, 0, 1, 1, 1, 1),
    checkpoint(4, 1, 0, 1, 2, 1, 1),
    checkpoint(4, 1, 0, 1, 3, 1, 1),
    checkpoint(4, 1, 0, 1, 4, 1, 1),
    checkpoint(12, 1, 1, 1, 1, 1, 1),
    checkpoint(12, 1, 1, 1, 2, 1, 1),
    checkpoint(12, 1, 1, 1, 3, 1, 1),
    checkpoint(12, 4, 1, 1, 1, 3, 1, local_mode=1, local_sfx=50),
    checkpoint(12, 4, 1, 2, 2, 3, 1, local_mode=1, local_sfx=50),
    checkpoint(12, 4, 1, 2, 3, 3, 1, local_mode=2, local_sfx=50),
    checkpoint(12, 4, 1, 3, 1, 3, 1, local_mode=2, local_sfx=50),
    checkpoint(12, 4, 1, 3, 1, 3, 1, local_mode=2, local_sfx=40),
    checkpoint(12, 1, 1, 1, 3, 3, 3),
    checkpoint(12, 4, 1, 3, 1, 3, 3, local_mode=1, local_sfx=50),
    checkpoint(12, 4, 1, 3, 1, 3, 3, local_mode=1, local_sfx=40),
    checkpoint(12, 4, 1, 2, 2, 3, 3, local_mode=1, local_sfx=40),
    checkpoint(12, 4, 1, 2, 3, 3, 3, local_mode=2, local_sfx=40),
    checkpoint(12, 1, 1, 1, 3, 3, 2, profile_mode=2, profile_sfx=40),
    checkpoint(12, 4, 1, 2, 3, 3, 2, profile_mode=2, profile_sfx=40, local_mode=2, local_sfx=40),
    checkpoint(12, 1, 1, 1, 3, 3, 2, profile_mode=2, profile_sfx=40),
    checkpoint(4, 1, 2, 1, 4, 4, 2, profile_mode=2, profile_sfx=40),
)
CHECKPOINT_LABELS = {0: 'initial_main_open', 3: 'main_options_selected', 4: 'options_open',
                     6: 'options_game_options_selected', 7: 'game_options_first_open',
                     11: 'cancel_edits_observed', 12: 'cancel_returned_options',
                     13: 'cancel_discard_verified_on_reentry', 16: 'accept_edits_observed',
                     17: 'accept_committed_and_returned_options',
                     18: 'accept_persisted_on_reentry', 19: 'verified_game_options_left',
                     20: 'returned_main_open'}


def bound_child_files():
    # Child-only hard ceiling for the real WAV and live PPM. No global limits,
    # helper process, fallback recording, truncation, or silent audio discard.
    resource.setrlimit(resource.RLIMIT_FSIZE, (MAX_WAV_BYTES, MAX_WAV_BYTES))
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))


def utc():
    return dt.datetime.now(dt.timezone.utc).isoformat()


def identity(path):
    before = path.stat()
    digest = hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            digest.update(block)
    after = path.stat()
    keys = ('st_dev', 'st_ino', 'st_size', 'st_mtime_ns', 'st_ctime_ns')
    if any(getattr(before, key) != getattr(after, key) for key in keys):
        raise RuntimeError('File changed while hashing: ' + str(path))
    return {'path': str(path), 'sha256': digest.hexdigest(),
            'stat': {key: getattr(after, key) for key in keys}}


def atomic_json(path, value):
    part = path.with_name(path.name + '.part')
    payload = json.dumps(value, indent=2, sort_keys=True) + '\n'
    if len(payload.encode('utf-8')) > 2 * 1024 * 1024:
        raise ValueError('Manifest/summary exceeded its 2 MiB bound')
    with part.open('x', encoding='utf-8') as stream:
        stream.write(payload); stream.flush(); os.fsync(stream.fileno())
    os.replace(part, path)


def uint(row, key):
    value = row.get(key)
    if type(value) is not int or value < 0:
        raise ValueError('Missing or invalid nonnegative integer: ' + key)
    return value


class Recorder:
    def __init__(self, args, out, manifest):
        self.args, self.out, self.manifest = args, out, manifest
        self.proc = None
        self.selector = selectors.DefaultSelector()
        self.raw = (out / 'game.log').open('xb', buffering=0)
        self.events = (out / 'events.jsonl').open('x', encoding='utf-8')
        self.frames = (out / 'frames.jsonl').open('x', encoding='utf-8')
        self.buffer = b''; self.eof = False; self.log_bytes = 0
        self.event_bytes = self.frame_log_bytes = self.stdin_bytes = 0
        self.launch_ns = self.ready_ns = self.deadline_ns = None
        self.stop_ns = self.quit_ns = self.term_ns = self.kill_ns = None
        self.ready = self.closed = self.last_state = self.first_frame = self.last_frame = None
        self.pending = None; self.last_request = None
        self.requests = self.captures = 0
        self.reason = None; self.problems = []; self.signals = []
        self.interrupted = None; self.event_io_failed = False
        self.states = []
        self.saw_closed_input = False; self.saw_open_input = False
        self.aperture_position = None; self.aperture_moved = False
        self.inputs_sent = self.releases_sent = self.action_index = 0
        self.active_action = None
        self.need_release = False
        self.action_receipts = []
        self.navigation_deadline_ns = None
        self.roundtrip_frame = None
        self.roundtrip_ns = None
        self.observations = {}
        self.capture_ack_times = []
        self.seen_state_reasons = set()
        self.diagnostic_states = []
        self.terminal_lines = []
        self.last_rejected_receipt = None
        self.first_rejected_receipt = None
        self.primary_problem = None
        self.gpu_publications = {}
        self.audio_last = None
        self.wav_observation = None
        self.open_checkpoints = set()

    def event(self, kind, **values):
        if self.event_io_failed:
            return
        try:
            payload = json.dumps({'kind': kind, 'monotonic_ns': time.monotonic_ns(),
                                  **values}, sort_keys=True) + '\n'
            size = len(payload.encode('utf-8'))
            if self.event_bytes + size > MAX_EVENTS:
                raise OSError('Evidence events reached their 64 MiB bound')
            self.events.write(payload); self.events.flush(); self.event_bytes += size
        except OSError as error:
            self.event_io_failed = True
            self.problems.append('Evidence event storage failed: ' + str(error))
            if self.reason is None:
                self.reason = 'evidence_storage_failure'; self.stop_ns = time.monotonic_ns()

    def stop(self, reason):
        if self.reason is None:
            self.reason = reason; self.stop_ns = time.monotonic_ns()
            self.event('stop', reason=reason)

    def problem(self, message):
        message = message[:2048]
        if self.primary_problem is None:
            self.primary_problem = {'message': message, 'monotonic_ns': time.monotonic_ns(),
                                    'action_index': self.action_index, 'active_action_index': self.active_action,
                                    'inputs_sent': self.inputs_sent, 'releases_sent': self.releases_sent,
                                    'last_preserved_frame': self.last_frame['frame_index'] if self.last_frame else None}
        if len(self.problems) < 128:
            self.problems.append(message); self.event('problem', message=message)
        self.stop('protocol_or_recording_failure')

    def send(self, command, edge=None):
        commands = (edge, command) if edge is not None else (command,)
        if any(item not in ALLOWED_COMMANDS for item in commands):
            raise RuntimeError('Unsupported host command rejected before stdin')
        if edge is not None and command != 'capture':
            raise RuntimeError('Every key edge requires its own capture barrier')
        if edge is None and command not in ('capture', 'quit'):
            raise RuntimeError('Standalone key commands are forbidden')
        if self.proc is None or self.proc.poll() is not None:
            raise RuntimeError('Child is not running')
        payload = ('\n'.join(commands) + '\n').encode('ascii')
        if self.stdin_bytes + len(payload) > MAX_STDIN_BYTES:
            raise RuntimeError('Bounded stdin byte count exceeded')
        before = time.monotonic_ns(); size = os.write(self.proc.stdin.fileno(), payload)
        after = time.monotonic_ns()
        self.stdin_bytes += size
        self.event('stdin', commands=commands, start_ns=before, complete_ns=after, bytes=size)
        if size != len(payload):
            raise RuntimeError('Partial stdin command; never retried')
        return after

    def check_audio(self, row, source=True, device=True):
        if uint(row, 'audio_rate') != AUDIO_RATE:
            raise ValueError('Original UI audio sample rate differs from 48000 Hz')
        mixed = uint(row, 'audio_mixed_frames')
        captured = uint(row, 'audio_captured_frames')
        accepted = uint(row, 'audio_accepted_nanoseconds')
        microseconds = uint(row, 'audio_clock_microseconds')
        if mixed != captured or mixed > MAX_WAV_FRAMES:
            raise ValueError('Real UI PCM/capture counts differ or exceed the WAV bound')
        if source and uint(row, 'audio_source_mixed_frames') != mixed:
            raise ValueError('Dedicated source mixer frame count differs from output PCM')
        if microseconds != accepted // 1000 or mixed != microseconds * AUDIO_RATE // 1_000_000:
            raise ValueError('UI PCM counts differ from the accepted source interval clock')
        if device:
            if row.get('audio_device_status') != 'not_requested':
                raise ValueError('This capture requires device-free genuine UI PCM')
            for key in ('audio_device_consumed_frames', 'audio_device_underrun_frames',
                        'audio_device_overflow_frames'):
                if uint(row, key) != 0:
                    raise ValueError('Unexpected physical-device activity: ' + key)
        observed = (mixed, captured, accepted, microseconds)
        if self.audio_last and any(a < b for a, b in zip(observed, self.audio_last)):
            raise ValueError('Process-owned audio counters regressed')
        self.audio_last = observed

    @staticmethod
    def route(state):
        return (state['screen'], state['page'], state['switches'])

    @staticmethod
    def matches_checkpoint(state, index):
        return all(state.get(key) == value for key, value in CHECKPOINTS[index].items())

    def check_state(self, row):
        if not isinstance(row, dict):
            raise ValueError('Source state is not an object')
        if row.get('audio_muted') is not False:
            raise ValueError('Real enabled original UI audio is required for Apply')
        for key in ('world_ticks', 'mixed_frames', 'unavailable_destinations', 'audio_failed', 'audio_suppressed'):
            if uint(row, key) != 0:
                raise ValueError('Unexpected activity in UI-only recording: ' + key)
        for key in ('updates', 'renders', 'submitted', 'selected_slot', 'saved_slot', 'source_rng', 'fire_serial',
                    'lifecycle', 'aperture_state', 'particles', 'source_packets', 'quads', 'spans',
                    'source_milliseconds', 'screen', 'page', 'switches', 'input_events', 'saved_group',
                    'requested_screen', 'requested_page', 'gpu_publication', 'phase', 'audio_submitted',
                    'game_options_accepts', 'game_sound_apply_revision', 'application_sound_revision'):
            uint(row, key)
        if type(row.get('selected_group')) is not int or row['selected_group'] not in (-1, 1, 2, 3):
            raise ValueError('Unexpected original navigation group')
        for key in ('input_shown', 'aperture_input_enabled'):
            if type(row.get(key)) is not bool:
                raise ValueError('Missing source state flag: ' + key)
        for key in ('aperture_top', 'aperture_bottom'):
            if type(row.get(key)) not in (float, int) or not math.isfinite(row[key]):
                raise ValueError('Invalid actual aperture position: ' + key)
        if self.route(row) not in ((4, 1, 0), (12, 1, 1), (12, 4, 1), (4, 1, 2)):
            raise ValueError('Source route is outside Main4/1 and Options12/{1,4}')
        if row['lifecycle'] not in (0, 1, 2, 3, 4, 6, 7, 9) or row['phase'] not in (0, 1, 2) or row['aperture_state'] not in (0, 1, 2, 3):
            raise ValueError('Unsupported source transition/aperture value')
        if row['input_events'] != self.inputs_sent or self.inputs_sent > len(ACTIONS):
            raise ValueError('Source input count differs from exactly the sent fresh presses')
        if self.roundtrip_ns is not None and (not self.open_input(row)
                or not self.matches_checkpoint(row, len(ACTIONS))):
            raise ValueError('Final Main regressed during the observed stable-return tail')
        count = self.inputs_sent
        expected = CHECKPOINTS[count]
        previous = CHECKPOINTS[max(0, count - 1)]
        actual_route = self.route(row)
        if actual_route not in (self.route(previous), self.route(expected)):
            raise ValueError('Page appeared outside the latest acknowledged input route')
        if (row['requested_screen'], row['requested_page']) != (expected['screen'], expected['page']):
            raise ValueError('Actual requested destination differs from the bounded sequence')
        # Profile writes are synchronous with an admitted key; page opening
        # cannot excuse an incorrect profile, remembered group, or Apply count.
        exact = {key: expected[key] for key in ('saved_group', 'game_profile_sound_mode',
                 'game_profile_volumes', 'game_profile_flags', 'application_sound_revision')}
        exact['saved_slot'] = (4 if count >= 4 else 1) if row['screen'] == 4 else (3 if count >= 7 else 1)
        exact['game_options_accepts'] = 1 if row['screen'] == 12 and count >= 17 else 0
        exact['game_sound_apply_revision'] = 2 if row['screen'] == 12 and count >= 17 else 0
        if row['page'] == 4:
            local = expected if expected['page'] == 4 else previous
            exact.update({key: value for key, value in local.items() if key.startswith('game_local_')})
        for key, value in exact.items():
            if row.get(key) != value or type(row.get(key)) is not type(value):
                raise ValueError('Observed profile/edit/Apply field differs after press %d: %s' % (count, key))
        if self.open_input(row):
            if not self.matches_checkpoint(row, count):
                raise ValueError('Open source state does not match the exact next checkpoint')
        else:
            # requestedPage90 genuinely detaches controller0 (-1, slot0).
            # It is valid only while opening; never use it to authorize input.
            source_exiting = actual_route == self.route(previous) and actual_route != self.route(expected)
            selected = previous if source_exiting else expected
            allowed_selection = {(selected['selected_group'], selected['selected_slot'])}
            incoming = (count == 0 or self.route(previous) != self.route(expected)) and actual_route == self.route(expected)
            initial_bound = (count == 0 and actual_route == (4, 1, 0) and row.get('reason') == 'bound'
                             and row['lifecycle'] == 0 and row['phase'] == 0)
            detached_entry = (incoming and row['page'] == 1
                              and not row['input_shown'] and not row['aperture_input_enabled']
                              and (initial_bound or (row['lifecycle'] == 2 and row['phase'] == 0)
                                   or (row['lifecycle'] == 4 and row['phase'] == 1)))
            if detached_entry:
                allowed_selection.add((-1, 0))
            if (row['selected_group'], row['selected_slot']) not in allowed_selection:
                raise ValueError('Unrequested selection during source aperture transition')
        token = row['gpu_publication']
        switch = row['switches']
        if not token:
            raise ValueError('Missing actual GPU bank publication token')
        if switch in self.gpu_publications:
            if self.gpu_publications[switch] != token:
                raise ValueError('GPU bank changed within a screen lifetime/page change')
        else:
            # Tokens are per MaterialTextureGl owner, not process-global IDs.
            # New screen owners may legitimately each publish token1.
            if switch != len(self.gpu_publications):
                raise ValueError('Screen bank publication order/identity was not observed')
            self.gpu_publications[switch] = token
        self.check_audio(row)
        if uint(row, 'audio_remainder_nanoseconds') != row['audio_accepted_nanoseconds'] % 1000:
            raise ValueError('Audio nanosecond remainder differs from accepted source time')
        if uint(row, 'audio_device_queued_frames') != 0:
            raise ValueError('Device-free capture unexpectedly queued device PCM')
        if self.last_state:
            for key in ('updates', 'renders', 'submitted', 'fire_serial', 'input_events', 'switches'):
                if row[key] < self.last_state[key]:
                    raise ValueError('Main counter regressed: ' + key)
        self.last_state = row

    @staticmethod
    def open_input(state):
        return (state['input_shown'] and state['aperture_input_enabled']
                and state['aperture_state'] == 0 and state['lifecycle'] == 0 and state['phase'] == 0)

    def remember(self, label, record):
        if label not in self.observations:
            self.observations[label] = {'frame_index': record['frame_index'],
                                        'native_frame': record['ack']['native_frame'],
                                        'state_ack_monotonic_ns': record['state_ack_monotonic_ns'],
                                        'state': record['state']}
            self.event('source_observation', label=label, frame_index=record['frame_index'])

    def observe_navigation(self, record):
        state = record['state']; count = state['input_events']
        if count == 0 and not state['input_shown'] and not state['aperture_input_enabled']:
            self.remember('initial_main_input_closed', record)
        if self.open_input(state):
            if not self.matches_checkpoint(state, count):
                raise ValueError('Cannot record an unverified open checkpoint')
            self.open_checkpoints.add(count)
            self.remember(CHECKPOINT_LABELS.get(count, 'open_after_press_%02d' % count), record)
        if count:
            before, after = CHECKPOINTS[count - 1], CHECKPOINTS[count]
            if self.route(before) != self.route(after):
                action = ACTIONS[count - 1][0]
                if self.route(state) == self.route(before) and state['aperture_state'] == 3 and state['phase'] == 2:
                    self.remember(action + '_closing', record)
                if self.route(state) == self.route(after) and state['aperture_state'] == 1 and state['phase'] == 1:
                    self.remember(action + '_opening', record)
        if record['edge'] == 'press':
            if self.active_action != record['action_index'] or self.need_release:
                raise ValueError('Unexpected key-press capture barrier')
            self.need_release = True
            self.event('press_acknowledged', action=ACTIONS[self.active_action][0],
                       frame_index=record['frame_index'], input_events=state['input_events'])
        elif record['edge'] == 'release':
            if self.active_action != record['action_index'] or not self.need_release:
                raise ValueError('Unexpected key-release capture barrier')
            self.action_receipts.append({'action': ACTIONS[self.active_action][0],
                                         'release_frame': record['frame_index'],
                                         'input_events': state['input_events']})
            self.event('release_barrier_acknowledged', action=ACTIONS[self.active_action][0],
                       frame_index=record['frame_index'],
                       evidence='Ordered host capture after up; no key-up receipt field is exposed')
            self.action_index += 1; self.active_action = None; self.need_release = False
            self.navigation_deadline_ns = record['state_ack_monotonic_ns'] + int(self.args.navigation_timeout * 1e9)
        if (state['switches'] == 2 and self.open_input(state) and self.matches_checkpoint(state, len(ACTIONS))
                and self.action_index == len(ACTIONS) and self.active_action is None):
            self.remember('returned_main_open', record)
            if self.roundtrip_frame is None:
                self.roundtrip_frame = record['frame_index']
                self.roundtrip_ns = record['state_ack_monotonic_ns']
                self.event('roundtrip_observed', frame_index=record['frame_index'])

    def request_capture(self):
        if self.pending is not None or self.reason is not None:
            raise RuntimeError('Cannot overlap captures or send input after stop')
        if self.requests >= MAX_CAPTURES:
            raise RuntimeError('Bounded capture count exceeded')
        edge = None; key_command = None; selected_action = None; source_frame = None
        if self.active_action is not None:
            if not self.need_release:
                raise RuntimeError('Press is still awaiting its capture barrier')
            selected_action = self.active_action; edge = 'release'
            key_command = 'up ' + ACTIONS[selected_action][1]
            self.releases_sent += 1
            source_frame = self.last_frame['frame_index']
        elif self.action_index < len(ACTIONS) and self.last_frame is not None:
            action = ACTIONS[self.action_index]; state = self.last_frame['state']
            if (self.open_input(state) and self.matches_checkpoint(state, self.action_index)
                    and state['input_events'] == self.action_index):
                selected_action = self.action_index; edge = 'press'
                key_command = 'down ' + action[1]
                self.active_action = selected_action; self.inputs_sent += 1
                source_frame = self.last_frame['frame_index']
        sent = self.send('capture', edge=key_command)
        self.requests += 1; self.last_request = sent
        self.pending = {'request_index': self.requests, 'request_monotonic_ns': sent,
                        'ack_deadline_ns': sent + int(self.args.ack_timeout * 1e9),
                        'edge': edge, 'host_key_command': key_command,
                        'action_index': selected_action,
                        'action': ACTIONS[selected_action][0] if selected_action is not None else None,
                        'gate_source_frame': source_frame,
                        'expected_input_events': self.inputs_sent}

    def preserve(self, state, now):
        pending = self.pending
        if pending is None or 'ack' not in pending:
            raise ValueError('Capture state without pending committed-image receipt')
        ack = pending['ack']
        if uint(state, 'submitted') != ack['native_frame']:
            raise ValueError('Capture image and following UI state frames differ')
        matching = ('screen', 'page', 'switches', 'requested_screen', 'requested_page',
                    'selected_group', 'selected_slot', 'saved_group', 'lifecycle', 'phase',
                    'aperture_state', 'aperture_input_enabled', 'gpu_publication',
                    'audio_mixed_frames', 'audio_captured_frames', 'audio_rate',
                    'audio_clock_microseconds', 'audio_accepted_nanoseconds')
        if any(state[key] != ack[key] for key in matching):
            raise ValueError('Capture image and following source state ownership fields differ')
        if uint(ack, 'accepted_update_sequence') != state['updates']:
            raise ValueError('Capture image does not name the accepted UI/audio update sequence')
        if state['input_events'] != pending['expected_input_events']:
            raise ValueError('Capture source input count differs from its key-edge expectation')
        live = self.out / 'live.ppm'
        before = live.stat()
        if not stat.S_ISREG(before.st_mode) or live.is_symlink():
            raise ValueError('Live PPM must be a regular nonsymlink file')
        with live.open('rb') as stream:
            image = stream.read(PPM_BYTES + 1)
        after = live.stat()
        keys = ('st_dev', 'st_ino', 'st_size', 'st_mtime_ns', 'st_ctime_ns')
        if any(getattr(before, k) != getattr(after, k) for k in keys):
            raise ValueError('Serialized acknowledged PPM changed while copying')
        if len(image) != PPM_BYTES or not image.startswith(PPM_HEADER):
            raise ValueError('Incomplete or incorrect640x448 P6 capture')
        number = self.captures + 1
        destination = self.out / 'frames' / ('frame-%06d.ppm' % number)
        part = destination.with_suffix('.ppm.part')
        copy_start = time.monotonic_ns()
        with part.open('xb') as stream:
            stream.write(image); stream.flush(); os.fsync(stream.fileno())
        os.replace(part, destination)
        record = {**pending, 'state': state, 'state_ack_monotonic_ns': now,
                  'frame_index': number, 'path': str(destination.relative_to(self.out)),
                  'sha256': hashlib.sha256(image).hexdigest(), 'bytes': len(image),
                  'copy_start_ns': copy_start, 'copy_complete_ns': time.monotonic_ns(),
                  'seconds_since_ready': (now - self.ready_ns) / 1e9,
                  'after_window': now >= self.deadline_ns,
                  'ack_late': now >= pending['ack_deadline_ns'],
                  'executable_sha256': self.manifest['executable']['sha256'],
                  'capture_hz_target': self.args.capture_hz,
                  'timing_provenance': 'Host observations of native capture receipts; no synthetic presentation timestamps'}
        payload = json.dumps(record, sort_keys=True) + '\n'
        size = len(payload.encode('utf-8'))
        if self.frame_log_bytes + size > MAX_FRAME_LOG:
            raise ValueError('Frame evidence reached its 16 MiB bound')
        self.frames.write(payload); self.frames.flush(); os.fsync(self.frames.fileno())
        self.frame_log_bytes += size
        self.captures = number; self.first_frame = self.first_frame or record; self.last_frame = record
        self.capture_ack_times.append(record['commit_ack_monotonic_ns'])
        self.saw_closed_input |= not state['input_shown'] and not state['aperture_input_enabled']
        self.saw_open_input |= state['input_shown'] and state['aperture_input_enabled']
        position = (state['aperture_top'], state['aperture_bottom'])
        if self.aperture_position is not None and position != self.aperture_position:
            self.aperture_moved = True
        self.aperture_position = position
        self.pending = None
        self.event('frame_preserved', frame_index=number, native_frame=ack['native_frame'])
        self.observe_navigation(record)
        if record['ack_late']:
            self.problem('Acknowledgment completed beyond original deadline')

    def line(self, text, now):
        self.event('stdout_line', received_ns=now, text=text)
        if text.startswith('interactive drive:') or text.startswith('ORIGINAL AI ') and 'boundary:' in text:
            self.terminal_lines.append({'received_ns': now, 'prefix': text[:2048]})
            self.terminal_lines = self.terminal_lines[-16:]
            self.problem('Native process failure/boundary: ' + text)
        if text.startswith('Development session restarting:') or text.startswith('Starting a fresh authored-grid session.'):
            self.problem('Unexpected process-internal session restart')
        if text.startswith('original_ui_destination unavailable'):
            self.terminal_lines.append({'received_ns': now, 'prefix': text[:2048]})
            self.terminal_lines = self.terminal_lines[-16:]
            self.problem('Requested UI destination was not admitted: ' + text)
        if text.startswith(('Development drive stopped:', 'live capture committed at tick ', 'capture_state wrong_way=',
                            'ui_state ', 'ORIGINAL AI FLEET ')):
            self.problem('A gameplay/host-menu path appeared in the original-UI-only log: ' + text)
        if text.startswith('original_main_ready '):
            row = json.loads(text[len('original_main_ready '):])
            if not isinstance(row, dict) or self.ready is not None:
                raise ValueError('Malformed or repeated original Main readiness')
            if uint(row, 'native_frame') != 1 or uint(row, 'width') != 640 or uint(row, 'height') != 448:
                raise ValueError('Readiness is not the first actual640x448 Main submission')
            if now >= self.launch_ns + int(self.args.startup_timeout * 1e9):
                raise ValueError('Main ready arrived after startup deadline')
            self.ready = row; self.ready_ns = now; self.deadline_ns = now + WINDOW_NS
            self.navigation_deadline_ns = now + int(self.args.navigation_timeout * 1e9)
            self.event('readiness_accepted', report=row, deadline_ns=self.deadline_ns)
        elif text.startswith('original_main_capture '):
            row = json.loads(text[len('original_main_capture '):])
            if self.pending is None or 'ack' in self.pending:
                raise ValueError('Unsolicited or repeated capture receipt')
            if uint(row, 'sequence') != self.pending['request_index']:
                raise ValueError('Capture sequence differs from request sequence')
            if uint(row, 'width') != 640 or uint(row, 'height') != 448 or row.get('file') != str(self.out / 'live.ppm'):
                raise ValueError('Capture target or dimensions differ from launched target')
            for key in ('screen', 'page', 'switches'):
                uint(row, key)
            self.check_audio(row, source=False, device=False)
            if uint(row, 'native_frame') < 1 or self.last_frame and row['native_frame'] < self.last_frame['ack']['native_frame']:
                raise ValueError('Capture native frame missing or regressed')
            self.pending['ack'] = row; self.pending['commit_ack_monotonic_ns'] = now
        elif text.startswith('original_main_state '):
            row = json.loads(text[len('original_main_state '):])
            if isinstance(row, dict):
                snapshot = {}
                for field in DIAGNOSTIC_FIELDS:
                    value = row.get(field)
                    snapshot[field] = value if type(value) in (bool, int, float, type(None)) else str(value)[:256]
                self.diagnostic_states.append({'received_ns': now, 'state': snapshot})
                self.diagnostic_states = self.diagnostic_states[-32:]
            self.check_state(row)
            if row.get('reason') == 'capture':
                self.preserve(row, now)
            elif row.get('reason') in ('ready', 'bound', 'quit'):
                if row['reason'] in self.seen_state_reasons:
                    raise ValueError('Repeated lifecycle state receipt')
                self.seen_state_reasons.add(row['reason'])
                self.states.append({'monotonic_ns': now, 'state': row})
            else:
                raise ValueError('Unrecognized original UI source state reason')
        elif text.startswith('original_main_closed '):
            if self.closed is not None or self.quit_ns is None:
                raise ValueError('Unexpected or repeated Main close receipt')
            row = json.loads(text[len('original_main_closed '):])
            if row.get('cleanup_complete') is not True or uint(row, 'world_ticks') != 0 or uint(row, 'window_swaps') != 0:
                raise ValueError('Main cleanup or headless no-driving ownership failed')
            if uint(row, 'captures') != self.captures or uint(row, 'native_frames') < 1:
                raise ValueError('Main cleanup receipt does not match acknowledged captures')
            if self.last_frame and row['native_frames'] < self.last_frame['ack']['native_frame']:
                raise ValueError('Main final frame count regressed')
            self.check_audio(row)
            self.closed = {'monotonic_ns': now, 'report': row}

    def drain(self, delay):
        for key, _ in self.selector.select(delay):
            chunk = os.read(key.fileobj.fileno(), 65536)
            if not chunk:
                self.eof = True; self.selector.unregister(key.fileobj); continue
            remaining = MAX_LOG - self.log_bytes
            if remaining > 0:
                kept = chunk[:remaining]; self.raw.write(kept); self.log_bytes += len(kept)
            if len(chunk) > remaining:
                self.problem('Raw log 32 MiB limit reached; retained prefix only')
                continue
            self.buffer += chunk
            while b'\n' in self.buffer:
                raw, self.buffer = self.buffer.split(b'\n', 1)
                if len(raw) > MAX_LINE:
                    self.problem('Oversized native log line'); continue
                try:
                    self.line(raw.decode('utf-8', errors='replace'), time.monotonic_ns())
                except (ValueError, TypeError, KeyError, OSError) as error:
                    self.last_rejected_receipt = {'received_ns': time.monotonic_ns(),
                                                  'prefix': raw.decode('utf-8', errors='replace')[:2048],
                                                  'error': str(error)[:2048]}
                    if self.first_rejected_receipt is None:
                        self.first_rejected_receipt = self.last_rejected_receipt
                    self.problem('Receipt rejected: ' + str(error))
            if len(self.buffer) > MAX_LINE:
                self.problem('Unterminated oversized native log line'); self.buffer = b''

    def cleanup_step(self, now):
        if self.pending and now < self.pending['ack_deadline_ns']:
            return
        if self.quit_ns is None:
            self.quit_ns = now
            try:
                self.send('quit')
            except (OSError, RuntimeError) as error:
                self.problem('Could not send explicit quit: ' + str(error))
        elif self.term_ns is None and now - self.quit_ns >= int(self.args.quit_timeout * 1e9):
            self.term_ns = now; self.kill(signal.SIGTERM)
        elif self.term_ns is not None and self.kill_ns is None and now - self.term_ns >= 2_000_000_000:
            self.kill_ns = now; self.kill(signal.SIGKILL)

    def kill(self, number):
        self.signals.append({'signal': number, 'monotonic_ns': time.monotonic_ns()})
        self.event('owned_process_group_signal', signal=number)
        try:
            os.killpg(self.proc.pid, number)
        except ProcessLookupError:
            pass

    def inspect_wav(self):
        # Structural RIFF validation only, after the owned process has exited.
        # No decoder, resampling, silence fabrication or audio quality claim.
        path = self.out / 'original-ui.wav'
        info = path.stat()
        if not stat.S_ISREG(info.st_mode) or path.is_symlink() or not 44 <= info.st_size <= MAX_WAV_BYTES:
            raise ValueError('WAV is missing, nonregular, or outside its hard byte bound')
        if self.closed is None:
            return {**identity(path), 'bytes': info.st_size, 'finalized': False,
                    'limitation': 'No successful cleanup receipt; retain as an untrusted partial WAV'}
        frames = self.closed['report']['audio_captured_frames']
        with path.open('rb') as stream:
            header = stream.read(44)
        expected = struct.pack('<4sI4s4sIHHIIHH4sI', b'RIFF', 36 + frames * 4, b'WAVE',
                               b'fmt ', 16, 1, 2, AUDIO_RATE, AUDIO_RATE * 4, 4, 16, b'data', frames * 4)
        if header != expected or info.st_size != 44 + frames * 4 or frames == 0:
            raise ValueError('Finalized WAV header/length differs from real closed PCM receipts')
        return {**identity(path), 'finalized': True, 'frames': frames, 'sample_rate': AUDIO_RATE,
                'channels': 2, 'sample_bits': 16, 'seconds_of_accepted_source_audio': frames / AUDIO_RATE,
                'decoded_or_audited_for_audible_quality': False, 'physical_playback_claimed': False}

    def run(self):
        old = {n: signal.signal(n, lambda number, frame: setattr(self, 'interrupted', number))
               for n in (signal.SIGINT, signal.SIGTERM)}
        try:
            self.launch_ns = time.monotonic_ns()
            self.event('single_launch_attempt', argv=self.manifest['argv'])
            self.proc = subprocess.Popen(self.manifest['argv'], cwd=str(ASSETS), env=self.manifest['environment'],
                                         stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                         bufsize=0, start_new_session=True, close_fds=True,
                                         preexec_fn=bound_child_files)
            os.set_blocking(self.proc.stdin.fileno(), False); os.set_blocking(self.proc.stdout.fileno(), False)
            self.selector.register(self.proc.stdout, selectors.EVENT_READ)
            running = identity(Path('/proc') / str(self.proc.pid) / 'exe')
            self.event('running_executable', identity=running)
            if running['sha256'] != self.manifest['executable']['sha256']:
                self.problem('Running executable differs from reviewed hash')
            exited_ns = None
            while True:
                self.drain(min(0.005, 0.25 / self.args.capture_hz)); now = time.monotonic_ns()
                if self.interrupted is not None:
                    self.stop('interrupted')
                if self.proc.poll() is not None:
                    exited_ns = exited_ns or now
                    if self.reason is None:
                        self.stop('unexpected_process_exit')
                    if self.eof or now - exited_ns >= 2_000_000_000:
                        break
                    continue
                if self.eof:
                    self.stop('stdout_eof_while_running')
                if self.ready_ns is None and now - self.launch_ns >= int(self.args.startup_timeout * 1e9):
                    self.stop('startup_timeout')
                if self.deadline_ns is not None and now >= self.deadline_ns:
                    self.stop('window_elapsed')
                if (self.roundtrip_ns is not None and self.last_frame is not None
                        and self.last_frame['state_ack_monotonic_ns'] - self.roundtrip_ns >= SEQUENCE_TAIL_NS
                        and self.pending is None):
                    self.stop('sequence_complete')
                wav = self.out / 'original-ui.wav'
                if wav.exists() and wav.stat().st_size >= MAX_WAV_BYTES:
                    self.problem('Original UI WAV reached the child-only hard file size bound')
                if (self.reason is None and self.roundtrip_frame is None
                        and self.navigation_deadline_ns is not None and now >= self.navigation_deadline_ns):
                    self.problem('Source navigation did not reach its next acknowledged gate before timeout')
                if self.pending and now >= self.pending['ack_deadline_ns'] and not self.pending.get('timeout_reported'):
                    self.pending['timeout_reported'] = True; self.problem('Capture acknowledgment timeout')
                if self.reason:
                    self.cleanup_step(now)
                    if self.kill_ns and now - self.kill_ns >= 2_000_000_000:
                        self.problem('Owned child unreaped after SIGKILL'); break
                elif self.ready_ns is not None and self.pending is None:
                    interval = math.ceil(1e9 / self.args.capture_hz)
                    if self.last_request is None or now - self.last_request >= interval:
                        self.request_capture()
        except BaseException as error:
            self.problem('Recorder exception: ' + type(error).__name__ + ': ' + str(error))
            if self.proc and self.proc.poll() is None:
                try:
                    if self.quit_ns is None:
                        self.quit_ns = time.monotonic_ns(); self.send('quit')
                    self.proc.wait(timeout=self.args.quit_timeout)
                except (OSError, RuntimeError, subprocess.TimeoutExpired):
                    self.kill(signal.SIGTERM)
                    try:
                        self.proc.wait(timeout=2)
                    except subprocess.TimeoutExpired:
                        self.kill(signal.SIGKILL)
                        try:
                            self.proc.wait(timeout=2)
                        except subprocess.TimeoutExpired:
                            self.problems.append('Owned child remained unreaped after exception cleanup')
        finally:
            for n, handler in old.items():
                signal.signal(n, handler)
            try:
                for _ in range(16):
                    if self.eof or not self.selector.get_map() or not self.selector.select(0):
                        break
                    self.drain(0)
                if self.buffer:
                    self.event('unterminated_log_suffix', text=self.buffer.decode('utf-8', errors='replace'))
                result = self.finish()
            except Exception as error:
                print('Summary unavailable; evidence prefix retained at %s: %s' % (self.out, error), file=sys.stderr)
                result = 2
            self.selector.close()
            if self.proc:
                self.proc.stdin.close(); self.proc.stdout.close()
            self.raw.close(); self.events.close(); self.frames.close()
        return result

    def finish(self):
        returncode = self.proc.poll() if self.proc else None
        try:
            ending = identity(Path(self.manifest['executable']['path']))
        except OSError as error:
            ending = {'error': str(error)}
        if returncode is not None:
            try:
                self.wav_observation = self.inspect_wav()
            except (OSError, ValueError, RuntimeError) as error:
                self.problem('WAV evidence remains partial/unverified: ' + str(error))
        transition_labels = [action[0] + suffix for index, action in enumerate(ACTIONS)
                             if self.route(CHECKPOINTS[index]) != self.route(CHECKPOINTS[index + 1])
                             for suffix in ('_closing', '_opening')]
        checks = {
            'actual_main_ready': self.ready is not None,
            'sequence_completed_within_forty_second_window':
                self.reason == 'sequence_complete' and self.stop_ns < self.deadline_ns,
            'two_or_more_acknowledged_frames': self.captures >= 2,
            'observed_source_opening_and_open_input': self.saw_closed_input and self.saw_open_input and self.aperture_moved,
            'initial_main_open': 'initial_main_open' in self.observations,
            'three_separate_main_downs_reached_slot4': 'main_options_selected' in self.observations,
            'all_twenty_one_open_checkpoints_observed': self.open_checkpoints == set(range(len(CHECKPOINTS))),
            'all_requested_source_closing_and_opening_transitions_sampled':
                all(label in self.observations for label in transition_labels),
            'cancel_discarded_real_edits_on_reentry': all(label in self.observations for label in
                ('cancel_edits_observed', 'cancel_returned_options', 'cancel_discard_verified_on_reentry')),
            'accept_persisted_profile_and_applied_real_sound_owner_on_reentry':
                all(label in self.observations for label in ('accept_edits_observed',
                    'accept_committed_and_returned_options', 'accept_persisted_on_reentry')),
            'three_screen_lifetimes_with_nonzero_stable_gpu_publication_tokens':
                len(self.gpu_publications) == 3,
            'returned_main_open_at_saved_slot4': self.roundtrip_frame is not None,
            'stable_return_tail_observed_in_actual_captures':
                self.roundtrip_ns is not None and self.last_frame is not None
                and self.last_frame['state_ack_monotonic_ns'] - self.roundtrip_ns >= SEQUENCE_TAIL_NS
                and self.open_input(self.last_frame['state'])
                and self.matches_checkpoint(self.last_frame['state'], len(ACTIONS)),
            'exactly_twenty_acknowledged_key_presses_and_release_barriers':
                self.inputs_sent == self.releases_sent == self.action_index == len(ACTIONS)
                and len(self.action_receipts) == len(ACTIONS) and self.active_action is None,
            'no_pending_capture': self.pending is None,
            'successful_main_cleanup_receipt': self.closed is not None,
            'explicit_quit_sent': self.quit_ns is not None,
            'native_process_exit_zero': returncode == 0,
            'no_forced_termination': not self.signals,
            'no_protocol_or_storage_errors': not self.problems and not self.event_io_failed,
            'reviewed_executable_unchanged': ending == self.manifest['executable'],
            'real_ui_wav_finalized_with_matching_receipts':
                self.wav_observation is not None and self.wav_observation.get('finalized') is True,
        }
        complete = all(checks.values())
        gaps = [b - a for a, b in zip(self.capture_ack_times, self.capture_ack_times[1:])]
        span = self.capture_ack_times[-1] - self.capture_ack_times[0] if len(self.capture_ack_times) >= 2 else 0
        cadence = {'requested_capture_hz': self.args.capture_hz,
                   'observed_capture_ack_hz': (len(gaps) * 1e9 / span) if span > 0 else None,
                   'maximum_capture_ack_gap_seconds': max(gaps) / 1e9 if gaps else None,
                   'capture_ack_span_seconds': span / 1e9,
                   'native_display_fps_claimed': False,
                   'wall_clock_video_mux_performed': False,
                   'interpolated_or_duplicated_frames_added': 0}
        summary = {'outcome': 'bounded_original_game_options_cancel_accept_observed' if complete else 'partial_or_failed_observation',
                   'checks': checks, 'stop_reason': self.reason, 'launch_ns': self.launch_ns,
                   'ready_ns': self.ready_ns, 'deadline_ns': self.deadline_ns, 'stop_ns': self.stop_ns,
                   'finished_ns': time.monotonic_ns(), 'finished_utc': utc(), 'return_code': returncode,
                   'requests': self.requests, 'frames': self.captures, 'pending': self.pending,
                   'ready': self.ready, 'closed': self.closed, 'first_frame': self.first_frame,
                   'last_frame': self.last_frame, 'lifecycle_states': self.states, 'problems': self.problems,
                   'source_observations': self.observations, 'completed_actions': self.action_receipts,
                   'observed_open_checkpoints': sorted(self.open_checkpoints),
                   'screen_gpu_publications': self.gpu_publications, 'wav': self.wav_observation,
                   'terminal_diagnostics': {'recent_source_states': self.diagnostic_states,
                                            'last_valid_source_state': self.last_state,
                                            'primary_problem': self.primary_problem,
                                            'first_rejected_receipt': self.first_rejected_receipt,
                                            'last_rejected_receipt': self.last_rejected_receipt,
                                            'terminal_native_lines': self.terminal_lines,
                                            'next_action_index': self.action_index,
                                            'active_action_index': self.active_action,
                                            'awaiting_release': self.need_release,
                                            'navigation_deadline_ns': self.navigation_deadline_ns,
                                            'raw_log_contains_full_receipt_prefix': True},
                   'roundtrip_frame': self.roundtrip_frame, 'roundtrip_ns': self.roundtrip_ns, 'inputs_sent': self.inputs_sent,
                   'releases_sent': self.releases_sent, 'cadence': cadence,
                   'bytes': {'raw_log': self.log_bytes, 'events': self.event_bytes,
                             'frame_log': self.frame_log_bytes, 'stdin': self.stdin_bytes,
                             'preserved_ppm': self.captures * PPM_BYTES,
                             'wav': self.wav_observation.get('bytes', self.wav_observation.get('stat', {}).get('st_size')) if self.wav_observation else None},
                   'forced_signals': self.signals, 'ending_executable': ending,
                   'limits': ['Only twenty bounded menu presses were allowed, using the original UI input owner.',
                              'RR_ORIGINAL_AI_ALL_RACERS=1 and RR_AUTODRIVE=0 remain required; world ticks must stay zero.',
                              'Only Main4/1 and Options12/{1,4} are covered; no other destination or gameplay launch.',
                              'Settings persistence means the same fresh process-owned profile on page re-entry, not an on-disk save.',
                              'Auto-save, split-screen and rumble flags are not edited; no consumer or hardware behavior is claimed.',
                              'Sound mode and SFX profile values are observed; all-channel volume propagation or audible quality is unclaimed.',
                              'Audio uses the genuine enabled UI owner with RR_AUDIO_DEVICE=0; PCM receipts/WAV are not physical playback.',
                              'WAV structure is checked without decoding, resampling or claiming audio/video synchronization.',
                              'GPU tokens are per owner; stability is checked within each reported screen lifetime, not as global identities.',
                              'Timestamps are real host observations, not invented native presentation times.',
                              'Thirty hertz is a request ceiling/target, not a measured thirty-frame-per-second result.',
                              'Every source-produced duplicate image is preserved; no interpolation, resampling or video mux.',
                              'A release barrier proves ordered host consumption; the current entry exposes no independent key-up receipt.',
                              'Visual quality and animation must be reviewed from retained frames and source state.',
                              'live.ppm alone is untrusted without its complete frames.jsonl record.']}
        atomic_json(self.out / 'summary.json', summary)
        print(json.dumps({'outcome': summary['outcome'], 'output': str(self.out), 'frames': self.captures}), flush=True)
        return 0 if complete else 2


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--expected-exe-sha256', required=True)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--root-reviewed-one-check', action='store_true')
    parser.add_argument('--not-before', required=True)
    parser.add_argument('--capture-hz', type=float, default=30.0)
    parser.add_argument('--startup-timeout', type=float, default=180.0)
    parser.add_argument('--ack-timeout', type=float, default=5.0)
    parser.add_argument('--quit-timeout', type=float, default=10.0)
    parser.add_argument('--navigation-timeout', type=float, default=8.0)
    args = parser.parse_args()
    if not args.root_reviewed_one_check:
        parser.error('Root review and launch authorization required; no process launched')
    if not re.fullmatch(r'[0-9a-fA-F]{64}', args.expected_exe_sha256):
        parser.error('Reviewed executable SHA256 must contain64 hex digits')
    for field, ceiling in (('capture_hz', 30), ('startup_timeout', 300), ('ack_timeout', 10),
                           ('quit_timeout', 15), ('navigation_timeout', 20)):
        if not 0 < getattr(args, field) <= ceiling:
            parser.error(field + ' outside its positive bounded range')
    gate = dt.datetime.fromisoformat(args.not_before.replace('Z', '+00:00'))
    if gate.tzinfo is None or gate < EARLIEST or dt.datetime.now(dt.timezone.utc) < gate:
        parser.error('Hourly launch gate not available; no waiting, retry or process launch')
    executable = args.executable.resolve(strict=True)
    if not stat.S_ISREG(executable.stat().st_mode) or not os.access(executable, os.X_OK):
        parser.error('Reviewed executable must be an executable regular file')
    with executable.open('rb') as stream:
        if stream.read(4) != b'\x7fELF':
            parser.error('Native ELF required; wrappers not accepted')
    executable_identity = identity(executable)
    if executable_identity['sha256'] != args.expected_exe_sha256.lower():
        parser.error('Reviewed executable hash mismatch')
    out = args.output.absolute()
    if out.parent.resolve() != STAGE.resolve() or out.exists() or out.is_symlink():
        parser.error('Output must be a new direct child directory of the recording stage')
    if not UI_ROOT.is_dir() or not AUDIO_ROOT.is_dir():
        parser.error('Original UI or original AUDIO input root is unavailable')
    corpus = identity(ASSETS / CORPUS)
    out.mkdir(mode=0o700)
    for name in ('frames', 'home', 'tmp', 'cache'):
        (out / name).mkdir(mode=0o700)
    environment = {'PATH': '/usr/bin:/bin', 'LANG': 'C', 'LC_ALL': 'C', 'TZ': 'UTC',
                   'HOME': str(out / 'home'), 'TMPDIR': str(out / 'tmp'),
                   'XDG_CACHE_HOME': str(out / 'cache'), **ENV,
                   'RR_AUDIO_WAV': str(out / 'original-ui.wav')}
    sources = ('tests/native/terrain_interactive_drive50du.cpp', 'tests/native/original_ui_main_entry50kg.h',
               'platform/linux/native/game_host50er.cpp', 'platform/linux/native/game_host50er.h',
               'platform/linux/native/vehicle_window_gl50ds.cpp',
               'platform/linux/native/original_ui_frontend50kg.cpp', 'platform/linux/native/original_ui_frontend50kg.h',
               'platform/linux/native/original_ui_main_session50kg.cpp',
               'platform/linux/native/original_ui_options_session50kg.cpp',
               'src/game/original_tournament_result50kg.cpp',
               'src/game/original_ui_tournament_input50kg.cpp',
               'src/assets/original_ui_tournament_page50kg.cpp',
               'platform/linux/native/original_ui_tournament_career_resources50kg.cpp',
               'src/assets/original_ui_screen_material_reference50kg.cpp',
               'src/game/original_world_environment50kg.cpp',
               'platform/linux/native/original_ui_audio_options50kg.cpp',
               'src/game/original_ui_audio_options50kg.cpp',
               'tests/native/original_ui_main_output50kg.h',
               'platform/linux/native/original_ui_main_audio50kg.cpp',
               'platform/linux/native/original_sound_mode50kg.h',
               'platform/linux/native/audio_output50hd.cpp', 'platform/linux/native/audio_mixer50hd.cpp',
               'src/game/original_ui_aperture_owner50kg.h', 'src/game/original_ui_transition_owner50kg.h',
               'src/game/original_ui_options_menu_controller50kg.cpp',
               'src/game/original_ui_game_options_controller50kg.cpp',
               'src/assets/original_ui_pages50kg.cpp')
    manifest = {'created_utc': utc(), 'executable': executable_identity, 'corpus': corpus,
                'recorder': identity(Path(__file__).resolve()),
                'current_source_files': [identity(REPO / path) for path in sources],
                'source_warning': 'Source hashes do not independently prove executable build provenance',
                'cwd': str(ASSETS), 'environment': environment,
                'argv': [str(executable), str(CORPUS), '--headless-live', str(out / 'live.ppm')],
                'allowed_stdin': ALLOWED_COMMANDS, 'maximum_window_seconds': WINDOW_NS / 1e9,
                'stable_return_tail_seconds': SEQUENCE_TAIL_NS / 1e9, 'capture_hz': args.capture_hz,
                'navigation_actions': ACTIONS, 'navigation_timeout': args.navigation_timeout,
                'expected_open_checkpoints': CHECKPOINTS,
                'startup_timeout': args.startup_timeout, 'ack_timeout': args.ack_timeout,
                'quit_timeout': args.quit_timeout, 'not_before': gate.isoformat(), 'one_launch_attempt': True,
                'bounds': {'captures': MAX_CAPTURES, 'ppm_bytes_each': PPM_BYTES, 'raw_log_bytes': MAX_LOG,
                           'event_bytes': MAX_EVENTS, 'frame_log_bytes': MAX_FRAME_LOG, 'stdin_bytes': MAX_STDIN_BYTES,
                           'wav_frames': MAX_WAV_FRAMES, 'wav_bytes': MAX_WAV_BYTES,
                           'child_rlimit_fsize_bytes': MAX_WAV_BYTES,
                           'preserved_ppm_bytes': MAX_CAPTURES * PPM_BYTES,
                           'maximum_evidence_bytes': MAX_CAPTURES * PPM_BYTES + 3 * MAX_WAV_BYTES
                               + MAX_LOG + MAX_EVENTS + MAX_FRAME_LOG + 4 * 1024 * 1024,
                           'evidence_bound_scope': 'Named evidence files; native home/tmp/cache files are separate. Child files individually share the WAV hard byte bound; core dumps disabled.',
                           'manifest_or_summary_bytes_each': 2 * 1024 * 1024},
                'hourly_gate_warning': 'This process cannot enforce the shared cross-task hourly test ledger; root must authorize exactly one check.',
                'authoring_status': 'Staged source only; not executed, built, or syntax-checked'}
    atomic_json(out / 'launch.json', manifest)
    return Recorder(args, out, manifest).run()


if __name__ == '__main__':
    sys.exit(main())
