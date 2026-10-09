"""Inventory every TA PSG using the frozen 48B decoder; do not widen support.

Merge only successfully decoded identity-transform, world-space meshes at their
stored positions. The aggregate is a neutral-material diagnostic, not a recovered
complete scene or validated track placement/visibility implementation.
"""
from pathlib import Path
import argparse
import collections
import hashlib
import json
import struct
import sys

sys.dont_write_bytecode = True
from decode_static48b import ROOT, decode_psg3, mesh_bytes, FormatError, require, sha

EXPECTED_DECODER = '875946365699396aaa0dd594a8e2b9cf1c4319cfc6079c53d686db98b998668a'
EXPECTED_TA = 'b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--container', type=Path, default=ROOT/'work/native/project/disc/TRACKS/TA.RES')
    parser.add_argument('--output', type=Path, default=ROOT/'evidence/assets48B-inventory')
    args = parser.parse_args()
    decoder_path = Path(__file__).with_name('decode_static48b.py')
    decoder_hash = sha(decoder_path.read_bytes())
    require(decoder_hash == EXPECTED_DECODER, '48B decoder differs from frozen source')
    require(28 <= args.container.stat().st_size <= 64*1024*1024, 'container size')
    raw = args.container.read_bytes()
    require(sha(raw) == EXPECTED_TA, 'inventory restricted to verified original TA.RES')
    sys.path.insert(0, str(ROOT/'work/native/project/scripts'))
    try:
        from validate_ui_resources07 import decode
    finally:
        sys.path.pop(0)
    records, blocks = decode(raw)
    flat = b''.join(b['data'] for b in blocks)
    meshes = [r for r in records if r['name'].lower().endswith('.psg')]
    require(len(meshes) == 230, 'original TA inventory count changed')
    args.output.mkdir(parents=True, exist_ok=True)
    per_mesh_dir = args.output/'decoded'
    per_mesh_dir.mkdir(exist_ok=True)
    accepted, rejected, spans = [], [], []
    aggregate_vertices, aggregate_indices = [], []
    totals = collections.Counter()
    for ordinal, record in enumerate(meshes):
        begin, count = record['logical_offset'], record['bytes']
        data = flat[begin:begin+count]
        row = {'ordinal': ordinal, **record, 'sha256': sha(data),
               'magic_version_hex': data[:8].hex()}
        # This peek is evidence only. A failed full decode never gains eligibility
        # for merging based on fields that merely happen to resemble this subset.
        if len(data) >= 12 and data[:8] == b'psg\0\x03\0\0\0':
            row['header_object_count'] = struct.unpack_from('<I',data,8)[0]
            if row['header_object_count'] == 1 and len(data) >= 336:
                row['observed_one_object_matrix'] = struct.unpack_from('<16f',data,140)
                row['observed_one_object_bounds'] = struct.unpack_from('<7fi',data,204)
                row['observed_material_count'] = struct.unpack_from('<I',data,236)[0]
                if row['observed_material_count'] == 1:
                    row['observed_world_flag_at332'] = struct.unpack_from('<I',data,332)[0]
        try:
            vertices, indices, info = decode_psg3(data)
        except FormatError as exc:
            row['status'] = 'rejected by frozen decoder'
            row['reason'] = str(exc)
            rejected.append(row)
            totals['rejected'] += 1
            continue
        world = info['quantization'][7]
        row.update({'status': 'decoded candidate', 'world_space_flag': world,
                    'vertices': len(vertices), 'indices': len(indices),
                    'triangles': len(indices)//3, 'strips': info['strips'],
                    'material': info['material'], 'bounds_error': info['max_bounds_error'],
                    'stored_bounds': info['stored_bounds_center_half_radius_parent'],
                    'decoded_bounds_min': info['decoded_bounds_min'],
                    'decoded_bounds_max': info['decoded_bounds_max'],
                    'normal_alignment': info['normal_alignment']})
        blob = mesh_bytes(vertices, indices)
        target = per_mesh_dir/f'{ordinal:03d}.rrmesh'
        target.write_bytes(blob)
        require(target.read_bytes() == blob, 'candidate output readback mismatch')
        row['output'] = {'path': str(target.resolve()), 'bytes':len(blob), 'sha256':sha(blob)}
        # Retain a complete parse report for accepted candidates, including exact
        # command extents, strip starts and original decoded normals.
        detail_path = per_mesh_dir/f'{ordinal:03d}.json'
        detail_path.write_text(json.dumps({'source':row, 'decoded':info},indent=2)+'\n')
        row['detail_path'] = str(detail_path.resolve())
        accepted.append(row)
        totals['decoded'] += 1
        totals['decoded_vertices'] += len(vertices)
        totals['decoded_triangles'] += len(indices)//3
        if world != 1:
            row['aggregate_disposition'] = 'excluded: local-space mesh has no validated scene placement'
            totals['local_excluded'] += 1
            continue
        require(len(aggregate_vertices)+len(vertices) <= 1_000_000, 'aggregate vertex limit')
        require(len(aggregate_indices)+len(indices) <= 3_000_000, 'aggregate index limit')
        base_v, base_i = len(aggregate_vertices), len(aggregate_indices)
        aggregate_vertices.extend(vertices)
        aggregate_indices.extend(i+base_v for i in indices)
        row['aggregate_disposition'] = 'included: original world-space positions, identity object matrix'
        spans.append({'member': record['name'], 'member_sha256':sha(data),
                      'logical_offset':begin, 'source_bytes':count,
                      'source_mesh_sha256':sha(blob), 'vertex_start':base_v,
                      'vertex_count':len(vertices), 'index_start':base_i,
                      'index_count':len(indices), 'material':info['material'],
                      'neutral_view':True})
        totals['world_included'] += 1
    require(len(accepted)+len(rejected) == len(meshes), 'inventory coverage mismatch')
    require(aggregate_vertices and spans, 'no world-space meshes accepted')
    combined = mesh_bytes(aggregate_vertices, aggregate_indices)
    # Verify each concatenated vertex byte span against its standalone decoder
    # output; verify index relocation independently from the concatenation loop.
    index_payload_offset = 16 + 24*len(aggregate_vertices)
    for span in spans:
        row = next(r for r in accepted if r['name']==span['member'])
        standalone = Path(row['output']['path']).read_bytes()
        vs, vc = span['vertex_start'],span['vertex_count']
        ixs, ic = span['index_start'],span['index_count']
        require(combined[16+24*vs:16+24*(vs+vc)] == standalone[16:16+24*vc],
                'aggregate changed original vertex bytes')
        for i in range(ic):
            got = struct.unpack_from('<I',combined,index_payload_offset+4*(ixs+i))[0]
            original = struct.unpack_from('<I',standalone,16+24*vc+4*i)[0]
            require(got == original+vs and vs <= got < vs+vc,
                    'aggregate changed connectivity or referenced another mesh')
    combined_path = args.output/'world-static-neutral.rrmesh'
    combined_path.write_bytes(combined)
    require(combined_path.read_bytes()==combined, 'aggregate readback mismatch')
    totals['aggregate_vertices'] = len(aggregate_vertices)
    totals['aggregate_indices'] = len(aggregate_indices)
    totals['aggregate_triangles'] = len(aggregate_indices)//3
    counts = collections.Counter(r['reason'].split(':',1)[0] for r in rejected)
    report = {
        'status':'diagnostic coverage only',
        'container':{'path':str(args.container.resolve()),'sha256':sha(raw),
                     'bytes':len(raw),'decoded_bytes':len(flat)},
        'frozen_decoder':{'path':str(decoder_path.resolve()),'sha256':decoder_hash},
        'inventory_tool_sha256':sha(Path(__file__).read_bytes()),
        'psg_count':len(meshes),'totals':dict(totals),'rejection_reason_counts':dict(counts),
        'accepted_candidates':accepted,'rejected':rejected,
        'aggregate':{'path':str(combined_path.resolve()),'sha256':sha(combined),'bytes':len(combined),
                     'bounds_min':[min(v[j] for v in aggregate_vertices) for j in range(3)],
                     'bounds_max':[max(v[j] for v in aggregate_vertices) for j in range(3)],
                     'member_spans':spans,
                     'verification':'Every vertex byte equals its standalone decoded output; every index relocation preserves within-member connectivity.'},
        'limitations':[
            'Frozen decoder unchanged; rejected variants are unsupported, not proven corrupt.',
            'No local-space props included without validated placement.',
            'No runtime scene instances, visibility, collision, animation, sky or material rendering recovered by this merge.',
            'Neutral material is an inspection convention; original per-vertex color bytes remain unmodified.',
            'Original world-space flags plus identity matrices authorize only this diagnostic aggregate; complete scene placement not accepted.',
            'Aggregate rendering or timing is not native gameplay acceptance or FPS.'
        ]}
    (args.output/'inventory.json').write_text(json.dumps(report,indent=2)+'\n')
    summary = ['# TA static geometry diagnostic coverage','',
               'Uses the frozen 48B decoder unchanged. Rejection means unsupported by that bounded subset, not corrupt original data.','',
               f"Inspected {len(meshes)} original TA PSG members; decoded {totals['decoded']}, rejected {totals['rejected']}.",
               f"Aggregate includes {totals['world_included']} world-space meshes; excludes {totals['local_excluded']} decoded local-space props.",
               f"Aggregate: {len(aggregate_vertices)} vertices, {len(aggregate_indices)//3} triangles. All positions and topology remain the decoded originals.",'',
               '## Rejection counts','']
    summary += [f'- {n}: {reason}' for reason,n in counts.most_common()]
    summary += ['', '## Scope',''] + ['- '+s for s in report['limitations']]
    (args.output/'README.md').write_text('\n'.join(summary)+'\n')
    print(json.dumps({'psg_count':len(meshes),'totals':dict(totals),
                      'rejection_reason_counts':dict(counts),'aggregate':str(combined_path.resolve()),
                      'aggregate_sha256':sha(combined)}))


if __name__ == '__main__':
    main()
