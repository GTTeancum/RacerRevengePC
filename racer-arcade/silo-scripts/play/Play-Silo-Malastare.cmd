@echo off
rem Silo build of the port on the second silo asset copy: the SC course (Baroonda Abyss slot,
rem tournament order 8) loads converted Malastare from
rem D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\assets-lc. Log: this folder\RacerRevenge.log.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Launch-RacerRevenge.ps1" -OriginalAssetRoot "D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\assets-lc"
