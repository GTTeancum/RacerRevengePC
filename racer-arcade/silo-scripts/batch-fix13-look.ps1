param([string]$list="4,11,15,17,23,24",[string]$tag="look")
# fix13 look-check runs: each converted pod as player on SC, framebuffer captures, 130 s.
$s='D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo'
foreach($r in ($list -split ",")){
  $o="fix13-$tag-$r"
  "=== $o $(Get-Date -Format HH:mm:ss)"
  & "$s\run-silo.ps1" -build silo-cv-15 -out $o -assets assets -racer ([int]$r) -seconds 130 -extra '--images --capture-every 4'
}
"=== done $(Get-Date -Format HH:mm:ss)"
