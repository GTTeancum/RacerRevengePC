"""Observation-only accounting for the frozen all-eight-original-AI telemetry."""
import math
import re


class AllOriginalAiEvidence:
    def __init__(self):
        self.headers = 0
        self.mode2 = False
        self.gc_disclosed = False
        self.contacts_enabled = False
        self.gc_normals = False
        self.observation_deadline_elapsed = None
        self.observation_window_seconds = None
        self.ui_camera_racer60 = None
        self.source_events = []
        self.global_rows = []
        self.groups = []
        self.pending_group = None
        self.ready_group = None
        self.races = []
        self.boundaries = []
        self.contract_violations = []

    @staticmethod
    def values(line):
        return dict(part.split('=', 1) for part in line.split() if '=' in part)

    def reject(self, event, reason):
        self.contract_violations.append(dict(event, reason=reason))
        raise RuntimeError(reason)

    def set_ui_camera_identity(self, racer60):
        if type(racer60) is not int or racer60 != 1:
            raise RuntimeError('This direct-start capture expects the actual default selected racer60=1')
        if self.ui_camera_racer60 is not None and self.ui_camera_racer60 != racer60:
            raise RuntimeError('Selected camera-followed original identity changed')
        self.ui_camera_racer60 = racer60

    def native_admitted(self):
        return (self.headers == 1 and self.mode2 and self.gc_disclosed
                and self.contacts_enabled and self.gc_normals)

    def ready_admitted(self):
        return bool(self.native_admitted() and self.ready_group is not None
                    and self.ui_camera_racer60 == self.ready_group['camera_racer60']
                    and not self.contract_violations)

    def observe(self, line, elapsed):
        event = {'line': line, 'observed_elapsed': elapsed}
        if ('forbidden HOST course driver' in line or line.startswith('HOST course-check autopilot enabled')
                or 'all-original-AI ownership requires RR_AUTODRIVE=0' in line):
            self.reject(event, 'Runtime reached forbidden HOST driving admission or call site')
        if line.startswith('HOST RACE:'):
            self.headers += 1
            self.source_events.append(event)
            if self.headers != 1 or not line.startswith('HOST RACE: 8 distinct original racers,'):
                self.reject(event, 'Requires exactly one cold session and eight actual racers')
        elif line.startswith('RACE MODE '):
            if self.mode2 or not line.startswith('RACE MODE 2:'):
                self.reject(event, 'Unexpected mode or repeated session')
            self.mode2 = True
            self.source_events.append(event)
        elif line.startswith('GC INCOMPLETE WORLD:'):
            if self.gc_disclosed:
                self.reject(event, 'Unexpected repeated GC cold setup')
            self.gc_disclosed = True
            self.source_events.append(event)
        elif line.startswith('GC source normals '):
            self.gc_normals = True
            self.source_events.append(event)
        elif line.startswith('RACER CONTACTS:'):
            if not line.startswith('RACER CONTACTS: enabled;'):
                self.reject(event, 'Real racer contacts must remain enabled')
            self.contacts_enabled = True
            self.source_events.append(event)
        elif line.startswith('original_ai_all '):
            if self.pending_group is not None:
                self.reject(event, 'Previous actual all-AI group lacked all eight rows')
            values = self.values(line)
            group = dict(event, values=values, rows={})
            for key in ('enabled', 'racer_count', 'human_count', 'ai_count', 'camera_slot', 'camera_racer60',
                        'host_driver_calls', 'controller_samples', 'keyboard_packets'):
                group[key] = int(values[key])
            group['reason'] = values['reason']
            self.global_rows.append(group)
            if (group['enabled'] != 1 or group['racer_count'] != 8 or group['human_count'] != 0
                    or group['ai_count'] != 8 or group['camera_slot'] != 0
                    or group['camera_racer60'] != self.ui_camera_racer60
                    or any(group[key] != 0 for key in ('host_driver_calls', 'controller_samples', 'keyboard_packets'))
                    or group['reason'] not in {'ready', 'status'}):
                self.reject(event, 'All-eight AI ownership/actual camera identity/zero input counters not established')
            if group['reason'] == 'ready' and self.ready_group is not None:
                self.reject(event, 'Repeated all-AI ready group would imply another cold session')
            if group['reason'] == 'status' and self.ready_group is None:
                self.reject(event, 'All-AI status arrived without actual cold-owner ready evidence')
            self.pending_group = group
        elif line.startswith('original_ai '):
            if self.pending_group is None:
                self.reject(event, 'Per-racer AI row lacks its actual all-AI group header')
            values = self.values(line)
            row = dict(event, values=values)
            for key in ('slot', 'source_index', 'racer60', 'vehicle58', 'initialized', 'owners_bound',
                        'active', 'phase', 'native_ticks', 'dispatched', 'state', 'startup', 'ordinary',
                        'brake_commands', 'bound_brake_steer', 'combat_scans', 'combat_ai_rejections', 'target1008'):
                row[key] = int(values[key])
            for key in ('CB8', 'steer', 'aeroBrakeEC', 'B94'):
                row[key] = float(values[key])
                if not math.isfinite(row[key]):
                    self.reject(event, 'Nonfinite actual AI telemetry')
            row['source_boundary'] = int(values['source_boundary'], 16)
            slot = row['slot']
            if (not 0 <= slot < 8 or row['source_index'] != slot or row['racer60'] <= 0
                    or row['vehicle58'] != 1 or row['initialized'] != 1 or row['owners_bound'] != 1
                    or row['active'] != 1 or row['phase'] != 0 or row['B94'] != 4
                    or row['source_boundary'] != 0
                    or not -1 <= row['target1008'] < 8 or row['target1008'] == slot
                    or any(row[key] < 0 for key in ('native_ticks', 'dispatched', 'startup', 'ordinary',
                                                   'brake_commands', 'bound_brake_steer', 'combat_scans', 'combat_ai_rejections'))):
                self.reject(event, 'Actual original-AI owner is incomplete, not Ready, inactive or mismatched')
            if row['state'] == 2 and row['CB8'] <= 5:
                self.reject(event, 'Sampled state 2 lacks genuine native CB8 > 5')
            if slot in self.pending_group['rows']:
                self.reject(event, 'Duplicate slot in actual all-AI telemetry group')
            if self.ready_group is not None:
                old = self.ready_group['rows'][slot]
                if row['racer60'] != old['racer60']:
                    self.reject(event, 'Actual per-slot original racer identity changed')
                previous = self.groups[-1]['rows'][slot]
                if any(row[key] < previous[key] for key in ('native_ticks', 'dispatched', 'startup', 'ordinary',
                                                            'brake_commands', 'bound_brake_steer', 'combat_scans', 'combat_ai_rejections')):
                    self.reject(event, 'Actual per-racer native or decision counters regressed')
            self.pending_group['rows'][slot] = row
            if len(self.pending_group['rows']) == 8:
                group = self.pending_group
                identities = [group['rows'][i]['racer60'] for i in range(8)]
                if len(set(identities)) != 8 or identities[0] != group['camera_racer60']:
                    self.reject(event, 'Eight distinct actual original identities and camera slot0 do not agree')
                group['completed_observed_elapsed'] = elapsed
                self.groups.append(group)
                if group['reason'] == 'ready':
                    self.ready_group = group
                self.pending_group = None
        elif line.startswith('race phase='):
            values = self.values(line)
            matches = re.findall(r'racer([1-8])\[([^\]]+)\]=([^ ]+)', line)
            rows = [{'presentation_row': int(index), 'name': name, 'fields': fields.split(',')}
                    for index, name, fields in matches]
            all_eight = (values.get('field_racers') == '8' and values.get('field_opponents') == '7'
                         and values.get('standings_rows') == '8' and values.get('presentation_valid') == '1'
                         and values.get('opponents_submitted') == '7' and len(rows) == 8
                         and {row['presentation_row'] for row in rows} == set(range(1, 9))
                         and len({row['name'] for row in rows}) == 8
                         and all(len(row['fields']) == 6 and row['fields'][-1] == '1' for row in rows))
            self.races.append(dict(event, values=values, rows=rows, all_eight_presented_active=all_eight))
        kind = None
        if line.startswith('ORIGINAL AI source boundary:'):
            kind = 'original_ai_unsupported_source_pc'
        elif line.startswith('ORIGINAL AI source-recovery boundary:'):
            kind = 'original_ai_source_recovery_rejected'
        elif line.startswith('ORIGINAL AI world boundary:'):
            kind = 'original_ai_partial_world_rejected'
        elif line.startswith('ORIGINAL AI session-rebuild boundary:'):
            kind = 'original_ai_session_rebuild_rejected'
        elif line.startswith('ORIGINAL AI dynamic-contact owner boundary:'):
            kind = 'original_ai_dynamic_contact_boundary'
        elif line.startswith('RACER CONTACT FAILURE:'):
            kind = 'native_contact_failure'
        elif (' stopped at a native source boundary; other racers continue.' in line
              or (' stopped: ' in line and '. Other racers continue.' in line)):
            kind = 'native_racer_stopped'
        elif line.startswith(('Host portal-boundary checkpoint recovery:', 'Portal boundary paused:',
                              'Starting source-derived host checkpoint session.', 'Starting a fresh authored-grid session.',
                              'Manual checkpoint ', 'Damage checkpoint recovery after ', 'Automatic checkpoint recovery after ',
                              'Damage recovery unavailable:', 'Checkpoint recovery unavailable:', 'interactive drive:')):
            kind = 'native_failure_or_recovery_boundary'
        if kind:
            self.boundaries.append(dict(event, kind=kind, values=self.values(line)))

    def report(self, frames):
        cutoff = min((event['observed_elapsed'] for event in self.boundaries), default=float('inf'))
        if self.observation_deadline_elapsed is not None:
            cutoff = min(cutoff, self.observation_deadline_elapsed)
        groups = [group for group in self.groups if group['completed_observed_elapsed'] < cutoff]
        ready = next((group for group in groups if group['reason'] == 'ready'), None)
        status = [group for group in groups if group['reason'] == 'status']
        per_racer = []
        for slot in range(8):
            initial = ready['rows'][slot] if ready else None
            rows = [group['rows'][slot] for group in status]
            last = rows[-1] if rows else None
            per_racer.append({
                'slot': slot, 'camera_followed': slot == 0,
                'actual_racer60': initial['racer60'] if initial else None,
                'source_index': initial['source_index'] if initial else None,
                'genuine_ai_owner_at_ready': initial is not None,
                'native_ticks_at_ready': initial['native_ticks'] if initial else None,
                'last_native_ticks': last['native_ticks'] if last else None,
                'native_tick_growth_observed': bool(initial and last and last['native_ticks'] > initial['native_ticks']),
                'AI_dispatch_growth_observed': bool(initial and last and last['dispatched'] > initial['dispatched']),
                'genuine_planner_growth_observed': bool(initial and last and
                    last['startup'] + last['ordinary'] > initial['startup'] + initial['ordinary']),
                'startup_state4_observed': any(row['state'] == 4 and row['startup'] > 0 for row in rows),
                'state2_native_CB8_gt5_observed': any(row['state'] == 2 and row['CB8'] > 5 and row['ordinary'] > 0 for row in rows),
                'brake_command_observed': any(row['brake_commands'] > 0 for row in rows),
                'simultaneous_native_bound_brake_steer_observed': any(row['bound_brake_steer'] > 0 for row in rows),
                'last_combat_scan_count': last['combat_scans'] if last else None,
                'last_combat_AI_rejection_count': last['combat_ai_rejections'] if last else None,
                'last_actual_target1008_manager_index': last['target1008'] if last else None,
            })
        fresh = [frame for frame in frames if frame.get('classification') == 'fresh_scene' and frame['ack_elapsed'] < cutoff]
        zero_inputs = bool(self.global_rows and not self.contract_violations and all(
            group[key] == 0 for group in self.global_rows
            for key in ('host_driver_calls', 'controller_samples', 'keyboard_packets')))
        milestones = {
            'direct_gc_single_session_admitted': self.native_admitted(),
            'all_eight_actual_original_ai_owners_at_ready': ready is not None,
            'camera_slot0_is_actual_original_ai': bool(ready and ready['rows'][0]['vehicle58'] == 1
                and ready['rows'][0]['racer60'] == self.ui_camera_racer60),
            'all_eight_native_ticks_advanced': all(row['native_tick_growth_observed'] for row in per_racer),
            'all_eight_AI_dispatches_advanced': all(row['AI_dispatch_growth_observed'] for row in per_racer),
            'all_eight_genuine_planners_advanced': all(row['genuine_planner_growth_observed'] for row in per_racer),
            'zero_HOST_driver_controller_sample_and_keyboard_packet_counters': zero_inputs,
            'fresh_rendered_camera_AI_tick_progress': bool(len(fresh) >= 2 and fresh[-1]['tick'] > fresh[0]['tick']),
        }
        return {
            'milestones': milestones,
            'all_eight_driving_evidence_satisfied': all(milestones.values()),
            'per_racer': per_racer,
            'all_eight_named_presentation_observed': any(event['all_eight_presented_active']
                and event['observed_elapsed'] < cutoff for event in self.races),
            'native_boundary_observed': bool(self.boundaries),
            'unsupported_stop_observed': bool(self.boundaries),
            'explicit_boundaries': self.boundaries,
            'contract_violations': self.contract_violations,
            'early_honest_boundary_does_not_erase_all_ai_ownership_proof': True,
            'surviving_full_selected_window_not_required_for_ownership_acceptance': True,
            'braking_and_state2_are_observations_not_forced_acceptance_requirements': True,
            'completed_lap_claim': False, 'full_original_ai_or_full_gc_claim': False,
            'retail_zero_human_bootstrap_claim': False,
            'telemetry_period_seconds': 1,
            'milestones_use_only_pre_boundary_and_pre_window_observations': True,
            'observation_deadline_elapsed': self.observation_deadline_elapsed,
            'observation_window_seconds': self.observation_window_seconds,
            'pending_partial_group': self.pending_group,
            'actual_fleet_groups': self.groups, 'global_counter_rows': self.global_rows,
            'race_presentation_samples': self.races, 'source_events': self.source_events,
            'AI_specific_terrain_support_not_directly_emitted': True,
            'bound_brake_steer_counter_proves_binding_not_same_frame_physics_completion': True,
        }
