import json, pathlib
D = pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\addon-arcade-146\DLC')
for p in sorted(D.iterdir()):
    d = json.loads((p / 'dlc.json').read_text(encoding='utf-8'))
    if d.get('kind') == 'pod':
        continue
    print('==', p.name, d.get('kind'))
    for f in p.rglob('*'):
        if f.is_file():
            print('  ', f.relative_to(p), f.stat().st_size)
    for i in d.get('items', []):
        print('  item', {k: v for k, v in i.items() if k != 'enhancements'})
