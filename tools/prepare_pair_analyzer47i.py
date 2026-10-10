"""Reuse the verified capture interpolation with explicit run directories."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
out=root/'tools/analyze_fixed_pair47i.py'
assert not out.exists()
text=(root/'tools/analyze_pair47c.py').read_text()
text=text.replace("parser.add_argument('--to-vblank',type=int,default=7000)", "parser.add_argument('--to-vblank',type=int,default=6900)\nparser.add_argument('--baseline',type=Path,required=True)\nparser.add_argument('--candidate',type=Path)\nparser.add_argument('--expected-sha256',required=True)")
a="""    matches=list((args.root/'work').glob('native-run47c-'+mode+'-*'))
    if not matches: return {'status':'not started'}
    assert len(matches)==1, 'Select a unique experiment before comparing'
    run=matches[0]"""
b="""    run=args.baseline if mode=='baseline' else args.candidate
    if run is None: return {'status':'not started'}
    run=run.resolve()
    assert sha(run/'racer_native_probe.exe')==args.expected_sha256.lower(), 'Frozen executable hash differs'"""
assert text.count(a)==1
text=text.replace(a,b)
text=text.replace("    exit_file=run/'native.exit'", "    validation=run/'fixed-stop-validation.json'\n    if validation.exists(): result['fixed_stop']=json.loads(validation.read_text(encoding='utf-8-sig'))\n    exit_file=run/'native.exit'")
text=text.replace("and r.get('cards_unchanged') for r", "and r.get('cards_unchanged') and (r.get('fixed_stop') or {}).get('verified') for r")
text=text.replace("'batched'", "'candidate'")
out.write_text(text,encoding='utf-8',newline='\n')
print(out)
