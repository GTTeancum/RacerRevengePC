#pragma once
// PRIVATE, UNACTIVATED CANDIDATE. Source-only; not compiled or executed.
// Owns exact original TA, RA, GC, SA, MA or GA intro INI bytes and pins the existing real provider
// and immutable APPSTART metadata. This is not an emitter, Sound, group member,
// PCM asset, playable stream, live resource request or readiness certificate.
#include "original_standard_archive_provider50la.h"
#include "original_stream_metadata50kz.h"
// Reuse only the existing fixed-stack SHA-256 helper. No cut owner is created.
#include "original_ta_cut_authoring_candidate.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rr::assets {
// Largest authenticated original intro NumEvents (TB introAud.ini: 5). The
// original keeps intro events in a growable ContainerList (292650/2941F0);
// this is the port's fixed host bound for every intro event array, checked
// against every catalogue row at compile time.
inline constexpr std::size_t originalRaceIntroEventsMax50LA=5;
class OriginalTaIntroAuthoringCandidate final {
public:
    using Provider=OriginalStandardArchiveProvider50LA;
    using Metadata=OriginalStreamMetadata50KZ;
    enum class Status : std::uint8_t {
        complete,input,providerLookup,mountIdentity,memberIdentity,sourceChanged,
        memberHash,layout,metadataIdentity,allocation
    };
    struct Diagnostic {
        Status status=Status::input;
        unsigned eventId=0;
        // 0=audio\\introAud.ini, 1=audio\\scrptLns.ini; sentinel otherwise.
        std::size_t member=static_cast<std::size_t>(-1);
    };
    struct Bytes {const std::uint8_t* data=nullptr;std::size_t size=0;};
    struct Event {
        std::uint8_t eventId=0,cameraNumber=0,span=0;
        // Exact case/separators from our immutable original introAud.ini copy.
        // No caller-supplied filename and no slash/PCM-name transformation.
        std::string_view filename;
        bool isStream=false,isStereo=false,isLoop=false;
        std::uint32_t factoryFlags=0;
        const OriginalStreamMetadataRow50KZ* metadata=nullptr;
        std::size_t filenameByteOffset=0,cameraNumberByteOffset=0;
    };
    OriginalTaIntroAuthoringCandidate(const OriginalTaIntroAuthoringCandidate&)=delete;
    OriginalTaIntroAuthoringCandidate& operator=(const OriginalTaIntroAuthoringCandidate&)=delete;
    OriginalTaIntroAuthoringCandidate(OriginalTaIntroAuthoringCandidate&&)=delete;
    OriginalTaIntroAuthoringCandidate& operator=(OriginalTaIntroAuthoringCandidate&&)=delete;
    ~OriginalTaIntroAuthoringCandidate()=default;

