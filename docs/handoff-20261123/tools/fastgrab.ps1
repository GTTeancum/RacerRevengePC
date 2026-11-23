# Fast read-only grab: for any of our runs whose evidence name matches $Match,
# save a frame every $IntervalMs for $Seconds after its window appears.
param([string]$Match = 'lightsnap', [int]$IntervalMs = 100, [int]$Seconds = 50, [int]$WaitMinutes = 360)
$code = @'
using System; using System.Runtime.InteropServices;
public class PWF { [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr h, IntPtr dc, uint f);
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out R r);
  [StructLayout(LayoutKind.Sequential)] public struct R { public int L, T, Ri, B; } }
'@
Add-Type -TypeDefinition $code; Add-Type -AssemblyName System.Drawing
$root = 'D:\Programming\GitHub\RacerRevengePC\work\scratch\frames\fast'
$done = @{}
$end = (Get-Date).AddMinutes($WaitMinutes)
while ((Get-Date) -lt $end) {
  foreach ($g in Get-CimInstance Win32_Process -Filter "Name='RacerRevengePC.exe'") {
    $p = Get-CimInstance Win32_Process -Filter "ProcessId=$($g.ParentProcessId)" -ErrorAction SilentlyContinue
    if (-not $p -or $p.CommandLine -match 'silo' -or $p.CommandLine -notmatch "--output\s+evidence[/\\](\S*$Match\S*)") { continue }
    $run = $Matches[1]; if ($done[$run]) { continue }
    $h = [IntPtr]::Zero
    for ($t = 0; $t -lt 600 -and $h -eq [IntPtr]::Zero; $t++) { try { $h = (Get-Process -Id $g.ProcessId -ErrorAction Stop).MainWindowHandle } catch { break }; Start-Sleep -Milliseconds 100 }
    if ($h -eq [IntPtr]::Zero) { continue }
    $done[$run] = $true; $dir = Join-Path $root $run; New-Item -ItemType Directory -Force $dir | Out-Null
    $stop = (Get-Date).AddSeconds($Seconds); $i = 0
    while ((Get-Date) -lt $stop) {
      try {
        $r = New-Object PWF+R; [PWF]::GetClientRect($h, [ref]$r) | Out-Null
        if ($r.Ri -gt 100) {
          $b = New-Object Drawing.Bitmap ($r.Ri + 16), ($r.B + 40); $gr = [Drawing.Graphics]::FromImage($b); $dc = $gr.GetHdc()
          [PWF]::PrintWindow($h, $dc, 2) | Out-Null; $gr.ReleaseHdc($dc); $gr.Dispose()
          $s = New-Object Drawing.Bitmap $b, 480, ([int](480 * $b.Height / $b.Width)); $s.Save((Join-Path $dir ('{0:D4}.jpg' -f $i)), [Drawing.Imaging.ImageFormat]::Jpeg); $s.Dispose(); $b.Dispose(); $i++
        }
      } catch {}
      Start-Sleep -Milliseconds $IntervalMs
    }
  }
  Start-Sleep -Seconds 2
}
