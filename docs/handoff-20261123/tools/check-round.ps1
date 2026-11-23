param(
  [string]$Run,                       # evidence folder of one run (RacerRevenge.log, stderr.log, stdout.log)
  [string]$Course,                    # course code as data: TA..SC, RA, NV
  [ValidateSet('single','time_trial','tournament')][string]$Mode='single',
  [int]$Pod=0,                        # player racer (0 = add-on pod)
  [ValidateSet('hall','course','tournament','any')][string]$Ends='any',
  [ValidateSet('pause','tt','none')][string]$Restart='none',
  [int]$Laps=1,
  [switch]$Registry,                  # add-on course: every common_track_data line must say receipt=none
  [Nullable[int]]$RecordQuit=$null,   # Time Trial Pause QUIT: 1 = qualifies (page 3), 0 = no record
  [int]$Cuts=0,                       # expected common_track_data role=cuts value (0 = not checked)
  [double]$FpsTolerance=2.0,
  [string]$Summary='',                # append the row (and detail lines) to this file
  [string]$Round=''                   # or: check every run of round-<Round>.json
)
# Round checker of the common rewrite (PLAN 4.4 / 5.5, writer R11). Turns one run log into one
# row: pass/fail, the first refusal (step, role, owner, pc), data receipts matched over total,
# laps, the route ack sequence, Results / Hall / Course reached, archive released, FPS average,
# and the diff against that course's newest recorded evidence log (PLAN 5.1 item 3). It parses
# exactly the keys of tests\native\common_race_log50cr.h (common_race_begin, common_load,
# common_track_data, common_storage(_refused), common_roster, common_pod_receipt,
# common_race_start, common_lap, common_race_end, pause_quit_record, pause_refused,
# common_teardown, common_run_summary) plus course_archive_release, race_ack,
# frontend_race_return and window_submit_fps. Failure signatures are those of
# work\scratch\batch145\regress149.ps1. Exit 0 = pass.
$ErrorActionPreference='Stop'
$here='D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite'
$evidence='D:\Programming\GitHub\RacerRevengePC\evidence'

if($Round){
  $m=Get-Content (Join-Path $here "round-$Round.json") -Raw | ConvertFrom-Json
  $bad=0
  foreach($r in @($m)){
    $a=@{Run=$r.evidence;Course=$r.course;Mode=$r.mode;Pod=$r.pod;Ends=$r.ends;Restart=$r.restart;Laps=$r.laps}
    if($r.registry){$a.Registry=$true}
    if($null -ne $r.recordQuit){$a.RecordQuit=[int]$r.recordQuit}
    if($r.cuts){$a.Cuts=[int]$r.cuts}
    if($Summary){$a.Summary=$Summary}
    & $PSCommandPath @a
    if($LASTEXITCODE -ne 0){$bad++}
  }
  "round $Round runs=$(@($m).Count) failed=$bad"
  exit ([int]($bad -ne 0))
}
if(-not $Run -or -not $Course){throw 'usage: check-round.ps1 -Run <evidence folder> -Course <code> [-Mode ..] [-Ends ..] [-Restart ..] | -Round <tag>'}

