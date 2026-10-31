#!/usr/bin/env python3
"""Serial exact interactive closure compile/link only. Never runs binaries/tests."""
from pathlib import Path
import hashlib,json,os,shutil,subprocess,sys,time

def sha(p):
    h=hashlib.sha256()
    with Path(p).open('rb') as f:
        for b in iter(lambda:f.read(1048576),b''):h.update(b)
    return h.hexdigest()
def write(p,v):
    t=p.with_suffix(p.suffix+'.tmp');t.write_text(json.dumps(v,indent=2)+'\n');os.replace(t,p)
def main():
    root=Path(__file__).resolve().parents[2];out=Path(sys.argv[1]).resolve();assert not out.exists();out.mkdir(parents=True)
    compiler=shutil.which(os.environ.get('CXX','g++-14'));assert compiler
    # Keep CXX as the real compiler; the optional launcher is never its identity.
    launcher_name=os.environ.get('RR_CXX_LAUNCHER','')
    launcher=None;cache_inputs={}
    if launcher_name:
        assert launcher_name=='ccache','Only the explicitly reviewed ccache launcher is supported'
        launcher=shutil.which(launcher_name);assert launcher
        for name in ('CCACHE_CONFIGPATH','CCACHE_EXTRAFILES'):
            path=Path(os.environ[name]).resolve();assert path.is_file()
            cache_inputs[name]=dict(path=str(path),sha256=sha(path))
    compile_prefix=([launcher] if launcher else [])+[compiler]
    tracked=subprocess.check_output(['git','ls-files','-z'],cwd=root).decode().split('\0');tracked=[p for p in tracked if p]
    baseline={p:sha(root/p) for p in tracked};write(out/'source-hashes.json',baseline)
    receipt=dict(source_commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip(),compiler=compiler,compiler_sha256=sha(compiler),runtime_executed=False,tests_executed=False,objects=[],pass_=False)
    receipt['compiler_launcher']=dict(path=launcher,sha256=sha(launcher),inputs=cache_inputs) if launcher else None
    receipt['pass']=receipt.pop('pass_');write(out/'RESULT.json',receipt)
    def run(label,argv):
        assert shutil.disk_usage(out).free>256*1024*1024
        before=Path('/proc/meminfo').read_text();write(out/(label+'.command.json'),dict(argv=argv,memory_before=before))
        started=time.monotonic()
        with (out/(label+'.log')).open('wb') as log:
            result=subprocess.run(argv,cwd=root,stdout=log,stderr=subprocess.STDOUT,timeout=1800)
        write(out/(label+'.status.json'),dict(exit_code=result.returncode,seconds=time.monotonic()-started,memory_after=Path('/proc/meminfo').read_text()))
        print(label,result.returncode,flush=True)
        if result.returncode:raise RuntimeError('Command failed: '+label)
    try:
        command_file=out/'generated-command.json'
        run('inventory',[sys.executable,str(root/'tools/prepare_interactive_build50dv.py'),str(command_file),str(out/'unused-output')])
        command=json.loads(command_file.read_text());units=[];options=[];skip=False
        common=['-fmax-errors=8','-ftrack-macro-expansion=0','--param=ggc-min-expand=1','--param=ggc-min-heapsize=4096','-fno-fast-math','-frounding-math','-ffp-contract=off','-pthread','-g0']
        for arg in command[1:]:
            if skip:skip=False;continue
            if arg=='-o':skip=True;continue
            if arg.endswith(('.cpp','.c')) and not arg.startswith('-'):
                p=Path(arg).resolve();assert p.is_relative_to(root) and p.is_file();units.append(str(p.relative_to(root)));continue
            if arg in ('-lz','-ldl'):continue
            assert arg.startswith(('-I','-D','-U','-std=','-O')) or arg in common,('Unreviewed option',arg)
            if not arg.startswith(('-O','-std=')) and arg not in common:options.append(arg)
        win=json.loads((root/'platform/windows/native/interactive50du.sources.json').read_text())
        for p,pin in win['sourceListProvenance'].items():assert sha(root/p)==pin
        assert len(units)==607 and len(set(units))==607 and all(x.endswith('.cpp') for x in units)
        assert len(win['sources'])==606 and len(win['nativeDependencySources'])==6
        assert set(win['sources']).issubset(units) and len(set(units)-set(win['sources']))==1
        assert '-lz' in command and '-ldl' in command
        write(out/'source-closure.json',units)
        objects=[]
        for i,unit in enumerate(units):
            heavy=unit.startswith(('tests/native/','platform/linux/native/','src/game/original_results104','src/game/original_race_end_')) or unit in ('src/game/original_race_timing_manager50la.cpp','src/game/original_race_music_manager50la.cpp','src/game/original_gc_gameplay_lighting_candidate.cpp')
            obj=out/('%04d.o'%i);dep=out/('%04d.d'%i)
            run('%04d'%i,compile_prefix+common+['-O0' if heavy else '-O2','-std=c++20']+options+['-MD','-MF',str(dep),'-c',str(root/unit),'-o',str(obj)])
            assert obj.is_file() and dep.is_file();objects.append(str(obj));receipt['objects'].append(dict(unit=unit,sha256=sha(obj),dependencies_sha256=sha(dep)));write(out/'RESULT.json',receipt)
        exe=out/'racer-revenge'
        run('link',[compiler]+objects+['-pthread','-lz','-ldl','-o',str(exe)])
        assert exe.read_bytes()[:4]==b'\x7fELF'
        assert all(sha(root/p)==h for p,h in baseline.items()),'Tracked source changed during build'
        assert sha(compiler)==receipt['compiler_sha256'],'Compiler changed during build'
        if launcher:
            assert sha(launcher)==receipt['compiler_launcher']['sha256'],'Cache launcher changed during build'
            assert all(sha(v['path'])==v['sha256'] for v in cache_inputs.values()),'Cache policy/toolchain receipt changed during build'
        receipt.update(binary_sha256=sha(exe),binary=str(exe));receipt['pass']=True
    except Exception as exc:
        receipt['error']=repr(exc);raise
    finally:write(out/'RESULT.json',receipt)
if __name__=='__main__':main()
