param([ValidateSet('x64','x86','both')][string]$Architecture='both')
$ErrorActionPreference='Stop'
$rrRoot50b=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$rrVc50b='C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.44.35207'
$rrSdk50b='C:/Program Files (x86)/Windows Kits/10'
$rrStamp50b=Get-Date -Format 'yyyyMMdd-HHmmss-fff'
$rrEvidence50b=Join-Path $rrRoot50b "work/local50B/impact-effects/build-$rrStamp50b"
$rrArchList50b=if($Architecture -eq 'both'){@('x64','x86')}else{@($Architecture)}
foreach($rrArch50b in $rrArchList50b){
 $rrBuild50b=Join-Path $rrEvidence50b $rrArch50b
 New-Item -ItemType Directory -Force $rrBuild50b | Out-Null
 $env:PATH="$rrVc50b/bin/HostX64/$rrArch50b;"+$env:PATH
 $env:INCLUDE="$rrVc50b/include;$rrSdk50b/Include/10.0.26100.0/ucrt;$rrSdk50b/Include/10.0.26100.0/shared;$rrSdk50b/Include/10.0.26100.0/um"
 $env:LIB="$rrVc50b/lib/$rrArch50b;$rrSdk50b/Lib/10.0.26100.0/ucrt/$rrArch50b;$rrSdk50b/Lib/10.0.26100.0/um/$rrArch50b"
 $rrFlags50b=@('/nologo','/EHsc','/std:c++17','/O2','/fp:strict','/W4','/WX')
 if($rrArch50b -eq 'x86'){$rrFlags50b+='/arch:IA32'}
 Push-Location $rrBuild50b
 try{
  & "$rrVc50b/bin/HostX64/$rrArch50b/cl.exe" @rrFlags50b /c "$rrRoot50b/src/game/impact_effects50b.cpp" /Fo:implementation.obj 2>&1 | Tee-Object -FilePath implementation.log
  if($LASTEXITCODE -ne 0){throw '50B implementation compilation failed'}
  & "$rrVc50b/bin/HostX64/$rrArch50b/cl.exe" @rrFlags50b /c "$rrRoot50b/src/game/scene_graph49p.cpp" /Fo:scene_graph.obj 2>&1 | Tee-Object -FilePath scene_graph.log
  if($LASTEXITCODE -ne 0){throw "scene graph compilation failed"}
  & "$rrVc50b/bin/HostX64/$rrArch50b/cl.exe" @rrFlags50b "$rrRoot50b/tests/native/impact_effects50b.cpp" /Fo:regression.obj implementation.obj scene_graph.obj /Fe:impact_effects50b.exe 2>&1 | Tee-Object -FilePath regression.log
  if($LASTEXITCODE -ne 0){throw '50B regression compilation failed'}
  & ./impact_effects50b.exe "$rrRoot50b/work/local50B/impact-effects/fixtures.bin" 2>&1 | Tee-Object -FilePath test.log
  if($LASTEXITCODE -ne 0){throw '50B regression failed'}
  & "$rrVc50b/bin/HostX64/$rrArch50b/dumpbin.exe" /disasm implementation.obj | Set-Content -LiteralPath disassembly.txt
  @{architecture=$rrArch50b;flags=$rrFlags50b;exit=0;sources=(Get-FileHash -Algorithm SHA256 -LiteralPath "$rrRoot50b/src/game/impact_effects50b.h","$rrRoot50b/src/game/impact_effects50b.cpp","$rrRoot50b/tests/native/impact_effects50b.cpp","$rrRoot50b/work/local50B/impact-effects/fixtures.bin");binary=(Get-FileHash impact_effects50b.exe -Algorithm SHA256)} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath summary.json
 }finally{Pop-Location}
}
Write-Output $rrEvidence50b



