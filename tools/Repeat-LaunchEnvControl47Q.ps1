param([string]$AfterRace='evidence/launchEnv-race47Q-20260930-164842-029')
$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
$rrAfter=Join-Path $rrRoot "$AfterRace/race.exit"
while(-not (Test-Path $rrAfter)){Start-Sleep -Seconds 3}
if((Get-Content $rrAfter).Trim() -ne '0'){throw 'Initial pair state validation failed'}
# Let the queued short 47R startup comparisons finish before reserving a clean run.
$rrStartup=Join-Path $rrRoot 'evidence/47R/startup.exit'
while(-not (Test-Path $rrStartup)){Start-Sleep -Seconds 3}
if((Get-Content $rrStartup).Trim() -ne '0'){throw '47R startup failed; inspect before repeat'}
$rrEvidence=Join-Path $rrRoot ('evidence/launchEnv-repeat47Q-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrPair=Get-Content (Join-Path $rrRoot "$AfterRace/runs.json") -Raw | ConvertFrom-Json
$rrCandidate=($rrPair | Where-Object {$_.mode -eq '1'}).run
$rrExe=Join-Path $rrRoot "$AfterRace/bin/racer_launch47q_fixed_probe.exe"
$rrHash=(Get-FileHash $rrExe).Hash
@{start_utc=[DateTime]::UtcNow.ToString('o');reason='Repeat control because initial source scan overlapped timing window';candidate=$rrCandidate;binary_sha256=$rrHash;status='pending'} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'queued.json')
Write-Output "Clean control repeat: $rrEvidence"
try {
    & (Join-Path $PSScriptRoot 'Run-Native47Q.ps1') -DiagnosticCache 0 -DisableGsProfile -Seconds 2400 -Vblank 7000 -QuietTrace -BinarySource $rrExe | Tee-Object (Join-Path $rrEvidence 'run.log')
    $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter 'native-run47q-0-0-1-1-1-1-*' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    @{run=$rrRun.FullName;candidate=$rrCandidate} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'run.json')
    $rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
    & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') $rrCandidate $rrRun.FullName --output (Join-Path $rrEvidence 'state.json') *> (Join-Path $rrEvidence 'state.log')
    if($LASTEXITCODE -ne 0){throw 'Clean control saved state differs'}
    & $rrPython (Join-Path $PSScriptRoot 'analyze_fixed_pair47i.py') --baseline $rrRun.FullName --candidate $rrCandidate --expected-sha256 $rrHash --output (Join-Path $rrEvidence 'timing.json') *> (Join-Path $rrEvidence 'timing.log')
    if($LASTEXITCODE -ne 0){throw 'Clean timing analysis failed'}
    0 | Set-Content (Join-Path $rrEvidence 'race.exit')
} catch {$_ | Out-String | Set-Content (Join-Path $rrEvidence 'error.log');1 | Set-Content (Join-Path $rrEvidence 'race.exit');throw}
