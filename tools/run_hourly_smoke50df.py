#!/usr/bin/env python3
"""Run exactly one native smoke command, enforcing a shared rolling-hour ledger.
Usage: run_hourly_smoke50df.py LEDGER_JSON COMMAND_JSON OUTPUT_DIR
Command JSON: {"cwd": "...", "command": ["/path/to/program", "argument"]}.
This runner does not build, retry, run controls, or run sanitizer variants.
"""
from pathlib import Path
from datetime import datetime,timezone,timedelta
import json,subprocess,sys,os

def stamp():return datetime.now(timezone.utc)
def main():
 if len(sys.argv)!=4:raise SystemExit(__doc__)
 ledger=Path(sys.argv[1]).resolve();spec=json.loads(Path(sys.argv[2]).read_text());out=Path(sys.argv[3]).resolve()
 if not isinstance(spec.get('command'),list)or not spec['command']or not all(isinstance(x,str)for x in spec['command']):raise SystemExit('invalid command')
 lock=ledger.with_suffix(ledger.suffix+'.lock');ledger.parent.mkdir(parents=True,exist_ok=True)
 try:fd=os.open(lock,os.O_WRONLY|os.O_CREAT|os.O_EXCL,0o600)
 except FileExistsError:raise SystemExit('another hourly test reservation exists; inspect before clearing it')
 try:
  os.close(fd);old=json.loads(ledger.read_text())if ledger.exists()else{}
  now=stamp()
  if old.get('last_started'):
   allowed=datetime.fromisoformat(old['last_started'])+timedelta(hours=1)
   if now<allowed:raise SystemExit('hourly test cap: next permitted '+allowed.isoformat())
  out.mkdir(parents=True,exist_ok=False)
  record={'last_started':now.isoformat(),'command':spec['command'],'cwd':spec['cwd'],'status':'running'}
  ledger.write_text(json.dumps(record,indent=2));(out/'command.json').write_text(json.dumps(spec,indent=2))
  try:
   p=subprocess.run(spec['command'],cwd=spec['cwd'],capture_output=True,text=True,timeout=120)
   (out/'stdout.log').write_text(p.stdout);(out/'stderr.log').write_text(p.stderr)
   record.update(status='passed'if p.returncode==0 else'failed',returncode=p.returncode)
  except (OSError,subprocess.TimeoutExpired)as error:
   (out/'runner-error.txt').write_text(str(error));record.update(status='failed',returncode=124)
  record['finished']=stamp().isoformat();ledger.write_text(json.dumps(record,indent=2));(out/'receipt.json').write_text(json.dumps(record,indent=2))
  print(json.dumps(record),flush=True);raise SystemExit(record['returncode'])
 finally:
  lock.unlink()
if __name__=='__main__':main()
