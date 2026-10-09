param(
    [ValidateRange(1,7200)][int]$Seconds=1800,
    [ValidateRange(0,4096)][int]$VuCount=0,
    [ValidateRange(1,2147483647)][int]$VuStart=1,
    [string]$ButtonScript='',
    [switch]$IncludeResume,
    [switch]$VuArm,
    [switch]$ProfileHost,
    [switch]$QuietTrace,
    [ValidateSet('0','1')][string]$ReadyCache='0',
    [ValidateSet('0','1')][string]$IdleBatch='1',
    [ValidateRange(1,10000000)][int]$Vblank=7000,
    [string]$BinarySource=''
)
$ErrorActionPreference='Stop'
$rrRoot = Split-Path -Parent $PSScriptRoot
$rrWork = Join-Path $rrRoot 'work'
if((Get-Content (Join-Path $rrRoot 'evidence/47H/build.exit')).Trim() -ne '0'){throw '47H IOP-cache build is not verified'}
if((Get-Content (Join-Path $rrRoot 'evidence/47B/build-native3.exit')).Trim() -ne '0'){throw '47B2 build is not verified'}
$Variant='iop47h'
$Mode=$ReadyCache+'-'+$IdleBatch
$env:RR_IOP_READY_CACHE47G=$ReadyCache
$env:RR_IOP_READY_STATS47G='1'
$env:RR_IOP_IDLE_BATCH=$IdleBatch
$env:RR_STOP_AT_VBLANK47F=[string]$Vblank
$env:RR_IOP_IDLE_STATS='1'
function Get-CardManifest {
    $files=@(Get-ChildItem (Join-Path $rrWork 'native/project/disc/mc0'),(Join-Path $rrWork 'native/project/disc/mc1') -File -Recurse -Force | Sort-Object FullName)
    $records=@($files | ForEach-Object { @{path=$_.FullName;bytes=$_.Length;sha256=(Get-FileHash -LiteralPath $_.FullName).Hash} })
    ConvertTo-Json -InputObject $records -Depth 4 -Compress
}
$cardBefore=Get-CardManifest

$project = Join-Path $rrWork 'native\project'
$exe = Join-Path $rrWork 'native\ninja-build-release-opt\racer_iop47h_fixed_probe.exe'
if($BinarySource){$exe=(Resolve-Path -LiteralPath $BinarySource).Path}
if((Get-Content (Join-Path $rrWork 'build-native41y.exit')).Trim() -ne '0'){throw 'Require final post-routing-fix native build41Y exit0'}
foreach($gate in @('build-native44f.exit','build-native44i.exit','test-microvu44e.exit','test-native-bridge44g.exit')){if((Get-Content (Join-Path $rrWork $gate)).Trim() -ne '0'){throw "Require actual0: $gate"}}
if((Get-Content (Join-Path $rrWork 'test-native-vu44x.exit')).Trim() -ne '0'){throw 'Require corrected adapter241checks actual0'}
if((Get-Content (Join-Path $rrWork 'native-run44k-20260930-094844-155/native.exit')).Trim() -ne '0'){throw 'Require previous pilot terminal0'}
if((Get-Content (Join-Path $rrWork 'native-run44z-20260930-101748-875/native.exit')).Trim() -ne '0'){throw 'Require corrected previous pilot terminal0'}
foreach($gate in @('build-native45v.exit','test-iop-capture45v.exit')){if((Get-Content (Join-Path $rrWork $gate)).Trim() -ne '0'){throw "Required gate: $gate"}}
if((Get-Content (Join-Path $rrWork 'native-run45l-20260930-110710-362/native.exit')).Trim() -ne '0'){throw 'Require prior pilot terminal0'}
$env:RR_PCSX2_VU44A='1'
$run = Join-Path $rrWork (('native-run47h-'+$Mode+'-')+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $run | Out-Null
$env:RR_CAPTURE_IOP45N=Join-Path $run 'iop-block'

# Preserve the exact tested binary and input before launch; subsequent builds may
# replace the build-tree output, but this diagnostic run remains hash-stable.
Copy-Item -LiteralPath $exe -Destination (Join-Path $run 'racer_native_probe.exe')
# Keep FFmpeg-enabled diagnostic runs self-contained: the probe is copied out of
# the build tree, so staged runtime DLLs must accompany it for Windows loading.
$buildDir = Split-Path -Parent $exe
Get-ChildItem -LiteralPath $buildDir -Filter '*.dll' -File -ErrorAction SilentlyContinue | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $run $_.Name) -Force
}
if(-not $ButtonScript){$ButtonScript=Join-Path $rrWork 'native-run45z-20260930-112950-077/buttons.txt'}
Copy-Item -LiteralPath $ButtonScript -Destination (Join-Path $run 'buttons.txt')

