param(
  [Parameter(Mandatory=$true)][string]$Build,        # the builder's build: folder with build-receipt.json, or its exe
  [string]$Tag='1',                                  # evidence names: cr<Tag>-NN-<course>-<mode>-<pod>
  [switch]$Continue,                                 # do not stop at the stop-or-go gate after run 3
  [int]$From=1,                                      # first run number (resume after the gate: -From 4)
  [int[]]$Only=@(),                                  # run numbers to run (default: all from -From, in order)
  [switch]$SkipHostCheck,                            # do not require the host data check report to pass
  [string]$HostCheckReport='D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\check\track_check-report.txt',
  [string]$Runner='D:\Programming\GitHub\RacerRevengePC\tools\capture\run_windows_takeover.py',
  [string]$Python='C:\Users\smmel\AppData\Local\Programs\Python\Python312\python.exe',
  [string]$Assets='C:\Games\Star Wars Racer Revenge',
  [string]$Sdl='D:\Programming\GitHub\RacerRevengePC\toolchains\sdl2-msvc\extracted\SDL2-2.32.10\lib\x64\SDL2.dll',
  [string]$DlcRoot='D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\addon-arcade-146\DLC',
  [switch]$DryRun                                    # print the runner command lines only
)
# The common rewrite verification round (PLAN 5.3, writer R11). 17 muted, unattended,
# logs-only runs, one game instance at a time, chained:
#   14 one-lap Single Races, one per stock course plus Vengeance (start, in-place Pause
#   RESTART, finish, Results, then the Hall or the Course screen), 2 Time Trials (GC with the
#   records page, Vengeance with a Course QUIT) and 1 tournament race on RA.
# Runs 1-3 are the stop-or-go gate: the round stops after run 3 unless -Continue is given, and
# stops at the first failed run. Every run is checked by check-round.ps1 against the existing
# evidence logs and the receipt table; the rows go to round-<Tag>.txt next to this script.
# Player pods rotate (16 stock pods and one add-on pod are the player; the other stock pods
# race as AI; the host check covers all 23). No screenshots: --logs-only on every run.
# Requires the host data check (check\track_check.exe) to have passed first (PLAN 4.3).
$ErrorActionPreference='Stop'
$here='D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite'
$repo='D:\Programming\GitHub\RacerRevengePC'
$saves='D:\Programming\GitHub\RacerRevengePC\work\scratch\saves'
$check=Join-Path $here 'check-round.ps1'
$summary=Join-Path $here "round-$Tag.txt"
$manifest=Join-Path $here "round-$Tag.json"

# The build: a folder with build-receipt.json, or the exe inside one.
$buildDir=if(Test-Path $Build -PathType Leaf){Split-Path -Parent (Resolve-Path $Build)}else{(Resolve-Path $Build).Path}
if(-not (Test-Path (Join-Path $buildDir 'build-receipt.json'))){throw "no build-receipt.json in $buildDir (pass the build folder or its exe)"}

if(-not $SkipHostCheck -and -not $DryRun){
  if(-not (Test-Path $HostCheckReport)){throw "host data check report missing: $HostCheckReport (run check\build_track_check.py --run --report ... first, PLAN 4.3)"}
  $last=Get-Content $HostCheckReport | Select-String -Pattern '^track_check passes=' | Select-Object -Last 1
  if(-not $last -or $last.Line -notmatch 'result=pass'){throw "host data check did not pass: $($last.Line) ($HostCheckReport)"}
}

# Shared driving: the unattended driver holds throttle, steers toward the next gate, cycles the
# camera, repairs and unsticks; the run ends 20 s after the race returns to a frontend screen.
$drive='--logs-only --hold-throttle --steer-invert --camera-cycle --repair --unstick --race-full --return-hold 20'
$single="--route main-rider-course --laps 1 --pause-restart 20 --results-exit main $drive"
$tt="--route main-rider-course --course-mode time_trial --laps 1 --results-exit main $drive"
$dlcEnv="--env RR_DLC_ROOT=$DlcRoot"

