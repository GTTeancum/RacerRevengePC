"""Adversarial parser checks plus original-asset independent invariants."""
from pathlib import Path
import json
import struct
import sys
from decode_static48b import decode_psg3, decode_pst1, mesh_bytes, texture_bytes, triangle_list, FormatError, sha

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT/'evidence/assets48B'
checks = 0


def check(ok, message):
    global checks
    checks += 1
    if not ok:
        raise AssertionError(message)


def rejects(fn, data, label):
    global checks
    try:
        fn(data)
    except FormatError:
        checks += 1
        return
    raise AssertionError('accepted invalid input: '+label)


def mutated(blob, offset, fmt, *values):
    data = bytearray(blob)
    struct.pack_into(fmt, data, offset, *values)
    return bytes(data)


def main():
    fixtures = [('ground', 53, 14, 25, 24, '34135fefe81be7a98797d64cb3068feebdd7ee063cd5ccd34c42fb635dc23e4e'),
                ('rock', 73, 16, 41, 40, 'f66b3bb55b7fadd4c855c42376d0862f2de1824ec932d49428ace548e4eeaaaa')]
    reports = []
    for name, nv, ns, nt, positive, digest in fixtures:
        b = (OUT/name/'original.psg').read_bytes()
        check(sha(b) == digest, 'original source fixture changed')
        vertices, indices, report = decode_psg3(b)
        check((len(vertices), len(indices)//3, report['strips']) == (nv, nt, ns), 'independent original debug counts')
        check(report['normal_alignment'] == {'positive':positive, 'negative':0, 'zero':1}, 'original stored normals invariant')
        check(len(indices) == (nv - 2*ns)*3, 'every ADC suppresses one emission')
        check(all(0 <= i < nv for i in indices), 'index range')
        data = mesh_bytes(vertices, indices)
        check(len(data) == 16 + 24*nv + 12*nt and data[:8] == b'RRMESH01', 'viewer contract')
        check(data == (OUT/name/'mesh.rrmesh').read_bytes(), 'deterministic export')
        # Every strict prefix of each real asset must reject. This traverses all
        # header, string, float, attribute, command and alignment boundaries.
        for size in range(len(b)):
            rejects(decode_psg3, b[:size], f'{name} truncation {size}')
        for offset,fmt,value,label in [
            (4,'<I',5,'later version'), (8,'<I',0xffffffff,'huge object count'),
            (140,'<f',2.,'unsupported transform'), (204,'<f',float('nan'),'nonfinite bounds'),
            (236,'<I',2,'extra material'), (304,'<f',float('inf'),'infinite quantum'),
            (304,'<f',0.,'zero quantum'), (320,'<f',1e9,'wrong origin'),
            (336,'<I',2,'multiple LODs'), (348,'<I',1,'unsupported surface'),
            (360,'<H',nv+1,'vertex count mismatch'), (362,'<H',ns+1,'strip count mismatch'),
            (364,'<I',0x7fffffff,'overflowing payload'), (368,'<I',0x6c008000,'zero count'),
            (388,'<I',0x6dff8001,'mismatched VIF count'),
            (398,'<h',16,'missing first ADC'), (398,'<h',-32752,'later ADC convention')]:
            rejects(decode_psg3, mutated(b,offset,fmt,value), name+' '+label)
        # Header plus extent adjusted cannot hide a nonzero trailing command.
        tail = bytearray(b)
        tail[-4:] = b'\x01\x00\x00\x00'
        rejects(decode_psg3, bytes(tail), name+' nonzero trailing word')
        reports.append({'fixture':name, 'source_sha256':sha(b), 'vertices':nv, 'triangles':nt,
                        'normal_alignment':report['normal_alignment'], 'bounds_error':report['max_bounds_error']})
    # Independently written hand-sized strips: odd/even winding and explicit
    # new strip must reset local parity without joining disconnected geometry.
    points = [(0,0,0,-16),(1,0,0,-16),(0,1,0,16),(1,1,0,16),
              (5,0,0,-16),(6,0,0,-16),(5,1,0,16)]
    ix,lens = triangle_list(points,0)
    check(ix == [0,1,2,2,1,3,4,5,6] and lens == [4,3], 'disconnected strip analytical oracle')
    rejects(lambda x: triangle_list(x,0), points[:2], 'short strip')
    rejects(lambda x: triangle_list(x,0), [(0,0,0,16)]*3, 'no ADC')
    for name,*_ in fixtures:
        b = (OUT/name/'original.pst').read_bytes()
        palette, surfaces, report = decode_pst1(b)
        check([(s['width'],s['height']) for s in surfaces] == [(16,16),(32,32),(64,64),(128,128),(256,256)], 'original mip dimension progression')
        check(len(palette) == 256 and all(len(c)==4 for c in palette), 'palette extent')
        check(surfaces[-1]['indices'] == b[22876:] and len(surfaces[-1]['indices']) == 65536,
              'independent base mip byte extent')
        for sw,file in [(False,'texture-linear-palette.rrtex'), (True,'texture-csm1-palette.rrtex')]:
            out = texture_bytes(palette, surfaces[-1], sw)
            check(out[:16] == struct.pack('<8sII',b'RRTEX001',256,256) and len(out)==262160, 'texture contract')
            check(out == (OUT/name/file).read_bytes(), 'deterministic texture candidate')
        truncations = set(range(1040)) | {len(b)-1, len(b)//2}
        for s in surfaces:
            at = s['header_offset']
            truncations.update([at,at+1,at+15,at+16,at+16+s['bytes']-1])
        for size in sorted(truncations):
            rejects(decode_pst1,b[:size],f'{name} PST prefix {size}')
        for offset,fmt,val,label in [(0,'<H',2,'later revision'),(2,'<H',8,'streaming'),
            (4,'<I',39,'later format enum'),(8,'<I',0xffffffff,'huge mips'),
            (1036,'<I',0xffffffff,'huge height'),(1040,'<I',17,'nonpower width'),
            (1044,'<I',256,'later SizeBytes'),(1308,'<I',64,'wrong mip step')]:
            rejects(decode_pst1,mutated(b,offset,fmt,val),name+' '+label)
        rejects(decode_pst1,b+b'\0',name+' surplus texture byte')
    summary={'status':'PASS','checks':checks,'fixtures':reports,
             'limitations':['No original shader/texture differential acceptance from parser tests.',
                            'Candidate image interpretation remains unverified.'],
             'test_sha256':sha(Path(__file__).read_bytes())}
    (OUT/'parser-tests.json').write_text(json.dumps(summary,indent=2)+'\n')
    print(json.dumps(summary))


if __name__ == '__main__':
    main()
