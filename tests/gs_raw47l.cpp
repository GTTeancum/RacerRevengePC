#include "rr_gs_capture.h"
#include <memory>
#include <cstring>
#include <cstdio>

// Raw-GIF stand-in intentionally ignores structured draws, just like Borrowed41n.
// The non-raw mode depends on them and detects accidentally elided submissions.
class ObservedBackend final : public GSRasterBackend {
public:
    explicit ObservedBackend(bool raw):raw(raw){}
    bool raw; uint64_t submits=0,packets=0,flushes=0,syncs=0,cluts=0,uploads=0;
    uint32_t streamHash=2166136261u,drawHash=2166136261u;
    uint8_t* vram=nullptr;uint32_t size=0;GSTransferSnapshot transfer{};
    void Initialize(uint8_t* p,uint32_t n) override {vram=p;size=n;}
    void Reset() override {transfer={};}
    bool SupportsRawGIF() const override {return raw;}
    void ProcessRawGIF(unsigned path,const uint8_t* p,uint32_t n) override {
        ++packets;streamHash=(streamHash^path)*16777619u;
        for(uint32_t i=0;i<n;++i)streamHash=(streamHash^p[i])*16777619u;
    }
    void Submit(const GSPrimitiveBatch& b) override {
        ++submits;
        if(raw)return;
        auto mix=[&](uint32_t n){drawHash=(drawHash^n)*16777619u;};
        mix(b.vertexCount);mix(b.state.prim.type);mix(b.state.context.frame.fbp);
        mix(b.state.context.tex0.tbp0);mix(b.state.textureWidth);mix(b.state.textureHeight);
        mix(b.state.linearFilter);mix(static_cast<uint32_t>(b.state.context.alpha));
        for(unsigned i=0;i<b.vertexCount;++i){const auto& v=b.vertices[i];
            mix(static_cast<uint32_t>(v.x*16));mix(static_cast<uint32_t>(v.y*16));
            mix(static_cast<uint32_t>(v.z));mix(v.r|(v.g<<8)|(v.b<<16)|(v.a<<24));}
    }
    void LoadClut(const GSTex0Reg&,const GSTexClutReg&) override {++cluts;}
    void BeginTransfer(const GSTransferCommand& c) override {
        transfer={};transfer.direction=c.direction;transfer.x=c.trxpos.dsax;
        transfer.y=c.trxpos.dsay;transfer.totalPixels=uint32_t(c.trxreg.rrw)*c.trxreg.rrh;
    }
    void UploadImage(const uint8_t*,uint32_t n) override {uploads+=n;transfer.copiedPixels+=n/4;}
    void Flush() override {++flushes;}
    void TextureFlush() override {}
    void Sync(GSSyncReason) override {++syncs;}
    PresentationFrame Present(const GSPresentationRequest& r) override {
        // The existing frontend capture API uses a fixed 640-pixel row stride.
        PresentationFrame f;f.width=640;f.height=2;f.pixels.resize(640*2*4);
        const uint32_t color=raw?streamHash:drawHash;
        for(unsigned i=0;i<f.pixels.size();++i)f.pixels[i]=static_cast<uint8_t>(color>>((i%4)*8));
        f.displayFbp=static_cast<uint32_t>(r.dispfb1&511);
        f.sourceFbp=r.hasPreferredSource?r.preferredSource.fbp:f.displayFbp;
        f.usedPreferred=r.hasPreferredSource;return f;
    }
    bool ClearFramebuffer(const GSContext&,uint32_t) override {return false;}
    uint32_t ConsumeLocalToHostBytes(uint8_t*,uint32_t) override {return 0;}
    uint32_t ReadVram(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t) const override {return 0;}
    void WriteVram(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t) override {}
    void SnapshotVram(std::vector<uint8_t>& out) const override {out.assign(vram,vram+size);}
    GSTransferSnapshot GetTransferSnapshot() const override {return transfer;}
};
int main(int argc,char** argv) {
    if(argc!=3)return 2;
    _putenv_s("RR_PCSX2_GS_DLL","");_putenv_s("RR_GS_GPU33T_SHADER","");
    const bool raw=std::atoi(argv[2])!=0;
    std::vector<uint8_t> vram(4194304);GSRegisters regs{};
    auto gs=std::make_unique<GS>();gs->init(vram.data(),static_cast<uint32_t>(vram.size()),&regs);
    auto backend=std::make_unique<ObservedBackend>(raw);auto* observed=backend.get();
    gs->setRasterBackend(std::move(backend));
    gs->setDebugHistoryPaused(false); // Match the real capture-enabled game probe.
    auto reg=[&](uint8_t address,uint64_t value){
        uint64_t packet[4]={1u|(1ull<<15)|(1ull<<60),0x0e,value,address};
        gs->processGIFPacket(reinterpret_cast<const uint8_t*>(packet),sizeof(packet),3);
    };
    auto xyz=[](unsigned x,unsigned y,uint32_t z){return uint64_t(x*16)|(uint64_t(y*16)<<16)|(uint64_t(z)<<32);};
    uint32_t fixture=0,preferredHits=0; uint64_t checks=0;
    std::filesystem::create_directories(argv[1]);
    std::ofstream state(std::filesystem::path(argv[1])/"state.txt");
    for(unsigned config=0;config<3;++config)for(unsigned context=0;context<2;++context)for(unsigned prim=0;prim<7;++prim){
        gs->clearDebugHistory();regs.vsyncTick.store(++fixture);
        reg(GS_REG_PRMODECONT,1);
        reg(GS_REG_FRAME_1+context,20u|(10ull<<16));
        reg(GS_REG_TEX0_1+context,10u|(10ull<<14)|(6ull<<26)|(6ull<<30));
        reg(GS_REG_XYOFFSET_1+context,0);
        reg(GS_REG_ALPHA_1+context,0x64u|((config==1?0x80ull:0x60ull)<<32));
        reg(GS_REG_TEST_1+context,0x50013);
        reg(GS_REG_SCISSOR_1+context,(639ull<<16)|(447ull<<48));
        reg(GS_REG_TEX1_1+context,uint64_t(config)<<5);
        reg(GS_REG_PRIM,prim|(context<<9)|(1u<<4)|(1u<<6)|(1u<<8));
        for(unsigned v=0;v<12;++v){
            reg(GS_REG_RGBAQ,0x3f80000000000000ull|(uint64_t(v*13)<<24)|0x00332211);
            reg(GS_REG_UV,uint64_t(v*16)|(uint64_t(v*32)<<16));
            unsigned x=(v&1)?640:0,y=(v&1)?448:0;
            if(config==1){x=640-x;y=448-y;}
            if(config==2){x=10+v*5;y=20+v*3;}
            reg(config==2 && v%4==0?GS_REG_XYZ3:GS_REG_XYZ2,xyz(x,y,0x123456+v));
            GSFrameReg frame{};uint32_t dest=0;
            bool preferred=gs->getPreferredDisplaySource(frame,dest);preferredHits+=preferred;
            state<<fixture<<' '<<v<<' '<<preferred<<' '<<frame.fbp<<' '<<frame.fbw<<' '<<unsigned(frame.psm)<<' '<<dest<<'\n';
            ++checks;
        }
        reg(GS_REG_SIGNAL,0x0fff00fff1234567ull);
        reg(GS_REG_LABEL,0xffff00001234abcdull);
        reg(GS_REG_FINISH,0);
        state<<"irq "<<regs.csr.load()<<' '<<regs.siglblid<<'\n';
        rrCaptureGs(*gs,regs,(std::filesystem::path(argv[1])/("frame-"+std::to_string(fixture))).string(),fixture*100);
    }
    state.close();
    std::ofstream counters(std::filesystem::path(argv[1])/"backend.json");
    counters<<"{\"submits\":"<<observed->submits<<",\"packets\":"<<observed->packets
        <<",\"flushes\":"<<observed->flushes<<",\"syncs\":"<<observed->syncs<<",\"cluts\":"<<observed->cluts
        <<",\"stream_hash\":"<<observed->streamHash<<",\"draw_hash\":"<<observed->drawHash
        <<",\"preferred_hits\":"<<preferredHits<<",\"checks\":"<<checks<<"}\n";
    std::printf("GS47L fixtures=%u preferred_hits=%u state_observations=%llu\n",fixture,preferredHits,checks);
    return preferredHits?0:3;
}
