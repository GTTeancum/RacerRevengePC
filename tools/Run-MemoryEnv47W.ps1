$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot 'evidence/47W';$rrGate=Join-Path $rrEvidence 'build-gate.json'
while(-not (Test-Path $rrGate)){Start-Sleep -Seconds 5}
$rrBuildGate=Get-Content $rrGate -Raw | ConvertFrom-Json
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
$rrPrior=Get-ChildItem (Join-Path $rrRoot 'evidence') -Directory -Filter 'captureOff47V-*' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
$rrReferences=Get-Content (Join-Path $rrPrior.FullName 'runs.json') -Raw | ConvertFrom-Json
$rrBin=Join-Path $rrEvidence 'bin';New-Item -ItemType Directory -Path $rrBin | Out-Null
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
$rrExe=Join-Path $rrBin 'racer_memoryenv47w_fixed_probe.exe';Copy-Item (Join-Path $rrBuild 'racer_memoryenv47w_fixed_probe.exe') $rrExe
Get-ChildItem $rrBuild -Filter '*.dll' | ForEach-Object {Copy-Item $_.FullName $rrBin}
$rrHash=(Get-FileHash $rrExe).Hash.ToLowerInvariant();if($rrHash -ne $rrBuildGate.binary_sha256){throw 'Frozen memory candidate differs'}
$rrRuns=@()
try {
    foreach($rrStop in @(500,7000)){
        foreach($rrMode in @('0','1')){
            & (Join-Path $PSScriptRoot 'Run-Native47W.ps1') -Seconds 2400 -Vblank $rrStop -MemoryDiagnostics $rrMode -DiagnosticLines 1 -EventGate 1 -DiagnosticCache 1 -DisableGsProfile -DisableVuCapture -QuietTrace -BinarySource $rrExe | Tee-Object (Join-Path $rrEvidence "$rrStop-$rrMode-run.log")
            $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter "native-run47w-$rrMode-1-1-1-1-0-1-1-1-1-*" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
            $rrRuns+=@{stop=$rrStop;mode=$rrMode;run=$rrRun.FullName};$rrRuns | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'runs.json')
            $rrReference=($rrReferences | Where-Object stop -eq $rrStop).run
            & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') $rrReference $rrRun.FullName --output (Join-Path $rrEvidence "$rrStop-$rrMode-state.json") *> (Join-Path $rrEvidence "$rrStop-$rrMode-state.log")
            $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "$rrStop-$rrMode-state.exit")
            if($rrCode -ne 0){throw 'Memory diagnostic candidate changed native state'}
            $rrState=Get-Content (Join-Path $rrEvidence "$rrStop-$rrMode-state.json") -Raw | ConvertFrom-Json
            if(-not $rrState.files.'mixed.s16le'.equal -or -not $rrState.files.'final.ppm'.equal){throw 'Memory diagnostic candidate changed audio or final image'}
        }
        0 | Set-Content (Join-Path $rrEvidence "$rrStop-pair.exit")
    }
    $rrBase=($rrRuns | Where-Object {$_.stop -eq 7000 -and $_.mode -eq '0'}).run
    $rrCandidate=($rrRuns | Where-Object {$_.stop -eq 7000 -and $_.mode -eq '1'}).run
    & $rrPython (Join-Path $PSScriptRoot 'analyze_fixed_pair47i.py') --baseline $rrBase --candidate $rrCandidate --expected-sha256 $rrHash --output (Join-Path $rrEvidence 'timing.json') *> (Join-Path $rrEvidence 'timing.log')
    if($LASTEXITCODE -ne 0){throw 'Memory candidate timing analysis failed'}
    0 | Set-Content (Join-Path $rrEvidence 'native.exit')
} catch {$_ | Out-String | Set-Content (Join-Path $rrEvidence 'native-error.log');1 | Set-Content (Join-Path $rrEvidence 'native.exit');throw}
