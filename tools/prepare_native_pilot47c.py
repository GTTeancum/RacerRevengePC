"""Derive a controlled pair launcher from the preserved 45Z capture settings."""
from pathlib import Path

root = Path(__file__).resolve().parents[1]
text = (root / 'work/run-native45z.ps1').read_text(encoding='utf-8-sig')
text = text.replace("    [switch]$QuietTrace", "    [switch]$QuietTrace,\n    [ValidateSet('baseline','batched')][string]$Mode='baseline'")
text = text.replace('$PSScriptRoot', '$rrWork')
text = text.replace("$ErrorActionPreference='Stop'", """$ErrorActionPreference='Stop'
$rrRoot = Split-Path -Parent $PSScriptRoot
$rrWork = Join-Path $rrRoot 'work'
if((Get-Content (Join-Path $rrRoot 'evidence/47B/build-native3.exit')).Trim() -ne '0'){throw '47B2 build is not verified'}
$env:RR_IOP_IDLE_BATCH=if($Mode -eq 'batched'){'1'}else{'0'}
$env:RR_IOP_IDLE_STATS='1'
function Get-CardManifest {
    $files=@(Get-ChildItem (Join-Path $rrWork 'native/project/disc/mc0'),(Join-Path $rrWork 'native/project/disc/mc1') -File -Recurse -Force | Sort-Object FullName)
    $records=@($files | ForEach-Object { @{path=$_.FullName;bytes=$_.Length;sha256=(Get-FileHash -LiteralPath $_.FullName).Hash} })
    ConvertTo-Json -InputObject $records -Depth 4 -Compress
}
$cardBefore=Get-CardManifest
""")
text = text.replace("'native-run45z-'", "('native-run47c-'+$Mode+'-')")
text = text.replace("if(-not $ButtonScript){$ButtonScript=Join-Path $project 'analysis\\buttons14-native.txt'}", "if(-not $ButtonScript){$ButtonScript=Join-Path $rrWork 'native-run45z-20260930-112950-077/buttons.txt'}")
text = text.replace("@{seconds=$Seconds;binary=", "@{mode=$Mode;idle_batch=$env:RR_IOP_IDLE_BATCH;seconds=$Seconds;binary=")
text = text.replace("same immutable native game/core", "same immutable 47B candidate binary, differing only in idle batch flag")
text = text.replace("release-opt-45v-iop-evidence-pcsx2-vu-44r-fp-34a-yield", "release-opt-47b-idle-adapter-pcsx2-vu-44r-fp-34a-yield")
text = text.replace("$nativeHandle=$process.Handle", "$cardBefore | Set-Content (Join-Path $run 'cards-before.json')\n$nativeHandle=$process.Handle")
text += """
$cardAfter=Get-CardManifest
$cardAfter | Set-Content (Join-Path $run 'cards-after.json')
if($cardBefore -cne $cardAfter){throw 'Memory card state changed; do not start the second run without isolating cards'}
if($exitCode -ne 0){throw "Native pilot failed: $exitCode"}
"""
target = root / 'tools/Run-Native47C.ps1'
assert not target.exists(), 'Preserve the launch script used for evidence'
target.write_text('\n'.join(line.rstrip() for line in text.splitlines())+'\n', encoding='utf-8', newline='\n')
print(target)
