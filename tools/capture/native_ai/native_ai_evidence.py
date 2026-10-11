"""Source-derived telemetry accounting only. Does not launch or alter a runtime."""
import math
import re


class NativeAiEvidence:
    def __init__(self):
        self.ai = []
        self.races = []
        self.native_ticks = []
        self.boundaries = []
        self.source_events = []
        self.headers = 0
        self.mode2 = False
        self.gc_disclosed = False
        self.contacts_enabled = False
        self.gc_normals = False
        self.observation_deadline_elapsed = None

    @staticmethod
    def values(line):
        return dict(part.split('=', 1) for part in line.split() if '=' in part)

    def observe(self, line, elapsed):
        event = {'line': line, 'observed_elapsed': elapsed}
        if line.startswith('HOST RACE:'):
            self.headers += 1
            self.source_events.append(event)
            if self.headers != 1 or not line.startswith('HOST RACE: 8 distinct original racers,'):
                raise RuntimeError('Expected exactly one cold session and all eight real racers')
        elif line.startswith('RACE MODE '):
            if self.mode2 or not line.startswith('RACE MODE 2:'):
                raise RuntimeError('Unexpected mode or repeated session')
            self.mode2 = True
            self.source_events.append(event)
        elif line.startswith('GC INCOMPLETE WORLD:'):
            if self.gc_disclosed:
                raise RuntimeError('Repeated GC cold setup')
            self.gc_disclosed = True
            self.source_events.append(event)
        elif line.startswith('GC source normals '):
            self.gc_normals = True
            self.source_events.append(event)
        elif line.startswith('RACER CONTACTS:'):
            if not line.startswith('RACER CONTACTS: enabled;'):
                raise RuntimeError('Real racer contacts must remain enabled')
            self.contacts_enabled = True
            self.source_events.append(event)
        elif line.startswith('original_ai '):
            values = self.values(line)
            item = dict(event, values=values)
            for key in ('slot', 'vehicle58', 'state', 'startup', 'ordinary', 'brake_commands', 'bound_brake_steer'):
                item[key] = int(values[key])
            for key in ('CB8', 'steer', 'aeroBrakeEC', 'B94'):
                item[key] = float(values[key])
                if not math.isfinite(item[key]):
                    raise RuntimeError('Nonfinite native AI telemetry')
            if item['slot'] != 1 or item['vehicle58'] != 1 or item['B94'] != 4:
                raise RuntimeError('Genuine slot-1 vehicle58=1/native rate ownership not observed')
            if item['state'] == 2 and item['CB8'] <= 5:
                raise RuntimeError('State 2 reported without genuine native CB8 > 5')
            self.ai.append(item)
        elif line.startswith('race phase='):
            values = self.values(line)
            rows = re.findall(r'racer([1-8])\[([^\]]+)\]=([^ ]+)', line)
            roster = [{'row': int(row), 'name': name, 'fields': fields.split(',')}
                      for row, name, fields in rows]
            all_eight = (values.get('field_racers') == '8' and values.get('field_opponents') == '7'
                         and values.get('standings_rows') == '8' and values.get('presentation_valid') == '1'
                         and values.get('opponents_submitted') == '7' and len(roster) == 8
                         and {r['row'] for r in roster} == set(range(1, 9))
                         and len({r['name'] for r in roster}) == 8
                         and all(len(r['fields']) == 6 and r['fields'][-1] == '1' for r in roster))
            self.races.append(dict(event, values=values, roster=roster, all_eight_active=all_eight))
        elif line.startswith('finish_continuation '):
            values = self.values(line)
            ticks = [int(values['racer%d_native_ticks' % i]) for i in range(8)]
            self.native_ticks.append(dict(event, ticks=ticks, active_native=int(values['active_native']),
                                          values=values))
        boundary_kind = None
        if line.startswith('ORIGINAL AI source boundary:'):
            boundary_kind = 'original_ai_unsupported_source_pc'
        elif line.startswith('ORIGINAL AI source-recovery boundary:'):
            boundary_kind = 'original_ai_source_recovery_rejected'
        elif line.startswith('ORIGINAL AI world boundary:'):
            boundary_kind = 'original_ai_partial_world_rejected'
        elif line.startswith('ORIGINAL AI session-rebuild boundary:'):
            boundary_kind = 'original_ai_session_rebuild_rejected'
        elif line.startswith('ORIGINAL AI dynamic-contact owner boundary:'):
            boundary_kind = 'original_ai_dynamic_contact_unsupported'
        elif line.startswith('RACER CONTACT FAILURE:'):
            boundary_kind = 'native_contact_failure'
        elif (' stopped at a native source boundary; other racers continue.' in line
              or (' stopped: ' in line and '. Other racers continue.' in line)):
            boundary_kind = 'native_racer_stopped'
        elif line.startswith(('Host portal-boundary checkpoint recovery:', 'Portal boundary paused:',
                              'Starting source-derived host checkpoint session.',
                              'Starting a fresh authored-grid session.', 'Manual checkpoint ',
                              'Damage checkpoint recovery after ', 'Automatic checkpoint recovery after ',
                              'Damage recovery unavailable:', 'Checkpoint recovery unavailable:',
                              'UNDERTRACK', 'Undertrack', 'NATIVE IMPACT', 'interactive drive:')):
            boundary_kind = 'native_failure_or_recovery_boundary'
        if boundary_kind:
            self.boundaries.append(dict(event, kind=boundary_kind, values=self.values(line)))

    def ready_admitted(self):
        return (self.headers == 1 and self.mode2 and self.gc_disclosed
                and self.contacts_enabled and self.gc_normals)

    def report(self, frames):
        cutoff = min((e['observed_elapsed'] for e in self.boundaries), default=float('inf'))
        if self.observation_deadline_elapsed is not None:
            cutoff = min(cutoff, self.observation_deadline_elapsed)
        ai_before_stop = [e for e in self.ai if e['observed_elapsed'] < cutoff]
        races_before_stop = [e for e in self.races if e['observed_elapsed'] < cutoff]
        startup = [e for e in ai_before_stop if e['state'] == 4 and e['startup'] > 0
                   and e['values'].get('source_boundary') == '0']
        ordinary = [e for e in ai_before_stop if e['state'] == 2 and e['CB8'] > 5 and e['ordinary'] > 0
                    and e['values'].get('source_boundary') == '0']
        ordered = bool(startup and ordinary and startup[0]['observed_elapsed'] < ordinary[-1]['observed_elapsed'])
        bound = [e for e in ai_before_stop if e['bound_brake_steer'] > 0]
        simultaneous = [e for e in ai_before_stop if e['steer'] != 0 and e['aeroBrakeEC'] == 1]
        native_samples = [e for e in self.native_ticks if e['active_native'] == 8
                          and e['observed_elapsed'] < cutoff and all(t > 1 for t in e['ticks'])]
        advancing = bool(len(native_samples) >= 2 and all(
            b > a for a, b in zip(native_samples[0]['ticks'], native_samples[-1]['ticks'])))
        fresh = [f for f in frames if f.get('classification') == 'fresh_scene'
                 and f['ack_elapsed'] < cutoff]
        milestones = {
            'direct_gc_single_session_admitted': self.ready_admitted(),
            'actual_eight_racers_observed_active': any(e['all_eight_active'] for e in races_before_stop),
            'all_eight_native_tick_counters_advanced_between_samples': advancing,
            'genuine_ai_slot1_vehicle58_observed': bool(ai_before_stop),
            'startup_state4_observed': bool(startup),
            'state2_with_native_CB8_gt5_observed': bool(ordinary),
            'state4_then_state2_observed_in_order': ordered,
            'genuine_brake_command_observed': any(e['brake_commands'] > 0 for e in ai_before_stop),
            'simultaneous_native_bound_brake_steer_counter_observed': bool(bound),
            'simultaneous_brake_steer_in_status_sample': bool(simultaneous),
            'fresh_rendered_gameplay_tick_progress': bool(len(fresh) >= 2 and fresh[-1]['tick'] > fresh[0]['tick']),
        }
        required = [key for key in milestones if key != 'simultaneous_brake_steer_in_status_sample']
        return {
            'milestones': milestones,
            'minimum_first_slice_milestones_observed': all(milestones[k] for k in required),
            'unsupported_stop_observed': bool(self.boundaries),
            'explicit_boundaries': self.boundaries,
            'completed_lap_claim': False,
            'full_original_ai_or_full_gc_claim': False,
            'cold_state0_directly_observed': False,
            'startup_and_ordinary_are_planner_counters_not_completed_physics': True,
            'bound_brake_steer_is_native_binding_evidence_not_same_frame_physics_completion': True,
            'AI_terrain_support_not_directly_emitted_by_frozen_telemetry': True,
            'telemetry_period_seconds': 5,
            'missing_evidence_is_not_inferred_from_wall_time': True,
            'milestones_use_only_evidence_observed_before_first_boundary': True,
            'milestones_use_only_evidence_observed_within_ready_window': True,
            'observation_deadline_elapsed': self.observation_deadline_elapsed,
            'ai_samples': self.ai, 'race_samples': self.races,
            'native_tick_samples': self.native_ticks, 'source_events': self.source_events,
        }
