#pragma once
#include "original_ui_main_entry50kg.h"
#include <array>
#include <cstring>

// Native cold cache ownership only. These four genuine ScreenMT row refs
// ensure that the later original PositionStats construction finds populated
// canonical objects. This is NOT an original PositionStats/Entity identity,
// renderer lease, guest allocation, GPU readiness flag or draw capability.
namespace original_position_stats_prestage50LA {
namespace a=rr::assets;
struct Diagnostic {
    const char* failure=nullptr;
    std::size_t source=0;
    a::OriginalStandardArchiveProvider50LA::Diagnostic provider;
    a::OriginalStandardMaterialDiagnostic50LA material;
    a::OriginalUiPageDiagnostic50KG parse;
    a::OriginalApplicationFontDiagnostic50LA font;
};
class Owner final {
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const a::OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<const a::OriginalUiArchive50KG> overlays_;
    std::shared_ptr<const a::OriginalApplicationFonts50LA::Entry> font_;
    std::array<std::int32_t,4> rows_{{-1,-1,-1,-1}};
    std::array<bool,4> retained_{};
    bool attempted_=false,complete_=false,closed_=false;
    inline static constexpr std::array<const char*,4> names_{{
        "ui\\textures\\stats","ui\\textures\\maptop",
        "font\\eurostileboldbold13ingame","ui\\textures\\RadarOverlay"}};
    static bool fail(Diagnostic& d,const char* why)noexcept{d.failure=why;return false;}
public:
    Owner()noexcept=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    // Also covers an exception before the root's closeWindow is installed.
    // A pinned/in-flight table is never discarded on failed retirement.
    ~Owner(){if(!close())std::terminate();}

    // Call only in the cold root, after authentic UI source staging and before
    // RA captures provider revision/collector or any renderer is created.
    // The context is minted by the SAME Application's actual live Video.
    bool stage(const original_main_entry50KG::Application& application,
        const std::filesystem::path& originalRoot,
        std::shared_ptr<const a::OriginalStandardMaterialContext50LA> context,
        Diagnostic& d)noexcept {
        d={};
        if(attempted_||closed_)return fail(d,"PositionStats preload is single-attempt");
        attempted_=true;
        try {
            if(!application.enabled||!context||!context->live()||
               context->materialApplication()!=application.materialApplication)
                return fail(d,"PositionStats preload requires the actual live Application construction context");
            domain_=application.standardMaterialDomain();context_=std::move(context);
            if(!domain_||!domain_->prepared()||
               !domain_->materials382B38()->screenApplicationMatches(*application.materialApplication))
                return fail(d,"PositionStats preload canonical Application table mismatch");
            overlays_=original_main_entry50KG::readArchive(originalRoot,a::OriginalUiArchiveId50KG::overlays);
            const auto provider=domain_->archiveProvider3E1A70();
            if(!provider||!provider->mountAuthenticatedUi(overlays_,d.provider)||
               !provider->sealNativeMounts(d.provider))
                return fail(d,"PositionStats preload authenticated OVERLAYS mount rejected");
            std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials;
            for(std::size_t i=0;i<names_.size();++i){
                d.source=i;const auto member=overlays_->find(std::string(names_[i])+".psm");
                if(!member.identity||!member.data||member.size!=member.identity->bytes)
                    return fail(d,"PositionStats preload original PSM missing");
                auto source=std::make_shared<a::OriginalStandardMaterialSource50LA>();
                if(!source->prepare(member.identity->name,
                    std::vector<std::uint8_t>(member.data,member.data+member.size),d.parse))
                    return fail(d,"PositionStats preload original PSM rejected");
                materials.push_back(std::move(source));
            }
            std::vector<a::OriginalStandardTextureSourceInput50LA> textures;
            for(const char* path:{"ui\\textures\\stats.pst","font\\eurostileboldbold13ingame.pst"}){
                const auto member=overlays_->find(path);
                if(!member.identity||!member.data||member.size!=member.identity->bytes)
                    return fail(d,"PositionStats preload original PST missing");
                textures.push_back({member.identity->name,
                    std::vector<std::uint8_t>(member.data,member.data+member.size)});
            }
            if(!domain_->appendOriginalSources(materials,textures,d.material))
                return fail(d,"PositionStats preload canonical source append rejected");
            const auto table=domain_->materials382B38();
            for(std::size_t i=0;i<names_.size();++i){
                d.source=i;
                if(!table->find298C80(names_[i],rows_[i],d.material)||
                   !table->retainScreen14D590(rows_[i],d.material))
                    return fail(d,"PositionStats preload ScreenMT reference rejected; partial refs retained");
                retained_[i]=true;
            }
            if(!application.loadPositionStatsFont190E64(overlays_,d.font))
                return fail(d,"PositionStats preload original font load rejected; partial refs retained");
            font_=application.fonts328()->find282210(a::OriginalApplicationFonts50LA::Name::eurostile13);
            // Retain before RA constructs its world/roster/particle owners.
            // Their single final whole-table population also resolves these
            // positive-ref HUD rows. Publishing a collector here would close
            // that genuine cold-construction phase prematurely.
            complete_=true;
            return true; // Source/reference stage only; verify after RA population.
        }catch(...){return fail(d,"PositionStats preload exception; partial refs retained");}
    }

