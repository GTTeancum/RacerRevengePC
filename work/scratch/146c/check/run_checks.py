"""Host check of the add-on course layer (helper 146c): course_check.exe (the game's own
DLC startup pass plus dlc_course_entries50dl, no game run) over the arcade test root,
the RR_TEST_ADDON_COURSE selection, no DLC folder, and broken copies of one course
package. dlc_pack.py check must give the same verdict on every broken copy.
Broken copies: work\\scratch\\dlc\\roots\\addon-courses-146c-broken\\<case>\\DLC (copies only;
the test root is never modified).
"""
import json, os, re, shutil, subprocess, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
EXE = HERE / 'course_check.exe'
STOCK = r'C:\Games\Star Wars Racer Revenge'
ROOTS = Path(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots')
ALL = ROOTS / 'addon-arcade-146' / 'DLC'
BROKEN = ROOTS / 'addon-courses-146c-broken'
PACK = HERE.parents[3] / 'tools' / 'dlc' / 'dlc_pack.py'


def run(root, label, extra=None):
    env = dict(os.environ, RR_ORIGINAL_ASSET_ROOT=STOCK, RR_DLC_ROOT=str(root))
    env.pop('RR_TEST_ADDON_COURSE', None)
    env.update(extra or {})
    r = subprocess.run([str(EXE)], env=env, capture_output=True, text=True, errors='replace')
    print(f'===== {label} (exit {r.returncode})')
    out = r.stderr + r.stdout
    print(out.strip())
    return r.returncode, out


def edit_json(pkg, fn):
    p = pkg / 'dlc.json'
    d = json.loads(p.read_bytes().decode('utf-8'))
    fn(d)
    p.write_bytes((json.dumps(d, indent=1, ensure_ascii=False) + '\n').encode('utf-8'))


def regen(pkg):
    subprocess.run([sys.executable, '-I', str(PACK), 'files', str(pkg)], check=True, capture_output=True)


def case(name, setup, expect, package='arcade-etti-iv'):
    root = BROKEN / name / 'DLC'
    if root.parent.exists():
        shutil.rmtree(root.parent)
    pkg = root / package
    shutil.copytree(ALL / package, pkg)
    setup(pkg)
    code, out = run(root, 'broken: ' + name)
    m = re.search(r'dlc_package id=' + re.escape(package) + r' .*status=refused.* reason="([^"]*)"', out)
    game = bool(m) and expect in m.group(1)
    r = subprocess.run([sys.executable, '-I', str(PACK), 'check', str(root), '--stock', STOCK], capture_output=True, text=True, errors='replace')
    tool = r.returncode != 0 and expect in r.stdout
    print('dlc_pack:', r.stdout.strip().splitlines()[0] if r.stdout.strip() else r.stderr.strip()[:300])
    print(f'CASE {name}: {"PASS" if game and tool else "FAIL"} (game={int(game)} dlc_pack={int(tool)}; expected {expect!r})')
    return game and tool


def main():
    results = []
    code, out = run(ALL, 'arcade root (7 pods, 5 courses)')
    keys = re.findall(r'^dlc_course key=(\S+) number=(\d+) code=(\S+)', out, re.M)
    ok = code == 0 and 'check result=ok' in out and len(keys) == 5 and \
        [k[1] for k in keys] == ['32', '33', '34', '35', '36'] and \
        all(f'code={c} ' in out for c in ('eb', 'lc', 'nb', 'ns', 'nv')) and \
        out.count('tuning=inherited:TRACKS/TB.RES#tracks\\tb.tun') == 5 and \
        out.count('particles=inherited:TRACKS/TA.RES') == 5 and out.count('intro=accepted_no_lines') == 5 and \
        out.count('loading=file:') == 5 and out.count(' route=none name=') == 5 and \
        out.count('modes=single,time_trial background=stock:tb') == 5 and out.count('background_borrowed=1') == 5
    print(f'CASE arcade-root: {"PASS" if ok else "FAIL"}'); results.append(ok)
    key = 'n64-vengeance/nv'
    code, out = run(ALL, 'test course ' + key, {'RR_TEST_ADDON_COURSE': key})
    ok = code == 0 and re.search(r'dlc_course_test key=n64-vengeance/nv number=\d+ code=nv status=selected route=none launch=refused', out) \
        and re.search(r'check test_course=3\d', out)
    print(f'CASE test-course: {"PASS" if ok else "FAIL"}'); results.append(bool(ok))
    code, out = run(ALL, 'test course unknown', {'RR_TEST_ADDON_COURSE': 'arcade-etti-iv/zz'})
    ok = code == 0 and 'dlc_course_test key=arcade-etti-iv/zz status=refused' in out and 'check test_course=0' in out
    print(f'CASE test-course-unknown: {"PASS" if ok else "FAIL"}'); results.append(ok)
    code, out = run(ROOTS / 'no-such-root-146c', 'no DLC folder', {'RR_TEST_ADDON_COURSE': key})
    ok = code == 0 and len(re.findall(r'^dlc_', out, re.M)) == 2 and 'packages=0' in out and \
        'dlc_course_test key=n64-vengeance/nv status=refused' in out and ' controls=0 ' in out and 'dlc_course key' not in out
    print(f'CASE no-dlc: {"PASS" if ok else "FAIL"} (dlc_root line plus the refused test selection only)'); results.append(ok)
    code, out = run(ROOTS / 'no-such-root-146c', 'no DLC folder, no test variable')
    ok = code == 0 and len(re.findall(r'^dlc_', out, re.M)) == 1 and 'packages=0' in out
    print(f'CASE no-dlc-silent: {"PASS" if ok else "FAIL"} (only the dlc_root line)'); results.append(ok)

    def short_loading(pkg):
        f = pkg / 'UI/TEXTURES/LD_EB.PST'; f.write_bytes(f.read_bytes()[:-1]); regen(pkg)
    def wide_loading(pkg):
        f = pkg / 'UI/TEXTURES/LD_EB.PST'; b = bytearray(f.read_bytes()); b[1040:1044] = (512).to_bytes(4, 'little'); f.write_bytes(bytes(b)); regen(pkg)
    def preview_wrong_size(pkg):   # the 640x448 loading screen offered as the 256x256 hologram preview
        shutil.copyfile(pkg / 'UI/TEXTURES/LD_EB.PST', pkg / 'UI/TEXTURES/HOLO_EB.PST'); regen(pkg)
    def preview_not_pst(pkg):
        f = pkg / 'UI/TEXTURES/HOLO_EB.PST'; b = bytearray(f.read_bytes()); b[0] = 9; f.write_bytes(bytes(b)); regen(pkg)
    def background_wrong(pkg):   # the 256x256 preview offered as the full-screen background
        shutil.copyfile(pkg / 'UI/TEXTURES/HOLO_EB.PST', pkg / 'UI/TEXTURES/BKG_EB.PST'); regen(pkg)
        edit_json(pkg, lambda d: d['items'][0].update(course_background={'file': 'UI/TEXTURES/BKG_EB.PST'}))
    def background_stock_bad(pkg):
        edit_json(pkg, lambda d: d['items'][0].update(course_background='stock:zz'))
    def mode_circuit(pkg):
        edit_json(pkg, lambda d: d['items'][0].update(modes=['single', 'vs', 'circuit']))
    def no_inherit(pkg):
        edit_json(pkg, lambda d: d['items'][0].pop('inherit'))
    cases = [('loading-short', short_loading, 'is not the 640x448 one-level layout the loading presenter reads'),
             ('loading-wide', wide_loading, 'is not the 640x448 one-level layout the loading presenter reads'),
             ('preview-wrong-size', preview_wrong_size, 'the stock texture it stands in for (ui\\textures\\holo_tb.pst) is kind 1, 1 level(s), 256x256'),
             ('preview-not-pst', preview_not_pst, 'is not a PST texture'),
             ('no-inherit', no_inherit, 'no tracks\\eb.tun and no inherit.tuning'),
             ('background-wrong', background_wrong, 'is not the 640x448 one-level layout of the stock course-screen backgrounds'),
             ('background-stock-bad', background_stock_bad, 'course_background stock must be a stock course code'),
             ('mode-circuit', mode_circuit, 'mode circuit needs a later version of the port')]
    for name, fn, expect in cases:
        results.append(case(name, fn, expect))
    # Own background file in the stock layout and modes with vs: admitted.
    root = BROKEN / 'good-background' / 'DLC'
    if root.parent.exists():
        shutil.rmtree(root.parent)
    pkg = root / 'arcade-etti-iv'
    shutil.copytree(ALL / 'arcade-etti-iv', pkg)
    shutil.copyfile(pkg / 'UI/TEXTURES/LD_EB.PST', pkg / 'UI/TEXTURES/BKG_EB.PST'); regen(pkg)
    edit_json(pkg, lambda d: d['items'][0].update(course_background={'file': 'UI/TEXTURES/BKG_EB.PST'}, modes=['single', 'vs']))
    code, out = run(root, 'own background + vs')
    ok = code == 0 and 'course_background=file' in out and \
        'modes=single,vs background=dlc:/arcade-etti-iv/UI/TEXTURES/BKG_EB.PST' in out and 'check result=ok' in out
    print(f'CASE good-background: {"PASS" if ok else "FAIL"}'); results.append(ok)
    print(f'RESULT {sum(results)}/{len(results)} pass')
    return 0 if all(results) else 1


if __name__ == '__main__':
    sys.exit(main())
