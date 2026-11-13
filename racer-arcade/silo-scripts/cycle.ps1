param([string]$base,[string]$next,[string]$out,[string]$assets='assets',[int]$racer=3,[int]$seconds=600)
# One silo iteration: worktree-local relink, then a muted logs-only run.
$s='D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo'
cmd /c "$s\tree\build\silo-relink.cmd $base $next" | Out-Null
$c="$s\tree\build\$next-console.log"
if(-not (Select-String -Path $c -Pattern 'rr_interactive_vehicle50du.exe [0-9A-F]{64}' -Quiet)){
 "BUILD FAILED"; Get-Content $c -Tail 5
 Get-ChildItem "$s\tree\build\$next\*.log" | Select-String -Pattern 'error C|error LNK' | Select-Object -First 10 | ForEach-Object { $_.Line }
 exit 1
}
(Get-Content $c -TotalCount 1)
& "$s\run-silo.ps1" -build $next -out $out -assets $assets -racer $racer -seconds $seconds
