param(
    [Parameter(Mandatory=$true)][string]$Mesh,
    [string]$Texture,
    [switch]$Scene,
    [ValidateSet('x86','x64')][string]$Architecture = 'x64',
    [ValidateRange(1,120)][int]$Frames = 3
)
$ErrorActionPreference = 'Stop'
$rrRoot = Split-Path -Parent $PSScriptRoot
$rrExe = Join-Path $rrRoot "build\native-pc48E-$Architecture\racer_native_pc.exe"
$rrMesh = (Resolve-Path -LiteralPath $Mesh).Path
$rrTexture = if ($Texture) { (Resolve-Path -LiteralPath $Texture).Path } else { $null }
$rrRun = Join-Path $rrRoot ('evidence\nativePc48E-run-' + $Architecture + '-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrRun | Out-Null
Copy-Item -LiteralPath $rrExe -Destination (Join-Path $rrRun 'racer_native_pc.exe')
Copy-Item -LiteralPath $rrMesh -Destination (Join-Path $rrRun 'mesh.rrmesh')
if ($rrTexture) { Copy-Item -LiteralPath $rrTexture -Destination (Join-Path $rrRun 'texture.rrtex') }
$rrArguments = @('--mesh','mesh.rrmesh','--frames',"$Frames",'--capture','capture.ppm','--report','report.json')
if ($Scene) { $rrArguments[0] = '--scene' }
if ($rrTexture) { $rrArguments += @('--texture','texture.rrtex') }
$rrFiles = @('racer_native_pc.exe','mesh.rrmesh')
if ($rrTexture) { $rrFiles += 'texture.rrtex' }
$rrHashes = foreach ($rrName in $rrFiles) {
    $rrPath = Join-Path $rrRun $rrName
    @{name=$rrName;bytes=(Get-Item -LiteralPath $rrPath).Length;sha256=(Get-FileHash -LiteralPath $rrPath -Algorithm SHA256).Hash.ToLowerInvariant()}
}
@{scope='bounded native asset inspection, not gameplay or a benchmark';architecture=$Architecture;arguments=$rrArguments;input_files=$rrHashes;source_mesh=$rrMesh;source_texture=$rrTexture} |
    ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $rrRun 'launch.json')
Push-Location -LiteralPath $rrRun
try {
    & (Join-Path $rrRun 'racer_native_pc.exe') @rrArguments 1> 'stdout.log' 2> 'stderr.log'
    $rrExit = $LASTEXITCODE
    Set-Content -LiteralPath 'native.exit' -Value $rrExit
    if ($rrExit -ne 0) { throw "Native viewer failed ($rrExit): $rrRun" }
    if (!(Test-Path -LiteralPath 'capture.ppm') -or !(Test-Path -LiteralPath 'report.json')) { throw 'Missing native viewer outputs' }
    Get-FileHash -LiteralPath 'capture.ppm','report.json' -Algorithm SHA256 | ConvertTo-Json |
        Set-Content -LiteralPath 'output-hashes.json'
} finally { Pop-Location }
Write-Output $rrRun