Remove-Item Env:RR_CAPTURE_GS_STREAM33P -ErrorAction SilentlyContinue
Remove-Item Env:RR_GS_GPU33T_SHADER -ErrorAction SilentlyContinue
$bridgeSource=Join-Path $rrWork 'pcsx2-integration40s\build\pcsx2-gsrunner\racer_pcsx2_bridge44r.dll'
$env:RR_PCSX2_GS_DLL=Join-Path $run 'racer_pcsx2_bridge44r.dll'
Copy-Item -LiteralPath $bridgeSource -Destination $env:RR_PCSX2_GS_DLL
Copy-Item -LiteralPath (Join-Path $rrWork 'reference\pcsx2-master\bin\resources') -Destination (Join-Path $run 'resources') -Recurse
$env:PATH=(Join-Path $rrWork 'pcsx2-integration40s\deps\bin')+';'+$env:PATH
Get-FileHash -LiteralPath $env:RR_PCSX2_GS_DLL -Algorithm SHA256 | ConvertTo-Json | Set-Content (Join-Path $run 'bridge-hash.json')
$env:RR_CD_IMAGE = Join-Path $project 'original\RacerRevenge.iso'
$env:RR_VIRTUAL_PAD='1'
$env:RR_PAD_SCRIPT=Join-Path $run 'buttons.txt'
$env:RR_TRACE_ASSERT='1'
Remove-Item Env:RR_TRACE_SPU_IRQ -ErrorAction SilentlyContinue
Remove-Item Env:RR_TRACE_VIF_IRQ -ErrorAction SilentlyContinue
Remove-Item Env:RR_CAPTURE_VIF_SEQUENCE33H -ErrorAction SilentlyContinue
$env:RR_TRACE_IOP_SIO2='1'
$env:RR_TRACE_IOP_RPC_THREAD='1'
$env:RR_TRACE_CDVD='1'
if($QuietTrace){
 foreach($trace in @('RR_TRACE_IOP_SIO2','RR_TRACE_IOP_RPC_THREAD','RR_TRACE_CDVD')){Remove-Item "Env:$trace" -ErrorAction SilentlyContinue}
}
if($VuArm){$env:RR_CAPTURE_VU_ARM=Join-Path $run 'vu.arm'}else{Remove-Item Env:RR_CAPTURE_VU_ARM -ErrorAction SilentlyContinue}
if($ProfileHost){$env:RR_PROFILE_HOST=Join-Path $run 'host-profile.csv'}else{Remove-Item Env:RR_PROFILE_HOST -ErrorAction SilentlyContinue}
$env:RR_CAPTURE_VU=Join-Path $run 'vu'
$env:RR_CAPTURE_VU_COUNT="$VuCount"
$env:RR_CAPTURE_VU_START="$VuStart"
$env:RR_CAPTURE_VU_RESUME=if($IncludeResume -or $VuArm){'1'}else{'0'}
$env:RR_CAPTURE_FRAME=Join-Path $run 'final.ppm'
$env:RR_CAPTURE_SEQUENCE=Join-Path $run 'frame'
$env:RR_CAPTURE_SEQUENCE_MAX='360'
$env:RR_CAPTURE_MATERIAL=Join-Path $run 'material'
$env:RR_CAPTURE_MATERIAL_COUNT='64'
Remove-Item Env:RR_TRACE_GS_TRANSFERS -ErrorAction SilentlyContinue
$env:RR_TRACE_GS_TRANSFERS_ARM='1'
Remove-Item Env:RR_TRACE_IPU -ErrorAction SilentlyContinue
$env:RR_STOP_FILE=Join-Path $run 'stop.request'
$env:RR_CAPTURE_MATERIAL_RECT='40 60 220 180'
Remove-Item Env:RR_PROFILE_EE -ErrorAction SilentlyContinue
$env:RR_CAPTURE_MIXED_PCM=Join-Path $run 'mixed.s16le'
Remove-Item Env:RR_CAPTURE_IOP_SOUND -ErrorAction SilentlyContinue
$env:RR_DUMP_IOP=Join-Path $run 'iop.bin'
$env:RR_DUMP_RAM=Join-Path $run 'ee.bin'
Get-FileHash -LiteralPath (Join-Path $run 'racer_native_probe.exe') -Algorithm SHA256 | ConvertTo-Json | Set-Content (Join-Path $run 'binary-hash.json')
@{ready_cache=$ReadyCache;variant=$Variant;stop_at_vblank=$Vblank;mode=$Mode;idle_batch=$env:RR_IOP_IDLE_BATCH;seconds=$Seconds;binary=(Join-Path $run 'racer_native_probe.exe');elf=(Join-Path $project 'disc\SLUS_202.68');disc=$env:RR_CD_IMAGE;input=$env:RR_PAD_SCRIPT;vu_count=$VuCount;vu_start=$VuStart;vu_include_resume=($IncludeResume.IsPresent -or $VuArm.IsPresent);vu_arm=$env:RR_CAPTURE_VU_ARM;host_profile=$ProfileHost.IsPresent;quiet_trace=$QuietTrace.IsPresent;gs_stream=$env:RR_CAPTURE_GS_STREAM33P;pcsx2_dll=$env:RR_PCSX2_GS_DLL;gpu_backend="44r-whole-pcsx2-dx11-microvu-fp";iop_capture=$env:RR_CAPTURE_IOP45N;iop_capture_limit=16;microvu_enabled=$true;ee_profile=$false;diagnostic_reduction='EE sampling, upload and IPU observers disabled; host sampling off by default; fixed VBlank observer; explicit registry variant and idle batch flag';build='release-opt-47h-iop-ready-cache-diagnostic'} | ConvertTo-Json | Set-Content (Join-Path $run 'launch.json')

