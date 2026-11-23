# Still frames outside the tests (user-allowed live-window grabs). One game at a time.
Set-Location D:\Programming\GitHub\RacerRevengePC
$py='C:\Users\smmel\AppData\Local\Programs\Python\Python312\python.exe'
$runner='D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\cr-main\tools\capture\run_windows_takeover.py'
$grab='D:\Programming\GitHub\RacerRevengePC\work\scratch\frames\fastgrab.ps1'
$dlc='D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\testers-cr21\DLC'
$c=@('--build','D:\Programming\GitHub\RacerRevengePC\build\cr-31','--assets','C:\Games\Star Wars Racer Revenge','--sdl','D:\Programming\GitHub\RacerRevengePC\toolchains\sdl2-msvc\extracted\SDL2-2.32.10\lib\x64\SDL2.dll','--seed-save','D:\Programming\GitHub\RacerRevengePC\work\scratch\saves\order0.bin','--env',"RR_DLC_ROOT=$dlc")
$drive=@('--hold-throttle','--steer-invert','--repair','--unstick','--race-full')
$runs=@(
 @{n='eyecheck-cr31-tourn-addon';grab=@(3000,110);a=@('--route','main-tournament-course-ta','--addon-rider','arcade-ark-roose/a2r-ark','--hold-tournament','60','--seconds','120')},
 @{n='eyecheck-cr31-garage-addon';grab=@(3000,140);a=@('--route','main-tournament-course-ta','--addon-rider','arcade-ark-roose/a2r-ark','--hold-garage','60','--garage-from','tournament','--seconds','150')},
 @{n='eyecheck-cr31-gb-flash';grab=@(100,45);a=(@('--player-racer','11','--course','gb','--route','main-rider-course','--laps','1','--seconds','90')+$drive)}
)
foreach($r in $runs){
  $g=Start-Process powershell -ArgumentList @('-NoProfile','-ExecutionPolicy','Bypass','-File',$grab,'-Match',$r.n,'-IntervalMs',"$($r.grab[0])",'-Seconds',"$($r.grab[1])",'-WaitMinutes','4') -PassThru -WindowStyle Hidden
  & $py -B $runner @c --output "evidence\$($r.n)" @($r.a) *> "evidence\$($r.n).console.txt"
  "$($r.n) exit $LASTEXITCODE $(Get-Date -Format s)"
  Start-Sleep 3; if(-not $g.HasExited){Stop-Process -Id $g.Id -Force}
}
"frames31 done"
