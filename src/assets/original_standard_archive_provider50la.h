#pragma once
#include "import_decode48c.h"
#include "original_ui_archive50kg.h"
#include <cstring>
#include <exception>
#include <thread>
#include <limits>
#include <memory>
#include <utility>

namespace original_main_entry50KG { struct Application; }
namespace rc_cold_sources50LA { class Owner; }
namespace tb_cold_sources50LA { class Owner; }
namespace gb_cold_sources50LA { class Owner; }
namespace mc_cold_sources50LA { class Owner; }
namespace rr::assets {
class OriginalArchiveStagedReader50LA;
// The actual bounded NATIVE Application mount set. This is not a claim that
// retail3E1A70's complete mount chronology/priority has been reconstructed.
// A mount always contains an entire authenticated archive directory, never a
// list of selected materials or caller-supplied absent names. Authentication
// belongs to the existing real Rig/Track archive caller; it must pass the full
// decode of the SAME bytes it authenticated, including every directory entry.
// OriginalUiArchive50KG already enforces this complete authentication itself.
class OriginalStandardArchiveProvider50LA final {
public:
    enum class Lookup : std::uint8_t { unknown,absent,present };
    enum class Status : std::uint8_t { complete,input,capacity,identity,conflict,allocation };
    struct Diagnostic {
        Status status=Status::complete;
        std::size_t mount=static_cast<std::size_t>(-1),ordinal=static_cast<std::size_t>(-1);
    };
    struct MemberView {
        const std::string* member=nullptr;
        const std::uint8_t* data=nullptr;
        std::size_t size=0,mount=static_cast<std::size_t>(-1),ordinal=static_cast<std::size_t>(-1);
        std::uint64_t revision=0;
    };
    struct MountView {
        const std::string* archiveName=nullptr;
        const std::string* archiveSha256=nullptr;
        std::size_t archiveBytes=0,memberCount=0;
    };
    OriginalStandardArchiveProvider50LA()=default;
    OriginalStandardArchiveProvider50LA(const OriginalStandardArchiveProvider50LA&)=delete;
    OriginalStandardArchiveProvider50LA& operator=(const OriginalStandardArchiveProvider50LA&)=delete;

