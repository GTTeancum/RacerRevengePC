r"""INI-style text files (.spl .tun .pos .wtt .ini .prt .emt .scn .toy .acb .mcf .psm ...).

Two layers:
  parse(bytes) -> Ini: every line kept with its exact spelling (lossless; write_lossless()
      reproduces the file byte for byte, including comments, odd spacing, missing final EOL).
  Ini.sections(): semantic view [(section, [(key, value)])], key/value stripped of spaces.
  write_canonical(sections): the layout the original tools wrote for machine-made files:
      "[Section]\r\n" + "Key=Value\r\n"... + "\r\n" per section (blank line after each).
Value formatting helpers match the originals: f32 printed with %f (C printf of a float
promoted to double), vectors as "x,y,z" with no spaces.
"""
import re
import struct

_SEC = re.compile(rb'^(\s*)\[([^\]]*)\](.*)$')


class Line:
    __slots__ = ('kind', 'raw', 'eol', 'name', 'key', 'value')

    def __init__(self, kind, raw, eol, name=None, key=None, value=None):
        self.kind = kind; self.raw = raw; self.eol = eol
        self.name = name; self.key = key; self.value = value


class Ini:
    def __init__(self, lines):
        self.lines = lines

    def sections(self, keep_preamble=False):
        out = []
        cur = None
        for ln in self.lines:
            if ln.kind == 'section':
                cur = (ln.name, []); out.append(cur)
            elif ln.kind == 'kv':
                if cur is None:
                    cur = ('', []); out.append(cur)
                cur[1].append((ln.key, ln.value))
        if not keep_preamble:
            out = [s for s in out if s[0] != '' or s[1]]
        return out

    def get(self, section, key, default=None):
        for name, kv in self.sections():
            if name.lower() == section.lower():
                for k, v in kv:
                    if k.lower() == key.lower():
                        return v
        return default

    def section(self, section):
        for name, kv in self.sections():
            if name.lower() == section.lower():
                return dict((k, v) for k, v in kv)
        return None


def parse(data):
    lines = []
    parts = data.split(b'\n')
    for i, p in enumerate(parts):
        last = i == len(parts) - 1
        if last and p == b'':
            break
        if p.endswith(b'\r'):
            body, eol = p[:-1], b'\r\n'
        else:
            body, eol = p, b'\n'
        if last:
            eol = b''          # no final line ending
        m = _SEC.match(body)
        s = body.strip()
        if m and not s.startswith((b';', b'//')):
            lines.append(Line('section', body, eol, name=m.group(2).decode('latin-1').strip()))
        elif s.startswith((b';', b'//', b'#')) or not s:
            lines.append(Line('other', body, eol))
        elif b'=' in body:
            k, v = body.split(b'=', 1)
            lines.append(Line('kv', body, eol, key=k.decode('latin-1').strip(),
                              value=v.decode('latin-1').strip()))
        else:
            lines.append(Line('other', body, eol))
    return Ini(lines)


def write_lossless(ini):
    return b''.join(ln.raw + ln.eol for ln in ini.lines)


def write_canonical(sections):
    out = []
    for name, kv in sections:
        out.append('[%s]\r\n' % name)
        for k, v in kv:
            out.append('%s=%s\r\n' % (k, v))
        out.append('\r\n')
    return ''.join(out).encode('latin-1')


def f32(x):
    """%f of the value rounded to float32 (the originals print float32 fields this way)."""
    return '%f' % struct.unpack('<f', struct.pack('<f', float(x)))[0]


def vec(v):
    return ','.join(f32(c) for c in v)


def parse_vec(s):
    return [float(c) for c in s.split(',')]
