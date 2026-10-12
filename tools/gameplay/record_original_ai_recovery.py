#!/usr/bin/env python3
"""Source-only candidate: one bounded, capture/quit-only original-AI process.

This file has not been executed or syntax-checked during preparation. Review it
before use. A successful recorder result is an observation, not race acceptance.
"""

import argparse
import datetime as dt
import hashlib
import json
import os
from pathlib import Path
import re
import selectors
import signal
import stat
import struct
import subprocess
import sys
import time


STAGING = Path("/workspace/shared/recovery-20261010/ai-capture-reconstruction")
REPO = Path("/workspace/shared/rr-recovery-git")
ASSETS = Path("/workspace/shared/recovery-20261010/native-assets-prepared-01")
CORPUS = Path("evidence/staticType2Producer49X/original-word/native-fixtures.json")
EARLIEST = dt.datetime(2026, 10, 10, 15, 18, tzinfo=dt.timezone.utc)
MAX_LINE_BYTES = 1_048_576
MAX_LOG_BYTES = 100 * 1024 * 1024
PPM_HEADER = b"P6\n640 480\n255\n"
PPM_BYTES = len(PPM_HEADER) + 640 * 480 * 3
RR_ENV = {
    "RR_TRACK": "GC",
    "RR_GC_INCOMPLETE_WORLD": "1",
    "RR_RACE": "1",
    "RR_RACE_MODE": "2",
    "RR_START_MENU": "0",
    "RR_ORIGINAL_AI_ALL_RACERS": "1",
    "RR_AUTODRIVE": "0",
    "RR_DISABLE_RACER_CONTACTS": "0",
    "RR_AUDIO": "1",
    "RR_AUDIO_DEVICE": "0",
    "RR_AUTHORED_WORLD_FOG": "1",
}
SOURCE_PATHS = (
    "tests/native/terrain_interactive_drive50du.cpp",
    "platform/linux/native/game_host50er.cpp",
    "src/game/vehicle_frame49w.h",
    "platform/linux/native/audio_output50hd.cpp",
)
GC_LIGHT_SOURCE_PATHS = (
    "src/game/original_gc_gameplay_lighting_candidate.h",
    "src/game/original_gc_gameplay_lighting_candidate.cpp",
    "src/game/native_gc_selected_lights_candidate.h",
    "src/game/original_racer_light_selection50km.h",
    "platform/linux/native/vehicle_shader_gl50cz.h",
    "platform/linux/native/vehicle_shader_gl50cz.cpp",
    "platform/linux/native/vehicle_window_gl50ds.h",
    "platform/linux/native/vehicle_window_gl50ds.cpp",
    "tools/prepare_interactive_build50dv.py",
)
COMMIT_RE = re.compile(
    r"^live capture committed at tick (\d+)( \(retained before undertrack pause\))?$"
)
BOUNDARY_RE = re.compile(r"^ORIGINAL AI .*\bboundary:")
COUNTERS = ("host_driver_calls", "controller_samples", "keyboard_packets")
EXPECTED_RACER60 = (1, 3, 4, 7, 8, 9, 10, 11)


def utc_now():
    return dt.datetime.now(dt.timezone.utc).isoformat()