# Newest recorded evidence logs per course, newest first (PLAN 5.1 item 3).
$baselines=@{
 'TA'=@('r149p3-TA-23','c149b-ta-restart','c148-ta-single5','c148-ta-tt5','c148-ta-pausequit7','eyecheck-pause149b-ta-quit');
 'SA'=@('r149p3-SA-18','c147-sa-r19','d146p21-fp-SA','eyecheck-stock-hall-record-sa23');
 'MA'=@('r149p3-MA-8','d146p23-fp-MA'); 'GA'=@('w149p3-GA-24','c148-ga-single5','d146p23-fp-GA');
 'TB'=@('r149p3-TB-11','w145c-TB-MB-19'); 'TC'=@('r149p3-TC-4','w145c-RC-TC-19');
 'MB'=@('r149p3-MB-7','w145c-TB-MB-19'); 'SC'=@('r146a-SC-3','r145c-SC-3'); 'GB'=@('f145c-GB-16','r145g5-GB-9');
 'MC'=@('w145c-MC-GC-19','r145g5-MC-15'); 'GC'=@('r149p3-GC-22','r146a-GC-22'); 'RC'=@('w145c-RC-TC-19','r145c-RC-20');
 'RA'=@('r143h-RA-16'); 'NV'=@('c147-nv-single','c148-nv-tt5','eyecheck-pause149b-nv-quit','eyecheck-dlc-course-record-nv4')
}
# A run's mode-matched baseline is preferred (single / time_trial / tournament), else the newest log.
function Log-Lines([string]$dir){
  $all=@()
  foreach($n in 'RacerRevenge.log','stderr.log','stdout.log'){$p=Join-Path $dir $n;if(Test-Path $p){$all+=Get-Content $p -Encoding UTF8}}
  ,$all
}
function Field([string]$line,[string]$name){
  $m=[regex]::Match($line,"(?:^| )$([regex]::Escape($name))=(""[^""]*""|\S+)")
  if($m.Success){$m.Groups[1].Value.Trim('"')}else{$null}
}
function Fps-Average($lines){
  $v=@($lines | ForEach-Object { if($_ -match '^window_submit_fps (\S+)'){[double]$Matches[1]} })
  if($v.Count){[math]::Round(($v | Measure-Object -Average).Average,1)}else{0}
}
function Run-Mode([string]$dir){
  $l=Join-Path $dir 'launch.json'
  if(-not (Test-Path $l)){return 'unknown'}
  $j=Get-Content $l -Raw | ConvertFrom-Json
  if($j.route -eq 'main-tournament-course-ta'){'tournament'}elseif($j.courseMode -eq 'time_trial'){'time_trial'}else{'single'}
}

$lines=Log-Lines $Run
$fail=New-Object System.Collections.Generic.List[string]
$note=New-Object System.Collections.Generic.List[string]
if(-not $lines.Count){$fail.Add("no log in $Run")}

# ---- clean exit and failure signatures (regress149.ps1 set, plus the common vocabulary)
$sig='require_failed|NATIVE_UNHANDLED|terminate called|Canonical Results input stopped|frontend failure|native_terminate last|_prefix_rejected|_begin_rejected|race_mount_release_refused|^common_storage_refused|^pause_refused|result=refused|course_archive_release released=0|^common_teardown .*released=0'
$sigLines=@($lines | Where-Object { $_ -match $sig })
foreach($s in ($sigLines | Select-Object -First 6)){$fail.Add('signature: '+$s.Substring(0,[Math]::Min(220,$s.Length)))}
$summaryLine=$lines | Where-Object { $_ -match '^common_run_summary ' } | Select-Object -Last 1
if(-not $summaryLine){$fail.Add('no common_run_summary line (run did not reach its clean exit)')}
elseif((Field $summaryLine 'result') -ne 'pass'){$fail.Add("common_run_summary result=$(Field $summaryLine 'result')")}
$firstRefusal=if($summaryLine){Field $summaryLine 'first_refusal'}else{$null}
if($firstRefusal -and $firstRefusal -ne 'none'){$firstRefusal+=" pc=$(Field $summaryLine 'first_refusal_pc') reason=$(Field $summaryLine 'first_refusal_reason')"}
if(-not $firstRefusal -or $firstRefusal -eq 'none'){
  $r=$lines | Where-Object { $_ -match '^common_load .*result=refused' } | Select-Object -First 1
  if($r){$firstRefusal="$(Field $r 'step')/$(Field $r 'role')/$(Field $r 'owner') pc=$(Field $r 'pc') reason=$(Field $r 'reason')"}
  elseif($sigLines.Count){$firstRefusal='signature: '+$sigLines[0].Substring(0,[Math]::Min(120,$sigLines[0].Length))}
  else{$firstRefusal='none'}
}
$released=@($lines | Where-Object { $_ -match '^course_archive_release released=1' }).Count
if(-not $released){$fail.Add('no course_archive_release released=1')}

