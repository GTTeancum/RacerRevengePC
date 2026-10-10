"""Independent original-ELF fast-upload/PCSX2 layout oracle (offline only)."""
from pathlib import Path
import json,re,struct,sys
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parents[2]

class Elf:
    def __init__(self,path):
        self.data=path.read_bytes();self.segments=[]
        po=struct.unpack_from('<I',self.data,28)[0];ps,pn=struct.unpack_from('<HH',self.data,42)
        for i in range(pn):
            kind,off,va,_,size=struct.unpack_from('<5I',self.data,po+i*ps)
            if kind==1:self.segments.append((off,va,size))
    def read(self,va,size):
        for off,base,count in self.segments:
            if base<=va and va+size<=base+count:return self.data[off+va-base:off+va-base+size]
        raise ValueError('address outside original ELF load segment')
    def words(self,va,count):return struct.unpack('<'+'I'*count,self.read(va,4*count))

def source_table(text,name):
    fragment=text[text.index(name):];fragment=fragment[fragment.index('{'):fragment.index('};')]
    fragment=re.sub(r'//[^\n]*','',fragment)
    return [int(x) for x in re.findall(r'\d+',fragment)]

class ReferenceLayout:
    def __init__(self,elf=None):
        self.elf=elf or Elf(ROOT/'work/native/project/disc/SLUS_202.68')
        # Original game data tables referenced by24D3E0/24D470, not PCS X2.
        self.byte_permutation=self.elf.words(0x382fd0,128)
        self.original_block8=self.elf.words(0x3831d0,32)
        self.original_block32=self.elf.words(0x383250,32)
        self.original32_inverse={block:(i%8,i//8) for i,block in enumerate(self.original_block32)}
        source=(ROOT/'work/reference/pcsx2-master/pcsx2/GS/GSTables.cpp').read_text()
        self.gs_block32=source_table(source,'_blockTable32')
        self.gs_block8=source_table(source,'_blockTable8')
        self.gs_column32=source_table(source,'columnTable32')
        self.gs_column8=source_table(source,'columnTable8')
        self.gs_clut=source_table(source,'clutTableT32I8')
        assert [len(x) for x in [self.gs_block32,self.gs_block8,self.gs_column32,self.gs_column8,self.gs_clut]]==[32,32,64,256,128]

    def original_block(self,src):
        # Literal scalar translation of24D3E0: four64-byte input portions;
        # alternating original64-entry lookup tables; no inferred row flip.
        assert len(src)==256
        return [src[64*j+self.byte_permutation[64*(j&1)+i]] for j in range(4) for i in range(64)]

    def original_page(self,src):
        #24D470 converts one128x64 PSMT8 source page into64x32 PSMCT32 upload
        # pixels. Original block tables choose placement of each16x16 block.
        assert len(src)==8192;dst=[None]*8192
        for by in range(4):
            for bx in range(8):
                block=[src[(16*by+y)*128+16*bx+x] for y in range(16) for x in range(16)]
                converted=self.original_block(block)
                dx,dy=self.original32_inverse[self.original_block8[by*8+bx]]
                for y in range(8):
                    start=(dy*8+y)*256+dx*32;dst[start:start+32]=converted[y*32:(y+1)*32]
        assert None not in dst
        return dst

    def original_upload(self,src,w,h):
        #24D6F0 pads partial source pages, invokes24D470, then copies the
        # relevant output rows. This is the optional CPU upload conversion,
        # not the native asset decoder and never used by the final viewer.
        assert len(src)==w*h and w>=16 and h>=16
        dst=[None]*(w*h)
        for py in range((h+63)//64):
            for px in range((w+127)//128):
                cw=min(w,128);ch=min(h,64);page=[0]*8192
                for y in range(ch):
                    start=(py*64+y)*w+px*128
                    page[y*128:y*128+cw]=src[start:start+cw]
                converted=self.original_page(page)
                for y in range(ch//2):
                    start=(py*32+y)*(w*2)+px*256
                    dst[start:start+cw*2]=converted[y*256:y*256+cw*2]
        assert None not in dst
        return dst

    def address32(self,x,y,bw):
        # Byte address:64x32 pixel pages,8x8 blocks,4-byte pixels.
        page=(y//32)*bw+x//64
        return 8192*page+256*self.gs_block32[((y%32)//8)*8+(x%64)//8]+4*self.gs_column32[(y%8)*8+x%8]

    def address8(self,x,y,bw):
        # Byte address:128x64 pixel pages,16x16 blocks,1-byte pixels.
        page=(y//64)*(bw>>1)+x//128
        return 8192*page+256*self.gs_block8[((y%64)//16)*8+(x%128)//16]+self.gs_column8[(y%16)*16+x%16]

    def upload32_read8(self,upload,w,h):
        bw8=max(2,w//64);bw32=bw8//2
        # GS page footprint, including minimum128-texel stride for narrow images.
        vm=[None]*(((h+63)//64)*(bw8>>1)*8192)
        for y in range(h//2):
            for x in range(w//2):
                src=(y*(w//2)+x)*4;at=self.address32(x,y,bw32)
                vm[at:at+4]=upload[src:src+4]
        out=[vm[self.address8(x,y,bw8)] for y in range(h) for x in range(w)]
        assert None not in out
        return out

    def palette_mapping(self):
        # Original257B50 uploads raw palette as16x16 PSMCT32. Supplied
        # PCS X2 GSClut uses physical clutTableT32I8 for CSM1/CPSM32/CSA0.
        vm=[None]*256
        for i in range(256):vm[self.address32(i%16,i//16,1)//4]=i
        return [vm[self.gs_clut[i&127]|(i&128)] for i in range(256)]

def main():
    ref=ReferenceLayout();report=[]
    for w,h in [(w,h) for w in (16,32,64,128,256,512,1024) for h in (16,32,64,128,256,512,1024)]:
        # Every input coordinate has a unique integer label; unlike repeated
        # byte patterns, this detects every possible misplaced texel.
        labels=list(range(w*h));got=ref.upload32_read8(ref.original_upload(labels,w,h),w,h)
        bad=sum(x!=y for x,y in zip(labels,got));assert bad==0,(w,h,bad)
        report.append({'width':w,'height':h,'unique_coordinate_labels':len(labels),'different_texels':bad})
    mapping=ref.palette_mapping();swap=lambda i:(i&~24)|((i&8)<<1)|((i&16)>>1)
    assert mapping==[swap(i) for i in range(256)]
    result={'status':'PASS','original_upload_roundtrips':report,'palette_mapping':mapping,
            'palette_rule':'swap index bits3 and4; raw RGBA byte order retained',
            'scope':'Offline original-function translation + independent supplied PCS X2 memory/CLUT tables; no guest runtime or rasterizer executed.'}
    out=ROOT/'evidence/textures48D';out.mkdir(parents=True,exist_ok=True)
    (out/'layout-oracle.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k!='palette_mapping'}))

if __name__=='__main__':main()
