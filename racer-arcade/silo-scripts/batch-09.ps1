$s='D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo'
$ai='--repair --unstick --results-confirm 30'
foreach($r in 3,16,22){ & "$s\run-silo.ps1" -build silo-cv-08 -out "silo-eb-ai2-$r" -assets assets -racer $r -seconds 900 -extra $ai }
