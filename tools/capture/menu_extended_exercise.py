"""Future opt-in input coverage. No process creation or automatic execution.

Requires the frozen menu_exercise.py alongside this module. Its original flow
and one authorized racer-selection cold rebuild remain unchanged.
"""
import json

from menu_exercise import MenuExercise


class PausedUiSequence:
    """One actual key/capture request, then its matching native acknowledgment."""

    def __init__(self, name, initial, steps):
        self.name = name
        self.initial = initial
        self.steps = steps
        self.index = 0
        self.waiting = False
        self.sent_at = None
        self.frame = None
        self.events = []
        self.captures = []

    @property
    def done(self):
        return self.index == len(self.steps)

    def step(self, at, send, request_capture):
        if self.waiting:
            if at - self.sent_at > 10.0:
                raise TimeoutError('Extended menu action has no native acknowledgment: ' + self.steps[self.index]['name'])
            return
        if self.done:
            return
        item = self.steps[self.index]
        self.sent_at = at
        self.waiting = True
        if 'capture' in item:
            request_capture(item['capture'])
        else:
            send('down ' + item['key'])
            send('up ' + item['key'])

    def capture_committed(self, label, frame):
        if (not self.waiting or self.done or self.frame is not None
                or self.steps[self.index].get('capture') != label):
            raise RuntimeError('Unmatched extended menu image: ' + label)
        self.frame = frame
        self.captures.append(frame)  # Preserve completed-image evidence even if its UI ack fails.

    def observe(self, event):
        if not self.waiting or self.done:
            raise RuntimeError('Unexpected UI event outside an extended menu action')
        step = self.steps[self.index]
        expected_reason = 'capture' if 'capture' in step else 'transition'
        if event['reason'] != expected_reason:
            raise RuntimeError('Unexpected acknowledgment kind for ' + step['name'])
        if (event.get('menu') is not True or event.get('page') != step['page']
                or MenuExercise.identity_of(event) != MenuExercise.identity_of(self.initial)
                or MenuExercise.clocks(event) != MenuExercise.clocks(self.initial)):
            raise RuntimeError('Page/session/bank/paused clock mismatch for ' + step['name'])
        if any(event.get(key) != self.initial.get(key) for key in ('loaded_racer', 'selected_racer')):
            raise RuntimeError('Extended UI navigation changed a racer selection')
        effective = self.initial['audio_effective']
        if (event.get('audio_enabled') is not True or event.get('audio_effective') != effective
                or event.get('audio_requested') != step['requested']
                or event.get('audio_pending') is not (step['requested'] != effective)):
            raise RuntimeError('Pending/effective audio contract failed for ' + step['name'])
        receipt = dict(event, verified_action=step['name'])
        if 'capture' in step:
            if self.frame is None:
                raise RuntimeError('Extended UI telemetry arrived without its completed image')
            if self.frame['audio_sample_frames'] != event['mixed_frames']:
                raise RuntimeError('Paused WAV samples differ from native shared mix')
            self.frame['ui_state'] = receipt
            self.frame = None
        self.events.append(receipt)
        self.index += 1
        self.waiting = False


