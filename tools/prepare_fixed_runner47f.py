"""Derive fixed-VBlank launch settings from the immutable 47C experiment."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
target=root/'tools/Run-Native47F.ps1'
assert not target.exists()
text=(root/'tools/Run-Native47C.ps1').read_text()
before="    [ValidateSet('baseline','batched')][string]$Mode='baseline'"
after="""    [ValidateSet('reference','compact')][string]$Variant='reference',
    [ValidateSet('0','1')][string]$IdleBatch='0',
    [ValidateRange(1,10000000)][int]$Vblank=7000"""
assert text.count(before)==1
text=text.replace(before,after)
text=text.replace("$rrWork = Join-Path $rrRoot 'work'", "$rrWork = Join-Path $rrRoot 'work'\nif((Get-Content (Join-Path $rrRoot 'evidence/47F/build.exit')).Trim() -ne '0'){throw '47F fixed-stop build is not verified'}")
text=text.replace("$env:RR_IOP_IDLE_BATCH=if($Mode -eq 'batched'){'1'}else{'0'}",
    "$Mode=$Variant+'-'+$IdleBatch\n$env:RR_IOP_IDLE_BATCH=$IdleBatch\n$env:RR_STOP_AT_VBLANK47F=[string]$Vblank")
text=text.replace("'native\\ninja-build-release-opt\\racer_native_probe.exe'",
    "('native\\ninja-build-release-opt\\'+$(if($Variant -eq 'compact'){'racer_compact47f_probe.exe'}else{'racer_fixed47f_probe.exe'}))")
text=text.replace("'native-run47c-'","'native-run47f-'")
text=text.replace("@{mode=$Mode;idle_batch=", "@{variant=$Variant;stop_at_vblank=$Vblank;mode=$Mode;idle_batch=")
text=text.replace('same immutable 47B candidate binary, differing only in idle batch flag',
    'fixed VBlank observer; explicit registry variant and idle batch flag')
text=text.replace('release-opt-47b-idle-adapter-pcsx2-vu-44r-fp-34a-yield','release-opt-47f-fixed-vblank-diagnostic')
text+='''
$rrExpectedStop="[rr:fixed-stop47f] target=$Vblank actual=$Vblank "
$rrStopLines=@(Select-String -LiteralPath (Join-Path $run 'native.log') -SimpleMatch $rrExpectedStop)
@{expected_vblank=$Vblank;matching_stop_count=$rrStopLines.Count;native_exit=$exitCode;cards_unchanged=($cardBefore -ceq $cardAfter);verified=($rrStopLines.Count -eq 1)} | ConvertTo-Json | Set-Content (Join-Path $run 'fixed-stop-validation.json')
if($rrStopLines.Count -ne 1) {
    throw 'Requested VBlank stop was not reached exactly; wall-clock completion is insufficient'
}
'''
target.write_text(text,encoding='utf-8',newline='\n')
print(target)
