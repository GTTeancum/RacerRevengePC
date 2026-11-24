"""Run the bundled importers against the verified installation inputs."""
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import zipfile

root = Path(__file__).resolve().parent
arc = root/'racer-arcade'
n64 = root/'racer-n64'
models = [143,130,145,136,135,139,228,227]
def run(script, *args):
    subprocess.run([sys.executable,'-B',str(root/'tool.py'),script,*map(str,args)],cwd=root,check=True,creationflags=subprocess.CREATE_NO_WINDOW)

def import_games():
    arcade = os.environ.get('RR_IMPORT_ARCADE')
    rom = os.environ.get('RR_IMPORT_N64')
    for dll in ('avcodec-61.dll','avutil-59.dll','libwinpthread-1.dll','msvcp140.dll','msvcp140_2.dll'):
        shutil.copyfile(Path(os.environ['RR_CONVERT_STOCK'])/dll,root/'runtime'/dll)
    if not arcade or not rom:
        raise ValueError("Both optional games are required for the full installation")
    groups = []
    if rom:
        print('Importing Episode I Racer',flush=True)
        data = bytearray(Path(rom).read_bytes())
        if data[:2] == b'\x37\x80':
            data[::2],data[1::2] = data[1::2],data[::2]
        elif data[:2] == b'\x40\x12':
            data[::4],data[1::4],data[2::4],data[3::4] = data[3::4],data[2::4],data[1::4],data[::4]
        (n64/'rom').mkdir()
        (n64/'rom/racer.z64').write_bytes(data)
        (n64/'blocks').mkdir()
        for name,(offset,size) in json.loads((root/'blocks.json').read_text()).items():
            (n64/'blocks'/name).write_bytes(data[offset:offset+size])
        run('racer-n64/tools/n64r/export_track.py', *models, '--no-preview')
        for model in models:
            print('Importing N64 course %d of %d' % (models.index(model)+1,len(models)),flush=True)
            uv=[] if model==227 else ['--no-uv-scroll']
            run('racer-n64/tools/n2r/n2r_convert.py',model,*uv,'--phy-class','B','--scale','2.2','--out',root/'converted')
        groups.append('n64')
    if arcade:
        print('Importing Racer Arcade',flush=True)
        chips = arc/'rom/chips'
        chips.mkdir(parents=True)
        with zipfile.ZipFile(arcade) as z:
            for entry in z.infolist():
                name = Path(entry.filename).name.lower()
                if name.startswith(('epr-','mpr-')) and name.isascii() and all(c.isalnum() or c in '.-' for c in name):
                    with z.open(entry) as src,(chips/name).open('wb') as dst:
                        shutil.copyfileobj(src,dst)
        (arc/'img').mkdir()
        run('racer-arcade/tools/interleave.py',chips,arc/'img')
        run('racer-arcade/tools/dump_race.py',arc/'img',arc/'notes')
        run('racer-arcade/tools/export_course.py',1,2,'--no-preview')
        run('racer-arcade/tools/a2r/geo_course.py',1,2)
        for course in [1,2]:
            run('racer-arcade/tools/a2r/trk_convert.py',course,'--phy-class','B','--out',root/'converted')
        print('Importing the six Arcade pilots',flush=True)
        run('racer-arcade/tools/a2r/pod_convert.py')
        run('racer-arcade/tools/a2r/ui_convert.py')
        groups.append('arcade')
    for group in groups:
        for recipe in sorted((root/'recipes').glob(group+'-*.json')):
            print('Packaging '+recipe.stem,flush=True)
            run('assemble.py',recipe.stem)

if __name__ == '__main__':
    import_games()
