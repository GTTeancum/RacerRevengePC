"""Host check of the DLC registry over the test root and broken copies (helper 146p).
Runs registry_check.exe (the game's own startup pass, no game run) once per root.
Broken copies are made under work\\scratch\\dlc\\roots\\addon-pods-146-broken\\<case>\\DLC
from copies of the test root (the test root and addon-out are never modified).
"""
import json, os, re, shutil, subprocess, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
EXE = HERE / 'registry_check.exe'
STOCK = r'C:\Games\Star Wars Racer Revenge'
ROOTS = Path(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots')
GOOD = ROOTS / 'addon-arcade-pods-146' / 'DLC'  # copies of racer-arcade dlc-addons (7 pods)
ALL = ROOTS / 'addon-arcade-146' / 'DLC'  # copies of racer-arcade dlc-addons (all 12)
BUILT = [ROOTS / 'addon-pods-146' / 'DLC', ROOTS / 'addon-all-146' / 'DLC']  # make_addon_root.py output
BROKEN = ROOTS / 'addon-pods-146-broken'
PACK = Path(r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a061eb9837b6bc01f\tools\dlc\dlc_pack.py')


def run(root, label):
    env = dict(os.environ, RR_ORIGINAL_ASSET_ROOT=STOCK, RR_DLC_ROOT=str(root))
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


def rename_member(archive, old, new):
    assert len(old) == len(new)
    b = archive.read_bytes()
    o = old.encode('ascii')
    assert b.count(o) == 1, (archive, old)
    archive.write_bytes(b.replace(o, new.encode('ascii')))


def case(name, setup, expect):
    root = BROKEN / name / 'DLC'
    if root.exists():
        shutil.rmtree(root.parent)
    pkg = root / 'arcade-ark-roose'
    shutil.copytree(GOOD / 'arcade-ark-roose', pkg)
    setup(root, pkg)
    code, out = run(root, 'broken: ' + name)
    refused = re.search(r'dlc_package id=arcade-ark-roose\S* .*status=refused.* reason="([^"]*)"', out) or \
        re.search(r'dlc_package id=\S+ .*status=refused.* reason="([^"]*)"', out)
    ok = bool(refused) and expect in refused.group(1)
    print(f'CASE {name}: {"PASS" if ok else "FAIL"} (expected a refusal containing {expect!r})')
    return ok


def main():
    results = []
    code, out = run(GOOD, 'test root (7 add-on pods)')
    nums = sorted(int(n) for n in re.findall(r'check pod number=(\d+)', out))
    ok = code == 0 and nums == list(range(32, 39)) and 'dlc_registry packages=7 refused=0 courses=0 pods=7' in out
    print(f'CASE good-root: {"PASS" if ok else "FAIL"} (numbers {nums})'); results.append(ok)
    if ALL.exists():
        code, out = run(ALL, 'all add-on packages (7 pods, 5 courses)')
        ok = code == 0 and 'pods=7' in out and 'courses=5' in out
        print(f'CASE all-root: {"PASS" if ok else "FAIL"}'); results.append(ok)
    for b in BUILT:
        if b.exists():
            code, out = run(b, 'builder root ' + b.parent.name)
            ok = code == 0 and 'refused=0' in out
            print(f'CASE {b.parent.name}: {"PASS" if ok else "FAIL"}'); results.append(ok)
    env_key = 'arcade-neva-kee/a2r-nev'
    os.environ['RR_TEST_ADDON_RIDER'] = env_key
    code, out = run(GOOD, 'test rider ' + env_key)
    del os.environ['RR_TEST_ADDON_RIDER']
    ok = code == 0 and f'dlc_rider_test key={env_key} number=36 status=selected' in out and 'test_rider=36' in out
    print(f'CASE test-rider: {"PASS" if ok else "FAIL"}'); results.append(ok)
    code, out = run(ROOTS / 'no-such-root-146', 'no DLC folder')
    ok = code == 0 and len(re.findall(r"^dlc_", out, re.M)) == 1 and 'packages=0' in out and 'admitted_packages=0' in out
    print(f'CASE no-dlc: {"PASS" if ok else "FAIL"} (only the dlc_root line)'); results.append(ok)

    def bad_stem(root, pkg):
        edit_json(pkg, lambda d: d['items'][0].update(stem='pod04'))
    def other_stem_member(root, pkg):
        rename_member(pkg / 'PODS/A2R_ARK/A2R_ARK.RES', 'Pods\\a2r_ark\\a2r_ark.ai', 'Pods\\a2r_arx\\a2r_ark.ai'); regen(pkg)
    def missing_member(root, pkg):
        edit_json(pkg, lambda d: d['items'][0]['ui'].update(results_icon='ui\\textures\\a2r_ark_none.pst'))
    def unnamed_member(root, pkg):
        edit_json(pkg, lambda d: d['items'][0]['ui']['pod_pages'].pop())
    def foreign_material(root, pkg):
        rename_member(pkg / 'PODS/A2R_ARK/A2R_ARK.RES', 'textures\\a2r_ark_3.psm', 'textures\\gcz_xyz_3.psm'); regen(pkg)
    def foreign_ui_material(root, pkg):
        rename_member(pkg / 'UI/A2R_ARK_UI.RES', 'ui\\textures\\a2r_ark_shdw.psm', 'ui\\textures\\04_shdw_arkx.psm'); regen(pkg)
        edit_json(pkg, lambda d: d['items'][0]['ui'].update(shadow=['ui\\textures\\a2r_ark_shdw.pst', 'ui\\textures\\04_shdw_arkx.psm']))
    def manifest_shipped(root, pkg):
        shutil.copyfile(Path(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\addon-out\arcade-ark-roose\addon-manifest.json'),
                        pkg / 'addon-manifest.json')
    def stem_twice(root, pkg):
        other = root / 'zz-ark-copy'
        shutil.copytree(pkg, other)
        edit_json(other, lambda d: d.update(id='zz-ark-copy'))
    def old_contract(root, pkg):
        edit_json(pkg, lambda d: d.update(requires_port='dlc-contract-1'))
    def anywhere_ai(root, pkg):
        edit_json(pkg, lambda d: d['items'][0].update(ai='anywhere'))
    def no_name_string(root, pkg):
        edit_json(pkg, lambda d: d.update(strings={'english': {}}))
    cases = [('bad-stem', bad_stem, 'not pod plus two digits'),
             ('member-of-another-stem', other_stem_member, 'named for another pod'),
             ('missing-member', missing_member, 'lacks member ui\\textures\\a2r_ark_none.pst'),
             ('unnamed-menu-member', unnamed_member, 'is not named by any ui field'),
             ('foreign-material', foreign_material, 'neither a shared original name nor named with the stem'),
             ('foreign-menu-material', foreign_ui_material, 'neither a shared original name nor named with the stem'),
             ('manifest-shipped', manifest_shipped, 'not listed in dlc.json'),
             ('stem-twice', stem_twice, 'pod stem a2r_ark is already used by package arcade-ark-roose'),
             ('contract-1', old_contract, 'dlc-contract-2'),
             ('ai-anywhere', anywhere_ai, 'needs a later version'),
             ('no-name-string', no_name_string, 'has no English string')]
    for n, s, e in cases:
        results.append(case(n, s, e))
    print(f'SUMMARY {sum(results)}/{len(results)} passed')
    return 0 if all(results) else 1


if __name__ == '__main__':
    sys.exit(main())
