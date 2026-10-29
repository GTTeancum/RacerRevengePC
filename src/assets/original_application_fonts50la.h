#pragma once
#include "original_standard_material_domain50la.h"
#include "original_standard_material_context50la.h"
#include "original_ui_font50kg.h"
#include "original_loading_font_source50la.h"
#include <new>
#include <array>
#include <cstring>
#include <exception>
#include <memory>
#include <string>
#include <vector>

namespace rr::game { class OriginalLoadingScreen40Mode0LA; class OriginalResults10450LA; class OriginalRaceF850LA; }
namespace original_main_entry50KG { struct Application; }
namespace rr::platform { class OriginalUiGarageScreenText50KG; }
namespace rr::assets {
enum class OriginalApplicationFontStatus50LA : std::uint8_t {
    complete, phase, source, font, texture, allocation
};
struct OriginalApplicationFontDiagnostic50LA {
    OriginalApplicationFontStatus50LA status=OriginalApplicationFontStatus50LA::complete;
    std::uint32_t sourcePC=0;
    std::size_t font=0, published=0;
    bool sourceEffectsCommitted=false;
    OriginalUiCelDiagnostic50KG cel;
    OriginalStandardTextureDiagnostic50LA texture;
};
// First, bounded Application328 owner. No public constructor, replacement
// domain, arbitrary font name, reload1, draw certification or demand setter.
// Only the real Application can instantiate it with its existing domain and
// Video-minted context. Native strong leases keep original borrowed font
// pointers safe; they do NOT add source texture references on every copy.
class OriginalApplicationFonts50LA final {
    friend struct original_main_entry50KG::Application;
    friend class rr::game::OriginalLoadingScreen40Mode0LA;
    friend class rr::game::OriginalPauseStorage50LA;
    friend class rr::game::OriginalResults10450LA;
    friend class rr::game::OriginalRaceF850LA;
    friend class rr::platform::OriginalUiGarageScreenText50KG;
#include "original_loading_font_partial50la.inl"
public:
    enum class Name : std::uint8_t { eurostile13=0, denmark16=1, eurostileItalic26=2, frontendDenmrk16=3, frontendVenus16=4, frontendDenmrk14=5, frontendVenus14=6, frontendDenmrk12=7, frontendAlaska13=8, frontendSwhutt44=9, frontendHelvcondblack14=10, frontendDenmrk20=11, frontendVenus20=12, frontendDenmrk18=13, frontendDenmrk14credits=14 };
    class Entry final {
        friend class OriginalApplicationFonts50LA;
        friend class rr::game::OriginalPauseStorage50LA;
    friend class rr::game::OriginalResults10450LA;
    friend class rr::game::OriginalRaceF850LA;
    friend class rr::platform::OriginalUiGarageScreenText50KG;
        std::shared_ptr<OriginalStandardMaterialDomain50LA> domain_;
        std::shared_ptr<const OriginalStandardMaterialContext50LA> context_;
        std::shared_ptr<const OriginalUiArchive50KG> archive_;
        OriginalUiFont50KG metrics_;
        std::unique_ptr<LoadingFontPartial> loadingPartial_;
        bool loadingOuterAttempted_=false,loadingOuterKnownNull_=false;
        bool loadingTextureRetained_=false;
        std::unique_ptr<LoadingRegistryNode> loadingRegistryNode_;
        bool loadingRegistryPublished_=false;

