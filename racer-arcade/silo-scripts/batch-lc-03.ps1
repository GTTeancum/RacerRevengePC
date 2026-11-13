$s='D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo'
$ai='--repair --unstick --results-confirm 30'
foreach($r in 3,16,22){ "=== lc pod $r"; & "$s\run-silo.ps1" -build silo-cv-09 -out "silo-lc-ai3-$r" -assets assets-lc -racer $r -seconds 900 -extra $ai }
"=== eb frames"; & "$s\run-silo.ps1" -build silo-cv-09 -out silo-eb-frames-02 -assets assets -racer 3 -seconds 140 -extra '--repair --unstick --images --capture-every 8'
