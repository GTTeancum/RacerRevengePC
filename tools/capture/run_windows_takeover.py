"""Stage a verified build and exercise its existing in-process UI/AI route.

No host input or desktop capture. Images come from the game's framebuffer.
The receipt is execution evidence only; images/audio still need inspection.
"""
import argparse
import ctypes
import hashlib
import json
import os
import re
from pathlib import Path
import shutil
import struct
import subprocess
import time
import zlib

ROOT = Path(__file__).resolve().parents[2]
# A git worktree under <repo>\.claude\worktrees\<name> runs against the main
# repository's build, evidence and scratch folders (one shared tree of outputs).
if ROOT.parent.name == 'worktrees' and ROOT.parent.parent.name == '.claude':
    ROOT = ROOT.parents[2]

# The 23 original racer ids (no pods 5, 6, 13).
ORIGINAL_RACERS = [r for r in range(1, 27) if r not in (5, 6, 13)]
# Common rewrite B-C21 (writer R11): ONE free-play route for every course and pod.
# main-rider-course: Main -> Rider (the racer the profile remembers for free play:
# --player-racer seeds it, else the fresh Rider19) -> Course, where --course picks the course
# on the Course screen with the original course arrow, --laps sets the original Duration row and
# --course-mode the event type (tests/native/original_ui_capture_navigation50la.h Route).
# main-rider-course-ta is the same route under its old name. The per-course routes
# (main-rider-course-sa, main-gasgano07-course-sa, main-mawhonic09-course-ra, main-dud04/ben02-course-ga,
# main-alder10/ody11-course-ma) and their game-side test profiles are retired.
FREE_PLAY_ROUTES = ('main-rider-course', 'main-rider-course-ta')
PLAYER_RACER_ROUTES = FREE_PLAY_ROUTES + ('main-tournament-course-ta',)
ADDON_RIDER_ROUTES = FREE_PLAY_ROUTES
# DLC phase 3 (146c, PORT EXTENSION, NOT ORIGINAL): add-on course item key and the free-play route it rides.
ADDON_COURSE_KEY = re.compile(r'[a-z0-9][a-z0-9-]{2,47}/[a-z0-9][a-z0-9-]{1,46}')
ADDON_COURSE_ROUTES = FREE_PLAY_ROUTES
# A course code as data: a stock code (TA..SC) or an add-on course code (nv, ...).
COURSE_CODE = re.compile(r'[A-Za-z0-9]{2}')


# Tournament course order (career progress219C) -> course code.
TOURNAMENT_ORDER = ('TA', 'SA', 'MA', 'GA', 'RA', 'TB', 'MB', 'GB', 'SC', 'MC', 'GC', 'RC', 'TC')


def seeded_tournament_track(data):
    """Course code the seed sends a tournament run to: the Tournament-remembered
    racer's (6FC) own career progress. None if the file is not readable."""
    try:
        u32 = lambda at: struct.unpack_from('<I', data, at)[0]
        if u32(0) != 0x42505252 or u32(4) != 1:
            return None
        unlock = 16 + 12
        remembered = unlock + 27 * 4 * 2
        careers = remembered + 4 * 4 + 7 * 4
        career_size = 16 + 27 * (6 + 13 + 13) * 4
        racer = u32(remembered + 4 * 3)
        progress = u32(careers + racer * career_size + 12)
        return TOURNAMENT_ORDER[progress] if 0 <= progress < len(TOURNAMENT_ORDER) else None
    except (struct.error, IndexError):
        return None


def seed_player_racer(data, racer):
    """Return progress-v1.bin bytes in which `racer` is unlocked, remembered
    for Free Play (6F0) and Tournament (6FC), and has reached the same
    tournament course as the seed's Anakin19 career (progress byte and that
    career's own diagonal medals). Layout: saveNativeBetaProgress in
    platform/linux/native/original_ui_rider_profile50kg.cpp. Only fields the
    game's own unlock grant and Rider Confirm write are touched (unlock10FAC,
    remembered); hidden0730 stays as the game left it. The physics identity
    word is kept, so the seed must be a file the game itself wrote."""
    d = bytearray(data)
    u32 = lambda at: struct.unpack_from('<I', d, at)[0]
    put = lambda at, v: struct.pack_into('<I', d, at, v)
    if u32(0) != 0x42505252 or u32(4) != 1 or u32(8) != len(d) - 16 or \
            u32(12) != zlib.crc32(bytes(d[16:])) & 0xffffffff:
        raise ValueError('Seed save is not an intact progress-v1 file')
    unlock = 16 + 12
    remembered = unlock + 27 * 4 * 2
    careers = remembered + 4 * 4 + 7 * 4
    pod_size = (6 + 13 + 13) * 4
    career_size = 16 + 27 * pod_size
    career = lambda c: careers + c * career_size
    medal = lambda c, p, o: career(c) + 16 + p * pod_size + 24 + o * 4
    put(unlock + 4 * racer, 1)
    put(remembered + 4 * 0, racer)  # 6F0, mode0 Free Play
    put(remembered + 4 * 3, racer)  # 6FC, mode2 Tournament
    if racer != 19:
        put(career(racer) + 12, u32(career(19) + 12))  # progress219C
        for o in range(13):
            put(medal(racer, racer, o), u32(medal(19, 19, o)))
    put(8, len(d) - 16)
    put(12, zlib.crc32(bytes(d[16:])) & 0xffffffff)
    return bytes(d)


def seed_free_play(data):
    """Return progress-v1.bin bytes for the free-play route on any course: course
    availability 11018 (second header word, saveNativeBetaProgress in
    platform/linux/native/original_ui_rider_profile50kg.cpp) = all 13 stock courses, the
    value the game itself writes once a career has opened every course (high bits kept), and
    Main remembered slot 6E0 = 1 (the route's entry). The CRC and size words are rewritten;
    anything off-layout is refused. Which courses the Course screen offers stays the game's
    own rule over this data."""
    d = bytearray(data)
    u32 = lambda at: struct.unpack_from('<I', d, at)[0]
    if len(d) < 28 or u32(0) != 0x42505252 or u32(4) != 1 or u32(8) != len(d) - 16 or \
            u32(12) != zlib.crc32(bytes(d[16:])) & 0xffffffff:
        raise ValueError('Seed save is not an intact progress-v1 file')
    struct.pack_into('<I', d, 16 + 4, (u32(16 + 4) & 0xe0) | 13)   # availability11018
    struct.pack_into('<I', d, 16 + 8, 1)                            # savedSlot6E0
    struct.pack_into('<I', d, 8, len(d) - 16)
    struct.pack_into('<I', d, 12, zlib.crc32(bytes(d[16:])) & 0xffffffff)
    return bytes(d)


def seed_dlc_hall(pod_key):
    """DLC save side file (schema rrpc-dlc-progress/1, the layout DlcProgress50DL
    ::serialise writes, src/assets/dlc_progress50dl.cpp) holding one add-on pod
    record on stock TA, faster than any stock row: lap 1.00 s, race 5.00 s.
    Empty slots are null; knockouts stay empty. The stock save is not touched."""
    entry = lambda t: {'time': t, 'name': 'EYECHECK', 'racer': pod_key}
    return {'schema': 'rrpc-dlc-progress/1', 'courses': {},
            'stock_course_addon_pod_records': {'TA': {'lap': [entry(1.0), None, None],
                                                      'race': [entry(5.0), None, None],
                                                      'knockouts': [None, None, None]}},
            'pods': {pod_key: {'unlocked': True}}, 'careers': {}}


