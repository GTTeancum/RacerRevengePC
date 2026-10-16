#!/usr/bin/env python3
"""Retain every authentic MA/SA ATM frame and material timing field.

UNEXECUTED source candidate. Uses source-equivalent kind0/indexed/IPUM decoding only in
an authorized central cold-preparation slot. Does not claim live animation.
"""
import argparse
import json
import math
from pathlib import Path
import struct
import sys
sys.dont_write_bytecode = True
from ma_sa_asset_sources import Sources, animation_bindings, need, norm, sha


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--track', choices=('MA', 'SA'), required=True)
    parser.add_argument('--intake', type=Path, required=True)
    parser.add_argument('--output-root', type=Path, required=True)
    args = parser.parse_args()
    sys.path.insert(0, str(args.repo/'tools/assets'))
    from decode_source_textures50fc import parse_resource, decode_indexed, decode_ipu
    originals = Sources(args.intake, args.track)
    track, cfg = args.track, originals.cfg
    importer = json.loads((args.intake/'native-import/import.json').read_bytes())
    imported = {norm(row['source_member']): row for row in importer['members']}
    output = args.output_root/('evidence/atm'+track)
    output.mkdir(parents=True, exist_ok=True)
    generated = args.output_root/'tests/native'
    generated.mkdir(parents=True, exist_ok=True)
    rows, source_rows, frame_rows, animation_rows, first, rgba_bytes = [], [], [], [], 0, 0
    placement = json.loads((args.output_root/('evidence/visual'+track)/'export-placed-normal/export-report.json').read_bytes())
    for source_index, original in enumerate(sorted(cfg['atm_members'], key=lambda row: norm(row['name']))):
        member, name = original['name'], Path(original['path']).stem
        data = originals.read(member)
        frames = parse_resource(member.lower(), data, allow_kind0=True)
        bindings = animation_bindings(imported, name)
        record = dict(member=member, name=name, source_sha256=sha(data), source_bytes=len(data),
                      first_texture_index=first, frame_count=len(frames), material_bindings=bindings, frames=[])
        for binding in bindings:
            need(0 <= binding['initial_frame'] < len(frames), 'Authored animation initial frame extent')
            need(binding['fps'] is not None and math.isfinite(binding['fps']) and 0 < binding['fps'] <= 60,
                 'Explicit supported original animation frame rate required: '+binding['material_member'])
            material = originals.read(binding['material_member'])
            binding.update(material_sha256=sha(material), material_bytes=len(material))
            material_name = Path(binding['material_member']).stem
            binding['placed_spans'] = sum(1
                for instance in placement['world_instances'] for surface in instance['surfaces']
                if norm(surface['material']) == norm(material_name))
            animation_rows.append('    {"%s","%s","%s","%s",%du,%du,%du,%du,%sf,%du},' %
                (material_name,name,binding['material_member'],sha(material),len(material),source_index,
                 binding['material_pass'],binding['initial_frame'],repr(float(binding['fps'])),
                 binding['placed_spans']))
        source_rows.append('    {"%s","%s",%du,%du,%du},' %
            (norm(member),sha(data),len(data),len(frames),first))
        for fi, frame in enumerate(frames):
            mip = frame['mips'][-1]
            if frame['kind'] in (0, 1):
                rgba, yuv_sha = decode_indexed(mip['data'], frame['palette'], frame['kind']), None
            else:
                rgba, yuv_sha = decode_ipu(mip['data'], mip['width'], mip['height'], 'ffmpeg')
            rrtex = struct.pack('<8sII', b'RRTEX001', mip['width'], mip['height'])+rgba
            filename = name.lower()+'-frame'+str(fi)+'-mip'+str(mip['level'])+'.rrtex'
            (output/filename).write_bytes(rrtex)
            rgba_bytes += len(rgba)
            entry = dict(frame=fi, start=frame['start'], bytes=frame['end']-frame['start'],
                pst_sha256=sha(data[frame['start']:frame['end']]), native_sha256=sha(rrtex),
                width=mip['width'], height=mip['height'], mip=mip['level'], kind=frame['kind'],
                output_filename=filename, encoded_sha256=sha(mip['data']), yuv_sha256=yuv_sha)
            record['frames'].append(entry)
            # Frame metadata extends the existing scalar layout with the retained filename.
            # Kind2 is retained as a genuine RRTEX image for a cold-file owner;
            # never pass those frames through native48E's indexed-only decoder.
            frame_rows.append('    {%du,%du,%du,%du,%du,"%s","%s","%s"},' %
                (entry['start'],entry['bytes'],entry['width'],entry['height'],entry['mip'],
                 entry['pst_sha256'],entry['native_sha256'],filename))
        first += len(frames)
        rows.append(record)
    need(len(rows) == len(cfg['atm_members']) and first <= 64*len(rows), 'Complete bounded original animation closure')
    (generated/(track.lower()+'_atm_source_index.inc')).write_text(
        '// Exact original ATM resources and contiguous source-frame bank ranges.\n'+'\n'.join(source_rows)+'\n')
    (generated/(track.lower()+'_atm_animation_index.inc')).write_text(
        '// Original material bindings; shared ATM frame ranges are deliberate.\n'+'\n'.join(animation_rows)+'\n')
    (generated/(track.lower()+'_atm_frame_index.inc')).write_text(
        '// Genuine original embedded PST extents and retained RRTEX identities.\n'+'\n'.join(frame_rows)+'\n')
    (output/'manifest.json').write_text(json.dumps(dict(schema='RR_ORIGINAL_'+track+'_ATM_FRAMES_V1',
        archive_sha256=cfg['archive_sha256'], animations=rows, frames=first, rgba_bytes=rgba_bytes,
        limitations=['All images are source-decoded cold outputs, not a live animation implementation.',
                     'Per-material animation fields remain intact; runtime must honor source controls.',
                     'IPUM uses retained scalar FFmpeg IDCT and is not hardware framebuffer parity.']),
        indent=2, sort_keys=True)+'\n')
    print(json.dumps(dict(track=track, animations=len(rows), frames=first, rgba_bytes=rgba_bytes)))


if __name__ == '__main__':
    main()
