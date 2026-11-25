"""Build the self-contained Windows launcher and its offline media import tools."""
import argparse
import hashlib
import os
import re
from pathlib import Path
import subprocess
import tempfile
import zipfile

HERE = Path(__file__).resolve().parent


def digest(stream):
    h = hashlib.sha256()
    for data in iter(lambda: stream.read(1 << 20), b''):
        h.update(data)
    return h.hexdigest()


def main():
    p = argparse.ArgumentParser(description=__doc__)
    for name in ('game', 'stock', 'dlc', 'icon', 'out', 'mame-tools', 'sevenzip-extra'):
        p.add_argument('--' + name, required=True, type=Path)
    p.add_argument('--crt', type=Path, help='Microsoft.VC143.CRT x64 redistributable directory')
    p.add_argument('--converter', type=Path, help='Bundled offline converter and runtime ZIP for public releases')
    p.add_argument('--public', action='store_true', help='Package the PS2 installer and Pugwis template')
    p.add_argument('--arcade', type=Path)
    p.add_argument('--n64', type=Path)
    p.add_argument('--race-frame', type=Path, help='Native Mos Espa gameplay frame for the personal launcher banner')
    a = p.parse_args()
    if not a.arcade or not a.n64:
        p.error('reference hashes require --arcade and --n64')
    if a.public and (not a.converter or not a.crt):
        p.error('public builds require --converter and --crt')
    a.out.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(dir=a.out) as temp:
        root = Path(temp)
        artwork = []
        if a.race_frame:
            from PIL import Image
            with Image.open(a.race_frame) as frame:
                w, h = frame.size
                banner = frame.crop((0, round(h * .40), w, round(h * .75)))
                banner.convert('RGB').save(root / 'banner.jpg', quality=94)
            artwork = ['/resource:' + str((root / 'banner.jpg').resolve()) + ',banner.jpg']
        def row(name, f):
            return digest(f) + '\t' + name + '\n'
        with zipfile.ZipFile(root / 'content.zip', 'w', zipfile.ZIP_DEFLATED, compresslevel=5) as z:
            tool = a.mame_tools / 'chdman.exe'
            with tool.open('rb') as stream:
                if digest(stream) != '8a74468e3b0879698835b57c3b58e88e5a51e4de73bee6ef755c28530b5b040f':
                    raise ValueError('Use chdman.exe from the official MAME 0.289 Windows x64 package')
            z.write(tool, 'chdman.exe')
            notices = 'base/Notices/MAME/'
            z.write(a.mame_tools / 'COPYING', notices + 'COPYING')
            for license_text in ('BSD-2-Clause', 'BSD-3-Clause', 'BSL-1.0', 'CC0', 'GPL-2.0', 'LGPL-2.1', 'MIT', 'Zlib'):
                name = 'docs/legal/' + license_text
                z.write(a.mame_tools / name, notices + name)
            z.writestr(notices + 'SOURCE.txt', 'MAME 0.289, unmodified Windows x64 chdman.exe\r\n'
                       'https://github.com/mamedev/mame/releases/tag/mame0289\r\n'
                       'https://github.com/mamedev/mame/tree/mame0289\r\n')
            archiver = a.sevenzip_extra / 'x64/7za.exe'
            with archiver.open('rb') as stream:
                if digest(stream) != '7ddf0a3bc451e166679296be630d6f0573eb386159c04184761c43020c484291':
                    raise ValueError('Use the x64 7za.exe from the official 7-Zip Extra 26.05 package')
            z.write(archiver, '7za.exe')
            z.write(a.sevenzip_extra / 'License.txt', 'base/Notices/7-Zip/License.txt')
            z.writestr('base/Notices/7-Zip/SOURCE.txt', '7-Zip Extra 26.05, unmodified x64 7za.exe\r\n'
                       'https://github.com/ip7z/7zip/releases/tag/26.05\r\n')
            with zipfile.ZipFile(a.arcade) as src:
                rows = []
                for f in sorted(src.namelist()):
                    if not f.endswith('/'):
                        with src.open(f) as stream:
                            rows.append(row(Path(f).name, stream))
                z.writestr('arcade.tsv', ''.join(rows))
            with a.n64.open('rb') as rom:
                if rom.read(4) != bytes.fromhex('80371240'):
                    raise ValueError('Build reference must be a big-endian N64 ROM')
                rom.seek(0)
                z.writestr('n64.tsv', row('USA', rom))
            if a.public:
                z.write(a.converter, 'converter.zip', compress_type=zipfile.ZIP_STORED)
            z.write(a.game, 'base/RacerRevengePC.exe')
            for name in ('SDL2.dll','avcodec-61.dll','avutil-59.dll','libwinpthread-1.dll'):
                z.write(a.stock / name, 'base/' + name)
            if a.public:
                for name in ('msvcp140.dll','msvcp140_2.dll','vcruntime140.dll','vcruntime140_1.dll'):
                    z.write(a.crt / name, 'base/' + name)
            groups = set()
            for folder in sorted(a.dlc.iterdir()):
                if not (folder / 'dlc.json').is_file():
                    continue
                group = folder.name.split('-')[0]
                if group not in ('arcade','n64') and folder.name != 'demolition-pugwis':
                    continue
                if a.public and group != 'demolition':
                    continue
                groups.add(group)
                prefix = 'base/Templates/' if group == 'demolition' else group + '/DLC/'
                if a.public:
                    for relative in ('dlc.json', 'PODS/DEM_PUGWIS/DEM_PUGWIS.RES',
                                     'PODS/DEM_PUGWIS/DEM_PUGWIS.CAM', 'UI/DEM_PUGWIS_UI.RES'):
                        f = folder / relative
                        member = prefix + f.relative_to(a.dlc).as_posix()
                        if relative == 'dlc.json':
                            manifest = re.sub(r'(?i)[A-Z]:\\\\[^"\n]*?\\\\work\\\\scratch\\\\',
                                              '', f.read_text(encoding='utf-8'))
                            z.writestr(member, manifest)
                        else:
                            z.write(f, member)
                    continue
                for f in sorted(folder.rglob('*')):
                    if f.is_file():
                        z.write(f, prefix + f.relative_to(a.dlc).as_posix())
            if groups != ({'demolition'} if a.public else {'arcade','n64','demolition'}):
                raise ValueError('Missing optional packages or Pugwis template')
            z.writestr('base/Templates/README.txt', 'Pugwis is an editable example pod. Setup installs the playable copy in DLC/demolition-pugwis. This separate copy is available for editing.\r\n')
        compiler = Path(os.environ['SystemRoot']) / 'Microsoft.NET/Framework64/v4.0.30319/csc.exe'
        env = dict(os.environ, TEMP=temp, TMP=temp)
        exe = root / 'RacerRevengeLauncher.exe'
        subprocess.run([str(compiler), '/nologo', *(['/define:PUBLIC_RELEASE'] if a.public else []), '/target:winexe', '/platform:x64', '/optimize+',
                        '/reference:System.Windows.Forms.dll,System.Drawing.dll,System.IO.Compression.dll',
                        '/win32icon:' + str(a.icon.resolve()), '/resource:' + str((root/'content.zip').resolve()) + ',content.zip',
                        '/resource:' + str(HERE.parents[1] / 'linux/native/keyboard_actions.inc') + ',keyboard-actions',
                        '/resource:' + str(HERE.parents[2] / 'src/game/control_preset_rows.inc') + ',pad-presets',
                        '/out:' + str(exe.resolve()), *artwork, *map(str, sorted(HERE.glob('*.cs')))], env=env, check=True)
        os.replace(exe, a.out / exe.name)


if __name__ == '__main__':
    main()