# Course ids 0..12 (original course descriptors / Hall row order).
HALL_COURSES = ('TA', 'TB', 'TC', 'RC', 'MA', 'MB', 'MC', 'RA', 'GA', 'GB', 'GC', 'SA', 'SC')
SLOW_HALL_SECONDS = 3599.99  # shown by 189000 as 59:59.99


def seed_slow_hall(data, code, free_play_main):
    """Return progress-v1.bin bytes in which course `code`'s Hall rows are all
    slow (lap and race 59:59.99, knockouts 0) so any finished time qualifies
    under the unchanged 1ABFC0/1AC440 rules. Layout = the order
    saveNativeBetaProgress writes (platform/linux/native/
    original_ui_rider_profile50kg.cpp): 16-byte RRPB header, 3 words, unlock
    27, hidden 27, remembered 4, remembered course 7, 27 careers of
    (4 words + 27 pods * 32 words), 29 names * 21 words, then 13 Hall courses
    of (3 lap floats, 3 race floats, 3 knockout words, 9 names * 21 words).
    Only those 9 numeric words change (names kept). free_play_main also sets
    the Main remembered slot 6E0 to 1 (the free-play route's fresh entry).
    The CRC and size words are rewritten; anything off-layout is refused."""
    d = bytearray(data)
    u32 = lambda at: struct.unpack_from('<I', d, at)[0]
    if len(d) < 16 or u32(0) != 0x42505252 or u32(4) != 1 or u32(8) != len(d) - 16 or \
            u32(12) != zlib.crc32(bytes(d[16:])) & 0xffffffff:
        raise ValueError('Seed save is not an intact progress-v1 file')
    words = 3 + 27 + 27 + 4 + 7 + 27 * (4 + 27 * 32) + 29 * 21
    hall = 16 + 4 * words
    course_bytes = 4 * (3 + 3 + 3 + 9 * 21)
    if hall + 13 * course_bytes != len(d):
        raise ValueError(f'Seed save size {len(d)} does not match the RRPB layout ({hall + 13 * course_bytes})')
    course = HALL_COURSES.index(code)
    at = hall + course * course_bytes
    if not (16 <= at and at + 36 <= len(d)):
        raise ValueError('Hall row offset outside the seed')
    struct.pack_into('<6f', d, at, *([SLOW_HALL_SECONDS] * 6))
    struct.pack_into('<3I', d, at + 24, 0, 0, 0)
    if free_play_main:
        struct.pack_into('<I', d, 16 + 8, 1)  # savedSlot6E0 (third header word)
    struct.pack_into('<I', d, 8, len(d) - 16)
    struct.pack_into('<I', d, 12, zlib.crc32(bytes(d[16:])) & 0xffffffff)
    return bytes(d)

def seed_slow_dlc_course_hall(key, dlc_root):
    """DLC save side file (schema rrpc-dlc-progress/2, the layout DlcProgress50DL
    ::serialise writes, src/assets/dlc_progress50dl.cpp) holding the add-on
    course `key` (<package-id>/<item-id>) with all-slow Hall rows (lap and race
    59:59.99, name SLOW, knockouts empty), so any finished time on that course
    qualifies under the unchanged 1ABFC0/1AC440 rules. records_key and the
    archive SHA-256 come from the package's own dlc.json under dlc_root, so the
    entry matches the item (an unknown package or item is refused). Only the
    side file is written; the stock save is not touched. PORT EXTENSION."""
    package, item_id = key.split('/', 1)
    manifest = Path(dlc_root) / package / 'dlc.json'
    if not manifest.is_file():
        raise ValueError(f'--seed-slow-dlc-hall: no package {package} in {dlc_root}')
    data = json.loads(manifest.read_text(encoding='utf-8'))
    item = next((i for i in data.get('items', []) if i.get('kind') == 'course' and i.get('id') == item_id), None)
    if item is None:
        raise ValueError(f'--seed-slow-dlc-hall: package {package} has no course item {item_id}')
    archive = next((f for f in data.get('files', []) if f.get('path') == item.get('archive')), None)
    sha = (archive or {}).get('sha256', '')
    if not re.fullmatch(r'[0-9a-f]{64}', sha):
        raise ValueError(f'--seed-slow-dlc-hall: no SHA-256 for {item.get("archive")} in {manifest}')
    slow = lambda: {'time': SLOW_HALL_SECONDS, 'name': 'SLOW'}
    return {'schema': 'rrpc-dlc-progress/2',
            'courses': {key: {'records_key': item.get('records_key', item_id), 'archive_sha256': sha,
                              'hall': {'lap': [slow(), slow(), slow()], 'race': [slow(), slow(), slow()],
                                       'knockouts': [None, None, None]}}},
            'stock_course_addon_pod_records': {}, 'pods': {}, 'careers': {}}


