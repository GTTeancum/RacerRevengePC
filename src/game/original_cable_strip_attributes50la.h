namespace ra_pod_scene50LA {class Owner;}
namespace original_local_flare50LA {class Emission;}
#pragma once
namespace original_cable_selection50LA { class Binding; }
#include "original_application_entities50la.h"
#include "original_strip_stream_history23cca0.h"
#include <array>
#include <cstdint>
#include <memory>
#include <optional>

namespace rr::game {
class OriginalRacerStripOwners50LA;
class OriginalPodCableAllocation50LA;
// Native CPU attributes. No guest commands, byte offsets, addresses, packing,
// interpreter or submission capability. A later native renderer consumes these
// typed attributes after their genuine producers have supplied every value.
class OriginalCableStripAttributes50LA final {
    friend class ::original_local_flare50LA::Emission;
public:
    struct Scalar {
        std::optional<float> value;
        // The uninitialized23CCA0 scratch lane copied by the source. Absence
        // remains explicit until a genuine setter publishes a numeric value.
        std::uint8_t unknownScratchLane=0xffu;
        static Scalar unknown(std::uint8_t lane)noexcept{return {std::nullopt,lane};}
    };
    struct Position {
        std::array<Scalar,3> xyz{{Scalar::unknown(0),Scalar::unknown(1),Scalar::unknown(2)}};
        // Source vertex control value, NOT a homogeneous position coordinate.
        std::int32_t stripControl=0;
    };
    struct Triple {
        std::array<Scalar,3> components{{Scalar::unknown(0),Scalar::unknown(1),Scalar::unknown(2)}};
    };
    struct Texcoord {
        std::array<Scalar,2> uv{{Scalar::unknown(0),Scalar::unknown(1)}};
    };
    using Color=std::array<std::optional<std::uint32_t>,4>;
    struct VertexAlias {std::uint32_t batch=0,ordinal=0;};
    struct VertexCopies {
        std::uint32_t count=0;
        std::array<VertexAlias,2> aliases{};
    };
    struct Batch {
        std::uint32_t count=0;
        const Position* positions=nullptr;
        const Triple* triples=nullptr;
        const Texcoord* texcoords=nullptr;
    };
    OriginalCableStripAttributes50LA()=default;
    OriginalCableStripAttributes50LA(const OriginalCableStripAttributes50LA&)=delete;
    OriginalCableStripAttributes50LA& operator=(const OriginalCableStripAttributes50LA&)=delete;
    // Inspection only, including unknown values. This is not render readiness.
    const Batch* batches()const noexcept{return group_?group_->batches.get():nullptr;}
    std::uint32_t batchCount()const noexcept{return group_?group_->batchCount:0;}
    std::uint32_t originalVertexCount()const noexcept{return group_?group_->vertexCount:0;}
    const VertexCopies* originalVertices()const noexcept{return group_?group_->aliases.get():nullptr;}
private:
    friend class OriginalPodCableAllocation50LA;
    friend class ::ra_pod_scene50LA::Owner; //Concrete14AB30 and1B3BB0 World strip services; no racer-owner substitution.
    friend class ::original_cable_selection50LA::Binding;
    friend class OriginalRacerStripOwners50LA;
    using Matrix=std::array<std::array<float,4>,4>;
    // Retain only numeric data initialized by23CB20. Their source member
    // locations identify them without assigning an unproven render meaning.
    // There is no DMA/VIF wrapper or self-address field.
    struct CommonData {
        enum class Layout { cableFormat7 };
        // Native semantic selection, never source packet words.
        std::optional<Layout> layout;
        Matrix matrix40{},matrix90{},matrixE0{};
        // C0 IS the fourth90 row:23CC14 initializes it to zero. Callback
        // stores must update this same backing, not an independent color.
        std::array<float,4>& colorC0()noexcept{return matrix90[3];}
        std::array<float,4> vector130{{1.f,1.f,1.f,1.f}};
        CommonData()noexcept {
            for(unsigned i=0;i<4;++i){matrix40[i][i]=1.f;matrixE0[i][i]=1.f;}
        }
    };
    template<class T> struct View {T* values=nullptr;std::uint32_t count=0;};
    struct NativeVertices {
        std::array<Position,20> positions;
        std::array<Triple,20> triples;
        std::array<Texcoord,20> texcoords;
        std::array<Color,20> colors;
    };
    struct Group {
        std::uint32_t vertexCount=0,batchCount=0;
        std::unique_ptr<std::uint32_t[]> counts;
        std::unique_ptr<Batch[]> batches;
        std::unique_ptr<View<Position>[]> positions;
        std::unique_ptr<View<Triple>[]> triples;
        // Format7 does not publish these source streams. Empty optionals
        // record absence; they never stand for authored zero attributes.
        std::unique_ptr<std::optional<View<Triple>>[]> extra;
        std::unique_ptr<std::optional<View<Color>>[]> colors;
        std::unique_ptr<View<Texcoord>[]> texcoords;
        std::unique_ptr<VertexCopies[]> aliases;
    };
    std::unique_ptr<CommonData> common_;
    std::unique_ptr<NativeVertices> vertices_;
    std::unique_ptr<Group> group_;
    std::unique_ptr<std::uint32_t[]> groupCounts_;

