#pragma once
#include "original_standard_material_domain50la.h"
#include "original_standard_material_context50la.h"
#include "original_ui_font50kg.h"
#include <array>
#include <cstring>
#include <exception>
#include <memory>
#include <string>

namespace original_main_entry50KG { struct Application; }
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
    friend class rr::game::OriginalPauseStorage50LA;
public:
    enum class Name : std::uint8_t { eurostile13=0, denmark16=1, eurostileItalic26=2 };
    class Entry final {
        friend class OriginalApplicationFonts50LA;
        friend class rr::game::OriginalPauseStorage50LA;
        std::shared_ptr<OriginalStandardMaterialDomain50LA> domain_;
        std::shared_ptr<const OriginalStandardMaterialContext50LA> context_;
        std::shared_ptr<const OriginalUiArchive50KG> archive_;
        OriginalUiFont50KG metrics_;
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
    std::array<std::shared_ptr<const Entry>,3> entries_{};
    std::array<std::size_t,3> publicationOrder_{};
    std::size_t publicationCount_=0;
    // Unpublished acquired state survives failure until owner cleanup;
    // previously appended entries and table population effects are retained.
    std::shared_ptr<Entry> pending_;
    bool terminal_=false;
    OriginalApplicationFontDiagnostic50LA terminalDiagnostic_{};
    OriginalApplicationFonts50LA(std::shared_ptr<OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const OriginalStandardMaterialContext50LA> context)
        :domain_(std::move(domain)),context_(std::move(context)){}
    static void release(Entry& e)noexcept {
        if(e.texture408_>=0){
            if(!e.domain_||!e.domain_->textures383400()||
               !e.domain_->textures383400()->release29E400(e.texture408_))std::terminate();
            e.texture408_=-1;e.texture_=nullptr;
        }
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
        pending_.reset();
        for(std::size_t n=0;n<publicationCount_;++n)entries_[publicationOrder_[n]].reset();
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
    //Only the fixed private source wrappers select names/count. The existing
    //canonical acquisition, whole-table population and failure body is shared.
    template<std::size_t Count>
    bool loadNames282430(std::shared_ptr<const OriginalUiArchive50KG> archive,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
        const std::array<Name,Count>& requested,std::uint32_t sourceCaller,
        OriginalApplicationFontDiagnostic50LA& d)noexcept {
        d={};
        if(terminal_){d=terminalDiagnostic_;return false;}
        using S=OriginalApplicationFontStatus50LA;
        if(fontDirectory36B420!="font\\")return fail(d,S::source,0x17C848);
        if(!domain_||!domain_->prepared()||!context_||!context_->live()||
           domain_->application_!=context_->materialApplication()||pending_||
           domain_->textures383400()->populationTerminal()||
           domain_->materials382B38()->populationTerminal()||
           domain_->materials382B38()->demandTerminal())return fail(d,S::phase,sourceCaller);
        if(!archive||!archive->loaded()||archive->identity()!=
           originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,S::source,sourceCaller);
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed()||!domain_->resolver3E1A70()->prepared())
            return fail(d,S::source,0x281880);
        bool committed=false;
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
            for(const auto& entry:entries_){
                if(!entry)continue;
                if(entry->domain_!=domain_||entry->context_!=context_||
                    !entry->archive_||!entry->archive_->loaded()||
                    entry->archive_->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays)||
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
            static const char* names[]={"eurostileboldbold13ingame","denmarkbold16ingame","eurostileboldbolditalic26ingame"};
            for(const auto name:requested){
                const auto i=static_cast<std::size_t>(name);
                d.font=i;
                if(entries_[i])continue; //2824C8, reload0: preserve object and handle.
                const std::string base=std::string("font\\")+names[i];
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
                pending_=std::shared_ptr<Entry>(new Entry(domain_,context_,archive));
                pending_->cel_=cel;pending_->pst_=pst;
                if(!decodeOriginalUiCel50KG(cel.data,cel.size,pending_->metrics_,d.cel))
                    return stop(S::font,0x2817D0);
                //281EC0..281F44 scopes the real application directory only
                //after CEL decoding. Native early returns also restore it.
                struct DirectoryScope {
                    std::string& actual;std::string previous;bool active=true;
                    DirectoryScope(std::string& a,const std::string& selected)
                        :actual(a),previous(a){actual=selected;}
                    void restore()noexcept{if(active){actual.swap(previous);active=false;}}
                    ~DirectoryScope(){restore();}
                } directory(actualDirectory36B970,fontDirectory36B420);
                const std::string textureName=actualDirectory36B970+names[i];
                auto table=domain_->textures383400();
                if(!table->bindMaterialContext(context_,d.texture))
                    return stop(S::texture,0x281F14);
                if(!table->acquire299330AndRetain298A30(textureName,pending_->texture408_,d.texture))
                    return stop(S::texture,0x281F28);
                committed=true;d.sourceEffectsCommitted=true;
                // Original281F34 scans the entire canonical texture table.
                // Do not restrict it to this row or roll back earlier rows.
                // The original full scan skips positive published rows;
                // only null pending rows acquire CPU resources. It does not
                // rebuild material collectors or touch native GL publication.
                if(!existingEntriesIntact())return stop(S::phase,0x281F34);
                if(!table->populate29E340(domain_->resolver3E1A70(),context_,d.texture))
                    return stop(S::texture,0x281F34);
                if(!existingEntriesIntact())return stop(S::phase,0x281F90);
                directory.restore(); //281F44 precedes manager publication282510.
                OriginalStandardTextureResource50LA* resource=nullptr;
                OriginalStandardTextureDiagnostic50LA resolved;
                if(!table->resolve298E60(pending_->texture408_,resource,resolved)||!resource){
                    d.texture=resolved;
                    return stop(S::texture,0x281F90);
                }
                pending_->texture_=resource;
                entries_[i]=pending_;pending_.reset();
                publicationOrder_[publicationCount_++]=i;++d.published;
            }
            d.sourceEffectsCommitted=committed;return true;
        }catch(...){return stop(S::allocation,0x2824D0);}
    }
};
}