    // Cold serial snapshot from the actual Application-owned provider. No raw
    // byte/member-view, filename, claimed digest or caller-ready overload exists.
    // The caller must retain genuine shared ownership and serialize this call
    // with provider mount/retirement work on its owning thread. Its existing
    // mountAuthenticatedRes contract authenticates the complete archive at the
    // real upstream caller; this factory independently hashes the COPIED members.
    // It does not recover full retail file-manager mount chronology or prove
    // that this provider is currently bound to a particular race controller.
    static std::unique_ptr<const OriginalTaIntroAuthoringCandidate> create(
        std::shared_ptr<const Provider> provider,std::shared_ptr<const Metadata> metadata,
        Diagnostic& diagnostic) noexcept {
        diagnostic={};
        if(!provider||!metadata||!provider->sealed()||!provider->revision()||
           !metadata->prepared()||metadata->size()!=originalStreamMetadataRows50KZ)
            return nullptr;
        try {
            const auto revision=provider->revision();
            std::array<Provider::MemberView,2> views{};
            const Specification* spec=nullptr;
            std::unique_ptr<OwnedSpecification> owned;
            for(std::size_t i=0;i<views.size();++i){
                diagnostic.member=i;
                if(provider->lookup132820(memberNames_[i],views[i])!=Provider::Lookup::present){
                    diagnostic.status=Status::providerLookup;return nullptr;
                }
                const auto& v=views[i];Provider::MountView mount;
                if(!provider->mount(v.mount,mount)||!mount.archiveName||!mount.archiveSha256){
                    diagnostic.status=Status::mountIdentity;return nullptr;
                }
                if(!i){for(const auto& candidate:specifications_)if(*mount.archiveName==candidate.archiveName)spec=&candidate;
                    if(spec)spec=admittedSpecification(*spec);
                    // The common-route course whose code is not stock (146r):
                    // its row from its own archive (the registry cut slot).
                    else if((owned=registrySpecification(*mount.archiveName)))spec=&owned->spec;}
                if(!spec||*mount.archiveName!=spec->archiveName||*mount.archiveSha256!=spec->archiveDigest||
                   mount.archiveBytes!=spec->archiveBytes||mount.memberCount!=spec->memberCount){
                    diagnostic.status=Status::mountIdentity;return nullptr;
                }
                if(!v.member||!v.data||*v.member!=memberNames_[i]||v.size!=spec->memberSizes[i]||
                   v.ordinal!=spec->memberOrdinals[i]||!v.bankGeneration||v.revision!=revision){
                    diagnostic.status=Status::memberIdentity;return nullptr;
                }
            }
            if(views[0].mount!=views[1].mount||
               views[0].bankGeneration!=views[1].bankGeneration){
                diagnostic.status=Status::mountIdentity;return nullptr;
            }
            auto next=std::unique_ptr<OriginalTaIntroAuthoringCandidate>(new OriginalTaIntroAuthoringCandidate);
            next->provider_=std::move(provider);next->metadata_=std::move(metadata);
            next->capturedMount_=views[0].mount;next->capturedGeneration_=views[0].bankGeneration;
            next->capturedRevision_=revision;next->specification_=spec;
            next->ownedSpecification_=std::move(owned); // heap row: spec stays valid
            for(std::size_t i=0;i<views.size();++i){
                diagnostic.member=i;
                next->bytes_[i].assign(views[i].data,views[i].data+views[i].size);
                const auto& b=next->bytes_[i];
                if(!ta_cut_authoring_candidate_detail::hashMatches(b.data(),b.size(),spec->memberDigests[i])){
                    diagnostic.status=Status::memberHash;return nullptr;
                }
            }
            // Verify the same lookup winner/generation after copying. This is
            // a serial phase check, not a replacement for C++ thread safety.
            if(!next->provider_->sealed()||next->provider_->revision()!=revision){
                diagnostic.status=Status::sourceChanged;return nullptr;
            }
            for(std::size_t i=0;i<views.size();++i){
                Provider::MemberView now;
                if(next->provider_->lookup132820(memberNames_[i],now)!=Provider::Lookup::present||
                   now.member!=views[i].member||now.data!=views[i].data||now.size!=views[i].size||
                   now.mount!=views[i].mount||now.ordinal!=views[i].ordinal||
                   now.bankGeneration!=views[i].bankGeneration||now.revision!=revision){
                    diagnostic.member=i;diagnostic.status=Status::sourceChanged;return nullptr;
                }
            }
            // The course's own intro data, read by key (292650/2941F0 read
            // [ResponseEmitter] NumEvents and [EventN]; 158B30 reads [LineN]
            // CameraNumber). The stock row's byte offsets, camera values and
            // metadata indices are RECEIPTS (one line when the data differs).
            const auto& ini=next->bytes_[0];const auto& script=next->bytes_[1];
            std::uint32_t numEvents=0,numLines=0;
            diagnostic.member=0;
            // NumEvents 0 is the data's own answer (an add-on course with no
            // announcer lines): no intro events; the cut cameras still play.
            if(!iniNumber(ini,"ResponseEmitter","NumEvents",numEvents)||
               numEvents>originalRaceIntroEventsMax50LA){
                diagnostic.status=Status::layout;return nullptr;
            }
            // NumberOfLines absent = one line per event (bounded by the same store).
            if(!iniNumber(script,"General","NumberOfLines",numLines))numLines=numEvents;
            bool stockShape=numEvents==spec->eventCount;
            for(std::size_t i=0;i<numEvents;++i){
                diagnostic.eventId=static_cast<unsigned>(i+1);diagnostic.member=0;
                const std::string eventName="Event"+std::to_string(i+1),lineName="Line"+std::to_string(i+1);
                std::size_t f=0,length=0,camera=0,cameraLength=0;
                std::string_view stream,stereo,loop;
                if(!iniValue(ini,eventName,"FileName",f,length)||!length||
                   !iniView(ini,eventName,"IsStream",stream)||!iniView(ini,eventName,"IsStereo",stereo)||
                   !iniView(ini,eventName,"IsLoop",loop)){
                    diagnostic.status=Status::layout;return nullptr;
                }
                auto& e=next->events_[i];
                e.eventId=static_cast<std::uint8_t>(i+1);
                // A line without CameraNumber (or beyond NumberOfLines) names no
                // cut: camera 0, which every consumer skips (1779CC).
                e.cameraNumber=0;
                diagnostic.member=1;
                if(i<numLines&&iniValue(script,lineName,"CameraNumber",camera,cameraLength)&&cameraLength){
                    //158B30->158920->30AE70(base0) stops on the first comma:
                    //RA's "1,2,3,4" / "5,6,7" become1/5, not a list.
                    unsigned value=0;std::size_t k=camera;
                    for(;k<camera+cameraLength&&script[k]>='0'&&script[k]<='9';++k){
                        value=value*10u+unsigned(script[k]-'0');
                        if(value>255u){diagnostic.status=Status::layout;return nullptr;}
                    }
                    if(k==camera){diagnostic.status=Status::layout;return nullptr;}
                    e.cameraNumber=static_cast<std::uint8_t>(value);
                }
                diagnostic.member=0;
                // Span is ABSENT in every authenticated Line section; 177750's
                // source default is0. No source bytes are invented.
                e.span=0;
                e.filename={reinterpret_cast<const char*>(ini.data()+f),length};
                e.isStream=stream=="1";e.isStereo=stereo=="1";e.isLoop=loop=="1";
                // 292764..29279C: stream?2:1, OR stereo0x40 and loop0x80.
                e.factoryFlags=(e.isStream?2u:1u)|(e.isStereo?0x40u:0u)|(e.isLoop?0x80u:0u);
                e.filenameByteOffset=f;e.cameraNumberByteOffset=camera;
                e.metadata=next->metadata_->findForStreamRequest(e.isStream,e.filename);
                if(!e.metadata){diagnostic.status=Status::metadataIdentity;return nullptr;}
                if(i>=spec->eventCount||f!=spec->filenameOffsets[i]||camera!=spec->cameraOffsets[i]||
                   std::string_view(reinterpret_cast<const char*>(script.data()+camera),cameraLength)!=spec->cameraValues[i]||
                   e.metadata->sourceIndex!=spec->metadataIndices[i])stockShape=false;
            }
            next->eventCount_=static_cast<unsigned>(numEvents);
            if(next->ownedSpecification_){
                // Registry course: one line, no stock row to compare against.
                std::fprintf(stderr,"course_intro_rows track=%.*s events=%u lines=%u stock_row=0 cameras=",
                    int(spec->track.size()),spec->track.data(),unsigned(numEvents),unsigned(numLines));
                for(std::size_t i=0;i<numEvents;++i)std::fprintf(stderr,"%s%u",i?",":"",unsigned(next->events_[i].cameraNumber));
                std::fprintf(stderr,"\n");
            }else if(!stockShape){
                std::fprintf(stderr,"course_intro_rows track=%.*s events=%u stock_events=%u cameras=",
                    int(spec->track.size()),spec->track.data(),unsigned(numEvents),spec->eventCount);
                for(std::size_t i=0;i<numEvents;++i)std::fprintf(stderr,"%s%u",i?",":"",unsigned(next->events_[i].cameraNumber));
                std::fprintf(stderr,"\n");
            }
            diagnostic={Status::complete,0,static_cast<std::size_t>(-1)};
            return std::unique_ptr<const OriginalTaIntroAuthoringCandidate>(next.release());
        }catch(...){diagnostic.status=Status::allocation;return nullptr;}
    }
    // Published rows/filename views remain stable until this owner is destroyed;
    // metadata rows also have the independent retained immutable metadata pin.
    const Event* event(unsigned eventId) const noexcept {
        return eventId>=1&&eventId<=eventCount()?&events_[eventId-1]:nullptr;
    }
    unsigned eventCount()const noexcept{return specification_?eventCount_:0;}
    std::string_view trackId()const noexcept{return specification_?specification_->track:std::string_view{};}
    const std::shared_ptr<const Metadata>& metadataOwner() const noexcept {return metadata_;}
    const std::shared_ptr<const Provider>& providerOwner() const noexcept {return provider_;}
    const Provider* providerIdentity() const noexcept {return provider_.get();}
    const OriginalTaIntroAuthoringCandidate* ownerIdentity() const noexcept {return this;}
    Bytes originalIntroBytes() const noexcept {return {bytes_[0].data(),bytes_[0].size()};}
    Bytes originalScriptBytes() const noexcept {return {bytes_[1].data(),bytes_[1].size()};}
    // Historical copied-source provenance ONLY. Provider retention does not
    // prevent logical unmount/reentry and these values are not a live file ID.
    std::size_t capturedMount() const noexcept {return capturedMount_;}
    std::uint64_t capturedBankGeneration() const noexcept {return capturedGeneration_;}
    std::uint64_t capturedProviderRevision() const noexcept {return capturedRevision_;}
private:
    OriginalTaIntroAuthoringCandidate()=default;
    std::shared_ptr<const Provider> provider_;
    std::shared_ptr<const Metadata> metadata_;
    std::array<std::vector<std::uint8_t>,2> bytes_;
    std::array<Event,originalRaceIntroEventsMax50LA> events_{};
    std::size_t capturedMount_=static_cast<std::size_t>(-1);
    std::uint64_t capturedGeneration_=0,capturedRevision_=0;
    struct Specification {
        std::string_view track,archiveName,archiveDigest;
        std::size_t archiveBytes,memberCount;unsigned eventCount;std::size_t filenameLength;
        std::array<std::size_t,2> memberSizes,memberOrdinals;
        std::array<const char*,2> memberDigests;
        std::array<std::size_t,originalRaceIntroEventsMax50LA> filenameOffsets,streamOffsets,stereoOffsets,loopOffsets,cameraOffsets,metadataIndices;
        std::array<std::string_view,originalRaceIntroEventsMax50LA> cameraValues;
    };
    // Registry course row (146r, PORT EXTENSION): owned copies of the slot's
    // strings so this owner never borrows a slot a later course may replace.
    struct OwnedSpecification {
        std::string track,archiveName,archiveDigest;
        std::array<std::string,2> digests;
        Specification spec{};
    };
    const Specification* specification_=nullptr;
    std::unique_ptr<OwnedSpecification> ownedSpecification_;
    unsigned eventCount_=0;
    // Minimal INI lookup over the copied member bytes: "[Section]" then
    // "Key = value" lines (case-insensitive names, value trimmed, ends at CR/LF
    // or ';'). Returns the value's byte offset/length inside the member.
    static bool iequal(std::string_view a,std::string_view b)noexcept{
        if(a.size()!=b.size())return false;
        for(std::size_t i=0;i<a.size();++i){
            char x=a[i],y=b[i];if(x>='A'&&x<='Z')x=char(x+32);if(y>='A'&&y<='Z')y=char(y+32);
            if(x!=y)return false;
        }
        return true;
    }
    static std::string_view trim(std::string_view v)noexcept{
        while(!v.empty()&&(v.front()==' '||v.front()=='\t'))v.remove_prefix(1);
        while(!v.empty()&&(v.back()==' '||v.back()=='\t'))v.remove_suffix(1);
        return v;
    }
    static bool iniValue(const std::vector<std::uint8_t>& bytes,std::string_view section,std::string_view key,
        std::size_t& offset,std::size_t& length)noexcept{
        const std::string_view text(reinterpret_cast<const char*>(bytes.data()),bytes.size());
        bool inSection=false;std::size_t pos=0;
        while(pos<text.size()){
            std::size_t end=text.find_first_of("\r\n",pos);if(end==std::string_view::npos)end=text.size();
            const auto row=trim(text.substr(pos,end-pos));
            if(!row.empty()&&row.front()=='['){
                const auto close=row.find(']');
                inSection=close!=std::string_view::npos&&iequal(trim(row.substr(1,close-1)),section);
            }else if(inSection){
                const auto eq=row.find('=');
                if(eq!=std::string_view::npos&&iequal(trim(row.substr(0,eq)),key)){
                    auto value=row.substr(eq+1);const auto semi=value.find(';');
                    if(semi!=std::string_view::npos)value=value.substr(0,semi);
                    value=trim(value);
                    offset=static_cast<std::size_t>(value.data()-text.data());length=value.size();return true;
                }
            }
            pos=end;while(pos<text.size()&&(text[pos]=='\r'||text[pos]=='\n'))++pos;
        }
        return false;
    }
    static bool iniView(const std::vector<std::uint8_t>& bytes,std::string_view section,std::string_view key,
        std::string_view& out)noexcept{
        std::size_t o=0,l=0;if(!iniValue(bytes,section,key,o,l))return false;
        out={reinterpret_cast<const char*>(bytes.data()+o),l};return true;
    }
    static bool iniNumber(const std::vector<std::uint8_t>& bytes,std::string_view section,std::string_view key,
        std::uint32_t& out)noexcept{
        std::string_view v;if(!iniView(bytes,section,key,v)||v.empty())return false;
        std::uint32_t n=0;
        for(char c:v){if(c<'0'||c>'9'||n>100000u)return false;n=n*10u+std::uint32_t(c-'0');}
        out=n;return true;
    }
    // Common course rule: a non-stock course archive admitted at course select
    // (its cut row derived from its own data, original_track_cut_identity50la.h)
    // gets its mount label, member count and intro member sizes/positions/
    // digests from that row. FLAG stand-in: event byte offsets stay the stock
    // row's; the layout check below still verifies them against the bytes.
    static const Specification* admittedSpecification(const Specification& stock)noexcept{
        const auto* slot=originalTrackCutRuntimeSlot50LA(stock.track);
        if(!slot||!slot->present)return &stock;
        static std::array<Specification,13> rows{};
        std::size_t index=0;
        for(;index<specifications_.size();++index)if(&specifications_[index]==&stock)break;
        if(index>=rows.size())return &stock;
        auto& row=rows[index];row=stock;
        const auto& id=slot->identity;
        row.archiveDigest=slot->archiveSha;row.archiveBytes=id.archiveBytes;row.memberCount=id.memberCount;
        for(std::size_t i=0;i<2;++i){row.memberSizes[i]=id.introMembers[i].size;row.memberOrdinals[i]=id.introMembers[i].ordinal;
            row.memberDigests[i]=slot->introShas[i].c_str();}
        return &row;
    }
    // The common-route course whose code is not stock: its row is the registry
    // cut slot derived from its own archive (course_cut_identity50ci.h), matched
    // by the mounted archive name. No stock event rows: eventCount 0, so every
    // event the data names is logged (course_intro_rows ... stock_row=0).
    static std::unique_ptr<OwnedSpecification> registrySpecification(std::string_view archiveName){
        const auto& slot=originalTrackCutRuntimeRows50LA()[originalTrackCutRegistrySlot50LA];
        if(!slot.present||!slot.code[0]||slot.archiveName.empty()||slot.archiveName!=archiveName)return {};
        auto owned=std::make_unique<OwnedSpecification>();
        owned->track.assign(slot.code.data(),2);owned->archiveName=slot.archiveName;owned->archiveDigest=slot.archiveSha;
        auto& s=owned->spec;
        s.track=owned->track;s.archiveName=owned->archiveName;s.archiveDigest=owned->archiveDigest;
        s.archiveBytes=slot.identity.archiveBytes;s.memberCount=slot.identity.memberCount;s.eventCount=0;s.filenameLength=0;
        for(std::size_t i=0;i<2;++i){
            owned->digests[i]=slot.introShas[i];
            s.memberSizes[i]=slot.identity.introMembers[i].size;s.memberOrdinals[i]=slot.identity.introMembers[i].ordinal;
            s.memberDigests[i]=owned->digests[i].c_str();
        }
        s.filenameOffsets={};s.streamOffsets={};s.stereoOffsets={};s.loopOffsets={};s.cameraOffsets={};s.metadataIndices={};
        s.cameraValues={};
        return owned;
    }
    inline static constexpr std::array<Specification,13> specifications_{{
        {"TA","TRACKS/TA.RES","b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8",
         3334562,383,3,39,{346,119},{381,380},
         {"22e1afe07f5f34033b63a75fa8b5c2a30f49869f57cfa96926ba5c51eba9bf50","1a325c68bbccaa8dfa94f87a8d1dcb743d029c59d96771797ce7045135cf6a0f"},
         {57,161,265},{109,213,317},{123,227,331},{135,239,343},{56,85,114},{6,16,7},{"1","2","3"}},
        {"RA","TRACKS/RA.RES","1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2",
         4604561,1010,2,37,{238,98},{1008,1007},
         {"72aa1f6ad04da68e6c7f345a05471e0d7b6cfcc8166dc18e9ed2fe8de6df76ce","f7234be2254765e73546711e11d44c7b02828b17c023ef0aeacb75b9cd316bf7"},
         {57,159,0},{107,209,0},{121,223,0},{133,235,0},{56,91,0},{26,27,0},{"1,2,3,4","5,6,7",""}},
        {"GC","TRACKS/GC.RES","69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9",
         4205067,1190,3,37,{340,117},{1186,1185},
         {"9270868317ef863dc0d5044346e712049b0edf4688e634ddb46b349b04c54826","a59dc183ec92b00a2cd0e7940f66552207074b10ceba9ffb61461f7eeed529c8"},
         {57,159,261},{107,209,311},{121,223,325},{133,235,337},{56,85,114},{43,44,45},{"1","2","3"}},
        {"SA","TRACKS/SA.RES","34826bbb3a3690ee20ac40f70515f7e903291cdf4546f7d8ab8d58849f30d96a",
         3786696,1065,2,38,{240,88},{1063,1062},
         {"9f9d20f76a7b729a0cc471a2b8e4cd4c269a75803be1256be127644b471df15a","d6aad56024e0ac3bfaf0bc3c138ab089cc0953b52bf395b78c58d7b70984b1cb"},
         {57,160,0},{108,211,0},{122,225,0},{134,237,0},{56,85,0},{25,24,0},{"1","2",""}},
        {"MA","TRACKS/MA.RES","2a674f62e9d9d9ef9e6c2f7224d2aa0c55aeb883e55c499d58dfd3b00cddb228",
         3691592,485,3,37,{340,117},{483,482},
         {"0ff6ce708dad3507cfb5de7107e9f14d2fcc5cde1ef3a53b7ec1e84b3fbd302f","bacbdb0d6ac4bc793f0d459f5a88e73fb09c469976757eb26c21610a059bbd86"},
         {57,159,261},{107,209,311},{121,223,325},{133,235,337},{56,85,114},{33,34,35},{"1","2","4"}},
        {"GA","TRACKS/GA.RES","063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9",
         2834942,970,2,37,{238,88},{968,967},
         {"3b99e6b992b8ebe102a4fd89841be92d80793d348aa683abb3fc3992eccf9785","c28784d774a7a1d65fe118d3e94d5c3c7aa8bff490ee81bfaeeb0e7adfc58d90"},
         {57,159,0},{107,209,0},{121,223,0},{133,235,0},{56,85,0},{40,41,0},{"1","3",""}},
        {"MB","TRACKS/MB.RES","6aae7d59981cae6c741b14e147021960fdb4d8e68e21c217a446f779d3c821ca",
         4421779,487,2,37,{238,88},{485,484},
         {"1ce76eba38a94afb4a9e0ecaf82b7416781fef75480683299f734747b46638e4","c28784d774a7a1d65fe118d3e94d5c3c7aa8bff490ee81bfaeeb0e7adfc58d90"},
         {57,159,0},{107,209,0},{121,223,0},{133,235,0},{56,85,0},{36,37,0},{"1","3",""}},
        {"SC","TRACKS/SC.RES","8305641f0ac8fefeae6603145ad4f8441612e41926751ff853170763e66dfa4d",
         2974353,984,3,38,{343,117},{982,981},
         {"a74d454fd671470aa9f4a507a026369973936999535f39606ce84595c3dc9d1f","a59dc183ec92b00a2cd0e7940f66552207074b10ceba9ffb61461f7eeed529c8"},
         {57,160,263},{108,211,314},{122,225,328},{134,237,340},{56,85,114},{21,22,23},{"1","2","3"}},
        {"GB","TRACKS/GB.RES","e92af24305431dc5a96cebb5ebcf89159adb6d371045b2c5424dacd1f74b7d99",
         4088586,1049,2,37,{238,88},{1047,1046},
         {"aabe2a0edae88ecd40b6b9a3475f53712c1ba1bb7910c0b0434da95baf23268a","c28784d774a7a1d65fe118d3e94d5c3c7aa8bff490ee81bfaeeb0e7adfc58d90"},
         {57,159,0},{107,209,0},{121,223,0},{133,235,0},{56,85,0},{42,46,0},{"1","3",""}},
        {"RC","TRACKS/RC.RES","ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b",
         4655141,1121,3,37,{340,121},{1119,1118},
         {"50a0837c37e985fb6fa55041d9198838b33b49f1325fae919209f9f38319f8fb","459d8eaf30df82d889f0c1c4fc13fcd157dc0050cc2ecb0424c5af66e5e93641"},
         {57,159,261},{107,209,311},{121,223,325},{133,235,337},{56,85,116},{30,31,32},{"1","2,3","4,5"}},
        {"TC","TRACKS/TC.RES","7b5486f4029dabe137919127c879faef92b8bd2fb9df5c0d1a1d122e92731490",
         3607868,509,3,39,{346,117},{507,506},
         {"45cd6a12b36aee70edefddd0b3b8f808878117ca35af237aa2438173d990ca13","88aa20cdf0a8352baab8edad438f825115098cf0f463f4ec1e3090d30cbedc73"},
         {57,161,265},{109,213,317},{123,227,331},{135,239,343},{56,85,114},{15,13,14},{"1","3","4"}},
        {"MC","TRACKS/MC.RES","f8854752f639676257d44e33479a0c31c047a17e4cd592d02f483ad593179c81",
         3822171,432,2,37,{238,88},{430,429},
         {"fe01f53fb25282782610823d5cfacfa1ccac754342beca2b9837dfb01d24d67d","c28784d774a7a1d65fe118d3e94d5c3c7aa8bff490ee81bfaeeb0e7adfc58d90"},
         {57,159,0},{107,209,0},{121,223,0},{133,235,0},{56,85,0},{38,39,0},{"1","3",""}},
        {"TB","TRACKS/TB.RES","1188206d7b1f64910c119192829fc357b9d4a9f021d64e8a2b7561f2c113a09b",
         3749557,1465,5,39,{556,175},{1463,1462},
         {"bd83e5412da7cbf14f979c17c0002c798f477848ed0af38883197dfdc2320423","5197d42995287097b6d1f5981db602c5e7e148891392566427ff4b14f1bd22fc"},
         {57,161,265,369,473},{109,213,317,421,525},{123,227,331,435,539},{135,239,343,447,551},{56,85,114,143,172},{8,9,10,11,12},{"1","2","3","4","5"}},
    }};
    inline static constexpr std::array<const char*,2> memberNames_{{"audio\\introAud.ini","audio\\scrptLns.ini"}};
};
}
