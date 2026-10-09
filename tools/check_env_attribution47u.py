from pathlib import Path
import argparse,csv,json
p=argparse.ArgumentParser();p.add_argument('directory',type=Path);a=p.parse_args()
files=list(a.directory.glob('test-env-*.tsv'));assert len(files)==2,files
totals={};per_thread=[]
for f in files:
    rows=list(csv.DictReader(f.open(),delimiter='\t'));counts={}
    for r in rows:
        counts[r['name']]=counts.get(r['name'],0)+int(r['calls'])
        totals[r['name']]=totals.get(r['name'],0)+int(r['calls'])
    assert counts.get('<overflow>')==0,counts
    per_thread.append(counts)
assert totals['RR_ENV47U_TEST_VALUE']==1000 and totals['RR_ENV47U_TEST_ABSENT']==1000,totals
assert sorted(t['RR_ENV47U_TEST_VALUE'] for t in per_thread)==[300,700],per_thread
print(json.dumps(dict(return_value_tests=True,counts=totals,per_thread=per_thread),indent=2))
