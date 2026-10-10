param([ValidateSet('x64','x86','both')][string]$Architecture='both')
$ErrorActionPreference='Stop'
$rrRoot=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$rrVc='C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.44.35207'
$rrSdk='C:/Program Files (x86)/Windows Kits/10'
$rrEvidence=Join-Path $rrRoot ('work/impactAuthoring49Y/build-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$rrArchList=if($Architecture -eq 'both'){@('x64','x86')}else{@($Architecture)}
foreach($rrArch in $rrArchList){
 $rrBuild=Join-Path $rrEvidence $rrArch
 New-Item -ItemType Directory -Force $rrBuild | Out-Null
 $env:PATH="$rrVc/bin/HostX64/$rrArch;"+$env:PATH
 $env:INCLUDE="$rrVc/include;$rrSdk/Include/10.0.26100.0/ucrt;$rrSdk/Include/10.0.26100.0/shared;$rrSdk/Include/10.0.26100.0/um"
 $env:LIB="$rrVc/lib/$rrArch;$rrSdk/Lib/10.0.26100.0/ucrt/$rrArch;$rrSdk/Lib/10.0.26100.0/um/$rrArch"
 $rrFlags=@('/nologo','/EHsc','/std:c++17','/O2','/fp:strict','/W4','/WX')
 if($rrArch -eq 'x86'){$rrFlags+='/arch:IA32'}
 Push-Location $rrBuild
 try{
  & "$rrVc/bin/HostX64/$rrArch/cl.exe" @rrFlags /c "$rrRoot/src/assets/impact_authoring49y.cpp" /Fo:implementation.obj 2>&1 | Tee-Object -FilePath implementation.log
  if($LASTEXITCODE -ne 0){throw 'implementation compilation failed'}
  & "$rrVc/bin/HostX64/$rrArch/cl.exe" @rrFlags /c "$rrRoot/src/assets/pod_physics49n.cpp" /Fo:number.obj 2>&1 | Tee-Object -FilePath number.log
  if($LASTEXITCODE -ne 0){throw 'number compilation failed'}
  & "$rrVc/bin/HostX64/$rrArch/cl.exe" @rrFlags "$rrRoot/tests/native/impact_authoring49y.cpp" /Fo:regression.obj implementation.obj number.obj /Fe:impact_authoring49y.exe 2>&1 | Tee-Object -FilePath regression.log
  if($LASTEXITCODE -ne 0){throw 'regression compilation failed'}
  & ./impact_authoring49y.exe "$rrRoot/work/impactAuthoring49Y" 2>&1 | Tee-Object -FilePath test.log
  if($LASTEXITCODE -ne 0){throw 'regression failed'}
  @{architecture=$rrArch;flags=$rrFlags;exit=0;sources=(Get-FileHash -Algorithm SHA256 -LiteralPath "$rrRoot/src/assets/impact_authoring49y.h","$rrRoot/src/assets/impact_authoring49y.cpp","$rrRoot/tests/native/impact_authoring49y.cpp","$rrRoot/src/assets/pod_physics49n.cpp","$rrRoot/work/impactAuthoring49Y/decimal.tsv");binary=(Get-FileHash impact_authoring49y.exe -Algorithm SHA256)} | ConvertTo-Json -Depth 5 | Set-Content summary.json
 }finally{Pop-Location}
}
Write-Output $rrEvidence
