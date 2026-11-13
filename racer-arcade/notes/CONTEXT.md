# Racer Arcade -> Racer Revenge porting: shared context (Nov 12 2026)

Goal: port Star Wars Racer Arcade (Sega Hikaru, 2000) tracks and racers into the
Racer Revenge PC port (this repo). Order: tracks on planets Revenge lacks first,
then racers Revenge lacks, then the remaining tracks.

Files (all under D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade):
- rom\chips\            the 36 MAME swracer chip dumps (read-only)
- img\eprom.bin          16 MB program: epr-23174..23177, pairs (ic29,ic30),(ic31,ic32)
                         interleaved 16-bit words -> 32-bit bus. Header "SAMURAI", SEGA
                         ENTERPRISES, "STARWARS RACER ARCADE(JPN/USA/...)". SH-4 code, little endian.
- img\mask.bin           256 MB data: mpr-23086..23117 in pairs, same interleave
                         (pair order ic37+ic38, ic39+ic40, ... ic67s+ic68s). Not encrypted
                         (the game declares a 315-5881 key 2903DAD5, unknown which reads use it).
                         No file names inside. 1 MB blocks 63,79,94,95,140-143,175,194-199,216-223
                         are all zero; 243-255 are uniform filler.
- img\*.strings.txt      offset + printable strings
- tools\interleave.py    builds img\ from rom\chips
- odin\odin-v0.1.1-windows-x86_64\  Odin Hikaru emulator (GPL; source github.com/Tovarichtch/Odin,
                         fork of Valkyrie by Stefano Teso; MAME also has a hikaru driver).
                         Cannot run yet: needs MAME sets hikaru + mie, not on this PC.

Known from program strings: pilots by 3-letter code ALD ANA ARK BEN BOL CLE DUD EBE ELA GAS
MAR MAW NEV ODY RAT SEB TEE WAN (GMOT_* motion names, select motions for ANA BEN GAS SEB);
courses SA_COURSE2, ETTI4, MALASTARE, SA_COURSE4 (*_LENS at 0x2E7EE0, *_STAT at 0x3D4F6C);
"loading collision" 0x3D4F08, LoadStaticModel/Object, InitLandscape, DefineDispPod,
DefinePilot, Kaadu (Naboo) objects, pod water ref, landscape texture/object.

Rules: scratch only in this folder (never AppData/temp). Run Python on ROM data with -I and
scripts kept in tools\. Do not modify anything outside work\scratch\racer-arcade.
Do not download binaries/executables. Reading public web pages/source is fine.
Notes go in notes\<topic>.md. Keep findings evidence-based (offsets, bytes), flag guesses.

## Roster (confirmed Nov 12)
Wookieepedia "Star Wars Racer Arcade" (appearances, 17 names) + the game's own voice credits
(EPROM 0x2509AB..: "EBE ENDOCOTT", "RATTS TYERELL", "BOLES ROOR", ...):
ALD Aldar Beedo, ANA Anakin Skywalker, ARK Ark "Bumpy" Roose, BEN Ben Quadinaros, BOL Boles Roor,
CLE Clegg Holdfast, DUD Dud Bolt, EBE Ebe Endocott (in credits, not on the wiki list), ELA Elan Mak,
GAS Gasgano, MAR Mars Guo, MAW Mawhonic, NEV Neva Kee, ODY Ody Mandrell, RAT Ratts Tyerell,
SEB Sebulba, TEE Teemto Pagalies, WAN Wan Sandage. Playable: Anakin, Ben Quadinaros, Gasgano, Sebulba.
New to Revenge: Ark "Bumpy" Roose, Boles Roor, Ebe Endocott, Elan Mak, Neva Kee, Ratts Tyerell.
Tracks: Bantha Tracks (Tatooine, Easy), Smuggler's Cove (Etti IV, Normal),
Pixelito Challenge (Malastare, Hard), Boonta Eve Classic (Tatooine, Expert).
