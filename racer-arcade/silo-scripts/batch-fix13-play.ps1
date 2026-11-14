# fix13 player runs: each converted pod as player on SC (logs only), then AI-field races with player 3.
param([string]$list="4,15,17,23,24",[string]$ai="3",[int]$sec=440,[string]$tag="sc",[string]$build="silo-cv-15")
$s='D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo'
foreach($r in ($list -split ',')){ if(-not $r){continue}
  $o="fix13-$tag-$r"
  "=== $o $(Get-Date -Format HH:mm:ss)"
  & "$s\run-silo.ps1" -build $build -out $o -assets assets -racer ([int]$r) -seconds $sec -extra '--repair --unstick --results-confirm 30'
}
foreach($a in ($ai -split ',')){ if(-not $a){continue}
  $o="fix13-$tag-ai-$a"
  "=== $o $(Get-Date -Format HH:mm:ss)"
  & "$s\run-silo.ps1" -build $build -out $o -assets assets -racer 3 -seconds $sec -extra '--repair --unstick --results-confirm 30'
}
"=== done $(Get-Date -Format HH:mm:ss)"
