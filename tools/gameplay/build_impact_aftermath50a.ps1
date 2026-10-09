param([ValidateSet('x64','x86','both')][string]$Architecture='both')
$ErrorActionPreference='Stop'
$rrRoot50a=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$rrVc50a='C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.44.35207'
$rrSdk50a='C:/Program Files (x86)/Windows Kits/10'
$rrStamp50a=Get-Date -Format 'yyyyMMdd-HHmmss-fff'
$rrEvidence50a=Join-Path $rrRoot50a "work/local50A/impact-aftermath/build-$rrStamp50a"
$rrArchList50a=if($Architecture -eq 'both'){@('x64','x86')}else{@($Architecture)}
foreach($rrArch50a in $rrArchList50a){
 $rrBuild50a=Join-Path $rrEvidence50a $rrArch50a
 New-Item -ItemType Directory -Force $rrBuild50a | Out-Null
 $env:PATH="$rrVc50a/bin/HostX64/$rrArch50a;"+$env:PATH
 $env:INCLUDE="$rrVc50a/include;$rrSdk50a/Include/10.0.26100.0/ucrt;$rrSdk50a/Include/10.0.26100.0/shared;$rrSdk50a/Include/10.0.26100.0/um"
 $env:LIB="$rrVc50a/lib/$rrArch50a;$rrSdk50a/Lib/10.0.26100.0/ucrt/$rrArch50a;$rrSdk50a/Lib/10.0.26100.0/um/$rrArch50a"
 $rrFlags50a=@('/nologo','/EHsc','/std:c++17','/O2','/fp:strict','/W4','/WX')
 if($rrArch50a -eq 'x86'){$rrFlags50a+='/arch:IA32'}
 Push-Location $rrBuild50a
 try{
  & "$rrVc50a/bin/HostX64/$rrArch50a/cl.exe" @rrFlags50a /c "$rrRoot50a/src/game/impact_aftermath50a.cpp" /Fo:implementation.obj 2>&1 | Tee-Object -FilePath implementation.log
  if($LASTEXITCODE -ne 0){throw '50A implementation compilation failed'}
  & "$rrVc50a/bin/HostX64/$rrArch50a/cl.exe" @rrFlags50a "$rrRoot50a/tests/native/impact_aftermath50a.cpp" /Fo:regression.obj implementation.obj /Fe:impact_aftermath50a.exe 2>&1 | Tee-Object -FilePath regression.log
  if($LASTEXITCODE -ne 0){throw '50A regression compilation failed'}
  & ./impact_aftermath50a.exe "$rrRoot50a/work/local50A/impact-aftermath/repair-fixtures.bin" 2>&1 | Tee-Object -FilePath test.log
  if($LASTEXITCODE -ne 0){throw '50A regression failed'}
  & "$rrVc50a/bin/HostX64/$rrArch50a/dumpbin.exe" /disasm implementation.obj | Set-Content -LiteralPath disassembly.txt
  @{architecture=$rrArch50a;flags=$rrFlags50a;exit=0;sources=(Get-FileHash -Algorithm SHA256 -LiteralPath "$rrRoot50a/src/game/impact_aftermath50a.h","$rrRoot50a/src/game/impact_aftermath50a.cpp","$rrRoot50a/tests/native/impact_aftermath50a.cpp","$rrRoot50a/work/local50A/impact-aftermath/repair-fixtures.bin");binary=(Get-FileHash impact_aftermath50a.exe -Algorithm SHA256)} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath summary.json
 }finally{Pop-Location}
}
Write-Output $rrEvidence50a


