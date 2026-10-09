"""Native content cache migration, corruption/cancellation, binding and draw checks."""
from pathlib import Path
import argparse, hashlib, json, shutil, subprocess, tempfile

ROOT = Path(__file__).resolve().parents[1]
if not __debug__:
    raise RuntimeError('Verification requires normal Python execution')
def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()
def js(p):
    return json.loads(p.read_text(encoding='utf-8-sig'))

def main():
    ap = argparse.ArgumentParser(); ap.add_argument('--architecture', choices=['x64','x86'], required=True)
    args = ap.parse_args(); exe = ROOT/f'build/native-pc48F-{args.architecture}/racer_native_pc.exe'
    out = Path(tempfile.mkdtemp(prefix=f'import48F-{args.architecture}-',dir=ROOT/'evidence'))
    cache = out/'native-cache'; fixture = out/'original-fixture'
    for name in ['SLUS_202.68','TRACKS/TA.RES','PODS/POD01/POD01.RES']:
        target=fixture/name; target.parent.mkdir(parents=True,exist_ok=True)
        shutil.copyfile(ROOT/'work/native/project/disc'/name,target)
    original_hashes = {str(p.relative_to(fixture)):sha(p) for p in fixture.rglob('*') if p.is_file()}
    shutil.copytree(ROOT/'evidence/import48E-x64-2ugjf7ni/native-cache',cache)
    old_current = (cache/'current.json').read_bytes()
    old_objects = {p.name:sha(p) for p in (cache/'objects').glob('*')}
    records=[]
    def run(label, arguments, expected=0):
        command=[str(exe),*map(str,arguments)]
        p=subprocess.run(command,capture_output=True,encoding='utf-8',timeout=180)
        (out/(label+'.stdout')).write_text(p.stdout); (out/(label+'.stderr')).write_text(p.stderr)
        records.append(dict(label=label,arguments=command,exit=p.returncode))
        assert p.returncode==expected,(label,p.returncode,p.stdout,p.stderr)
        return json.loads(p.stdout) if p.stdout.startswith('{') else None
    base=['--import','--source',fixture,'--cache',cache]
    cancelled=run('01-migration-cancel',base+['--cancel-after','3'],130)
    assert cancelled['cancelled'] and (cache/'current.json').read_bytes()==old_current
    imported=run('02-migration-resume',base); assert imported['success'] and imported['reused']>=228 and imported['converted']>0
    for name,digest in old_objects.items(): assert sha(cache/'objects'/name)==digest
    manifest=js(Path(imported['manifest_path'])); current=(cache/'current.json').read_bytes()
    groups={s:[m for m in manifest['members'] if m['status']==s] for s in ['converted_geometry','converted_texture','converted_material','converted_world']}
    assert len(groups['converted_geometry'])==241
    assert len(groups['converted_world'])==1
    assert groups['converted_texture'] and groups['converted_material']
    comparisons=[]
    for group,container in [('TA','TRACKS/TA.RES'),('POD01','PODS/POD01/POD01.RES')]:
        mapping=js(ROOT/f'evidence/textures48D/{group}/material-mapping.json')
        # Every frozen seven-texture mip is an independent expected native byte file.
        for texture in groups['converted_texture']:
            if texture['container']!=container: continue
            name=texture['member'].replace('\\','/').split('/')[-1][:-4]
            frozen=ROOT/f'evidence/textures48D/{group}/textures'/name
            if not frozen.exists(): continue
            for mip in texture['native']['mips']:
                actual=cache/'objects'/f"{texture['native']['key']}-mip{mip['index']}.rrtex"
                expected=frozen/f"mip-{mip['width']}x{mip['height']}.rrtex"
                assert actual.read_bytes()==expected.read_bytes()
                comparisons.append(dict(member=texture['member'],index=mip['index'],sha256=sha(actual)))
    assert len(comparisons)==25
    reused=run('03-reuse',base); assert reused['converted']==0 and (cache/'current.json').read_bytes()==current
    victim=groups['converted_texture'][0]['native']; mip=cache/'objects'/(victim['key']+'-mip0.rrtex')
    original=mip.read_bytes(); bad=bytearray(original); bad[-1]^=1; mip.write_bytes(bad)
    run('04-repair-mip',base); assert mip.read_bytes()==original and (cache/'current.json').read_bytes()==current
    victim=groups['converted_material'][0]['native']; meta=cache/'objects'/(victim['key']+'.json')
    original=meta.read_bytes(); altered=json.loads(original); altered['passes'][0]['program']='corrupted'
    meta.write_text(json.dumps(altered)); run('05-repair-material',base)
    assert meta.read_bytes()==original and (cache/'current.json').read_bytes()==current
    captures=[]
    for label,container,member in [('ground','TRACKS/TA.RES','geometry\\zone01groundstrips16.psg'),
                                    ('chariot','PODS/POD01/POD01.RES','pods\\pod01\\chariot.psg')]:
        capture=out/(label+'.ppm'); report=out/(label+'.json')
        run('06-render-'+label,['--cache',cache,'--container',container,'--member',member,'--frames','3','--capture',capture,'--report',report])
        data=js(report); assert data['success'] and data['last_frame_completed_readback'] and data['textured_spans_per_frame']>0
        assert data['bound_native_textures']>0 and data['native_mip_levels']>0
        if label=='chariot': assert data['draw_spans_per_frame']>1 and data['bound_native_textures']>1
        captures.append(dict(label=label,capture=str(capture),sha256=sha(capture),report_sha256=sha(report)))
    world_args=['--world','--cache',cache,'--container','TRACKS/TA.RES','--member',r'tracks\ta.scb']
    run('09-world-render',world_args+['--frames','3','--capture',out/'world.ppm','--report',out/'world.json'])
    world_report=js(out/'world.json')
    assert world_report['success'] and world_report['last_frame_completed_readback']
    for key,value in dict(world_original_instances=318,world_visual_instances=284,world_collision_only_instances=34,
        world_unique_geometry=228,world_saved_root_matrices_replaced=42,world_zones=9,world_portals=10,
        vertex_count=110196,index_count=209514).items():assert world_report[key]==value,(key,world_report[key])
    assert world_report['world_placed_mesh_sha256']=='4b38721b18705c459da282fe7736e3f3d45f3c7a0c21daeca033ca96833415d7'
    assert world_report['bound_native_textures']>0 and world_report['textured_spans_per_frame']>0
    victim=groups['converted_world'][0]['native']; world_path=cache/'objects'/(victim['key']+'.rrworld')
    original_world=world_path.read_bytes(); bad=bytearray(original_world); bad[-1]^=1; world_path.write_bytes(bad)
    run('10-reject-bad-world',world_args,1)
    repaired=run('11-repair-world',base)
    assert repaired['converted']==1 and world_path.read_bytes()==original_world and (cache/'current.json').read_bytes()==current
    # World output paths obey the same non-overwrite policy as scene inspection.
    run('12-reject-world-output',world_args+['--report',world_path],1)
    assert world_path.read_bytes()==original_world
    geometry=next(m['native'] for m in groups['converted_geometry'] if m['container']=='TRACKS/TA.RES')
    scene_path=cache/'objects'/(geometry['key']+'.rrscene'); good_scene=scene_path.read_bytes()
    broken=bytearray(good_scene);broken[-1]^=1;scene_path.write_bytes(broken)
    run('13-reject-world-bad-geometry',world_args,1)
    repaired=run('14-repair-world-geometry',base)
    assert repaired['converted']==1 and scene_path.read_bytes()==good_scene
    assert (cache/'current.json').read_bytes()==current
    import ctypes
    api=ctypes.WinDLL('kernel32',use_last_error=True)
    api.CreateFileW.argtypes=[ctypes.c_wchar_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p]
    api.CreateFileW.restype=ctypes.c_void_p
    api.CloseHandle.argtypes=[ctypes.c_void_p]
    lock=api.CreateFileW(str(cache/'import.lock'),0x80000000,0,None,3,0x80,None)
    assert lock not in (None,ctypes.c_void_p(-1).value),ctypes.get_last_error()
    try:run('15-reject-world-locked-cache',world_args,1)
    finally:assert api.CloseHandle(lock)
    # An invalid active manifest must not quietly fall back to unverified scene data.
    manifest_path=Path(imported['manifest_path']); original=manifest_path.read_bytes()
    manifest_path.write_bytes(original+b' ')
    run('07-reject-bad-manifest',['--cache',cache,'--container','TRACKS/TA.RES','--member','geometry\\zone01groundstrips16.psg'],1)
    manifest_path.write_bytes(original)
    run('08-reject-cache-output',['--cache',cache,'--container','TRACKS/TA.RES','--member','geometry\\zone01groundstrips16.psg','--report',cache/'current.json'],1)
    assert (cache/'current.json').read_bytes()==current
    assert original_hashes=={str(p.relative_to(fixture)):sha(p) for p in fixture.rglob('*') if p.is_file()}
    report=dict(passed=True,architecture=args.architecture,executable_sha256=sha(exe),native_invocations=len(records),
        counts={k:len(v) for k,v in groups.items()},old_content_objects_unchanged=len(old_objects),
        frozen_mip_comparisons=comparisons,captures=captures,runs=records,original_source_hashes=original_hashes)
    (out/'checks.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(dict(output=str(out),passed=True,counts=report['counts'],native_invocations=len(records))))

if __name__=='__main__':main()
