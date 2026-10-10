$ErrorActionPreference='Stop'
$rrRoot49y=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$rrVc49y='C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.44.35207'
$rrSdk49y='C:/Program Files (x86)/Windows Kits/10'
$env:PATH="$rrVc49y/bin/HostX64/x64;"+$env:PATH
$env:INCLUDE="$rrVc49y/include;$rrSdk49y/Include/10.0.26100.0/ucrt;$rrSdk49y/Include/10.0.26100.0/shared;$rrSdk49y/Include/10.0.26100.0/um"
$env:LIB="$rrVc49y/lib/x64;$rrSdk49y/Lib/10.0.26100.0/ucrt/x64;$rrSdk49y/Lib/10.0.26100.0/um/x64"
$rrSource49y=Get-Content -Raw -LiteralPath "$rrRoot49y/src/game/static_impact49y.cpp"
$rrMutations49y=@(
 @('threshold-last','chosen=f.read(in.tuning.severities14[i]);break;','chosen=f.read(in.tuning.severities14[i]);'),
 @('ignore-defense','f.sub(1.0f,in.reductionCF0)','1.0f'),
 @('wrong-front-cone','normal[2]<-0.75f','normal[2]<-0.85f'),
 @('lose-impact-sign','s.impactBA0[i]=in.accumulatedImpact1C0[i]','s.impactBA0[i]=std::fabs(in.accumulatedImpact1C0[i])'),
 @('overwrite-ties',')>std::fabs(f.read(s.impactBA0[i]))',')>=std::fabs(f.read(s.impactBA0[i]))')
)
$rrResults49y=@()
foreach($rrMutation49y in $rrMutations49y){
 if(!$rrSource49y.Contains($rrMutation49y[1])){throw 'Mutation anchor missing'}
 $rrDir49y=Join-Path $rrRoot49y "work/local49Y/static-impact/mutations/$($rrMutation49y[0])"
 New-Item -ItemType Directory -Force $rrDir49y | Out-Null
 $rrSource49y.Replace($rrMutation49y[1],$rrMutation49y[2]) | Set-Content -LiteralPath "$rrDir49y/mutant.cpp"
 Push-Location $rrDir49y
 try{
  & "$rrVc49y/bin/HostX64/x64/cl.exe" /nologo /EHsc /std:c++17 /O2 /fp:strict /I"$rrRoot49y/src/game" mutant.cpp "$rrRoot49y/tests/native/static_impact49y.cpp" /Fe:mutant.exe 2>&1 | Tee-Object -FilePath build.log
  if($LASTEXITCODE -ne 0){throw 'Mutation compile failed, not a test detection'}
  & ./mutant.exe "$rrRoot49y/work/local49Y/static-impact/fixtures.bin" 2>&1 | Tee-Object -FilePath test.log
  $rrExit49y=$LASTEXITCODE
  if($rrExit49y -ne 1){throw "Mutation not detected: $($rrMutation49y[0])"}
  $rrResults49y+=@{name=$rrMutation49y[0];testExit=$rrExit49y;source=(Get-FileHash mutant.cpp -Algorithm SHA256).Hash}
 }finally{Pop-Location}
}
$rrResults49y | ConvertTo-Json | Set-Content -LiteralPath "$rrRoot49y/work/local49Y/static-impact/mutations/summary.json"
