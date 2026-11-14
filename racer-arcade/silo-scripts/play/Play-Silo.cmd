@echo off
rem Silo build of the port (branch silo/arcade-sc-override, build silo-cv-17) on the silo asset copy.
rem The SC course (Baroonda Abyss slot, tournament order 8) loads the converted Etti IV when
rem D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\assets\converted-content.json lists TRACKS/SC.RES. Log: this folder\RacerRevenge.log.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Launch-RacerRevenge.ps1" -OriginalAssetRoot "D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\assets"
