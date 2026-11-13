param([string]$build,[string]$out,[string]$assets,[int]$racer=3,[int]$seconds=600,[string]$extra='--repair --unstick --destroy 1@3,2@3,3@3,4@3,5@3,6@3,7@3 --results-confirm 30')
# Muted, logs-only silo run (SC tournament route via order8 seed save) from the worktree runner.
$s='D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo'
Set-Location "$s\tree"
$a="$s\$assets"
$args2=@('-B','tools/capture/run_windows_takeover.py','--build',"build/$build",'--output',"evidence/$out",'--assets',$a,
 '--sdl','D:\Programming\GitHub\RacerRevengePC\toolchains\sdl2-msvc\extracted\SDL2-2.32.10\lib\x64\SDL2.dll','--logs-only',
 '--route','main-tournament-course-ta','--seed-save','D:\Programming\GitHub\RacerRevengePC\work\scratch\saves\order8.bin',
 '--player-racer',"$racer",'--seconds',"$seconds",'--hold-throttle','--steer-invert','--camera-cycle','--race-full')+($extra -split ' ' | Where-Object {$_})
& 'C:\Users\smmel\AppData\Local\Programs\Python\Python312\python.exe' @args2 > "$s\run-$out.log" 2>&1
$l="$s\tree\evidence\$out\RacerRevenge.log"
"exit=$LASTEXITCODE"
if(Test-Path $l){
 Select-String -Path $l -Pattern 'converted_content|original_loading_assets background=LD_|ORIGINAL RACER 0|require_failed|NATIVE_UNHANDLED|native_terminate|frontend failure|player_finished=1|^race_ack|^tournament_route' | Select-Object -First 25 | ForEach-Object { $_.Line.Substring(0,[Math]::Min(260,$_.Line.Length)) }
 $g=(Select-String -Path $l -Pattern '^green_trace phase=1').Count; "green_trace_rows=$g"
 $fps=(Select-String -Path $l -Pattern '^window_submit_fps (\S+)' | ForEach-Object { [double]$_.Matches[0].Groups[1].Value })
 if($fps){"fps windows=$($fps.Count) avg=$([math]::Round(($fps|Measure-Object -Average).Average,1)) min=$([math]::Round(($fps|Measure-Object -Minimum).Minimum,1)) max=$([math]::Round(($fps|Measure-Object -Maximum).Maximum,1))"}
 Select-String -Path $l -Pattern '^race phase' | Select-Object -Last 1 | ForEach-Object { $_.Line.Substring(0,[Math]::Min(200,$_.Line.Length)) }
 Select-String -Path $l -Pattern 'sealed_laps=\d+' -AllMatches | ForEach-Object { $_.Matches | ForEach-Object { $_.Value } } | Sort-Object -Unique
}