def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def inside(path):
    path = path.resolve()
    if not path.is_relative_to(ROOT):
        raise ValueError(f'Project output must remain in repository: {path}')
    return path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--assets', type=Path, required=True)
    parser.add_argument('--sdl', type=Path, required=True)
    parser.add_argument('--route', choices=['main-rider-course', 'main-rider-course-ta',
                        'main-tournament-course-ta', 'none'], default='main-rider-course')
    parser.add_argument('--logs-only', action='store_true', default=True, help='Record stage and timing receipts without pixel readback or image files')
    parser.add_argument('--seconds', type=int, default=180)
    parser.add_argument('--profile-performance', action='store_true',
                        help='Enable existing CPU/render/display phase measurements')
    parser.add_argument('--profile-gpu-stages', action='store_true',
                        help='Serialize rendering stages for diagnosis; not production FPS measurement')
    parser.add_argument('--camera-trace', action='store_true',
                        help='Log actual attached-camera poses without changing camera behavior')
    parser.add_argument('--restart-once', action='store_true',
                        help='Exercise native race teardown/rebuild after first race evidence; not Pause-menu input')
    parser.add_argument('--renderer-switch', action='append', default=[],
                        choices=['RR_EXACT_PS2_SPECULAR', 'RR_GPU_FINISH_EACH_FRAME', 'RR_ORIGINAL_ASPECT',
                                 'RR_TRACE_ORIGINAL_UI_STAGES', 'RR_TRACE_PHASES',
                                 'RR_GARAGE_PROOF_SHADOW', 'RR_GARAGE_KERNEL_SHADOW',
                                 'RR_GARAGE_LEGACY_INDEX', 'RR_GARAGE_LEGACY_FINISH',
                                 'RR_GARAGE_LEGACY_PARALLEL', 'RR_GARAGE_LEGACY_BLEND',
                                 'RR_GARAGE_COVERAGE_PER_TRIANGLE', 'RR_GARAGE_COVERAGE_VERIFY',
                                 'RR_CPU_POD_VERTICES', 'RR_TEST_PAIR_ROUTE_COMPARE', 'RR_COVERAGE_PAIR_EXACT', 'RR_GPU_POD_VERTEX_CHECK',
                                'RR_COLLISION_ADMISSION_CHECK',
                                 'RR_SHADOW_TRACE'],
                        help='Set one renderer comparison switch to 1 (repeatable); recorded in execution.json')
    parser.add_argument('--track', default='TA', help='Direct-start track when --route none')
    parser.add_argument('--all-ai', action='store_true',
                        help='Original AI drives every pod, camera on pod 0 (spectator)')
    parser.add_argument('--race-full', action='store_true',
                        help='Keep racing until the watchdog budget instead of stopping after FPS evidence')
    parser.add_argument('--steer-invert', action='store_true',
                        help='Invert the test steering toward the next gate')
    parser.add_argument('--input-device', choices=['keyboard', 'controller'],
                        help='Force the human input device (controller uses the real connected pad)')
    parser.add_argument('--pad-hold-a', action='store_true',
                        help='Hold pad A through the green light (focused window), release 2 s after green')
    parser.add_argument('--stop-start', action='store_true',
                        help='With --hold-throttle: 4 s throttle / 3 s coast, repeating')
    parser.add_argument('--maximized', action='store_true',
                        help='Open the (resizable, --window) test window maximized')
    parser.add_argument('--no-fxaa', action='store_true', help='Turn FXAA off for this run')
    parser.add_argument('--destroy', help='Destroy racers during the race: slot@seconds[,slot@seconds] (0 = player)')
    parser.add_argument('--env', action='append', default=[],
                        help='Test-only: pass RR_NAME=value to the game (repeatable; other RR_ variables are not inherited)')
    parser.add_argument('--undertrack', help='Test-only: drop racers below the track: slot@seconds[,slot@seconds] (0 = player)')
    parser.add_argument('--camera-cycle', action='store_true',
                        help='Tap the view key (C) 1 s after green and every 15 s (reaches the follow camera)')
    parser.add_argument('--results-confirm', type=float, default=0.0,
                        help='Tap Enter every N seconds once the Results replay is running (0 = off)')
    parser.add_argument('--results-exit', choices=['main'],
                        help='Test only: on the native free-play Results panel (the one a single race ends on; '
                             'not the tournament Results replay), choose Main Menu and confirm it, as a player '
                             'would (Down, Enter, Down, Enter, 1 s apart; sets RR_TEST_RESULTS_EXIT). The race '
                             'returns to the original Main and the run then idles at Main until --seconds')
    # PC-NATIVE ADDITION, NOT ORIGINAL: the Main menu EXIT GAME row.
    parser.add_argument('--main-exit', choices=['fresh', 'return'],
                        help='Test only (PC-native EXIT GAME row): on Main, press Up until EXIT GAME is '
                             'highlighted, Enter to open its dialog, Enter to confirm, one key per settled '
                             'frame as a player would (sets RR_TEST_MAIN_EXIT). "fresh": the first Main, no '
                             'race. "return": the frontend after a race (use with --results-exit main); Back '
                             'is pressed on Rider/Course/Hall/Options until Main. The run passes when the game '
                             'closed itself cleanly: native_exit_code=0 and a plain SESSION END')
    parser.add_argument('--hold-results', action='store_true',
                        help='Test only: stay on the Results screen the race ends on (native free-play panel or '
                             'the tournament Results replay) and write ONE frame of it as an image 3 s after it '
                             'appears (sets RR_TEST_RESULTS_HOLD=1; turns logs-only off for this run). The run then '
                             'holds there until --seconds. Not with --results-exit or --results-confirm')
    parser.add_argument('--hold-results-hall', action='store_true',
                        help='Test only: when the race returns to the Hall of Fame (the 1AB5BC record row, after '
                             '--results-exit main), write the settled Hall frame once as '
                             'capture/frontend-session-N/hall-M.ppm (sets RR_TEST_RESULTS_HALL_CAPTURE=1; turns '
                             'logs-only off for this run). The run then holds on the Hall until --seconds')
    parser.add_argument('--ieee-subnormals', action='store_true',
                        help='Reproduction only: disable the PS2 no-subnormal float mode')
    parser.add_argument('--window', help='Open a resizable window at WIDTHxHEIGHT (widescreen tests)')
    parser.add_argument('--hold-throttle', action='store_true',
                        help='Hold the human keyboard throttle once the race is green (launch test)')
    parser.add_argument('--garage-close-frame', type=int, default=-1,
                        help='Test only: close the window at native frame N of a race-return Garage (0 = before its first frame)')
    parser.add_argument('--repair', action='store_true',
                        help='With --hold-throttle: hold the repair key (R) 2 s of every 6 s')
    parser.add_argument('--unstick', action='store_true',
                        help='With --hold-throttle: tap the manual reset key (T) when the pod is wedged for 3 s')
    parser.add_argument('--seed-slow-hall', metavar='CODE', type=str.upper,
                        help='Test only: with --seed-save, make course CODE\'s Hall rows all slow (lap/race '
                             '59:59.99, knockouts 0) so the unattended driver\'s time is a record (unchanged '
                             'original rules). For the free-play route also sets the Main slot to 1')
    parser.add_argument('--seed-slow-dlc-hall', metavar='PACKAGE/ITEM',
                        help='Test only (DLC add-on course records, PORT EXTENSION): write saves\\dlc-progress-v1.json '
                             'with all-slow Hall rows (lap/race 59:59.99, name SLOW) for this add-on course, so the '
                             'unattended driver\'s time on it is a record. Only the DLC side file is written. Needs '
                             '--env RR_DLC_ROOT=<DLC folder>; use with --addon-course <same key> --results-exit main '
                             '[--hold-results-hall]')
    parser.add_argument('--pause-quit', type=float, default=0.0, metavar='SECONDS',
                        help='Test only: after SECONDS of racing press Start (Escape), step Down to the original '
                             'Pause QUIT row (row 10) and press Cross (Enter), one key per poll, as a player would '
                             '(sets RR_TEST_PAUSE_QUIT). Free Play: the original 1BDA74 row '
                             '(Course; a Time Trial asks its record first)')
    parser.add_argument('--pause-restart', type=float, default=0.0, metavar='SECONDS',
                        help='Test only: as --pause-quit, but the RESTART row (row 1, 1BD990) '
                             '(sets RR_TEST_PAUSE_RESTART)')
    parser.add_argument('--seed-save', type=Path,
                        help='Test only: copy this prepared native progress file (progress-v1.bin format) into the run save directory before launch')
    parser.add_argument('--player-racer', type=int, choices=ORIGINAL_RACERS,
                        help='Test only: unlock and remember this original racer in the seeded save '
                             '(requires --seed-save; main-tournament-course-ta only). The game '
                             'loads it through its own Rider screen; the route only checks it opened on it')
    # PORT EXTENSION, NOT ORIGINAL (DLC phase 2): add-on riders and courses.
    parser.add_argument('--addon-rider', metavar='PACKAGE/ITEM',
                        help='Test only (DLC): on --route main-rider-course-ta, press Rider Next until this '
                             'admitted add-on rider is highlighted, then Confirm (sets RR_TEST_ADDON_RIDER). '
                             'Needs the package in the DLC root, e.g. --env RR_DLC_ROOT=<folder>\\DLC')
    parser.add_argument('--eyecheck-dlc-hall', metavar='PACKAGE/ITEM',
                        help='Eye check (DLC Hall merge): seed saves\\dlc-progress-v1.json with a TA record of this '
                             'admitted add-on pod (lap 1.00 s, race 5.00 s, name EYECHECK; faster than any stock TA '
                             'row), then drive Main -> Options -> Hall of Fame (Options slot 4) -> TA in process '
                             '(route main-options-hall-ta, no manual input), write that Hall frame as an image '
                             '(logs-only off for this run) and hold there until --seconds runs out (use 90). '
                             'Needs --env RR_DLC_ROOT=<folder with the package>. Name the output eyecheck-dlc-hall-...')
    parser.add_argument('--hold-rider', action='store_true',
                        help='Eye check: stop on the Rider screen (no Confirm) and write that Rider frame as an image '
                             '(turns logs-only off for this run). With --addon-rider it holds with that add-on '
                             'highlighted (RR_TEST_ADDON_RIDER_HOLD=1); without it, on the stock rider the route lands '
                             'on (RR_TEST_RIDER_HOLD=1; --route main-rider-course-ta lands on Rider19). The run then '
                             'idles until its wall budget; use a short --seconds (90)')
    parser.add_argument('--addon-course', metavar='PACKAGE/ITEM',
                        help='Test only (DLC add-on course, PORT EXTENSION): on the free-play Course screen press the '
                             'course arrow (Next) until this admitted add-on course is highlighted, then the original '
                             'Course Confirm (sets RR_TEST_ADDON_COURSE_PICK). Needs --env RR_DLC_ROOT=<DLC folder> and --route '
                             + ' / '.join(ADDON_COURSE_ROUTES))
    parser.add_argument('--course-mode', '--addon-course-mode', dest='course_mode', choices=['single', 'time_trial'],
                        help='Free-play Course (--route main-rider-course-ta, stock TA or with --addon-course): first set '
                             'the event type (original Down to the event-type row, Right until Single Race / Time Trial '
                             'shows, Up back to the course row), then pick the course (sets RR_TEST_COURSE_SUBTYPE=1 or 3). '
                             'Default: the event type the Course screen opens on (Single Race on a fresh profile)')
    parser.add_argument('--hold-course', action='store_true',
                        help='With --addon-course: stop on the Course screen with the add-on course highlighted (no Confirm, '
                             'sets RR_TEST_ADDON_COURSE_HOLD=1) and write that Course frame as an image for an eye check '
                             '(turns logs-only off for this run). The run then idles until its wall budget; use a short '
                             '--seconds (60)')
    # Common rewrite round (R11, test only): one free-play route, course picked on the Course screen.
    parser.add_argument('--course', metavar='CODE',
                        help='Test only: on the free-play Course screen press the original course arrow until '
                             'this course (a stock code such as GA, or an add-on course code such as nv) is '
                             'highlighted, then Confirm (sets RR_TEST_COURSE). Needs --seed-save (the run seeds '
                             'course availability 13 and Main slot 1 into it); add-on codes need --env RR_DLC_ROOT')
    parser.add_argument('--laps', type=int, metavar='N',
                        help='Test only: set the Course screen Duration (laps) row to N (1..25) through the '
                             'original Left/Right laps move before Confirm (sets RR_TEST_COURSE_LAPS). Free play '
                             'only: the tournament keeps its original 3 laps')
    parser.add_argument('--return-hold', type=int, default=0, metavar='SECONDS',
                        help='Test only: once a race has returned to a frontend screen (Results then Hall, Course '
                             'or the tournament screens: frontend_race_return), end the run SECONDS later through '
                             'the game\'s own clean wall-budget exit (sets RR_TEST_RETURN_HOLD_SECONDS)')
    parser.add_argument('--gpu-vertex-check', type=int, default=0,
                        help='Diagnostic: compare GPU pod vertices with the CPU reference every N rendered frames')
    parser.add_argument('--keep-binaries', action='store_true',
                        help='Keep the staged program/DLL copies and audio recording after a logs-only run '
                             '(by default they are deleted when the run ends; logs are always kept)')
    args = parser.parse_args()
    if os.name != 'nt' or not 10 <= args.seconds <= 1800:
        parser.error('Windows required; watchdog must be 10..1800 seconds (chained-course runs need up to 30 min)')
    if args.player_racer is not None:
        if not args.seed_save:
            parser.error('--player-racer needs --seed-save: a progress file the game itself wrote')
        if args.route not in PLAYER_RACER_ROUTES:
            parser.error('--player-racer works only with --route ' + ' / '.join(PLAYER_RACER_ROUTES))
    addon_key = re.compile(r'[a-z0-9][a-z0-9-]{1,46}/[a-z0-9][a-z0-9-]{1,46}')
    if args.addon_rider is not None:
        if not addon_key.fullmatch(args.addon_rider):
            parser.error('--addon-rider expects <package-id>/<item-id>, e.g. arcade-ark-roose/a2r-ark')
        if args.route not in ADDON_RIDER_ROUTES:
            parser.error('--addon-rider works only with --route ' + ' / '.join(ADDON_RIDER_ROUTES))
        if args.player_racer is not None:
            parser.error('--addon-rider and --player-racer cannot be combined')
    if args.hold_course and args.addon_course is None:
        parser.error('--hold-course needs --addon-course')
    if args.course_mode and args.route not in FREE_PLAY_ROUTES:
        parser.error('--course-mode works only with --route ' + ' / '.join(FREE_PLAY_ROUTES))
    if args.course is not None:
        if not COURSE_CODE.fullmatch(args.course):
            parser.error('--course expects a two-character course code (TA, GA, nv, ...)')
        if args.route not in FREE_PLAY_ROUTES:
            parser.error('--course works only with --route ' + ' / '.join(FREE_PLAY_ROUTES))
        if args.addon_course is not None:
            parser.error('--course and --addon-course both pick the course; use one')
        if not args.seed_save:
            parser.error('--course needs --seed-save: a progress file the game itself wrote')
    if args.laps is not None:
        if not 1 <= args.laps <= 25:
            parser.error('--laps must be 1..25 (the original Duration range)')
        if args.route not in FREE_PLAY_ROUTES:
            parser.error('--laps works only on the free-play route; the tournament keeps its original 3 laps')
    if not 0 <= args.return_hold <= 600:
        parser.error('--return-hold must be 0..600 seconds')
    if args.hold_course and args.hold_rider:
        parser.error('--hold-course and --hold-rider cannot be combined (each stops its own screen)')
    if args.hold_rider and args.route not in ADDON_RIDER_ROUTES:
        parser.error('--hold-rider works only with --route ' + ' / '.join(ADDON_RIDER_ROUTES))
    if args.hold_results and (args.results_exit or args.results_confirm):
        parser.error('--hold-results holds the Results screen; it cannot be combined with --results-exit or --results-confirm')
    if args.main_exit == 'return' and not args.results_exit:
        parser.error('--main-exit return needs --results-exit main (the race must return to the frontend)')
    if args.main_exit and (args.hold_results or args.hold_results_hall or args.hold_rider or args.hold_course or
                           args.eyecheck_dlc_hall is not None or args.restart_once):
        parser.error('--main-exit ends the run from Main; it cannot be combined with a hold, eye check or --restart-once')
    if args.eyecheck_dlc_hall is not None:
        if not addon_key.fullmatch(args.eyecheck_dlc_hall):
            parser.error('--eyecheck-dlc-hall expects <package-id>/<item-id>, e.g. arcade-ark-roose/a2r-ark')
        if args.addon_rider is not None or args.player_racer is not None:
            parser.error('--eyecheck-dlc-hall runs its own Main/Options/Hall route; it cannot be combined with other test routes')
        args.route = 'main-options-hall-ta'   # in-process HallRoute (no manual input)
        if not any(item.startswith('RR_DLC_ROOT=') for item in args.env):
            parser.error('--eyecheck-dlc-hall needs --env RR_DLC_ROOT=<folder with the package>')
    if args.seed_slow_dlc_hall is not None:
        if not ADDON_COURSE_KEY.fullmatch(args.seed_slow_dlc_hall):
            parser.error('--seed-slow-dlc-hall expects <package-id>/<item-id>, for example n64-vengeance/nv')
        if args.eyecheck_dlc_hall is not None:
            parser.error('--seed-slow-dlc-hall and --eyecheck-dlc-hall both write the DLC side file; use one')
        if not any(item.startswith('RR_DLC_ROOT=') and len(item) > len('RR_DLC_ROOT=') for item in args.env):
            parser.error('--seed-slow-dlc-hall needs --env RR_DLC_ROOT=<folder holding the DLC packages>')
    if args.addon_course is not None:
        if not ADDON_COURSE_KEY.fullmatch(args.addon_course):
            parser.error('--addon-course expects <package-id>/<item-id>, for example n64-vengeance/nv')
        if args.route not in ADDON_COURSE_ROUTES:
            parser.error('--addon-course works only with --route ' + ' / '.join(ADDON_COURSE_ROUTES))
        if not any(item.startswith('RR_DLC_ROOT=') and len(item) > len('RR_DLC_ROOT=') for item in args.env):
            parser.error('--addon-course needs --env RR_DLC_ROOT=<folder holding the DLC packages>')
    build, output = inside(args.build), inside(args.output)
    assets, sdl = args.assets.resolve(), args.sdl.resolve()
    receipt = json.loads((build / 'build-receipt.json').read_text(encoding='utf-8-sig'))
    if receipt['status'] != 'built-not-run':
        raise ValueError('A successful complete build receipt is required')
    executable = inside(Path(receipt['executable']))
    if digest(executable).lower() != receipt['executableSha256'].lower():
        raise ValueError('Executable differs from the build receipt')
    for name in ['SLUS_202.68', 'APPSTART.RES', 'LOC.RES']:
        if not (assets / name).is_file():
            raise ValueError(f'Missing original asset: {name}')
    runtime = [sdl, assets / 'avcodec-61.dll', assets / 'avutil-59.dll',
               assets / 'libwinpthread-1.dll']
    if sdl.name != 'SDL2.dll' or not all(p.is_file() for p in runtime):
        raise ValueError('SDL2 and the original installation codec DLLs are required')
    output.mkdir(parents=True, exist_ok=False)
    capture = output / 'capture'
    capture.mkdir()
    temporary = output / 'temp'
    temporary.mkdir()
    saves = output / 'saves'
    saves.mkdir()
    if args.seed_slow_hall is not None and (not args.seed_save or args.seed_slow_hall not in HALL_COURSES):
        raise ValueError('--seed-slow-hall needs --seed-save and a course code ' + '/'.join(HALL_COURSES))
    if args.seed_save:
        seed = args.seed_save.resolve()
        if not seed.is_file():
            raise ValueError(f'Missing seed save: {seed}')
        if args.player_racer is None:
            shutil.copy2(seed, saves / 'progress-v1.bin')
        else:
            (saves / 'progress-v1.bin').write_bytes(seed_player_racer(seed.read_bytes(), args.player_racer))
        if args.route in FREE_PLAY_ROUTES:  # the free-play route enters on Main slot 1, any course
            target = saves / 'progress-v1.bin'
            target.write_bytes(seed_free_play(target.read_bytes()))
            print('test_seed free_play availability=13 main_slot=1', flush=True)
        if args.seed_slow_hall is not None:
            target = saves / 'progress-v1.bin'
            target.write_bytes(seed_slow_hall(target.read_bytes(), args.seed_slow_hall,
                                              args.route in FREE_PLAY_ROUTES))  # free-play routes need Main slot 1
            print(f'test_seed hall={args.seed_slow_hall} slow=1', flush=True)
    if args.seed_slow_dlc_hall is not None:
        dlc_root = next(item.split('=', 1)[1] for item in args.env if item.startswith('RR_DLC_ROOT='))
        (saves / 'dlc-progress-v1.json').write_text(
            json.dumps(seed_slow_dlc_course_hall(args.seed_slow_dlc_hall, dlc_root), indent=2) + '\n')
        print(f'test_seed dlc_hall={args.seed_slow_dlc_hall} slow=1', flush=True)
    if args.eyecheck_dlc_hall is not None:
        (saves / 'dlc-progress-v1.json').write_text(json.dumps(seed_dlc_hall(args.eyecheck_dlc_hall), indent=2) + '\n')
    staged = output / 'RacerRevengePC.exe'
    shutil.copy2(executable, staged)
    for dll in runtime:
        shutil.copy2(dll, output / dll.name)
    env = {k: v for k, v in os.environ.items()
           if not k.upper().startswith(('RR_', 'SDL_', 'MESA_', 'LIBGL_'))}
    env.update(TEMP=str(temporary), TMP=str(temporary),
               RR_NATIVE_SAVE_DIR=str(saves), RR_AUDIO_WAV=str(output / 'audio.wav'),
               RR_WINDOW_CAPTURE_DIR=str(capture), RR_WINDOW_CAPTURE_AUTO='1',
               RR_UNATTENDED_CAPTURE='1', RR_UNATTENDED_UI_ROUTE=args.route,
               RR_UNATTENDED_SECONDS=str(max(1, args.seconds - 5)),
               RR_ORIGINAL_AI_ALL_RACERS='0', RR_TRACK_FPS_BASELINE='1',
               RR_NATIVE_FIXED_WINDOW='1',
               RR_TEST_MUTE_OUTPUT='1',  # Tests are silent; player builds are not.
               RR_VIDEO_MAXIMIZED='0')  # Never cover the desktop unless asked.
    if args.eyecheck_dlc_hall is not None:
        # In-process route Main -> Options -> Hall TA, then hold (PORT EXTENSION,
        # tests/native/original_ui_capture_navigation50la.h HallRoute).
        env['RR_UNATTENDED_UI_ROUTE'] = 'main-options-hall-ta'
    # A --hold-rider / --hold-course / --eyecheck-dlc-hall run writes its held frame as an image (eye check).
    logs_only = (args.logs_only and not args.hold_rider and not args.hold_course and not args.hold_results and
                 not args.hold_results_hall and args.eyecheck_dlc_hall is None and
                 not ((args.pause_quit or args.pause_restart) and '--logs-only' not in __import__('sys').argv))
    if logs_only:
        env['RR_EVIDENCE_LOGS_ONLY'] = '1'
    if args.camera_trace:
        env['RR_CAMERA_POSE_TRACE'] = '1'
    # Test-only (PC port, not original): every injected input below is
    # in-process (RR_TEST_* read by the game), so the run must not depend on
    # the game window being in front. RR_TEST_IGNORE_FOCUS stops focus loss
    # from pausing or releasing those inputs; a real minimise/close still does.
    injected = (args.hold_throttle or args.steer_invert or args.camera_cycle or args.results_confirm or args.results_exit or args.hold_results or
                args.destroy or args.undertrack or getattr(args, 'pad_hold_a', False) or args.player_racer is not None or args.pause_quit or args.pause_restart or
                args.main_exit)
    if injected:
        env['RR_TEST_IGNORE_FOCUS'] = '1'
    if args.pause_quit:
        env['RR_TEST_PAUSE_QUIT'] = str(args.pause_quit)
    elif args.pause_restart:
        env['RR_TEST_PAUSE_RESTART'] = str(args.pause_restart)
    if args.hold_throttle:
        env['RR_TEST_HOLD_THROTTLE'] = '1'
    if args.repair:
        env['RR_TEST_REPAIR'] = '1'
    if args.unstick:
        env['RR_TEST_UNSTICK'] = '1'
    if args.stop_start:
        env['RR_TEST_STOP_START'] = '1'
    if args.maximized:
        env['RR_VIDEO_MAXIMIZED'] = '1'
    if args.no_fxaa:
        env['RR_VIDEO_FXAA'] = '0'
    if args.destroy:
        env['RR_TEST_DESTROY'] = args.destroy
    if args.undertrack:
        env['RR_TEST_UNDERTRACK'] = args.undertrack
    for item in args.env:
        key, sep, value = item.partition('=')
        if not sep or not key.startswith('RR_'):
            parser.error('--env expects RR_NAME=value')
        env[key] = value
    if args.results_confirm:
        env['RR_TEST_RESULTS_CONFIRM'] = str(args.results_confirm)
    if args.results_exit:
        env['RR_TEST_RESULTS_EXIT'] = args.results_exit
        # On the original Results screen (Results104, now Single Race too) the
        # exit is the player confirming its pages: 1AB5BC then leaves for
        # Hall11 (a record), Unlock10 or Course6, as the original does.
        env.setdefault('RR_TEST_RESULTS_CONFIRM', '2')
    if args.main_exit:
        env['RR_TEST_MAIN_EXIT'] = args.main_exit
    if args.hold_results:
        env['RR_TEST_RESULTS_HOLD'] = '1'
    if args.hold_results_hall:
        env['RR_TEST_RESULTS_HALL_CAPTURE'] = '1'
    if args.camera_cycle:
        env['RR_TEST_CAMERA_CYCLE'] = '1'
    if args.ieee_subnormals:
        env['RR_TEST_IEEE_SUBNORMALS'] = '1'
    if args.garage_close_frame >= 0:
        env['RR_TEST_CLOSE_GARAGE_AT_FRAME'] = str(args.garage_close_frame)
    if args.gpu_vertex_check > 0:
        env['RR_GPU_POD_VERTEX_CHECK'] = str(args.gpu_vertex_check)
    if args.pad_hold_a:
        env['RR_TEST_PAD_HOLD_A'] = '1'
    if args.window:
        env['RR_TEST_WINDOW_SIZE'] = args.window
        env['RR_NATIVE_FIXED_WINDOW'] = '0'
    if args.input_device:
        env['RR_HUMAN_INPUT_DEVICE'] = args.input_device
    if args.player_racer is not None:
        env['RR_TEST_PLAYER_RACER'] = str(args.player_racer)
    if args.addon_rider is not None:
        env['RR_TEST_ADDON_RIDER'] = args.addon_rider
    if args.hold_rider:
        env['RR_TEST_ADDON_RIDER_HOLD' if args.addon_rider is not None else 'RR_TEST_RIDER_HOLD'] = '1'
        env['RR_EVIDENCE_LOGS_ONLY'] = '0'   # this run writes the held Rider frame as an image
    if args.addon_course is not None:
        # Picked through the real Course screen (course arrow, then the original Confirm);
        # RR_TEST_ADDON_COURSE (startup pre-selection, no UI) is deliberately not set.
        env['RR_TEST_ADDON_COURSE_PICK'] = args.addon_course
    if args.hold_course:
        env['RR_TEST_ADDON_COURSE_HOLD'] = '1'
    if args.course_mode:
        # (Before R11 this also turned logs-only off, so every Time Trial run wrote images;
        # only --hold-course writes the Course frame now.)
        env['RR_TEST_COURSE_SUBTYPE'] = '3' if args.course_mode == 'time_trial' else '1'
    if args.course is not None:
        env['RR_TEST_COURSE'] = args.course
    if args.laps is not None:
        env['RR_TEST_COURSE_LAPS'] = str(args.laps)
    if args.return_hold:
        env['RR_TEST_RETURN_HOLD_SECONDS'] = str(args.return_hold)
    if args.steer_invert:
        env['RR_TEST_STEER_INVERT'] = '1'
    # A run that must reach Results (and what follows it) keeps racing past
    # the FPS evidence: --results-exit / --hold-results / --hold-results-hall
    # imply --race-full (otherwise the race stops before its finish).
    if (args.results_exit or args.hold_results or args.hold_results_hall or args.pause_quit or args.pause_restart) and not args.race_full:
        args.race_full = True
        print('race_full implied by --results-exit/--hold-results/--hold-results-hall/--pause-quit/--pause-restart', flush=True)
    if args.race_full:
        env['RR_TEST_RACE_FULL_BUDGET'] = '1'
    if args.all_ai:
        env['RR_ORIGINAL_AI_ALL_RACERS'] = '1'
    if args.route == 'none':
        env.pop('RR_UNATTENDED_UI_ROUTE', None)
        env['RR_TRACK'] = args.track
    if args.profile_performance or args.profile_gpu_stages:
        env.update(RR_TRACE_PHASES='1', RR_TRACE_RENDER='1')
    if args.profile_gpu_stages:
        env['RR_TRACE_GPU_STAGES'] = '1'
    if args.restart_once:
        env['RR_UNATTENDED_RESTART_ONCE'] = '1'
    for switch in args.renderer_switch:
        env[switch] = '1'
    command = [str(staged), '--assets', str(assets)]
    result = dict(command=command, route=args.route, playerRacer=args.player_racer,
                  watchdogSeconds=args.seconds,
                  sourceBuildReceipt=str(build / 'build-receipt.json'),
                  executableSha256=digest(staged),
                  runtimeHashes={p.name: digest(output / p.name) for p in runtime},
                  nativeSaveDirectory=str(saves), audioCapturePath=str(output / 'audio.wav'),
                  visualVerified=False, audioVerified=False, hostInputUsed=False, logsOnly=logs_only,
                  addonRider=args.addon_rider, addonRiderHold=args.hold_rider, addonCourse=args.addon_course,
                  addonCourseHold=args.hold_course, courseMode=args.course_mode,
                  eyecheckDlcHall=args.eyecheck_dlc_hall, resultsHold=args.hold_results, resultsHallCapture=args.hold_results_hall,
                  seedSlowHall=args.seed_slow_hall, course=args.course, laps=args.laps, returnHold=args.return_hold,
                  resultsExit=args.results_exit,
                  cameraPoseTrace=args.camera_trace, nativeRestartOnce=args.restart_once, rendererSwitches=sorted(set(args.renderer_switch)),
                  performanceProfiling=args.profile_performance or args.profile_gpu_stages,
                  serializedGpuStageProfiling=args.profile_gpu_stages)
    (output / 'launch.json').write_text(json.dumps(result, indent=2) + '\n')
    # Inherited process error mode prevents OS crash-dialog waits. Application
    # dialogs, if any, remain bounded by termination of this exact child handle.
    # User rule: one instance for this session, one for the arcade session.
    # Never start while another of OUR RacerRevengePC.exe runs (any copy not
    # under the work/scratch/racer-arcade folder, i.e. main-tree builds and evidence
    # outputs). Arcade runs and the user's silo/play are ignored. Wait, never
    # kill; the watchdog starts after the wait.
    arcade = str(ROOT / 'work' / 'scratch' / 'racer-arcade').lower()
    waited = 0.0
    while True:
        listing = subprocess.run(['powershell', '-NoProfile', '-Command',
            "Get-CimInstance Win32_Process -Filter \"Name='RacerRevengePC.exe'\" | ForEach-Object { $_.ExecutablePath }"],
            capture_output=True, text=True, errors='replace').stdout
        ours = [line.strip() for line in listing.splitlines()
                if line.strip() and not line.strip().lower().startswith(arcade)]
        if not ours:
            break
        if waited == 0.0:
            print('one_instance_wait: another RacerRevengePC.exe of this session is running; waiting', flush=True)
        time.sleep(15.0); waited += 15.0
    result['oneInstanceWaitSeconds'] = waited
    prior_mode = ctypes.windll.kernel32.SetErrorMode(0x0001 | 0x0002 | 0x8000)
    started = time.monotonic()
    child = None
    try:
        with (output / 'stdout.log').open('wb') as stdout, (output / 'stderr.log').open('wb') as stderr:
            child = subprocess.Popen(command, cwd=ROOT, env=env, stdout=stdout,
                                     stderr=stderr, creationflags=subprocess.CREATE_NO_WINDOW)
            result['pid'] = child.pid
            try:
                result['exitCode'] = child.wait(timeout=args.seconds)
                result['watchdogExpired'] = False
            except subprocess.TimeoutExpired:
                child.kill()
                result['exitCode'] = child.wait()
                result['watchdogExpired'] = True
    finally:
        if child is not None and child.poll() is None:
            child.kill()
            child.wait()
        ctypes.windll.kernel32.SetErrorMode(prior_mode)
        result['seconds'] = time.monotonic() - started
        result['childStopped'] = child is not None and child.poll() is not None
        summary = capture / 'window-evidence-summary.json'
        if summary.is_file():
            result['gameSummary'] = json.loads(summary.read_text())
        log = output / 'RacerRevenge.log'
        log_text = log.read_text(errors='replace') if log.is_file() else ''
        displays = re.findall(r'native_player_display generation=(\d+) sdl_id=(\d+) video=(\S+) host=(\S+)', log_text)
        retained = re.findall(r'native_player_window phase=retained sdl_id=(\d+) video=(\S+) host=(\S+)', log_text)
        lifecycle = re.findall(r'native_window_lifetime phase=(created|destroyed) sdl_id=(\d+) window=(\S+)', log_text)
        # The first Display is constructed before its window, hence SDL ID 0.
        owners = {tuple(row[2:]) for row in displays}
        live_ids = {row[1] for row in displays if row[1] != '0'}
        result['windowContinuity'] = dict(displays=displays, retained=retained, lifecycle=lifecycle,
            verified=bool(len(displays) >= 2 and len(owners) == 1 and retained and
                          len(lifecycle) == 2 and lifecycle[0][0] == 'created' and
                          lifecycle[1][0] == 'destroyed' and lifecycle[0][1] != '0' and
                          lifecycle[0][1:] == lifecycle[1][1:] and
                          live_ids == {lifecycle[0][1]} and
                          all(row[0] == lifecycle[0][1] and tuple(row[1:]) in owners for row in retained)))
        # The course the run asked for (data): --course, the add-on course item, else TA (fresh Course entry).
        track = (args.course.upper() if args.course else
                 args.addon_course.split('/', 1)[1].upper() if args.addon_course else 'TA')
        # A seeded tournament run goes to the course the seed's career reached.
        if args.route == 'main-tournament-course-ta' and args.seed_save:
            track = seeded_tournament_track((saves / 'progress-v1.bin').read_bytes()) or track
        result['expectedTrack'] = track
        loading_sessions = [(1, 'UI'), (2, track)] + ([(3, track)] if args.restart_once else [])
        loading_files = [capture / f'loading-session-{session}' / f'loading-{code}-{frame}.ppm'
                         for session, code in loading_sessions for frame in (1, 2)]
        loading_assets = re.findall(r'original_loading_assets background=(\S+) layout=(\S+) background_sha256=([0-9a-f]{64})', log_text)
        result['originalLoadingScreens'] = dict(assets=loading_assets,
            captures=[str(p) for p in loading_files],
            captured=all(p.is_file() and p.stat().st_size > 32 for p in loading_files))
        result['routeCompleted'] = bool(result.get('exitCode') == 0 and
            not result.get('watchdogExpired', True) and
            result['windowContinuity']['verified'] and
            (logs_only or result['originalLoadingScreens']['captured']) and
            {row[0] for row in loading_assets} == {'LD_UI.PST', f'LD_{track}.PST'} and
            result.get('gameSummary', {}).get('completed') and
            result.get('gameSummary', {}).get('cleanup_complete'))
        if args.restart_once:
            restart = result.get('gameSummary', {}).get('native_restart_test', {})
            first = restart.get('first_race') or {}
            second = result.get('gameSummary', {}).get('track', {})
            files = [first.get('image_file'), first.get('receipt_file'), first.get('baseline_file'),
                     second.get('image_file'), second.get('receipt_file'),
                     result.get('gameSummary', {}).get('baseline_file')]
            if logs_only:
                files = [first.get('receipt_file'), first.get('baseline_file'), second.get('receipt_file'), result.get('gameSummary', {}).get('baseline_file')]
            result['nativeRestartVerified'] = bool(restart.get('issued') and
                len(displays) == 3 and len(retained) == 2 and len(set(files)) == (4 if logs_only else 6) and
                all(path and inside(Path(path)).is_file() for path in files) and
                second.get('receipt_file' if logs_only else 'image_file') == str(capture / 'race-session-3' / ('track-1.json' if logs_only else 'track-1.ppm')))
            if result['nativeRestartVerified']:
                before = json.loads(Path(first['receipt_file']).read_text())
                after = json.loads(Path(second['receipt_file']).read_text())
                result['nativeRestartVerified'] = bool(
                    before.get('session') == 2 and after.get('session') == 3 and
                    before.get('course_selection') == after.get('course_selection') and
                    before.get('selected_rider') == after.get('selected_rider') and
                    all(row.get('track') == track and row.get('race_phase') == 2 and
                        row.get('settled') and row.get('window_swap_submitted') and
                        row.get('world_ticks', 0) > 0 and 0 < row.get('race_seconds', 0) < 2
                        for row in (before, after)))
            result['routeCompleted'] = result['routeCompleted'] and result['nativeRestartVerified']
        if logs_only:
            result['noImagesWritten'] = not any(p.suffix.lower() in {'.ppm', '.png', '.jpg', '.pending'} for p in output.rglob('*'))
            result['routeCompleted'] = result['routeCompleted'] and result['noImagesWritten']
        if args.eyecheck_dlc_hall is not None:
            # Held on the Hall TA page until the wall budget: complete when the
            # Hall frame was written and the hold was reached.
            texts = log_text
            for name in ('stderr.log', 'stdout.log'):
                extra = output / name
                if extra.is_file():
                    texts += extra.read_text(errors='replace')
            hall_images = sorted(str(p) for p in capture.rglob('hall-*.ppm'))
            result['dlcHallEyecheck'] = dict(
                key=args.eyecheck_dlc_hall, hallImages=hall_images,
                holdReached='hall_route hold screen=11 course=0 planet=0' in texts)
            result['routeCompleted'] = bool(hall_images and result['dlcHallEyecheck']['holdReached'])
        if args.hold_rider:
            # A held run never confirms and ends at its wall budget, so the race
            # receipts above are absent. It is complete when the Rider frame with
            # the add-on highlighted was written and the hold was reached.
            texts = log_text
            for name in ('stderr.log', 'stdout.log'):
                extra = output / name
                if extra.is_file():
                    texts += extra.read_text(errors='replace')
            rider_images = sorted(str(p) for p in capture.rglob('rider-*.ppm'))
            result['addonRiderHold'] = dict(
                key=args.addon_rider, riderImages=rider_images,
                holdReached=(f'addon_rider_route hold key={args.addon_rider} ' in texts) if args.addon_rider
                            else ('rider_route hold rider=' in texts),
                carousel=re.findall(r'dlc_rider_carousel [^\r\n]*', texts),
                draw=re.findall(r'dlc_rider_draw [^\r\n]*', texts),
                model=re.findall(r'dlc_rider_model[ _][^\r\n]*', texts))
            result['routeCompleted'] = bool(rider_images and result['addonRiderHold']['holdReached'])
        if args.hold_course:
            # Same rule for a held Course screen: the Course frame with the add-on
            # course highlighted was written and the hold was reached.
            texts = log_text
            for name in ('stderr.log', 'stdout.log'):
                extra = output / name
                if extra.is_file():
                    texts += extra.read_text(errors='replace')
            course_images = sorted(str(p) for p in capture.rglob('course-*.ppm'))
            result['addonCourseHold'] = dict(
                key=args.addon_course, courseImages=course_images,
                holdReached=f'addon_course_route hold key={args.addon_course} ' in texts,
                controls=re.findall(r'dlc_course_control [^\r\n]*', texts),
                carousel=re.findall(r'dlc_course_carousel [^\r\n]*', texts),
                draw=re.findall(r'dlc_course_draw [^\r\n]*', texts),
                images=re.findall(r'dlc_course_images [^\r\n]*', texts))
            result['routeCompleted'] = bool(course_images and result['addonCourseHold']['holdReached'])
        elif args.addon_course is not None:
            texts = log_text
            for name in ('stderr.log', 'stdout.log'):
                extra = output / name
                if extra.is_file():
                    texts += extra.read_text(errors='replace')
            result['addonCoursePick'] = dict(
                key=args.addon_course,
                confirmed=f'addon_course_route confirm key={args.addon_course} ' in texts,
                eventMoves=re.findall(r'addon_course_route event_move=[^\r\n]*', texts),
                launch=re.findall(r'course_route_launch [^\r\n]*', texts),
                record=re.findall(r'dlc_course_record [^\r\n]*', texts),
                raceAck=re.findall(r'race_ack route=[^\r\n]*', texts),
                restore=re.findall(r'dlc_course_carousel action=restore[^\r\n]*', texts),
                selected=re.findall(r'dlc_course_confirm [^\r\n]*', texts),
                carousel=re.findall(r'dlc_course_carousel [^\r\n]*', texts),
                draw=re.findall(r'dlc_course_draw [^\r\n]*', texts),
                hall=re.findall(r'dlc_hall_(?:addon|draw) [^\r\n]*', texts),
                progress=re.findall(r'dlc_progress [^\r\n]*', texts),
                seededSlowHall=args.seed_slow_dlc_hall)
        quit_texts = log_text + ''.join((output / name).read_text(errors='replace')
                                        for name in ('stderr.log', 'stdout.log') if (output / name).is_file())
        if args.main_exit:
            # PC-NATIVE ADDITION (EXIT GAME): the game closed itself from Main through the
            # window-Close path. Proof is in the log, not a frame: the dialog was confirmed,
            # the frontend took the clean close, the process returned 0 and the log ends with
            # a plain SESSION END (no exit=terminate, no refused close step).
            markers = re.findall(r'=+ RacerRevenge (SESSION [^\r\n]*?) UTC', log_text)
            close = re.findall(r'pc_native_exit_game frontend_close route_screen=(\d+) session=(\d+) race_return=(\d)', quit_texts)
            result['mainExit'] = dict(
                mode=args.main_exit,
                rowReady='pc_native_exit_game row=ready' in quit_texts,
                rowUnavailable=re.findall(r'pc_native_exit_game row=unavailable[^\r\n]*', quit_texts),
                steps=re.findall(r'(?:test_main_exit|pc_native_exit_game) [^\r\n]*', quit_texts),
                frontendClose=close,
                nativeExitCode=re.findall(r'native_exit_code=(-?\d+)', quit_texts),
                sessionMarkers=markers,
                refusals=re.findall(r'frontend_close_refused[^\r\n]*', quit_texts),
                terminate='native_terminate' in quit_texts or 'exit=terminate' in quit_texts)
            me = result['mainExit']
            result['routeCompleted'] = bool(
                result.get('exitCode') == 0 and not result.get('watchdogExpired', True) and
                me['rowReady'] and 'pc_native_exit_game dialog=confirm' in quit_texts and
                'pc_native_exit_game exit_requested=1' in quit_texts and
                close and close[-1][0] == '4' and
                (close[-1][1] == '0' if args.main_exit == 'fresh' else int(close[-1][1]) > 0) and
                me['nativeExitCode'] == ['0'] and markers and markers[-1].strip() == 'SESSION END' and
                not me['refusals'] and not me['terminate'] and
                (not logs_only or result.get('noImagesWritten', True)))
        quit_lines = re.findall(r'(?:test_pause_quit|pause_quit|pause_quit_admission|pause_request|pause_open|pause_row|pause_close|'
                                r'pause_owner|pause_navigation|original_pause_owner|time_trial_restart) [^\r\n]*', quit_texts)
        if args.pause_quit or args.pause_restart or quit_lines:
            result['pauseQuit'] = dict(seconds=args.pause_quit or args.pause_restart,
                                       row=1 if (args.pause_restart and not args.pause_quit) else 10, lines=quit_lines)
        (output / 'execution.json').write_text(json.dumps(result, indent=2) + '\n')
        # Disk hygiene (user request 2026-11-13): a logs-only run keeps its logs and
        # receipts; the staged program, DLL copies and audio recording (~65 MB per
        # run) are removed once the child has stopped.
        if args.logs_only and not args.keep_binaries and result.get('childStopped'):
            for name in [staged.name, *(dll.name for dll in runtime), 'audio.wav']:
                try:
                    (output / name).unlink(missing_ok=True)
                except OSError:
                    pass
            for extra in output.glob('audio-*.wav'):
                try:
                    extra.unlink()
                except OSError:
                    pass
    print(json.dumps(result, indent=2))
    return 0 if result['routeCompleted'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
