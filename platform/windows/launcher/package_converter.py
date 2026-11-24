"""Pack the portable converter sources and a prepared embedded Python runtime."""
import argparse
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('runtime', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    source = Path(__file__).resolve().with_name('converter')
    with ZipFile(args.output, 'w', ZIP_DEFLATED, compresslevel=5) as bundle:
        for base, subfolder in [(source, ''), (args.runtime, 'runtime/')]:
            for item in sorted(base.rglob('*')):
                if item.is_file() and '__pycache__' not in item.parts and item.suffix not in ('.pyc','.lib','.exp'):
                    bundle.write(item, subfolder + item.relative_to(base).as_posix())

if __name__ == '__main__':
    main()
