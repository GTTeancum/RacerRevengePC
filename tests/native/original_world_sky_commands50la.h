#pragma once
#include "../../src/assets/original_standard_material_context50la.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <memory>
#include <new>
#include <limits>
#include <utility>
#include <initializer_list>
#include <cfenv>
#include <cmath>

namespace ra_pod_scene50LA { class Owner; }
namespace original_world_sky50LA {
// CPU ownership only. These source command encodings are NEVER submitted to a
// native GPU. Header04 has no fabricated 32-bit guest address: its raw bytes
// remain reserved, and the actual native pointer is represented separately.
class Commands {
    friend class ::ra_pod_scene50LA::Owner;
    static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559,
                  "source pose stores require IEEE binary32");
    struct AlignedDelete {
        void operator()(std::uint8_t* p) const noexcept {
            ::operator delete(p,std::align_val_t(64));
        }
    };
    struct Packet {
        std::unique_ptr<std::uint8_t,AlignedDelete> bytes;
        std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA> context;
        const std::uint8_t* header04=nullptr;
        std::size_t size=0;
        std::uint32_t constructorPC=0;
        bool complete=false;
        // A partial allocation is owned even before the source return/publication.
        // Destruction requires no live Video/provider and invokes no command.
    };
    struct Plain133F80 {
        // Same existing owner and buffer20; native64 alignment is stronger than
        // source heap305700, with matching aligned64 deletion. No second buffer.
        std::unique_ptr<std::uint8_t,AlignedDelete> buffer20;
        std::uint32_t field24=0,field28=0;
        std::int32_t field2C=-1;
        // Unknown until134000 clears00..1F; raw04 stays unencoded after finish.
        std::array<std::uint32_t,8> header{};
        const std::uint8_t* nativeHeader04=nullptr;
        bool headerKnown=false,geometryAttempted=false,geometryActive=false;
        bool geometryComplete=false;
        std::uint32_t geometryPolicy=0,failurePC=0;
    };
    struct CellInput {
        std::size_t face=0,row=0,column=0;
        std::array<float,3> topLeft{},right{},below{},diagonal{};
        std::uint32_t policy36C0F0=0,subdivisionsX=3,subdivisionsY=2;
    };
    using Context=rr::assets::OriginalStandardMaterialContext50LA;
    static void w(Packet& p,std::size_t o,std::uint32_t v) noexcept {
        for(unsigned i=0;i<4;++i)p.bytes.get()[o+i]=std::uint8_t(v>>(8*i));
    }
    static void d(Packet& p,std::size_t o,std::uint64_t v) noexcept {
        for(unsigned i=0;i<8;++i)p.bytes.get()[o+i]=std::uint8_t(v>>(8*i));
    }
    static void h(Packet& p,std::size_t o,std::uint16_t v) noexcept {
        p.bytes.get()[o]=std::uint8_t(v);p.bytes.get()[o+1]=std::uint8_t(v>>8);
    }
    static void b(Packet& p,std::size_t o,std::uint8_t mask,std::uint8_t value) noexcept {
        auto& x=p.bytes.get()[o];x=std::uint8_t((x&mask)|value);
    }
    static bool begin(std::shared_ptr<const Context> c,std::size_t size,
        std::uint32_t entry,std::uint32_t allocationPC,
        std::unique_ptr<Packet>& out,std::uint32_t& pc) noexcept {
        pc=entry;
        if(out||!c||!c->live()||c->depthBufferBase2E0()!=280u||
            c->depthBufferFormat5C()!=0x31u)return false;
        try {
            out=std::make_unique<Packet>();out->context=std::move(c);
            out->constructorPC=entry;out->size=size;pc=allocationPC;
            out->bytes.reset(static_cast<std::uint8_t*>(::operator new(size,std::align_val_t(64))));
            // Source306478 follows305500 before the first command store.
            std::memset(out->bytes.get(),0,size);
            return true;
        } catch(...) {return false;}
    }
    static void preamble(Packet& p,std::uint32_t unpack) noexcept {
        w(p,0x10,0x11000000);w(p,0x14,0x03000000);
        w(p,0x18,0x02000000);w(p,0x1c,unpack);
    }
    static void firstTag(Packet& p,std::uint8_t registers) noexcept {
        b(p,0x27,0xf3,0);b(p,0x21,0x7f,0x80);
        b(p,0x27,0x0f,registers);h(p,0x20,0x8001);
        b(p,0x28,0xf0,0x0e);
    }
    static void finish(Packet& p,std::uint16_t count) noexcept {
        h(p,0,count);p.bytes.get()[2]=0;p.bytes.get()[3]=0x30;
        p.header04=p.bytes.get()+0x10; // source pointer store, native-width adaptation
        w(p,8,0);w(p,12,0);p.complete=true;
    }
    static bool canRetire(const Packet& p) noexcept {
        // No source/provider/Video read is needed to free actual CPU backing.
        if(!p.context || (p.size!=0x340 && p.size!=0xc0 && p.size!=0x50))return false;
        if(p.bytes && (reinterpret_cast<std::uintptr_t>(p.bytes.get())&63u))return false;
        if(!p.complete)return p.header04==nullptr;
        return p.bytes && p.header04==p.bytes.get()+0x10;
    }
    static bool current(const Packet& p,const Context& context) noexcept {
        if(!canRetire(p)||!p.complete||p.context.get()!=&context||!context.live())return false;
        return (p.constructorPC==0x12a120 && p.size==0x340)||
               (p.constructorPC==0x12a510 && p.size==0xc0)||
               (p.constructorPC==0x12a730 && p.size==0x50);
    }
    static bool canRetire(const Plain133F80& p) noexcept {
        if(p.geometryActive)return false;
        if(!p.geometryAttempted)return !p.buffer20&&p.field24==0&&p.field28==0&&
            p.field2C==-1&&!p.headerKnown&&!p.nativeHeader04&&!p.geometryComplete;
        if(!p.headerKnown||p.field2C!=1||p.field24!=128||p.field28>128||p.geometryPolicy>1)return false;
        if(p.buffer20&&(reinterpret_cast<std::uintptr_t>(p.buffer20.get())&63u))return false;
        if(!p.geometryComplete)return !p.nativeHeader04;
        const auto expected=p.geometryPolicy?28u:112u;
        return p.buffer20&&p.field28==expected&&p.nativeHeader04==p.buffer20.get()&&
            p.header[0]==(0x30000000u|(expected/4))&&p.header[1]==0&&
            p.header[2]==0&&p.header[3]==0&&p.header[4]==0&&p.header[5]==0&&
            p.header[6]==0&&p.header[7]==0;
    }
    // Earlier command-owner reentry must recognize this SAME owner's subsequent
    // partial/complete geometry state. This does not certify geometry return.
    static bool current(const Plain133F80& p) noexcept {return canRetire(p);}
    static bool buildGeometry12ACD0Or12B0D0(Plain133F80&,const CellInput&,
        std::uint32_t&) noexcept;
    static bool create133F80(std::unique_ptr<Plain133F80>& out,std::uint32_t& pc) noexcept {
        pc=0x133f80;if(out)return false;
        try {out=std::make_unique<Plain133F80>();pc=0x133f94;return true;}
        catch(...) {return false;}
    }
    static bool create12A120(std::shared_ptr<const Context> context,
        const std::array<float,3>& innerPose,
        std::unique_ptr<Packet>& out,std::uint32_t& pc) noexcept {
        pc=0x12a120;
        // This actual RA inner cube has source pose XYZ=0; no numeric transform
        // or source unknown W is inferred. Caller proves the same inner owner.
        if(innerPose[0]!=0.f||innerPose[1]!=0.f||innerPose[2]!=0.f)return false;
        if(!begin(std::move(context),0x340,0x12a120,0x12a154,out,pc))return false;
        auto& p=*out;preamble(p,0x6c028320);firstTag(p,0x10);
        d(p,0x30,0x0005000c);d(p,0x38,0x47);
        w(p,0x40,0x14000000);w(p,0x4c,0x6c2a8000);
        w(p,0x50,0x3f800000);w(p,0x64,0x3f800000);w(p,0x78,0x3f800000);
        // Source load/store of pose lanes (including signed zero bit patterns).
        for(unsigned i=0;i<3;++i){std::uint32_t bits;std::memcpy(&bits,&innerPose[i],4);w(p,0x80+4*i,bits);}
        w(p,0x8c,0x3f800000);
        for(auto o:{0x190,0x1a4,0x1b8,0x1cc})w(p,o,0x3f800000);
        for(auto o:{0x1e8,0x1e4,0x1e0,0x1f8,0x1f4,0x1f0,0x208,0x204,0x200,0x218,0x214,0x210})w(p,o,0x42fe0000);
        // 12A304..3A0: zero QW, EOP, PRE, PRIM=29, FLG=0,NREG=3.
        b(p,0x271,0x7f,0x80);b(p,0x275,0xbf,0x40);
        d(p,0x270,(std::uint64_t(29)<<47)|(std::uint64_t(0x40)<<40)|0x8000);
        b(p,0x277,0xf3,0);b(p,0x277,0x0f,0x30);
        b(p,0x278,0xf0,2);b(p,0x278,0x0f,0x10);b(p,0x279,0xf0,4);h(p,0x270,0x8005);
        // Two independent source QWs, not aliases of the first tag.
        for(auto o:{0x280,0x2a0}) {
            h(p,o,1);b(p,o+1,0x7f,0x80);b(p,o+5,0xbf,0x40);
            b(p,o+7,0xf3,0);b(p,o+7,0x0f,0x10);b(p,o+8,0xf0,0x0f);
            w(p,o+0x1c,0x7f);
        }
        // Remaining zero stores are already actual306478 zero backing.
        w(p,0x2e0,6);w(p,0x2fc,0x6c02802b);
        w(p,0x320,0x14000010);w(p,0x324,0x10000000);
        w(p,0x328,0x030000b5);w(p,0x32c,0x02000190);
        finish(p,0x33);pc=0x12a500;return true;
    }
    static bool create12A510(std::shared_ptr<const Context> context,
        const std::array<float,3>& innerPose,std::uint32_t sourceSkyPolicy3E0990,
        std::unique_ptr<Packet>& out,std::uint32_t& pc) noexcept {
        pc=0x12a510;
        if(sourceSkyPolicy3E0990!=1||innerPose[0]!=0.f||innerPose[1]!=0.f||innerPose[2]!=0.f)return false;
        if(!begin(std::move(context),0xc0,0x12a510,0x12a530,out,pc))return false;
        auto& p=*out;preamble(p,0x6c038320);firstTag(p,0x20);b(p,0x28,0x0f,0xe0);
        d(p,0x38,6);
        // 12A64C MOVZ: policy!=0 leaves a0=0, NOT 1. Thus bit32 is zero.
        d(p,0x40,std::uint64_t(p.context->depthBufferBase2E0())|
            (std::uint64_t(p.context->depthBufferFormat5C())<<24));
        d(p,0x48,0x4e);w(p,0x50,0x14000000);w(p,0x5c,0x6c048000);
        w(p,0x60,0x3f800000);w(p,0x74,0x3f800000);w(p,0x88,0x3f800000);
        for(unsigned i=0;i<3;++i){std::uint32_t bits;std::memcpy(&bits,&innerPose[i],4);w(p,0x90+4*i,bits);}
        w(p,0x9c,0x3f800000);w(p,0xa0,0x14000010);w(p,0xa4,0x10000000);
        w(p,0xa8,0x030000b5);w(p,0xac,0x02000190);
        finish(p,0xb);pc=0x12a720;return true;
    }
    static bool create12A730(std::shared_ptr<const Context> context,
        std::unique_ptr<Packet>& out,std::uint32_t& pc) noexcept {
        if(!begin(std::move(context),0x50,0x12a730,0x12a748,out,pc))return false;
        auto& p=*out;preamble(p,0x6c028320);firstTag(p,0x10);
        d(p,0x30,std::uint64_t(p.context->depthBufferBase2E0())|
            (std::uint64_t(p.context->depthBufferFormat5C())<<24));
        d(p,0x38,0x4e);w(p,0x40,0x14000000);w(p,0x44,0x10000000);
        finish(p,4);pc=0x12a890;return true;
    }
};
}

#include "original_world_sky_geometry50la.inl"
