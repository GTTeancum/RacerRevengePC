"""Future genuine-finish UI exercise for an already-running authorized client.

This module never launches a process, fabricates a finish, changes game state,
or runs on import. A full-race driver supplies actual logs, captures, and input.
It is intentionally not wired into the short Racing-only video runner.
"""
import json
import math
import re


class ResultsExercise:
    PREFIX = 'host_ui_state '

    def __init__(self, return_to_main=False, finish_timeout_seconds=600.0):
        if not math.isfinite(finish_timeout_seconds) or finish_timeout_seconds <= 0:
            raise ValueError('An explicit positive genuine-finish observation bound is required')
        self.return_to_main = return_to_main
        self.finish_timeout_seconds = finish_timeout_seconds
        self.started_at = None
        self.finish_event = None
        self.sealed = None
        self.complete_race_seen = False
        self.identity = None
        self.last_ui = None
        self.steps = []
        self.index = 0
        self.waiting = False
        self.sent_at = None
        self.hold_until = None
        self.frame = None
        self.initial = None
        self.live_hold = None
        self.final_front = None
        self.main_action_seen = False
        self.state = 'wait_genuine_finish'
        self.receipt = {
            'enabled': True, 'state': self.state, 'verified': False,
            'return_to_main_requested': return_to_main,
            'genuine_finish_timeout_seconds': finish_timeout_seconds,
            'events': [], 'captures': [], 'confirmed_departures': [],
            'scope': 'Genuine Results; Cancel-first Restart/Main/Exit; live confirmation clocks; optional confirmed Main',
            'not_covered': 'Actual accepted Restart/Exit; arbitrary later track; original-game scene sequencing',
        }

    @property
    def done(self):
        return self.state == 'done'

    @staticmethod
    def identity_of(event):
        return event['session'], event['audio_bank_id']

    @staticmethod
    def clocks(event):
        return event['tick'], event['mixed_frames']

    def set_state(self, state):
        self.state = state
        self.receipt['state'] = state

    def operation(self, kind, name, action=0, confirming=False, page=0, key=None, seconds=None):
        self.steps.append(dict(kind=kind, name=name, action=action, confirming=confirming,
                               page=page, key=key, seconds=seconds))

    def prepare_steps(self):
        self.operation('capture', 'results-initial')
        self.operation('key', 'results-open-restart', confirming=True, key='Enter')
        self.operation('capture', 'results-restart-cancel-first', confirming=True)
        self.operation('hold', 'results-live-confirmation-hold', confirming=True, seconds=2.0)
        self.operation('capture', 'results-restart-live-held', confirming=True)
        self.operation('key', 'results-cancel-restart-escape', key='Escape')
        self.operation('capture', 'results-restart-cancelled')
        self.operation('key', 'results-select-main', action=1, key='Down')
        self.operation('key', 'results-open-main', action=1, confirming=True, key='Enter')
        self.operation('capture', 'results-main-cancel-first', action=1, confirming=True)
        self.operation('key', 'results-cancel-main-enter', action=1, key='Enter')
        self.operation('capture', 'results-main-cancelled', action=1)
        self.operation('key', 'results-select-exit', action=2, key='Down')
        self.operation('key', 'results-open-exit', action=2, confirming=True, key='Enter')
        self.operation('capture', 'results-exit-cancel-first', action=2, confirming=True)
        self.operation('key', 'results-cancel-exit-escape', action=2, key='Escape')
        self.operation('capture', 'results-exit-cancelled', action=2)
        if self.return_to_main:
            self.operation('key', 'results-return-select-main', action=1, key='Up')
            self.operation('key', 'results-return-open-main', action=1, confirming=True, key='Enter')
            self.operation('key', 'results-return-select-accept', action=1, confirming=True, key='Down')
            self.operation('key', 'results-return-confirm-main', action=1, confirming=True, page=1, key='Enter')
            # The next Main poll cancels the old Results confirmation in its
            # reducer and may emit a results reason without leaving Main.
            self.operation('capture', 'results-final-main', action=1, page=1)

    def capture_committed(self, label, frame):
        if (not self.waiting or self.frame is not None or self.index >= len(self.steps)
                or self.steps[self.index]['kind'] != 'capture' or self.steps[self.index]['name'] != label):
            raise RuntimeError('Unmatched genuine Results image: ' + label)
        self.frame = frame
        self.receipt['captures'].append(frame)  # Retain partial/failed evidence too.

    def observe(self, line, at):
        match = re.match(r'^RACE FINISH: player place=(\d+)/(\d+) time=([0-9.eE+\-]+) seconds\.$', line)
        if match:
            place, field, seconds = int(match[1]), int(match[2]), float(match[3])
            if not (1 <= place <= field and math.isfinite(seconds) and seconds > 0):
                raise RuntimeError('Invalid actual race-finish receipt')
            self.finish_event = dict(place=place, field=field, seconds=seconds, line=line, observed_elapsed=at)
        if line.startswith('race phase=3 '):
            self.complete_race_seen = True
        if line.startswith('finish_continuation '):
            values = dict(word.split('=', 1) for word in line.split()[1:] if '=' in word)
            ready = (values.get('player_finished') == '1' and values.get('results_closed') == '1'
                     and values.get('host_results_ready') == '1')
            if ready:
                sealed = (int(values['sealed_place']), float(values['sealed_time']), int(values['sealed_laps']))
                if not (sealed[0] > 0 and math.isfinite(sealed[1]) and sealed[1] > 0 and sealed[2] == 3):
                    raise RuntimeError('Results requires an actual sealed three-lap player finish')
                if self.sealed is not None and sealed != self.sealed:
                    raise RuntimeError('Official results changed during live Results presentation')
                self.sealed = sealed
                self.receipt['actual_finish_continuation'] = dict(values, line=line, observed_elapsed=at)
        if line.startswith('host_results_action '):
            values = dict(word.split('=', 1) for word in line.split()[1:] if '=' in word)
            if (not self.return_to_main or self.index >= len(self.steps)
                    or self.steps[self.index]['name'] != 'results-return-confirm-main'
                    or values.get('action') != '2' or self.main_action_seen):
                raise RuntimeError('A cancelled/unrequested Results action left the session')
            self.main_action_seen = True
            self.receipt['confirmed_departures'].append(dict(values, observed_elapsed=at))
        if not line.startswith(self.PREFIX):
            return None
        item = json.loads(line[len(self.PREFIX):])
        if (not isinstance(item, dict) or item.get('reason') not in {'ready', 'transition', 'results', 'capture'}
                or any(type(item.get(key)) is not int for key in ('page', 'session', 'audio_bank_id', 'tick', 'mixed_frames'))
                or not isinstance(item.get('menu'), bool)):
            raise RuntimeError('Malformed native UI observation in Results exercise')
        event = dict(item, observed_elapsed=at)
        self.last_ui = event
        self.receipt['events'].append(event)
        if self.identity is not None and self.identity_of(item) != self.identity:
            raise RuntimeError('Results navigation replaced the native/audio owner')
        if self.state == 'wait_genuine_finish':
            if item['reason'] == 'ready':
                # Allow the separately authorized pregame selection flow, but
                # never combine finish evidence from two cold sessions.
                self.finish_event = self.sealed = None
                self.complete_race_seen = False
            return event
        if self.done:
            return event
        # Main's first subsequent poll clears the now-inactive confirmation;
        # it is additional actual telemetry, not a second input acknowledgment.
        if (self.main_action_seen and item['reason'] == 'results' and item['page'] == 1
                and item.get('results_visible') is False and item.get('results_confirming') is False):
            return event
        if not self.waiting or self.index >= len(self.steps):
            raise RuntimeError('Unrequested Results UI transition/capture')
        step = self.steps[self.index]
        expected_reason = 'capture' if step['kind'] == 'capture' else 'transition' if step['page'] == 1 else 'results'
        if item['reason'] != expected_reason:
            raise RuntimeError('Unexpected Results acknowledgment for ' + step['name'])
        if (item['page'] != step['page'] or item['menu'] != bool(step['page'])
                or item.get('results_visible') is not (step['page'] == 0)
                or item.get('results_action') != step['action']
                or item.get('results_confirming') is not step['confirming']):
            raise RuntimeError('Results action/confirmation/page mismatch for ' + step['name'])
        if self.identity is None:
            self.identity = self.identity_of(item)
        if step['kind'] == 'capture':
            if self.frame is None or self.frame['tick'] != item['tick']:
                raise RuntimeError('Results image lacks matching actual native-frame acknowledgment')
            self.frame['ui_state'] = event
            if step['name'] == 'results-initial':
                self.initial = event
                self.receipt['initial'] = event
            elif step['name'] == 'results-restart-cancel-first':
                self.live_hold = (event, self.frame['audio_sample_frames'])
            elif step['name'] == 'results-restart-live-held':
                before, samples = self.live_hold
                if (at - before['observed_elapsed'] < 2.0
                        or not all(b > a for a, b in zip(self.clocks(before), self.clocks(item)))
                        or self.frame['audio_sample_frames'] <= samples):
                    raise RuntimeError('Results confirmation stopped native simulation or shared PCM')
                self.receipt['confirmation_native_mix_and_pcm_advance'] = True
            elif step['name'] == 'results-final-main':
                if not self.main_action_seen or self.clocks(item) != self.clocks(self.final_front):
                    raise RuntimeError('Confirmed Main did not freeze only after actual departure')
            self.frame = None
        elif step['name'] == 'results-return-confirm-main':
            if not self.main_action_seen:
                raise RuntimeError('Main transition lacks its confirmed Results action receipt')
            self.final_front = event
        self.index += 1
        self.waiting = False
        if self.index == len(self.steps):
            if not self.receipt.get('confirmation_native_mix_and_pcm_advance'):
                raise RuntimeError('Live Results continuation was never established')
            self.receipt['verified'] = True
            self.set_state('done')
        return event

    def step(self, at, latest_race, send, request_capture):
        if self.started_at is None:
            self.started_at = at
        if self.done:
            return
        if self.state == 'wait_genuine_finish':
            if at - self.started_at > self.finish_timeout_seconds:
                raise TimeoutError('No genuine sealed finish and ready Results within the declared bound')
            if not (self.finish_event and self.sealed and self.complete_race_seen):
                return
            if self.finish_event['place'] != self.sealed[0]:
                raise RuntimeError('Sealed place disagrees with actual finish event')
            self.receipt['actual_race_finish'] = self.finish_event
            self.receipt['sealed_result'] = self.sealed
            self.prepare_steps()
            self.set_state('exercise_results')
        if self.waiting:
            if at - self.sent_at > 10.0:
                raise TimeoutError('Results input/capture has no actual acknowledgment: ' + self.steps[self.index]['name'])
            return
        step = self.steps[self.index]
        if step['kind'] == 'hold':
            if self.hold_until is None:
                self.hold_until = at + step['seconds']
            if at >= self.hold_until:
                self.hold_until = None
                self.index += 1
            return
        self.sent_at = at
        self.waiting = True
        if step['kind'] == 'capture':
            request_capture(step['name'])
        else:
            send('down ' + step['key'])
            send('up ' + step['key'])
