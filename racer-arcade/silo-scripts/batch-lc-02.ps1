$s='D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo'
$ai='--repair --unstick --results-confirm 30'
foreach($r in 3,16,22){ "=== pod $r"; & "$s\run-silo.ps1" -build silo-cv-08 -out "silo-lc-ai2-$r" -assets assets-lc -racer $r -seconds 900 -extra $ai }
