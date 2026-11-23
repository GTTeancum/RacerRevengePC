$t='D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite'
$dlc='D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\testers-cr21\DLC'
$runner='D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\cr-main\tools\capture\run_windows_takeover.py'
$py='C:\Users\smmel\AppData\Local\Programs\Python\Python312\python.exe'
$c=@('--build','D:\Programming\GitHub\RacerRevengePC\build\cr-29','--assets','C:\Games\Star Wars Racer Revenge','--sdl','D:\Programming\GitHub\RacerRevengePC\toolchains\sdl2-msvc\extracted\SDL2-2.32.10\lib\x64\SDL2.dll','--logs-only','--seed-save','D:\Programming\GitHub\RacerRevengePC\work\scratch\saves\order0.bin')
$drive=@('--hold-throttle','--steer-invert','--camera-cycle','--repair','--unstick','--race-full')
$e='D:\Programming\GitHub\RacerRevengePC\evidence'
foreach($n in 3,16,9,11,1){
  powershell -NoProfile -ExecutionPolicy Bypass -File "$t\round.ps1" -Build D:\Programming\GitHub\RacerRevengePC\build\cr-29 -Tag 29 -SkipHostCheck -Only $n -Runner $runner -DlcRoot $dlc | Out-Null
  "run $n exit $LASTEXITCODE $(Get-Date -Format s)"
}
foreach($code in 'nm','nd'){
  & $py -B $runner @c @drive --output "$e\cr29-$code-single" --seconds 420 --player-racer 9 --course $code --route main-rider-course --laps 1 --results-exit main --return-hold 15 --env RR_DLC_ROOT=$dlc *> "$e\cr29-$code-single.console.txt"; "$code $LASTEXITCODE $(Get-Date -Format s)"
}
"round29 done $(Get-Date -Format s)"