# ---- data: common_track_data and common_pod_receipt receipts, common_load order
$data=@($lines | Where-Object { $_ -match '^common_track_data ' })
$pods=@($lines | Where-Object { $_ -match '^common_pod_receipt ' })
if(-not $data.Count){$fail.Add('no common_track_data lines')}
$want=if($Registry){'none'}else{'match'}
# A stock row fails only on receipt=differs that check\DIFFERS.txt does not explain (role
# prefixes below); receipt=none means the table has no value for that role.
$explained=@('lights.local','atm.animations','atm.frames','atm.sources','sky.cubes','sky.tiles')
$dataOk=@($data | Where-Object { (Field $_ 'receipt') -eq $want }).Count
$bad=@($data | Where-Object { $r=(Field $_ 'receipt'); if($Registry){$r -ne 'none'}else{$r -like 'differs*' -and $explained -notcontains (Field $_ 'role')} })
foreach($d in ($bad | Select-Object -First 8)){$fail.Add("track_data receipt not $want : $d")}
foreach($d in ($data | Where-Object { (Field $_ 'course') -and (Field $_ 'course').ToUpper() -ne $Course.ToUpper() } | Select-Object -First 3)){$fail.Add("track_data names another course: $d")}
# Stock pods' receipts must match; an add-on pod has none.
$podOk=@($pods | Where-Object { $p=(Field $_ 'receipt'); $p -eq 'match' -or ($p -eq 'none' -and [int](Field $_ 'pod') -ge 32) }).Count
foreach($d in ($pods | Where-Object { $p=(Field $_ 'receipt'); -not ($p -eq 'match' -or ($p -eq 'none' -and [int](Field $_ 'pod') -ge 32)) } | Select-Object -First 8)){$fail.Add("pod receipt: $d")}
if(-not $pods.Count){$fail.Add('no common_pod_receipt lines')}
$receipts="$($dataOk+$podOk)/$($data.Count+$pods.Count)"
# common_load steps in the original order within each load (a step number never goes back
# except when a new race load starts at step 01).
$prev=$null;$orderBad=0;$warn=''
foreach($l in ($lines | Where-Object { $_ -match '^common_load ' })){
  $s=Field $l 'step';if(-not $s){continue}
  $p=$s.Split('.');$key=[int]$p[0]*100+$(if($p.Count -gt 1){[int]$p[1]}else{0})
  # The umbrella line of a step (05 after its 05.NN sub-steps) and step 11 (the speech banks the
  # shared live audio loads once per process, before the track) are not order points. The port's
  # cold order runs 08/10 before the camera (06); that is recorded in W0-RULINGS, so a step going
  # back is reported as a warning, not a failure.
  if($p.Count -eq 1 -and $null -ne $prev -and [math]::Floor($prev/100) -eq [int]$p[0]){continue}
  if([int]$p[0] -eq 11){continue}
  if($null -ne $prev -and $key -lt $prev -and $key -gt 100){$orderBad++;$warn+="order:$s<$prev "}
  $prev=$key
}
$cutsLine=$data | Where-Object { (Field $_ 'role') -eq 'cuts' } | Select-Object -First 1
$cutsValue=if($cutsLine){[int](Field $cutsLine 'value')}else{-1}
if($Cuts -and $cutsValue -ne $Cuts){$fail.Add("cuts value=$cutsValue, expected $Cuts")}
$trigLine=$data | Where-Object { (Field $_ 'role') -eq 'triggers' } | Select-Object -First 1
$trigValue=if($trigLine){[int](Field $trigLine 'value')}else{-1}

# ---- race start, restart in place
$startIdx=@();$restartIdx=@();$freshIdx=@();$releaseIdx=@()
for($i=0;$i -lt $lines.Count;$i++){
  $l=$lines[$i]
  if($l -match '^common_race_start reason=fresh'){$startIdx+=$i}
  elseif($l -match '^common_race_start reason=restart'){$restartIdx+=$i}
  elseif($l -match '^Starting a fresh'){$freshIdx+=$i}
  elseif($l -match '^course_archive_release '){$releaseIdx+=$i}
}
$restartState='none'
if(-not $startIdx.Count){$fail.Add('no common_race_start reason=fresh')}
if($Restart -ne 'none'){
  $caller=if($Restart -eq 'pause'){'1BD934'}else{'1ABC0C'}
  $hit=$restartIdx | Where-Object { $l=$lines[$_]; (Field $l 'pc') -eq '1AC8E0' -and (Field $l 'caller') -eq $caller -and (Field $l 'result') -eq 'ok' } | Select-Object -First 1
  if($null -eq $hit){$fail.Add("no common_race_start reason=restart pc=1AC8E0 caller=$caller result=ok")}
  else{
    $s0=($startIdx | Where-Object { $_ -lt $hit } | Select-Object -Last 1)
    $between=@($freshIdx+$releaseIdx | Where-Object { $_ -gt $s0 -and $_ -lt $hit }).Count
    if($between){$fail.Add("restart was not in place: $between session reload/archive release line(s) between the fresh start and the restart")}
    $restartState="inplace caller=$caller"
    if($Restart -eq 'tt'){
      # 216690 keeps CB4 when 11258==3: the restart line's best lap equals the player's best before it.
      $cb4=[double](Field $lines[$hit] 'best_lap_cb4')
      $lap=$null;for($i=$hit-1;$i -ge 0;$i--){if($lines[$i] -match '^common_lap racer=0 '){$lap=$lines[$i];break}}
      $best=if($lap){[double](Field $lap 'best')}else{-1}
      if($cb4 -le 0 -or [math]::Abs($cb4-$best) -gt 0.0005){$fail.Add("Time Trial best lap not kept across the restart: best_lap_cb4=$cb4, player best before=$best")}
      $restartState+=" best_lap_cb4=$cb4"
    }
  }
}

