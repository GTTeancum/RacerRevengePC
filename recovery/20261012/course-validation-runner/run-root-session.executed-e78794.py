#!/usr/bin/env python3
"""UNEXECUTED SOURCE: one root-owned Course preparation/build/menu session.

No imports, syntax checks, compiler calls, conversion or launch were performed
while authoring this file. Root must review it and explicitly invoke it once.
"""
import argparse
import ctypes
import datetime as dt
import fcntl
import hashlib
import json
import os
from pathlib import Path
import re
import selectors
import signal
import stat
import subprocess
import sys
import time

BASE = Path('/workspace/shared')
REPO = BASE / 'rr-recovery-git'
WORK = BASE / 'recovery-20261012/course-validation-01'
ASSETS = BASE / 'recovery-20261010/native-assets-prepared-01'
INPUT = BASE / 'input-recovery-20261012/extracted'
OBJECTS = BASE / 'recovery-20261012/gc-light-fog-validation-01/objects'
LEDGER = BASE / 'rr-last-central-test.json'
LOCK = BASE / 'rr-central-test.lock'
COMMIT = 'f62504351fc63f4749ff68aa45eb89803f46ad4e'
SNAPSHOT = 'recovery/20261012/course-validation-plan/'
RECORDER = SNAPSHOT + 'record_course_roundtrip_candidate.py'
EXTRACTOR = SNAPSHOT + 'extract_rider19_pst_candidate.cpp'
RECORDER_SHA = '18fb2086d49b55074773ca72448a08f50580db4b6f9adaf8b101f86ecd60475a'
EXTRACTOR_SHA = '63524603c62a3e589eda0f000ea01df7b0fce19fc5211adcf90b91df5c1d9fb0'
CATALOG = 'src/assets/original_ui_ipum_texture_cache50kg_generated.inc'
CATALOG_SHA = '957b0959e08c2facdd9ae99632f22d047ea76cb717b81db042e8f1bd676d8f92'
PST_SHA = 'ad0bcd92f415d65d8bf68ae9e65fe355043e297e834cdffeae4ceac7cfa60cd3'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
HELPER_SHA = 'e482393f6b384863d17c7cc84aa930affd466eb6c6718dfd1292713408bf9efe'
GENERATOR_SHA = '6110004287240621db66e464afc87bbf53b9628843d3e051313b531c03fab646'
MIP_SHA = (
    'fe3d1143c277222cb007cd8135cf6cddb79558a6280c318378a329ef99b66575',
    '9698ecfcc57039c0f2556a2e64eb0e9b7353d9400b3f7624111a912919ac48bd',
    '1eeb1faf0a0379d50a02faefd99d47b1d7fce3d523bf51243e2c0a6a1e4ee114',
    'c02fc660a81ade4c0016fa5f9739b8ebffd8ca470b0b3a9ab89aeb5dd6730afb',
    '57c5076fb52f906996b8d74726e7089bb08344463e377063a480c566c6c56bae',
)
ARCHIVES = (
    ('APPSTART.RES',58977,'04eb376a507d0745925e4351903a8ef7a1024d37d8250b78892cba0c74220b89'),
    ('LOC.RES',35925,'b02e9c437311fd18b9dcd52c0d02222320be4eb7e5bfa17a60f564b05e6d4a0e'),
    ('UI/TEXTURES/OVERLAYS.RES',154528,'ab51e88aef70d4a619ca3e614c64b90bad07deb3b9f0ec918577a44a8effb15c'),
    ('UI/UIFONTS.RES',44870,'883c0fa962a94597e7f6f3ca62721b33c7ee3200149e8a3e0b464f894f4798d2'),
    ('UI/UIMAIN/UIMAIN.RES',370304,'b2019a9dcd541baeba90ef3e3b57480f9a023155290aec84a195ecdf1b990f84'),
    ('UI/UIBKG.RES',1330049,'67b7774815bd4352e72103160e323c00e062f2e2845e05e9720b38796185091f'),
    ('UI/CONTPOP.RES',4318,'290a8f255bac1b9e2f33fbcf54ba84066ea590c4910036a8c100572ed81f3574'),
    ('UI/UICOURSE/UICOURSE.RES',1227424,'f45585dd7fe7ba2a24cfc5b6ce4d3fb5efa6bc279f636e071aecf5ccf970c855'),
    ('UI/UIRIDER/UIRIDER.RES',5552683,'112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856'),
    ('UI/UIOPTION/UIOPTION.RES',200341,'fde295f01154aa9fa6fb7ff5faccdb0a3ac8380214241ba3d13490ebd0d8ade2'),
    ('UI/UIPODTEX.RES',2653798,'ffb0f32563e3e5b6e3fefe915bbb37a00610e1a9a44163bce9737c2ac9d6e641'),
    ('UI/PODPHYS.RES',46563,'8361cb484ea841e1772cc0ddec2223fa923e397c1cc9a89f06ab06b1cd7ca2c8'),
)
COURSE_SOURCES = (
    'platform/linux/native/original_ui_course_draw_contribution50kg.cpp',
    'platform/linux/native/original_ui_course_draw_resources50kg.cpp',
    'platform/linux/native/original_ui_course_hologram_draw50kg.cpp',
    'platform/linux/native/original_ui_course_session50kg.cpp',
    'src/assets/original_ui_course_background50kg.cpp',
    'src/assets/original_ui_course_hologram_assets50kg.cpp',
    'src/game/original_ui_course_camera50kg.cpp',
    'src/game/original_ui_course_clip_camera50kg.cpp',
    'src/game/original_ui_course_hologram_scene50kg.cpp',
    'src/game/original_ui_course_lighting50kg.cpp',
    'src/game/original_ui_course_page_draw50kg.cpp',
    'src/game/original_ui_course_render_context50kg.cpp',
    'src/game/original_ui_course_selection50kg.cpp',
    'src/game/original_ui_course_widget_controller50kg.cpp',
    'src/game/original_ui_course_widgets50kg.cpp',
)
INTERRUPTED = None
DEADLINE = None
MAX_PHASE_LOG = 64 * 1024 * 1024


