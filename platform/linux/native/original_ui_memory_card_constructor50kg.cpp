#include "original_ui_memory_card_constructor50kg.h"
#include "../../../src/assets/original_ui_pages50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include <exception>
#include <cstring>
#include <limits>
#include <bit>
#include <cfenv>
#include <cmath>
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
    if(role>3)return false;
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
    return prefix&&!iconPostLoadApplied_[role]&&!q.resource58&&!q.packetOwner_&&!q.dmaData4_&&!q.packetRevision_&&
        (!q.material54||!q.material54->packet10);
}
bool OriginalUiMemoryCardConstructor50KG::completeFirstIconComponent1FF058(Diagnostic& d)noexcept {
    return completeIconComponent(0,d);
}
bool OriginalUiMemoryCardConstructor50KG::completeTriangleComponent1FF110(Diagnostic& d)noexcept {
    return completeIconComponent(1,d);
}
bool OriginalUiMemoryCardConstructor50KG::completeIconComponent(std::size_t role,Diagnostic& d)noexcept {
    d={};const auto table=domain_?domain_->materials382B38():nullptr;std::int32_t row=-1;
    if(role>3||!table||!optionsSource_||!iconOccurrenceCurrent(role,*table,row)){d.sourcePC=0x2DF158;return false;}
    // These preflights are outside any population callback. Earlier packet,
    // placement and exact canonical bindings remain actual prerequisites.
    for(std::size_t prior=0;prior<role;++prior)
        if(!completedIconStateCurrent(prior)||!table->preflightMemoryCardIconBinding(*this,prior,d.quad.material)){
            d.sourcePC=d.quad.material.sourcePC?d.quad.material.sourcePC:iconProgressPC(prior);return false;
        }
    auto& q=quads_[loadRoles_[role].constructionIndex];const auto& post=iconPostLoads_[role];
    if(q.stage_!=Quad::Stage::loaded){
        if(!q.completeMemoryCardIcon2DF158(*this,role,d.quad)){
            d.sourcePC=d.quad.sourcePC;d.sourceEffectsCommitted=d.quad.sourceEffectsCommitted;return false;
        }
        d.sourceEffectsCommitted=true;
    }
    if(!iconOccurrenceCurrent(role,*table,row)||!table->preflightMemoryCardIconBinding(*this,role,d.quad.material)){
        d.sourcePC=d.quad.material.sourcePC?d.quad.material.sourcePC:post.startPC;return false;
    }
    // Source compares X then Y, stores X then Y, marks dirty bit0, and only
    // inside that changed-position branch marks bit1 for nonzero rotation.
    // No packet regeneration; each packet remains its real initialized owner.
    if(!iconPostLoadApplied_[role]){
        if(q.position10[0]!=post.x||q.position10[1]!=post.y){
            q.position10[0]=post.x;q.position10[1]=post.y;
            q.flags5CValue=std::uint8_t((q.flags5CValue&~1u)|1u);q.flags5CKnownMask|=1;
            if(q.uv30[3]!=0.f){q.flags5CValue=std::uint8_t((q.flags5CValue&~2u)|2u);q.flags5CKnownMask|=2;}
        }
        iconPostLoadApplied_[role]=true;d.sourceEffectsCommitted=true;
    }
    if(q.position10[0]!=post.x||q.position10[1]!=post.y){d.sourcePC=post.positionPC;return false;}
    d.sourcePC=post.stopPC;return true; // No following role setup here.
}
bool OriginalUiMemoryCardConstructor50KG::loadSquareCirclePrefix1FF1C8(Diagnostic& d)noexcept {
    d={};bool effects=false;
    if(!resourcePrefixCurrent()||!loadRolePrefixCurrent()){d.sourcePC=0x1FF1C8;return false;}
    for(std::size_t role=2;role<4;++role){Diagnostic step;
        // Completed predecessors are not re-populated on a resumed call. The
        // next real operation validates their bindings before any new effect.
        if(completedIconStateCurrent(role)){effects=true;continue;}
        if(!retainIconRolePrefix(role,step)){d=step;d.sourceEffectsCommitted|=effects;return false;}
        effects|=step.sourceEffectsCommitted;
        if(!populateIconResources2DF144(role,step)){d=step;d.sourceEffectsCommitted|=effects;return false;}
        effects|=step.sourceEffectsCommitted;
        if(!completeIconComponent(role,step)){d=step;d.sourceEffectsCommitted|=effects;return false;}
        effects|=step.sourceEffectsCommitted;d=step;
    }
    const auto table=domain_->materials382B38();
    for(std::size_t role=0;role<4;++role)
        if(!completedIconStateCurrent(role)||!table->preflightMemoryCardIconBinding(*this,role,d.quad.material)){
            d.sourcePC=d.quad.material.sourcePC?d.quad.material.sourcePC:iconProgressPC(role);
            d.sourceEffectsCommitted|=effects;return false;
        }
    d.sourcePC=0x1FF328;d.sourceEffectsCommitted|=effects;return true; // Before authored strips.
}
bool OriginalUiMemoryCardConstructor50KG::iconOccurrenceCurrent(std::size_t role,
    const assets::OriginalStandardMaterialTable50LA& table,std::int32_t& row)const noexcept {
    if(role==0)return firstIconOccurrenceCurrent(table,row);
    row=-1;
    if(role>3||role>=retainedLoadRoles_||!resourcePrefixCurrent()||!loadRolePrefixCurrent()||
       domain_->materials382B38().get()!=&table)return false;
    row=quads_[loadRoles_[role].constructionIndex].material54->material00;return row>=0;
}
bool OriginalUiMemoryCardConstructor50KG::pendingFamilyOccurrenceCurrent(
    const assets::OriginalMemoryCardScreenFallback50KG& proof)const noexcept {
    std::int32_t row=-1;const auto table=domain_?domain_->materials382B38():nullptr;
    return proof.roleIndex_<=3&&table&&iconOccurrenceCurrent(proof.roleIndex_,*table,row)&&optionsSource_&&proof.options_==optionsSource_&&
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
    if(role>3||!table||!iconOccurrenceCurrent(role,*table,row)||!optionsSource_||pendingFamilyRetired_||
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
    d={};if(role>3){d.sourcePC=0x2DF144;return false;}
    auto& progress=iconPopulation_[role];d.sourceInputsAdmitted=progress.inputsAdmitted;
    const auto table=domain_?domain_->materials382B38():nullptr;std::int32_t row=-1;
    if(!table||!iconOccurrenceCurrent(role,*table,row)||!optionsSource_||pendingFamilyRetired_){d.sourcePC=0x2DF144;return false;}
    auto& quad=quads_[loadRoles_[role].constructionIndex];
    if(quad.stage_!=Quad::Stage::materialRetained){
        const bool okay=table->preflightMemoryCardIconBinding(*this,role,d.quad.material);
        d.sourcePC=okay?iconProgressPC(role):d.quad.material.sourcePC;
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
               !assets::OriginalMemoryCardScreenFallback50KG::constructionMemberSupported(role,input.identity(),pst.ordinal)){d.sourcePC=0x132820;return false;}
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
bool OriginalUiMemoryCardConstructor50KG::populateStripResources2DF144(Diagnostic& d,std::size_t role)noexcept {
    d={};if(role<4||role>6){d.sourcePC=0x2DF144;return false;}
    auto& progress=stripPopulation_[role-4];d.sourceInputsAdmitted=progress.inputsAdmitted;
    const auto failurePC=[](const assets::OriginalStandardMaterialDiagnostic50LA& material,std::uint32_t fallback)noexcept {
        return material.status==assets::OriginalStandardMaterialStatus50LA::texture&&material.texture.sourcePC?
            material.texture.sourcePC:(material.sourcePC?material.sourcePC:fallback);
    };
    const auto table=domain_?domain_->materials382B38():nullptr;std::int32_t row=-1;
    if(!table||!stripOccurrenceCurrent(*table,row,role)||!optionsSource_||pendingFamilyRetired_){d.sourcePC=0x2DF144;return false;}
    auto& quad=quads_[loadRoles_[role].constructionIndex];
    if(quad.stage_!=Quad::Stage::materialRetained){
        const bool okay=table->preflightMemoryCardStripBinding(*this,d.quad.material,role);
        d.sourcePC=okay?stripProgressPC(role):failurePC(d.quad.material,0x2DF158);
        d.sourceInputsAdmitted=progress.inputsAdmitted;return okay;
    }
    if(progress.attempted){d.sourcePC=progress.stopPC;d.sourceInputsAdmitted=progress.inputsAdmitted;return false;}
    using Provider=assets::OriginalStandardArchiveProvider50LA;
    const auto provider=domain_->archiveProvider3E1A70();
    bool firstPending=false,noTexturePending=false;
    if(!provider||!provider->sealed()||!table->preflightMemoryCardStripPopulation(*this,firstPending,noTexturePending,d.quad.material,role)){
        d.sourcePC=failurePC(d.quad.material,0x2DF144);return false;
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
            // Separate real active selections: authored PSM and its map image
            // need not (and here do not) come from the same archive.
            Provider::MemberView psm,pst;
            if(provider->lookup132820("ui\\textures\\overlay1top.psm",psm)!=Provider::Lookup::present||
               !psm.member||!psm.data||!psm.size||!psm.bankGeneration||psm.revision!=revision){d.sourcePC=0x29B628;return false;}
            assets::OriginalUiEncodedArchiveView50KG psmInput;
            if(!provider->encodedUiInput(psm.mount,psmInput)||psmInput.providerIdentity()!=provider.get()||!psmInput.identity()){
                d.sourcePC=0x132820;return false;
            }
            const auto* psmIdentity=assets::originalUiMemberIdentity50KG(psmInput.identity()->id,psm.ordinal);
            if(!psmIdentity||!psmIdentity->name||!psmIdentity->sha256||psmIdentity->bytes!=1090||psm.size!=psmIdentity->bytes||
               !Provider::sameName(psmIdentity->name,*psm.member)||!Provider::sameName(*psm.member,"ui\\textures\\overlay1top.psm")||
               std::strcmp(psmIdentity->sha256,"8dcfc7644c8a2d60deb40cf20e2bc8a9b358f0200024da31612563302a9f4230")){
                d.sourcePC=0x29B628;return false;
            }
            auto source=std::make_shared<assets::OriginalStandardMaterialSource50LA>();
            const std::vector<std::uint8_t> bytes(psm.data,psm.data+psm.size);
            if(!source->prepare(*psm.member,bytes,d.quad.material.document)||source->section("Prologue")||source->section("Epilogue")){
                d.sourcePC=0x29B6D8;return false;
            }
            const auto* general=source->section("General");const auto* pass=source->section("ShaderPass_1");
            std::int32_t count=0,maps=0;
            if(!general||!pass||!assets::originalUiInteger50KG(*general,"TotalShaderPasses",0,count,d.quad.material.document,0x29B718)||count!=1||
               !assets::originalUiInteger50KG(*pass,"totalTextureMaps",1,maps,d.quad.material.document,0x247214)||maps!=1){
                d.sourcePC=0x247214;return false;
            }
            const auto* program=pass->find("microProgramName");const auto* map=pass->find("TextureMap_1");
            const auto* group=pass->find("ShaderPassGroup");
            if(!program||!Provider::sameName(program->text,"ScreenObject")||!map||!Provider::sameName(map->text,"overlay1")||
               !group||!Provider::sameName(group->text,"top")){d.sourcePC=0x247110;return false;}
            // Keep the genuine parsed source, including empty/default-valued
            // attributes. The actual constructor interprets it, not a recipe.
            sources.push_back(std::move(source));
            if(provider->lookup132820("ui\\textures\\overlay1.pst",pst)!=Provider::Lookup::present||
               !pst.member||!pst.data||!pst.size||!pst.bankGeneration||pst.revision!=revision){d.sourcePC=0x2472F0;return false;}
            assets::OriginalUiEncodedArchiveView50KG pstInput;
            if(!provider->encodedUiInput(pst.mount,pstInput)||pstInput.providerIdentity()!=provider.get()||
               pstInput.identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::controllerPopup)){
                d.sourcePC=0x132820;return false;
            }
            const auto* pstIdentity=assets::originalUiMemberIdentity50KG(pstInput.identity()->id,pst.ordinal);
            if(!pstIdentity||pstIdentity!=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::controllerPopup,3)||
               !pstIdentity->sha256||pstIdentity->bytes!=604||pst.size!=pstIdentity->bytes||
               !Provider::sameName(pstIdentity->name,*pst.member)||!Provider::sameName(*pst.member,"ui\\textures\\overlay1.pst")||
               std::strcmp(pstIdentity->sha256,"f562a3dded6733713aeaaaf833a95a3c1f243f75086a4a72d7e8e2aca03e32d9")){
                d.sourcePC=0x2472F0;return false;
            }
            textures.push_back({*pst.member,std::vector<std::uint8_t>(pst.data,pst.data+pst.size)});
        }
        bool stillFirst=false,stillNoTexture=false;
        const bool beforeAdmission=table->preflightMemoryCardStripPopulation(*this,stillFirst,stillNoTexture,d.quad.material,role);
        if(!beforeAdmission||stillFirst!=firstPending||stillNoTexture!=noTexturePending||provider->revision()!=revision||!provider->sealed()){
            d.sourcePC=beforeAdmission?0x2DF144:failurePC(d.quad.material,0x2DF144);return false;
        }
        // Authored positive construction: no ordinary-icon absence proof is
        // created. The actual Object captures its own positive history.
        // Real corpus admission owns its own immutable/provider receipts. It
        // may retain input files, but does not fabricate material publication.
        if(!domain_->appendOriginalSources(sources,textures,d.quad.material)){
            d.sourcePC=failurePC(d.quad.material,0x2DF144);return false;
        }
        progress.inputsAdmitted=true;d.sourceInputsAdmitted=true;
        const bool afterAdmission=table->preflightMemoryCardStripPopulation(*this,stillFirst,stillNoTexture,d.quad.material,role);
        if(!afterAdmission||stillFirst!=firstPending||stillNoTexture!=noTexturePending||provider->revision()!=revision||!provider->sealed()){
            d.sourcePC=afterAdmission?0x2DF144:failurePC(d.quad.material,0x2DF144);return false;
        }
        progress.attempted=true;progress.stopPC=0x2DF144;
        // Actual full ascending scans, exactly14D370 then29E340. No collector
        // rebuild, per-row shortcut, borrowed Garage proof or source re-retain.
        if(!domain_->populateMemoryCardResources2DF144(context_,d.quad.material)){
            d.sourceEffectsCommitted=d.quad.material.sourceEffectsCommitted;
            d.sourcePC=failurePC(d.quad.material,0x2DF144);
            progress.stopPC=d.sourcePC;return false;
        }
        const auto populated=d.quad.material;d.sourceEffectsCommitted=populated.sourceEffectsCommitted;
        assets::OriginalStandardMaterialDiagnostic50LA binding;
        if(!table->preflightMemoryCardStripBinding(*this,binding,role)){
            d.sourcePC=failurePC(binding,0x2DF158);d.quad.material=binding;
            // Keep real pair telemetry/effects while reporting the subsequent
            // binding failure. A read-only preflight must not erase history.
            d.quad.material.populationVisited=populated.populationVisited;
            d.quad.material.populationPublished=populated.populationPublished;
            d.quad.material.sourceEffectsCommitted|=populated.sourceEffectsCommitted;
            d.quad.material.texture.populationVisited=populated.texture.populationVisited;
            d.quad.material.texture.populationPublished=populated.texture.populationPublished;
            d.quad.material.texture.sourceEffectsCommitted|=populated.texture.sourceEffectsCommitted;
            progress.stopPC=d.sourcePC;return false;
        }
        // Preserve the successful pair's material/texture diagnostics unchanged;
        // the binding observation above grants no additional source effects.
        quad.stage_=Quad::Stage::populated;quad.pendingPC_=0x2DF158;
        progress.stopPC=0x2DF158;d.sourcePC=0x2DF158;return true;
    }catch(...){d.sourcePC=progress.attempted?progress.stopPC:0x2DF144;
        d.sourceInputsAdmitted=progress.inputsAdmitted;return false;}
}
bool OriginalUiMemoryCardConstructor50KG::firstIconRetirementIdentity(
    const assets::OriginalStandardMaterialTable50LA& table,std::int32_t& row)const noexcept {
    return retainedRoleRetirementIdentity(0,table,row);
}
bool OriginalUiMemoryCardConstructor50KG::retainedRoleRetirementIdentity(std::size_t role,
    const assets::OriginalStandardMaterialTable50LA& table,std::int32_t& row)const noexcept {
    row=-1;if(role>6||role>=retainedLoadRoles_||!inputsCurrent()||phase_!=Phase::retiring||!retainedLoadRoles_||retiredQuads_||
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
    d.sourcePC=result?(iconPostLoadApplied_[0]?0x1FF100:stage==Quad::Stage::loaded?0x2DF54C:
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
        d.sourcePC=d.resourcePopulationPending?0x2DF144:iconPostLoadApplied_[0]?0x1FF100:
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
    if(roleIndex>=4)return stripPacketStateCurrent(roleIndex);
    return iconPacketStateCurrent(roleIndex);
}
bool OriginalUiMemoryCardConstructor50KG::stripPacketStateCurrent(std::size_t role)const noexcept {
    if(role<4||role>6)return false;
    const auto& q=quads_[loadRoles_[role].constructionIndex];
    if(q.nativeGarageFifoPins_||q.nativeMenuWarningPins_)return false;
    if(q.stage_==Quad::Stage::loaded)
        return q.pendingPC_==0&&q.packetRevision_==1&&q.resource58&&q.resource58==q.packetOwner_.get()&&
            q.material54&&q.material54->packet10==q.resource58&&q.dmaData4_==q.resource58+1&&
            reinterpret_cast<std::uintptr_t>(q.resource58)%64u==0;
    const bool prefix=(q.stage_==Quad::Stage::materialRetained&&q.pendingPC_==0x2DF144)||
        (q.stage_==Quad::Stage::populated&&q.pendingPC_==0x2DF158)||
        (q.stage_==Quad::Stage::dimensionsStored&&q.pendingPC_==0x2DF1CC);
    return prefix&&!stripPostLoadApplied_[role-4]&&!q.resource58&&!q.packetOwner_&&!q.dmaData4_&&!q.packetRevision_&&
        (!q.material54||!q.material54->packet10);
}
bool OriginalUiMemoryCardConstructor50KG::completedStripStateCurrent(std::size_t role)const noexcept {
    if(role<4||role>6||role>=retainedLoadRoles_||!stripPostLoadApplied_[role-4]||!retainedRoleCurrent(role))return false;
    const auto& q=quads_[loadRoles_[role].constructionIndex];
    if(q.stage_!=Quad::Stage::loaded||q.position10[0]!=0.f||q.position10[1]!=0.f||
       q.uv30[1]!=640.f||q.uv30[2]!=(role==6?14.f:16.f)||q.color44!=0xFFF2851Bu)return false;
    for(std::size_t i=0;i<4;++i){const auto& rgba=q.resource58[4+3*i].words;
        if(rgba[0]!=121u||rgba[1]!=66u||rgba[2]!=13u||
           rgba[3]!=std::uint32_t(std::int16_t(q.alpha48)>>2))return false;
    }
    return stripStqCurrent(role);
}
std::uint32_t OriginalUiMemoryCardConstructor50KG::stripProgressPC(std::size_t role)const noexcept {
    if(role<4||role>6)return 0x1FF868;
    const auto& q=quads_[loadRoles_[role].constructionIndex];
    return stripPostLoadApplied_[role-4]?(role==4?0x1FF49C:role==5?0x1FF67C:0x1FF868):q.stage_==Quad::Stage::loaded?0x2DF54C:q.pendingPC_;
}
bool OriginalUiMemoryCardConstructor50KG::completeStripComponent(Diagnostic& d,std::size_t role)noexcept {
    d={};const auto table=domain_?domain_->materials382B38():nullptr;std::int32_t row=-1;
    if(role<4||role>6||!table||!optionsSource_||!stripOccurrenceCurrent(*table,row,role)){d.sourcePC=0x2DF158;return false;}
    for(std::size_t prior=0;prior<4;++prior)
        if(!completedIconStateCurrent(prior)||!table->preflightMemoryCardIconBinding(*this,prior,d.quad.material)){
            d.sourcePC=d.quad.material.sourcePC?d.quad.material.sourcePC:iconProgressPC(prior);return false;
        }
    auto& q=quads_[loadRoles_[role].constructionIndex];
    if(q.stage_!=Quad::Stage::loaded){
        if(!q.completeMemoryCardStrip2DF158(*this,d.quad,role)){
            d.sourcePC=d.quad.sourcePC;d.sourceEffectsCommitted=d.quad.sourceEffectsCommitted;return false;
        }
        d.sourceEffectsCommitted=true;
    }
    // Real positive construction history and current occurrence, not an icon
    // fallback capability. Allocation failure retains dimensionsStored; a
    // post-allocation binding failure retains the actual initialized packet.
    if(!stripOccurrenceCurrent(*table,row,role)||!table->preflightMemoryCardStripBinding(*this,d.quad.material,role)){
        d.sourcePC=d.quad.material.sourcePC?d.quad.material.sourcePC:(role==4?0x1FF340:role==5?0x1FF4B4:0x1FF694);return false;
    }
    // Single-precision source multiplication requires the existing packet
    // arithmetic mode. Check before the first authored post-effect store.
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(q.scale4C)||!std::isfinite(q.scale50)){
        d.sourcePC=role==4?0x1FF340:role==5?0x1FF4B4:0x1FF694;return false;
    }
    if(!stripPostLoadApplied_[role-4]){
        // 1FF360 then1FF3CC: independent scalar comparisons/stores. These are
        // UV extents, NOT reciprocal scales, XY placement or STQ rewrites.
        if(q.uv30[1]!=640.f){
            q.uv30[1]=640.f;q.flags5CValue|=1;q.flags5CKnownMask|=1;
            if(q.uv30[3]!=0.f){q.flags5CValue|=2;q.flags5CKnownMask|=2;}
        }
        if(q.uv30[2]!=(role==6?14.f:16.f)){
            q.uv30[2]=role==6?14.f:16.f;q.flags5CValue|=1;q.flags5CKnownMask|=1;
            if(q.uv30[3]!=0.f){q.flags5CValue|=2;q.flags5CKnownMask|=2;}
        }
        if(role>4){
            // Actual 1FF5A4..5FC / 1FF784..7E8 products and store order.
            // Do not replace zero products or recompute reciprocal dimensions.
            volatile float sl=0.f*q.scale4C;
            q.resource58[12].words[0]=std::bit_cast<std::uint32_t>(float(sl));
            q.resource58[6].words[0]=std::bit_cast<std::uint32_t>(float(sl));
            volatile float sr=32.f*q.scale4C;
            q.resource58[15].words[0]=std::bit_cast<std::uint32_t>(float(sr));
            q.resource58[9].words[0]=std::bit_cast<std::uint32_t>(float(sr));
            volatile float tt=(role==5?32.f:18.f)*q.scale50;
            q.resource58[9].words[1]=std::bit_cast<std::uint32_t>(float(tt));
            q.resource58[6].words[1]=std::bit_cast<std::uint32_t>(float(tt));
            volatile float tb=(role==5?0.f:32.f)*q.scale50;
            q.resource58[15].words[1]=std::bit_cast<std::uint32_t>(float(tb));
            q.resource58[12].words[1]=std::bit_cast<std::uint32_t>(float(tb));
        }
        // 1FF43C..498 (later identical channel order at1FF61C/1FF808): channel-major, reverse vertex order on the actual
        // allocated packet. Preserve alpha, STQ and every other packet word.
        if(q.color44!=0xFFF2851Bu){
            q.resource58[13].words[0]=121;q.resource58[10].words[0]=121;
            q.resource58[7].words[0]=121;q.resource58[4].words[0]=121;
            q.resource58[13].words[1]=66;q.resource58[10].words[1]=66;
            q.resource58[7].words[1]=66;q.resource58[4].words[1]=66;
            q.resource58[13].words[2]=13;q.resource58[10].words[2]=13;
            q.resource58[7].words[2]=13;q.resource58[4].words[2]=13;
            q.color44=0xFFF2851Bu;
        }
        stripPostLoadApplied_[role-4]=true;d.sourceEffectsCommitted=true;
    }
    if(!completedStripStateCurrent(role)){d.sourcePC=role==4?0x1FF498:role==5?0x1FF678:0x1FF864;return false;}
    d.sourcePC=stripProgressPC(role);return true; // No renderer/static-clone or page readiness.
}
bool OriginalUiMemoryCardConstructor50KG::completedIconStateCurrent(std::size_t role)const noexcept {
    if(role>3||role>=retainedLoadRoles_||!iconPostLoadApplied_[role]||!retainedRoleCurrent(role))return false;
    const auto& q=quads_[loadRoles_[role].constructionIndex];const auto& post=iconPostLoads_[role];
    return q.stage_==Quad::Stage::loaded&&q.position10[0]==post.x&&q.position10[1]==post.y;
}
std::uint32_t OriginalUiMemoryCardConstructor50KG::iconProgressPC(std::size_t role)const noexcept {
    if(role>3||role>=retainedLoadRoles_)return role<loadRoles_.size()?loadRoles_[role].callPC:0x1FF328;
    const auto& q=quads_[loadRoles_[role].constructionIndex];
    return iconPostLoadApplied_[role]?iconPostLoads_[role].stopPC:
        q.stage_==Quad::Stage::loaded?0x2DF54C:q.pendingPC_;
}
bool OriginalUiMemoryCardConstructor50KG::completedLoadRoleCurrent(std::size_t role)const noexcept {
    return role<4?completedIconStateCurrent(role):completedStripStateCurrent(role);
}
bool OriginalUiMemoryCardConstructor50KG::preflightLoadRoleBinding(std::size_t role,Diagnostic& d)const noexcept {
    const auto table=domain_?domain_->materials382B38():nullptr;
    if(!table){d.sourcePC=0x2DF144;return false;}
    const bool okay=role<4?table->preflightMemoryCardIconBinding(*this,role,d.quad.material):
        table->preflightMemoryCardStripBinding(*this,d.quad.material,role);
    if(!okay)d.sourcePC=d.quad.material.sourcePC;return okay;
}
bool OriginalUiMemoryCardConstructor50KG::stripStqCurrent(std::size_t role)const noexcept {
    if(role<4||role>6)return false;
    const auto& q=quads_[loadRoles_[role].constructionIndex];
    if(!q.resource58||std::fegetround()!=FE_TONEAREST||!std::isfinite(q.scale4C)||!std::isfinite(q.scale50))return false;
    volatile float sl=0.f*q.scale4C,sr=32.f*q.scale4C;
    volatile float tt=(role==5?32.f:18.f)*q.scale50,tb=(role==5?0.f:32.f)*q.scale50;
    const std::array<std::uint32_t,4> ss=role==4?std::array<std::uint32_t,4>{0,0x3F800000,0,0x3F800000}:
        std::array<std::uint32_t,4>{std::bit_cast<std::uint32_t>(float(sl)),std::bit_cast<std::uint32_t>(float(sr)),
            std::bit_cast<std::uint32_t>(float(sl)),std::bit_cast<std::uint32_t>(float(sr))};
    const std::array<std::uint32_t,4> ts=role==4?std::array<std::uint32_t,4>{0,0,0x3F800000,0x3F800000}:
        std::array<std::uint32_t,4>{std::bit_cast<std::uint32_t>(float(tt)),std::bit_cast<std::uint32_t>(float(tt)),
            std::bit_cast<std::uint32_t>(float(tb)),std::bit_cast<std::uint32_t>(float(tb))};
    for(std::size_t i=0;i<4;++i){const auto& stq=q.resource58[6+3*i].words;
        if(stq[0]!=ss[i]||stq[1]!=ts[i]||stq[2]!=0x3F800000||stq[3]!=0)return false;
    }
    return true;
}
bool OriginalUiMemoryCardConstructor50KG::loadLaterStrips1FF4AC(Diagnostic& d)noexcept {
    d={};bool effects=false;
    if(!resourcePrefixCurrent()||!loadRolePrefixCurrent()){d.sourcePC=0x1FF4AC;return false;}
    for(std::size_t role=5;role<7;++role){Diagnostic step;
        if(completedStripStateCurrent(role)){effects=true;continue;}
        if(!retainStripRolePrefix(role,step)){d=step;d.sourceEffectsCommitted|=effects;return false;}
        effects|=step.sourceEffectsCommitted;
        if(!populateStripResources2DF144(step,role)){d=step;d.sourceEffectsCommitted|=effects;return false;}
        effects|=step.sourceEffectsCommitted;
        if(!completeStripComponent(step,role)){d=step;d.sourceEffectsCommitted|=effects;return false;}
        effects|=step.sourceEffectsCommitted;
    }
    for(std::size_t role=0;role<7;++role)
        if(!completedLoadRoleCurrent(role)||!preflightLoadRoleBinding(role,d)){
            d.sourceEffectsCommitted|=effects;if(!d.sourcePC)d.sourcePC=0x1FF868;return false;
        }
    d.sourceEffectsCommitted=effects;d.sourcePC=0x1FF868;return true;
}
bool OriginalUiMemoryCardConstructor50KG::storeRendererClearMode1FF874(Diagnostic& d)noexcept {
    d={};
    if(!resourcePrefixCurrent()||!optionsSource_||retainedLoadRoles_!=7||!loadRolePrefixCurrent()){
        d.sourcePC=0x1FF868;return false;
    }
    for(std::size_t role=0;role<7;++role)
        if(!completedLoadRoleCurrent(role)||!preflightLoadRoleBinding(role,d)){
            if(!d.sourcePC)d.sourcePC=role<4?iconProgressPC(role):stripProgressPC(role);return false;
        }
    // This is the actual Root-issued442F70 Display backing already retained
    // by the detached constructor, not a copied renderer or borrowed race
    // helper. Revalidate after all binding observations, before the SW.
    if(!resourcePrefixCurrent()||display_->localizationServiceActive_||display_->localizationServiceFaulted_||
       display_->dispatch_.pendingCallbackDC||display_->dispatch_.count54||display_->dispatch_.count60){
        d.sourcePC=0x1FF874;return false;
    }
    if(rendererClearModeStored1FF874_){
        // Resume without replaying the historical store over a later writer.
        if(display_->sourceClearMode2C_!=0){d.sourcePC=0x1FF874;return false;}
        d.sourceEffectsCommitted=true;d.sourcePC=0x1FF874;return true;
    }
    // Unconditional branch-delay SW, even if the optional tuple is absent.
    // No fog/color/counter changes, frame effect, callback, drain or restore.
    display_->sourceClearMode2C_=0;
    rendererClearModeStored1FF874_=true;
    d.sourceEffectsCommitted=true;d.sourcePC=0x1FF874;return true;
    // Stop at the completed store itself: no fabricated null/non-null s0,
    // no inferred1FF878 tuple copy or1FF890 constructor return.
}
bool OriginalUiMemoryCardConstructor50KG::loadRolePrefixCurrent(bool completedPredecessors)const noexcept {
    if(retainedLoadRoles_>7||constructedQuads_!=7||retiredQuads_)return false;
    for(std::size_t r=0;r<loadRoles_.size();++r){
        const auto& role=loadRoles_[r];const auto& q=quads_[role.constructionIndex];
        if(r<retainedLoadRoles_){
            if(!retainedRoleCurrent(r)||(completedPredecessors&&r+1<retainedLoadRoles_&&!completedLoadRoleCurrent(r)))return false;
        }else if(!q.memoryCardPrefixOnly_||q.offset!=role.offset||!q.constructed||q.garage_||
            q.stage_!=Quad::Stage::constructed||q.material54||q.primitiveOwner_||q.resource58||
            q.packetOwner_||q.dmaData4_||q.packetRevision_||q.nativeGarageFifoPins_||q.nativeMenuWarningPins_)return false;
    }
    return true;
}
bool OriginalUiMemoryCardConstructor50KG::nextRetainRole(std::size_t& role,Diagnostic& d)const noexcept {
    role=loadRoles_.size();
    if(!resourcePrefixCurrent()||!loadRolePrefixCurrent()){
        d.sourcePC=loadRoles_[retainedLoadRoles_<7?retainedLoadRoles_:6].callPC;return false;
    }
    if(retainedLoadRoles_==7){d.sourcePC=stripProgressPC(6);return false;} // No containing-page grant.
    for(std::size_t prior=0;prior<retainedLoadRoles_;++prior){
        if(!completedLoadRoleCurrent(prior)){
            d.sourcePC=prior<4?iconProgressPC(prior):stripProgressPC(prior);
            d.resourcePopulationPending=quads_[loadRoles_[prior].constructionIndex].stage_==Quad::Stage::materialRetained;
            return false;
        }
        if(!preflightLoadRoleBinding(prior,d)){
            d.sourcePC=d.quad.material.sourcePC;return false;
        }
    }
    role=retainedLoadRoles_;return true;
}
bool OriginalUiMemoryCardConstructor50KG::retainNextRolePrefix1FF110(Diagnostic& d)noexcept {
    return retainIconRolePrefix(1,d);
}
bool OriginalUiMemoryCardConstructor50KG::retainIconRolePrefix(std::size_t requested,Diagnostic& d)noexcept {
    d={};if(requested>3){d.sourcePC=0x1FF328;return false;}
    if(requested<retainedLoadRoles_){
        std::int32_t row=-1;
        if(!resourcePrefixCurrent()||!loadRolePrefixCurrent()||
           !iconOccurrenceCurrent(requested,*domain_->materials382B38(),row)){d.sourcePC=loadRoles_[requested].callPC;return false;}
        const auto& q=quads_[loadRoles_[requested].constructionIndex];
        d.sourceEffectsCommitted=true;d.resourcePopulationPending=q.stage_==Quad::Stage::materialRetained;
        d.sourcePC=iconProgressPC(requested);return true;
    }
    std::size_t actual=loadRoles_.size();if(!nextRetainRole(actual,d))return false;
    if(actual!=requested){d.sourcePC=loadRoles_[actual].callPC;return false;}
    const bool result=quads_[loadRoles_[actual].constructionIndex].retainMemoryCardRole2465B0(*this,actual,d.quad);
    d.sourceEffectsCommitted=d.quad.sourceEffectsCommitted;d.resourcePopulationPending=result;
    d.sourcePC=d.quad.sourcePC;return result;
}
bool OriginalUiMemoryCardConstructor50KG::stripOccurrenceCurrent(
    const assets::OriginalStandardMaterialTable50LA& table,std::int32_t& row,std::size_t role)const noexcept {
    row=-1;
    if(role<4||role>6||role>=retainedLoadRoles_||!resourcePrefixCurrent()||!loadRolePrefixCurrent()||
       domain_->materials382B38().get()!=&table)return false;
    row=quads_[loadRoles_[role].constructionIndex].material54->material00;return row>=0;
}
bool OriginalUiMemoryCardConstructor50KG::retainFirstStripPrefix1FF338(Diagnostic& d)noexcept {
    return retainStripRolePrefix(4,d);
}
bool OriginalUiMemoryCardConstructor50KG::retainStripRolePrefix(std::size_t role,Diagnostic& d)noexcept {
    d={};if(role<4||role>6){d.sourcePC=0x1FF868;return false;}
    if(role<retainedLoadRoles_){std::int32_t row=-1;
        if(!domain_||!stripOccurrenceCurrent(*domain_->materials382B38(),row,role)){d.sourcePC=loadRoles_[role].callPC;return false;}
        const auto& q=quads_[loadRoles_[role].constructionIndex];
        d.sourceEffectsCommitted=true;d.resourcePopulationPending=q.stage_==Quad::Stage::materialRetained;
        d.sourcePC=stripProgressPC(role);return true;
    }
    std::size_t actual=loadRoles_.size();if(!nextRetainRole(actual,d))return false;
    if(actual!=role){d.sourcePC=loadRoles_[actual].callPC;return false;}
    const bool result=quads_[loadRoles_[role].constructionIndex].retainMemoryCardRole2465B0(*this,role,d.quad);
    d.sourceEffectsCommitted=d.quad.sourceEffectsCommitted;d.resourcePopulationPending=result;
    d.sourcePC=d.quad.sourcePC;return result;
}
bool OriginalUiMemoryCardConstructor50KG::preflightFirstStripBinding(Diagnostic& d)const noexcept {
    d={};const auto table=domain_?domain_->materials382B38():nullptr;
    if(!table){d.sourcePC=0x2DF144;return false;}
    const bool result=table->preflightMemoryCardStripBinding(*this,d.quad.material);
    // Binding observation does not advance the Quad: report its actual pair
    // cursor, including retained-before-pair versus genuinely populated.
    const auto& q=quads_[loadRoles_[4].constructionIndex];
    d.sourcePC=result?stripProgressPC():d.quad.material.sourcePC;
    d.resourcePopulationPending=result&&q.stage_==Quad::Stage::materialRetained;return result;
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
        // Cleanup requires actual retained/cold owner shape, not arithmetic
        // mode or authored content readiness. It grants no advancement.
        if(!loadRolePrefixCurrent(false)||drainPhase_||!display_||!displayPinned_){d.sourcePC=0x1FEE40;return false;}
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
bool OriginalMemoryCardScreenFallback50KG::constructionMemberSupported(std::size_t role,
    const OriginalUiArchiveIdentity50KG* archive,std::size_t ordinal)noexcept {
    if(role>3||!archive||archive!=originalUiArchiveIdentity50KG(archive->id))return false;
    if(role<2&&archive!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::controllerPopup)&&
       archive!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))return false;
    constexpr const char* names[]{"ui\\textures\\icon_x.pst","ui\\textures\\icon_tri.pst",
        "ui\\textures\\icon_sqr.pst","ui\\textures\\icon_cir.pst"};
    const auto* member=originalUiMemberIdentity50KG(archive->id,ordinal);
    if(!member||!member->name||!OriginalStandardArchiveProvider50LA::sameName(member->name,names[role]))return false;
    if(role<2)return true;
    // Catalog identity, not equal bytes, identifies the actual selected archive.
    // These exact original members occur in eight authenticated UI archives.
    const char* hash=role==2?"4ab9cac37b5359729352e70d6ff92bf91a2841db60e98f91cff467ba37e51ae8":
        "24f6ea44860157377c506324176cf3ea93802ec7fe0708bd91c74a3030967abf";
    return member->bytes==604&&member->sha256&&std::strcmp(member->sha256,hash)==0;
}
bool OriginalMemoryCardScreenFallback50KG::occurrenceCurrent()const noexcept {
    const auto owner=issuer_.lock();return owner&&owner->pendingFamilyOccurrenceCurrent(*this);
}
bool OriginalMemoryCardScreenFallback50KG::captureAt29B638(const std::string& material,
    const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,
    const std::shared_ptr<OriginalStandardTextureTable50LA>& textures,
    const std::shared_ptr<const OriginalStandardProgramTable50LA>& programs,Observation& out)const noexcept {
    out=Observation{};
    if(!occurrenceCurrent()||context!=context_||textures!=textures_||programs!=programs_||!programs||!provider_||!provider_->sealed()||
       roleIndex_>3||material.size()!=name_.size()+4||
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
        if(!constructionMemberSupported(roleIndex_,identity,pst.ordinal))return false;
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
