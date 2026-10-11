"""Staged optional UI exercise. No process creation and no automatic execution."""
import json
import re


class MenuExercise:
    PREFIX = 'host_ui_state '

    def __init__(self, select_racer3=False):
        self.state = 'wait_front'
        self.changed_at = 0.0
        self.identity = None
        self.paused = None
        self.pause_first = None
        self.pending_capture = None
        self.hold_until = None
        self.resume_check_at = None
        self.started_transition_at = None
        self.last_ui = None
        self.select_racer3 = select_racer3
        self.selected_cold_admitted = False
        self.expected_cold_headers = 0
        self.expected_cold_starts = 0
        self.selected_native_roster = {}
        self.receipt = {
            'enabled': True, 'state': self.state, 'verified': False,
            'pause_hold_requested_seconds': 2.0, 'events': [], 'captures': [],
            'contract': 'host_ui_state JSON; one Start, pause/resume same prepared bank, final Main, then quit',
            'bank_identity_note': 'Actual audio-bank address plus native session counter; not a content hash',
            'select_racer3': select_racer3,
            'selection_method': 'Actual Main-menu row activation; no identity environment override' if select_racer3 else None,
            'pregame_cold_changes_allowed': 1 if select_racer3 else 0,
        }

    @property
    def video_ready(self):
        return self.state == 'video_ready'

    @property
    def done(self):
        return self.state == 'done'

    def transition(self, state, at):
        self.state = state
        self.changed_at = at
        self.receipt['state'] = state

    @staticmethod
    def identity_of(item):
        return item['session'], item['audio_bank_id']

    @staticmethod
    def clocks(item):
        return item['tick'], item['mixed_frames']

    def require_page(self, item, page):
        if item['page'] != page or item['menu'] != bool(page):
            raise RuntimeError('Unexpected native menu page in ' + self.state)
        if self.identity is not None and self.identity_of(item) != self.identity:
            raise RuntimeError('Native session/audio bank changed during the one-session menu exercise')

    def same_clocks(self, before, after):
        if self.clocks(before) != self.clocks(after):
            raise RuntimeError('Native tick or mixed audio advanced during the paused interval')

    def capture_committed(self, label, frame):
        if self.pending_capture is not None:
            raise RuntimeError('Previous menu capture lacks host_ui_state acknowledgement')
        self.pending_capture = (label, frame)
        self.receipt['captures'].append(frame)

    def admit_session_header(self, count, line, at):
        if count == 1:
            return
        if (not self.select_racer3 or self.state != 'wait_selected_cold_ready'
                or count != 2 or self.expected_cold_headers != 0
                or self.expected_cold_starts != 1):
            raise RuntimeError('Unexpected additional native race session')
        self.expected_cold_headers = 1
        self.receipt['selected_session_header'] = {'line': line, 'observed_elapsed': at}

    def observe_native(self, line, at):
        if not self.select_racer3:
            return
        if line == 'Starting a fresh authored-grid session.':
            if self.state != 'wait_selected_cold_ready' or self.expected_cold_starts != 0:
                raise RuntimeError('Unexpected cold restart outside initial racer selection')
            self.expected_cold_starts = 1
            self.receipt['selection_cold_start'] = {'line': line, 'observed_elapsed': at}
        match = re.match(r'^ORIGINAL RACER (\d+): (.+) \[(POD\d+)\]', line)
        if match and self.state == 'wait_selected_cold_ready':
            slot, name, pod = match.groups()
            if int(slot) in self.selected_native_roster:
                raise RuntimeError('Duplicate native racer installation in selected session')
            self.selected_native_roster[int(slot)] = {
                'slot': int(slot), 'name': name, 'pod': pod, 'line': line, 'observed_elapsed': at}

    def verify_live_roster(self, race):
        if not self.select_racer3:
            return
        names = dict((int(slot), name) for slot, name in
                     re.findall(r'racer(\d+)\[([^\]]+)\]=', race['line']))
        if (len(names) != 8 or set(names) != set(range(1, 9))
                or names[1] != 'Clegg Holdfast' or names[2] != 'Anakin Skywalker (ep1)'
                or len(set(names.values())) != 8):
            raise RuntimeError('Live roster does not show unique Clegg player and Anakin opponent slots')
        self.receipt.update(actual_roster_verified=True, live_selected_roster=race,
                            actual_role_contract='Native bank slot 0 / race tuple 1 is human; slots 1..7 are HOST opponents')

    def observe(self, line, at):
        self.observe_native(line, at)
        if not line.startswith(self.PREFIX):
            return None
        item = json.loads(line[len(self.PREFIX):])
        if (not isinstance(item, dict) or item.get('reason') not in {'ready', 'transition', 'capture'}
                or not isinstance(item.get('menu'), bool)
                or any(type(item.get(key)) is not int for key in
                       ('page', 'session', 'audio_bank_id', 'tick', 'mixed_frames'))
                or item['session'] <= 0 or item['audio_bank_id'] <= 0
                or not 0 <= item['page'] <= 6 or item['tick'] < 0 or item['mixed_frames'] < 0):
            raise RuntimeError('Malformed actual UI-state telemetry')
        event = dict(item, observed_elapsed=at)
        self.last_ui = event
        self.receipt['events'].append(event)
        if self.select_racer3:
            if any(type(item.get(key)) is not int for key in ('loaded_racer', 'selected_racer')):
                raise RuntimeError('Racer-selection exercise needs actual loaded and selected identity telemetry')
            if self.selected_cold_admitted and (item['loaded_racer'], item['selected_racer']) != (3, 3):
                raise RuntimeError('Loaded Clegg identity changed after the one permitted pregame cold selection')
        if self.state == 'wait_front':
            if item['reason'] != 'ready':
                raise RuntimeError('Initial UI acknowledgement was not ready')
            self.require_page(item, 1)
            self.identity = self.identity_of(item)
            self.receipt['initial_front'] = event
            if self.select_racer3 and (item['loaded_racer'], item['selected_racer']) != (1, 1):
                raise RuntimeError('Clegg test must begin from the genuine default Anakin frontend')
            self.transition('front_capture_ready', at)
        elif item['reason'] == 'transition':
            if self.state == 'wait_racer_row':
                self.require_page(item, 1)
                if (item['loaded_racer'], item['selected_racer']) != (1, 1):
                    raise RuntimeError('Row navigation unexpectedly changed the loaded/selected racer')
                self.transition('select_choice_ready', at)
            elif self.state == 'wait_racer_choice':
                self.require_page(item, 1)
                if (item['loaded_racer'], item['selected_racer']) != (1, 3):
                    raise RuntimeError('Actual menu activation did not select Clegg before loading')
                self.receipt['menu_choice_clegg'] = event
                self.transition('racer_choice_capture_ready', at)
            elif self.state == 'wait_start_row':
                self.require_page(item, 1)
                if (item['loaded_racer'], item['selected_racer']) != (1, 3):
                    raise RuntimeError('Clegg selection changed while returning to Start row')
                self.transition('selected_start_ready', at)
            elif self.state == 'wait_start_closed':
                self.require_page(item, 0)
                self.started_transition_at = at
                self.receipt['start_closed'] = event
                self.transition('wait_racing', at)
            elif self.state == 'wait_pause_open':
                self.require_page(item, 2)
                if item['tick'] <= 0 or item['mixed_frames'] <= 0:
                    raise RuntimeError('Pause must follow actual native and mixed-audio advancement')
                self.paused = event
                self.receipt['pause_open'] = event
                self.transition('pause_first_capture_ready', at)
            elif self.state == 'wait_resume_closed':
                self.require_page(item, 0)
                self.same_clocks(self.paused, item)
                self.receipt['resume_closed'] = event
                self.resume_check_at = at + 1.0
                self.transition('resume_settle', at)
            elif self.state == 'wait_final_front':
                self.require_page(item, 1)
                self.receipt['final_front_open'] = event
                self.transition('final_front_capture_ready', at)
            else:
                raise RuntimeError('Unexpected UI transition during ' + self.state)
        elif item['reason'] == 'capture':
            if self.pending_capture is None:
                if self.state != 'video_ready':
                    raise RuntimeError('UI capture telemetry lacks its requested capture')
                self.require_page(item, 0)
            else:
                label, frame = self.pending_capture
                frame['ui_state'] = event
                self.pending_capture = None
                if label == 'front':
                    self.require_page(item, 1)
                    self.transition('select_row_ready' if self.select_racer3 else 'start_ready', at)
                elif label == 'racer-choice':
                    self.require_page(item, 1)
                    if (item['loaded_racer'], item['selected_racer']) != (1, 3):
                        raise RuntimeError('Captured choice does not show selected Clegg with original bank retained')
                    self.transition('select_start_row_ready', at)
                elif label == 'pause-first':
                    self.require_page(item, 2)
                    self.same_clocks(self.paused, item)
                    self.pause_first = event
                    self.receipt['pause_first_pcm_sample_frames'] = frame['audio_sample_frames']
                    if frame['audio_sample_frames'] != item['mixed_frames']:
                        raise RuntimeError('Paused actual WAV count differs from native mixed-frame telemetry')
                    self.hold_until = at + 2.0
                    self.receipt['pause_first'] = event
                    self.transition('pause_hold', at)
                elif label == 'pause-held':
                    self.require_page(item, 2)
                    self.same_clocks(self.paused, item)
                    self.same_clocks(self.pause_first, item)
                    if (frame['audio_sample_frames'] != self.receipt['pause_first_pcm_sample_frames']
                            or frame['audio_sample_frames'] != item['mixed_frames']):
                        raise RuntimeError('Actual WAV advanced during the pause or disagrees with native mix count')
                    held = at - self.pause_first['observed_elapsed']
                    if held < 2.0:
                        raise RuntimeError('Paused observations are less than two wall seconds apart')
                    self.receipt.update(pause_held=event, pause_hold_measured_seconds=held,
                                        paused_clocks_unchanged=True, paused_pcm_sample_count_unchanged=True)
                    self.transition('resume_ready', at)
                elif label == 'resumed':
                    self.require_page(item, 0)
                    if not all(b > a for a, b in zip(self.clocks(self.paused), self.clocks(item))):
                        raise RuntimeError('Native and mixed clocks did not both advance after resume')
                    self.receipt.update(resumed=event, resumed_clocks_advanced=True,
                                        same_session_and_audio_bank=True)
                    self.transition('video_ready', at)
                elif label == 'final-front':
                    self.require_page(item, 1)
                    self.same_clocks(self.receipt['final_front_open'], item)
                    self.receipt.update(final_front_capture=event, verified=True)
                    self.transition('done', at)
                else:
                    raise RuntimeError('Unknown menu capture purpose')
        elif item['reason'] == 'ready':
            if (not self.select_racer3 or self.state != 'wait_selected_cold_ready'
                    or self.selected_cold_admitted or self.expected_cold_headers != 1
                    or self.expected_cold_starts != 1
                    or item['session'] != self.receipt['initial_front']['session'] + 1
                    or (item['loaded_racer'], item['selected_racer']) != (3, 3)
                    or item['menu'] or item['page'] != 0):
                raise RuntimeError('Unexpected second ready/session acknowledgement')
            expected = {0: ('Clegg Holdfast', 'POD03'), 1: ('Anakin Skywalker (ep1)', 'POD01')}
            if (set(self.selected_native_roster) != set(range(8))
                    or any((self.selected_native_roster[slot]['name'], self.selected_native_roster[slot]['pod']) != values
                           for slot, values in expected.items())):
                raise RuntimeError('Cold native roster does not actually install Clegg as player and Anakin as opponent')
            self.identity = self.identity_of(item)  # Exactly one authorized pregame replacement.
            self.selected_cold_admitted = True
            self.receipt.update(selected_session_ready=event, selected_native_roster=self.selected_native_roster,
                                pregame_cold_changes_observed=1)
            self.started_transition_at = at
            self.transition('wait_racing', at)
        return event

    def step(self, at, latest_race, send, request_capture):
        limit = 75 if self.state == 'wait_selected_cold_ready' else 15
        if self.state not in {'wait_front', 'video_ready', 'done'} and at - self.changed_at > limit:
            raise TimeoutError('Menu exercise stalled at ' + self.state)
        if self.state == 'front_capture_ready':
            self.transition('wait_front_capture', at)
            request_capture('front')
        elif self.state == 'start_ready':
            self.transition('wait_start_closed', at)
            # Activation is its own host event cycle, with no navigation pulse.
            send('down Enter')
            send('up Enter')
        elif self.state == 'select_row_ready':
            self.transition('wait_racer_row', at)
            send('down Down')
            send('up Down')
        elif self.state == 'select_choice_ready':
            self.transition('wait_racer_choice', at)
            send('down Enter')
            send('up Enter')
        elif self.state == 'racer_choice_capture_ready':
            self.transition('wait_racer_choice_capture', at)
            request_capture('racer-choice')
        elif self.state == 'select_start_row_ready':
            self.transition('wait_start_row', at)
            send('down Up')
            send('up Up')
        elif self.state == 'selected_start_ready':
            self.transition('wait_selected_cold_ready', at)
            self.receipt['selected_start_command_elapsed'] = at
            send('down Enter')
            send('up Enter')
        elif (self.state == 'wait_racing' and latest_race and latest_race['phase'] == 2
              and latest_race['elapsed'] > self.started_transition_at):
            self.verify_live_roster(latest_race)
            self.transition('wait_pause_open', at)
            send('pause')
        elif self.state == 'pause_first_capture_ready':
            self.transition('wait_pause_first_capture', at)
            request_capture('pause-first')
        elif self.state == 'pause_hold' and at >= self.hold_until:
            self.transition('wait_pause_held_capture', at)
            request_capture('pause-held')
        elif self.state == 'resume_ready':
            self.transition('wait_resume_closed', at)
            send('resume')
        elif self.state == 'resume_settle' and at >= self.resume_check_at:
            self.transition('wait_resumed_capture', at)
            request_capture('resumed')
        elif self.state == 'final_front_capture_ready':
            self.transition('wait_final_front_capture', at)
            request_capture('final-front')

    def finish_video(self, at, send):
        if self.state != 'video_ready':
            raise RuntimeError('Video ended before menu resume was verified')
        self.receipt['video_finished_elapsed'] = at
        self.transition('wait_final_front', at)
        send('menu')
