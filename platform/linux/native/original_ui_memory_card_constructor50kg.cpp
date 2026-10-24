#include "original_ui_memory_card_constructor50kg.h"
#include "../../../src/assets/original_ui_pages50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include <exception>
#include <cstring>
#include <limits>
namespace rr::platform {
OriginalUiMemoryCardConstructor50KG::~OriginalUiMemoryCardConstructor50KG()noexcept {
    if(entity_.reference00||constructedQuads_!=retiredQuads_||displayPinned_||
       phase_==Phase::detachedConstructed||phase_==Phase::retiring)std::terminate();
}
bool OriginalUiMemoryCardConstructor50KG::inputsCurrent()const noexcept {
    return root_&&root_->memoryCardConstructorOwner_.get()==this&&scope_&&scope_->issuer_==root_&&root_->sharedUiScopeOwner_==scope_&&
        scope_->live().okay()&&domain_&&domain_==root_->standardMaterialDomainOwner_&&domain_->prepared()&&
        context_&&context_==root_->materialContextOwner_&&context_->live()&&
        video_&&video_==root_->videoOwner_&&video_->originalMain&&video_->packetZMaximum==0xffffff&&
        context_->applicationAnimationClock().sameOwner(root_->animationClock())&&
        root_->materialApplication&&context_->materialApplication()==root_->materialApplication&&
        domain_->materials382B38()->screenApplicationMatches(*root_->materialApplication);
}
bool OriginalUiMemoryCardConstructor50KG::constructDetached1FEC80(const Root& root,
    std::shared_ptr<OriginalUiMemoryCardConstructor50KG>& output,Diagnostic& d)noexcept {
    d={};if(output||root.memoryCardConstructorOwner_){d.sourcePC=0x1D6670;return false;}
    try {
        auto next=std::shared_ptr<OriginalUiMemoryCardConstructor50KG>(new OriginalUiMemoryCardConstructor50KG);
        next->root_=&root;next->scope_=root.sharedUiScopeOwner_;
        next->domain_=root.standardMaterialDomainOwner_;next->context_=root.materialContextOwner_;next->video_=root.videoOwner_;
        next->phase_=Phase::bound;
        // Establish real cleanup custody BEFORE any counted source effect.
        const auto display=root.raceDisplayOwner50LA_.lock();
        if(!display||display->application_!=&root||display->video_!=next->video_||
           !display->frontendOwnerCurrent50KG()||!display->canRetain()){
            d.sourcePC=0x1D6670;return false;
        }
        next->display_=display;display->pin();next->displayPinned_=true;
        root.memoryCardConstructorOwner_=next;
        const auto rejectBeforeCount=[&]()noexcept {
            display->unpin();next->displayPinned_=false;next->display_.reset();
            root.memoryCardConstructorOwner_.reset();next->phase_=Phase::empty;
        };
        if(!next->inputsCurrent()){rejectBeforeCount();d.sourcePC=0x1FEC80;return false;}
        d.entity=next->scope_->constructDetachedMemoryCard1FEC80(*next,next->entity_);
        if(!d.entity.okay()){rejectBeforeCount();d.sourcePC=d.entity.sourcePC;return false;}
        // All remaining constructor stores are nonfallible. No loader, callback
        // or allocation occurs after the real Entity count/enrollment.
        next->vtable_=0x3D0990;
        constexpr std::array<std::uint32_t,7> offsets{{0x40,0xA0,0x100,0x160,0x1C0,0x220,0x280}};
        for(std::size_t i=0;i<offsets.size();++i){
            if(!next->quads_[i].constructMemoryCardPrefix2DEF40(offsets[i]))std::terminate();
            ++next->constructedQuads_;
        }
        next->ids3BC_.fill(-1);next->timer398_=0;next->completed3A4_=false;next->animate3B0_=true;
        next->phase_=Phase::detachedConstructed;
        output=std::move(next);d.sourceEffectsCommitted=true;d.sourcePC=0x1FEE38;return true;
    }catch(...){d.sourcePC=0x1D6670;return false;}
}
bool OriginalUiMemoryCardConstructor50KG::resourcePrefixCurrent()const noexcept {
    return inputsCurrent()&&(root_->materialDirectory36B970_=="ui\\textures\\"||root_->materialDirectory36B970_=="ui\\textures")&&
        phase_==Phase::detachedConstructed&&display_&&displayPinned_&&
        display_->application_==root_&&display_->video_==video_&&!display_->retiring_&&
        display_->memoryCardConstructorDrainCurrent50KG()&&
        scope_->detachedMemoryCardCurrent(*this,entity_,false);
}
bool OriginalUiMemoryCardConstructor50KG::retainOptionsSources(
    const std::shared_ptr<const assets::OriginalUiArchive50KG>& archive)noexcept {
    std::int32_t row=-1;
    const auto table=domain_?domain_->materials382B38():nullptr;
    if(!table||!firstIconOccurrenceCurrent(*table,row)||!archive||!archive->loaded()||
       archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::options))return false;
    if(optionsSource_)return optionsSource_==archive;
    try {
        // Reuse the actual source parsers, never caller-supplied page values or
        // the cold bank's readiness flag as canonical issuer authority.
        const auto widgets=archive->member(1),memoryCard=archive->member(46);
        if(widgets.identity!=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::options,1)||
           memoryCard.identity!=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::options,46)||
           !widgets.data||widgets.size!=widgets.identity->bytes||
           !memoryCard.data||memoryCard.size!=memoryCard.identity->bytes)return false;
        assets::OriginalUiDat50KG widgetDat,pageDat;assets::OriginalUiPage50KG page;
        assets::OriginalUiPageDiagnostic50KG diagnostic;
        if(!assets::readOriginalUiDat50KG(*archive,"UI/UIOption/Widgets.Dat",widgetDat,diagnostic,
               assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
           !assets::readOriginalUiDat50KG(*archive,"UI/UIOption/memcard.dat",pageDat,diagnostic)||
           !assets::readOriginalUiPage50KG(pageDat,widgetDat,page,diagnostic))return false;
        if(page.identity!=memoryCard.identity||page.pageNo!=5||page.widgetCount!=357||
           page.controls.size()!=1||page.groups.size()!=1||page.controls[0].widgets.size()!=2||
           page.controls[0].widgets[0]!=2||page.controls[0].widgets[1]!=16||
           page.groups[0].controls.size()!=1||page.groups[0].controls[0]!=0||!page.groups[0].keepEnabled||
           !page.groups[0].associations.empty()||!page.hasPopup||
           page.popupWidgets!=std::array<std::int32_t,9>{{23,43,41,42,30,31,40,44,45}}||
           page.openSize!=69||page.openTime!=.5f||page.start!="Open")return false;
        // Final actual-owner check after parsing/allocation and before the sole
        // source-custody store. No callbacks, source refs or material stores.
        if(!firstIconOccurrenceCurrent(*table,row)||optionsSource_)return false;
        optionsSource_=archive;return true;
    }catch(...){return false;}
}
bool OriginalUiMemoryCardConstructor50KG::firstIconOccurrenceCurrent(
    const assets::OriginalStandardMaterialTable50LA& table,std::int32_t& row)const noexcept {
    row=-1;if(!resourcePrefixCurrent()||!retainedLoadRoles_||constructedQuads_!=7||retiredQuads_||
       domain_->materials382B38().get()!=&table)return false;
    const auto& role=loadRoles_[0];const auto& quad=quads_[role.constructionIndex];
    if(!quad.memoryCardPrefixOnly_||quad.offset!=role.offset||quad.garage_||!firstIconPacketStateCurrent()||!quad.constructed||
       !quad.material54||quad.material54!=quad.primitiveOwner_.get()||
       !quad.material54->materialStored||!quad.material54->materialRetained||
       quad.material54->type0C!=15||(quad.material54->flags0F&1u)||
       quad.material54->domain!=domain_||quad.material54->context!=context_||
       quad.domain_!=domain_||quad.context_!=context_||quad.video_!=video_)return false;
    row=quad.material54->material00;return row>=0;
}
bool OriginalUiMemoryCardConstructor50KG::firstIconPacketStateCurrent()const noexcept {
    return iconPacketStateCurrent(0);
}
bool OriginalUiMemoryCardConstructor50KG::iconPacketStateCurrent(std::size_t role)const noexcept {
    if(role>1)return false;
    const auto& q=quads_[loadRoles_[role].constructionIndex];
    if(q.nativeGarageFifoPins_||q.nativeMenuWarningPins_)return false;
    if(q.stage_==Quad::Stage::loaded){
        return q.pendingPC_==0&&q.packetRevision_==1&&q.resource58&&q.resource58==q.packetOwner_.get()&&
            q.material54&&q.material54->packet10==q.resource58&&q.dmaData4_==q.resource58+1&&
            reinterpret_cast<std::uintptr_t>(q.resource58)%64u==0;
    }
    const bool prefix=(q.stage_==Quad::Stage::materialRetained&&q.pendingPC_==0x2DF144)||
        (q.stage_==Quad::Stage::populated&&q.pendingPC_==0x2DF158)||
        (q.stage_==Quad::Stage::dimensionsStored&&q.pendingPC_==0x2DF1CC);
    return prefix&&!(role==0?firstIconPostLoadApplied_:trianglePostLoadApplied_)&&!q.resource58&&!q.packetOwner_&&!q.dmaData4_&&!q.packetRevision_&&
        (!q.material54||!q.material54->packet10);
}
bool OriginalUiMemoryCardConstructor50KG::completeFirstIconComponent1FF058(Diagnostic& d)noexcept {
    d={};const auto table=domain_?domain_->materials382B38():nullptr;std::int32_t row=-1;
    if(!table||!optionsSource_||!firstIconOccurrenceCurrent(*table,row)) {d.sourcePC=0x2DF158;return false;}
    auto& q=quads_[loadRoles_[0].constructionIndex];
    if(q.stage_!=Quad::Stage::loaded){
        if(!q.completeMemoryCardFirstIcon2DF158(*this,d.quad)){
            d.sourcePC=d.quad.sourcePC;d.sourceEffectsCommitted=d.quad.sourceEffectsCommitted;return false;
        }
        d.sourceEffectsCommitted=true;
    }
    if(!firstIconOccurrenceCurrent(*table,row)||!table->preflightMemoryCardFirstIconBinding(*this,d.quad.material)){
        d.sourcePC=d.quad.material.sourcePC?d.quad.material.sourcePC:0x1FF060;return false;
    }
    // Exact caller post-return stores1FF060..0FC. The packet remains its real
    // zero-position initialized allocation; dirty marking is not regeneration.
    if(!firstIconPostLoadApplied_){
        if(q.position10[0]!=514.f||q.position10[1]!=378.f){
            q.position10[0]=514.f;q.position10[1]=378.f;
            q.flags5CValue=std::uint8_t((q.flags5CValue&~1u)|1u);q.flags5CKnownMask|=1;
            if(q.uv30[3]!=0.f){q.flags5CValue=std::uint8_t((q.flags5CValue&~2u)|2u);q.flags5CKnownMask|=2;}
        }
        firstIconPostLoadApplied_=true;d.sourceEffectsCommitted=true;
    }
    if(q.position10[0]!=514.f||q.position10[1]!=378.f){d.sourcePC=0x1FF0AC;return false;}
    d.sourcePC=0x1FF100;return true; // Stop BEFORE next-role setup/call.
}
bool OriginalUiMemoryCardConstructor50KG::completeTriangleComponent1FF110(Diagnostic& d)noexcept {
    d={};const auto table=domain_?domain_->materials382B38():nullptr;std::int32_t row=-1;
    if(!table||!optionsSource_||!iconOccurrenceCurrent(1,*table,row)){d.sourcePC=0x2DF158;return false;}
    auto& q=quads_[loadRoles_[1].constructionIndex];
    if(q.stage_!=Quad::Stage::loaded){
        if(!q.completeMemoryCardIcon2DF158(*this,1,d.quad)){
            d.sourcePC=d.quad.sourcePC;d.sourceEffectsCommitted=d.quad.sourceEffectsCommitted;return false;
        }
        d.sourceEffectsCommitted=true;
    }
    if(!iconOccurrenceCurrent(1,*table,row)||!table->preflightMemoryCardIconBinding(*this,1,d.quad.material)){
        d.sourcePC=d.quad.material.sourcePC?d.quad.material.sourcePC:0x1FF118;return false;
    }
    // Actual caller1FF118..1FF1B4. The freshly initialized packet remains
    // at zero position: dirty bits request later source work, not regeneration.
    if(!trianglePostLoadApplied_){
        if(q.position10[0]!=427.f||q.position10[1]!=378.f){
            q.position10[0]=427.f;q.position10[1]=378.f;
            q.flags5CValue=std::uint8_t((q.flags5CValue&~1u)|1u);q.flags5CKnownMask|=1;
            if(q.uv30[3]!=0.f){q.flags5CValue=std::uint8_t((q.flags5CValue&~2u)|2u);q.flags5CKnownMask|=2;}
        }
        trianglePostLoadApplied_=true;d.sourceEffectsCommitted=true;
    }
    if(q.position10[0]!=427.f||q.position10[1]!=378.f){d.sourcePC=0x1FF164;return false;}
    d.sourcePC=0x1FF1B8;return true; // Stop BEFORE Square setup/call.
}
bool OriginalUiMemoryCardConstructor50KG::iconOccurrenceCurrent(std::size_t role,
    const assets::OriginalStandardMaterialTable50LA& table,std::int32_t& row)const noexcept {
    if(role==0)return firstIconOccurrenceCurrent(table,row);
    row=-1;
    if(role!=1||!resourcePrefixCurrent()||retainedLoadRoles_!=2||!loadRolePrefixCurrent()||
       domain_->materials382B38().get()!=&table)return false;
    row=quads_[loadRoles_[role].constructionIndex].material54->material00;return row>=0;
}
bool OriginalUiMemoryCardConstructor50KG::pendingFamilyOccurrenceCurrent(
    const assets::OriginalMemoryCardScreenFallback50KG& proof)const noexcept {
    std::int32_t row=-1;const auto table=domain_?domain_->materials382B38():nullptr;
    return proof.roleIndex_<=1&&table&&iconOccurrenceCurrent(proof.roleIndex_,*table,row)&&optionsSource_&&proof.options_==optionsSource_&&
        quads_[loadRoles_[proof.roleIndex_].constructionIndex].stage_==Quad::Stage::materialRetained&&
        !pendingFamilyRetired_&&proof.name_==loadRoles_[proof.roleIndex_].qualifiedName&&
        proof.constructionIndex_==loadRoles_[proof.roleIndex_].constructionIndex&&
        proof.componentOffset_==loadRoles_[proof.roleIndex_].offset&&proof.sourceCallerPC_==loadRoles_[proof.roleIndex_].callPC&&proof.table_==table.get()&&proof.row_==row&&
        proof.primitive_==quads_[loadRoles_[proof.roleIndex_].constructionIndex].material54&&
        proof.context_==context_&&proof.textures_==domain_->textures383400()&&
        proof.programs_==domain_->programs382B70()&&
        proof.provider_==domain_->archiveProvider3E1A70();
}
bool OriginalUiMemoryCardConstructor50KG::prepareFirstIconConstructionFamily(Diagnostic& d)noexcept {
    return prepareIconConstructionFamily(0,d);
}
bool OriginalUiMemoryCardConstructor50KG::prepareIconConstructionFamily(std::size_t role,Diagnostic& d)noexcept {
    d={};std::int32_t row=-1;const auto table=domain_?domain_->materials382B38():nullptr;
    if(role>1||!table||!iconOccurrenceCurrent(role,*table,row)||!optionsSource_||pendingFamilyRetired_||
       quads_[loadRoles_[role].constructionIndex].stage_!=Quad::Stage::materialRetained){d.sourcePC=0x29B638;return false;}
    try {
        auto proof=std::shared_ptr<assets::OriginalMemoryCardScreenFallback50KG>(new assets::OriginalMemoryCardScreenFallback50KG);
        proof->roleIndex_=role;proof->name_=loadRoles_[role].qualifiedName;
        proof->constructionIndex_=loadRoles_[role].constructionIndex;proof->componentOffset_=loadRoles_[role].offset;
        proof->sourceCallerPC_=loadRoles_[role].callPC;
        proof->issuer_=root_->memoryCardConstructorOwner_;proof->table_=table.get();proof->row_=row;
        proof->primitive_=quads_[loadRoles_[role].constructionIndex].material54;
        proof->options_=optionsSource_;proof->context_=context_;proof->textures_=domain_->textures383400();
        proof->provider_=domain_->archiveProvider3E1A70();proof->programs_=domain_->programs382B70();
        const bool result=table->bindMemoryCardPendingAfterRetain(*this,std::move(proof),d.quad.material);
        d.sourcePC=result?0x2DF144:d.quad.material.sourcePC;d.resourcePopulationPending=result;return result;
    }catch(...){d.sourcePC=0x29B638;return false;}
}
bool OriginalUiMemoryCardConstructor50KG::populateFirstIconResources2DF144(Diagnostic& d)noexcept {
    return populateIconResources2DF144(0,d);
}
bool OriginalUiMemoryCardConstructor50KG::populateTriangleResources2DF144(Diagnostic& d)noexcept {
    return populateIconResources2DF144(1,d);
}
bool OriginalUiMemoryCardConstructor50KG::populateIconResources2DF144(std::size_t role,Diagnostic& d)noexcept {
    d={};if(role>1){d.sourcePC=0x2DF144;return false;}
    auto& progress=iconPopulation_[role];d.sourceInputsAdmitted=progress.inputsAdmitted;
    const auto table=domain_?domain_->materials382B38():nullptr;std::int32_t row=-1;
    if(!table||!iconOccurrenceCurrent(role,*table,row)||!optionsSource_||pendingFamilyRetired_){d.sourcePC=0x2DF144;return false;}
    auto& quad=quads_[loadRoles_[role].constructionIndex];
    if(quad.stage_!=Quad::Stage::materialRetained){
        const bool okay=table->preflightMemoryCardIconBinding(*this,role,d.quad.material);
        d.sourcePC=okay?((role==0&&firstIconPostLoadApplied_)?0x1FF100:
            (role==1&&trianglePostLoadApplied_)?0x1FF1B8:quad.stage_==Quad::Stage::loaded?0x2DF54C:
            quad.stage_==Quad::Stage::dimensionsStored?0x2DF1CC:0x2DF158):d.quad.material.sourcePC;
        d.sourceInputsAdmitted=progress.inputsAdmitted;return okay;
    }
    if(progress.attempted){d.sourcePC=progress.stopPC;d.sourceInputsAdmitted=progress.inputsAdmitted;return false;}
    using Provider=assets::OriginalStandardArchiveProvider50LA;
    const auto provider=domain_->archiveProvider3E1A70();
    bool firstPending=false,noTexturePending=false;
    if(!provider||!provider->sealed()||!table->preflightMemoryCardPopulation(*this,firstPending,noTexturePending,d.quad.material,role)){
        d.sourcePC=d.quad.material.sourcePC?d.quad.material.sourcePC:0x2DF144;return false;
    }
    const auto revision=provider->revision();
    try {
        std::vector<std::shared_ptr<const assets::OriginalStandardMaterialSource50LA>> sources;
        std::vector<assets::OriginalStandardTextureSourceInput50LA> textures;
        if(noTexturePending){
            Provider::MemberView member;
            if(provider->lookup132820("ui\\textures\\NoTexture.psm",member)!=Provider::Lookup::present||
               !member.member||!member.data||!member.size){d.sourcePC=0x29B628;return false;}
            assets::OriginalUiEncodedArchiveView50KG input;
            if(!provider->encodedUiInput(member.mount,input)||input.providerIdentity()!=provider.get()||!input.identity()){
                d.sourcePC=0x132820;return false;
            }
            const auto* identity=assets::originalUiMemberIdentity50KG(input.identity()->id,member.ordinal);
            if(!identity||!identity->sha256||identity->bytes!=491||member.size!=identity->bytes||
               !Provider::sameName(identity->name,*member.member)||
               !Provider::sameName(*member.member,"ui\\textures\\NoTexture.psm")||
               std::strcmp(identity->sha256,"f88625f9aaaa564646d77f3a95d47e1d60f94a9bf6c3052beda86bbe00bd5b8a")){
                d.sourcePC=0x29B628;return false;
            }
            auto source=std::make_shared<assets::OriginalStandardMaterialSource50LA>();
            std::vector<std::uint8_t> bytes(member.data,member.data+member.size);
            if(!source->prepare(*member.member,bytes,d.quad.material.document)||source->section("Prologue")){
                d.sourcePC=0x29B6D8;return false;
            }
            const auto* general=source->section("General");const auto* pass=source->section("ShaderPass_1");
            std::int32_t count=0,maps=1;
            if(!general||!pass||!assets::originalUiInteger50KG(*general,"TotalShaderPasses",0,count,d.quad.material.document,0x29B718)||count!=1||
               !assets::originalUiInteger50KG(*pass,"totalTextureMaps",1,maps,d.quad.material.document,0x247214)||maps!=0){
                d.sourcePC=0x247214;return false;
            }
            const auto* program=pass->find("microProgramName");
            if(program&&!Provider::sameName(program->text,"ScreenObject")){d.sourcePC=0x2471BC;return false;}
            sources.push_back(std::move(source));
        }
        if(firstPending){
            const std::string materialName=loadRoles_[role].qualifiedName;
            Provider::MemberView psm,pst;
            if(provider->lookup132820(materialName+".psm",psm)!=Provider::Lookup::absent||
               provider->lookup132820(materialName+".pst",pst)!=Provider::Lookup::present||
               !pst.member||!pst.data||!pst.size){d.sourcePC=0x29B638;return false;}
            assets::OriginalUiEncodedArchiveView50KG input;
            if(!provider->encodedUiInput(pst.mount,input)||input.providerIdentity()!=provider.get()||
               (input.identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::controllerPopup)&&
                input.identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::overlays))){d.sourcePC=0x132820;return false;}
            const auto* member=assets::originalUiMemberIdentity50KG(input.identity()->id,pst.ordinal);
            if(!member||member->bytes!=pst.size||!Provider::sameName(member->name,*pst.member)||
               !Provider::sameName(*pst.member,materialName+".pst")){d.sourcePC=0x132820;return false;}
            textures.push_back({*pst.member,std::vector<std::uint8_t>(pst.data,pst.data+pst.size)});
        }
        bool stillFirst=false,stillNoTexture=false;
        if(!table->preflightMemoryCardPopulation(*this,stillFirst,stillNoTexture,d.quad.material,role)||
           stillFirst!=firstPending||stillNoTexture!=noTexturePending||provider->revision()!=revision||!provider->sealed()){
            d.sourcePC=0x2DF144;return false;
        }
        if(firstPending){Diagnostic family;if(!prepareIconConstructionFamily(role,family)){d=family;d.sourceInputsAdmitted=progress.inputsAdmitted;return false;}}
        // Real corpus admission owns its own immutable/provider receipts. It
        // may retain input files, but does not fabricate material publication.
        if(!domain_->appendOriginalSources(sources,textures,d.quad.material)){
            d.sourcePC=d.quad.material.sourcePC;return false;
        }
        progress.inputsAdmitted=true;d.sourceInputsAdmitted=true;
        if(!table->preflightMemoryCardPopulation(*this,stillFirst,stillNoTexture,d.quad.material,role)||
           stillFirst!=firstPending||stillNoTexture!=noTexturePending||provider->revision()!=revision||!provider->sealed()){
            d.sourcePC=0x2DF144;return false;
        }
        progress.attempted=true;progress.stopPC=0x2DF144;
        // Actual full ascending scans, exactly14D370 then29E340. No collector
        // rebuild, per-row shortcut, borrowed Garage proof or source re-retain.
        if(!domain_->populateMemoryCardResources2DF144(context_,d.quad.material)){
            d.sourceEffectsCommitted=d.quad.material.sourceEffectsCommitted;
            d.sourcePC=d.quad.material.status==assets::OriginalStandardMaterialStatus50LA::texture&&d.quad.material.texture.sourcePC?
                d.quad.material.texture.sourcePC:(d.quad.material.sourcePC?d.quad.material.sourcePC:0x2DF144);
            progress.stopPC=d.sourcePC;return false;
        }
        d.sourceEffectsCommitted=d.quad.material.sourceEffectsCommitted;
        if(!table->preflightMemoryCardIconBinding(*this,role,d.quad.material)){
            d.sourcePC=d.quad.material.sourcePC;progress.stopPC=d.sourcePC;return false;
        }
        quad.stage_=Quad::Stage::populated;quad.pendingPC_=0x2DF158;
        progress.stopPC=0x2DF158;d.sourcePC=0x2DF158;return true;
    }catch(...){d.sourcePC=progress.attempted?progress.stopPC:0x2DF144;
        d.sourceInputsAdmitted=progress.inputsAdmitted;return false;}
}
bool OriginalUiMemoryCardConstructor50KG::firstIconRetirementIdentity(
    const assets::OriginalStandardMaterialTable50LA& table,std::int32_t& row)const noexcept {
    return iconRetirementIdentity(0,table,row);
}
bool OriginalUiMemoryCardConstructor50KG::iconRetirementIdentity(std::size_t role,
    const assets::OriginalStandardMaterialTable50LA& table,std::int32_t& row)const noexcept {
    row=-1;if(role>1||role>=retainedLoadRoles_||!inputsCurrent()||phase_!=Phase::retiring||!retainedLoadRoles_||retiredQuads_||
       !display_||!displayPinned_||display_->application_!=root_||display_->video_!=video_||
       !display_->memoryCardConstructorDrainCurrent50KG()||domain_->materials382B38().get()!=&table||
       !scope_->detachedMemoryCardCurrent(*this,entity_,true))return false;
    const auto& quad=quads_[loadRoles_[role].constructionIndex];
    if(!retainedRoleCurrent(role)||!quad.material54||quad.material54!=quad.primitiveOwner_.get()||
       !quad.material54->materialRetained||quad.material54->domain!=domain_||quad.material54->context!=context_)return false;
    row=quad.material54->material00;return row>=0;
}
bool OriginalUiMemoryCardConstructor50KG::preflightExistingFirstIconBinding(Diagnostic& d)const noexcept {
    d={};if(!inputsCurrent()){d.sourcePC=0x2DF158;return false;}
    const auto table=domain_->materials382B38();
    const bool result=table&&table->preflightMemoryCardFirstIconBinding(*this,d.quad.material);
    d.resourcePopulationPending=result&&quads_[loadRoles_[0].constructionIndex].stage_==Quad::Stage::materialRetained;
    const auto stage=quads_[loadRoles_[0].constructionIndex].stage_;
    d.sourcePC=result?(firstIconPostLoadApplied_?0x1FF100:stage==Quad::Stage::loaded?0x2DF54C:
        stage==Quad::Stage::dimensionsStored?0x2DF1CC:0x2DF158):d.quad.material.sourcePC;return result;
}
bool OriginalUiMemoryCardConstructor50KG::retainFirstIconPrefix1FF058(Diagnostic& d)noexcept {
    d={};
    if(!resourcePrefixCurrent()){d.sourcePC=0x1FF058;return false;}
    // True means this component's reference prefix is represented. It does
    // not claim the containing caller ran; later component progress is
    // reported by its exact retained cursor, without replaying the retain.
    if(retainedLoadRoles_){std::int32_t row=-1;
        if(!firstIconOccurrenceCurrent(*domain_->materials382B38(),row)){d.sourcePC=0x1FF058;return false;}
        const auto stage=quads_[loadRoles_[0].constructionIndex].stage_;d.sourceEffectsCommitted=true;
        d.resourcePopulationPending=stage==Quad::Stage::materialRetained;
        d.sourcePC=d.resourcePopulationPending?0x2DF144:firstIconPostLoadApplied_?0x1FF100:
            stage==Quad::Stage::loaded?0x2DF54C:stage==Quad::Stage::dimensionsStored?0x2DF1CC:0x2DF158;return true;}
    const bool result=quads_[loadRoles_[0].constructionIndex].retainMemoryCardFirstIcon2465B0(*this,d.quad);
    d.sourceEffectsCommitted=d.quad.sourceEffectsCommitted;d.resourcePopulationPending=result;d.sourcePC=d.quad.sourcePC;return result;
}
bool OriginalUiMemoryCardConstructor50KG::retainedRoleCurrent(std::size_t roleIndex)const noexcept {
    if(roleIndex>=retainedLoadRoles_||roleIndex>=loadRoles_.size())return false;
    const auto& role=loadRoles_[roleIndex];const auto& q=quads_[role.constructionIndex];
    if(!q.memoryCardPrefixOnly_||q.offset!=role.offset||!q.constructed||q.garage_||
       !q.material54||q.material54!=q.primitiveOwner_.get()||
       !q.material54->materialStored||!q.material54->materialRetained||
       q.material54->material00<0||q.material54->type0C!=15||(q.material54->flags0F&1u)||
       q.material54->domain!=domain_||q.material54->context!=context_||
       q.domain_!=domain_||q.context_!=context_||q.video_!=video_||
       q.nativeGarageFifoPins_||q.nativeMenuWarningPins_)return false;
    return iconPacketStateCurrent(roleIndex);
}
bool OriginalUiMemoryCardConstructor50KG::loadRolePrefixCurrent()const noexcept {
    // Only X/Triangle component prefixes are represented here. No later
    // retain is admitted even after Triangle completion; no scalar broadening.
    if(retainedLoadRoles_>2||constructedQuads_!=7||retiredQuads_)return false;
    if(retainedLoadRoles_>1&&(!firstIconPostLoadApplied_||
       quads_[loadRoles_[0].constructionIndex].stage_!=Quad::Stage::loaded||
       quads_[loadRoles_[0].constructionIndex].position10[0]!=514.f||
       quads_[loadRoles_[0].constructionIndex].position10[1]!=378.f))return false;
    for(std::size_t r=0;r<loadRoles_.size();++r){
        const auto& role=loadRoles_[r];const auto& q=quads_[role.constructionIndex];
        if(r<retainedLoadRoles_){if(!retainedRoleCurrent(r))return false;}
        else if(!q.memoryCardPrefixOnly_||q.offset!=role.offset||!q.constructed||q.garage_||
            q.stage_!=Quad::Stage::constructed||q.material54||q.primitiveOwner_||q.resource58||
            q.packetOwner_||q.dmaData4_||q.packetRevision_||q.nativeGarageFifoPins_||q.nativeMenuWarningPins_)return false;
    }
    return true;
}
bool OriginalUiMemoryCardConstructor50KG::nextRetainRole(std::size_t& role,Diagnostic& d)const noexcept {
    role=loadRoles_.size();
    if(!resourcePrefixCurrent()||!loadRolePrefixCurrent()){
        d.sourcePC=loadRoles_[retainedLoadRoles_?1:0].callPC;return false;
    }
    if(!retainedLoadRoles_){role=0;return true;}
    // Completion is not inferred from reference count, stage alone, or a caller
    // scalar. Require actual X packet, applied post-load stores and canonical
    // current binding before entering the next authored component call.
    if(retainedLoadRoles_!=1){
        const auto& triangle=quads_[loadRoles_[1].constructionIndex];
        d.sourcePC=trianglePostLoadApplied_?0x1FF1B8:
            triangle.stage_==Quad::Stage::loaded?0x2DF54C:triangle.pendingPC_;
        d.resourcePopulationPending=triangle.stage_==Quad::Stage::materialRetained;
        return false; // Actual triangle cursor; no square admission.
    }
    const auto& first=quads_[loadRoles_[0].constructionIndex];
    if(first.stage_!=Quad::Stage::loaded||!firstIconPostLoadApplied_){
        d.sourcePC=first.stage_==Quad::Stage::materialRetained?0x2DF144:
            first.stage_==Quad::Stage::populated?0x2DF158:
            first.stage_==Quad::Stage::dimensionsStored?0x2DF1CC:0x2DF54C;
        d.resourcePopulationPending=first.stage_==Quad::Stage::materialRetained;return false;
    }
    if(first.position10[0]!=514.f||first.position10[1]!=378.f){d.sourcePC=0x1FF0AC;return false;}
    if(!domain_->materials382B38()->preflightMemoryCardFirstIconBinding(*this,d.quad.material)){
        d.sourcePC=d.quad.material.sourcePC;return false;}
    role=1;return true;
}
bool OriginalUiMemoryCardConstructor50KG::retainNextRolePrefix1FF110(Diagnostic& d)noexcept {
    d={};std::size_t role=loadRoles_.size();
    if(retainedLoadRoles_==2){
        if(!resourcePrefixCurrent()||!loadRolePrefixCurrent()){d.sourcePC=0x1FF110;return false;}
        const auto& q=quads_[loadRoles_[1].constructionIndex];
        d.sourceEffectsCommitted=true;d.resourcePopulationPending=q.stage_==Quad::Stage::materialRetained;
        d.sourcePC=trianglePostLoadApplied_?0x1FF1B8:q.stage_==Quad::Stage::loaded?0x2DF54C:
            q.stage_==Quad::Stage::dimensionsStored?0x2DF1CC:q.pendingPC_;return true;
    }
    if(!retainedLoadRoles_){d.sourcePC=0x1FF058;return false;}
    if(!nextRetainRole(role,d)||role!=1)return false;
    const bool result=quads_[loadRoles_[role].constructionIndex].retainMemoryCardRole2465B0(*this,role,d.quad);
    d.sourceEffectsCommitted=d.quad.sourceEffectsCommitted;d.resourcePopulationPending=result;
    d.sourcePC=d.quad.sourcePC;return result;
}
bool OriginalUiMemoryCardConstructor50KG::drainColdQuad2DEFE0(Diagnostic& d)noexcept {
    if(phase_!=Phase::retiring||drainPhase_>2||!inputsCurrent()||
       !scope_->detachedMemoryCardCurrent(*this,entity_,true)){d.sourcePC=0x2DF018;return false;}
    if(!display_||!displayPinned_){d.sourcePC=0x2DF018;return false;}
    auto& display=*display_;
    if(!displayPinned_||display.application_!=root_||display.video_!=video_||
       !display.memoryCardConstructorDrainCurrent50KG()){d.sourcePC=0x2DF018;return false;}
    if(!drainPhase_){std::int32_t actualV0=0;
        const auto result=game::serviceOriginalRenderer244D60(display.dispatch_,true,display.renderer_,d.renderer,actualV0,
            game::OriginalCameraDrainPolicy::initializedContext);
        if(result.status!=game::OriginalCameraStartStatus::complete){d.sourcePC=result.sourcePC;return false;}
        drainPhase_=1;
    }
    if(drainPhase_==1){
        // Slot24's blocking242EE0(1,3) cannot be replaced by GL completion
        // while an unrepresented source callback or either queue remains.
        if(display.dispatch_.pendingCallbackDC){d.sourcePC=0x242F78;return false;}
        if(display.dispatch_.count54||display.dispatch_.count60){d.sourcePC=0x245090;return false;}
        if(!display.renderer_.finishInitializedContextWork(d.renderer)||!d.renderer.contextWorkCompleted){d.sourcePC=0x2DF030;return false;}
        drainPhase_=2;
    }
    return true;
}
bool OriginalUiMemoryCardConstructor50KG::retireDetached142590(Diagnostic& d)noexcept {
    d={};if(phase_==Phase::retired){d.sourcePC=0x1413E0;return true;}
    if(!inputsCurrent()||(phase_!=Phase::detachedConstructed&&phase_!=Phase::retiring)){
        d.sourcePC=0x142590;return false;}
    if(phase_==Phase::detachedConstructed){
        if(!loadRolePrefixCurrent()||drainPhase_||!display_||!displayPinned_){d.sourcePC=0x1FEE40;return false;}
        // Full role-prefix identity was checked above. Cleanup still follows
        // reverse CONSTRUCTION order, never the distinct load-role sequence.
        for(const auto& q:quads_)if(!q.preflightRetirement2DEFE0(d.quad)){
            d.sourcePC=0x1FEE40;return false;
        }
        for(auto p:dynamicArrays_)if(p){d.sourcePC=0x1FEF20;return false;}
        for(auto p:staticArrays34C_)if(p){d.sourcePC=0x1FEE7C;return false;}
        d.entity=scope_->beginDetachedMemoryCardRetirement142590(*this,entity_);
        if(!d.entity.okay()){d.sourcePC=d.entity.sourcePC;return false;}
        phase_=Phase::retiring;d.sourceEffectsCommitted=true;
        // Empty dynamic arrays reset1FEF20, then nine static array count stores.
        dynamicCounts_.fill(0);for(auto& count:staticCounts370_)count=0;
    }
    d.sourceEffectsCommitted=true;
    if(retainedLoadRoles_&&!pendingFamilyRetired_){
        if(!domain_->materials382B38()->clearMemoryCardPendingForRetirement(*this,d.quad.material)){
            d.sourcePC=d.quad.material.sourcePC;return false;
        }
        pendingFamilyRetired_=true;
    }
    while(retiredQuads_<7){auto& q=quads_[6-retiredQuads_];
        if(!q.preflightRetirement2DEFE0(d.quad)){d.sourcePC=d.quad.sourcePC;return false;}
        if(!drainColdQuad2DEFE0(d))return false;
        q.retirementDrainPhase_=drainPhase_;
        if(!q.retireResources2DF038(d.quad)){d.sourcePC=d.quad.sourcePC;return false;}
        ++retiredQuads_;drainPhase_=0;
    }
    const auto keepAlive=root_->memoryCardConstructorOwner_;
    d.entity=scope_->finishDetachedMemoryCardRetirement1413E0(*this,entity_);
    if(!d.entity.okay()){d.sourcePC=d.entity.sourcePC;return false;}
    phase_=Phase::retired;
    if(displayPinned_){display_->unpin();displayPinned_=false;}
    display_.reset();root_->memoryCardConstructorOwner_.reset();d.sourcePC=0x1413E0;return true;
}
}
namespace rr::game {
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::constructDetachedMemoryCard1FEC80(MemoryCardConstructor& owner,C& c)noexcept {
    auto r=live();if(!r.okay())return r;
    if(!dispatcherLive_||!linked(dispatcher_,&application_->root10_)||detachedMemoryCardOwner_||detachedMemoryCard_||
       owner.scope_.get()!=this||owner.root_!=issuer_||!owner.inputsCurrent()||
       owner.phase_!=MemoryCardConstructor::Phase::bound||&c!=&owner.entity_||c.reference00||
       application_->objectCount423F60_==std::numeric_limits<std::uint32_t>::max())return {Status::phase,0x1FEC80};
    if(!application_->entityAuthority_.reserveAdditional(1)||!application_->entityAuthority_.canEnroll(c,key(&c)))
        return {Status::capacity,0x1D6670};
    const auto init=initializeOriginalWreckComponent50LA(c,K::entity141300,scopeGeneration_,key(&c),true,application_->objectCount423F60_);
    if(init.status!=OriginalWreckComponentStatus50LA::complete||!application_->entityAuthority_.enroll(c))std::terminate();
    detachedMemoryCardOwner_=&owner;detachedMemoryCard_=&c;return {Status::complete,0x1FEC8C};
}
bool OriginalUiEntityScope50KG::detachedMemoryCardCurrent(const MemoryCardConstructor& owner,const C& c,bool retiring)const noexcept {
    if(!live().okay()||detachedMemoryCardOwner_!=&owner||detachedMemoryCard_!=&c||
       owner.scope_.get()!=this||owner.root_!=issuer_||&c!=&owner.entity_||detachedMemoryCardRetiring_!=retiring||
       c.kind!=K::entity141300||c.sessionGeneration!=scopeGeneration_||c.objectKey!=key(&c)||
       c.previous10||c.next14||c.firstChild18||c.parent1C||c.mask20||c.descendants24)return false;
    return retiring?(!c.reference00&&!contains(application_->entityAuthority_.view(),&c)&&application_->objectCount423F60_):
        (c.reference00==1&&linked(c,nullptr));
}
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::beginDetachedMemoryCardRetirement142590(MemoryCardConstructor& owner,C& c)noexcept {
    if(!detachedMemoryCardCurrent(owner,c,false)||!application_->entityAuthority_.canRetire(c))return {Status::phase,0x142590};
    if(!application_->entityAuthority_.retire(c))std::terminate();
    --c.reference00;detachedMemoryCardRetiring_=true;return {Status::complete,0x104970};
}
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::finishDetachedMemoryCardRetirement1413E0(MemoryCardConstructor& owner,C& c)noexcept {
    if(!detachedMemoryCardCurrent(owner,c,true)||owner.retiredQuads_!=7||owner.drainPhase_)
        return {Status::phase,0x1413E0};
    --application_->objectCount423F60_;detachedMemoryCard_=nullptr;detachedMemoryCardOwner_=nullptr;
    detachedMemoryCardRetiring_=false;return {Status::complete,0x1413E0};
}
}

