#!/usr/bin/env python3
"""Cold POD07 candidate preparation; never enables Course or Rider admission.

Source-only delivery, not executed by its author. A later authorized parent run
uses a separately reviewed native importer. No disc scan or guest execution.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import stat
import struct
import sys
import types

sys.dont_write_bytecode = True

# Original pins: tests/native/original_player_identity50ik_fixture.h, POD07.
IDENTITY = {
    'resource_bytes': 354874,
    'resource_sha256': '0cde5fac5d9a379e1ca426409809ba5c04eec6895013577f03639c69859fd8a8',
    'phy_sha256': '8615aca792d1efb68abcda048dd2b6cc9207c6c044300bf2306b6618edaffa48',
    'speech_sha256': {
        'curses': 'b64a320ffbe23f0676e327983470076c862b1c6bd40139e895ff82cbd0af3102',
        'taunts': 'd74158fb8097b441968fff62f83c3c5bc51a65aef3fa7d07b982cddf8cc5f952',
        'special': '92e9203f008c2c0afc47087ca72d2b37625a93a542371182facc60a6247ca41d',
        'sounds': 'f0b3b4950bd40f71736964ee6cdd9c6e97424ff84e6b57ca464170cb2ab72be2',
    },
}
UI_SHA = '245b0bd530e6797637ac72bbf2de8f101f3b1da280e00e8fc24e4b4930d79f3b'
DAX_SHA = 'ee0e83e2bc80e896c783d0f0f4058d381677faf8da7ce8f8e9cbfb06671d17e9'
# Exact bytes of the reviewed local original_roster/prepare_original_roster.py.
# Any change (including checkout line endings) requires a new source review.
HELPER_SHA = '5b984aeb41409b735e9820a7bab6365c5336f46250e28f2ebd615a179d002f71'

# Round-two static reads of the original files, not generated output pins.
# Full member/block/mip evidence: docs/recovery/gasgano-original-evidence-round2-20261014.md.
GASGANO_UI_INPUTS = (
    ('UI/UIRIDER/UIRIDER.RES', 5552683, '112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856'),
    ('UI/UIPODTEX.RES', 2653798, 'ffb0f32563e3e5b6e3fefe915bbb37a00610e1a9a44163bce9737c2ac9d6e641'),
    ('UI/PODPHYS.RES', 46563, '8361cb484ea841e1772cc0ddec2223fa923e397c1cc9a89f06ab06b1cd7ca2c8'),
    ('UI/UIBKG.RES', 1330049, '67b7774815bd4352e72103160e323c00e062f2e2845e05e9720b38796185091f'),
    ('AUDIO/SPEECH/UI/GASSEL01.VAG', 26656, '4076d3b4943d5828a0b7977ca0bc9a6e1fdffc1e4a0925f41aed560713183c90'),
    ('AUDIO/SPEECH/UI/GASSEL02.VAG', 26512, 'c647576fb9e438d58b921142c0592041ba2a9ccb0a9db539e0f6ceeb34eb28e9'),
    ('AUDIO/SFX/RACE/GEN2_NGN.VAG', 23888, 'b94dd496dcdb953e60921ee32412ca31ad6f6a2801441828bca34fa1b2e5b895'),
    ('AUDIO/SFX/RACE/AS_BEAM.VAG', 7744, '52f6ed8fd30bacb5d8058da66e611273310ce9bdf99d74161b7689f6d78ec7d4'),
)
SLUS_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
# ELF PT_LOAD file offset 0x80 maps VA 0x100000. Static words, never executed.
VOICE_WORDS = ((0x1E7C7C, 0x24030002), (0x1E7CA8, 0x2410000F),
               (0x1E7CFC, 0xACA353D8), (0x1E7DDC, 0xAC505444))


def gasgano_ui_evidence(root, slus):
    """Authenticate source presence only; this cannot admit native UI outputs."""
    need((root is None) == (slus is None), 'Supply both --gasgano-ui-source-root and --slus')
    if root is None:
        return None
    sources = []
    for relative, count, digest in GASGANO_UI_INPUTS:
        path = root / relative
        exact(path, digest, count)
        sources.append({'disc_path': relative, 'bytes': count, 'sha256': digest})
    raw = exact(slus, SLUS_SHA, 2972720)
    for pc, word in VOICE_WORDS:
        need(struct.unpack_from('<I', raw, pc - 0x100000 + 0x80)[0] == word,
             'Original Gasgano constructor word differs')
    return {
        'sources': sources, 'slus_sha256': SLUS_SHA,
        'constructor_voice': {'racer60': 7, 'first': 15, 'count': 2,
                              'count_store_pc': '0x1e7cfc', 'first_store_pc': '0x1e7ddc'},
        'character_widget': 41, 'character_mcf': 'ui/anim/07c.mcf',
        'required_ipum_source': {
            'member': 'ui/textures/07c_2.pst', 'logical_offset': 6316032,
            'bytes': 29512, 'sha256': 'cbd5e15d11e7b1b9336a1e8f965b18ecdb93d948e770baa2bc3005e3576b11ce',
            'mip_dimensions': [16, 32, 64, 128, 256]},
        'native_ipum_output_authenticated': False,
        'scope': 'Original source presence and constructor evidence; no conversion or runtime admission.',
    }


def need(value, message):
    if not value:
        raise ValueError(message)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def absolute_plain(path):
    """Reject traversal before normalization; Windows uses ordinary local paths."""
    path = Path(path)
    need('..' not in path.parts, 'Parent traversal is forbidden: ' + str(path))
    if os.name == 'nt':
        need(not path.drive or (path.root and re.fullmatch(r'[A-Za-z]:', path.drive)),
             'Use an ordinary local drive path, not drive-relative/UNC/device paths')
        need(not path.root or path.drive, 'Root-relative Windows paths are ambiguous')
        for part in path.parts[1:] if path.anchor else path.parts:
            need(not part.endswith((' ', '.')) and not re.search(r'[<>:"|?*\x00-\x1f]', part) and
                 not re.fullmatch(r'(?i:CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\..*)?', part),
                 'Noncanonical Windows path component: ' + part)
    return path if path.is_absolute() else Path.cwd() / path


def canonical_existing(path, directory=False):
    path = absolute_plain(path)
    # Inspect the lexical chain before resolve can hide a junction or symlink.
    for part in (*reversed(path.parents), path):
        info = part.lstat()
        need(not stat.S_ISLNK(info.st_mode) and
             not (getattr(info, 'st_file_attributes', 0) & 0x400),
             'Link/reparse path is not supported: ' + str(part))
        expected_directory = part != path or directory
        need(stat.S_ISDIR(info.st_mode) if expected_directory else stat.S_ISREG(info.st_mode),
             'Unexpected path type: ' + str(part))
    resolved = path.resolve(strict=True)
    need(resolved == path, 'Path resolves to a different location: ' + str(path))
    return resolved


def fresh_output(path):
    path = absolute_plain(path)
    need(path.name and path != path.parent, 'Output must name a new directory')
    parent = canonical_existing(path.parent, directory=True)
    output = parent / path.name
    try:
        output.lstat()  # Unlike exists(), also detects dangling links/reparse entries.
    except FileNotFoundError:
        return output
    raise ValueError('Output already has a filesystem entry: ' + str(output))


def regular(path):
    return canonical_existing(path).read_bytes()


def exact(path, digest, count=None):
    data = regular(path)
    need((count is None or len(data) == count) and sha(data) == digest,
         'Original identity mismatch: ' + str(path))
    return data


def ui_name(raw):
    # Same original UI-comment association as select_original_roster.py;
    # an event seen here is NOT proof of the Rider constructor's voice range.
    comment, offset, names = None, 0, []
    for line in raw.splitlines(keepends=True):
        text = line.decode('ascii').strip()
        if text.startswith(';'):
            comment = (text[1:].strip(), offset)
        if re.fullmatch(r'FileName\s*=\s*speech\\UI\\gassel\d+\.vag', text, re.I):
            need(comment is not None, 'Missing original Gasgano UI comment')
            names.append(comment)
        offset += len(line)
    need(names and len({name for name, at in names}) == 1,
         'Missing/ambiguous original Gasgano UI identity')
    return names[0]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--resource', type=Path, required=True, help='Exact POD07.RES')
    parser.add_argument('--uiaccept', type=Path, required=True, help='Exact AUDIO/UIACCEPT.INI')
    parser.add_argument('--announcer', type=Path, required=True, help='Exact POD07ANN.INI')
    parser.add_argument('--importer', type=Path, required=True, help='Reviewed native host importer')
    parser.add_argument('--importer-sha256', required=True, help='Parent-reviewed importer binary SHA-256')
    parser.add_argument('--output', type=Path, required=True, help='New directory under an existing parent')
    parser.add_argument('--gasgano-ui-source-root', type=Path,
                        help='Optional exact original UI/audio source root; no directory scan')
    parser.add_argument('--slus', type=Path,
                        help='Original SLUS_202.68 for static identity/word reads only; never executed')
    parser.add_argument('--execute-cold-preparation', action='store_true')
    args = parser.parse_args()
    need(args.execute_cold_preparation, 'Explicit later cold-preparation authorization required')
    need(re.fullmatch(r'[0-9a-f]{64}', args.importer_sha256), 'Invalid importer SHA-256')
    output = fresh_output(args.output)
    inputs = [canonical_existing(p) for p in
              (args.resource, args.uiaccept, args.announcer, args.importer)]
    resource, ui, announcer, importer = inputs
    need(resource.name.upper() == 'POD07.RES', 'Only original POD07 is supported')
    source = exact(resource, IDENTITY['resource_sha256'], IDENTITY['resource_bytes'])
    ui_raw = exact(ui, UI_SHA)
    dax_raw = exact(announcer, DAX_SHA, 2496)
    exact(importer, args.importer_sha256)
    display_name, comment_offset = ui_name(ui_raw)
    ui_root = (canonical_existing(args.gasgano_ui_source_root, directory=True)
               if args.gasgano_ui_source_root is not None else None)
    slus = canonical_existing(args.slus) if args.slus is not None else None
    ui_evidence = gasgano_ui_evidence(ui_root, slus)
    helper_path = canonical_existing(Path(__file__).parent / 'original_roster/prepare_original_roster.py')
    helper_bytes = exact(helper_path, HELPER_SHA)  # Before any helper code executes.
    protected = [p.parent for p in (*inputs, helper_path)]
    if slus is not None:
        protected.append(slus.parent)
    if ui_root is not None:
        protected.append(ui_root)
    need(all(output != p and p not in output.parents and output not in p.parents for p in protected),
         'Output must be separate from input/helper directories')
    need(fresh_output(output) == output, 'Output path changed')
    output.mkdir()  # No parents/exist_ok: reserve only the verified fresh leaf.
    output_identity = output.lstat()

    def retain_output():
        need(canonical_existing(output, directory=True) == output, 'Output path changed')
        current = output.lstat()
        need((current.st_dev, current.st_ino) == (output_identity.st_dev, output_identity.st_ino),
             'Output directory was replaced')

    retain_output()
    # Execute exactly the authenticated source buffer, never a cached .pyc or
    # a second on-disk source read. This occurs only in an authorized future run.
    helper = types.ModuleType('pod07_original_preparer')
    helper.__file__ = str(helper_path)
    exec(compile(helper_bytes, str(helper_path), 'exec'), helper.__dict__)
    retain_output()
    exact(importer, args.importer_sha256)
    pod = helper.prepare_pod(importer, resource, output / 'pod07', {'POD07': IDENTITY})
    retain_output()
    need(pod['pod_id'] == 'POD07' and pod['name'] == 'Gasgano', 'Original PHY name differs')
    need([r['role'] for r in pod['components']] == ['Chariot', 'LeftEngine', 'RightEngine'],
         'Unexpected original POD07 component topology')
    need(helper.norm(pod['phy']['source_member']) == 'pods/pod07/pod07.phy',
         'Unexpected original PHY member')
    report = json.loads(regular(output / 'pod07/import.json'))
    taunts = [r for r in report['members'] if helper.norm(r['source_member']) ==
              'audio/speech/pod07/taunts.ini']
    need(len(taunts) == 1, 'Missing original POD07 taunts')
    taunts_file = Path(taunts[0]['source_file'])
    need(not taunts_file.anchor and len(taunts_file.parts) == 1 and taunts_file.name not in ('.', '..'),
         'Expected a flat retained speech filename')
    taunts_path = output / 'pod07' / taunts_file
    taunts_raw = exact(taunts_path, IDENTITY['speech_sha256']['taunts'])
    prefixes = set(re.findall(rb'FileName\s*=\s*speech\\POD07\\([a-z0-9]{3})[a-z0-9_]+\.vag',
                              taunts_raw, re.I))
    need({p.lower() for p in prefixes} == {b'gas'}, 'Original speech/UI name join differs')
    need(regular(resource) == source, 'Original resource changed')
    exact(importer, args.importer_sha256)
    need(exact(helper_path, HELPER_SHA) == helper_bytes, 'Preparer source changed')
    retain_output()
    provenance = output / 'provenance'
    provenance.mkdir()
    for name, raw in [('UIACCEPT.INI', ui_raw), ('POD07ANN.INI', dax_raw), ('POD07-TAUNTS.INI', taunts_raw)]:
        with (provenance / name).open('xb') as stream:
            stream.write(raw)
    candidate = {
        'schema': 'RR_ADDITIONAL_ORIGINAL_RACER_CANDIDATE_20261014',
        'racer60': 7, 'pod_id': 'POD07', 'path': 'pod07/pod.json',
        'resource_sha256': IDENTITY['resource_sha256'], 'phy_sha256': IDENTITY['phy_sha256'],
        'display_name': display_name, 'ui_comment_byte_offset': comment_offset,
        'ui_sha256': UI_SHA, 'announcer_sha256': DAX_SHA, 'announcer_events': 30,
        'importer_sha256': args.importer_sha256, 'preparer_sha256': sha(helper_bytes),
        'preparer_expected_sha256': HELPER_SHA, 'output_directory': str(output),
        'course_admitted': False, 'rider_ui_ready': False, 'runtime_validated': False,
        'unresolved_texture_bindings': pod['unresolved_texture_bindings'],
        'gasgano_ui_source_evidence': ui_evidence,
        'pending': ['Authenticate native Rider07 image/material/stat/model owners, including 07c_2 IPUM output.',
                    'Parent must integrate proven Rider07 voice first=15/count=2 only after destination closure.',
                    'Parent must integrate and validate original rig/COL/handling/audio/UI owners.',
                    'Retain original AI for every entrant, including followed slot0.'],
    }
    # This receipt is not roster.json and cannot accidentally admit a singleton field.
    retain_output()
    with (output / 'candidate.json').open('x', encoding='utf-8') as stream:
        json.dump(candidate, stream, indent=2)
        stream.write('\n')


if __name__ == '__main__':
    main()