def need(okay, message):
    if not okay:
        raise RuntimeError(message)


def now():
    return dt.datetime.now(dt.timezone.utc)


def check_time():
    need(INTERRUPTED is None, 'Root runner interrupted by signal ' + str(INTERRUPTED))
    need(DEADLINE is None or time.monotonic() < DEADLINE, 'Current phase deadline exceeded')


def no_symlinks(path):
    need(path.is_absolute(), 'Absolute path required: ' + str(path))
    for current in (path, *path.parents):
        need(not current.is_symlink(), 'Symlink component rejected: ' + str(current))


def digest(path, maximum=1024 * 1024 * 1024):
    no_symlinks(path)
    before = path.stat()
    need(stat.S_ISREG(before.st_mode) and 0 < before.st_size <= maximum,
         'Regular bounded nonempty file required: ' + str(path))
    h = hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            check_time()
            h.update(block)
    after = path.stat()
    need(all(getattr(before, k) == getattr(after, k) for k in
             ('st_dev','st_ino','st_size','st_mtime_ns','st_ctime_ns')),
         'File changed while hashing: ' + str(path))
    return h.hexdigest()


def authenticate(path, count, sha):
    need(path.stat().st_size == count and digest(path, count) == sha,
         'Authenticated input identity mismatch: ' + str(path))
    return {'path': str(path), 'bytes': count, 'sha256': sha}


def load_json(path, maximum=8 * 1024 * 1024):
    no_symlinks(path)
    need(path.is_file() and 0 < path.stat().st_size <= maximum, 'Bounded JSON required: ' + str(path))
    return json.loads(path.read_text())


def atomic_json(path, value):
    no_symlinks(path)
    payload = (json.dumps(value, indent=2, sort_keys=True) + '\n').encode()
    need(len(payload) <= 8 * 1024 * 1024, 'Root ledger/provenance exceeded 8 MiB')
    temporary = path.with_name(path.name + '.root-%d.part' % os.getpid())
    with temporary.open('xb') as stream:
        stream.write(payload)
        stream.flush()
        os.fsync(stream.fileno())
    os.replace(temporary, path)
    fd = os.open(path.parent, os.O_RDONLY | os.O_DIRECTORY)
    try:
        os.fsync(fd)
    finally:
        os.close(fd)


