#!/usr/bin/env python3
"""Production conversion of the authenticated original GC opening PSG.

Uses the staged bounded signed24-in-V4_32 extension. No software tests,
emulator/guest execution or native client runs are performed.
"""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import sys

sys.dont_write_bytecode = True


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--repo', type=Path, required=True)
    p.add_argument('--source', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    args = p.parse_args()
    original = args.source.read_bytes()
    if sha(original) != '5070ad1c3d7435233ff14bf71aae91bc65fa65d9829d9cdeb5b58659756b0af6':
        raise ValueError('Exact GC original opening source required')
    sys.path.insert(0, str(args.repo/'tools/assets'))
    decoder_path = Path(__file__).with_name('decode_scene48c.py')
    spec = importlib.util.spec_from_file_location('gc_signed32_scene', decoder_path)
    decoder = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(decoder)
    scene = decoder.decode_psg3_scene(original, allow_signed32_quantized_positions=True)
    vertices, indices, spans = decoder.export_lod(scene)
    mesh = decoder.mesh_bytes(vertices, indices)
    native = bytearray(b'RRSCENE1')

    def u32(value):
        native.extend(struct.pack('<I', value))

    def doubles(values):
        native.extend(struct.pack('<'+str(len(values))+'d', *values))

    def text(value):
        raw = value.encode('ascii')
        u32(len(raw))
        native.extend(raw)

    # Exact established encodeSceneAsset48D serialization. The original source
    # hierarchy, bone IDs, normals, UVs and absent-color semantics are retained.
    for value in (1, scene['quantization'][7], bool(scene['matrix_palette']), len(scene['nodes']),
                  len(scene['materials']), len(scene['lods']), scene['decoded_vertices_all_lods'],
                  scene['surfaces_all_lods']):
        u32(value)
    doubles(scene['quantization'][:7])
    for node in scene['nodes']:
        text(node['name'])
        u32(node['parent'] & 0xffffffff)
        doubles(node['matrix'])
        doubles(node['bind_matrix'])
        doubles(node['bounds_center_half_radius'])
    for material in scene['materials']:
        text(material)
    for lod in scene['lods']:
        doubles([lod['cutoff']])
        u32(len(lod['surfaces']))
        for surface in lod['surfaces']:
            u32(surface['material_index'])
            u32(surface['original_colors'][0] is not None)
            doubles([surface['uv_size']])
            u32(len(surface['vertices_local']))
            u32(len(surface['indices']))
            for v, normal, bone, colors in zip(surface['vertices_local'], surface['normals_local'],
                                                surface['bone_indices'], surface['original_colors']):
                doubles(v[:3])
                doubles(normal)
                doubles(v[3:5])
                native.extend(bytes(colors if colors is not None else (0, 0, 0, 0)))
                native.extend(struct.pack('<H', bone))
            for index in surface['indices']:
                u32(index)
    u32(len(spans))
    for span in spans:
        for name in ('material_index', 'vertex_start', 'vertex_count', 'index_start', 'index_count', 'original_vertex_colors'):
            u32(span[name])
    u32(len(mesh))
    native.extend(mesh)
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output/'opening.rrscene').write_bytes(native)
    (args.output/'opening.rrmesh').write_bytes(mesh)
    source_report = dict(source_sha256=sha(original), source_bytes=len(original), source_member='anim\\opening.psg',
                         decoder_sha256=sha(decoder_path.read_bytes()), scene_sha256=sha(native), mesh_sha256=sha(mesh),
                         nodes=len(scene['nodes']), vertices=len(vertices), indices=len(indices), spans=spans,
                         bounds_checks=scene['bounds_checks'], quantization=scene['quantization'],
                         native_validation='Not compiled or loaded with the staged C++ parser; root owns that validation.',
                         scene=scene)
    (args.output/'original-opening.json').write_text(json.dumps(source_report, indent=2)+'\n')
    print(json.dumps({k:v for k,v in source_report.items() if k not in ('scene', 'spans', 'bounds_checks')}))


if __name__ == '__main__':
    main()