# Redirect the native streams independently: Windows PowerShell treats native
# stderr as an error under Stop when the old merged-redirection form is used.
$process=Start-Process -FilePath (Join-Path $run 'racer_native_probe.exe') -ArgumentList @(('"'+(Join-Path $project 'disc\SLUS_202.68')+'"'),"$Seconds") -WorkingDirectory $project -WindowStyle Hidden -RedirectStandardOutput (Join-Path $run 'stdout.log') -RedirectStandardError (Join-Path $run 'native.log') -PassThru
@{utc=[DateTime]::UtcNow.ToString('o');pid=$process.Id;run=$run;binary_sha256=(Get-FileHash -LiteralPath (Join-Path $run 'racer_native_probe.exe')).Hash;bridge_sha256=(Get-FileHash -LiteralPath $env:RR_PCSX2_GS_DLL).Hash} | ConvertTo-Json | Set-Content (Join-Path $run 'process-launch.json')
$cardBefore | Set-Content (Join-Path $run 'cards-before.json')
$nativeHandle=$process.Handle
$process.WaitForExit()
$exitCode = $process.ExitCode
if ($null -eq $exitCode) { $exitCode = 'unknown' }
$exitCode | Set-Content (Join-Path $run 'native.exit')
Write-Output "Run: $run; exit=$exitCode. A diagnostic exit is not gameplay acceptance."







$cardAfter=Get-CardManifest
$cardAfter | Set-Content (Join-Path $run 'cards-after.json')
if($cardBefore -cne $cardAfter){throw 'Memory card state changed; do not start the second run without isolating cards'}
if($exitCode -ne 0){throw "Native pilot failed: $exitCode"}

$rrExpectedStop="[rr:fixed-stop47f] target=$Vblank actual=$Vblank "
$rrStopLines=@(Select-String -LiteralPath (Join-Path $run 'native.log') -SimpleMatch $rrExpectedStop)
@{expected_vblank=$Vblank;matching_stop_count=$rrStopLines.Count;native_exit=$exitCode;cards_unchanged=($cardBefore -ceq $cardAfter);verified=($rrStopLines.Count -eq 1)} | ConvertTo-Json | Set-Content (Join-Path $run 'fixed-stop-validation.json')
if($rrStopLines.Count -ne 1) {
    throw 'Requested VBlank stop was not reached exactly; wall-clock completion is insufficient'
}