def copy_new(source, target, maximum=64 * 1024 * 1024, allow_empty=False):
    no_symlinks(source)
    no_symlinks(target)
    info = source.stat()
    need(stat.S_ISREG(info.st_mode) and (0 if allow_empty else 1) <= info.st_size <= maximum,
         'Unsafe/oversized copy source: ' + str(source))
    target.parent.mkdir(parents=True, exist_ok=True, mode=0o700)
    total = 0
    with source.open('rb') as src, target.open('xb') as dst:
        while True:
            check_time()
            data = src.read(1024 * 1024)
            if not data:
                break
            total += len(data)
            need(total <= maximum, 'Copy source grew beyond bound')
            dst.write(data)
        dst.flush()
        os.fsync(dst.fileno())
    need(total == info.st_size, 'Copy extent changed')


def process_stat(pid):
    try:
        raw = Path('/proc/%d/stat' % pid).read_text()
    except FileNotFoundError:
        return None
    fields = raw[raw.rfind(')') + 2:].split()
    need(len(fields) >= 20, 'Malformed process identity')
    return {'pid': pid, 'state': fields[0], 'ppid': int(fields[1]),
            'pgrp': int(fields[2]), 'session': int(fields[3]), 'start_ticks': int(fields[19])}


def owned_descendants():
    # Only /proc process identity metadata is read. No cmdlines, environ, user
    # computer, credentials or unrelated process contents are accessed.
    table = {}
    for entry in Path('/proc').iterdir():
        if entry.name.isdecimal():
            row = process_stat(int(entry.name))
            if row:
                table[row['pid']] = row
    owned = {os.getpid()}
    while True:
        new = {pid for pid, row in table.items() if row['ppid'] in owned} - owned
        if not new:
            break
        owned |= new
    return [table[pid] for pid in sorted(owned - {os.getpid()}) if pid in table]


def signal_owned(rows, number):
    for row in rows:
        live = process_stat(row['pid'])
        if live and live['start_ticks'] == row['start_ticks']:
            try:
                os.kill(row['pid'], number)
            except ProcessLookupError:
                pass


def reap_adopted(leader=None):
    # Popen owns reaping its leader; do not steal its return status.
    for row in owned_descendants():
        if row['pid'] != leader and row['ppid'] == os.getpid() and row['state'] == 'Z':
            try:
                os.waitpid(row['pid'], os.WNOHANG)
            except ChildProcessError:
                pass


def enable_subreaper():
    # Process-local Linux ownership, not a system/network configuration change.
    libc = ctypes.CDLL(None, use_errno=True)
    need(libc.prctl(36, 1, 0, 0, 0) == 0, 'Cannot enable Linux child subreaper')
    value = ctypes.c_int()
    need(libc.prctl(37, ctypes.byref(value), 0, 0, 0) == 0 and value.value == 1,
         'Child-subreaper ownership was not established')


