#pragma once
// PRIVATE, UNACTIVATED CANDIDATE. Source-only; not compiled or executed.
// Owns exact original TA intro INI bytes and pins the existing real provider
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
            for(std::size_t i=0;i<views.size();++i){
                diagnostic.member=i;
                if(provider->lookup132820(memberNames_[i],views[i])!=Provider::Lookup::present){
                    diagnostic.status=Status::providerLookup;return nullptr;
                }
                const auto& v=views[i];Provider::MountView mount;
                if(!v.member||!v.data||*v.member!=memberNames_[i]||v.size!=memberSizes_[i]||
                   v.ordinal!=memberOrdinals_[i]||!v.bankGeneration||v.revision!=revision){
                    diagnostic.status=Status::memberIdentity;return nullptr;
                }
                if(!provider->mount(v.mount,mount)||!mount.archiveName||!mount.archiveSha256||
                   *mount.archiveName!="TRACKS/TA.RES"||*mount.archiveSha256!=archiveDigest_||
                   mount.archiveBytes!=3334562u||mount.memberCount!=383u){
                    diagnostic.status=Status::mountIdentity;return nullptr;
                }
            }
            if(views[0].mount!=views[1].mount||
               views[0].bankGeneration!=views[1].bankGeneration){
                diagnostic.status=Status::mountIdentity;return nullptr;
            }
            auto next=std::unique_ptr<OriginalTaIntroAuthoringCandidate>(new OriginalTaIntroAuthoringCandidate);
            next->provider_=std::move(provider);next->metadata_=std::move(metadata);
            next->capturedMount_=views[0].mount;next->capturedGeneration_=views[0].bankGeneration;
            next->capturedRevision_=revision;
            for(std::size_t i=0;i<views.size();++i){
                diagnostic.member=i;
                next->bytes_[i].assign(views[i].data,views[i].data+views[i].size);
                const auto& b=next->bytes_[i];
                if(!ta_cut_authoring_candidate_detail::hashMatches(b.data(),b.size(),memberDigests_[i])){
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
            for(std::size_t i=0;i<next->events_.size();++i){
                diagnostic.eventId=static_cast<unsigned>(i+1);diagnostic.member=0;
                const auto f=filenameOffsets_[i];const auto camera=cameraOffsets_[i];
                if(f+39u+2u>ini.size()||ini[f+39]!='\r'||ini[f+40]!='\n'||
                   ini[streamOffsets_[i]]!='1'||ini[stereoOffsets_[i]]!='0'||ini[loopOffsets_[i]]!='0'||
                   camera+2u>=script.size()||script[camera]!=static_cast<std::uint8_t>('1'+i)||
                   script[camera+1]!='\r'||script[camera+2]!='\n'){
                    diagnostic.status=Status::layout;return nullptr;
                }
                auto& event=next->events_[i];
                event.eventId=static_cast<std::uint8_t>(i+1);
                event.cameraNumber=static_cast<std::uint8_t>(script[camera]-'0');
                // Span is ABSENT in all three authenticated Line sections.
                // 177750's source default is0; no source bytes2..7 are invented.
                event.span=0;
                event.filename={reinterpret_cast<const char*>(ini.data()+f),39};
                event.isStream=ini[streamOffsets_[i]]=='1';
                event.isStereo=ini[stereoOffsets_[i]]=='1';
                event.isLoop=ini[loopOffsets_[i]]=='1';
                // 292764..29279C: stream?2:1, OR stereo0x40 and loop0x80.
                // Actual TA flags are2, not the superseded earlier0x41 decode.
                event.factoryFlags=(event.isStream?2u:1u)|(event.isStereo?0x40u:0u)|(event.isLoop?0x80u:0u);
                event.filenameByteOffset=f;event.cameraNumberByteOffset=camera;
                event.metadata=next->metadata_->findForStreamRequest(event.isStream,event.filename);
                if(!event.metadata||event.metadata!=next->metadata_->row(metadataIndices_[i])||
                   event.metadata->sourceIndex!=metadataIndices_[i]||
                   event.metadata->songNumber!=metadataIndices_[i]+1u){
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
        return eventId>=1&&eventId<=3?&events_[eventId-1]:nullptr;
    }
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
    inline static constexpr const char* archiveDigest_=
        "b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8";
    inline static constexpr std::array<const char*,2> memberNames_{{"audio\\introAud.ini","audio\\scrptLns.ini"}};
    inline static constexpr std::array<std::size_t,2> memberSizes_{{346,119}};
    inline static constexpr std::array<std::size_t,2> memberOrdinals_{{381,380}};
    inline static constexpr std::array<const char*,2> memberDigests_{{
        "22e1afe07f5f34033b63a75fa8b5c2a30f49869f57cfa96926ba5c51eba9bf50",
        "1a325c68bbccaa8dfa94f87a8d1dcb743d029c59d96771797ce7045135cf6a0f"}};
    inline static constexpr std::array<std::size_t,3> filenameOffsets_{{57,161,265}};
    inline static constexpr std::array<std::size_t,3> streamOffsets_{{109,213,317}};
    inline static constexpr std::array<std::size_t,3> stereoOffsets_{{123,227,331}};
    inline static constexpr std::array<std::size_t,3> loopOffsets_{{135,239,343}};
    inline static constexpr std::array<std::size_t,3> cameraOffsets_{{56,85,114}};
    inline static constexpr std::array<std::size_t,3> metadataIndices_{{6,16,7}};
};
}
