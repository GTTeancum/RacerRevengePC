# tools/racer-arcade

Independent tools branch (not part of `main`, never merged into it). Tooling and research notes for
porting Star Wars Racer Arcade (Sega Hikaru) tracks and racers into Racer Revenge.

Contains code and notes only. No ROM dumps, no extracted game assets (textures, models, audio,
converted tracks) and no game files: those stay in the gitignored working folder
`D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade` and are rebuilt from the user's own ROM set.

- `racer-arcade/tools/` ROM interleave, SH-4 disassembler, Hikaru GPU/mesh decoders, course and pod
  exporters, previews.
- `racer-arcade/tools/a2r/` arcade-to-Revenge converter: one transform (`a2r_space.py`), geometry
  (`geo_*.py`), track data (`trk_*.py`), silo installer (`trk_silo.py`).
- `racer-arcade/notes/` format and program notes, porting plan (`PORTING-PLAN.md`), conversion spec.
- `racer-arcade/silo-scripts/` scripts that build and run the silo test of converted tracks raced in
  the SC slot (worktree branch `silo/arcade-sc-override`, local only).