class Session:
    def __init__(self, state):
        self.state = state
        self.journal_healthy = True
        self.env = {'PATH':'/usr/bin:/bin', 'LANG':'C', 'LC_ALL':'C', 'TZ':'UTC',
                    'HOME':str(WORK/'home'), 'TMPDIR':str(WORK/'tmp'),
                    'XDG_CACHE_HOME':str(WORK/'cache'), 'CXX':'/usr/bin/g++',
                    'PYTHONDONTWRITEBYTECODE':'1', 'GIT_OPTIONAL_LOCKS':'0',
                    'GIT_CONFIG_NOSYSTEM':'1', 'GIT_TERMINAL_PROMPT':'0'}

    def save(self):
        try:
            atomic_json(WORK/'central-session.json', self.state)
            atomic_json(LEDGER, self.state)
        except BaseException:
            self.journal_healthy = False
            raise

    def run(self, name, argv, seconds, grace=5):
        global DEADLINE
        check_time()
        need(not owned_descendants(), 'Prior phase still owns live descendants')
        phase = {'name':name, 'argv':list(map(str, argv)), 'started_at_utc':now().isoformat(),
                 'timeout_seconds':seconds, 'cleanup_grace_seconds':grace, 'signals':[]}
        self.state['phases'].append(phase)
        self.save()
        proc = None
        selector = selectors.DefaultSelector()
        size = 0
        eof = False
        failed = None
        def drain(delay):
            nonlocal size, eof
            for key, _ in selector.select(delay):
                data = os.read(key.fileobj.fileno(), 65536)
                if not data:
                    selector.unregister(key.fileobj)
                    eof = True
                    continue
                remaining = max(0, MAX_PHASE_LOG - size)
                log.write(data[:remaining])
                size += min(len(data), remaining)
                if len(data) > remaining:
                    phase['log_truncated'] = True
                    raise RuntimeError(name + ' log exceeded 64 MiB; retained prefix')
        try:
            with (WORK/(name+'.log')).open('xb', buffering=0) as log:
                try:
                    proc = subprocess.Popen(phase['argv'], cwd=REPO, env=self.env,
                                            stdin=subprocess.DEVNULL, stdout=subprocess.PIPE,
                                            stderr=subprocess.STDOUT, start_new_session=True, close_fds=True)
                    phase['leader_pid'] = proc.pid
                    phase['leader_identity'] = process_stat(proc.pid)
                    os.set_blocking(proc.stdout.fileno(), False)
                    selector.register(proc.stdout, selectors.EVENT_READ)
                    deadline = time.monotonic() + seconds
                    while proc.poll() is None:
                        drain(0.05)
                        need(INTERRUPTED is None, 'Runner interrupted')
                        need(time.monotonic() < deadline, name + ' timeout')
                    phase['exit_code'] = proc.wait()
                    end_drain = time.monotonic() + 2
                    while not eof and time.monotonic() < end_drain:
                        drain(0.02)
                    reap_adopted()
                    need(not owned_descendants(), name + ' left descendants after leader exit')
                    need(phase['exit_code'] == 0, name + ' failed; no retry authorized')
                except BaseException as error:
                    failed = error
                    phase['error'] = str(error)
                    # The recorder gets its own full cleanup opportunity first.
                    # Its native child has a different session and is not assumed
                    # to die when the recorder process or process group exits.
                    if proc is not None and proc.poll() is None:
                        row = process_stat(proc.pid)
                        if row:
                            signal_owned([row], signal.SIGTERM)
                            phase['signals'].append({'signal':15, 'targets':[row]})
                    for number, duration in ((None, grace), (signal.SIGTERM,5), (signal.SIGKILL,5)):
                        until = time.monotonic() + duration
                        while time.monotonic() < until:
                            if proc is not None:
                                proc.poll()
                            reap_adopted(proc.pid if proc is not None and proc.poll() is None else None)
                            rows = owned_descendants()
                            if not rows:
                                break
                            if number is not None:
                                signal_owned(rows, number)
                                # Bounded summary; identities, not every poll.
                                if not phase['signals'] or phase['signals'][-1]['signal'] != int(number):
                                    phase['signals'].append({'signal':int(number), 'targets':rows[:128]})
                            try:
                                drain(0.05)
                            except Exception as cleanup_error:
                                phase['drain_error'] = str(cleanup_error)
                                time.sleep(0.05)
                        if not owned_descendants():
                            break
                    if proc is not None and proc.poll() is not None:
                        phase['exit_code'] = proc.wait()
                    reap_adopted()
                    end_drain = time.monotonic() + 2
                    while not eof and time.monotonic() < end_drain:
                        try:
                            drain(0.02)
                        except Exception:
                            break
                finally:
                    if proc is not None and proc.stdout is not None:
                        proc.stdout.close()
                    selector.close()
                    phase['log_bytes'] = size
                    phase['remaining_descendants'] = owned_descendants()
                    phase['finished_at_utc'] = now().isoformat()
                    self.save()
        finally:
            DEADLINE = None
        if failed is not None:
            raise failed

    def git(self, name, *args):
        self.run(name, ['/usr/bin/git','-c','core.fsmonitor=false','-c','core.hooksPath=/dev/null',
                        '-C',str(REPO),*args], 30)
        return (WORK/(name+'.log')).read_bytes()

    def freeze(self, label):
        need(self.git(label+'-head','rev-parse','HEAD').decode().strip() == COMMIT,
             'Actual HEAD differs from the root-provided frozen commit')
        need(self.git(label+'-branch','symbolic-ref','HEAD').decode().strip() == 'refs/heads/main',
             'Only exact main is permitted')
        need(not self.git(label+'-status','status','--porcelain=v1','--untracked-files=all').strip(),
             'Frozen repository is dirty or has untracked files')
        need(digest(REPO/CATALOG) == CATALOG_SHA, 'Retained IPUM catalog changed')

    def internal(self, name, seconds, function):
        global DEADLINE
        phase = {'name':name, 'started_at_utc':now().isoformat(), 'timeout_seconds':seconds,
                 'kind':'bounded file staging/authentication, no project execution'}
        self.state['phases'].append(phase)
        self.save()
        DEADLINE = time.monotonic() + seconds
        try:
            function()
            check_time()
            phase['exit_code'] = 0
        except BaseException as error:
            phase['error'] = str(error)
            phase['exit_code'] = 1
            raise
        finally:
            DEADLINE = None
            phase['finished_at_utc'] = now().isoformat()
            self.save()

    def preserve_cache_metadata(self, label):
        destination = WORK/label
        destination.mkdir(mode=0o700)
        total = 0
        files = sorted(p for p in OBJECTS.iterdir() if p.suffix in ('.json','.d','.log'))
        need(len(files) <= 8192, 'Object-cache metadata file count exceeded bound')
        for source in files:
            total += source.stat().st_size
            need(total <= 256 * 1024 * 1024, 'Object-cache metadata exceeds 256 MiB')
            copy_new(source, destination/source.name, 64*1024*1024, allow_empty=True)
        self.state[label] = {'files':len(files), 'bytes':total,
                             'scope':'Full cache metadata/log snapshot, includes previously reused build evidence'}

    def execute(self):
        for name in ('home','tmp','cache'):
            (WORK/name).mkdir(mode=0o700)
        self.freeze('initial')
        self.state['tree'] = self.git('frozen-tree','rev-parse','HEAD^{tree}').decode().strip()
        for name, relative, sha in (('recorder',RECORDER,RECORDER_SHA),('extractor',EXTRACTOR,EXTRACTOR_SHA)):
            committed = self.git(name+'-committed-source','show',COMMIT+':'+relative)
            need(hashlib.sha256(committed).hexdigest() == sha and digest(REPO/relative) == sha,
                 'Committed/reviewed snapshot identity mismatch: '+relative)
        def stage():
            for name in ('ui-root','sources'):
                (WORK/name).mkdir(mode=0o700)
            copy_new(REPO/RECORDER, WORK/'sources/record_course_roundtrip_candidate.py')
            copy_new(REPO/EXTRACTOR, WORK/'sources/extract_rider19_pst_candidate.cpp')
            self.state['ui_archives'] = []
            for relative, count, sha in ARCHIVES:
                origin = INPUT/('RacerRevenge-original-game-UI-menu-and-videos' if relative.startswith('UI/')
                                else 'RacerRevenge-original-game-APPSTART-LOC')/relative
                authenticate(origin,count,sha)
                target = WORK/'ui-root'/relative
                copy_new(origin,target)
                self.state['ui_archives'].append(authenticate(target,count,sha))
            self.state['original_elf'] = authenticate(ASSETS/'work/native/project/disc/SLUS_202.68',2972720,ELF_SHA)
            need(digest(REPO/'tools/assets/decode_source_textures50fc.py') == HELPER_SHA,
                 'Retained image helper identity changed')
            need(digest(REPO/'tools/assets/recover_rider_ipum50kg.py') == GENERATOR_SHA,
                 'Retained IPUM converter identity changed')
            self.state['native_corpus_sha256'] = digest(ASSETS/'evidence/staticType2Producer49X/original-word/native-fixtures.json')
            no_symlinks(ASSETS/'work/native/project/disc/AUDIO')
            need((ASSETS/'work/native/project/disc/AUDIO').is_dir(), 'Cold native audio input root missing')
        self.internal('stage-authenticate-inputs',120,stage)
        self.freeze('before-extractor')
        self.run('compile-extractor', ['/usr/bin/g++','-std=c++20','-O2','-I'+str(REPO/'src'),
                 str(WORK/'sources/extract_rider19_pst_candidate.cpp'),
                 str(REPO/'src/assets/original_ui_archive50kg.cpp'),str(REPO/'src/assets/import_decode48c.cpp'),
                 '-lz','-o',str(WORK/'extract-rider19-pst')],120)
        self.freeze('after-extractor-build')
        self.state['extractor_executable_sha256'] = digest(WORK/'extract-rider19-pst')
        rider = WORK/'ui-root/UI/UIRIDER/UIRIDER.RES'
        self.run('extract-rider19-pst',[str(WORK/'extract-rider19-pst'),str(rider),str(WORK/'pst')],30)
        self.state['extracted_pst'] = authenticate(WORK/'pst/19c_1.pst',23096,PST_SHA)
        self.freeze('before-conversion')
        self.run('convert-ipum',['/usr/bin/python3','-B',str(REPO/'tools/assets/recover_rider_ipum50kg.py'),
                 '--repo',str(REPO),'--source-pst',str(WORK/'pst/19c_1.pst'),'--rider-archive',str(rider),
                 '--elf',str(ASSETS/'work/native/project/disc/SLUS_202.68'),
                 '--output-root',str(WORK/'ipum-cold'),'--execute-cold-conversion'],180)
        self.freeze('after-conversion')
        cache = WORK/'ipum-cold/evidence/ui-rider-ipum50kg'
        def verify_cache():
            manifest = load_json(cache/'manifest.json')
            need(manifest.get('source_pst_sha256') == PST_SHA and manifest.get('source_pst_bytes') == 23096
                 and manifest.get('source_archive_sha256') == ARCHIVES[8][2]
                 and manifest.get('source_archive_bytes') == 5552683 and manifest.get('elf_sha256') == ELF_SHA
                 and manifest.get('converter_sha256') == HELPER_SHA and manifest.get('generator_sha256') == GENERATOR_SHA,
                 'Converted cache source provenance mismatch')
            records = manifest.get('mips')
            need(isinstance(records,list) and len(records) == 5, 'Exactly five converted mip records required')
            self.state['verified_cache'] = []
            for mip, sha in enumerate(MIP_SHA):
                name = '19c_1-mip%d.rrtex' % mip
                dim = 16 << mip
                actual = authenticate(cache/name,16+dim*dim*4,sha)
                need(records[mip].get('filename') == name and records[mip].get('level') == mip
                     and records[mip].get('rrtex_sha256') == sha and records[mip].get('width') == dim
                     and records[mip].get('height') == dim and records[mip].get('rrtex_bytes') == actual['bytes'],
                     'Cache manifest differs from independently authenticated mip')
                self.state['verified_cache'].append(actual)
            generated = digest(WORK/'ipum-cold'/CATALOG)
            need(manifest.get('generated_catalog_sha256') == generated, 'Generated provenance catalog hash mismatch')
            self.state['cache_manifest_sha256'] = digest(cache/'manifest.json')
            self.state['generated_catalog_sha256'] = generated
            self.state['generated_catalog_matches_committed'] = generated == CATALOG_SHA
            self.state['cache_policy'] = 'Five immutable committed image hashes must match; generated catalog retained only, never copied to source'
        self.internal('verify-five-cache-hashes',30,verify_cache)
        self.internal('retain-prior-cache-metadata',120,lambda:self.preserve_cache_metadata('prior-build-cache'))
        self.freeze('before-native-build')
        binary = WORK/'rr-native-course'
        argvfile = WORK/'command.json'
        self.run('prepare-native-argv',['/usr/bin/python3','-B',str(REPO/'tools/prepare_interactive_build50dv.py'),
                 str(argvfile),str(binary)],30)
        command = load_json(argvfile)
        need(isinstance(command,list) and all(isinstance(x,str) for x in command)
             and command[0] == '/usr/bin/g++' and command.count('-o') == 1
             and command[command.index('-o')+1] == str(binary), 'Unexpected native compiler/output argv')
        required = (*COURSE_SOURCES,'src/game/original_ui_rider_voice_choice50kg.cpp',
                    'tests/native/terrain_interactive_drive50du.cpp','src/game/original_gc_gameplay_lighting_candidate.cpp',
                    'platform/linux/native/original_ui_frontend50kg.cpp','platform/linux/native/original_ui_rider_session50kg.cpp')
        need(all(str(REPO/p) in command for p in required), 'Integrated source closure missing')
        sources = [x for x in command if x.endswith(('.cpp','.cc','.cxx'))]
        need(all(Path(x).is_relative_to(REPO) for x in sources), 'Detached/outside source in native build')
        need(not any('terrain_offscreen_drive' in x or '/recovery/' in x for x in sources),
             'Detached diagnostic fixture substituted for integrated runtime')
        self.state['compiler_argv_sha256'] = digest(argvfile)
        self.state['native_build_attempted'] = True
        self.save()
        try:
            self.run('build-native',['/usr/bin/python3','-B',str(REPO/'tools/build_focused_incremental50ci.py'),
                     str(argvfile),str(OBJECTS)],900)
        finally:
            # Current cache metadata may be partial on failure. Do not relabel
            # an old last-build receipt as a successful current build.
            if not owned_descendants() and INTERRUPTED is None:
                self.internal('retain-current-cache-metadata',120,lambda:self.preserve_cache_metadata('current-build-cache'))
        receipt = load_json(OBJECTS/'last-build.json')
        need(receipt.get('link_exit') == 0 and isinstance(receipt.get('objects'),list), 'No successful current link receipt')
        object_rows = receipt['objects']
        need([row['source'] for row in object_rows] == sources, 'Build receipt source order differs from native argv')
        replacement = {row['source']:row['object'] for row in object_rows}
        need(receipt.get('link_command') == [replacement.get(x,x) for x in command], 'Current link argv differs')
        for row in object_rows:
            need(Path(row['object']).is_relative_to(OBJECTS) and digest(Path(row['object'])) == row['sha256'],
                 'Linked object identity mismatch')
        copy_new(OBJECTS/'last-build.json',WORK/'build-receipt.json')
        self.state['build_receipt_sha256'] = digest(WORK/'build-receipt.json')
        self.freeze('after-native-build')
        with binary.open('rb') as stream:
            need(stream.read(4) == b'\x7fELF', 'Native ELF executable required')
        self.state['executable_sha256'] = digest(binary)
        self.state['entry_header_sha256'] = digest(REPO/'tests/native/original_ui_main_entry50kg.h')
        need(digest(WORK/'sources/record_course_roundtrip_candidate.py') == RECORDER_SHA, 'Recorder snapshot changed')
        self.state['recorder_sha256'] = RECORDER_SHA
        self.freeze('before-menu')
        self.state['runtime_launch_attempted'] = True
        self.save()
        self.run('one-menu-observation',['/usr/bin/python3','-B',str(WORK/'sources/record_course_roundtrip_candidate.py'),str(binary),
                 '--expected-exe-sha256',self.state['executable_sha256'],
                 '--expected-entry-sha256',self.state['entry_header_sha256'],'--source-commit',COMMIT,
                 '--build-provenance',str(WORK/'build-receipt.json'),'--ui-root',str(WORK/'ui-root'),
                 '--rider-ipum-cache',str(cache),'--output',str(WORK/'take-01'),
                 '--root-reviewed-one-check','--not-before',self.state['started_at_utc'],
                 '--capture-hz','10','--startup-timeout','180','--ack-timeout','5',
                 '--navigation-timeout','12','--quit-timeout','10'],300,grace=30)
        summary = load_json(WORK/'take-01/summary.json')
        need(summary.get('outcome') == 'observed' and summary.get('presses') == summary.get('releases') == 3
             and summary.get('screen_activations') == [5,6,5] and not summary.get('forced_signals')
             and summary.get('native_closed',{}).get('cleanup_complete') is True
             and summary.get('native_closed',{}).get('world_ticks') == 0,
             'Recorder did not establish the requested bounded menu observation')
        self.state['menu_summary_sha256'] = digest(WORK/'take-01/summary.json')
        self.state['menu_outcome'] = summary['outcome']
        self.state['captured_frames'] = summary.get('frames')
        self.freeze('final')
        need(digest(binary) == self.state['executable_sha256'], 'Native executable changed during observation')


