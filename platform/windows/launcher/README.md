# Building the Windows release launcher

Players can download the Windows ZIP and follow INSTALL.txt. These steps are for rebuilding the installer.

Use the x64 .NET Framework C# compiler and the Windows port executable. The builder needs reference hashes for the two optional ROMs. PS2 setup identifies Racer Revenge from the disc filesystem, boot executable and resource archives. The player supplies the original games.

1. Prepare the official Python 3.12.10 Windows x64 embeddable distribution in a local runtime folder. Add NumPy 1.26.4, SciPy 1.15.3 and Pillow 12.1.1 from Windows CPython 3.12 x64 wheels. Unpack the modules, adjacent DLL directories and license metadata beside python.exe.
2. From an x64 Visual Studio developer environment, compile convert_tile.cpp and src/assets/original_ipu_decoder.cpp together as convert_tile.dll with /LD /MD /EHsc /std:c++17. Include src/assets and third_party/ffmpeg-7.1.5/include. Put the DLL beside the embedded python.exe. It uses the same shared IPU decoder libraries as the game.
3. Run package_converter.py RUNTIME_DIR CONVERTER_ZIP with Python to assemble the converter archive.
4. Extract `chdman.exe`, `COPYING`, and `docs/legal` from the official [MAME 0.289 Windows x64 package](https://github.com/mamedev/mame/releases/tag/mame0289). The builder embeds the reader and its notices.
5. Extract `x64/7za.exe` and `License.txt` from [7-Zip Extra 26.05](https://github.com/ip7z/7zip/releases/tag/26.05).
6. Run `build.py --public` with `--mame-tools` and `--sevenzip-extra` pointing at the two extracted tool directories, `--converter` at the converter ZIP, `--crt` at the x64 Microsoft.VC143.CRT directory, `--game` at the Windows game executable, `--stock` at the four game runtime DLLs' directory, `--dlc` at the directory containing demolition-pugwis, `--arcade` and `--n64` at the reference ROMs, `--icon` at the launcher ICO, and `--out` at a build output directory. Supply `--race-frame` with the native gameplay frame to include the launcher banner.

Setup runs the converter with its own Python interpreter in a staging directory. Stock setup installs Pugwis in DLC. A full import adds 16 more DLC folders. Both modes keep a separate editable Pugwis copy in Templates.

To run the disc-reader regression test, compile `tests/DiscImageTests.cs`, `DiscImage.cs`, and `Disc.cs` together with the Framework compiler. Pass the test executable a scratch directory. It generates sector fixtures for cooked data, Mode 1, Mode 2 XA and CUE pregaps.
