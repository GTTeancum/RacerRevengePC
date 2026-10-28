#pragma once
// Implementation-only F8 type1 backing. No Pause/Garage object or render
// grant; these are this F8 owner's actual2E1AE0 packet/primitive allocations.
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <optional>
#include <type_traits>

namespace rr::game {
struct OriginalRaceF8PolygonStorage50LA final {
    using Qword=std::array<std::uint32_t,4>;
    struct alignas(16) PacketVertex {
        Qword rgba{};
        // Historical source+14 name STQ denotes POSITION. Source+24 denotes
        // texture coordinates. Retain their real ordered packet destinations.
        std::array<float,4> stq{},xyz{};
    };
    struct alignas(16) Packet {
        Qword dma00{},vif10{},gif20{};
        std::array<float,4> viewport30{};
        std::array<PacketVertex,4> vertices{};
        Qword end100{},unused110{};
    };
    static_assert(std::is_standard_layout_v<Packet>&&sizeof(PacketVertex)==48&&
        offsetof(Packet,vertices)==64&&offsetof(Packet,end100)==256&&sizeof(Packet)==288);
    struct PacketDelete {
        void operator()(Packet* packet)const noexcept {
            if(packet){std::destroy_at(packet);::operator delete(packet,std::align_val_t{64});}
        }
    };
    struct Descriptor {
        std::unique_ptr<Packet,PacketDelete> allocation00;
        Qword* vif04=nullptr;Qword* gif08=nullptr;
        std::array<float,4>* tag0C=nullptr;
        //2E1AE0 never initializes descriptor+10 on this path. Do not certify0.
        std::optional<std::uint32_t> field10;
        Qword* dmaData4=nullptr; //Native address, never packed into a guest word.
        bool pointersKnown=false;
    };
    struct Primitive {
        //299AA0/2461E0 constructor, then real2465B0 retain and2E2144.
        std::int32_t material00=-1;void* optional04=nullptr;
        std::uint8_t type0C=0,field0D=0,field0E=255,flags0F=0;
        Packet* packet10=nullptr;void* optional14=nullptr;
        float scale18=1.f,scale1C=1.f;
        std::uint16_t field20=0,field22=0;
        std::size_t reference=0; //Native existing canonical-reference ledger index.
        bool materialStored=false,materialRetained=false;
    };
    struct Vertex {
        std::array<float,4> authored00{{0,0,1,1}}; //2E2190.
        std::uint32_t aliases10=0;
        std::array<float,4>* stq14=nullptr;
        std::array<float,4>* secondary18=nullptr;
        Qword* rgba1C=nullptr;Qword* secondaryColor20=nullptr;
        std::array<float,4>* xyz24=nullptr;
        std::array<float,4>* secondaryTexture28=nullptr;
    };
    struct Polygon {
        std::unique_ptr<Vertex[]> vertices00;
        std::uint32_t count04=0,count08=0,type0C=2;
        float scale10=1.f,field14=0.f,field18=1.f;
        std::array<float,4> offset20{{0,0,0,1}};
        std::unique_ptr<std::unique_ptr<Primitive>[]> primitives30;
        std::uint32_t count34=0,owns38=0,dirty3C=0;
        std::uint8_t flags40=3;
        std::unique_ptr<Descriptor[]> packets44;
        std::uint32_t count48=0,capacity4C=0;
        // Native mutation/lifetime history, not original packet fields or a
        // submission certificate. No F8 FIFO record producer exists yet.
        std::uint64_t packetRevision=0;
        std::uint32_t nativePacketPins=0;
    };
};
}
