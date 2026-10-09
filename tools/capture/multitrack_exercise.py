"""Source-only staged TA -> incomplete GC UI controller. Never launches a process."""
import json


class MultiTrackExercise:
    PREFIX = 'host_ui_state '

    def __init__(self):
        self.state = 'wait_front'
        self.changed_at = 0.0
        self.identity = None
        self.track = 'TA'
        self.selected = 'TA'
        self.last_ui = None
        self.pending_capture = None
        self.expected = None
        self.actions = []
        self.frozen = None
        self.pause_first = None
        self.resume_baseline = None
        self.racing_since = None
        self.segment_started = 0.0
        self.cold_starts = 0
        self.headers = 0
        self.mode_logs = []
        self.normals = {}
        self.incomplete_log = None
        self.video_first = None
        self.video_previous = None
        self.receipt = {
            'enabled': True, 'verified': False, 'state': self.state,
            'contract': 'One process, prepared TA Start, real Pause/Resume, cancelled then confirmed Main, GC disclosure, one GC cold Start, dense GC video, quit',
            'events': [], 'captures': [], 'native_evidence': [],
            'cold_rebuilds_allowed': 1, 'cold_rebuilds_observed': 0,
            'bank_identity_note': 'Session counter plus actual bank-owner address. A cold allocator may reuse an address; identity is the pair.',
            'gc_complete_world_admitted': False,
            'known_gc_omissions': ['zone2 windmill animation/type1 collision',
                'zone11 triggered opening', 'authored LostAI branch triggers',
                'Gate sound', 'mode 1 record configuration'],
        }

    @property
    def video_ready(self):
        return self.state == 'video_ready'

    @property
    def done(self):
        return self.state == 'done'

    @staticmethod
    def identity_of(item):
        return item['session'], item['audio_bank_id']

    @staticmethod
    def clocks(item):
        return item['tick'], item['mixed_frames']

    def transition(self, state, at):
        self.state = state
        self.changed_at = at
        self.receipt['state'] = state

    def require_state(self, item, page, selected=None):
        selected = self.selected if selected is None else selected
        if (item['page'] != page or item['menu'] != bool(page)
                or self.identity_of(item) != self.identity
                or item['loaded_track'] != self.track
                or item['selected_track'] != selected
                or item['track_pending'] != (self.track != selected)
                or item['loaded_track_incomplete'] != (self.track == 'GC')):
            raise RuntimeError('Unexpected page, track, or bank/session identity at ' + self.state)
        if self.frozen is not None and self.clocks(item) != self.clocks(self.frozen):
            raise RuntimeError('Native tick or mixed clock advanced while a menu held the session')

    def add_key(self, key, page, label, **extra):
        self.actions.append(dict(kind='key', key=key, page=page, label=label, **extra))

    def add_capture(self, label, page, **extra):
        self.actions.append(dict(kind='capture', page=page, label=label, **extra))

    def build_ta_sequence(self):
        self.add_capture('ta-front', 1)
        self.add_key('Enter', 0, 'ta-start', unfreeze=True)
        self.actions.append(dict(kind='racing', seconds=3.0, label='ta-real-drive'))
        self.add_capture('ta-driving', 0, advancing=True)
        self.add_key('Escape', 2, 'ta-pause', freeze=True)
        self.add_capture('ta-pause-first', 2, pause_first=True)
        self.actions.append(dict(kind='delay', seconds=2.0, label='ta-pause-hold'))
        self.add_capture('ta-pause-held', 2, pause_held=True)
        self.add_key('Escape', 0, 'ta-resume', resume=True)
        self.actions.append(dict(kind='delay', seconds=1.0, label='ta-resume-settle'))
        self.add_capture('ta-resumed', 0, resumed=True)
        self.add_key('Escape', 2, 'ta-main-pause', freeze=True)
        self.add_key('Down', 2, 'ta-restart-row')
        self.add_key('Down', 2, 'ta-main-row')
        self.add_key('Enter', 5, 'ta-main-confirm-cancel-first')
        self.add_capture('ta-main-cancel-first', 5)
        self.add_key('Enter', 2, 'ta-main-cancelled')
        self.add_capture('ta-main-cancelled', 2)
        self.add_key('Down', 2, 'ta-restart-row-again')
        self.add_key('Down', 2, 'ta-main-row-again')
        self.add_key('Enter', 5, 'ta-main-confirm-again')
        self.add_key('Down', 5, 'ta-main-confirm-accept-row')
        self.add_key('Enter', 1, 'ta-main-confirmed')
        self.add_capture('ta-returned-main', 1)
        # Current admitted layout: Start0 / Racer1 / Controls2 / Audio3 / Track4 / Exit5.
        for row in range(1, 5):
            self.add_key('Down', 1, 'track-row-navigation-%d' % row)
        self.add_key('Enter', 8, 'open-tracks')
        self.add_capture('tracks-ta-selected', 8)
        self.add_key('Down', 8, 'gc-track-row')
        self.add_key('Enter', 9, 'gc-disclosure-cancel-first')
        self.add_capture('gc-incomplete-disclosure', 9)
        self.add_key('Down', 9, 'gc-disclosure-select-row')
        self.add_key('Enter', 1, 'gc-selected-pending', selected='GC')
        self.add_capture('gc-pending-ta-loaded', 1)
        self.actions.append(dict(kind='cold_start', label='gc-cold-start'))

    def admit_session_header(self, count, line, at):
        if count == 1 and self.state == 'wait_front' and self.headers == 0:
            self.headers = 1
        elif (count == 2 and self.state == 'wait_gc_ready' and self.headers == 1
              and self.cold_starts == 1):
            self.headers = 2
        else:
            raise RuntimeError('Unexpected native race session header; only one TA-to-GC cold rebuild is allowed')
        self.receipt['native_evidence'].append(dict(line=line, elapsed=at))

    def observe_native(self, line, at):
        if line == 'Starting a fresh authored-grid session.':
            if self.state != 'wait_gc_ready' or self.cold_starts != 0:
                raise RuntimeError('Unexpected fresh session outside the confirmed GC Start')
            self.cold_starts = 1
            self.receipt['cold_rebuilds_observed'] = 1
            self.receipt['native_evidence'].append(dict(line=line, elapsed=at))
        elif line.startswith('Starting source-derived host checkpoint session.'):
            raise RuntimeError('Unrequested checkpoint session')
        elif line.startswith('RACE MODE '):
            if not line.startswith('RACE MODE 2:') or len(self.mode_logs) >= 2:
                raise RuntimeError('Both sessions must retain explicitly requested mode 2')
            self.mode_logs.append(dict(line=line, elapsed=at))
            self.receipt['native_evidence'].append(self.mode_logs[-1])
        elif line.startswith(('TA source normals ', 'GC source normals ')):
            track = line[:2]
            if (track in self.normals or (track == 'TA' and self.state != 'wait_front')
                    or (track == 'GC' and self.state != 'wait_gc_ready')):
                raise RuntimeError('Unexpected duplicate or out-of-session track source-normal admission')
            self.normals[track] = dict(line=line, elapsed=at)
            self.receipt['native_evidence'].append(self.normals[track])
        elif line.startswith('GC INCOMPLETE WORLD:'):
            if self.state != 'wait_gc_ready' or self.incomplete_log is not None:
                raise RuntimeError('Unexpected GC incomplete-world admission')
            self.incomplete_log = dict(line=line, elapsed=at)
            self.receipt['native_evidence'].append(self.incomplete_log)

    def capture_committed(self, label, frame):
        if (self.pending_capture is not None or self.expected is None
                or self.expected['kind'] != 'capture' or self.expected['label'] != label):
            raise RuntimeError('Unexpected menu capture acknowledgement')
        self.pending_capture = frame
        self.receipt['captures'].append(frame)

    def observe(self, line, at):
        self.observe_native(line, at)
        if not line.startswith(self.PREFIX):
            return None
        item = json.loads(line[len(self.PREFIX):])
        bools = ('menu', 'track_pending', 'loaded_track_incomplete', 'gc_incomplete_opt_in',
                 'gc_mode2_admitted', 'audio_enabled', 'audio_pending', 'results_visible')
        if (not isinstance(item, dict) or item.get('reason') not in {'ready', 'transition', 'capture'}
                or any(type(item.get(k)) is not bool for k in bools)
                or any(type(item.get(k)) is not int for k in
                       ('page', 'session', 'audio_bank_id', 'tick', 'mixed_frames', 'loaded_racer', 'selected_racer'))
                or not 0 <= item['page'] <= 9 or item['session'] <= 0
                or item['audio_bank_id'] <= 0 or item['tick'] < 0 or item['mixed_frames'] < 0
                or (item['loaded_racer'], item['selected_racer']) != (1, 1)
                or not item['audio_enabled'] or item['audio_pending'] or item['results_visible']
                or not item['gc_incomplete_opt_in'] or not item['gc_mode2_admitted']):
            raise RuntimeError('Malformed or unexpected actual UI/track/audio telemetry')
        event = dict(item, observed_elapsed=at)
        self.receipt['events'].append(event)
        self.last_ui = event
        if self.state == 'wait_front':
            if (item['reason'] != 'ready' or item['session'] != 1 or self.headers != 1
                    or len(self.mode_logs) != 1 or 'TA' not in self.normals):
                raise RuntimeError('Initial ready lacks the real TA mode-2 source/session admission')
            self.identity = self.identity_of(item)
            self.require_state(item, 1)
            self.frozen = event
            self.receipt['ta_ready'] = event
            self.build_ta_sequence()
            self.transition('actions', at)
        elif self.state == 'wait_gc_ready':
            if (item['reason'] != 'ready' or item['session'] != self.identity[0] + 1
                    or self.headers != 2 or self.cold_starts != 1
                    or len(self.mode_logs) != 2 or 'GC' not in self.normals or self.incomplete_log is None):
                raise RuntimeError('GC ready lacks the single authorized cold source/session admission')
            self.identity = self.identity_of(item)
            self.track = self.selected = 'GC'
            self.frozen = None
            self.require_state(item, 0)
            self.receipt['gc_ready'] = event
            self.segment_started = at
            self.racing_since = None
            self.actions = [dict(kind='racing', seconds=1.0, label='gc-real-drive')]
            self.add_capture('gc-driving-before-video', 0, advancing=True)
            self.transition('actions', at)
        elif item['reason'] == 'ready':
            raise RuntimeError('Unexpected additional ready/session')
        elif item['reason'] == 'transition':
            if self.state != 'wait_action' or self.expected is None or self.expected['kind'] != 'key':
                raise RuntimeError('Unexpected UI transition at ' + self.state)
            action = self.expected
            selected = action.get('selected', self.selected)
            self.require_state(item, action['page'], selected)
            self.selected = selected
            if action.get('freeze'):
                self.frozen = event
            if action.get('resume'):
                self.resume_baseline = self.frozen
                self.frozen = None
            if action.get('unfreeze'):
                self.frozen = None
                self.segment_started = at
            self.receipt[action['label']] = event
            self.expected = None
            self.transition('actions', at)
        elif item['reason'] == 'capture':
            if self.state == 'video_ready':
                self.require_state(item, 0)
                if self.video_previous is not None and any(
                        b < a for a, b in zip(self.clocks(self.video_previous), self.clocks(item))):
                    raise RuntimeError('Native or mixed clock regressed within the GC video segment')
                if self.video_first is None:
                    self.video_first = event
                self.video_previous = event
            elif self.state == 'wait_action' and self.expected and self.expected['kind'] == 'capture':
                action, frame = self.expected, self.pending_capture
                if frame is None or frame['tick'] != item['tick']:
                    raise RuntimeError('Capture telemetry lacks its exact committed image/tick')
                self.require_state(item, action['page'])
                frame['ui_state'] = event
                if frame['audio_sample_frames'] is None:
                    raise RuntimeError('Actual native PCM evidence is missing')
                if self.frozen is not None and frame['audio_sample_frames'] != item['mixed_frames']:
                    raise RuntimeError('Paused actual WAV sample count differs from the frozen mix clock')
                if action.get('pause_first'):
                    self.pause_first = event
                    self.receipt['pause_first_pcm_frames'] = frame['audio_sample_frames']
                if action.get('pause_held'):
                    held = at - self.pause_first['observed_elapsed']
                    if held < 2 or frame['audio_sample_frames'] != self.receipt['pause_first_pcm_frames']:
                        raise RuntimeError('The two-second pause did not retain frozen actual PCM')
                    self.receipt.update(paused_clocks_unchanged=True, pause_hold_seconds=held)
                if action.get('resumed'):
                    if not all(b > a for a, b in zip(self.clocks(self.resume_baseline), self.clocks(item))):
                        raise RuntimeError('Both native and actual mixed clocks must advance after resume')
                    self.receipt.update(resumed_clocks_advanced=True, pause_resume_same_identity=True)
                if action.get('advancing'):
                    baseline = self.receipt['ta_ready' if self.track == 'TA' else 'gc_ready']
                    if not all(b > a for a, b in zip(self.clocks(baseline), self.clocks(item))):
                        raise RuntimeError('No real native/mixed advancement in the selected track')
                self.receipt[action['label']] = event
                self.expected = self.pending_capture = None
                if action.get('final'):
                    self.receipt['verified'] = True
                    self.transition('done', at)
                else:
                    self.transition('actions', at)
            else:
                raise RuntimeError('Unrequested actual UI capture at ' + self.state)
        return event

    def step(self, at, latest_race, send, request_capture):
        limit = 150 if self.state in {'wait_front', 'wait_gc_ready'} else 20
        if self.state not in {'video_ready', 'done'} and at - self.changed_at > limit:
            raise TimeoutError('Multi-track exercise stalled at ' + self.state)
        if self.state == 'wait_delay':
            if at >= self.expected['until']:
                self.expected = None
                self.transition('actions', at)
            return
        if self.state == 'wait_racing':
            if latest_race and latest_race['elapsed'] > self.segment_started:
                if latest_race['phase'] == 2:
                    if self.racing_since is None:
                        self.racing_since = at
                        self.receipt[self.expected['label'] + '-racing'] = latest_race
                    if at - self.racing_since >= self.expected['seconds']:
                        self.expected = None
                        self.transition('actions', at)
                elif self.racing_since is not None:
                    raise RuntimeError('Race left Racing during the short real drive')
            return
        if self.state != 'actions':
            return
        if not self.actions:
            if self.track != 'GC' or self.headers != 2 or self.cold_starts != 1:
                raise RuntimeError('Video readiness lacks the intended GC session')
            self.transition('video_ready', at)
            return
        action = self.actions.pop(0)
        self.expected = action
        if action['kind'] == 'key':
            self.transition('wait_action', at)
            send('down ' + action['key'])
            send('up ' + action['key'])
        elif action['kind'] == 'capture':
            self.transition('wait_action', at)
            request_capture(action['label'])
        elif action['kind'] == 'delay':
            action['until'] = at + action['seconds']
            self.transition('wait_delay', at)
        elif action['kind'] == 'racing':
            self.racing_since = None
            self.transition('wait_racing', at)
        elif action['kind'] == 'cold_start':
            if self.selected != 'GC' or self.track != 'TA' or self.cold_starts:
                raise RuntimeError('Cold Start was not preceded by the accepted GC menu choice')
            self.expected = None
            self.transition('wait_gc_ready', at)
            send('down Enter')
            send('up Enter')
        else:
            raise RuntimeError('Unknown controller action')

    def finish_video(self, at, send):
        if self.state != 'video_ready':
            raise RuntimeError('GC video ended before the selected session was admitted')
        if self.video_first is None or not all(b > a for a, b in
                zip(self.clocks(self.video_first), self.clocks(self.video_previous))):
            raise RuntimeError('GC video did not advance both actual native and mixed clocks')
        self.receipt['video_first'] = self.video_first
        self.receipt['video_last'] = self.video_previous
        self.receipt['video_finished_elapsed'] = at
        self.add_capture('gc-final-driving', 0, final=True)
        self.transition('actions', at)
