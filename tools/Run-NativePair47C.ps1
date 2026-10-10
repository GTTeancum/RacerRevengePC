param([ValidateRange(1,7200)][int]$Seconds=1800)
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrPair=Join-Path $rrRoot ('evidence/native-pair47C-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrPair | Out-Null
@{seconds_each=$Seconds;utc=[DateTime]::UtcNow.ToString('o');order=@('baseline','batched');comparison='same binary/settings/input; idle batching flag differs';acceptance='pending; no gameplay or FPS claim'} | ConvertTo-Json | Set-Content (Join-Path $rrPair 'pair.json')
try {
    & (Join-Path $PSScriptRoot 'Run-Native47C.ps1') -Seconds $Seconds -Mode baseline -QuietTrace | Tee-Object (Join-Path $rrPair 'baseline.log')
    & (Join-Path $PSScriptRoot 'Run-Native47C.ps1') -Seconds $Seconds -Mode batched -QuietTrace | Tee-Object (Join-Path $rrPair 'batched.log')
    0 | Set-Content (Join-Path $rrPair 'pair.exit')
} catch {
    $_ | Out-String | Set-Content (Join-Path $rrPair 'error.log')
    1 | Set-Content (Join-Path $rrPair 'pair.exit')
    throw
}