        std::shared_ptr<OriginalLoadingFontSource50LA> loadingSource_;
        std::string loadingSavedDirectory_;
        std::string* loadingDirectory_=nullptr;
        OriginalUiMemberView50KG cel_,pst_;
        std::int16_t texture408_=-1;
        const OriginalStandardTextureResource50LA* texture_=nullptr;
        explicit Entry(std::shared_ptr<OriginalStandardMaterialDomain50LA> domain,
            std::shared_ptr<const OriginalStandardMaterialContext50LA> context,
            std::shared_ptr<const OriginalUiArchive50KG> archive)
            :domain_(std::move(domain)),context_(std::move(context)),archive_(std::move(archive)){}
    public:
        Entry(const Entry&)=delete;
        Entry& operator=(const Entry&)=delete;
        ~Entry(){OriginalApplicationFonts50LA::release(*this);}
        const OriginalUiFont50KG& metrics()const noexcept{return metrics_;}
        const OriginalUiMemberIdentity50KG* celIdentity()const noexcept{return cel_.identity;}
        const OriginalUiMemberIdentity50KG* atlasIdentity()const noexcept{return pst_.identity;}
        // Immutable CPU resource only. Closing Video does not destroy this
        // lease, and a pointer is never evidence of present GPU readiness.
        const OriginalStandardTextureResource50LA* texture281F90()const noexcept{return texture_;}
    };
private:
    std::shared_ptr<OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context_;
    // Proven in-game names and all twelve original1B740C frontend names.
    // Represented identities are NOT an original registry/GPU capacity.
    // Keep in-game denmark16 distinct from frontend denmrk16.
    struct Specification {const char* basename;OriginalUiArchiveId50KG archive;};
    static constexpr std::array<Specification,15> specifications_{{
        {"eurostileboldbold13ingame",OriginalUiArchiveId50KG::overlays},
        {"denmarkbold16ingame",OriginalUiArchiveId50KG::overlays},
        {"eurostileboldbolditalic26ingame",OriginalUiArchiveId50KG::overlays},
        {"denmrk16",OriginalUiArchiveId50KG::fonts},
        {"venus16",OriginalUiArchiveId50KG::fonts},
        {"denmrk14",OriginalUiArchiveId50KG::fonts},
        {"venus14",OriginalUiArchiveId50KG::fonts},
        {"denmrk12",OriginalUiArchiveId50KG::fonts},
        {"alaska13",OriginalUiArchiveId50KG::fonts},
        {"swhutt44",OriginalUiArchiveId50KG::fonts},
        {"helvcondblack14",OriginalUiArchiveId50KG::fonts},
        {"denmrk20",OriginalUiArchiveId50KG::fonts},
        {"venus20",OriginalUiArchiveId50KG::fonts},
        {"denmrk18",OriginalUiArchiveId50KG::fonts},
        {"denmrk14credits",OriginalUiArchiveId50KG::fonts}}};
    std::array<std::shared_ptr<const Entry>,specifications_.size()> entries_{};
    std::array<std::size_t,specifications_.size()> publicationOrder_{};
    std::size_t publicationCount_=0;
    LoadingRegistryNode* loadingRegistryHead_=nullptr;
    LoadingRegistryNode* loadingRegistryTail_=nullptr;
    std::uint32_t loadingRegistryCount_=0;

