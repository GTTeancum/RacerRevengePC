# Build silo-cv-08, then sequential muted logs-only runs (no compile during runs).
$s='D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo'
cmd /c "$s\tree\build\silo-relink.cmd silo-cv-07 silo-cv-08" | Out-Null
$c="$s\tree\build\silo-cv-08-console.log"
if(-not (Select-String -Path $c -Pattern 'rr_interactive_vehicle50du.exe [0-9A-F]{64}' -Quiet)){ "BUILD FAILED"; Get-Content $c -Tail 5; exit 1 }
"built"
$ai='--repair --unstick --results-confirm 30'
& "$s\run-silo.ps1" -build silo-cv-08 -out silo-sc-orig-03 -assets assets-orig -racer 9
& "$s\run-silo.ps1" -build silo-cv-08 -out silo-eb-ai-03 -assets assets -racer 3 -seconds 900 -extra $ai
& "$s\run-silo.ps1" -build silo-cv-08 -out silo-eb-ai-16 -assets assets -racer 16 -seconds 900 -extra $ai
& "$s\run-silo.ps1" -build silo-cv-08 -out silo-eb-ai-22 -assets assets -racer 22 -seconds 900 -extra $ai
