#pragma once
// Immutable native input custody for the pending source2C1170 continuation.
// This does not execute Config/file-manager calls, construct Event/Sound owners,
// initialize speech, authenticate supplied VAG data, or grant source return.
#include "native_racer_rig50hw_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "../../src/assets/original_standard_archive_provider50la.h"
#include "../../src/assets/original_stream_metadata50kz.h"
#include "../../src/assets/original_ta_cut_authoring_candidate.h"
#include <array>
#include <cstring>
#include <memory>
#include <new>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace live_audio50HF {class Owner;}
namespace native_player_speech50LA {
class Authoring final {
    friend class ::live_audio50HF::Owner;
public:
    using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
    using Metadata=rr::assets::OriginalStreamMetadata50KZ;
    using Rig=native_racer50HW::Rig;
    enum class Status : std::uint8_t {
        input,lookup,mount,member,hash,changed,layout,metadata,allocation,captured
    };
    struct Diagnostic {
        Status status=Status::input;
        std::size_t member=static_cast<std::size_t>(-1),offset=0;
        unsigned eventId=0;
    };
    struct Field {
        bool present=false;
        std::size_t offset=0;
        std::string_view text;
    };
    struct Event {
        unsigned id=0;
        bool sectionPresent=false;
        // Absent authoring is retained as absent. Native catalog defaults are
        // checked separately; they are not source Config default receipts.
        Field filename,isStream,isStereo,isLoop;
        const rr::assets::OriginalStreamMetadataRow50KZ* metadata=nullptr;
    };
    struct Category {
        Field declaredCount;
        unsigned count=0;
        std::array<Event,24> events{};
    };
    Authoring(const Authoring&)=delete;Authoring& operator=(const Authoring&)=delete;
    Authoring(Authoring&&)=delete;Authoring& operator=(Authoring&&)=delete;
    ~Authoring()=default;
    const Category* category(unsigned n)const noexcept{return n<3?&categories_[n]:nullptr;}
    const std::array<Field,9>& countFields()const noexcept{return countFields_;}
    const std::array<std::uint8_t,9>& counts()const noexcept{return counts_;}
    const Rig* rigIdentity()const noexcept{return rig_.get();}
    std::uint32_t racer60()const noexcept{return identity_->racer60;}
    const std::shared_ptr<const Provider>& providerOwner()const noexcept{return provider_;}
    const std::shared_ptr<const Metadata>& metadataOwner()const noexcept{return metadata_;}
    std::uint64_t capturedRevision()const noexcept{return revision_;}
    // Recheck only when a later source input consumer needs current namespace
    // custody. Teardown uses the retained immutable copies, not this live gate.
    bool mountedCurrent()const noexcept {
        if(thread_!=std::this_thread::get_id()||!provider_||!provider_->sealed()||
           provider_->revision()!=revision_)return false;
        for(const auto& member:members_){Provider::MemberView now;
            if(provider_->lookup132820(member.name,now)!=Provider::Lookup::present||
               now.member!=member.view.member||now.data!=member.view.data||now.size!=member.view.size||
               now.mount!=member.view.mount||now.ordinal!=member.view.ordinal||
               now.bankGeneration!=member.view.bankGeneration||now.revision!=revision_)return false;
        }
        return true;
    }
private:
    struct Member {
        std::string name;
        Provider::MemberView view;
        std::vector<std::uint8_t> bytes;
    };
    struct Specification {
        std::uint32_t racer60;
        std::size_t memberCount,firstOrdinal;
        std::array<std::size_t,4> sizes;
    };
    Authoring()=default;
    std::shared_ptr<const Provider> provider_;
    // Aliases the real Operation Resources control block, retaining the SAME
    // selected Rig. No borrowed raw Rig or second archive decode is accepted.
    std::shared_ptr<const Rig> rig_;
    std::shared_ptr<const Metadata> metadata_;
    const original_player_identity50IK::Identity* identity_=nullptr;
    std::array<Member,4> members_;
    std::array<Field,9> countFields_;
    std::array<std::uint8_t,9> counts_{};
    std::array<Category,3> categories_;
    std::uint64_t revision_=0;
    std::thread::id thread_;
    inline static constexpr const char* countKeys_[9]={
        "NumberOfLowDamageSounds","NumberOfWallHitSounds","NumberOfPassedSounds",
        "NumberOfAttackedSounds","NumberOfPassingSounds","NumberOfAttackingSounds",
        "NumberOfDeathSounds","NumberOfWinSounds","NumberOfLoseSounds"};
    inline static constexpr std::array<Specification,23> specifications_{{
        {1,96,91,{291,1330,844,655}}, {2,112,107,{291,810,584,655}},
        {3,89,84,{291,810,584,655}}, {4,89,84,{291,810,584,655}},
        {7,89,84,{291,810,584,655}}, {8,96,91,{291,810,584,655}},
        {9,93,88,{291,810,584,655}}, {10,92,87,{291,810,584,655}},
        {11,89,84,{291,810,584,655}}, {12,99,94,{291,810,584,655}},
        {14,95,90,{291,810,584,655}}, {15,90,85,{291,810,584,655}},
        {16,96,91,{291,810,584,655}}, {17,99,94,{291,810,584,655}},
        {18,97,92,{291,810,584,655}}, {19,96,91,{291,1655,844,786}},
        {20,94,89,{291,810,584,655}}, {21,96,91,{291,1005,586,655}},
        {22,103,98,{291,810,584,655}}, {23,94,89,{291,810,584,655}},
        {24,91,86,{291,810,584,655}}, {25,97,92,{291,1005,648,786}},
        {26,89,83,{291,941,584,463}}
    }};
    static bool number(std::string_view text,unsigned& out)noexcept {
        if(text.empty())return false;out=0;
        for(const auto c:text){if(c<'0'||c>'9'||out>255u)return false;out=out*10u+unsigned(c-'0');}
        return out<=255u;
    }
    static std::string_view trim(std::string_view text)noexcept {
        while(!text.empty()&&(text.front()==' '||text.front()=='\t'))text.remove_prefix(1);
        while(!text.empty()&&(text.back()==' '||text.back()=='\t'))text.remove_suffix(1);
        return text;
    }
    // A bounded inventory of exact hash-pinned INI fields, not a replacement
    // for original1577D0/158xxx Config execution or its missing-key branches.
    bool inventory(std::size_t member,Diagnostic& d)noexcept {
        const auto& bytes=members_[member].bytes;
        const std::string_view text(reinterpret_cast<const char*>(bytes.data()),bytes.size());
        std::string_view section;unsigned event=0;bool headerSeen=false;
        for(std::size_t offset=0;offset<text.size();){
            d.offset=offset;const auto end=text.find("\r\n",offset);
            if(end==std::string_view::npos)return false;
            auto line=trim(text.substr(offset,end-offset));offset=end+2;
            if(line.empty()||line.front()==';')continue;
            if(line.front()=='['){
                if(line.back()!=']')return false;section=line.substr(1,line.size()-2);event=0;
                if(!member){if(section!="Indices"||headerSeen)return false;headerSeen=true;}
                else if(section=="ResponseEmitter"){if(headerSeen)return false;headerSeen=true;}
                else {
                    if(!headerSeen||section.size()<=5||section.substr(0,5)!="Event"||
                       !number(section.substr(5),event)||!event||event>categories_[member-1].count)return false;
                    auto& row=categories_[member-1].events[event-1];
                    if(row.sectionPresent)return false;row.sectionPresent=true;
                }
                d.eventId=event;continue;
            }
            const auto equal=line.find('=');if(equal==std::string_view::npos)return false;
            const auto key=trim(line.substr(0,equal)),value=trim(line.substr(equal+1));
            Field* field=nullptr;
            if(!member){
                if(section!="Indices")return false;
                for(std::size_t i=0;i<9;++i)if(key==countKeys_[i])field=&countFields_[i];
            }else if(section=="ResponseEmitter"&&key=="NumEvents")field=&categories_[member-1].declaredCount;
            else if(event){auto& row=categories_[member-1].events[event-1];
                if(key=="FileName")field=&row.filename;else if(key=="IsStream")field=&row.isStream;
                else if(key=="IsStereo")field=&row.isStereo;else if(key=="IsLoop")field=&row.isLoop;
            }
            if(!field||field->present||value.empty())return false;
            *field={true,std::size_t(value.data()-text.data()),value};
            if(member&&field==&categories_[member-1].declaredCount){
                auto& category=categories_[member-1];
                if(!number(value,category.count)||!category.count||category.count>category.events.size())return false;
                for(unsigned i=0;i<category.count;++i)category.events[i].id=i+1;
            }
        }
        if(!headerSeen)return false;
        if(!member){
            const auto expected=original_player_identity50IK::speechCounts(*identity_);
            for(std::size_t i=0;i<9;++i){unsigned count=0;
                if(!countFields_[i].present||!number(countFields_[i].text,count)||count!=expected[i])return false;
                counts_[i]=static_cast<std::uint8_t>(count);
            }
        }else {
            auto& category=categories_[member-1];if(!category.declaredCount.present)return false;
            const unsigned sum=member==1?unsigned(counts_[0])+counts_[1]+counts_[2]+counts_[3]:
                member==2?unsigned(counts_[4])+counts_[5]:unsigned(counts_[6])+counts_[7]+counts_[8];
            const bool sparse=identity_->racer60==26u&&member==1u;
            if(category.count!=sum+(sparse?1u:0u))return false;
            for(unsigned i=0;i<category.count;++i){auto& row=category.events[i];d.eventId=i+1;
                if(sparse&&i==1u){if(row.sectionPresent||row.filename.present||row.isStream.present||
                    row.isStereo.present||row.isLoop.present)return false;continue;}
                if(!row.sectionPresent||!row.filename.present||!row.isStream.present||
                   row.isStream.text!="1"||row.isStereo.present||row.isLoop.present)return false;
                row.metadata=metadata_->findForStreamRequest(true,row.filename.text);
                if(!row.metadata){d.status=Status::metadata;return false;}
            }
        }
        return true;
    }
    static std::unique_ptr<const Authoring> capture(std::shared_ptr<const Provider> provider,
        std::shared_ptr<const Rig> rig,std::shared_ptr<const Metadata> metadata,Diagnostic& d)noexcept {
        d={};
        if(!provider||!provider->sealed()||!provider->revision()||!rig||!rig->directOriginal||
           !metadata||!metadata->prepared()||metadata->size()!=rr::assets::originalStreamMetadataRows50KZ)return {};
        try {
            const auto* identity=original_player_identity50IK::find(rig->podId);
            if(!identity)return {};
            const Specification* specification=nullptr;
            for(const auto& row:specifications_)if(row.racer60==identity->racer60)specification=&row;
            if(!specification||rig->originalArchive.entries.size()!=specification->memberCount||
               rig->manifest.at("source_container_sha256").get<std::string>()!=identity->archiveSha256)return {};
            auto next=std::unique_ptr<Authoring>(new Authoring);
            next->provider_=std::move(provider);next->rig_=std::move(rig);next->metadata_=std::move(metadata);
            next->identity_=identity;next->revision_=next->provider_->revision();next->thread_=std::this_thread::get_id();
            const std::string prefix="audio\\speech\\"+native_racer50HW::rig_detail::lower(identity->podId)+"\\";
            constexpr const char* suffixes[]={"sounds.ini","curses.ini","taunts.ini","special.ini"};
            const char* hashes[]={original_player_identity50IK::speechCountsSha256(*identity),
                identity->cursesSha256,identity->tauntsSha256,identity->specialSha256};
            for(std::size_t i=0;i<4;++i){d.member=i;auto& member=next->members_[i];
                member.name=prefix+suffixes[i];auto& view=member.view;
                if(next->provider_->lookup132820(member.name,view)!=Provider::Lookup::present){d.status=Status::lookup;return {};}
                Provider::MountView mount;
                if(!next->provider_->mount(view.mount,mount)||!mount.archiveName||!mount.archiveSha256||
                   *mount.archiveName!=next->rig_->resourceFile.generic_string()||
                   *mount.archiveSha256!=identity->archiveSha256||mount.archiveBytes!=identity->archiveBytes||
                   mount.memberCount!=specification->memberCount){d.status=Status::mount;return {};}
                const auto ordinal=specification->firstOrdinal+i;
                const auto& source=next->rig_->originalArchive.entries[ordinal];
                const auto& decoded=next->rig_->originalArchive.decoded;
                if(!view.member||!view.data||*view.member!=member.name||source.name!=member.name||
                   view.ordinal!=ordinal||view.size!=specification->sizes[i]||source.bytes!=view.size||
                   !view.bankGeneration||view.revision!=next->revision_||
                   source.logical_offset>decoded.size()||source.bytes>decoded.size()-source.logical_offset||
                   std::memcmp(view.data,decoded.data()+source.logical_offset,view.size)||
                   (i&&(view.mount!=next->members_[0].view.mount||view.bankGeneration!=next->members_[0].view.bankGeneration))){
                    d.status=Status::member;return {};
                }
                member.bytes.assign(view.data,view.data+view.size);
                if(!rr::assets::ta_cut_authoring_candidate_detail::hashMatches(member.bytes.data(),member.bytes.size(),hashes[i])){
                    d.status=Status::hash;return {};
                }
            }
            if(!next->mountedCurrent()){d.status=Status::changed;return {};}
            for(std::size_t i=0;i<4;++i){d.member=i;d.offset=0;d.eventId=0;d.status=Status::layout;
                if(!next->inventory(i,d))return {};
            }
            d={Status::captured,static_cast<std::size_t>(-1),0,0};
            return std::unique_ptr<const Authoring>(next.release());
        }catch(const std::bad_alloc&){d.status=Status::allocation;return {};}
        catch(...){d.status=Status::input;return {};}
    }
};
} // namespace native_player_speech50LA
