param([ValidateSet('x64','x86','both')][string]$Architecture='both')
$ErrorActionPreference='Stop'
$rrRoot49z=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$rrVc49z='C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.44.35207'
$rrSdk49z='C:/Program Files (x86)/Windows Kits/10'
$rrStamp49z=Get-Date -Format 'yyyyMMdd-HHmmss-fff'
$rrEvidence49z=Join-Path $rrRoot49z "work/local49Z/impact-consumers/build-$rrStamp49z"
$rrArchList49z=if($Architecture -eq 'both'){@('x64','x86')}else{@($Architecture)}
foreach($rrArch49z in $rrArchList49z){
 $rrBuild49z=Join-Path $rrEvidence49z $rrArch49z
 New-Item -ItemType Directory -Force $rrBuild49z | Out-Null
 $env:PATH="$rrVc49z/bin/HostX64/$rrArch49z;"+$env:PATH
 $env:INCLUDE="$rrVc49z/include;$rrSdk49z/Include/10.0.26100.0/ucrt;$rrSdk49z/Include/10.0.26100.0/shared;$rrSdk49z/Include/10.0.26100.0/um"
 $env:LIB="$rrVc49z/lib/$rrArch49z;$rrSdk49z/Lib/10.0.26100.0/ucrt/$rrArch49z;$rrSdk49z/Lib/10.0.26100.0/um/$rrArch49z"
 $rrFlags49z=@('/nologo','/EHsc','/std:c++17','/O2','/fp:strict','/W4','/WX')
 if($rrArch49z -eq 'x86'){$rrFlags49z+='/arch:IA32'}
 Push-Location $rrBuild49z
 try{
  & "$rrVc49z/bin/HostX64/$rrArch49z/cl.exe" @rrFlags49z /c "$rrRoot49z/src/game/impact_consumers49z.cpp" /Fo:implementation.obj 2>&1 | Tee-Object -FilePath implementation.log
  if($LASTEXITCODE -ne 0){throw '49Z implementation compilation failed'}
  & "$rrVc49z/bin/HostX64/$rrArch49z/cl.exe" @rrFlags49z "$rrRoot49z/tests/native/impact_consumers49z.cpp" /Fo:regression.obj implementation.obj /Fe:impact_consumers49z.exe 2>&1 | Tee-Object -FilePath regression.log
  if($LASTEXITCODE -ne 0){throw '49Z regression compilation failed'}
  & ./impact_consumers49z.exe "$rrRoot49z/work/local49Z/impact-consumers/fixtures.bin" 2>&1 | Tee-Object -FilePath test.log
  if($LASTEXITCODE -ne 0){throw '49Z regression failed'}
  & "$rrVc49z/bin/HostX64/$rrArch49z/dumpbin.exe" /disasm implementation.obj | Set-Content -LiteralPath disassembly.txt
  @{architecture=$rrArch49z;flags=$rrFlags49z;exit=0;sources=(Get-FileHash -Algorithm SHA256 -LiteralPath "$rrRoot49z/src/game/impact_consumers49z.h","$rrRoot49z/src/game/impact_consumers49z.cpp","$rrRoot49z/tests/native/impact_consumers49z.cpp","$rrRoot49z/work/local49Z/impact-consumers/fixtures.bin");binary=(Get-FileHash impact_consumers49z.exe -Algorithm SHA256)} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath summary.json
 }finally{Pop-Location}
}
Write-Output $rrEvidence49z

