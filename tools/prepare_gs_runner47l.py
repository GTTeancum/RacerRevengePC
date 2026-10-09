from pathlib import Path
root=Path(__file__).resolve().parents[1]
source=(root/'tools/Run-Native47J2.ps1').read_text()
source=source.replace("    [ValidateSet('0','1')][string]$ReadyCache='1',", "    [ValidateSet('0','1','2')][string]$RawMode='1',\n    [ValidateSet('0','1')][string]$ReadyCache='1',")
source=source.replace("$Variant='iop47j2'", "$Variant='gs47l'\nif((Get-Content (Join-Path $rrRoot 'evidence/47L/build.exit')).Trim() -ne '0'){throw 'GS47L build not verified'}\n$env:RR_RAW_GS_FAST47L=$RawMode")
source=source.replace("$Mode=$ImportCache+'-'+$ReadyCache+'-'+$IdleBatch", "$Mode=$RawMode+'-'+$ImportCache+'-'+$ReadyCache+'-'+$IdleBatch")
source=source.replace('native-run47j2-','native-run47l-').replace('racer_iop47j2_fixed_probe.exe','racer_gs47l_fixed_probe.exe')
source=source.replace('release-opt-47j2-iop-ready-cache-diagnostic','release-opt-47l-raw-gs-diagnostic')
# Launch metadata is extended immediately beside the existing IOP mode field.
assert 'import_cache=$ImportCache' in source
source=source.replace('import_cache=$ImportCache','raw_gs_mode=$RawMode;import_cache=$ImportCache')
target=root/'tools/Run-Native47L.ps1';assert not target.exists()
target.write_text(source)
print('Prepared GS47L native runner with frozen per-run binary and explicit raw mode.')