    void initializeVertex174138(const OriginalApplicationEntities50LA& app,
        std::uint32_t vertex)noexcept {
        const auto& copies=group_->aliases[vertex];
        //174138 ->23C260 writes all copies of triple first;174160 ->23C090
        //then writes all position copies, preserving each stripControl.
        for(std::uint32_t c=0;c<copies.count;++c){
            const auto a=copies.aliases[c];
            auto& triple=group_->triples[a.batch].values[a.ordinal];
            for(unsigned k=0;k<3;++k)triple.components[k]={app.cableTriple433CF0_[k],0xffu};
        }
        for(std::uint32_t c=0;c<copies.count;++c){
            const auto a=copies.aliases[c];
            auto& position=group_->positions[a.batch].values[a.ordinal];
            for(unsigned k=0;k<3;++k)position.xyz[k]={app.cableOrigin433CD0_[k],0xffu};
        }
    }
    void setTexcoord23C800(std::uint32_t vertex,float u,float v)noexcept {
        OriginalStripStreamHistory23CCA0::invalidateNative(); //Legacy view-only writes cannot seed authentic UV process history.
        const auto& copies=group_->aliases[vertex];
        for(std::uint32_t c=0;c<copies.count;++c){
            const auto a=copies.aliases[c];auto& uv=group_->texcoords[a.batch].values[a.ordinal].uv;
            uv[0]={u,0xffu};uv[1]={v,0xffu};
        }
    }

    // Source lifetime order: group, common data, group counts, native payload,
    // then per-group arrays. Command-only builder bookkeeping has no native
    // consumer and is intentionally not reproduced as observable App state.
    void construct23BEB0(std::uint32_t& pc) {
        OriginalStripStreamHistory23CCA0::invalidateNative(); //Legacy projection cannot seed authentic process guards.
        pc=0x23bef0u;group_=std::make_unique<Group>();
        pc=0x23cb34u;common_=std::make_unique<CommonData>();
        pc=0x23bf30u;groupCounts_.reset(new std::uint32_t[1]);groupCounts_[0]=18;
        pc=0x23bf78u;vertices_=std::make_unique<NativeVertices>();
        auto& g=*group_;g.vertexCount=18;g.batchCount=2;
        pc=0x23cea4u;g.counts.reset(new std::uint32_t[2]);
        pc=0x23ceb4u;g.batches.reset(new Batch[2]);
        pc=0x23cec4u;g.positions.reset(new View<Position>[2]);
        pc=0x23ced4u;g.triples.reset(new View<Triple>[2]);
        pc=0x23cee4u;g.extra.reset(new std::optional<View<Triple>>[2]);
        pc=0x23cef4u;g.colors.reset(new std::optional<View<Color>>[2]);
        pc=0x23cf04u;g.texcoords.reset(new View<Texcoord>[2]);
        pc=0x23cf20u;g.aliases.reset(new VertexCopies[18]);
        for(std::uint32_t batch=0;batch<2;++batch){
            const std::uint32_t n=batch?4u:16u,firstOriginal=batch*14u,firstNative=batch*16u;
            g.counts[batch]=n;
            g.positions[batch]={vertices_->positions.data()+firstNative,n};
            g.triples[batch]={vertices_->triples.data()+firstNative,n};
            g.texcoords[batch]={vertices_->texcoords.data()+firstNative,n};
            g.batches[batch]={n,g.positions[batch].values,g.triples[batch].values,g.texcoords[batch].values};
            for(std::uint32_t v=0;v<n;++v){
                auto& copies=g.aliases[firstOriginal+v];
                copies.aliases[copies.count++]={batch,v};
                vertices_->positions[firstNative+v].stripControl=v<2?-14:14;
            }
        }
    }
    // Same23CCA0 numeric streams for one genuine four-vertex strip.
    // Format15 adds color; source scratch contents remain unknown until written.
    void constructQuad23BEB0(std::uint8_t format,std::uint32_t& pc) {
        OriginalStripStreamHistory23CCA0::invalidateNative(); //Same unrepresented process path, sticky across cleanup.
        pc=0x23bef0u;group_=std::make_unique<Group>();
        pc=0x23cb34u;common_=std::make_unique<CommonData>();
        pc=0x23bf30u;groupCounts_.reset(new std::uint32_t[1]);groupCounts_[0]=4;
        pc=0x23bf78u;vertices_=std::make_unique<NativeVertices>();
        auto& g=*group_;g.vertexCount=4;g.batchCount=1;
        pc=0x23cea4u;g.counts.reset(new std::uint32_t[1]);g.counts[0]=4;
        g.batches.reset(new Batch[1]);g.positions.reset(new View<Position>[1]);
        g.triples.reset(new View<Triple>[1]);g.extra.reset(new std::optional<View<Triple>>[1]);
        g.colors.reset(new std::optional<View<Color>>[1]);g.texcoords.reset(new View<Texcoord>[1]);
        g.aliases.reset(new VertexCopies[4]);
        g.positions[0]={vertices_->positions.data(),4};g.triples[0]={vertices_->triples.data(),4};
        g.texcoords[0]={vertices_->texcoords.data(),4};
        if(format&8u)g.colors[0]=View<Color>{vertices_->colors.data(),4};
        g.batches[0]={4,g.positions[0].values,g.triples[0].values,g.texcoords[0].values};
        for(std::uint32_t i=0;i<4;++i){g.aliases[i].count=1;g.aliases[i].aliases[0]={0,i};
            vertices_->positions[i].stripControl=i<2?-14:14;}
    }
    void release23BCF0()noexcept {
        common_.reset(); //23BD24, numeric common owner
        vertices_.reset(); //Native payload owner, source54 lifetime position
        if(group_){auto& g=*group_;
            g.counts.reset();g.batches.reset();g.positions.reset();g.triples.reset();
            g.extra.reset();g.colors.reset();g.texcoords.reset();g.aliases.reset();
            group_.reset();}
        groupCounts_.reset();
    }
};
}
