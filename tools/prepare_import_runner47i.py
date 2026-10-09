"""Stage the import-cache candidate using the exact fixed-stop game controls."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
out=root/'tools/Run-Native47I.ps1'
assert not out.exists()
text=(root/'tools/Run-Native47H.ps1').read_text()
text=text.replace("[string]$ReadyCache='0'", "[string]$ReadyCache='1',\n    [ValidateSet('0','1')][string]$ImportCache='1'")
text=text.replace("$Mode=$ReadyCache+'-'+$IdleBatch", "$Mode=$ImportCache+'-'+$ReadyCache+'-'+$IdleBatch\n$env:RR_IOP_IMPORT_CACHE47I=$ImportCache\n$env:RR_IOP_IMPORT_STATS47I='1'")
text=text.replace('47H','47I').replace('47h','47i')
text=text.replace('@{ready_cache=$ReadyCache;', '@{import_cache=$ImportCache;ready_cache=$ReadyCache;')
out.write_text(text,encoding='utf-8',newline='\n')
print(out)
