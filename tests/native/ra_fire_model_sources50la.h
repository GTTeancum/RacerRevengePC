#pragma once
#include "ra_cold_sources50la.h"
#include "../../src/assets/original_standard_material_domain50la.h"
#include "../../src/assets/original_standard_model_resource50la.h"
#include <array>
#include <cstring>
#include <exception>
#include <memory>
#include <utility>

namespace ra_fire_model_sources50LA {
namespace a=rr::assets;
// Actual immutable PSG payload and26E640 material-reference facets only.
// No240110 geometry, model Entity/graph identity, SoulTree attachment, packet,
// cache publication, populated material or renderer readiness is certified.
class Owner final {
public:
    struct Entry {
        std::size_t worldInstance=0;
        std::shared_ptr<const a::OriginalStandardModelResource50LA> payload;
        a::OriginalStandardMaterialReferences50LA materials;
    };
    struct Diagnostic {
        std::size_t entry=0,retainedPrefixes=0;
        const char* failure=nullptr;
        a::DecodeError48C decode{};
        a::OriginalStandardResourceDiagnostic50LA resource{};
        a::OriginalStandardMaterialDiagnostic50LA material{};
    };
private:
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain_;
    std::array<std::unique_ptr<Entry>,4> entries_{};
    bool attempted_=false,completed_=false,closed_=false;
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner(){if(!close())std::terminate();}
    bool prepare(std::shared_ptr<const ra_cold_sources50LA::Owner> source,
        std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain,
        const a::OriginalStandardMaterialPath50LA& actualPath,Diagnostic& d) noexcept {
        d={};const auto fail=[&](const char* why){d.failure=why;return false;};
        if(attempted_||closed_)return fail("RA model facet stage already attempted");attempted_=true;
        try {
            const auto* data=source?source->sourcesDecoded():nullptr;
            if(!data||!domain||!domain->prepared()||!actualPath.actualPrefix36B970||
               *actualPath.actualPrefix36B970!="textures\\")return fail("actual RA source, Application domain and texture directory required");
            const auto& provider=domain->archiveProvider3E1A70();
            if(!provider||!provider->sealed())return fail("actual sealed Application provider required");
            source_=std::move(source);domain_=std::move(domain);
            constexpr const char* hashes[]={
                "73ae9da221e542a57bc10397db61cc3f097aeed886a298e85d33edfa2c5982b0",
                "099498382e44dd50648e9c6807ea728a68af26e89f0c8f1f2266e6bf0b9ce10d",
                "29b7d02acf3bb7aa55a4386df2718be56c994b5622ddb80ad84cdcd3b6ef3bac",
                "e9a2e93f2e11fbbe3a6251623ec2ca47480968542e94505746defe1f5d5f459e"};
            for(std::size_t i=0;i<4;++i){
                d.entry=i;const auto member="geometry\\firep0t0"+std::to_string(i+1)+".psg";
                const auto slt="FireP0t0"+std::to_string(i+1)+".slt";
                std::size_t placement=0,matches=0;
                for(std::size_t n=0;n<data->world.instances.size();++n){const auto& instance=data->world.instances[n];
                    if(instance.zone!=1||instance.ordinal!=366+i)continue;
                    if(instance.geometry_member!=member||instance.parameter_block>=data->world.parameters.blocks.size())
                        return fail("actual FireP0t source placement differs");
                    std::size_t names=0;
                    for(const auto& field:data->world.parameters.blocks[instance.parameter_block].fields)
                        if(field.name=="slt"){if(field.value!=slt)return fail("actual SoulTree source name differs");++names;}
                    if(names!=1)return fail("unique actual SoulTree source field required");
                    placement=n;++matches;
                }
                if(matches!=1)return fail("unique actual zone1 FireP0t placement required");
                std::vector<std::uint8_t> bytes;
                if(!a::extractResMember48C(data->archive,member,bytes,d.decode,816)||bytes.size()!=816||
                   pod_materials50ed_fixture::detail::sha256(bytes)!=hashes[i])return fail("exact FireP0t PSG identity differs");
                a::OriginalStandardArchiveProvider50LA::MemberView mounted;
                a::OriginalStandardArchiveProvider50LA::MountView mount;
                if(provider->lookup132820(member,mounted)!=a::OriginalStandardArchiveProvider50LA::Lookup::present||
                   !provider->mount(mounted.mount,mount)||!mount.archiveSha256||*mount.archiveSha256!=ra_cold_sources50LA::archiveSha||
                   !mounted.data||mounted.size!=bytes.size()||std::memcmp(mounted.data,bytes.data(),bytes.size()))
                    return fail("same Application mounted RA PSG bytes required");
                auto payload=std::make_shared<a::OriginalStandardModelResource50LA>();
                if(!payload->prepare(member,bytes,d.resource))return fail("actual PSG payload decoder rejected");
                const auto* scene=payload->scene();
                if(payload->nodeCount()!=1||!scene||scene->materials.size()!=1||scene->materials[0]!="gargoylebody"||
                   scene->lods.size()!=1||scene->lods[0].surfaces.size()!=1||scene->identity_matrix_palette)
                    return fail("actual FireP0t payload structure differs");
                entries_[i]=std::make_unique<Entry>();entries_[i]->worldInstance=placement;
                entries_[i]->payload=std::move(payload);
                if(!entries_[i]->materials.acquire26E640(*entries_[i]->payload,domain_->materials382B38(),actualPath,d.material))
                    return fail("actual DefaultMT material-reference stage rejected");
                ++d.retainedPrefixes;
            }
            completed_=true;return true;
        }catch(...){return fail("RA model facet allocation failure");}
    }
    const Entry* sourceFacet(std::size_t i)const noexcept {
        return completed_&&!closed_&&i<entries_.size()?entries_[i].get():nullptr;
    }
    const a::WorldInstance48F* placement(std::size_t i)const noexcept {
        const auto* e=sourceFacet(i);const auto* s=source_?source_->sourcesDecoded():nullptr;
        return e&&s&&e->worldInstance<s->world.instances.size()?&s->world.instances[e->worldInstance]:nullptr;
    }
    bool close()noexcept {
        if(closed_)return true;
        for(const auto& e:entries_)if(e&&!e->materials.canRelease26E4C0())return false;
        for(auto& e:entries_){if(e){a::OriginalStandardMaterialDiagnostic50LA d;
            if(!e->materials.release26E4C0(d))return false;e.reset();}}
        domain_.reset();source_.reset();closed_=true;return true;
    }
};
}
