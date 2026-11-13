r"""PSM material (ASCII INI, CRLF) reader / writer for the a2r converter.

Revenge materials (e.g. TA textures\tarock.psm) are written by a tool that pads every key to
column 42 ("key" + spaces, "= value"); sections are "[Name]" (General carries a trailing space,
ShaderPass_N a ";-----" comment at column 42). The port's parser
(D:\Programming\GitHub\RacerRevengePC\tools\assets\decode_texture48d.py parse_material,
src\assets\import_texture48e.cpp decodeMaterialImpl48E) reads sections, key = value, ';' comments.

parse() turns each line into an item (section / key / comment / blank). A line is "canonical" if
the standard formatter reproduces it; otherwise its raw text is kept so write(parse(b)) == b
holds for every original. New materials (material()) use only the canonical formatter.
"""
KEY_COL = 42
PASS_COMMENT = ';---------------------------------------'


def fmt_key(key, value):
    return key.ljust(KEY_COL) + '= ' + value if value != '' else key.ljust(KEY_COL) + '= '


def fmt_section(name, comment=None):
    head = '[%s]' % name
    if name == 'General':
        return head + ' '
    if comment is not None:
        return head.ljust(KEY_COL) + comment
    return head


def _canon(item):
    k = item['type']
    if k == 'section':
        return fmt_section(item['name'], item.get('comment'))
    if k == 'key':
        return fmt_key(item['key'], item['value'])
    if k == 'comment':
        return item['text']
    return ''


def parse(blob):
    text = blob.decode('ascii')
    ends_crlf = text.endswith('\r\n')
    lines = text.split('\r\n')
    if ends_crlf:
        lines = lines[:-1]
    items = []
    canonical = True
    for ln in lines:
        s = ln.strip()
        if not s:
            it = dict(type='blank')
        elif s.startswith('['):
            name = s[1:s.index(']')]
            rest = s[s.index(']') + 1:].strip()
            it = dict(type='section', name=name, comment=rest or None)
        elif s.startswith(';'):
            it = dict(type='comment', text=s)
        elif '=' in s:
            k, v = s.split('=', 1)
            it = dict(type='key', key=k.strip(), value=v.strip())
        else:
            it = dict(type='comment', text=s)
        if _canon(it) != ln:
            it['raw'] = ln
            canonical = False
        items.append(it)
    return dict(items=items, crlf_end=ends_crlf, canonical=canonical)


def write(m):
    lines = [it.get('raw', _canon(it)) for it in m['items']]
    return ('\r\n'.join(lines) + ('\r\n' if m.get('crlf_end', True) else '')).encode('ascii')


def material(texture, program='PreLitCompressed', extra=()):
    """New single-pass material in the canonical layout. extra: (key, value) pairs appended to the
    pass (e.g. alpha test keys copied from an original material)."""
    items = [dict(type='section', name='General'), dict(type='key', key='TotalShaderPasses', value='1'),
             dict(type='blank'), dict(type='section', name='ShaderPass_1', comment=PASS_COMMENT),
             dict(type='key', key='microProgramName', value=program),
             dict(type='key', key='TextureMap_1', value=texture)]
    items += [dict(type='key', key=k, value=v) for k, v in extra]
    return write(dict(items=items, crlf_end=True))


def fields(m):
    """{section: {key: value}} view."""
    out, cur = {}, None
    for it in m['items']:
        if it['type'] == 'section':
            cur = out.setdefault(it['name'], {})
        elif it['type'] == 'key' and cur is not None:
            cur[it['key']] = it['value']
    return out
