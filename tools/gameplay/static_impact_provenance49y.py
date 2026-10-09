"""Read-only original ELF/authored provenance; writes only49Y evidence."""
from pathlib import Path
import argparse,hashlib,json,struct,zlib
ROOT=Path(__file__).resolve().parents[2];OUT=ROOT/'work/local49Y/static-impact/provenance'
def sha(x):return hashlib.sha256(x).hexdigest()
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--disc',type=Path,required=True);a=ap.parse_args();OUT.mkdir(parents=True,exist_ok=True)
 elf=(a.disc/'SLUS_202.68').read_bytes();assert sha(elf)=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
 chunks={}
 for name,lo,hi in [('cf0-constructor',0x210380,0x2103b0),('cf0-authored-loader',0x215d80,0x215f3c),('cf0-profile-override',0x216350,0x216514),('env-tuning-loader',0x1aa5d0,0x1aa788)]:
  raw=elf[lo-0x100000+0x80:hi-0x100000+0x80];(OUT/(name+'.txt')).write_text(''.join(f'{pc:08x} {struct.unpack_from("<I",raw,pc-lo)[0]:08x}\n' for pc in range(lo,hi,4)))
  chunks[name]=dict(start=hex(lo),end_exclusive=hex(hi),sha256=sha(raw))
 addresses=[0x3aadc0,0x3aadd0,0x3aadd8,0x3aade0,0x3aade8,0x3aadf0,0x3aadf8,0x3aae08,0x3aae10,0x3aae18,0x3aae20,0x3b29d8,0x3b2ad8,0x3b2ae8,0x3b20d8,0x3b2108,0x3b2b78]
 strings={}
 for va in addresses:
  offset=va-0x100000+0x80;strings[hex(va)]=elf[offset:elf.index(0,offset)].decode('ascii')
 resource=(a.disc/'TRACKS/TA.RES').read_bytes();version,meta,codec,flags,tb=struct.unpack_from('<5I',resource);assert (version,codec,flags)==(3,0x340,0)
 sizes=struct.unpack_from('<%dH'%(tb//2),resource,20);assert sizes[-1]==0 and 20+tb==8+meta
 offset=20+tb;count,index_bytes=struct.unpack_from('<II',resource,offset);offset+=8;index_end=offset+index_bytes;entries=[]
 for _ in range(count):
  n=struct.unpack_from('<I',resource,offset)[0];offset+=4;name=resource[offset:offset+n].decode('ascii');offset+=n;start,size=struct.unpack_from('<II',resource,offset);offset+=8;entries.append((name,start,size))
 assert offset==index_end
 decoded=bytearray()
 for size in sizes[:-1]:
  block=resource[offset:offset+size];stream=zlib.decompressobj();out=stream.decompress(block);assert stream.eof and not stream.unused_data and not stream.unconsumed_tail and len(out)==24576;decoded+=out;assert resource[offset+size]==255;offset+=size+1
 assert offset==len(resource)
 matches=[e for e in entries if e[0].lower()=='tracks\\ta.tun'];assert len(matches)==1
 name,start,size=matches[0];tun=bytes(decoded[start:start+size]);assert len(tun)==size;(OUT/'ta.tun').write_bytes(tun)
 phy=(ROOT/'evidence/authoredInitialization49N/authoring-source/Pod01.phy').read_bytes();assert sha(phy)=='47fcfbaab278d10506b302520adc51af4e3388ceb601daa3dbd588aea1f39a1b'
 report=dict(elf_sha256=sha(elf),source_blocks=chunks,strings=strings,resource=dict(path=str(a.disc/'TRACKS/TA.RES'),bytes=len(resource),sha256=sha(resource),member=name,logical_offset=start,member_bytes=size,member_sha256=sha(tun)),pod_phy=dict(path='evidence/authoredInitialization49N/authoring-source/Pod01.phy',sha256=sha(phy)),scope='Static ELF/source/authored evidence only. No RAM or live original game. CF0 profile selection and parser conversion remain explicit caller provenance.')
 (OUT/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(tun.decode('ascii'));print(json.dumps(report['resource']))
if __name__=='__main__':main()
