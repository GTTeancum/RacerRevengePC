// Exactness harness, off by default (RR_GARAGE_KERNEL_SHADOW=1). This translation
// unit compiles the four Garage numeric kernels a second time, renamed, with
// their former volatile-spill float helpers, so the Garage model lowering can
// run every surface through both versions and compare the lowered packets.
#define RR_GARAGE_VOLATILE_KERNELS 1
#define garage_prelit50KG garage_prelit50KG_volatile50
#define garage_diffuse50KG garage_diffuse50KG_volatile50
#define garage_specular50KG garage_specular50KG_volatile50
#define garage_palette50KG garage_palette50KG_volatile50
#include "../../../src/game/original_ui_garage_prelit50kg.cpp"
#include "../../../src/game/original_ui_garage_diffuse50kg.cpp"
#include "../../../src/game/original_ui_garage_specular50kg.cpp"
#include "../../../src/game/original_ui_garage_palette_packet50kg.cpp"
#include <cstring>
#include <new>
#include <type_traits>

namespace rr::platform::garage_kernel_shadow50KG {
namespace P=rr::game::garage_prelit50KG_volatile50;
namespace D=rr::game::garage_diffuse50KG_volatile50;
namespace S=rr::game::garage_specular50KG_volatile50;
namespace L=rr::game::garage_palette50KG_volatile50;
static_assert(std::is_trivially_copyable_v<P::Context>&&std::is_trivially_copyable_v<P::Preamble>&&
    std::is_trivially_copyable_v<P::RawVertex>&&std::is_trivially_copyable_v<P::Polygon>&&
    std::is_trivially_copyable_v<D::Context>&&std::is_trivially_copyable_v<D::Preamble>&&std::is_trivially_copyable_v<D::RawVertex>&&
    std::is_trivially_copyable_v<S::Context>&&std::is_trivially_copyable_v<S::Preamble>&&std::is_trivially_copyable_v<S::RawVertex>&&
    std::is_trivially_copyable_v<L::Context>&&std::is_trivially_copyable_v<L::Preamble>&&std::is_trivially_copyable_v<L::RawVertex>);
// which: 0 prelit, 1 diffuse, 2 specular, 3 palette; part: 0 Context, 1 Preamble, 2 RawVertex, 3 Polygon
std::size_t size(int which,int part)noexcept {
    switch(which*4+part){
    case 0:return sizeof(P::Context);case 1:return sizeof(P::Preamble);case 2:return sizeof(P::RawVertex);case 3:return sizeof(P::Polygon);
    case 4:return sizeof(D::Context);case 5:return sizeof(D::Preamble);case 6:return sizeof(D::RawVertex);case 7:return sizeof(P::Polygon);
    case 8:return sizeof(S::Context);case 9:return sizeof(S::Preamble);case 10:return sizeof(S::RawVertex);case 11:return sizeof(P::Polygon);
    case 12:return sizeof(L::Context);case 13:return sizeof(L::Preamble);case 14:return sizeof(L::RawVertex);case 15:return sizeof(P::Polygon);
    }
    return 0;
}
template<class T>T copy(const void* p)noexcept{T t;std::memcpy(static_cast<void*>(&t),p,sizeof t);return t;}
int prepare(int which,const void* context,const rr::game::OriginalRacerModelPacket50LA* packet,int family,void* preamble)noexcept {
    switch(which){
    case 0:{auto c=copy<P::Context>(context);auto* out=new(std::nothrow)P::Preamble();if(!out)return -1;
        const auto s=P::prepare(c,*out);std::memcpy(preamble,static_cast<const void*>(out),sizeof *out);delete out;return int(s);}
    case 1:{auto c=copy<D::Context>(context);auto* out=new(std::nothrow)D::Preamble();if(!out)return -1;
        const auto s=D::prepare(c,*out);std::memcpy(preamble,static_cast<const void*>(out),sizeof *out);delete out;return int(s);}
    case 2:{auto c=copy<S::Context>(context);auto* out=new(std::nothrow)S::Preamble();if(!out)return -1;
        const auto s=S::prepare(c,*out);std::memcpy(preamble,static_cast<const void*>(out),sizeof *out);delete out;return int(s);}
    case 3:{auto c=copy<L::Context>(context);auto* out=new(std::nothrow)L::Preamble();if(!out||!packet){delete out;return -1;}
        const auto s=L::prepare(*packet,c,static_cast<L::Family>(family),*out);
        std::memcpy(preamble,static_cast<const void*>(out),sizeof *out);delete out;return int(s);}
    }
    return -1;
}
int project(int which,const void* preamble,const void* raw3,void* polygon)noexcept {
    // preamble: 16-byte aligned storage holding the bytes prepare() produced.
    P::Polygon out{};int s=-1;
    switch(which){
    case 0:{const auto r=copy<std::array<P::RawVertex,3>>(raw3);
        s=int(P::projectTriangle(*static_cast<const P::Preamble*>(preamble),r,out));break;}
    case 1:{const auto r=copy<std::array<D::RawVertex,3>>(raw3);
        s=int(D::projectTriangle(*static_cast<const D::Preamble*>(preamble),r,out));break;}
    case 2:{const auto r=copy<std::array<S::RawVertex,3>>(raw3);
        s=int(S::projectTriangle(*static_cast<const S::Preamble*>(preamble),r,out));break;}
    case 3:{const auto r=copy<std::array<L::RawVertex,3>>(raw3);
        s=int(L::projectTriangle(*static_cast<const L::Preamble*>(preamble),r,out));break;}
    }
    std::memcpy(polygon,static_cast<const void*>(&out),sizeof out);return s;
}
}
