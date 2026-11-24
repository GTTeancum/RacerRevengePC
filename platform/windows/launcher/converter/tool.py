import os,sys,runpy
from pathlib import Path
kit=Path(__file__).resolve().parent
stock=Path(os.environ['RR_CONVERT_STOCK'])
arc=kit/'racer-arcade'; n64=kit/'racer-n64'
script=kit/sys.argv.pop(1)
sys.argv[0]=str(script)
sys.path[:0]=[str(i) for i in (script.parent,arc/'tools/a2r',arc/'tools',n64/'tools/n64r',n64/'tools/n2r',kit/'port/tools/assets')]
runpy.run_path(str(script),run_name='__main__')
