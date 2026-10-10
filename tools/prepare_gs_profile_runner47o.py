from pathlib import Path
root=Path(__file__).resolve().parents[1]
text=(root/'tools/Run-Native47N.ps1').read_text()
text=text.replace("    [ValidateSet('registry','memory')][string]$MemoryVariant='memory',","    [switch]$DisableGsProfile,")
text=text.replace("$Variant=$MemoryVariant+'47n'","$MemoryVariant='memory'\n$Variant='gsprofile47o'")
text=text.replace('native-run47n-','native-run47o-')
text=text.replace("$Mode=$MemoryVariant+'-'+$RawMode+","$Mode=([int](-not $DisableGsProfile.IsPresent)).ToString()+'-'+$RawMode+")
text=text.replace("$exe = Join-Path $rrWork ('native/ninja-build-release-opt/racer_'+$MemoryVariant+'47n_fixed_probe.exe')","$exe = Join-Path $rrWork 'native/ninja-build-release-opt/racer_gsprofile47o_fixed_probe.exe'")
text=text.replace("$rrLeafGates=Get-Content (Join-Path $rrRoot 'evidence/47N/build-gate.json') -Raw | ConvertFrom-Json\n$rrLeafGate=$rrLeafGates.$MemoryVariant", "$rrLeafGate=Get-Content (Join-Path $rrRoot 'evidence/47O/build-gate.json') -Raw | ConvertFrom-Json")
text=text.replace('Registry binary does not match the verified integration build','GS profiler binary does not match the verified build')
needle="New-Item -ItemType Directory -Path $run | Out-Null";assert text.count(needle)==1
text=text.replace(needle,needle+"\nif($DisableGsProfile){Remove-Item Env:RR_PROFILE_GS47O -ErrorAction SilentlyContinue}else{$env:RR_PROFILE_GS47O=Join-Path $run 'gs-profile.csv'}")
text=text.replace('release-opt-47n-compact-registry-diagnostic','release-opt-47o-renderer-call-profile')
text=text.replace('@{raw_gs_mode=$RawMode;','@{gs_profile47o=(-not $DisableGsProfile.IsPresent);raw_gs_mode=$RawMode;')
(root/'tools/Run-Native47O.ps1').write_text(text)
print('Prepared GS profile on/off runner with frozen binary and unchanged renderer DLL.')