def sha256_file(path):
    digest = hashlib.sha256()
    with open(path, "rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def file_identity(path):
    before = path.stat()
    digest = sha256_file(path)
    after = path.stat()
    keys = ("st_dev", "st_ino", "st_size", "st_mtime_ns", "st_ctime_ns")
    if any(getattr(before, key) != getattr(after, key) for key in keys):
        raise RuntimeError("File changed while hashing: " + str(path))
    return {
        "path": str(path), "sha256": digest,
        "stat": {key: getattr(after, key) for key in keys},
    }


def json_write(path, data):
    temporary = path.with_name(path.name + ".part")
    with temporary.open("x", encoding="utf-8") as stream:
        json.dump(data, stream, indent=2, sort_keys=True)
        stream.write("\n")
        stream.flush()
        os.fsync(stream.fileno())
    os.replace(temporary, path)


def append_json(stream, data):
    stream.write(json.dumps(data, sort_keys=True) + "\n")
    stream.flush()


def fields(line):
    return dict(re.findall(r"([^\s=]+)=([^\s]+)", line))


def integer(row, key):
    value = row.get(key)
    if value is None or not re.fullmatch(r"\d+", value):
        raise ValueError("Missing or invalid integer " + key)
    return int(value)


def read_wav(path, sample_rate, last_mixed_frames):
    result = {"path": str(path), "present": path.is_file(), "valid": False}
    if not result["present"]:
        return result
    result.update(file_identity(path))
    with path.open("rb") as stream:
        header = stream.read(44)
    result["file_bytes"] = path.stat().st_size
    if len(header) != 44:
        result["error"] = "Incomplete 44-byte source WAV header"
        return result
    (riff, riff_bytes, wave, fmt, fmt_bytes, encoding, channels, rate,
     byte_rate, alignment, bits, data, data_bytes) = struct.unpack(
         "<4sI4s4sIHHIIHH4sI", header
     )
    result.update({
        "riff_bytes": riff_bytes, "data_bytes": data_bytes,
        "sample_rate": rate, "channels": channels, "bits_per_sample": bits,
        "frames": data_bytes // 4, "last_capture_mixed_frames": last_mixed_frames,
        "duration_seconds": data_bytes / (rate * 4) if rate else None,
    })
    result["valid"] = bool(
        riff == b"RIFF" and wave == b"WAVE" and fmt == b"fmt "
        and data == b"data" and fmt_bytes == 16 and encoding == 1
        and channels == 2 and bits == 16 and alignment == 4
        and rate > 0 and rate == sample_rate and byte_rate == rate * 4
        and data_bytes % 4 == 0 and riff_bytes == data_bytes + 36
        and result["file_bytes"] == 44 + data_bytes and data_bytes > 0
        and (last_mixed_frames is None or data_bytes // 4 >= last_mixed_frames)
    )
    if not result["valid"]:
        result["error"] = "Header, actual length, format, or capture clock mismatch; retained unchanged"
    return result


class Recorder:
    def __init__(self, args, output, manifest):
        self.args = args
        self.output = output
        self.manifest = manifest
        self.proc = None
        self.selector = selectors.DefaultSelector()
        self.raw = (output / "game.log").open("xb", buffering=0)
        self.events = (output / "events.jsonl").open("x", encoding="utf-8")
        self.frames = (output / "frames.jsonl").open("x", encoding="utf-8")
        self.fleets = (output / "fleets.jsonl").open("x", encoding="utf-8")
        self.buffer = b""
        self.log_bytes = 0
        self.line_number = 0
        self.line_start_byte = 0
        self.eof = False
        self.process_exited_ns = None
        self.launch_ns = None
        self.ready_ns = None
        self.deadline_ns = None
        self.ready_ui = None
        self.ready_fleet = None
        self.native_gc_lighting = None
        self.native_ta_lighting = None
        self.audio_output = None
        self.sample_rate = None
        self.group = None
        self.groups = []
        self.last_rows = {}
        self.counter_headers = []
        self.pending = None
        self.last_request_ns = None
        self.capture_count = 0
        self.capture_requests = 0
        self.last_mixed_frames = None
        self.first_capture = None
        self.last_capture = None
        self.stop_reason = None
        self.stop_ns = None
        self.boundaries = []
        self.problems = []
        self.shutdown_actions = []
        self.quit_ns = None
        self.term_ns = None
        self.kill_ns = None
        self.signal_requested = None
        self.normal_stop_receipt = None
        self.event_io_failed = False

    def event(self, kind, **values):
        if self.event_io_failed:
            return
        try:
            append_json(self.events, {
                "kind": kind, "received_monotonic_ns": time.monotonic_ns(), **values,
            })
        except OSError as error:
            # Failed evidence storage must never prevent owned-child cleanup.
            self.event_io_failed = True
            self.problems.append({"monotonic_ns": time.monotonic_ns(),
                                  "message": "Event log write failed: " + str(error)})
            if self.stop_reason is None:
                self.stop_reason = "evidence_storage_failure"
                self.stop_ns = time.monotonic_ns()

    def stop(self, reason, detail=None):
        if self.stop_reason is None:
            self.stop_reason = reason
            self.stop_ns = time.monotonic_ns()
            self.event("stop", reason=reason, detail=detail)

    def problem(self, message):
        self.problems.append({"monotonic_ns": time.monotonic_ns(), "message": message})
        self.event("problem", message=message)
        self.stop("evidence_or_protocol_failure", message)

    def send(self, command):
        if command not in ("capture", "quit"):
            raise RuntimeError("Only capture and quit are permitted")
        if not self.proc or self.proc.poll() is not None:
            raise RuntimeError("Cannot send command to an exited process")
        payload = (command + "\n").encode("ascii")
        before = time.monotonic_ns()
        written = os.write(self.proc.stdin.fileno(), payload)
        after = time.monotonic_ns()
        self.event("stdin", command=command, write_start_ns=before,
                   write_complete_ns=after, bytes_written=written)
        if written != len(payload):
            raise RuntimeError("Partial command write; no retry")
        return after

    def request_capture(self):
        # This starts a new request, not a poll of the existing request. Keep
        # the defensive invariant here; the event loop must wait while pending.
        if self.pending is not None:
            raise RuntimeError("A second capture cannot be outstanding")
        now = time.monotonic_ns()
        interval = int(1_000_000_000 / self.args.capture_hz + 0.999999)
        if self.last_request_ns is not None and now - self.last_request_ns < interval:
            return
        if self.ready_ns is None or now >= self.deadline_ns or self.stop_reason:
            return
        sent = self.send("capture")
        self.last_request_ns = sent
        self.capture_requests += 1
        self.pending = {
            "request_index": self.capture_requests,
            "request_monotonic_ns": sent,
            "request_since_ready_seconds": (sent - self.ready_ns) / 1e9,
            "ack_deadline_ns": sent + int(self.args.ack_timeout * 1e9),
            "stage": "commit",
        }

    def check_ui(self, ui):
        wanted = {
            "session": 1, "menu": False, "loaded_track": self.args.track,
            "loaded_racer": 1,
            "selected_track": self.args.track, "track_pending": False,
            "loaded_track_incomplete": self.args.track == "GC", "gc_incomplete_opt_in": self.args.track == "GC",
            # Runtime publishes the explicit RR_RACE_MODE=2 admission flag,
            # independently of loaded track, so this remains True for TA.
            "gc_mode2_admitted": True, "audio_enabled": True,
            "audio_pending": False,
        }
        for key, value in wanted.items():
            if key not in ui or ui[key] != value:
                raise ValueError("Unexpected host_ui_state " + key)
        for key in ("tick", "mixed_frames", "audio_bank_id", "loaded_racer"):
            if type(ui.get(key)) is not int or ui[key] < 0:
                raise ValueError("Invalid host_ui_state " + key)
        if self.ready_ui and (
            ui["audio_bank_id"] != self.ready_ui["audio_bank_id"]
            or ui["loaded_racer"] != self.ready_ui["loaded_racer"]
        ):
            raise ValueError("Live audio/racer owner identity changed")

    def fleet_header(self, line, now):
        if self.group is not None:
            raise ValueError("New fleet header before all eight previous rows")
        row = fields(line)
        self.counter_headers.append({"monotonic_ns": now, "fields": row})
        for key, value in {
            "enabled": 1, "racer_count": 8, "human_count": 0,
            "ai_count": 8, "camera_slot": 0, "camera_racer60": 1,
            **{key: 0 for key in COUNTERS},
        }.items():
            if integer(row, key) != value:
                raise ValueError("All-AI header check failed: " + key)
        if row.get("reason") not in ("ready", "status"):
            raise ValueError("Unexpected fleet report reason")
        if row["reason"] == "ready" and self.ready_fleet is not None:
            raise ValueError("A second ready report would imply a rebuilt session")
        if self.ready_ui and integer(row, "camera_racer60") != self.ready_ui["loaded_racer"]:
            raise ValueError("Fleet camera owner differs from loaded UI racer")
        self.group = {"header": row, "header_monotonic_ns": now, "rows": {}}

    def fleet_row(self, line, now):
        if self.group is None:
            raise ValueError("Racer row without a fleet header")
        row = fields(line)
        slot = integer(row, "slot")
        if slot != len(self.group["rows"]) or slot > 7:
            raise ValueError("Fleet rows must be unique ordered slots 0 through 7")
        for key, value in {
            "source_index": slot, "vehicle58": 1, "initialized": 1,
            "owners_bound": 1, "active": 1, "phase": 0,
            "racer60": EXPECTED_RACER60[slot],
        }.items():
            if integer(row, key) != value:
                raise ValueError("Racer slot %d failed %s" % (slot, key))
        for key in ("native_ticks", "dispatched", "startup", "ordinary"):
            current = integer(row, key)
            previous = self.last_rows.get(slot)
            if previous and current < integer(previous, key):
                raise ValueError("Racer slot %d counter regressed: %s" % (slot, key))
        if int(row["source_boundary"], 16) != 0:
            raise ValueError("Fleet row contains a nonzero native source boundary")
        self.group["rows"][slot] = row
        if len(self.group["rows"]) != 8:
            return
        group = self.group
        self.group = None
        if len({integer(row, "native_ticks") for row in group["rows"].values()}) != 1:
            raise ValueError("Fleet rows disagree on shared completedFrames")
        group["complete_monotonic_ns"] = now
        group["group_index"] = len(self.groups)
        append_json(self.fleets, group)
        self.groups.append(group)
        self.last_rows = dict(group["rows"])
        if group["header"]["reason"] == "ready":
            if self.ready_ui is None or self.audio_output is None:
                raise ValueError("Complete ready fleet lacks ready UI/audio evidence")
            if self.args.native_gc_selected_lights and self.native_gc_lighting is None:
                raise ValueError("Requested native GC light-owner policy was not observed before ready")
            if self.args.native_ta_selected_lights and self.native_ta_lighting is None:
                raise ValueError("Requested native TA light-owner policy was not observed before ready")
            startup_deadline = self.launch_ns + int(self.args.startup_timeout * 1e9)
            if now >= startup_deadline:
                self.stop("startup_timeout", "Ready report arrived after startup deadline")
                return
            if self.stop_reason is not None:
                return
            self.ready_fleet = group
            self.ready_ns = now
            self.deadline_ns = now + int(self.args.ready_window_seconds * 1e9)
            self.event("readiness_accepted", ready_monotonic_ns=now,
                       target_deadline_monotonic_ns=self.deadline_ns,
                       fleet_group_index=group["group_index"])

    def commit_capture(self, ui, now):
        pending = self.pending
        if pending is None or pending["stage"] != "ui":
            raise ValueError("Capture UI without both preceding acknowledgments")
        pending["host_ui_state"] = ui
        pending["ui_ack_monotonic_ns"] = now
        pending["ui_ack_since_ready_seconds"] = (now - self.ready_ns) / 1e9
        pending["source_executable_sha256"] = self.manifest["executable"]["sha256"]
        pending["source_identity_manifest"] = "launch.json"
        pending["latest_complete_fleet_group_index"] = len(self.groups) - 1
        pending["after_target_deadline"] = now > self.deadline_ns
        pending["after_reported_boundary"] = bool(self.boundaries)
        problems = []
        # drain() can deliver the final acknowledgment before the outer loop
        # gets to its timeout check. Preserve the evidence, but never erase an
        # expired deadline by clearing pending after a late acknowledgment.
        pending["ack_deadline_exceeded"] = now >= pending["ack_deadline_ns"]
        if pending["ack_deadline_exceeded"] and not pending.get("timeout_reported"):
            pending["timeout_reported"] = True
            problems.append("Complete capture acknowledgment arrived after its deadline")
        try:
            self.check_ui(ui)
            if ui.get("reason") != "capture":
                raise ValueError("Acknowledging UI row is not a capture")
            if ui["tick"] != pending["tick"]:
                raise ValueError("Capture commit and UI ticks differ")
            if pending["capture_state"]["tick"] != pending["tick"]:
                raise ValueError("Capture state and commit ticks differ")
            if pending["capture_state"]["host_menu"] != 0 or pending["retained_before_undertrack_pause"]:
                raise ValueError("Retained/menu image is not active gameplay")
            if self.last_capture and pending["tick"] < self.last_capture["tick"]:
                raise ValueError("Capture tick regressed")
            if self.last_mixed_frames is not None and ui["mixed_frames"] < self.last_mixed_frames:
                raise ValueError("Audio clock regressed")
        except (ValueError, KeyError) as error:
            problems.append(str(error))
        live = self.output / "live.ppm"
        before = live.stat()
        with live.open("rb") as stream:
            image = stream.read(PPM_BYTES + 1)
        after = live.stat()
        if (before.st_ino, before.st_size, before.st_mtime_ns) != (
            after.st_ino, after.st_size, after.st_mtime_ns
        ):
            raise ValueError("Live PPM changed during serialized copy")
        if len(image) != PPM_BYTES or not image.startswith(PPM_HEADER):
            raise ValueError("Live PPM is not complete source 640 by 480 P6 output")
        number = self.capture_count + 1
        destination = self.output / "frames" / ("frame-%06d.ppm" % number)
        temporary = destination.with_suffix(".ppm.part")
        pending["copy_started_monotonic_ns"] = time.monotonic_ns()
        with temporary.open("xb") as stream:
            stream.write(image)
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(temporary, destination)
        pending.update({
            "frame_index": number, "frame_path": str(destination.relative_to(self.output)),
            "frame_sha256": hashlib.sha256(image).hexdigest(),
            "frame_bytes": len(image), "copy_complete_monotonic_ns": time.monotonic_ns(),
            "mixed_frames": ui.get("mixed_frames"),
            "audio_seconds": ui["mixed_frames"] / self.sample_rate if self.sample_rate else None,
            "capture_problems": problems,
        })
        pending.pop("stage")
        append_json(self.frames, pending)
        os.fsync(self.frames.fileno())
        self.capture_count = number
        self.first_capture = self.first_capture or dict(pending)
        self.last_capture = dict(pending)
        self.last_mixed_frames = ui.get("mixed_frames")
        self.pending = None
        self.event("frame_preserved", frame_index=number, frame_sha256=pending["frame_sha256"])
        for problem in problems:
            self.problem(problem)

    def process_line(self, line, now):
        # The raw log and timestamped line event are saved before parsing.
        if line.startswith("NATIVE GC SELECTED LIGHTS:"):
            if not self.args.native_gc_selected_lights:
                raise ValueError("Unexpected native GC selected-light policy")
            if self.native_gc_lighting is not None:
                raise ValueError("Duplicate native GC light-owner initialization")
            self.native_gc_lighting = {"monotonic_ns": now, "line": line}
        if line.startswith("NATIVE TA SELECTED LIGHTS:"):
            if not self.args.native_ta_selected_lights:
                raise ValueError("Unexpected native TA selected-light policy")
            if self.native_ta_lighting is not None:
                raise ValueError("Duplicate native TA light-owner initialization")
            self.native_ta_lighting = {"monotonic_ns": now, "line": line}
        if BOUNDARY_RE.match(line):
            self.boundaries.append({"monotonic_ns": now, "line": line})
            self.stop("source_boundary", line)
        if line.startswith("interactive drive:"):
            self.stop("source_boundary" if "boundary" in line else "game_failure", line)
        if line.startswith("Development drive stopped: ticks="):
            self.normal_stop_receipt = {"monotonic_ns": now, "line": line}
        if line.startswith("Development session restarting:") or line.startswith("Starting a fresh authored-grid session."):
            raise ValueError("Unexpected session restart")
        if line.startswith("Audio output "):
            row = fields(line)
            if row.get("device") != "not_requested" or integer(row, "capture") != 1:
                raise ValueError("Expected WAV capture with no audio device")
            self.sample_rate = integer(row, "rate")
            if self.sample_rate <= 0:
                raise ValueError("Invalid audio output rate")
            self.audio_output = row
        if line.startswith("original_ai_all "):
            self.fleet_header(line, now)
        elif line.startswith("original_ai slot="):
            self.fleet_row(line, now)
        match = COMMIT_RE.match(line)
        if match:
            if self.pending is None or self.pending["stage"] != "commit":
                raise ValueError("Unexpected or duplicate capture-commit acknowledgment")
            self.pending.update({
                "tick": int(match.group(1)),
                "retained_before_undertrack_pause": bool(match.group(2)),
                "commit_ack_monotonic_ns": now, "stage": "state",
            })
        elif line.startswith("capture_state "):
            if self.pending is None or self.pending["stage"] != "state":
                raise ValueError("capture_state arrived before commit acknowledgment")
            row = fields(line)
            self.pending.update({
                "capture_state": {key: integer(row, key) for key in ("wrong_way", "tick", "host_menu")},
                "state_ack_monotonic_ns": now, "stage": "ui",
            })
        elif line.startswith("host_ui_state "):
            ui = json.loads(line[len("host_ui_state "):])
            if ui.get("reason") == "ready":
                if self.ready_ui is not None:
                    raise ValueError("Duplicate ready UI")
                self.check_ui(ui)
                self.ready_ui = ui
            elif self.pending is not None and self.pending["stage"] == "ui":
                self.commit_capture(ui, now)
            elif ui.get("reason") == "capture":
                raise ValueError("Unrequested or out-of-order capture UI")
            else:
                self.check_ui(ui)

    def drain(self, wait_seconds):
        for key, _mask in self.selector.select(wait_seconds):
            data = os.read(key.fd, 65536)
            if not data:
                self.selector.unregister(key.fd)
                self.eof = True
                return
            self.raw.write(data)
            self.log_bytes += len(data)
            self.buffer += data
            while b"\n" in self.buffer:
                raw_line, self.buffer = self.buffer.split(b"\n", 1)
                now = time.monotonic_ns()
                self.line_number += 1
                line = raw_line.decode("utf-8", errors="replace").rstrip("\r")
                self.event("game_line", line_number=self.line_number,
                           log_byte_offset=self.line_start_byte,
                           line_received_monotonic_ns=now, text=line)
                self.line_start_byte += len(raw_line) + 1
                try:
                    self.process_line(line, now)
                except (ValueError, KeyError, TypeError, OSError) as error:
                    self.problem("Line %d: %s" % (self.line_number, error))
            if len(self.buffer) > MAX_LINE_BYTES:
                self.problem("Game log line exceeded bounded parser capacity")
                self.event("unparsed_line_prefix", bytes=len(self.buffer))
                self.line_start_byte += len(self.buffer)
                self.buffer = b""
            if self.log_bytes > MAX_LOG_BYTES:
                self.stop("log_limit", "Game log exceeded 100 MiB")

    def send_signal(self, signum, label):
        try:
            os.killpg(self.proc.pid, signum)
            self.shutdown_actions.append({"monotonic_ns": time.monotonic_ns(), "action": label})
            self.event("process_group_signal", signal=label)
        except ProcessLookupError:
            pass
        except OSError as error:
            self.problem("Cannot signal owned process group: " + str(error))

    def shutdown_step(self, now):
        # A pending request gets only its original bounded acknowledgment time.
        # No final extra capture is sent, and no retry is attempted.
        if self.pending is not None and now < self.pending["ack_deadline_ns"]:
            return
        if self.quit_ns is None:
            self.quit_ns = now
            try:
                self.send("quit")
                self.shutdown_actions.append({"monotonic_ns": now, "action": "quit"})
            except (OSError, RuntimeError) as error:
                self.problem("Quit could not be written: " + str(error))
            return
        if self.term_ns is None and now - self.quit_ns >= int(self.args.quit_timeout * 1e9):
            self.term_ns = now
            self.send_signal(signal.SIGTERM, "SIGTERM")
        elif self.term_ns is not None and self.kill_ns is None and now - self.term_ns >= 2_000_000_000:
            self.kill_ns = now
            self.send_signal(signal.SIGKILL, "SIGKILL")

    def run(self):
        def interruption(signum, _frame):
            self.signal_requested = signum

        old_signals = {number: signal.signal(number, interruption) for number in (signal.SIGINT, signal.SIGTERM)}
        try:
            self.launch_ns = time.monotonic_ns()
            self.event("launch_attempt", command=self.manifest["command"], cwd=str(ASSETS))
            self.proc = subprocess.Popen(
                self.manifest["command"], cwd=str(ASSETS), env=self.manifest["environment"],
                stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                bufsize=0, start_new_session=True, close_fds=True,
            )
            os.set_blocking(self.proc.stdin.fileno(), False)
            os.set_blocking(self.proc.stdout.fileno(), False)
            self.selector.register(self.proc.stdout, selectors.EVENT_READ)
            self.event("process_started", pid=self.proc.pid, launch_monotonic_ns=self.launch_ns)
            try:
                running = file_identity(Path("/proc") / str(self.proc.pid) / "exe")
                self.event("running_executable_identity", identity=running)
                if running["sha256"] != self.manifest["executable"]["sha256"]:
                    self.problem("Running executable hash differs from reviewed identity")
            except OSError as error:
                self.problem("Could not verify running executable identity: " + str(error))
            startup_deadline = self.launch_ns + int(self.args.startup_timeout * 1e9)
            while True:
                self.drain(0.025)
                now = time.monotonic_ns()
                if self.signal_requested is not None:
                    self.stop("interrupted", "Recorder received signal %d" % self.signal_requested)
                return_code = self.proc.poll()
                if return_code is not None:
                    if self.process_exited_ns is None:
                        self.process_exited_ns = now
                        self.event("process_exited", return_code=return_code)
                    if self.stop_reason is None:
                        self.stop("unexpected_process_exit", str(return_code))
                    if self.eof or now - self.process_exited_ns > 2_000_000_000:
                        break
                    continue
                if self.eof:
                    self.stop("log_eof", "Process remains alive without its log stream")
                if self.ready_ns is None and now >= startup_deadline:
                    self.stop("startup_timeout", "Complete all-eight readiness was not observed")
                if self.ready_ns is not None and now >= self.deadline_ns:
                    self.stop("window_elapsed")
                if self.pending is not None and now >= self.pending["ack_deadline_ns"]:
                    if not self.pending.get("timeout_reported"):
                        self.pending["timeout_reported"] = True
                        self.problem("Complete capture acknowledgment timed out; partial output retained")
                if self.stop_reason is not None:
                    self.shutdown_step(now)
                    if self.kill_ns is not None and now - self.kill_ns >= 2_000_000_000:
                        self.problem("Process did not reap within two seconds of SIGKILL")
                        break
                elif self.pending is None:
                    # Pending is normal across any number of polls/partial
                    # reads. Only commit_capture clears it, after the commit,
                    # capture_state, and following UI acknowledgment have been
                    # processed and the copied frame's manifest is durable.
                    # Until then, keep draining and checking the original ack
                    # deadline; do not request again or refresh that deadline.
                    self.request_capture()
        except BaseException as error:
            self.problem("Recorder exception: %s: %s" % (type(error).__name__, error))
            if self.proc is not None and self.proc.poll() is None:
                self.send_signal(signal.SIGTERM, "SIGTERM after recorder exception")
                try:
                    self.proc.wait(timeout=2)
                except subprocess.TimeoutExpired:
                    self.send_signal(signal.SIGKILL, "SIGKILL after recorder exception")
                    try:
                        self.proc.wait(timeout=2)
                    except subprocess.TimeoutExpired:
                        self.problem("Owned process still unreaped after exception cleanup")
        finally:
            for number, handler in old_signals.items():
                signal.signal(number, handler)
            # Drain already-readable bytes without restarting gameplay or a capture.
            try:
                for _ in range(16):
                    if self.eof or not self.selector.get_map():
                        break
                    if not self.selector.select(0):
                        break
                    self.drain(0)
            except Exception as error:
                self.problem("Final log drain failed: " + str(error))
            if self.buffer:
                self.event("unterminated_log_suffix", bytes=len(self.buffer),
                           text=self.buffer.decode("utf-8", errors="replace"))
            try:
                result = self.finish()
            except Exception as error:
                # In particular, a full disk can prevent the final summary.
                # Preserve all existing files and still close our resources.
                print("Recorder summary unavailable; retained prefix at %s: %s" %
                      (self.output, error), file=sys.stderr, flush=True)
                result = 2
            self.selector.close()
            if self.proc is not None:
                self.proc.stdin.close()
                self.proc.stdout.close()
            for stream in (self.raw, self.events, self.frames, self.fleets):
                stream.close()
        return result

    def finish(self):
        completed = time.monotonic_ns()
        observed_zero = bool(self.counter_headers) and all(
            all(item["fields"].get(key) == "0" for key in COUNTERS)
            for item in self.counter_headers
        )
        growth = {}
        if self.ready_fleet is not None:
            for slot in range(8):
                first = self.ready_fleet["rows"][slot]
                last = self.last_rows.get(slot, first)
                growth[slot] = {
                    "native_ticks_first": integer(first, "native_ticks"),
                    "native_ticks_last": integer(last, "native_ticks"),
                    "dispatched_first": integer(first, "dispatched"),
                    "dispatched_last": integer(last, "dispatched"),
                    "ordinary_last_observed": integer(last, "ordinary"),
                    "advanced": integer(last, "native_ticks") > integer(first, "native_ticks")
                    and integer(last, "dispatched") > integer(first, "dispatched"),
                }
        try:
            wav = read_wav(self.output / "audio.wav", self.sample_rate, self.last_mixed_frames)
        except (OSError, ValueError, struct.error) as error:
            wav = {"path": str(self.output / "audio.wav"), "valid": False, "error": str(error)}
        try:
            ending_executable = file_identity(Path(self.manifest["executable"]["path"]))
        except OSError as error:
            ending_executable = {"error": str(error)}
        identity_unchanged = ending_executable == self.manifest["executable"]
        return_code = self.proc.poll() if self.proc is not None else None
        checks = {
            "all_eight_readiness_observed": self.ready_fleet is not None,
            "requested_native_gc_light_owner_observed": not self.args.native_gc_selected_lights
            or self.native_gc_lighting is not None,
            "requested_native_ta_light_owner_observed": not self.args.native_ta_selected_lights
            or self.native_ta_lighting is not None,
            "no_partial_fleet_report": self.group is None,
            "all_observed_driver_controller_keyboard_counters_zero": observed_zero,
            "all_eight_native_tick_and_dispatch_growth_observed": len(growth) == 8
            and all(row["advanced"] for row in growth.values()),
            "at_least_two_acknowledged_frames": self.capture_count >= 2,
            "no_pending_capture": self.pending is None,
            "valid_source_wav": wav["valid"],
            "reviewed_executable_identity_unchanged": identity_unchanged,
            "normal_quit_receipt_observed": self.normal_stop_receipt is not None,
            "normal_process_exit": return_code == 0,
            "no_forced_termination": self.term_ns is None and self.kill_ns is None
            and not any("SIG" in row["action"] for row in self.shutdown_actions),
            "no_reported_boundary": not self.boundaries,
            "no_recorder_or_protocol_problem": not self.problems,
            "requested_ready_window_elapsed": self.stop_reason == "window_elapsed"
            and self.ready_ns is not None and self.stop_ns >= self.deadline_ns,
        }
        observation_complete = all(checks.values())
        summary = {
            "outcome": "bounded_window_observed" if observation_complete else "partial_or_failed_observation",
            "stop_reason": self.stop_reason, "recording_checks": checks,
            "launch_monotonic_ns": self.launch_ns, "ready_monotonic_ns": self.ready_ns,
            "target_deadline_monotonic_ns": self.deadline_ns, "stop_monotonic_ns": self.stop_ns,
            "finished_monotonic_ns": completed, "finished_utc": utc_now(),
            "capture_requests": self.capture_requests, "acknowledged_frames": self.capture_count,
            "first_capture": self.first_capture, "last_capture": self.last_capture,
            "pending_capture": self.pending, "last_complete_fleet": self.groups[-1] if self.groups else None,
            "partial_fleet_report": self.group, "per_slot_observed_growth": growth,
            "all_counter_headers": self.counter_headers,
            "boundaries": self.boundaries, "problems": self.problems,
            "shutdown_actions": self.shutdown_actions, "process_return_code": return_code,
            "normal_stop_receipt": self.normal_stop_receipt, "wav": wav,
            "lighting_policy": self.manifest["lighting_policy"],
            "native_gc_light_owner_observation": self.native_gc_lighting,
            "native_ta_light_owner_observation": self.native_ta_lighting,
            "ending_executable_identity": ending_executable,
            "limitations": [
                "This is a bounded " + self.args.track + " mode2 observation, not complete-track acceptance.",
                "The native_ticks fleet field is shared completedFrames, not an independent per-racer timer.",
                "Per-racer dispatched counts are reported separately and must be inspected alongside native_ticks.",
                "No final all-AI fleet row is emitted by quit; zero counters are verified only at logged snapshots.",
                "Ordinary counts are observations only; no ordinary-driving or lap acceptance is inferred.",
                "Frame timestamps measure host observations of complete log lines, not native GPU presentation time.",
                "WAV uses the simulation/mixer clock and may include unmatched prefix/suffix; no mux or stretching performed.",
                "live.ppm without a complete recorded three-part acknowledgment is untrusted partial output.",
                "TA lighting uses an explicit authored-zone top-two host spotlight scan; source light-volume query, cache and specular parity are not claimed.",
                "Native GC selected-light mode retains host normals/material/alpha/fog/specular limits; its startup line is owner-policy evidence, not original shader or pixel parity.",
            ],
        }
        json_write(self.output / "summary.json", summary)
        self.event("summary_written", observation_complete=observation_complete)
        print(json.dumps({"output": str(self.output), "outcome": summary["outcome"],
                          "stop_reason": self.stop_reason, "frames": self.capture_count}), flush=True)
        if observation_complete:
            return 0
        return 3 if self.boundaries or self.stop_reason == "source_boundary" else 2


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("executable", type=Path, help="New root-reviewed native ELF executable")
    parser.add_argument("--expected-exe-sha256", required=True)
    parser.add_argument("--output", required=True, type=Path, help="New direct child directory of staging")
    parser.add_argument("--root-reviewed-one-check", action="store_true",
                        help="Explicit local launch guard; does not grant authorization")
    parser.add_argument("--not-before", default=EARLIEST.isoformat(),
                        help="UTC launch gate, never earlier than 2026-10-10 15:18 UTC")
    parser.add_argument("--capture-hz", type=float, default=5.0)
    parser.add_argument("--track", choices=("GC", "TA"), default="GC")
    parser.add_argument("--native-ta-selected-lights", action="store_true",
                        help="Explicit native TA authored-zone spotlight scan and portal-color host lighting; requires --track TA and owner receipt")
    parser.add_argument("--native-gc-gate", action="store_true",
                        help="Opt into native swept collider/ authored Gate sphere contact; records the compatibility policy")
    parser.add_argument("--native-track-fog", action=argparse.BooleanOptionalAction, default=True,
                        help="Authored native track fog defaults on; --no-native-track-fog is a diagnostic opt-out")
    parser.add_argument("--native-gc-selected-lights", action="store_true",
                        help="Explicit native GC selected ambient/directional host-Lambert policy; records limits and requires its owner initialization before readiness")
    parser.add_argument("--ready-window-seconds", type=float, default=30.0,
                        help="Bounded observation after readiness, at most 180 seconds")
    parser.add_argument("--startup-timeout", type=float, default=180.0)
    parser.add_argument("--ack-timeout", type=float, default=5.0)
    parser.add_argument("--quit-timeout", type=float, default=10.0)
    parser.add_argument("--native-level-lights", action=argparse.BooleanOptionalAction, default=True,
                        help="Use authored native track lighting by default; --no-native-level-lights is a diagnostic opt-out")
    args = parser.parse_args()
    if not args.native_level_lights and (args.native_gc_selected_lights or args.native_ta_selected_lights):
        parser.error("Explicit lighting selection conflicts with --no-native-level-lights")
    if args.native_level_lights and not (args.native_gc_selected_lights or args.native_ta_selected_lights):
        args.native_gc_selected_lights = args.track == "GC"
        args.native_ta_selected_lights = args.track == "TA"
    if args.native_gc_selected_lights and args.native_ta_selected_lights:
        parser.error("Select only one native lighting policy")
    if args.native_gc_selected_lights and args.track != "GC":
        parser.error("GC lighting requires --track GC")
    if args.native_ta_selected_lights and args.track != "TA":
        parser.error("TA lighting requires --track TA")
    if args.native_gc_gate and args.track != "GC":
        parser.error("GC Gate policy requires --track GC")
    if not args.root_reviewed_one_check:
        parser.error("Review and explicit root launch permission are required; no process started")
    if not re.fullmatch(r"[0-9a-fA-F]{64}", args.expected_exe_sha256):
        parser.error("Expected executable identity must be a 64-digit SHA-256")
    for name, maximum in (("capture_hz", 5), ("ready_window_seconds", 180), ("startup_timeout", 300), ("ack_timeout", 10), ("quit_timeout", 15)):
        if not (0 < getattr(args, name) <= maximum):
            parser.error(name + " must be positive and no greater than " + str(maximum))
    gate = dt.datetime.fromisoformat(args.not_before.replace("Z", "+00:00"))
    if gate.tzinfo is None or gate < EARLIEST:
        parser.error("Launch gate must be timezone-aware and no earlier than the reserved-slot boundary")
    if dt.datetime.now(dt.timezone.utc) < gate:
        parser.error("Hourly slot is not available yet; no wait, retry, or process launch performed")
    executable = args.executable.resolve(strict=True)
    if not stat.S_ISREG(executable.stat().st_mode) or not os.access(executable, os.X_OK):
        parser.error("Executable must be an executable regular file")
    with executable.open("rb") as stream:
        if stream.read(4) != b"\x7fELF":
            parser.error("Only a native ELF executable is accepted; no wrappers")
    identity = file_identity(executable)
    if identity["sha256"] != args.expected_exe_sha256.lower():
        parser.error("Executable differs from root-reviewed SHA-256; no process started")
    output = args.output.absolute()
    if output.parent.resolve() != STAGING.resolve() or output.exists() or output.is_symlink():
        parser.error("Output must be a completely new direct child of staging")
    corpus_identity = file_identity(ASSETS / CORPUS)
    source_paths = SOURCE_PATHS + (GC_LIGHT_SOURCE_PATHS if (args.native_gc_selected_lights or args.native_ta_selected_lights) else ())
    source_identities = [file_identity(REPO / name) for name in source_paths]
    output.mkdir(mode=0o700, exist_ok=False)
    for name in ("frames", "home", "tmp", "cache"):
        (output / name).mkdir(mode=0o700)
    environment = {
        "PATH": "/usr/bin:/bin", "LANG": "C", "LC_ALL": "C", "TZ": "UTC",
        "HOME": str(output / "home"), "TMPDIR": str(output / "tmp"),
        "XDG_CACHE_HOME": str(output / "cache"),
        **RR_ENV, "RR_TRACK": args.track,
        "RR_GC_INCOMPLETE_WORLD": "1" if args.track == "GC" else "0",
        "RR_AUDIO_WAV": str(output / "audio.wav"),
        "RR_GC_NATIVE_GATE": "1" if args.native_gc_gate else "0",
        "RR_AUTHORED_WORLD_FOG": "1" if args.native_track_fog else "0",
        "RR_NATIVE_GC_SELECTED_LIGHTS": "1" if args.native_gc_selected_lights else "0",
        "RR_NATIVE_TA_SELECTED_LIGHTS": "1" if args.native_ta_selected_lights else "0",
    }
    manifest = {
        "prepared_candidate_validation": "Source-only; not executed or syntax-checked during authoring",
        "created_utc": utc_now(), "executable": identity,
        "recorder": file_identity(Path(__file__).resolve()),
        "current_checkout_source_identities": source_identities,
        "source_provenance_warning": "Current source hashes do not establish that the supplied executable was built from them",
        "corpus": corpus_identity, "cwd": str(ASSETS), "environment": environment,
        "environment_policy": "Constructed from the listed values only; no ambient RR, LD, GL, display, or wrapper variables inherited",
        "command": [str(executable), str(CORPUS), "--headless-live", str(output / "live.ppm")],
        "allowed_stdin_commands": ["capture", "quit"], "target_ready_window_seconds": args.ready_window_seconds,
        "expected_racer60_by_slot": list(EXPECTED_RACER60), "expected_camera_racer60": 1,
        "capture_hz_maximum": args.capture_hz, "startup_timeout_seconds": args.startup_timeout,
        "capture_ack_timeout_seconds": args.ack_timeout, "quit_timeout_seconds": args.quit_timeout,
        "termination_grace_seconds": 2, "kill_reap_grace_seconds": 2,
        "not_before": gate.isoformat(), "single_process_attempt": True,
        "gc_gate_contact_policy": "native_swept_type2_segments" if args.native_gc_gate else "disabled",
        "track_fog_policy": "reconstructed_native_track_fog" if args.native_track_fog else "disabled",
        "lighting_policy": "nativeTaAuthoredZoneSpotSelection" if args.native_ta_selected_lights else
        "nativeGcSelectedLights" if args.native_gc_selected_lights else "unchangedHost",
        "lighting_scope": "Authenticated TA authored colors, genuine racer visibility/pose, native zone scan/top-two spots and portal blend; host material/normal limits, no retail light query/cache parity" if args.native_ta_selected_lights else
        "Actual registered GC colors/directions and racer visibility, native per-publication reevaluation and existing host normals/material/alpha/fog; no full original shader/cache/specular parity",
    }
    json_write(output / "launch.json", manifest)
    return Recorder(args, output, manifest).run()


if __name__ == "__main__":
    sys.exit(main())
