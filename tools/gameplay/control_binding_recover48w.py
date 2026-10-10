#!/usr/bin/env python3
"""Freeze the remaining original input-to-model field binding, from current ELF only."""
import sys
sys.dont_write_bytecode=True
import argparse
import hashlib
import json
from pathlib import Path
import struct
import input_shaping_recover48v as prior

ROOT=Path(__file__).resolve().parents[2]
START,END=0x2181c4,0x218898

def decode(pc,w):
    op,rs,rt,rd,sa,fn=w>>26,(w>>21)&31,(w>>16)&31,(w>>11)&31,(w>>6)&31,w&63
    if op==0 and fn in (0,0x3c,0x3e):
        return f'{ {0:"sll",0x3c:"dsll32",0x3e:"dsrl32"}[fn]} {prior.REG[rd]}, {prior.REG[rt]}, {sa}'
    if op==0 and fn==0x21:return f'addu {prior.REG[rd]}, {prior.REG[rs]}, {prior.REG[rt]}'
    if op==0x24:
        im=struct.unpack('<h',struct.pack('<H',w&65535))[0]
        return f'lbu {prior.REG[rt]}, {im:#x}({prior.REG[rs]})'
    if op==0x11 and rs==16 and fn==3:return f'div.s f{sa}, f{rd}, f{rt}'
    return prior.decode(pc,w)

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--check',action='store_true');args=ap.parse_args()
    data=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
    if hashlib.sha256(data).hexdigest()!=prior.ELF_SHA256:raise ValueError('Unexpected ELF')
    po=struct.unpack_from('<I',data,28)[0];sz,n=struct.unpack_from('<HH',data,42)
    raw=None
    for i in range(n):
        t,o,v,_,s,_,_,_=struct.unpack_from('<8I',data,po+i*sz)
        if t==1 and v<=START<END<=v+s:raw=data[o+START-v:o+END-v];break
    if raw is None:raise ValueError('Missing original extent')
    words=struct.unpack('<'+'I'*(len(raw)//4),raw)
    lines=['#pragma once','#include <cstdint>','// Test-only words recovered from the hash-checked ELF.',f'inline constexpr std::uint32_t rr_binding_48w_start=0x{START:08x}u;','inline constexpr std::uint32_t rr_binding_48w[]={']
    listing=[]
    for i,w in enumerate(words):
        pc=START+i*4;text=decode(pc,w)
        lines.append(f'    0x{w:08x}u, // {pc:08x} {text}')
        listing.append(f'{pc:08x} {w:08x} {text}')
    lines.append('};')
    manifest={'elf_sha256':prior.ELF_SHA256,'start':hex(START),'end_exclusive':hex(END),'words':len(words),'block_sha256':hashlib.sha256(raw).hexdigest(),'ram_reads':0,'generated_source_reads':0}
    provenance={}
    extents={'player_constructor':(0x1a02c0,0x1a0308),'base_constructor_entry':(0x20f390,0x20f3e0),
             'base_defaults':(0x20f690,0x20f6e0),'base_create_index':(0x210770,0x2107ec),
             'player_create_inputs':(0x1a0570,0x1a05c0),'player_create_forward':(0x1a081c,0x1a084c),
             'selector_loader':(0x215408,0x2154a0),'aero_producer':(0x16cd90,0x16d874),
             'flow_vector_read':(0x28bcf4,0x28bd28),'player_vtable_first':(0x3ced50,0x3ced54),
             'player_descriptor_first':(0x3aa150,0x3aa154),'player_class_name':(0x3aa010,0x3aa010+len(b'PodPlayerVehicle\0'))}
    for name,(start,end) in extents.items():
        block=None
        for i in range(n):
            t,o,v,_,size,_,_,_=struct.unpack_from('<8I',data,po+i*sz)
            if t==1 and v<=start<end<=v+size:block=data[o+start-v:o+end-v];break
        if block is None:raise ValueError('Unmapped provenance extent '+name)
        provenance[name]={'start':hex(start),'end_exclusive':hex(end),'sha256':hashlib.sha256(block).hexdigest(),'hex_bytes':block.hex()}
    if provenance['player_vtable_first']['hex_bytes']!='50a13a00' or provenance['player_descriptor_first']['hex_bytes']!='10a03a00':raise ValueError('Player identity pointer mismatch')
    if bytes.fromhex(provenance['player_class_name']['hex_bytes'])!=b'PodPlayerVehicle\0':raise ValueError('Player class string mismatch')
    files={'tools/gameplay/control_binding_words48w.h':'\n'.join(lines)+'\n','evidence/controlBinding48W/original-disassembly.txt':'\n'.join(listing)+'\n','evidence/controlBinding48W/recovery-manifest.json':json.dumps(manifest,indent=2)+'\n','evidence/controlBinding48W/provenance-blocks.json':json.dumps(provenance,indent=2)+'\n'}
    for name,text in files.items():
        p=ROOT/name
        if args.check:
            if p.read_text()!=text:raise ValueError('Frozen output mismatch: '+name)
        else:
            p.parent.mkdir(exist_ok=True,parents=True)
            if p.exists() and p.read_text()!=text:raise ValueError('Refusing to replace historical evidence')
            p.write_text(text)
    print(json.dumps({'status':'verified' if args.check else 'frozen',**manifest}))
if __name__=='__main__':main()