    // Unpublished acquired state survives failure until owner cleanup;
    // previously appended entries and table population effects are retained.
    std::shared_ptr<Entry> pending_;
    bool terminal_=false;
    OriginalApplicationFontDiagnostic50LA terminalDiagnostic_{};
    OriginalApplicationFonts50LA(std::shared_ptr<OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const OriginalStandardMaterialContext50LA> context)
        :domain_(std::move(domain)),context_(std::move(context)){}
    static void release(Entry& e)noexcept {
        // Source CEL must already be closed, or explicit native abort must
        // have succeeded before any pending Font/resource backing is freed.
        if(e.loadingSource_&&e.loadingSource_->ownsCel(&e))std::terminate();
        if(e.loadingPartial_)for(auto& glyph:e.loadingPartial_->glyphs08)glyph.reset();
        // A genuine408 store may precede a failed298A30. Only release a
        // reference actually acquired by this Loading Font; generic ownership
        // behavior is unchanged. Unacquired rows remain in the same table.
        if(e.texture408_>=0&&(!e.loadingPartial_||e.loadingTextureRetained_)){
            if(!e.domain_||!e.domain_->textures383400()||
               !e.domain_->textures383400()->release29E400(e.texture408_))std::terminate();
            e.texture408_=-1;e.texture_=nullptr;
        }
        e.loadingPartial_.reset(); // native name/outer disposal after glyphs/material
    }
    static bool fail(OriginalApplicationFontDiagnostic50LA& d,
        OriginalApplicationFontStatus50LA status,std::uint32_t pc)noexcept {
        d.status=status;d.sourcePC=pc;return false;
    }
public:
    OriginalApplicationFonts50LA(const OriginalApplicationFonts50LA&)=delete;
    OriginalApplicationFonts50LA& operator=(const OriginalApplicationFonts50LA&)=delete;
    ~OriginalApplicationFonts50LA(){if(!close282190())std::terminate();}
    // Reject teardown under any ScreenText/renderer borrow; source manager
    // then destroys published fonts in insertion order, not array reverse.
    bool close282190()noexcept {
        for(const auto& entry:entries_)if(entry&&entry.use_count()!=1)return false;
        if(pending_&&pending_->loadingSource_){
            OriginalStandardTextureDiagnostic50LA native;
            if(!domain_||!domain_->textures383400()||
               !domain_->textures383400()->abortLoadingPstNative(pending_->loadingSource_,native))return false;
        }
        if(pending_&&pending_->loadingSource_&&pending_->loadingSource_->ownsCel(pending_.get())){
            OriginalStandardTextureDiagnostic50LA native;
            if(!pending_->loadingSource_->abortCelNative(pending_.get(),native))return false;
        }
        if(pending_&&pending_->loadingDirectory_){
            pending_->loadingDirectory_->swap(pending_->loadingSavedDirectory_);
            pending_->loadingDirectory_=nullptr; // native rollback, not281F44
        }
        pending_.reset();
        for(std::size_t n=0;n<publicationCount_;++n){
            auto& entry=entries_[publicationOrder_[n]];
            if(entry&&entry->loadingRegistryNode_){
                // Native disposal follows the represented source list order.
                // A prior allocationNULL count increment has no node and is
                // not silently reset to a fictional fresh constructor count.
                const auto* node=entry->loadingRegistryNode_.get();
                if(loadingRegistryHead_!=node||!loadingRegistryCount_)std::terminate();
                loadingRegistryHead_=node->next;--loadingRegistryCount_;
                if(!loadingRegistryHead_)loadingRegistryTail_=nullptr;
            }
            entry.reset();
        }
        publicationCount_=0;

        //282190 empties the SAME list; only282280 subsequently deletes it.
        // Successful cleanup is not a permanent font-load failure. Preserve
        // any genuine earlier failure/diagnostic rather than clearing it, but
        // leave a previously healthy registry available for reached282430.
        return true;
    }
    bool terminal()const noexcept{return terminal_;}
    std::shared_ptr<const Entry> find282210(Name name)const noexcept {
        const auto i=static_cast<std::size_t>(name);
        return i<entries_.size()?entries_[i]:nullptr;
    }
private:
#include "original_loading_font_kernel50la.inl"
    // Screen's explicit native unwind owns only its exact unpublished cold
    // Font. Published Application Entries and their ScreenText borrowers are
    // unaffected; this is not282190/281620 or source281F54.
    bool abortLoadingCelNative(const std::shared_ptr<OriginalLoadingFontSource50LA>& source)noexcept {
        if(!source)return false;
        if(!pending_)return true;
        if(pending_->loadingSource_!=source||(!pending_->loadingPartial_&&
           !pending_->loadingOuterAttempted_))return false;
        OriginalStandardTextureDiagnostic50LA native;
        if(!domain_||!domain_->textures383400()||
           !domain_->textures383400()->abortLoadingPstNative(source,native))return false;
        if(source->ownsCel(pending_.get())&&!source->abortCelNative(pending_.get(),native))return false;
        if(pending_->loadingDirectory_){
            pending_->loadingDirectory_->swap(pending_->loadingSavedDirectory_);
            pending_->loadingDirectory_=nullptr;
        }
        pending_.reset(); // terminal/history remains; never a fresh retry grant
        return true;
    }
    // Root validation through this owner's existing Domain friendship. The
    // Domain's constructor binds its canonical tables to this retained owner;
    // no material-table friendship or public readiness surface is introduced.
    static bool domainApplicationMatches(
        const std::shared_ptr<OriginalStandardMaterialDomain50LA>& domain,
        const std::shared_ptr<const OriginalUiMaterialApplication50LA>& application)noexcept {
        return domain&&domain->prepared()&&application&&
            domain->application_.get()==application.get()&&
            !domain->application_.owner_before(application)&&!application.owner_before(domain->application_);
    }
    // Concrete ScreenText checks its retained Entry through the already-owning
    // Font manager's TextureTable friendship. This reads the SAME canonical
    // row/resource and PST; it neither acquires a ref nor grants readiness.
    static bool currentResource281F90(const OriginalStandardMaterialDomain50LA& domain,const Entry& font)noexcept {
        const auto table=domain.textures383400();
        if(!table||!font.cel_.identity||!font.cel_.data||font.cel_.size!=font.cel_.identity->bytes||
           !font.pst_.identity||!font.pst_.data||font.pst_.size!=font.pst_.identity->bytes)return false;
        OriginalStandardTextureRowView50LA row;OriginalStandardTextureResource50LA* resource=nullptr;
        OriginalStandardTextureDiagnostic50LA diagnostic;
        if(!table->row(font.texture408_,row)||!row.references08||*row.references08<=0||
           !table->resolve298E60(font.texture408_,resource,diagnostic)||resource!=font.texture_||row.object04!=resource||
           !resource||!resource->constructed24DA30()||resource->animated102070()||
           !resource->references00()||*resource->references00()<=0)return false;
        const auto* bytes=resource->originalBytes();
        return bytes&&bytes->size()==font.pst_.size&&!std::memcmp(bytes->data(),font.pst_.data,font.pst_.size);
    }
    // Only Application supplies its actual mutable36B970 and constant36B420.
    // Exact TimeStats193A0C two-name/reload0 subset of282430. Input is the
    // authenticated OVERLAYS owner, already mounted in this SAME provider;
    // its PST corpus must already be appended to this SAME resolver by root.
    // No fake material is constructed to retain the font's texture408.
    bool loadTimeStats193A0C(std::shared_ptr<const OriginalUiArchive50KG> archive,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
        OriginalApplicationFontDiagnostic50LA& d)noexcept {
        return loadNames282430<2>(std::move(archive),actualDirectory36B970,fontDirectory36B420,
            {{Name::eurostile13,Name::denmark16}},0x193A0C,d);
    }
    // Source382C70 ->3AC9B0, one denmark16 request, reload0 at1B8BC4.
    // Separate from TimeStats' two-name list; preserves actual insertion order.
    bool loadPauseBase1B8BC4(std::shared_ptr<const OriginalUiArchive50KG> archive,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
        OriginalApplicationFontDiagnostic50LA& d)noexcept {
        return loadNames282430<1>(std::move(archive),actualDirectory36B970,fontDirectory36B420,
            {{Name::denmark16}},0x1B8BC4,d);
    }
    // Source382C78 also points to denmark16: derived1BA3E4, count1/reload0.
    bool loadPauseDerived1BA3E4(std::shared_ptr<const OriginalUiArchive50KG> archive,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
        OriginalApplicationFontDiagnostic50LA& d)noexcept {
        return loadNames282430<1>(std::move(archive),actualDirectory36B970,fontDirectory36B420,
            {{Name::denmark16}},0x1BA3E4,d);
    }
    // Results1BEF20 consumes actual382C80 in THIS order, count2/reload0.
    // Retains the same Application328 entries, not TimeStats' reversed list.
    bool loadResultsDerived1BEF20(std::shared_ptr<const OriginalUiArchive50KG> archive,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
        OriginalApplicationFontDiagnostic50LA& d)noexcept {
        return loadNames282430<2>(std::move(archive),actualDirectory36B970,fontDirectory36B420,
            {{Name::denmark16,Name::eurostile13}},0x1BEF20,d);
    }
    //198798 reads actual382C28 -> denmarkbold16ingame, count1/reload0.
    //The connected F8 caller follows complete Results on this SAME registry.
    bool loadRaceF8198798(std::shared_ptr<const OriginalUiArchive50KG> archive,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
        OriginalApplicationFontDiagnostic50LA& d)noexcept {
        return loadNames282430<1>(std::move(archive),actualDirectory36B970,fontDirectory36B420,
            {{Name::denmark16}},0x198798,d);
    }
    // Actual382C08 pair at18A99C: italic26, then existing13, reload0.
    bool loadSpeedBoost18A99C(std::shared_ptr<const OriginalUiArchive50KG> archive,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
        OriginalApplicationFontDiagnostic50LA& d)noexcept {
        return loadNames282430<2>(std::move(archive),actualDirectory36B970,fontDirectory36B420,
            {{Name::eurostileItalic26,Name::eurostile13}},0x18A99C,d);
    }
    //190E64: the original one-name list382C18, reload0. Do not request the
    //TimeStats/SpeedBoost partner font or alter existing insertion order.
    bool loadPositionStats190E64(std::shared_ptr<const OriginalUiArchive50KG> archive,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
        OriginalApplicationFontDiagnostic50LA& d)noexcept {
        return loadNames282430<1>(std::move(archive),actualDirectory36B970,fontDirectory36B420,
            {{Name::eurostile13}},0x190E64,d);
    }
    // Reached TextOverlay29CA40 calls282430 at29CB74, count1/reload0.
    // This is the frontend FONTS name, never the in-game denmark16 alias.
    // Actual caller/localization/Entity predecessors remain outside this leaf.
    bool loadGarageText29CB74(std::shared_ptr<const OriginalUiArchive50KG> archive,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
        OriginalApplicationFontDiagnostic50LA& d)noexcept {
        return loadGarageText29CB74(std::move(archive),actualDirectory36B970,fontDirectory36B420,
            Name::frontendDenmrk16,d);
    }
    bool loadGarageText29CB74(std::shared_ptr<const OriginalUiArchive50KG> archive,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,Name name,
        OriginalApplicationFontDiagnostic50LA& d)noexcept {
        if(name!=Name::frontendDenmrk16&&name!=Name::frontendVenus16&&name!=Name::frontendDenmrk14){
            d={};return fail(d,OriginalApplicationFontStatus50LA::source,0x29CB74);
        }
        return loadNames282430<1>(std::move(archive),actualDirectory36B970,fontDirectory36B420,
            {{name}},0x29CB74,d);
    }
    // Direct2822F0 calls at1CAB00 then1CAB1C, reload0; not a29CB74 call.
    // Both use the existing real2817D0/281F34 acquisition/population kernel.
    bool loadLoadingMode0_1CAB00(std::shared_ptr<const OriginalUiArchive50KG> archive,
        std::shared_ptr<OriginalLoadingFontSource50LA> source,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
        OriginalApplicationFontDiagnostic50LA& d)noexcept {
        if(!loadNames282430<1>(archive,actualDirectory36B970,fontDirectory36B420,
            {{Name::frontendDenmrk16}},0x1CAB00,d,source))return false;
        const bool priorEffects=d.sourceEffectsCommitted;
        const auto priorPublished=d.published;
        const bool complete=loadNames282430<1>(std::move(archive),actualDirectory36B970,fontDirectory36B420,
            {{Name::frontendVenus16}},0x1CAB1C,d,std::move(source));
        d.sourceEffectsCommitted|=priorEffects;d.published+=priorPublished;
        return complete;
    }
    //Only the fixed private source wrappers select names/count. The existing
    //canonical acquisition, whole-table population and failure body is shared.
    template<std::size_t Count>
    bool loadNames282430(std::shared_ptr<const OriginalUiArchive50KG> archive,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
        const std::array<Name,Count>& requested,std::uint32_t sourceCaller,
        OriginalApplicationFontDiagnostic50LA& d,
        std::shared_ptr<OriginalLoadingFontSource50LA> loadingSource={})noexcept {
        d={};
        if(terminal_){d=terminalDiagnostic_;return false;}
        using S=OriginalApplicationFontStatus50LA;
        if(fontDirectory36B420!="font\\")return fail(d,S::source,0x17C848);
        if(!domain_||!domain_->prepared()||!context_||!context_->live()||
           domain_->application_!=context_->materialApplication()||pending_||
           domain_->textures383400()->populationTerminal()||
           domain_->materials382B38()->populationTerminal()||
           domain_->materials382B38()->demandTerminal())return fail(d,S::phase,sourceCaller);
        if(!archive||!archive->loaded())return fail(d,S::source,sourceCaller);
        for(const auto name:requested){const auto i=static_cast<std::size_t>(name);
            if(i>=specifications_.size()||archive->identity()!=originalUiArchiveIdentity50KG(specifications_[i].archive))
                return fail(d,S::source,sourceCaller);
        }
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed()||!domain_->resolver3E1A70()->prepared())
            return fail(d,S::source,0x281880);
        bool committed=false;
        const bool loadingCall=sourceCaller==0x1CAB00||sourceCaller==0x1CAB1C;
        if(loadingCall&&!loadingSource)return fail(d,S::source,sourceCaller);
        const auto stop=[&](S status,std::uint32_t pc)noexcept {
            fail(d,status,pc);d.sourceEffectsCommitted=committed||d.texture.sourceEffectsCommitted;
            // Failure after any acquired row/population effect is terminal;
            // never retry a half-executed source load as a clean first call.
            if(d.sourceEffectsCommitted){terminal_=true;terminalDiagnostic_=d;}
            else pending_.reset();
            return false;
        };
        // CPU Entry borrows preserve immutable resources. Original282430
        // can append a later font while earlier ScreenTexts retain Entries.
        // Never retain RowView pointers across vector append; resolve tokens
        // afresh and compare real canonical objects/source bytes each time.
        const auto existingEntriesIntact=[&]()noexcept {
            const auto table=domain_->textures383400();
            for(std::size_t i=0;i<entries_.size();++i){const auto& entry=entries_[i];
                if(!entry)continue;
                if(entry->domain_!=domain_||entry->context_!=context_||
                    !entry->archive_||!entry->archive_->loaded()||
                    entry->archive_->identity()!=originalUiArchiveIdentity50KG(specifications_[i].archive)||
                    entry->texture408_<0||!entry->texture_||!entry->metrics_.decoded()||
                    !entry->cel_.identity||!entry->cel_.data||entry->cel_.size!=entry->cel_.identity->bytes||
                    !entry->pst_.identity||!entry->pst_.data||entry->pst_.size!=entry->pst_.identity->bytes)
                    return false;
                OriginalStandardTextureRowView50LA row;
                OriginalStandardTextureResource50LA* resource=nullptr;
                OriginalStandardTextureDiagnostic50LA resolved;
                if(!table->row(entry->texture408_,row)||!row.references08||*row.references08<=0||
                    !table->resolve298E60(entry->texture408_,resource,resolved)||
                    resource!=entry->texture_||row.object04!=resource||
                    !resource->constructed24DA30()||resource->animated102070()||
                    !resource->references00()||*resource->references00()<=0)return false;
                const auto* bytes=resource->originalBytes();
                if(!bytes||bytes->size()!=entry->pst_.size||
                    (entry->pst_.size&&std::memcmp(bytes->data(),entry->pst_.data,entry->pst_.size)!=0))return false;
            }
            return true;
        };
        if(!existingEntriesIntact())return stop(S::phase,0x281F90);
        try {
            for(const auto name:requested){
                const auto i=static_cast<std::size_t>(name);
                d.font=i;
                if(entries_[i]){
                    if(loadingCall){
                        d.sourcePC=sourceCaller;
                        const std::string name=std::string("font\\")+specifications_[i].basename+".cel";
                        if(!loadingSource->recordRegistryHit2822F0(entries_[i],name,entries_[i]->cel_,d.texture))
                            return stop(S::source,d.texture.sourcePC?d.texture.sourcePC:sourceCaller);
                    }
                    continue; //reload0 actual registry hit: no CEL open/read
                }
                if(loadingCall)d.sourcePC=0x282324;
                const std::string base=std::string("font\\")+specifications_[i].basename;
                const auto cel=archive->find(base+".cel"),pst=archive->find(base+".pst");
                if(!cel.identity||!cel.data||!pst.identity||!pst.data)
                    return stop(S::source,0x281880);
                // Exact mounted-provider bytes, not equivalent dimensions or
                // a detached screen bank. No missing source means absence.
                const auto matches=[&](const std::string& name,const OriginalUiMemberView50KG& m){
                    OriginalStandardArchiveProvider50LA::MemberView mounted;
                    return provider->lookup132820(name,mounted)==OriginalStandardArchiveProvider50LA::Lookup::present&&
                        mounted.data&&mounted.size==m.size&&
                        (!m.size||std::memcmp(mounted.data,m.data,m.size)==0);
                };
                if(!matches(base+".cel",cel)||!matches(base+".pst",pst))
                    return stop(S::source,0x281880);
                if(!loadingCall)fontSourceAssertions().unjoinedGenericProducer=true;
                pending_=std::shared_ptr<Entry>(new Entry(domain_,context_,archive));
                pending_->cel_=cel;pending_->pst_=pst;
                if(loadingCall){
                    // Eligibility is native, before source allocation/open. It
                    // does not publish source glyphs, move a cursor or set guards.
                    OriginalUiFont50KG admission;
                    if(fontSourceAssertions().unjoinedGenericProducer||
                       !decodeOriginalUiCel50KG(cel.data,cel.size,admission,d.cel))
                        return stop(S::font,0x282368);
                    // Native resolver bytes are staged before any source Font
                    // allocation/open. This is not the later numeric PST file
                    // producer, cache publication, or a materiallookup receipt.
                    d.sourcePC=0x282368;
                    OriginalStandardMaterialDiagnostic50LA admitted;
                    const std::vector<OriginalStandardTextureSourceInput50LA> input{{
                        pst.identity->name,std::vector<std::uint8_t>(pst.data,pst.data+pst.size)}};
                    if(!domain_->appendOriginalSources({},input,admitted)){
                        d.texture=admitted.texture;return stop(S::texture,0x282368);
                    }
                    const std::string celPath=base+".cel"; // native admission, before retained source prefix
                    pending_->loadingSource_=loadingSource;
                    committed=true; // retain native wrapper and any reached null publication
                    if(!loadLoadingCel2817D0(*pending_,celPath,specifications_[i].basename,d))
                        return stop(d.status,d.sourcePC);
                }else{
                    // Existing decoder is a native envelope, not a proved
                    // assertion producer. Never infer a later first-use guard
                    // from this path; a later cold Loading source call is bounded.
                    if(!decodeOriginalUiCel50KG(cel.data,cel.size,pending_->metrics_,d.cel))
                        return stop(S::font,0x2817D0);
                }
                // Resolver byte admission is native, never a source PST file
                // receipt. Loading additionally requires the exact lexical
                // file/cursor/cache producer during actual table population.
                if(sourceCaller==0x1BEF20){
                    OriginalStandardMaterialDiagnostic50LA admitted;
                    const std::vector<OriginalStandardTextureSourceInput50LA> input{{
                        pst.identity->name,std::vector<std::uint8_t>(pst.data,pst.data+pst.size)}};
                    if(!domain_->appendOriginalSources({},input,admitted)){
                        d.texture=admitted.texture;return stop(S::texture,0x281F14);
                    }
                }
                //281EC0..281F44 scopes the real application directory only
                //after CEL decoding. Generic native early returns restore it;
                //Loading retains reached directory stores until explicit abort.
                if(loadingCall)d.sourcePC=0x281EC8;
                struct DirectoryScope {
                    std::string& actual;std::string previous;bool active=true;
                    bool preserve=false;
                    DirectoryScope(std::string& a,const std::string& selected,bool retain)
                        :actual(a),previous(a),preserve(retain){actual=selected;}
                    void restore()noexcept{if(active){actual.swap(previous);active=false;}}
                    ~DirectoryScope(){if(!preserve)restore();}
                } directory(actualDirectory36B970,fontDirectory36B420,loadingCall);
                if(loadingCall){pending_->loadingSavedDirectory_.swap(directory.previous);
                    pending_->loadingDirectory_=&actualDirectory36B970;}
                if(loadingCall)d.sourcePC=0x281EF8;
                const std::string textureName=actualDirectory36B970+specifications_[i].basename;
                if(loadingCall){
                    auto& partial=*pending_->loadingPartial_;
                    if(textureName.size()>=partial.materialName190.size())
                        return stop(S::source,0x281EF8);
                    std::memcpy(partial.materialName190.data(),textureName.c_str(),textureName.size()+1);
                    partial.materialNameWritten=true;partial.materialLookupPending=true;
                }
                auto table=domain_->textures383400();
                if(!table->bindMaterialContext(context_,d.texture))
                    return stop(S::texture,0x281F14);
                if(loadingCall){
                    std::int16_t found=-1;
                    if(!table->lookupLoading299330(textureName,found,d.texture))
                        return stop(S::texture,d.texture.sourcePC?d.texture.sourcePC:0x281F14);
                    pending_->texture408_=found; //281F1C before actual298A30
                    pending_->loadingPartial_->materialLookupPending=false;
                    pending_->loadingPartial_->material408Stored=true;
                    if(!table->retainLoading298A30(found,d.texture))
                        return stop(S::texture,d.texture.sourcePC?d.texture.sourcePC:0x281F28);
                    pending_->loadingTextureRetained_=true;
                }else if(!table->acquire299330AndRetain298A30(textureName,pending_->texture408_,d.texture))
                    return stop(S::texture,0x281F28);
                committed=true;d.sourceEffectsCommitted=true;
                // Original281F34 scans the entire canonical texture table.
                // Do not restrict it to this row or roll back earlier rows.
                // The original full scan skips positive published rows;
                // only null pending rows acquire CPU resources. It does not
                // rebuild material collectors or touch native GL publication.
                if(!existingEntriesIntact())return stop(S::phase,0x281F34);
                if(loadingCall){
                    const auto resolver=domain_->resolver3E1A70();
                    if(!resolver->beginLoadingFontPstPopulation(loadingSource,pending_.get(),pst,d.texture))
                        return stop(S::texture,d.texture.sourcePC?d.texture.sourcePC:0x281F34);
                    pending_->loadingPartial_->populationEntered=true;
                    // Both calls are noexcept. End the lexical admission on
                    // every population outcome, retaining any actual partial
                    // PST attempt in Table custody rather than closing it.
                    const bool populated=table->populate29E340(resolver,context_,d.texture);
                    OriginalStandardTextureDiagnostic50LA ended;
                    const bool exited=resolver->endLoadingFontPstPopulation(*loadingSource,pending_.get(),ended);
                    if(!exited){d.texture=ended;return stop(S::texture,ended.sourcePC?ended.sourcePC:0x281F34);}
                    if(!populated)return stop(S::texture,d.texture.sourcePC?d.texture.sourcePC:0x281F34);
                    pending_->loadingPartial_->populationReturned=true;
                }else if(!table->populate29E340(domain_->resolver3E1A70(),context_,d.texture))
                    return stop(S::texture,0x281F34);
                if(!existingEntriesIntact())return stop(S::phase,0x281F90);
                if(loadingCall){
                    actualDirectory36B970.swap(pending_->loadingSavedDirectory_);
                    pending_->loadingDirectory_=nullptr;directory.active=false; //281F44
                    pending_->loadingPartial_->directoryRestored=true;
                    if(!loadingSource->closeCel131040(pending_.get(),d.texture))
                        return stop(S::source,d.texture.sourcePC?d.texture.sourcePC:0x281F54);
                    pending_->loadingPartial_->celClosed=true;
                    // Keep typed source authority until publication or exact
                    // native abort, even after its CEL occurrence is closed.
                }else directory.restore(); //281F44 on existing non-Loading path
                OriginalStandardTextureResource50LA* resource=nullptr;
                OriginalStandardTextureDiagnostic50LA resolved;
                if(!table->resolve298E60(pending_->texture408_,resource,resolved)||!resource){
                    d.texture=resolved;
                    return stop(S::texture,0x281F90);
                }
                pending_->texture_=resource;
                if(loadingCall){
                    pending_->loadingRegistryNode_.reset(new(std::nothrow) LoadingRegistryNode);
                    auto* node=pending_->loadingRegistryNode_.get();
                    if(node){node->font=pending_.get();node->next=nullptr;}
                    // Same source2822F0 calls281FE0. New cold joins after an
                    // unrepresented generic producer were refused at admission.
                    if(loadingRegistryTail_)loadingRegistryTail_->next=node;
                    else loadingRegistryHead_=node;
                    pending_->loadingRegistryPublished_=true;
                    ++loadingRegistryCount_; //282078 includes allocationNULL
                    if(!node)return stop(S::allocation,0x28207C);
                    loadingRegistryTail_=node;
                    pending_->loadingSource_.reset();
                }
                entries_[i]=pending_;pending_.reset();
                publicationOrder_[publicationCount_++]=i;++d.published;
                if(loadingCall){
                    // Native retirement receipt only AFTER the real nonNULL
                    // registry node/link/count and actual owner publication.
                    // Failure preserves that published Entry and all reached
                    // source effects; never roll it back into pending custody.
                    const auto& published=entries_[i];
                    const auto* partial=published->loadingPartial_.get();
                    if(!published->loadingOuterAttempted_||published->loadingOuterKnownNull_||
                       !partial||!partial->outerAllocated||!partial->namePublished||!partial->name04||
                       !partial->glyphSlotsConstructed||!partial->sourceReadsComplete||
                       !partial->materialNameWritten||partial->materialLookupPending||!partial->material408Stored||
                       !partial->populationEntered||!partial->populationReturned||
                       !partial->directoryRestored||!partial->celClosed||published->loadingDirectory_||
                       !published->loadingRegistryPublished_||!published->loadingRegistryNode_||
                       published->loadingRegistryNode_->font!=published.get()||
                       loadingRegistryTail_!=published->loadingRegistryNode_.get()||
                       !publicationCount_||publicationOrder_[publicationCount_-1]!=i||
                       !published->loadingTextureRetained_||published->texture408_<0||
                       !currentResource281F90(*domain_,*published))
                        return stop(S::phase,0x2823F4);
                    // Source independently requires the genuine successful
                    // CEL-close history and exact closed/associated PST record,
                    // same root/generation/row/resource. Weak Entry/resource
                    // leases prevent a Source->Entry->Resource->Source cycle.
                    if(!loadingSource->recordColdRegistry2823EC(published,published->cel_,published->pst_,
                        published->texture_,d.texture))
                        return stop(S::source,d.texture.sourcePC?d.texture.sourcePC:0x2823F4);
                }
            }
            d.sourceEffectsCommitted=committed;return true;
        }catch(...){return stop(S::allocation,loadingCall?d.sourcePC:0x2824D0);}
    }
};
}