def main():
    global INTERRUPTED, DEADLINE
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source-commit', required=True)
    parser.add_argument('--root-authorized-one-session', action='store_true')
    args = parser.parse_args()
    need(args.root_authorized_one_session and args.source_commit == COMMIT
         and re.fullmatch('[0-9a-f]{40}',args.source_commit), 'Exact reviewed commit and root session authorization required')
    no_symlinks(LOCK)
    no_symlinks(WORK)
    no_symlinks(REPO)
    no_symlinks(OBJECTS)
    with LOCK.open('a+') as lock:
        fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
        prior = load_json(LEDGER)
        need(prior.get('slot_assigned') is False and prior.get('status') in ('completed','failed'),
             'Central ledger is assigned, blocked or lacks a terminal status')
        eligible = dt.datetime.fromisoformat(prior['next_eligible_utc'])
        finished = dt.datetime.fromisoformat(prior['finished_at_utc'])
        start = now()
        need(eligible.tzinfo is not None and finished.tzinfo is not None
             and eligible >= finished+dt.timedelta(hours=1) and start >= eligible,
             'Centralized hourly gate is unavailable; no waiting or execution')
        need(not WORK.exists() and not WORK.is_symlink(), 'Session directory already exists; no retries or overwrite')
        need(OBJECTS.is_dir(), 'Expected existing dependency/object cache is unavailable')
        WORK.mkdir(mode=0o700)
        state = {'status':'running','slot_assigned':True,'kind':'one authenticated Rider IPUM preparation, integrated Course build and muted Main/Rider19/Course/Back observation',
                 'commit':COMMIT,'started_at_utc':start.isoformat(),'previous':prior,'phases':[],
                 'runtime_launch_attempted':False,'native_build_attempted':False,
                 'one_native_menu_launch_maximum':True,'no_driving_input':True,
                 'runner_sha256':digest(Path(__file__).resolve()),
                 'authoring_status':'Source-only runner; not imported, syntax-checked or executed while authored'}
        session = Session(state)
        session.save()  # Claims slot before any preparation, Git subprocess or compiler.
        old = {n:signal.signal(n,lambda number,frame:setattr(sys.modules[__name__],'INTERRUPTED',number))
               for n in (signal.SIGINT,signal.SIGTERM)}
        success = False
        try:
            enable_subreaper()
            state['subreaper_verified'] = True
            session.save()
            session.execute()
            success = True
        except BaseException as error:
            state['error'] = type(error).__name__+': '+str(error)
        finally:
            DEADLINE = None
            # A phase normally performs cleanup itself. This final guard also
            # covers failures in its cleanup/journal code; never clear a live slot.
            cleanup_error = None
            try:
                for number in (signal.SIGTERM,signal.SIGKILL):
                    until = time.monotonic()+5
                    while time.monotonic() < until:
                        reap_adopted()
                        rows = owned_descendants()
                        if not rows:
                            break
                        signal_owned(rows,number)
                        time.sleep(0.05)
                    if not owned_descendants():
                        break
                reap_adopted()
                remaining = owned_descendants()
            except BaseException as error:
                cleanup_error = str(error)
                remaining = [{'ownership':'unknown'}]
            end = now()
            state['cleanup_checked_at_utc'] = end.isoformat()
            state['remaining_descendants'] = remaining
            if cleanup_error:
                state['cleanup_error'] = cleanup_error
            if remaining or not session.journal_healthy:
                state.update(status='blocked_process_cleanup' if remaining else 'blocked_journal_integrity',
                             slot_assigned=True)
                state.pop('next_eligible_utc',None)
                state['operator_action_required'] = 'Root must resolve cleanup/journal uncertainty before releasing this slot.'
            else:
                state.update(status='completed' if success else 'failed',slot_assigned=False,
                             finished_at_utc=end.isoformat(),
                             next_eligible_utc=(end+dt.timedelta(hours=1)).isoformat())
            try:
                session.save()
            finally:
                for n,handler in old.items():
                    signal.signal(n,handler)
        print(json.dumps({'status':state['status'],'work':str(WORK),'commit':COMMIT,
                          'error':state.get('error'),'next_eligible_utc':state.get('next_eligible_utc')}),flush=True)
        return 0 if state['status'] == 'completed' else 2


if __name__ == '__main__':
    sys.exit(main())
