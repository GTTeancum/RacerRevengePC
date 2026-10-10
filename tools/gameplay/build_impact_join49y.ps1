param([ValidateSet('x64','x86')][string]$Architecture='x64',[ValidateSet('vehicle_impact49y','vehicle_impact_consumers49z','vehicle_impact_repair50a')][string]$TestName='vehicle_impact49y')
$ErrorActionPreference='Stop'
$rrRoot=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$rrVc='C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.44.35207'
$rrSdk='C:/Program Files (x86)/Windows Kits/10'
$rrOut=Join-Path $rrRoot ('evidence/frameImpact49Y/build-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff')+'-'+$Architecture+'-'+$TestName)
New-Item -ItemType Directory -Force $rrOut | Out-Null
$env:PATH="$rrVc/bin/HostX64/$Architecture;"+$env:PATH
$env:INCLUDE="$rrVc/include;$rrSdk/Include/10.0.26100.0/ucrt;$rrSdk/Include/10.0.26100.0/shared;$rrSdk/Include/10.0.26100.0/um"
$env:LIB="$rrVc/lib/$Architecture;$rrSdk/Lib/10.0.26100.0/ucrt/$Architecture;$rrSdk/Lib/10.0.26100.0/um/$Architecture"
$rrFlags=@('/nologo','/EHsc','/std:c++20','/O2','/Ob2','/DNDEBUG','/MT','/fp:strict','/utf-8')
if($Architecture -eq 'x86'){$rrFlags+='/arch:IA32'}
Push-Location $rrRoot
try {
 & "$rrVc/bin/HostX64/$Architecture/cl.exe" @rrFlags "tests/native/$TestName.cpp" "/Fo:$rrOut/test.obj" "build/native-frame49W-$Architecture/racer_native_vehicle_frame.lib" "build/native-pc48K-$Architecture/racer_native_zlib.lib" "/Fe:$rrOut/$TestName.exe" 2>&1 | Tee-Object -FilePath "$rrOut/compile.log"
 if($LASTEXITCODE -ne 0){throw 'joined compilation failed'}
 if($TestName -eq 'vehicle_impact49y'){& "$rrOut/$TestName.exe" "$rrOut/source-proof.log" 2>&1 | Tee-Object -FilePath "$rrOut/test.log"}else{& "$rrOut/$TestName.exe" 2>&1 | Tee-Object -FilePath "$rrOut/test.log"}
 if($LASTEXITCODE -ne 0){throw 'joined test failed'}
} finally {Pop-Location}
Write-Output $rrOut



