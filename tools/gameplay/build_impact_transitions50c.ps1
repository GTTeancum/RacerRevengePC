param([ValidateSet('x64','x86','both')][string]$Architecture='both')
$ErrorActionPreference='Stop'
$rrRoot50c=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$rrVc50c='C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.44.35207'
$rrSdk50c='C:/Program Files (x86)/Windows Kits/10'
$rrStamp50c=Get-Date -Format 'yyyyMMdd-HHmmss-fff'
$rrEvidence50c=Join-Path $rrRoot50c "work/local50C/impact-transitions/build-$rrStamp50c"
$rrArchList50c=if($Architecture -eq 'both'){@('x64','x86')}else{@($Architecture)}
foreach($rrArch50c in $rrArchList50c){
 $rrBuild50c=Join-Path $rrEvidence50c $rrArch50c
 New-Item -ItemType Directory -Force $rrBuild50c | Out-Null
 $env:PATH="$rrVc50c/bin/HostX64/$rrArch50c;"+$env:PATH
 $env:INCLUDE="$rrVc50c/include;$rrSdk50c/Include/10.0.26100.0/ucrt;$rrSdk50c/Include/10.0.26100.0/shared;$rrSdk50c/Include/10.0.26100.0/um"
 $env:LIB="$rrVc50c/lib/$rrArch50c;$rrSdk50c/Lib/10.0.26100.0/ucrt/$rrArch50c;$rrSdk50c/Lib/10.0.26100.0/um/$rrArch50c"
 $rrFlags50c=@('/nologo','/EHsc','/std:c++17','/O2','/fp:strict','/W4','/WX')
 if($rrArch50c -eq 'x86'){$rrFlags50c+='/arch:IA32'}
 Push-Location $rrBuild50c
 try{
  & "$rrVc50c/bin/HostX64/$rrArch50c/cl.exe" @rrFlags50c /c "$rrRoot50c/src/game/impact_transitions50c.cpp" /Fo:implementation.obj 2>&1 | Tee-Object -FilePath implementation.log
  if($LASTEXITCODE -ne 0){throw '50C implementation compilation failed'}
  & "$rrVc50c/bin/HostX64/$rrArch50c/cl.exe" @rrFlags50c "$rrRoot50c/tests/native/impact_transitions50c.cpp" /Fo:regression.obj implementation.obj /Fe:impact_transitions50c.exe 2>&1 | Tee-Object -FilePath regression.log
  if($LASTEXITCODE -ne 0){throw '50C regression compilation failed'}
  & ./impact_transitions50c.exe "$rrRoot50c/work/local50C/impact-transitions/fixtures.bin" 2>&1 | Tee-Object -FilePath test.log
  if($LASTEXITCODE -ne 0){throw '50C regression failed'}
  & "$rrVc50c/bin/HostX64/$rrArch50c/dumpbin.exe" /disasm implementation.obj | Set-Content -LiteralPath disassembly.txt
  @{architecture=$rrArch50c;flags=$rrFlags50c;exit=0;sources=(Get-FileHash -Algorithm SHA256 -LiteralPath "$rrRoot50c/src/game/impact_transitions50c.h","$rrRoot50c/src/game/impact_transitions50c.cpp","$rrRoot50c/tests/native/impact_transitions50c.cpp","$rrRoot50c/work/local50C/impact-transitions/fixtures.bin");binary=(Get-FileHash impact_transitions50c.exe -Algorithm SHA256)} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath summary.json
 }finally{Pop-Location}
}
Write-Output $rrEvidence50c


