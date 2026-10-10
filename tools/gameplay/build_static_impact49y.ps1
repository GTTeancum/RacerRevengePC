param([ValidateSet('x64','x86','both')][string]$Architecture='both')
$ErrorActionPreference='Stop'
$rrRoot49y=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$rrVc49y='C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.44.35207'
$rrSdk49y='C:/Program Files (x86)/Windows Kits/10'
$rrStamp49y=Get-Date -Format 'yyyyMMdd-HHmmss-fff'
$rrEvidence49y=Join-Path $rrRoot49y "work/local49Y/static-impact/build-$rrStamp49y"
$rrArchList49y=if($Architecture -eq 'both'){@('x64','x86')}else{@($Architecture)}
foreach($rrArch49y in $rrArchList49y){
 $rrBuild49y=Join-Path $rrEvidence49y $rrArch49y
 New-Item -ItemType Directory -Force $rrBuild49y | Out-Null
 $env:PATH="$rrVc49y/bin/HostX64/$rrArch49y;"+$env:PATH
 $env:INCLUDE="$rrVc49y/include;$rrSdk49y/Include/10.0.26100.0/ucrt;$rrSdk49y/Include/10.0.26100.0/shared;$rrSdk49y/Include/10.0.26100.0/um"
 $env:LIB="$rrVc49y/lib/$rrArch49y;$rrSdk49y/Lib/10.0.26100.0/ucrt/$rrArch49y;$rrSdk49y/Lib/10.0.26100.0/um/$rrArch49y"
 $rrFlags49y=@('/nologo','/EHsc','/std:c++17','/O2','/fp:strict','/W4','/WX')
 if($rrArch49y -eq 'x86'){$rrFlags49y+='/arch:IA32'}
 Push-Location $rrBuild49y
 try{
  & "$rrVc49y/bin/HostX64/$rrArch49y/cl.exe" @rrFlags49y /c "$rrRoot49y/src/game/static_impact49y.cpp" /Fo:implementation.obj 2>&1 | Tee-Object -FilePath implementation.log
  if($LASTEXITCODE -ne 0){throw '49Y implementation compilation failed'}
  & "$rrVc49y/bin/HostX64/$rrArch49y/cl.exe" @rrFlags49y "$rrRoot49y/tests/native/static_impact49y.cpp" /Fo:regression.obj implementation.obj /Fe:static_impact49y.exe 2>&1 | Tee-Object -FilePath regression.log
  if($LASTEXITCODE -ne 0){throw '49Y regression compilation failed'}
  & ./static_impact49y.exe "$rrRoot49y/work/local49Y/static-impact/fixtures.bin" 2>&1 | Tee-Object -FilePath test.log
  if($LASTEXITCODE -ne 0){throw '49Y regression failed'}
  & "$rrVc49y/bin/HostX64/$rrArch49y/dumpbin.exe" /disasm implementation.obj | Set-Content -LiteralPath disassembly.txt
  @{architecture=$rrArch49y;flags=$rrFlags49y;exit=0;sources=(Get-FileHash -Algorithm SHA256 -LiteralPath "$rrRoot49y/src/game/static_impact49y.h","$rrRoot49y/src/game/static_impact49y.cpp","$rrRoot49y/tests/native/static_impact49y.cpp","$rrRoot49y/work/local49Y/static-impact/fixtures.bin");binary=(Get-FileHash static_impact49y.exe -Algorithm SHA256)} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath summary.json
 }finally{Pop-Location}
}
Write-Output $rrEvidence49y