# The matrix (PLAN 5.3). save: seed file in work\scratch\saves; hall: seed slow Hall rows so the
# 1-lap time is a lap record (1AB884 Hall) - otherwise no record and 1AB908 returns to Course.
$runs=@(
 @{n=1; course='TA';mode='single';    pod=1;  ends='hall';      save='order0.bin';hall='TA';seconds=480;restart='pause';extra=$single},
 @{n=2; course='RA';mode='tournament';pod=16; ends='tournament';save='order4.bin';hall='';  seconds=720;restart='none'; extra="--route main-tournament-course-ta --results-confirm 30 $drive"; cuts=7},
 @{n=3; course='GC';mode='time_trial';pod=17; ends='hall';      save='order0.bin';hall='GC';seconds=660;restart='none'; extra="$tt --pause-quit 300"; recordQuit=1},  # QUIT after the first lap (a driven GC lap is over 170 s, cr-16). No finish restart: the original Time Trial never ends on its lap count (1B035C marks finished only for subtype 1; cr21-fix-tt.txt)
 @{n=4; course='SA';mode='single';    pod=2;  ends='course';    save='order0.bin';hall='';  seconds=480;restart='pause';extra=$single},
 @{n=5; course='MA';mode='single';    pod=3;  ends='hall';      save='order0.bin';hall='MA';seconds=480;restart='pause';extra=$single},
 @{n=6; course='GA';mode='single';    pod=4;  ends='course';    save='order0.bin';hall='';  seconds=480;restart='pause';extra=$single},
 @{n=7; course='TB';mode='single';    pod=7;  ends='hall';      save='order0.bin';hall='TB';seconds=480;restart='pause';extra=$single},
 @{n=8; course='TC';mode='single';    pod=8;  ends='course';    save='order0.bin';hall='';  seconds=480;restart='pause';extra=$single},
 @{n=9; course='MB';mode='single';    pod=9;  ends='hall';      save='order0.bin';hall='MB';seconds=480;restart='pause';extra=$single},
 @{n=10;course='SC';mode='single';    pod=10; ends='course';    save='order0.bin';hall='';  seconds=480;restart='pause';extra=$single},
 @{n=11;course='GB';mode='single';    pod=11; ends='hall';      save='order0.bin';hall='GB';seconds=480;restart='pause';extra=$single},
 @{n=12;course='MC';mode='single';    pod=12; ends='course';    save='order0.bin';hall='';  seconds=480;restart='pause';extra=$single},
 @{n=13;course='GC';mode='single';    pod=14; ends='hall';      save='order0.bin';hall='GC';seconds=480;restart='pause';extra=$single},
 @{n=14;course='RC';mode='single';    pod=15; ends='course';    save='order0.bin';hall='';  seconds=480;restart='pause';extra=$single},
 @{n=15;course='RA';mode='single';    pod=18; ends='hall';      save='order0.bin';hall='RA';seconds=480;restart='pause';extra=$single},
 @{n=16;course='NV';mode='single';    pod=0;  ends='course';    save='order0.bin';hall='';  seconds=480;restart='pause';extra="$single $dlcEnv --addon-rider arcade-ark-roose/a2r-ark"; registry=$true; addonPod='arcade-ark-roose/a2r-ark'},
 @{n=17;course='NV';mode='time_trial';pod=19; ends='course';    save='order0.bin';hall='';  seconds=600;restart='none'; extra="$tt --pause-quit 170 $dlcEnv"; registry=$true; recordQuit=0}
)

function Run-Name($r){
  $p=if($r.addonPod){'ADDON'}else{'P{0:D2}' -f $r.pod}
  'cr{0}-{1:D2}-{2}-{3}-{4}' -f $Tag,$r.n,$r.course,$r.mode,$p
}
function Run-Args($r){
  $name=Run-Name $r
  $a=@('-B',$Runner,'--build',$buildDir,'--output',(Join-Path $repo "evidence\$name"),'--assets',$Assets,'--sdl',$Sdl,'--seconds',"$($r.seconds)")
  $a+=@('--seed-save',(Join-Path $saves $r.save))
  if($r.hall){$a+=@('--seed-slow-hall',$r.hall)}
  if(-not $r.addonPod){$a+=@('--player-racer',"$($r.pod)")}
  # The course is picked on the Course screen (stock code or the add-on course code), never preselected.
  if($r.mode -ne 'tournament'){$a+=@('--course',$r.course.ToLower())}
  $a+=($r.extra -split ' ' | Where-Object {$_})
  ,$a
}

$selected=if($Only.Count){$runs | Where-Object {$Only -contains $_.n}}else{$runs | Where-Object {$_.n -ge $From}}
"round $Tag build=$buildDir started $(Get-Date -Format s)" | Tee-Object -FilePath $summary -Append
$records=@()
foreach($r in $selected){
  $name=Run-Name $r
  $a=Run-Args $r
  "run $($r.n) $name : $Python $($a -join ' ')" | Tee-Object -FilePath $summary -Append
  if($DryRun){continue}
  $console=Join-Path $repo "build\$name.console.txt"
  & $Python @a *> $console
  $rec=@{n=$r.n;name=$name;course=$r.course;mode=$r.mode;pod=$r.pod;addonPod=$r.addonPod;ends=$r.ends;restart=$r.restart;
         registry=[bool]$r.registry;recordQuit=$r.recordQuit;cuts=$r.cuts;laps=$(if($r.mode -eq 'tournament'){3}else{1});
         evidence=(Join-Path $repo "evidence\$name");console=$console}
  $records+=$rec
  $ckArgs=@{Run=$rec.evidence;Course=$r.course;Mode=$r.mode;Pod=$r.pod;Ends=$r.ends;Restart=$r.restart;Laps=$rec.laps;Summary=$summary}
  if($r.registry){$ckArgs.Registry=$true}
  if($null -ne $r.recordQuit){$ckArgs.RecordQuit=$r.recordQuit}
  if($r.cuts){$ckArgs.Cuts=$r.cuts}
  & $check @ckArgs
  $ok=$LASTEXITCODE -eq 0
  ($records | ConvertTo-Json -Depth 4) | Set-Content -Path $manifest -Encoding utf8
  if(-not $ok){"STOP: run $($r.n) $name failed (see the row above and $console)" | Tee-Object -FilePath $summary -Append;exit 1}
  if($r.n -eq 3 -and -not $Continue -and -not $Only.Count){
    "GATE: runs 1-3 passed. Resume with -From 4 (or run everything with -Continue)." | Tee-Object -FilePath $summary -Append;exit 0}
}
"round $Tag finished $(Get-Date -Format s)" | Tee-Object -FilePath $summary -Append
exit 0
