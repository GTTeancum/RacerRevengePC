#pragma once
// Owns the exact intro INI bytes (audio\\introAud.ini, audio\\scrptLns.ini) of the
// mounted course archive, for every course, and pins the existing real provider
// and immutable APPSTART metadata. Common rewrite (R2, B-C10): the archive and
// member identity come from the course's own derived cut row
// (original_track_cut_identity50la.h); the events come from the INIs by key
// (292650/2941F0, 158B30). No per-course specification table exists. This is not an emitter, Sound, group member,
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
                // The mounted archive's own derived row (every course, one rule).
                if(!i&&(owned=courseSpecification(*mount.archiveName)))spec=&owned->spec;
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
            // CameraNumber). The stock byte offsets, camera values and metadata
            // indices are receipts (stock_course_receipts50cr.h cuts.intro_event.*),
            // checked by the host data check, never read here.
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
            }
            next->eventCount_=static_cast<unsigned>(numEvents);
            // One line per course, the same for every course.
            std::fprintf(stderr,"course_intro_rows track=%.*s events=%u lines=%u cameras=",
                int(spec->track.size()),spec->track.data(),unsigned(numEvents),unsigned(numLines));
            for(std::size_t i=0;i<numEvents;++i)std::fprintf(stderr,"%s%u",i?",":"",unsigned(next->events_[i].cameraNumber));
            std::fprintf(stderr,"\n");
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
        std::size_t archiveBytes=0,memberCount=0;
        std::array<std::size_t,2> memberSizes{},memberOrdinals{};
        std::array<const char*,2> memberDigests{};
    };
    // Owned copies of the derived row's strings, so this owner never borrows a
    // row a later derivation of the same code may replace.
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
    // Every course: the specification is the mounted archive's own derived cut
    // row (course_cut_identity50ci.h at course select), matched by the mounted
    // archive name. No stock row and no per-course table.
    static std::unique_ptr<OwnedSpecification> courseSpecification(std::string_view archiveName){
        const auto* row=originalTrackCutRowForArchive50LA(archiveName);
        if(!row)return {};
        const auto code=originalTrackCutRowCode50LA(*row);
        if(code.size()!=2u||!row->identity.archiveName)return {};
        auto owned=std::make_unique<OwnedSpecification>();
        owned->track.assign(code.data(),code.size());owned->archiveName=row->identity.archiveName;
        owned->archiveDigest=row->archiveSha;
        auto& s=owned->spec;
        s.track=owned->track;s.archiveName=owned->archiveName;s.archiveDigest=owned->archiveDigest;
        s.archiveBytes=row->identity.archiveBytes;s.memberCount=row->identity.memberCount;
        for(std::size_t i=0;i<2;++i){
            owned->digests[i]=row->introShas[i];
            s.memberSizes[i]=row->identity.introMembers[i].size;s.memberOrdinals[i]=row->identity.introMembers[i].ordinal;
            s.memberDigests[i]=owned->digests[i].c_str();
        }
        return owned;
    }
    inline static constexpr std::array<const char*,2> memberNames_{{"audio\\introAud.ini","audio\\scrptLns.ini"}};
};
}
