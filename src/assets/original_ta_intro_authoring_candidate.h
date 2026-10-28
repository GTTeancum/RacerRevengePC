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
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rr::assets {
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
            for(std::size_t i=0;i<views.size();++i){
                diagnostic.member=i;
                if(provider->lookup132820(memberNames_[i],views[i])!=Provider::Lookup::present){
                    diagnostic.status=Status::providerLookup;return nullptr;
                }
                const auto& v=views[i];Provider::MountView mount;
                if(!provider->mount(v.mount,mount)||!mount.archiveName||!mount.archiveSha256){
                    diagnostic.status=Status::mountIdentity;return nullptr;
                }
                if(!i){for(const auto& candidate:specifications_)if(*mount.archiveName==candidate.archiveName)spec=&candidate;}
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
            // Fixed offsets are from these exact hash-pinned original members,
            // not a general permissive INI parser or embedded asset payload.
            const auto& ini=next->bytes_[0];const auto& script=next->bytes_[1];
            for(std::size_t i=0;i<spec->eventCount;++i){
                diagnostic.eventId=static_cast<unsigned>(i+1);diagnostic.member=0;
                const auto f=spec->filenameOffsets[i];const auto camera=spec->cameraOffsets[i];
                const auto length=spec->filenameLength;const auto cameraBytes=spec->cameraValues[i];
                if(f+length+2u>ini.size()||ini[f+length]!='\r'||ini[f+length+1]!='\n'||
                   ini[spec->streamOffsets[i]]!='1'||ini[spec->stereoOffsets[i]]!='0'||ini[spec->loopOffsets[i]]!='0'||
                   camera+cameraBytes.size()+2u>script.size()||
                   std::string_view(reinterpret_cast<const char*>(script.data()+camera),cameraBytes.size())!=cameraBytes||
                   script[camera+cameraBytes.size()]!='\r'||script[camera+cameraBytes.size()+1]!='\n'){
                    diagnostic.status=Status::layout;return nullptr;
                }
                auto& event=next->events_[i];
                event.eventId=static_cast<std::uint8_t>(i+1);
                event.cameraNumber=static_cast<std::uint8_t>(script[camera]-'0');
                //158B30->158920->30AE70(base0) stops on the first comma:
                //RA's exact "1,2,3,4" / "5,6,7" become1/5, not a list.
                //Span is ABSENT in every admitted authenticated Line section.
                // 177750's source default is0; no source bytes2..7 are invented.
                event.span=0;
                event.filename={reinterpret_cast<const char*>(ini.data()+f),length};
                event.isStream=ini[spec->streamOffsets[i]]=='1';
                event.isStereo=ini[spec->stereoOffsets[i]]=='1';
                event.isLoop=ini[spec->loopOffsets[i]]=='1';
                // 292764..29279C: stream?2:1, OR stereo0x40 and loop0x80.
                // Actual TA flags are2, not the superseded earlier0x41 decode.
                event.factoryFlags=(event.isStream?2u:1u)|(event.isStereo?0x40u:0u)|(event.isLoop?0x80u:0u);
                event.filenameByteOffset=f;event.cameraNumberByteOffset=camera;
                event.metadata=next->metadata_->findForStreamRequest(event.isStream,event.filename);
                if(!event.metadata||event.metadata!=next->metadata_->row(spec->metadataIndices[i])||
                   event.metadata->sourceIndex!=spec->metadataIndices[i]||
                   event.metadata->songNumber!=spec->metadataIndices[i]+1u){
                    diagnostic.status=Status::metadataIdentity;return nullptr;
                }
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
    unsigned eventCount()const noexcept{return specification_?specification_->eventCount:0;}
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
    std::array<Event,3> events_{};
    std::size_t capturedMount_=static_cast<std::size_t>(-1);
    std::uint64_t capturedGeneration_=0,capturedRevision_=0;
    struct Specification {
        std::string_view track,archiveName,archiveDigest;
        std::size_t archiveBytes,memberCount;unsigned eventCount;std::size_t filenameLength;
        std::array<std::size_t,2> memberSizes,memberOrdinals;
        std::array<const char*,2> memberDigests;
        std::array<std::size_t,3> filenameOffsets,streamOffsets,stereoOffsets,loopOffsets,cameraOffsets,metadataIndices;
        std::array<std::string_view,3> cameraValues;
    };
    const Specification* specification_=nullptr;
    inline static constexpr std::array<Specification,6> specifications_{{
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
         {57,159,0},{107,209,0},{121,223,0},{133,235,0},{56,85,0},{40,41,0},{"1","3",""}}
    }};
    inline static constexpr std::array<const char*,2> memberNames_{{"audio\\introAud.ini","audio\\scrptLns.ini"}};
};
}