class ExtendedMenuExercise(MenuExercise):
    """Controls/audio/cancellation extension; no extra race start or process."""

    DEFAULT_AUDIO = [50, 100, 80, 80]

    def __init__(self, select_racer3=False):
        super().__init__(select_racer3=select_racer3)
        self.sequence = None
        self.sequence_resume = None
        self.front_extra_done = False
        self.pause_extra_done = False
        self.pending_audio_after_resume = None
        self.receipt['extended'] = {
            'enabled': True, 'verified': False, 'front_verified': False,
            'pause_verified': False, 'resume_kept_effective_audio': False,
            'events': [], 'captures': [], 'sequences': [],
            'scope': 'Controls/Back; all four audio edits; defaults; pending settings across Resume; Cancel-first Restart/Main',
            'not_covered': 'Applying edited audio on a cold restart; actual Restart/Main acceptance; Results; another track',
        }

    @staticmethod
    def plan():
        return []

    @staticmethod
    def key(steps, name, key, page, requested):
        steps.append({'name': name, 'key': key, 'page': page, 'requested': list(requested)})

    @staticmethod
    def capture(steps, label, page, requested):
        steps.append({'name': label, 'capture': label, 'page': page, 'requested': list(requested)})

    def down(self, steps, count, page, requested, name):
        for index in range(count):
            self.key(steps, name + '-row-' + str(index + 1), 'Down', page, requested)

    def start_sequence(self, name, event, steps, resume, at):
        if (event.get('audio_enabled') is not True
                or event.get('audio_effective') != self.DEFAULT_AUDIO
                or event.get('audio_requested') != self.DEFAULT_AUDIO
                or event.get('audio_pending') is not False):
            raise RuntimeError('Extended coverage requires actual fresh default audio settings')
        self.sequence = PausedUiSequence(name, event, steps)
        self.receipt['extended']['sequences'].append({
            'name': name, 'verified': False,
            'events': self.sequence.events, 'captures': self.sequence.captures,
        })
        self.sequence_resume = resume
        self.transition('extended-' + name, at)

    def front_plan(self):
        steps = self.plan()
        audio = list(self.DEFAULT_AUDIO)
        # Reviewed active layout: Start0, Racer1, Controls2, Audio3, Exit4.
        self.down(steps, 2, 1, audio, 'front-controls')
        self.key(steps, 'open-front-controls', 'Enter', 3, audio)
        self.capture(steps, 'extended-front-controls', 3, audio)
        self.key(steps, 'controls-back-enter', 'Enter', 1, audio)
        self.down(steps, 3, 1, audio, 'front-audio')
        self.key(steps, 'open-front-audio', 'Enter', 7, audio)
        self.capture(steps, 'extended-front-audio', 7, audio)
        for row, field in enumerate(('effects', 'music', 'character', 'dax')):
            if row:
                self.key(steps, 'audio-next-' + field, 'Down', 7, audio)
            increase = audio[row] < 100
            changed = list(audio)
            changed[row] += 5 if increase else -5
            self.key(steps, 'edit-' + field, 'Right' if increase else 'Left', 7, changed)
            self.capture(steps, 'extended-audio-' + field + '-pending', 7, changed)
            if row < 3:
                self.key(steps, 'restore-' + field, 'Left' if increase else 'Right', 7, audio)
            else:
                audio = changed
        self.key(steps, 'audio-defaults-row', 'Down', 7, audio)
        audio = list(self.DEFAULT_AUDIO)
        self.key(steps, 'restore-original-defaults', 'Enter', 7, audio)
        self.capture(steps, 'extended-audio-defaults', 7, audio)
        self.key(steps, 'audio-back-row', 'Down', 7, audio)
        self.key(steps, 'audio-back-enter', 'Enter', 1, audio)
        self.capture(steps, 'extended-front-restored', 1, audio)
        return steps

    def pause_plan(self):
        steps = self.plan()
        audio = list(self.DEFAULT_AUDIO)
        # Reviewed Pause: Resume0, Restart1, Main2, Controls3, Audio4, Exit5.
        self.down(steps, 3, 2, audio, 'pause-controls')
        self.key(steps, 'open-pause-controls', 'Enter', 3, audio)
        self.capture(steps, 'extended-pause-controls', 3, audio)
        self.key(steps, 'controls-back-escape', 'Escape', 2, audio)
        self.down(steps, 4, 2, audio, 'pause-audio')
        self.key(steps, 'open-pause-audio', 'Enter', 7, audio)
        audio[0] = 55
        self.key(steps, 'pause-edit-next-effects', 'Right', 7, audio)
        self.capture(steps, 'extended-pause-audio-pending', 7, audio)
        self.key(steps, 'audio-back-escape-keep-pending', 'Escape', 2, audio)
        self.capture(steps, 'extended-pause-pending', 2, audio)
        self.key(steps, 'restart-row', 'Down', 2, audio)
        self.key(steps, 'open-restart-confirmation', 'Enter', 4, audio)
        self.capture(steps, 'extended-restart-cancel-first', 4, audio)
        self.key(steps, 'cancel-restart-enter', 'Enter', 2, audio)
        self.down(steps, 2, 2, audio, 'main-confirmation')
        self.key(steps, 'open-main-confirmation', 'Enter', 5, audio)
        self.capture(steps, 'extended-main-cancel-first', 5, audio)
        self.key(steps, 'cancel-main-escape', 'Escape', 2, audio)
        self.capture(steps, 'extended-pause-cancelled', 2, audio)
        self.pending_audio_after_resume = list(audio)
        return steps

    def capture_committed(self, label, frame):
        if self.sequence is not None:
            self.sequence.capture_committed(label, frame)
        else:
            super().capture_committed(label, frame)

    def observe(self, line, at):
        if self.sequence is not None:
            self.observe_native(line, at)
            if not line.startswith(self.PREFIX):
                return None
            item = json.loads(line[len(self.PREFIX):])
            if (not isinstance(item, dict) or item.get('reason') not in {'transition', 'capture'}
                    or type(item.get('page')) is not int or not 1 <= item['page'] <= 7
                    or any(type(item.get(key)) is not int for key in ('session', 'audio_bank_id', 'tick', 'mixed_frames'))):
                raise RuntimeError('Malformed extended menu acknowledgment')
            event = dict(item, observed_elapsed=at)
            self.last_ui = event
            self.receipt['events'].append(event)
            self.sequence.observe(event)
            if self.sequence.done:
                extended = self.receipt['extended']
                extended['events'].extend(self.sequence.events)
                extended['captures'].extend(self.sequence.captures)
                extended[self.sequence.name + '_verified'] = True
                extended['sequences'][-1]['verified'] = True
                if self.sequence.name == 'front':
                    self.front_extra_done = True
                else:
                    self.pause_extra_done = True
                self.sequence = None
                self.transition(self.sequence_resume, at)
            return event
        event = super().observe(line, at)
        if event is None:
            return None
        if (not self.front_extra_done and event['reason'] == 'capture'
                and self.state in {'start_ready', 'select_row_ready'}):
            self.start_sequence('front', event, self.front_plan(), self.state, at)
        elif not self.pause_extra_done and event['reason'] == 'capture' and self.state == 'resume_ready':
            self.start_sequence('pause', event, self.pause_plan(), 'resume_ready', at)
        elif self.pause_extra_done:
            if (event.get('audio_effective') != self.DEFAULT_AUDIO
                    or event.get('audio_requested') != self.pending_audio_after_resume
                    or event.get('audio_pending') is not True):
                raise RuntimeError('Resume or final Main applied/discarded pending audio without a new session')
            if event['reason'] == 'capture' and self.state == 'video_ready':
                self.receipt['extended']['resume_kept_effective_audio'] = True
            if self.done:
                extended = self.receipt['extended']
                extended['verified'] = bool(self.front_extra_done and self.pause_extra_done
                                            and extended['resume_kept_effective_audio'])
                if not extended['verified']:
                    raise RuntimeError('Extended menu coverage did not complete')
        return event

    def step(self, at, latest_race, send, request_capture):
        if self.sequence is not None:
            self.sequence.step(at, send, request_capture)
        else:
            super().step(at, latest_race, send, request_capture)
