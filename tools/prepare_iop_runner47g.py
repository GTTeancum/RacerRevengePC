"""Use fixed-stop launch controls with a single IOP-cache candidate executable."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
out=root/'tools/Run-Native47G.ps1'
assert not out.exists()
text=(root/'tools/Run-Native47F.ps1').read_text()
text=text.replace("[ValidateSet('reference','compact')][string]$Variant='reference'", "[ValidateSet('0','1')][string]$ReadyCache='0'")
text=text.replace("[string]$IdleBatch='0'", "[string]$IdleBatch='1'")
text=text.replace("[int]$Vblank=7000", "[int]$Vblank=7000,\n    [string]$BinarySource=''")
text=text.replace("$Mode=$Variant+'-'+$IdleBatch", "$Variant='iop47g'\n$Mode=$ReadyCache+'-'+$IdleBatch\n$env:RR_IOP_READY_CACHE47G=$ReadyCache\n$env:RR_IOP_READY_STATS47G='1'")
text=text.replace("evidence/47F/build.exit", "evidence/47G/build.exit")
text=text.replace("47F fixed-stop build", "47G IOP-cache build")
text=text.replace("('native\\ninja-build-release-opt\\'+$(if($Variant -eq 'compact'){'racer_compact47f_probe.exe'}else{'racer_fixed47f_probe.exe'}))", "'native\\ninja-build-release-opt\\racer_iop47g_fixed_probe.exe'")
text=text.replace("$exe = Join-Path $rrWork 'native\\ninja-build-release-opt\\racer_iop47g_fixed_probe.exe'", "$exe = Join-Path $rrWork 'native\\ninja-build-release-opt\\racer_iop47g_fixed_probe.exe'\nif($BinarySource){$exe=(Resolve-Path -LiteralPath $BinarySource).Path}")
text=text.replace("'native-run47f-'", "'native-run47g-'")
text=text.replace("@{variant=$Variant;", "@{ready_cache=$ReadyCache;variant=$Variant;")
text=text.replace("release-opt-47f-fixed-vblank-diagnostic", "release-opt-47g-iop-ready-cache-diagnostic")
out.write_text(text,encoding='utf-8',newline='\n')
print(out)
