$s='D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo'
foreach($r in 4,15,17,23,24){ "=== ui5-$r $(Get-Date -Format HH:mm:ss)"
  & "$s\run-silo.ps1" -build silo-cv-18 -out "fix14-ui5-$r" -assets assets -racer $r -seconds 75 -extra '--images --capture-every 4' }
"=== results $(Get-Date -Format HH:mm:ss)"
& "$s\run-silo.ps1" -build silo-cv-18 -out "fix14-ui5-res-17" -assets assets -racer 17 -seconds 480 -extra '--images --capture-every 60 --repair --unstick --results-confirm 30'
"=== done $(Get-Date -Format HH:mm:ss)"