# ---- laps (common_lap: one line per crossing)
$lapMax=@{}
foreach($l in ($lines | Where-Object { $_ -match '^common_lap ' })){
  $s=[int](Field $l 'racer');$n=[int](Field $l 'lap');if(-not $lapMax.ContainsKey($s) -or $n -gt $lapMax[$s]){$lapMax[$s]=$n}
}
$lapText=if($lapMax.Count){($lapMax.Keys | Sort-Object | ForEach-Object { "$_`:$($lapMax[$_])" }) -join ','}else{'-'}
if($Mode -ne 'time_trial'){
  if(-not $lapMax.ContainsKey(0)){$fail.Add('no common_lap line for the player (slot 0)')}
  elseif($lapMax[0] -lt $Laps){$fail.Add("player lap count $($lapMax[0]) < $Laps")}
  $over=@($lapMax.Keys | Where-Object { $lapMax[$_] -gt $Laps+1 })
  if($over.Count){$fail.Add("lap count above the race's $Laps for slot(s) $($over -join ',')")}
}

# ---- race end, Results, return
$endLines=@($lines | Where-Object { $_ -match '^common_race_end ' })
$routeEnd=($endLines | ForEach-Object { Field $_ 'route' }) -join ','
$acks=@($lines | Where-Object { $_ -match '^race_ack route=' } | ForEach-Object { Field $_ 'route' })
$returns=@($lines | Where-Object { $_ -match '^frontend_race_return first_screen=' } | ForEach-Object { [int](Field $_ 'first_screen') })
$resultsReached=@($endLines | Where-Object { (Field $_ 'route') -eq 'results' }).Count -gt 0
$ret=if($returns.Count){switch($returns[0]){11{'hall'}6{'course'}13{'garage'}7{'win'}10{'unlock'}default{"screen$_"}}}else{'none'}
switch($Ends){
  # The original shows the Unlock screen first when the race earned an unlock (cr20 TC); it is
  # accepted in place of the Hall or Course return and stays visible in the row as return=unlock.
  'hall'{ if(@('hall','unlock') -notcontains $ret){$fail.Add("returned to $ret, expected the Hall")} }
  'course'{ if(@('course','unlock') -notcontains $ret){$fail.Add("returned to $ret, expected the Course screen")} }
  'tournament'{ if(@('hall','win','unlock','garage') -notcontains $ret){$fail.Add("returned to $ret, expected a tournament screen")} }
}
if($Mode -ne 'time_trial' -and -not $resultsReached){$fail.Add('no common_race_end route=results')}
if($Mode -eq 'time_trial' -and $Restart -eq 'tt' -and @($endLines | Where-Object { (Field $_ 'route') -eq 'tt_restart' }).Count -eq 0){$fail.Add('no common_race_end route=tt_restart (Time Trial finish restart)')}
if($null -ne $RecordQuit){
  $q=$lines | Where-Object { $_ -match '^pause_quit_record ' } | Select-Object -Last 1
  if(-not $q){$fail.Add('no pause_quit_record line')}
  else{
    $qq=[int](Field $q 'qualify_1ABFC0');$pg=Field $q 'page'
    if($qq -ne $RecordQuit -or ($RecordQuit -eq 1 -and $pg -ne '3') -or ($RecordQuit -eq 0 -and $pg -ne 'none')){$fail.Add("pause_quit_record qualify_1ABFC0=$qq page=$pg, expected qualify=$RecordQuit")}
  }
}
$roster=@($lines | Where-Object { $_ -match '^common_roster ' })
$playerRow=$roster | Where-Object { (Field $_ 'human') -eq '1' } | Select-Object -First 1
if($Pod -and $playerRow -and [int](Field $playerRow 'pod') -ne $Pod){$fail.Add("player pod $(Field $playerRow 'pod'), expected $Pod")}
$fps=Fps-Average $lines

