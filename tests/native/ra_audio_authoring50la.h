#pragma once
#include "ra_cold_sources50la.h"
#include "../../src/assets/material_audio50hi.h"
#include <array>
#include <memory>
#include <stdexcept>
#include <utility>

namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_audio_authoring50LA {
namespace a=rr::assets;
// Immutable source closure only.42 ambient rows remain distinct source records;
// no runtime Sound, audio group, stream, event cursor or manager is invented.
class Owner final {
    friend class ::ra_cold_aggregate50LA::Owner;
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::array<std::vector<std::uint8_t>,3> bytes_{};
    a::ParameterDocument48F aub_;
    a::MaterialAudioCatalog50HI materials_;
    std::vector<a::AudioRequest50HC> requests_;
    std::array<const a::ParameterBlock48F*,42> ambient_{};
    bool attempted_=false,decoded_=false;
    static void need(bool b,const char* why){if(!b)throw std::runtime_error(why);}
    static const a::ParameterBlock48F& block(const a::ParameterDocument48F& p,const std::string& name){
        const a::ParameterBlock48F* result=nullptr;
        for(const auto& b:p.blocks)if(b.name==name){need(!result,"duplicate RA AUB section");result=&b;}
        need(result,"missing RA AUB section");return *result;
    }
    static const std::string& field(const a::ParameterBlock48F& b,const char* name){
        const std::string* result=nullptr;
        for(const auto& f:b.fields)if(f.name==name){need(!result,"duplicate consumed RA AUB field");result=&f.value;}
        need(result,"missing RA AUB field");return *result;
    }
public:
    struct Diagnostic {const char* failure=nullptr;std::string detail;a::DecodeError48C decode{};};
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    bool decode(std::shared_ptr<const ra_cold_sources50LA::Owner> source,Diagnostic& d) noexcept {
        d={};if(attempted_){d.failure="RA audio closure already attempted";return false;}attempted_=true;
        try{
            const auto* s=source?source->sourcesDecoded():nullptr;
            need(s&&s->world.zones.size()==10,"actual authenticated RA source owner required");source_=std::move(source);
            struct Pin{const char* member;std::size_t bytes;const char* sha;};
            constexpr Pin pins[3]={
                {"tracks\\ra.aub",22511,"c0977667b46b918872c3d351f745ce59260037b9f604b76aea23b11bb5c88c6a"},
                {"audio\\scrptLns.ini",98,"f7234be2254765e73546711e11d44c7b02828b17c023ef0aeacb75b9cd316bf7"},
                {"audio\\introAud.ini",238,"72aa1f6ad04da68e6c7f345a05471e0d7b6cfcc8166dc18e9ed2fe8de6df76ce"}};
            for(std::size_t i=0;i<3;++i)need(a::extractResMember48C(s->archive,pins[i].member,bytes_[i],d.decode,pins[i].bytes)&&
                bytes_[i].size()==pins[i].bytes&&pod_materials50ed_fixture::detail::sha256(bytes_[i])==pins[i].sha,"RA exact audio source identity");
            a::WorldLimits48F limits{};limits.retainDuplicateLabels=true;
            need(a::decodeParameters48F(bytes_[0],aub_,d.decode,nullptr,limits),"RA AUB source syntax");
            need(field(block(aub_,"sounds"),"numsoultreesounds")=="42","RA original ambient count");
            for(std::size_t i=0;i<ambient_.size();++i)
                ambient_[i]=&block(aub_,"soultreesound"+std::to_string(i+1));
            // Canonical parser owns the declared iteration/default policy.
            // Do not scan material2 into an extra assignment or repair authoring.
            need(a::readMaterialAudioCatalog50HI(bytes_[0],materials_,requests_,d.detail)&&
                materials_.parameters.rowCount==12&&materials_.parameters.driveRowCount==12&&
                materials_.materialCount==1&&materials_.defaultMaterial==1&&
                materials_.orderedObjectMaterials.empty()&&requests_.size()==21,"RA material source traversal differs");
            for(std::size_t i=0;i<12;++i)need(materials_.collisionSectionKeys[i]==i+1&&
                materials_.driveSectionKeys[i]==i+1,"RA material source section identity differs");
            decoded_=true;return true;
        }catch(...){d.failure="RA audio source closure rejected; private prefix retained";return false;}
    }
    // Stable borrows; no mutation or re-decode after pointers become visible.
    const a::MaterialAudioCatalog50HI* materials()const noexcept{return decoded_?&materials_:nullptr;}
    const std::vector<a::AudioRequest50HC>* materialRequests()const noexcept{return decoded_?&requests_:nullptr;}
    const a::ParameterBlock48F* ambientSource(std::size_t i)const noexcept{return decoded_&&i<42?ambient_[i]:nullptr;}
    const std::vector<std::uint8_t>* originalMember(std::size_t i)const noexcept{return decoded_&&i<3?&bytes_[i]:nullptr;}
};
}
