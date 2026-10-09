"""Independent frozen48E world decoding across every original track SCB."""
from pathlib import Path
import argparse, hashlib, json, sys
sys.dont_write_bytecode = True
from decode_scene48c import load_container
from decode_world48e import decode_world
ROOT = Path(__file__).resolve().parents[2]
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--output',type=Path,required=True);args=ap.parse_args()
    args.output.mkdir(parents=True,exist_ok=False)
    rows=[]
    for p in sorted((ROOT/'work/native/project/disc/TRACKS').glob('*.RES')):
        members,unused=load_container(p)
        for name,(raw,entry) in members.items():
            if not name.lower().endswith('.scb'):continue
            target=args.output/(p.stem.lower()+'-'+name.replace(chr(92),'_').replace('/','_'))
            target.write_bytes(raw)
            row=dict(container='TRACKS/'+p.name,member=name,source_sha256=hashlib.sha256(raw).hexdigest(),file=target.resolve().relative_to(ROOT).as_posix())
            try:
                world=decode_world(raw)
                row.update(accepted=True,zones=len(world['zones']),portals=len(world['portals']),instances=len(world['instances']))
                target.with_suffix('.reference.json').write_text(json.dumps(world,indent=2)+'\n')
            except Exception as e:row.update(accepted=False,error=str(e))
            rows.append(row)
    (args.output/'reference-index.json').write_text(json.dumps(rows,indent=2)+'\n')
    print(json.dumps(dict(occurrences=len(rows),accepted=sum(r['accepted'] for r in rows),output=str(args.output))))
if __name__=='__main__':main()