    // Cold serial transaction. No decoder, hash function, file IO, source
    // mutation or negative-result declaration occurs here. Complete immutable
    // decoded bytes and original directory order are retained by this owner.
    bool mountAuthenticatedRes(const std::string& archiveName,const std::string& archiveSha256,
        std::size_t archiveBytes,const ResArchive48C& archive,Diagnostic& d) noexcept {
        d={};
        if(!nameOkay(archiveName)||!digestOkay(archiveSha256)||!archiveBytes||
           archive.entries.empty()||archive.blocks.empty()||archive.decoded.empty())
            return fail(d,Status::input);
        if(archive.entries.size()>maxEntries||archive.decoded.size()>maxBytes)
            return fail(d,Status::capacity);
        try {
            auto next=std::make_unique<Mount>();
            next->name=archiveName;next->digest=archiveSha256;next->archiveBytes=archiveBytes;
            next->backing=std::make_shared<const std::vector<std::uint8_t>>(archive.decoded);
            next->retainedBytes=next->backing->size();next->rows.reserve(archive.entries.size());
            for(std::size_t i=0;i<archive.entries.size();++i){
                d.ordinal=i;const auto& row=archive.entries[i];
                if(!nameOkay(row.name)||row.logical_offset>next->backing->size()||
                   row.bytes>next->backing->size()-row.logical_offset)
                    return fail(d,Status::input);
                next->rows.push_back({row.name,next->backing->data()+row.logical_offset,
                    row.bytes,row.logical_offset});
            }
            return commit(std::move(next),d);
        }catch(...){return fail(d,Status::allocation);}
    }
    // Enumerates ALL authenticated members, not find()'s slash-normalizing
    // lookup. Pinning the loaded owner keeps original member data stable.
    bool mountAuthenticatedUi(std::shared_ptr<const OriginalUiArchive50KG> archive,
        Diagnostic& d) noexcept {
        d={};const auto* identity=archive?archive->identity():nullptr;
        if(!archive||!archive->loaded()||!identity||
           archive->memberCount()!=identity->memberCount||!identity->memberCount)
            return fail(d,Status::input);
        try {
            auto next=std::make_unique<Mount>();next->name=identity->discPath;
            next->digest=identity->sha256;next->archiveBytes=identity->bytes;next->ui=std::move(archive);
            if(!nameOkay(next->name)||!digestOkay(next->digest)||!next->archiveBytes||
               identity->memberCount>maxEntries)return fail(d,Status::identity);
            next->rows.reserve(identity->memberCount);
            for(std::size_t i=0;i<identity->memberCount;++i){
                d.ordinal=i;const auto source=next->ui->member(i);
                if(!source.identity||!source.data||source.size!=source.identity->bytes||
                   !nameOkay(source.identity->name))return fail(d,Status::identity);
                if(source.size>maxBytes-next->retainedBytes)return fail(d,Status::capacity);
                next->retainedBytes+=source.size;
                next->rows.push_back({source.identity->name,source.data,source.size,source.identity->logicalOffset});
            }
            return commit(std::move(next),d);
        }catch(...){return fail(d,Status::allocation);}
    }
    // Called after the real caller has mounted every archive participating in
    // THIS native namespace. New mounts unseal it. This is a transaction phase,
    // not a fabricated proof about unmounted retail archives or selected files.
    bool sealNativeMounts(Diagnostic& d) noexcept {
        d={};if(mounts_.empty()||!retainedSourcesAccessible())return fail(d,Status::input);
        sealed_=true;return true;
    }
    bool sealed()const noexcept{return sealed_;}
    std::uint64_t revision()const noexcept{return revision_;}
    std::size_t mountCount()const noexcept{return mounts_.size();}
    bool mount(std::size_t i,MountView& out)const noexcept {
        out={};if(!retainedSourcesAccessible()||i>=mounts_.size())return false;const auto& m=*mounts_[i];
        out={&m.name,&m.digest,m.archiveBytes,m.rows.size()};return true;
    }
    // Positive/absent cover every member of every actual native mount. Unknown
    // covers incomplete mounts, invalid queries and conflicting exact matches.
    // Unrelated ambiguous names do not invalidate this query's genuine absence.
    // Successful append never moves existing rows, names, bytes or mount IDs.
    Lookup lookup132820(const std::string& requested,MemberView& out)const noexcept {
        out={};out.revision=revision_;
        if(!retainedSourcesAccessible()||!sealed_||!nameOkay(requested))return Lookup::unknown;
        // Exact native POD17/RC collision authority is issued only by the
        // authenticated RC owner. All other names retain ambiguity rejection.
        if(sameName(requested,"textures\\specmap5.tga")){
            const Mount* policy=nullptr;
            for(std::size_t m=0;m<mounts_.size();++m){const auto& candidate=*mounts_[m];
                if(candidate.rcSpecmapEarlierMount==static_cast<std::size_t>(-1))continue;
                if(policy||!candidate.rcSource||candidate.rcSpecmapEarlierMount>=m||
                   candidate.rcSpecmapOrdinal>=candidate.rows.size())return Lookup::unknown;
                policy=&candidate;
            }
            if(policy){
                const auto earlier=policy->rcSpecmapEarlierMount;
                const auto& pod=*mounts_[earlier];
                if(policy->rcSpecmapEarlierOrdinal>=pod.rows.size())return Lookup::unknown;
                const auto& winner=pod.rows[policy->rcSpecmapEarlierOrdinal];
                const auto& retainedRc=policy->rows[policy->rcSpecmapOrdinal];
                if(!sameName(winner.name,requested)||!sameName(retainedRc.name,requested)||
                   winner.size!=12332||retainedRc.size!=196652)return Lookup::unknown;
                // Identical ordinary POD rows may coalesce. Only the exact
                // retained RC row may differ; any third unequal row rejects.
                for(const auto& mount:mounts_)for(const auto& row:mount->rows)
                    if(sameName(requested,row.name) && &row!=&retainedRc&&!equalBytes(winner,row))
                        return Lookup::unknown;
                out={&winner.name,winner.data,winner.size,earlier,policy->rcSpecmapEarlierOrdinal,revision_};
                return Lookup::present;
            }
        }
        const Row* matched=nullptr;
        for(std::size_t m=0;m<mounts_.size();++m){const auto& rows=mounts_[m]->rows;
            for(std::size_t i=0;i<rows.size();++i)if(sameName(requested,rows[i].name)){
                if(matched&&!equalBytes(*matched,rows[i])){
                    out={};out.revision=revision_;return Lookup::unknown;
                }
                if(!matched){matched=&rows[i];out={&rows[i].name,rows[i].data,rows[i].size,m,i,revision_};}
            }
        }
        return matched?Lookup::present:Lookup::absent;
    }
    // Only UI mounts carry this authenticated original-file backing. No
    // registration, revision/seal change, cache activation or page admission.
    // A view keeps immutable bytes alive even after this mount owner retires;
    // its provider pointer is identity only, not a lifetime/service capability.
    bool encodedUiInput(std::size_t mountIndex,OriginalUiEncodedArchiveView50KG& out) const noexcept {
        out={};
        if(mountIndex>=mounts_.size() || !mounts_[mountIndex]->ui) return false;
        out=OriginalUiEncodedArchiveView50KG(this,mounts_[mountIndex]->ui);
        return out.identity() && out.data() && out.size();
    }
    static bool sameName(const std::string& a,const std::string& b)noexcept {
        if(a.size()!=b.size())return false;
        for(std::size_t i=0;i<a.size();++i)if(lower(a[i])!=lower(b[i]))return false;
        return true;
    }
private:
    friend class ::rc_cold_sources50LA::Owner;
    friend class ::tb_cold_sources50LA::Owner;
    friend class ::gb_cold_sources50LA::Owner;
    friend class ::mc_cold_sources50LA::Owner;
    friend struct ::original_main_entry50KG::Application;
    bool issueStagedReader(const original_main_entry50KG::Application&,
        const std::shared_ptr<OriginalStandardArchiveProvider50LA>&,
        std::shared_ptr<OriginalArchiveStagedReader50LA>&) noexcept;
    struct Row {std::string name;const std::uint8_t* data=nullptr;std::size_t size=0,offset=0;};
    struct Mount {
        std::string name,digest;std::size_t archiveBytes=0,retainedBytes=0;
        std::shared_ptr<const std::vector<std::uint8_t>> backing;
        std::shared_ptr<const OriginalUiArchive50KG> ui;
        // Complete RC compact namespace; the typed source retains every stable
        // member and the exact encoded original. No generic row authority.
        std::shared_ptr<const rc_cold_sources50LA::Owner> rcSource;
        std::thread::id rcThread{};
        // Distinct complete authenticated TB compact source; never an RC retag.
        std::shared_ptr<const tb_cold_sources50LA::Owner> tbSource;
        std::thread::id tbThread{};
        // Distinct complete authenticated GB source; every member stays retained.
        std::shared_ptr<const gb_cold_sources50LA::Owner> gbSource;
        std::thread::id gbThread{};
        // Distinct complete authenticated MC source, with exact typed lifetime.
        std::shared_ptr<const mc_cold_sources50LA::Owner> mcSource;
        std::thread::id mcThread{};
        // Immutable private row identities, never a caller-set lookup flag.
        std::size_t rcSpecmapEarlierMount=static_cast<std::size_t>(-1);
        std::size_t rcSpecmapEarlierOrdinal=static_cast<std::size_t>(-1);
        std::size_t rcSpecmapOrdinal=static_cast<std::size_t>(-1);
        ~Mount(){if((rcSource&&rcThread!=std::this_thread::get_id())||
            (tbSource&&tbThread!=std::this_thread::get_id())||
            (gbSource&&gbThread!=std::this_thread::get_id())||
            (mcSource&&mcThread!=std::this_thread::get_id()))std::terminate();}
        std::vector<Row> rows;
    };
    static constexpr std::size_t maxMounts=64,maxEntries=65536,maxBytes=256u*1024u*1024u;
    static char lower(char c)noexcept{return c>='A'&&c<='Z'?char(c+('a'-'A')):c;}
    static bool nameOkay(const std::string& s)noexcept {
        if(s.empty()||s.size()>1024)return false;
        for(unsigned char c:s)if(c<32||c>=127)return false;
        return true;
    }
    static bool digestOkay(const std::string& s)noexcept {
        return s.size()==64&&s.find_first_not_of("0123456789abcdef")==std::string::npos;
    }
    static bool equalBytes(const Row& a,const Row& b)noexcept {
        return a.size==b.size&&(!a.size||std::memcmp(a.data,b.data,a.size)==0);
    }
    static bool fail(Diagnostic& d,Status s)noexcept{d.status=s;return false;}
    bool retainedSourcesAccessible()const noexcept {
        for(const auto& m:mounts_)if((m->rcSource&&m->rcThread!=std::this_thread::get_id())||
            (m->tbSource&&m->tbThread!=std::this_thread::get_id())||
            (m->gbSource&&m->gbThread!=std::this_thread::get_id())||
            (m->mcSource&&m->mcThread!=std::this_thread::get_id()))return false;
        return true;
    }
    bool commit(std::unique_ptr<Mount> next,Diagnostic& d) {
        if(!retainedSourcesAccessible())return fail(d,Status::identity);
        for(std::size_t m=0;m<mounts_.size();++m)if(sameName(mounts_[m]->name,next->name)){
            d.mount=m;const auto& old=*mounts_[m];
            if(old.rcSource!=next->rcSource||old.tbSource!=next->tbSource||old.gbSource!=next->gbSource||old.mcSource!=next->mcSource||old.digest!=next->digest||old.archiveBytes!=next->archiveBytes||
               old.rows.size()!=next->rows.size())return fail(d,Status::identity);
            for(std::size_t i=0;i<old.rows.size();++i){d.ordinal=i;
                if(old.rows[i].name!=next->rows[i].name||old.rows[i].offset!=next->rows[i].offset||
                   !equalBytes(old.rows[i],next->rows[i]))return fail(d,Status::identity);
            }
            d={};return true; //Same immutable mount, no revision/phase change.
        }
        if(mounts_.size()>=maxMounts||next->rows.size()>maxEntries-entryCount_||
           next->retainedBytes>maxBytes-retainedBytes_||revision_==std::numeric_limits<std::uint64_t>::max())
            return fail(d,Status::capacity);
        // Preserve every genuine mount/member, even unrelated duplicate names.
        // Query-local comparison rejects only an actually requested conflict;
        // mount order never substitutes for unrecovered retail priority.
        const auto entries=next->rows.size(),bytes=next->retainedBytes;
        mounts_.push_back(std::move(next));entryCount_+=entries;retainedBytes_+=bytes;
        ++revision_;sealed_=false;d={};return true;
    }
    std::vector<std::unique_ptr<const Mount>> mounts_;
    std::size_t entryCount_=0,retainedBytes_=0;
    std::uint64_t revision_=0;
    bool sealed_=false;
};
}
