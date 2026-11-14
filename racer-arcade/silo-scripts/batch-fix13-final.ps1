$s='D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo'
"=== look4-4 $(Get-Date -Format HH:mm:ss)"
& "$s\run-silo.ps1" -build silo-cv-15 -out fix13-look4-4 -assets assets -racer 4 -seconds 130 -extra '--images --capture-every 4'
"=== sc3-11 $(Get-Date -Format HH:mm:ss)"
& "$s\run-silo.ps1" -build silo-cv-15 -out fix13-sc3-11 -assets assets -racer 11 -seconds 480 -extra '--repair --unstick --results-confirm 30'
"=== lc $(Get-Date -Format HH:mm:ss)"
& "$s\run-silo.ps1" -build silo-cv-15 -out fix13-lc-3 -assets assets-lc -racer 3 -seconds 700 -extra '--repair --unstick --results-confirm 30'
"=== sc3-24 $(Get-Date -Format HH:mm:ss)"
& "$s\run-silo.ps1" -build silo-cv-15 -out fix13-sc3-24 -assets assets -racer 24 -seconds 520 -extra '--repair --unstick --results-confirm 30'
"=== done $(Get-Date -Format HH:mm:ss)"