namespace rr::assets {
bool OriginalMemoryCardScreenFallback50KG::occurrenceCurrent()const noexcept {
    const auto owner=issuer_.lock();return owner&&owner->pendingFamilyOccurrenceCurrent(*this);
}
bool OriginalMemoryCardScreenFallback50KG::captureAt29B638(const std::string& material,
    const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,
    const std::shared_ptr<OriginalStandardTextureTable50LA>& textures,
    const std::shared_ptr<const OriginalStandardProgramTable50LA>& programs,Observation& out)const noexcept {
    out=Observation{};
    if(!occurrenceCurrent()||context!=context_||textures!=textures_||programs!=programs_||!programs||!provider_||!provider_->sealed()||
       roleIndex_>1||material.size()!=name_.size()+4||
       !OriginalStandardArchiveProvider50LA::sameName(std::string_view(material).substr(0,name_.size()),name_)||
       !OriginalStandardArchiveProvider50LA::sameName(std::string_view(material).substr(name_.size()),".psm")||
       !options_||!options_->loaded()||options_->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::options))return false;
    try {
        const auto page=options_->member(46),widgets=options_->member(1);
        if(page.identity!=originalUiMemberIdentity50KG(OriginalUiArchiveId50KG::options,46)||
           widgets.identity!=originalUiMemberIdentity50KG(OriginalUiArchiveId50KG::options,1)||!page.data||!widgets.data)return false;
        using Provider=OriginalStandardArchiveProvider50LA;Provider::MemberView absent,pst;
        const auto revision=provider_->revision();
        if(provider_->lookup132820(material,absent)!=Provider::Lookup::absent||
           provider_->lookup132820(name_+".pst",pst)!=Provider::Lookup::present||
           !pst.member||!pst.data||!pst.size||absent.revision!=revision||pst.revision!=revision||
           !pst.bankGeneration||provider_->revision()!=revision||!provider_->sealed())return false;
        OriginalUiEncodedArchiveView50KG retained;
        if(!provider_->encodedUiInput(pst.mount,retained)||retained.providerIdentity()!=provider_.get())return false;
        const auto* identity=retained.identity();
        if(identity!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::controllerPopup)&&
           identity!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))return false;
        const auto* member=originalUiMemberIdentity50KG(identity->id,pst.ordinal);
        if(!member||!Provider::sameName(member->name,*pst.member)||member->bytes!=pst.size||
           !Provider::sameName(*pst.member,name_+".pst"))return false;
        Observation next;next.roleIndex=roleIndex_;next.constructionIndex=constructionIndex_;
        next.componentOffset=componentOffset_;next.sourceCallerPC=sourceCallerPC_;
        next.options=options_;next.selectedArchive=std::move(retained);next.provider=provider_;
        next.selectedBytes=pst.data;next.pstBytes.assign(pst.data,pst.data+pst.size);
        next.mount=pst.mount;next.ordinal=pst.ordinal;next.revision=revision;next.bankGeneration=pst.bankGeneration;
        // Capture is still the same construction event after allocations.
        if(!occurrenceCurrent()||provider_->revision()!=revision||!provider_->sealed())return false;
        out=std::move(next);return true;
    }catch(...){return false;}
}
}
