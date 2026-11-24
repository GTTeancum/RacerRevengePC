# Building the Windows release launcher

Players can download the Windows ZIP and follow INSTALL.txt. These steps are for rebuilding the installer.

Use the x64 .NET Framework C# compiler and the Windows port executable. The builder reads reference files locally to generate hashes checked during setup. It embeds the Pugwis template, runtime libraries and converters. The player supplies all other original game data.

1. Prepare the official Python 3.12.10 Windows x64 embeddable distribution in a local runtime folder. Add NumPy 1.26.4, SciPy 1.15.3 and Pillow 12.1.1 from Windows CPython 3.12 x64 wheels. Unpack the modules, adjacent DLL directories and license metadata beside python.exe.
2. From an x64 Visual Studio developer environment, compile convert_tile.cpp and src/assets/original_ipu_decoder.cpp together as convert_tile.dll with /LD /MD /EHsc /std:c++17. Include src/assets and third_party/ffmpeg-7.1.5/include. Put the DLL beside the embedded python.exe. It uses the same shared IPU decoder libraries as the game.
3. Run package_converter.py RUNTIME_DIR CONVERTER_ZIP with Python to assemble the converter archive.
4. Run build.py --public with --converter CONVERTER_ZIP, --crt pointing at the x64 Microsoft.VC143.CRT directory, --game pointing at the Windows game executable, --stock pointing at the extracted USA PS2 tree with the four runtime DLLs, --dlc pointing at the directory containing demolition-pugwis, --arcade and --n64 pointing at the reference ROMs, --icon pointing at the launcher ICO, and --out pointing at a build output directory. Supply --race-frame with the native gameplay frame to include the launcher banner.

Setup runs the converter with its own Python interpreter in a staging directory. Stock setup installs Pugwis in DLC. A full import adds 16 more DLC folders. Both modes keep a separate editable Pugwis copy in Templates.