    // Read-only check, also usable after RA's final population and after full
    // PositionStats construction. It never rebuilds or refreshes a stale seal.
    bool verify(Diagnostic& d)const noexcept {
        d={};
        try {
        if(!complete_||closed_||!domain_||!context_||!context_->live()||
           !font_||!font_->metrics().decoded()||!font_->texture281F90()||
           font_->celIdentity()!=overlays_->find("font\\eurostileboldbold13ingame.cel").identity||
           font_->atlasIdentity()!=overlays_->find("font\\eurostileboldbold13ingame.pst").identity)
            return fail(d,"PositionStats preload is incomplete or no longer live");
        const auto table=domain_->materials382B38();
        if(!table->coldCollectorCurrent(domain_->coldCollector14D440()))
            return fail(d,"PositionStats preload canonical collector is not current");
        const auto* screen=context_->materialApplication()->templates().get(a::OriginalUiTemplate50KG::screenMt);
        for(std::size_t i=0;i<names_.size();++i){
            d.source=i;a::OriginalStandardMaterialRowView50LA row;
            if(!retained_[i]||!table->row(rows_[i],row)||!row.references08||*row.references08<=0||
               row.associated30!=screen||!row.object04||!row.object04->constructed29B550()||
               !row.object04->source()||!row.object04->source()->originalBytes()||
               row.object04->textures383400()!=domain_->textures383400().get())
                return fail(d,"PositionStats preload retained canonical row changed");
            // No allocating path construction in this noexcept observation.
            const auto* memberName=row.object04->source()->member();
            if(!memberName)return fail(d,"PositionStats preload source member missing");
            const auto original=overlays_->find(*memberName);
            const auto& bytes=*row.object04->source()->originalBytes();
            if(!original.identity||!original.data||original.size!=bytes.size()||
               (original.size&&std::memcmp(original.data,bytes.data(),original.size)))
                return fail(d,"PositionStats preload canonical original bytes changed");
        }
        return true;
        }catch(...){return fail(d,"PositionStats preload verification exception");}
    }

    // Root orders this AFTER full HUD drains/retirement, camera/RA retirement,
    // renderer close and all borrowers of the RA collector. A rejected release
    // preserves the remaining refs; retry only after the real pin is retired.
    bool close()noexcept {
        if(closed_)return true;
        for(std::size_t i=retained_.size();i>0;--i)if(retained_[i-1]){
            if(!domain_||!domain_->materials382B38()->release14D500(rows_[i-1]))return false;
            retained_[i-1]=false;
        }
        font_.reset();overlays_.reset();context_.reset();domain_.reset();
        complete_=false;closed_=true;return true;
    }
};
}
