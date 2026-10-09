#!/usr/bin/env python3
"""Freeze TV camera loader source and numeric leaves from the hash-verified ELF.
Only the original ELF and this script's own outputs are inputs. No production,
RAM, reconstructed translation, ownership or allocation-failure investigation.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parents[2]
DEST = 'evidence/tvCamera49E/loader-source/'
ELF_SHA256 = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS = {
    'loader': (0x2d29f0, 0x2d30ac),
    'append_record': (0x2d2720, 0x2d286c),
    'integer_value': (0x158920, 0x158a64),
    'integer_default': (0x158b30, 0x158b7c),
    'float_default': (0x158b80, 0x158bf8),
    'vector_default': (0x158c00, 0x158cf8),
    'decimal_wrapper': (0x302e10, 0x302e2c),
    'decimal_reentrant_wrapper': (0x30ab18, 0x30ab44),
    'integer_reentrant_wrapper': (0x30ae70, 0x30aea4),
    'static_vector_defaults': (0x3cbeb0, 0x3cbf30),
    'base_initializer': (0x2d1f80, 0x2d2008),
    'tv_initializer': (0x2d2870, 0x2d28f8),
}
NUMERIC = {
    'narrow64': (0x310440, 0x310494),
    'unpack64': (0x30f8d0, 0x30f96c),
    'pack32_call': (0x311008, 0x311034),
    'pack32': (0x310498, 0x3105a4),
}
DATA = {'loader_strings': (0x3c2df0, 0x3c3020),
        'parser_constants': (0x3a6288, 0x3a637c)}
TOKEN_SLICES = {
    'integer_digit_stop': (0x309d64, 0x309d8c),
    'fractional_digit_stop_and_exponent_gate': (0x309ebc, 0x309eec),
    'short_decimal_dispatch': (0x30a008, 0x30a0f4),
    'short_decimal_division': (0x30a180, 0x30a1b4),
    'decimal_finish': (0x30aac4, 0x30ab18),
}
REG = 'zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp fp ra'.split()


def decode(pc, w):
    op, rs, rt, rd, sa, fn = w >> 26, w >> 21 & 31, w >> 16 & 31, w >> 11 & 31, w >> 6 & 31, w & 63
    im = (w & 65535) - (65536 if w & 32768 else 0)
    if w == 0: return 'nop'
    if op in (4, 5, 20, 21): return f'{ {4:"beq",5:"bne",20:"beql",21:"bnel"}[op]} {REG[rs]}, {REG[rt]}, {pc+4+4*im:#x}'
    if op in (6, 7, 22, 23): return f'{ {6:"blez",7:"bgtz",22:"blezl",23:"bgtzl"}[op]} {REG[rs]}, {pc+4+4*im:#x}'
    if op == 1 and rt in (0, 1, 2, 3): return f'{["bltz","bgez","bltzl","bgezl"][rt]} {REG[rs]}, {pc+4+4*im:#x}'
    if op in (2, 3): return f'{"j" if op==2 else "jal"} {((pc+4)&0xf0000000)|((w&0x3ffffff)<<2):#x}'
    if op in (9, 10, 11, 25): return f'{ {9:"addiu",10:"slti",11:"sltiu",25:"daddiu"}[op]} {REG[rt]}, {REG[rs]}, {im:#x}'
    if op in (12, 13, 14): return f'{ {12:"andi",13:"ori",14:"xori"}[op]} {REG[rt]}, {REG[rs]}, {w&65535:#x}'
    if op == 15: return f'lui {REG[rt]}, {w&65535:#x}'
    if op in (30, 31, 32, 35, 36, 40, 43, 49, 55, 57, 62, 63):
        name = {30:'lq',31:'sq',32:'lb',35:'lw',36:'lbu',40:'sb',43:'sw',49:'lwc1',55:'ld',57:'swc1',62:'sqc2',63:'sd'}[op]
        r = 'f'+str(rt) if op in (49,57) else 'vf'+str(rt) if op==62 else REG[rt]
        return f'{name} {r}, {im:#x}({REG[rs]})'
    if op == 0:
        if fn in (0,2,3,56,58,59,60,62,63): return f'{ {0:"sll",2:"srl",3:"sra",56:"dsll",58:"dsrl",59:"dsra",60:"dsll32",62:"dsrl32",63:"dsra32"}[fn]} {REG[rd]}, {REG[rt]}, {sa}'
        if fn in (4,6,7): return f'{ {4:"sllv",6:"srlv",7:"srav"}[fn]} {REG[rd]}, {REG[rt]}, {REG[rs]}'
        if fn in (10,11,33,35,36,37,38,42,43,45,47): return f'{ {10:"movz",11:"movn",33:"addu",35:"subu",36:"and",37:"or",38:"xor",42:"slt",43:"sltu",45:"daddu",47:"dsubu"}[fn]} {REG[rd]}, {REG[rs]}, {REG[rt]}'
        if fn==8: return f'jr {REG[rs]}'
        if fn==9: return f'jalr {REG[rd]}, {REG[rs]}'
        if fn==13: return f'break {(w>>6)&0xfffff:#x}'
    if op == 28 and fn==40 and sa==24: return f'por {REG[rd]}, {REG[rs]}, {REG[rt]}'
    if op == 17:
        if rs in (0,4): return f'{"mfc1" if rs==0 else "mtc1"} {REG[rt]}, f{rd}'
        if rs==8 and rt in (0,1,2,3): return f'{["bc1f","bc1t","bc1fl","bc1tl"][rt]} {pc+4+4*im:#x}'
        if rs==20 and fn==32: return f'cvt.s.w f{sa}, f{rd}'
        if rs==16:
            if fn in (0,1,2,3): return f'{["add.s","sub.s","mul.s","div.s"][fn]} f{sa}, f{rd}, f{rt}'
            if fn in (5,6,7): return f'{ {5:"abs.s",6:"mov.s",7:"neg.s"}[fn]} f{sa}, f{rd}'
            if fn in (50,52,54): return f'{ {50:"c.eq.s",52:"c.lt.s",54:"c.le.s"}[fn]} f{rd}, f{rt}'
    raise ValueError(f'Undecoded selected instruction {pc:#x}: {w:08x}')


def contract():
    return {
      'scope': 'Original loader and local record builder source recovery; exact numeric conversion leaves proposed for differential execution. Allocation/file/parser/diagnostic callees are boundaries, not failure or ownership investigations. No world/collision/query call bodies are followed.',
      'entry': '0x2d29f0 a0=TV camera object,a1=track prefix. Existing nonzero object+0x4c returns1 without reading a positions file. Otherwise formats %sTVCam.pos and opens a temporary reader.',
      'global_order': [{'section':'CameraPositions','field':'NumberOfCameras','kind':'int32','default':0,'destination':'object+0x4c','validation':'signed >0 required, no original upper bound'}, {'field':'SwitchEvery','kind':'float32 via parsed binary64','default':5,'destination':'object+0xdc'}],
      'per_camera_order': [
        {'field':'Position','kind':'xyz floats via parsed binary64','default':[0,0,0],'destination':'record+0x00'},
        {'field':'FOV','kind':'float32 via parsed binary64','default':-1,'effect':'Converted value == -1 triggers MaxFOV/MinFOV reads; otherwise copy the FOV bits to both fields, ignoring authored MaxFOV/MinFOV.'},
        {'field':'MaxFOV','kind':'float32','default':70,'conditional':'FOV == -1','destination':'record+0x34'},
        {'field':'MinFOV','kind':'float32','default':10,'conditional':'FOV == -1','destination':'record+0x38'},
        {'field':'MaxFOVDistance','kind':'float32','default':200,'destination':'record+0x3c'},
        {'field':'MinFOVDistance','kind':'float32','default':20,'destination':'record+0x40'},
        {'field':'InitialZone','kind':'int32','default':-1,'destination':'record+0x44','effect':'Copied unchanged; no decrement.'},
        {'field':'NumNextCameras','kind':'int32','default':0,'destination':'link row+0','validation':'signed >0 required; failure returns0 after prior writes'},
        {'field':'NextCamera%d','kind':'int32','default':-1,'effect':'For j=1..NumNextCameras store wrap32(value-1); missing produces -2; no range, duplicate, self-link, or cycle validation.'},
        {'field':'Look','kind':'xyz floats','default':[0,0,0],'destination':'record+0x10'},
        {'field':'Up','kind':'xyz floats','default':[0,0,0],'destination':'record+0x20'},
        {'field':'Stationary','kind':'float32','default':0,'destination':'record+0x30'},
        {'event':'append record via0x2d2720'},
        {'field':'TV','kind':'int32','default':50,'destination':'threshold+0','effect':'cvt.s.w(wrap32(100-TV))'},
        {'field':'Follow','kind':'int32','default':10,'destination':'threshold+4','effect':'previous threshold - cvt.s.w(Follow)'},
        {'field':'LeftSide','kind':'int32','default':10,'destination':'threshold+8','effect':'previous threshold - cvt.s.w(LeftSide)'},
        {'field':'RightSide','kind':'int32','default':10,'destination':'threshold+12','effect':'previous threshold - cvt.s.w(RightSide)'},
        {'field':'LookBack','kind':'int32','default':10,'destination':'threshold+16','effect':'previous threshold - cvt.s.w(LookBack)'},
        {'field':'FirstPerson','kind':'int32','default':0,'destination':'threshold+20','effect':'previous threshold - cvt.s.w(FirstPerson)'},
        {'field':'Blur','kind':'int32','default':25,'destination':'threshold+24','effect':'cvt.s.w(wrap32(100-Blur))'}],
      'record_layout': '80-byte stride at object+0x48, appended at object+0x50. Position128 +0; Look128 +0x10; Up128 +0x20; Stationary32 +0x30; maxFOV32 +0x34; minFOV32 +0x38; maxDistance32 +0x3c; minDistance32 +0x40; initialZone32 +0x44. Bytes+0x48..+0x4f untouched.',
      'append_store_order': ['Position128','maxFOV','minFOV','maxDistance','minDistance','initialZone','Look128','Up128','increment object+0x50','Stationary'],
      'append_semantics': 'Signed object+0x50 < object+0x4c check precedes writes. Loader ignores append result. No numeric transformation, normalization, clamping, angle conversion, derived field or sorting occurs.',
      'vectors': 'SQC2 vf0 initializes each destination128 to {0,0,0,1}; helper writes only xyz. Position default built locally; Look/Up default xyz source0x444440 is populated by frozen static initializer0x3cbeb0. Fourth word stays1 whether field is present or missing.',
      'presence': 'Loader ignores every section/field lookup return. No presence bit is written to camera/link/threshold records. Missing and explicit defaults are identical except native diagnostic metadata. FOV fallback tests converted numeric equality, not presence.',
      'original_validation': 'NumberOfCameras>0 and NumNextCameras>0 are the loader validation checks. No target-index check, maximum count, finite check, FOV ordering, distance ordering, positive-time requirement, range clamp, or probability-total check.',
      'partial_effects': 'NumberOfCameras writes directly before validation. SwitchEvery and table pointers precede camera iteration. Link row count/links write before record append; append precedes threshold writes. A later failure does not roll back globals, completed cameras or current link count. Future invocation sees nonzero count and returns1 without reload. This is not a transactional load.',
      'native_admission': 'Caller-owned output bounds, valid record/link indices, finite numeric tokens, bounded text, syntactic well-formedness, duplicate policy, and two-pass atomicity are separate native adapter constraints if adopted. They are not original checks. Inverted/equal FOV and distance pairs, signed zero and finite underflow must not be rejected merely for convenience.',
      'int_conversion': '0x158920 tests case-insensitive first-character T,F,Y,N and full ON,OFF before0x30ae70(base0). It stores low32 bits and returns1 when the field lookup succeeds; returned parse-end pointer is not validated. Fallback applies only when lookup fails.',
      'float_conversion': '0x158b80 calls0x302e10 ->0x30ab18 ->0x309c18 numeric text parsing, then0x310440 on returned64-bit bits, then SWC1. Scalar end pointer is null; trailing authored f/semicolon is not rejected by caller. String-to-binary64 callee is an explicit boundary, not reimplemented/recovered in this scope.',
      'vector_conversion': '0x158c00 retrieves at most128 bytes. Tokenizer0x30ab70 splits first/second components on comma and third on newline; each token passes through0x302e10 and0x310440. It stores x, then y, then z; missing lookup copies default xyz. Malformed partial vectors are not a safe native admission.',
      'narrow_finite': 'All finite IEEE binary64 bits admitted by proposed leaf. Exp11==0, including all binary64 subnormals, becomes signed zero. Normal source significand becomes31-bit mantissa with sticky low bit. Normal float range software-rounds ties to even. Float-underflow branch shifts and truncates (no nearest rounding). Exponent overflow and rounding carry may emit signed infinity. No host floating arithmetic or rounding-mode dependency needed.',
      'boundaries': {'file':'0x157320,0x1573b0,0x1577d0', 'sections':'0x157d00', 'lookup':'0x158240', 'tokenizer':'0x30ab70','decimal':'0x309c18','integer':'0x30ac38','format':'0x308c90','storage':'0x305700,0x2e8ca0,0x2e8d90','diagnostics':'0x133660,0x1336c0'},
      'verification_limit': 'Source freezing only. This manifest does not claim full parser/file loader execution, runtime game integration, or PC gameplay validation.'
    }


def generate():
    b = (ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
    if hashlib.sha256(b).hexdigest()!=ELF_SHA256: raise ValueError('Original ELF SHA256 mismatch')
    if b[:6]!=b'\x7fELF\x01\x01': raise ValueError('Expected little-endian ELF32')
    off=struct.unpack_from('<I',b,28)[0]; size,count=struct.unpack_from('<HH',b,42)
    segs=[struct.unpack_from('<8I',b,off+i*size) for i in range(count)]
    def extract(a,z):
        for kind,o,v,_,n,_,_,_ in segs:
            if kind==1 and v<=a<z<=v+n:return b[o+a-v:o+z-v],o+a-v
        raise ValueError(f'Unmapped extent {a:#x}..{z:#x}')
    files={}; listing=[]; header=['#pragma once','#include <cstdint>','// Original numeric words only, generated by tv_camera_recover49e.py.','namespace tv49e_source {']
    m={'elf_sha256':ELF_SHA256,'source':'work/native/project/disc/SLUS_202.68','ram_reads':0,'production_source_reads':0,'generated_source_reads':0,'resource_reads':0,'source_blocks':{},'numeric_blocks':{},'data_blocks':{},'contract':contract()}
    for category,blocks in [('source_blocks',BLOCKS),('numeric_blocks',NUMERIC),('data_blocks',DATA)]:
        for name,(a,z) in blocks.items():
            raw,o=extract(a,z); words=struct.unpack('<'+'I'*(len(raw)//4),raw)
            m[category][name]={'start':hex(a),'end_exclusive':hex(z),'words':len(words),'elf_file_offset':hex(o),'sha256':hashlib.sha256(raw).hexdigest()}
            files[DEST+name+'.bin']=raw
            listing.append(f'# {category}: {name}, [{a:#x}, {z:#x})')
            if category=='numeric_blocks':header += [f'inline constexpr std::uint32_t {name}_start=0x{a:x}u;',f'inline constexpr std::uint32_t {name}_end=0x{z:x}u;',f'inline constexpr std::uint32_t {name}[]={{']
            for i,w in enumerate(words):
                pc=a+4*i; d=f'.word 0x{w:08x}' if category=='data_blocks' else decode(pc,w)
                listing.append(f'{pc:08x} {w:08x} {d}')
                if category=='numeric_blocks':header.append(f'  0x{w:08x}u, // {pc:08x} {d}')
            if category=='numeric_blocks':header.append('};')
    header.append('}')
    files[DEST+'numeric_words49e.h']=('\n'.join(header)+'\n').encode()
    files[DEST+'original-disassembly.txt']=('\n'.join(listing)+'\n').encode()
    files[DEST+'recovery-manifest.json']=(json.dumps(m,indent=2)+'\n').encode()
    prefix={'elf_sha256':ELF_SHA256,'source':'work/native/project/disc/SLUS_202.68','scope':'Bounded source slices proving actual authored numeric-prefix acceptance; no complete libc parser or helper execution claim.','blocks':{},'findings':[
      'Integer loop continues only for ASCII0..9; fractional loop continues only for ASCII0..9. Next tests recognize e/E only; authored f and semicolon terminate the numeric prefix without invalidating it.',
      '0x302e10 passes null endptr through0x30ab18 to0x309c18. The returned prefix position is optional at0x30aac4 and caller never validates suffix.',
      'Observed authored short fixed decimals use fewer than10 significant digits and at most3 places; active short path converts accumulated int via0x3101e8 then divides by exact10^k table via0x30ff18. These two software arithmetic bodies remain explicit boundaries.',
      'Native complete-token validation, optional f suffix, semicolon comment stripping, and from_chars<double> are separately documented adapter choices; do not claim arbitrary original string-parser equivalence.']}
    prefix_listing=[]
    for name,(a,z) in TOKEN_SLICES.items():
        raw,o=extract(a,z)
        prefix['blocks'][name]={'start':hex(a),'end_exclusive':hex(z),'words':len(raw)//4,'elf_file_offset':hex(o),'sha256':hashlib.sha256(raw).hexdigest()}
        files[DEST+name+'.bin']=raw
        prefix_listing.append(f'# {name}: [{a:#x}, {z:#x})')
        for i,w in enumerate(struct.unpack('<'+'I'*(len(raw)//4),raw)):
            prefix_listing.append(f'{a+4*i:08x} {w:08x} {decode(a+4*i,w)}')
    files[DEST+'numeric-token-prefix-manifest.json']=(json.dumps(prefix,indent=2)+'\n').encode()
    files[DEST+'numeric-token-prefix-disassembly.txt']=('\n'.join(prefix_listing)+'\n').encode()
    return files,m


def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--check',action='store_true');args=ap.parse_args()
    files,m=generate()
    for name,b in files.items():
        p=ROOT/name
        if args.check:
            if p.read_bytes()!=b:raise ValueError('Frozen source mismatch: '+name)
        else:
            p.parent.mkdir(parents=True,exist_ok=True)
            if p.exists() and p.read_bytes()!=b:raise ValueError('Refusing to replace different frozen output: '+name)
            p.write_bytes(b)
    print(json.dumps({'status':'verified' if args.check else 'frozen','files':len(files),'elf_sha256':ELF_SHA256,'numeric_words':sum(v['words'] for v in m['numeric_blocks'].values())}))
if __name__=='__main__':main()
