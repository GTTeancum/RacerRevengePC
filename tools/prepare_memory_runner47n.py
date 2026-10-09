from pathlib import Path
root=Path(__file__).resolve().parents[1]
text=(root/'tools/Run-Native47M.ps1').read_text()
text=text.replace("    [ValidateSet('0','1','2')][string]$RawMode='1',","    [ValidateSet('registry','memory')][string]$MemoryVariant='memory',\n    [ValidateSet('0','1','2')][string]$RawMode='1',")
text=text.replace("$Variant='leaf47m'","$Variant=$MemoryVariant+'47n'")
text=text.replace('native-run47m-','native-run47n-').replace("$Mode=$RawMode+","$Mode=$MemoryVariant+'-'+$RawMode+")
text=text.replace("$exe = Join-Path $rrWork 'native\\ninja-build-release-opt\\racer_leaf47m_fixed_probe.exe'","$exe = Join-Path $rrWork ('native/ninja-build-release-opt/racer_'+$MemoryVariant+'47n_fixed_probe.exe')")
text=text.replace("$rrLeafGate=Get-Content (Join-Path $rrRoot 'evidence/47M/accepted-native-build.json') -Raw | ConvertFrom-Json", "$rrLeafGates=Get-Content (Join-Path $rrRoot 'evidence/47N/build-gate.json') -Raw | ConvertFrom-Json\n$rrLeafGate=$rrLeafGates.$MemoryVariant")
text=text.replace('Native leaf binary does not match the accepted original-function differential','Registry binary does not match the verified integration build')
text=text.replace('release-opt-47m-compact-leaf-diagnostic','release-opt-47n-compact-registry-diagnostic')
needle='$process.WaitForExit()';assert text.count(needle)==1
text=text.replace(needle,'''# Read-only host process observations, once per second. These include Windows,
# PCSX2, driver-facing allocations and DLLs, not a stock-Xbox memory budget.
$rrMemorySamples=[System.Collections.Generic.List[object]]::new()
while(-not $process.WaitForExit(1000)){
    try {
        $process.Refresh()
        $rrMemorySamples.Add(@{utc=[DateTime]::UtcNow.ToString('o');private_bytes=$process.PrivateMemorySize64;working_set_bytes=$process.WorkingSet64;os_peak_working_set_bytes=$process.PeakWorkingSet64;virtual_bytes=$process.VirtualMemorySize64;cpu_seconds=$process.TotalProcessorTime.TotalSeconds})
    } catch {
        if(-not $process.HasExited){throw}
    }
}
$process.WaitForExit()
@{scope='Sampled Windows process including reference renderer/DLLs; not Xbox peak memory or GPU unified-memory accounting';interval_ms=1000;samples=$rrMemorySamples.ToArray()} | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $run 'host-memory.json')''')
(root/'tools/Run-Native47N.ps1').write_text(text)
print('Prepared registry variants with preserved launch evidence and sampled host memory.')