# ---- against the course's newest recorded evidence log (behaviour only)
$base=$null;$cmp='no baseline log'
$names=@($baselines[$Course.ToUpper()])
$present=@($names | Where-Object { $_ -and (Test-Path (Join-Path $evidence "$_\RacerRevenge.log")) })
$base=($present | Where-Object { (Run-Mode (Join-Path $evidence $_)) -eq $Mode } | Select-Object -First 1)
if(-not $base){$base=$present | Select-Object -First 1}
if($base){
  $bl=Log-Lines (Join-Path $evidence $base)
  $bMode=Run-Mode (Join-Path $evidence $base)
  $bFps=Fps-Average $bl
  $bTrig=$bl | Where-Object { $_ -match '^course_triggers (site|course)=' } | Select-Object -First 1
  $bTrigN=if($bTrig){[int](Field $bTrig 'count')}else{-1}
  $bCut=$bl | Where-Object { $_ -match '^course_cut_identity ' } | Select-Object -First 1
  $bCutN=if($bCut){[int](Field $bCut 'cuts')}else{-1}
  $bAcks=@($bl | Where-Object { $_ -match '^race_ack route=' } | ForEach-Object { Field $_ 'route' })
  $bRel=@($bl | Where-Object { $_ -match '^course_archive_release released=1' }).Count
  $parts=@("base=$base($bMode)")
  $parts+="fps=$fps/$bFps"
  # A slowdown is the regression; a faster run than the recorded log is not (cr-17: RA 56.9 vs 45.7).
  if($bFps -and $fps -and ($bFps-$fps) -gt $FpsTolerance){$fail.Add("FPS $fps vs $bFps in $base (more than $FpsTolerance FPS slower)")}
  if($bTrigN -ge 0){$parts+="triggers=$trigValue/$bTrigN";if($trigValue -ge 0 -and $trigValue -ne $bTrigN){$note.Add("trigger count $trigValue differs from $base ($bTrigN); the receipt table is the authority")}}
  if($bCutN -ge 0){$parts+="cuts=$cutsValue/$bCutN";if($cutsValue -ge 0 -and $cutsValue -ne $bCutN){$fail.Add("cut count $cutsValue differs from $base ($bCutN)")}}
  $parts+="released=$released/$bRel"
  if($bMode -eq $Mode){$parts+="acks=$($acks -join '>')/$($bAcks -join '>')";if($acks.Count -and $bAcks.Count -and ($acks | Select-Object -First 1) -ne ($bAcks | Select-Object -First 1)){$note.Add("first race_ack differs from ${base}: $($acks | Select-Object -First 1) vs $($bAcks | Select-Object -First 1) (see PLAN 5.6 for expected differences)")}}
  else{$parts+="acks=n/a(base is $bMode)"}
  $cmp=$parts -join ' '
}

$result=if($fail.Count){'FAIL'}else{'PASS'}
$name=Split-Path -Leaf $Run
$row="$result $name course=$Course mode=$Mode pod=$Pod first_refusal=[$firstRefusal] receipts=$receipts laps=$lapText routes=end:$routeEnd/ack:$($acks -join '>') results=$([int]$resultsReached) return=$ret restart=$restartState archive_released=$released fps_avg=$fps order_warn=[$warn] | $cmp"
$out=@($row)+($fail | ForEach-Object { "  FAIL $_" })+($note | ForEach-Object { "  NOTE $_" })
$out | ForEach-Object { $_ }
if($Summary){$out | Add-Content -Path $Summary -Encoding utf8}
exit ([int]($fail.Count -ne 0))
