#include "vehicle_window_gl50ds.h"
#include "original_ui_garage_quad50kg.h"
#include "original_ui_memory_card_constructor50kg.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <bit>
#include <atomic>
#include <limits>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <exception>
#include <new>
#include <optional>
namespace rr::platform {
void OriginalUiGarageQuad50KG::PacketDelete::operator()(Qword* p)const noexcept {::operator delete[](p,std::align_val_t{64});}
OriginalUiGarageQuad50KG::~OriginalUiGarageQuad50KG()noexcept {
    //2DEFE0 requires the actual renderer callbacks even when never loaded.
    if(nativeGarageFifoPins_||nativeMenuWarningPins_||constructed||primitiveOwner_||packetOwner_)std::terminate();
}
const char* OriginalUiGarageQuad50KG::authoredName(std::uint32_t at)noexcept {
    switch(at){case 0x40:case 0xA0:case 0x100:return "overlay1popup";
        case 0x160:return "icon_x_popup";case 0x1C0:return "icon_tri_popup";
        case 0x2A0:return "icon_x";case 0x300:return "icon_tri";
        case 0x360:return "icon_sqr";case 0x3C0:return "icon_cir";
        case 0x520:case 0x580:return "garagetab";case 0x5E0:return "podstat2";
        case 0x33F0:return "arrwleftsm";case 0x3450:return "arrwrghtsm";default:break;}
    return at>=0x640&&at<0x640+120*0x60&&(at-0x640)%0x60==0?"notch":nullptr;
}
bool OriginalUiGarageQuad50KG::initializeColdPrefix2DEF40(std::uint32_t at,bool memoryCardOnly)noexcept {
    if(stage_!=Stage::empty)return false;offset=at;
    position10={0,0,0,1};size20={0,0,0,1};material54=nullptr;resource58=nullptr;
    scale4C=scale50=1;position10[0]=position10[1]=0;uv30={1,0,0,0};
    color44=0x00FFFFFF;alpha48=255;size20[0]=size20[1]=0;field40=1;
    flags5CValue=0;flags5CKnownMask=3;constructed=true;stage_=Stage::constructed;
    memoryCardPrefixOnly_=memoryCardOnly;return true;
}
bool OriginalUiGarageQuad50KG::construct2DEF40(std::uint32_t at)noexcept {
    if(!authoredName(at))return false;
    return initializeColdPrefix2DEF40(at,false);
}
bool OriginalUiGarageQuad50KG::constructMemoryCardPrefix2DEF40(std::uint32_t at)noexcept {
    if(at!=0x40&&at!=0xA0&&at!=0x100&&at!=0x160&&at!=0x1C0&&at!=0x220&&at!=0x280)return false;
    // No authoredName()/Garage source capability; loader stays inaccessible.
    return initializeColdPrefix2DEF40(at,true);
}
bool OriginalUiGarageQuad50KG::retainMemoryCardFirstIcon2465B0(OriginalUiMemoryCardConstructor50KG& owner,Diagnostic& d)noexcept {
    if(owner.retainedLoadRoles_){d={};d.status=Status::phase;d.sourcePC=0x1FF058;return false;}
    return retainMemoryCardRole2465B0(owner,0,d);
}
bool OriginalUiGarageQuad50KG::retainMemoryCardRole2465B0(
    OriginalUiMemoryCardConstructor50KG& owner,std::size_t roleIndex,Diagnostic& d)noexcept {
    d={};OriginalUiMemoryCardConstructor50KG::Diagnostic ownerDiagnostic;std::size_t actualRole=0;
    if(!owner.nextRetainRole(actualRole,ownerDiagnostic)||actualRole!=roleIndex){
        d.status=Status::phase;d.sourcePC=ownerDiagnostic.sourcePC?ownerDiagnostic.sourcePC:
            actualRole<owner.loadRoles_.size()?owner.loadRoles_[actualRole].callPC:0x2DF144;return false;
    }
    const auto& role=owner.loadRoles_[roleIndex];
    if(!owner.resourcePrefixCurrent()||owner.phase_!=OriginalUiMemoryCardConstructor50KG::Phase::detachedConstructed||
       owner.constructedQuads_!=7||owner.retiredQuads_||owner.retainedLoadRoles_!=roleIndex||
       &owner.quads_[role.constructionIndex]!=this||!memoryCardPrefixOnly_||offset!=role.offset||
       stage_!=Stage::constructed||!constructed||material54||resource58)
        {d.status=Status::phase;d.sourcePC=role.callPC;return false;}
    // Actual component2461E0 ownership, prepared before any published effect.
    // No Garage RootInputs, archive, fallback proof or caller-supplied row/name.
    try {
        auto primitive=std::make_unique<Primitive>();
        const auto table=owner.domain_->materials382B38();
        std::int32_t row=-1;
        if(!table||!table->find298C80(role.qualifiedName,row,d.material)||row<0||
           !table->preflightRetain14D590(row,d.material))
            return fail(d,Status::material,d.material.sourcePC?d.material.sourcePC:0x299CF8);
        // Serial actual-owner preflight; no allocation/callback follows until
        // the retain completes. This is not permission to populate the table.
        domain_=owner.domain_;context_=owner.context_;video_=owner.video_;
        primitive->domain=domain_;primitive->context=context_;
        primitiveOwner_=std::move(primitive);material54=primitiveOwner_.get();stage_=Stage::primitivePublished;
        material54->material00=row;material54->materialStored=true;
        auto& guard=OriginalUiGarageScreenText50KG::guard8CD8();
        auto& assertion=OriginalUiGarageScreenText50KG::assertion443640();
        if(!guard.load(std::memory_order_acquire)){assertion.store(1,std::memory_order_relaxed);guard.store(1,std::memory_order_release);}
        // Same source14D590 ref increment and unconditional ScreenMT store.
        // The preflight above is identical to the mutator's own guard.
        if(!table->retainScreen14D590(row,d.material))std::terminate();
        material54->materialRetained=true;material54->type0C=15;material54->flags0F&=std::uint8_t(~1u);
        stage_=Stage::materialRetained;pendingPC_=0x2DF144;
        ++owner.retainedLoadRoles_;d.sourceEffectsCommitted=true;d.sourcePC=0x2DF144;
        return true;
    }catch(...){return fail(d,Status::allocation,0x2DF0E8);}
}
bool OriginalUiGarageQuad50KG::fail(Diagnostic& d,Status status,std::uint32_t pc)noexcept {
    d.status=status;d.sourcePC=pc;pendingPC_=pc;d.sourceEffectsCommitted=material54!=nullptr;
    if(material54)stage_=Stage::failed;return false;
}
bool OriginalUiGarageQuad50KG::inputsCurrent(const RootInputs& in)noexcept {
    return in.issuer_&&in.domain_&&in.domain_->prepared()&&in.context_&&in.context_->live()&&
       in.video_&&in.video_->originalMain&&in.video_->packetZMaximum==0xffffff&&
       in.context_->applicationAnimationClock().sameOwner(in.issuer_->animationClock())&&
       in.context_->materialApplication()==in.issuer_->materialApplication&&
       in.domain_->materials382B38()->screenApplicationMatches(*in.issuer_->materialApplication)&&
       in.garage_&&in.garage_->loaded()&&(in.garage_->identity()==assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::garage)||
        in.garage_->identity()==assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::controllerPopup))&&
       in.directory_&&(*in.directory_=="ui\\textures\\"||*in.directory_=="ui\\textures");
}
bool OriginalUiGarageQuad50KG::retainPrimitive2465B0(const RootInputs& in,Primitive& p,const char* name,Diagnostic& d)noexcept {
    const auto stop=[&](Status status,std::uint32_t pc)noexcept {
        d.status=status;d.sourcePC=pc;d.sourceEffectsCommitted=true;return false;
    };
    if(!inputsCurrent(in)||p.materialStored||!name)return stop(Status::input,0x2465B0);
    try {
        p.domain=in.domain_;p.context=in.context_;
        std::string requested=*in.directory_;if(requested.back()!='\\')requested+='\\';requested+=name;
        const auto table=in.domain_->materials382B38();std::int32_t row=-1;
        const bool found=table->find298C80(requested,row,d.material);
        p.material00=row;p.materialStored=true;
        //The actual shared299BE0 record, also used by canonical ScreenText.
        auto& guard=OriginalUiGarageScreenText50KG::guard8CD8();
        auto& assertion=OriginalUiGarageScreenText50KG::assertion443640();
        if(!guard.load(std::memory_order_acquire)){assertion.store(1,std::memory_order_relaxed);guard.store(1,std::memory_order_release);}
        if((assertion.load()&&row==-1)||!found)return stop(Status::material,0x299CF8);
        if(!table->retainScreen14D590(row,d.material))return stop(Status::material,d.material.sourcePC);
        p.materialRetained=true;p.type0C=15;p.flags0F&=std::uint8_t(~1u);
        d.sourceEffectsCommitted=true;return true;
    }catch(...){return stop(Status::allocation,0x299BE0);}
}
bool OriginalUiGarageQuad50KG::loadAperturePrimitive2E20F0(const RootInputs& in,std::unique_ptr<Primitive>& p,Diagnostic& d)noexcept {
    d={};if(!inputsCurrent(in)||p){d.status=Status::input;d.sourcePC=0x2E20A8;return false;}
    try{p=std::make_unique<Primitive>();}catch(...){d.status=Status::allocation;d.sourcePC=0x2E20A8;return false;}
    //Unlike Quad2DF0B0, Polygon2E1AE0 does NOT run14D370/29E340 here.
    return retainPrimitive2465B0(in,*p,"NoTexture",d);
}
bool OriginalUiGarageQuad50KG::demand1FDD60(Diagnostic& d)noexcept {
    d={};const bool background=offset==0x5E0;
    const bool notch=offset>=0x640&&offset<0x3340&&((offset-0x640)%0x60)==0;
    if((!background&&!notch)||stage_!=Stage::loaded||!constructed||
       !domain_||!domain_->prepared()||!context_||!context_->live()||!garage_||
       garage_->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::garage)||
       !primitiveOwner_||material54!=primitiveOwner_.get()||!material54->materialRetained||
       material54->domain!=domain_||material54->context!=context_||!resource58)
        return fail(d,Status::phase,0x2DF8A0);
    const auto table=domain_->materials382B38();const auto textures=domain_->textures383400();
    if(!table||!textures||!table->demandGarage2DF8A0(material54->material00,*textures,
        background?0x1FDDA8u:0x1FDDE8u,d.material))
        return fail(d,Status::material,d.material.sourcePC?d.material.sourcePC:0x2DF8A0);
    d.sourcePC=background?0x1FDDA8:0x1FDDE8;
    d.sourceEffectsCommitted=d.material.sourceEffectsCommitted;return true;
}
bool OriginalUiGarageQuad50KG::load2DF0B0(const RootInputs& in,Diagnostic& d)noexcept {
    d={};if(memoryCardPrefixOnly_||stage_!=Stage::constructed||!constructed||material54||resource58)
        return fail(d,Status::phase,0x2DF0B0);
    if(!inputsCurrent(in)||((offset<0x200)!=(in.garage_->identity()==
       assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::controllerPopup))))
        return fail(d,Status::input,0x2DF0B0);
    const auto provider=in.domain_->archiveProvider3E1A70();
    if(!provider||!provider->sealed()||std::fegetround()!=FE_TONEAREST)
        return fail(d,Status::input,0x2DF0B0);
    try {
        domain_=in.domain_;context_=in.context_;video_=in.video_;garage_=in.garage_;
        primitiveOwner_=std::make_unique<Primitive>();material54=primitiveOwner_.get();stage_=Stage::primitivePublished;
        std::string requested=*in.directory_;if(requested.back()!='\\')requested+='\\';requested+=authoredName(offset);
        if(!retainPrimitive2465B0(in,*material54,authoredName(offset),d))
            return fail(d,d.status,d.sourcePC);
        const auto table=domain_->materials382B38();const auto row=material54->material00;
        stage_=Stage::materialRetained;
        assets::OriginalStandardMaterialRowView50LA retainedRow;
        if(!table->row(row,retainedRow))return fail(d,Status::material,0x298E60);
        const bool sharedIcon=offset==0x2A0||offset==0x300||offset==0x360||offset==0x3C0;
        const bool reuseShared=sharedIcon&&retainedRow.object04;
        // Authentic current RootInputs and the actual retained primitive are
        // checked independently of historical construction family.
        if(reuseShared&&(!inputsCurrent(in)||in.domain_!=domain_||in.context_!=context_||in.video_!=video_||in.garage_!=garage_||
           !table->preflightGarageLoadedSharedIcon(*this,d.material)))return fail(d,Status::material,0x2DF144);
        assets::OriginalStandardArchiveProvider50LA::MemberView psm;
        std::optional<assets::OriginalStandardArchiveProvider50LA::Lookup> lookup;
        if(!reuseShared)lookup=provider->lookup132820(requested+".psm",psm);
        if(lookup&&*lookup!=assets::OriginalStandardArchiveProvider50LA::Lookup::present&&
           *lookup!=assets::OriginalStandardArchiveProvider50LA::Lookup::absent)
            return fail(d,Status::material,0x132820);
        //Native source corpus input only. Parsing authenticated bytes does not
        //create canonical materials/textures or replace their source refcounts.
        std::vector<std::shared_ptr<const assets::OriginalStandardMaterialSource50LA>> sources;
        std::vector<assets::OriginalStandardTextureSourceInput50LA> textures;
        const auto appendTextureInput=[&](const std::string& name) {
            //Input seeding follows the REAL resolver's24DADC priority, but
            //does not construct a texture or advance an animated prototype.
            using Provider=assets::OriginalStandardArchiveProvider50LA;
            for(const char* extension:{".pst",".ipu",".tga",".atm"}){
                Provider::MemberView image;const auto result=provider->lookup132820(name+extension,image);
                if(result==Provider::Lookup::present){
                    //A present IPU remains unsupported at canonical24DB60;
                    //never seed a lower-priority image to get around it.
                    if(std::strcmp(extension,".ipu")&&image.member&&image.data)
                        textures.push_back({*image.member,std::vector<std::uint8_t>(image.data,image.data+image.size)});
                    return;
                }
                if(result!=Provider::Lookup::absent)return;
            }
        };
        const auto appendPsm=[&](const std::string& name)->bool {
            assets::OriginalStandardArchiveProvider50LA::MemberView member;
            if(provider->lookup132820(name+".psm",member)!=assets::OriginalStandardArchiveProvider50LA::Lookup::present||!member.member||!member.data)return false;
            auto source=std::make_shared<assets::OriginalStandardMaterialSource50LA>();
            std::vector<std::uint8_t> bytes(member.data,member.data+member.size);
            if(!source->prepare(*member.member,bytes,d.material.document))return false;
            for(const auto& section:*source->sections())if(section.name.compare(0,11,"ShaderPass_")==0){
                std::int32_t maps=1;
                if(!assets::originalUiInteger50KG(section,"totalTextureMaps",1,maps,d.material.document,0x247214)||maps<0||maps>2)return false;
                for(std::int32_t map=0;map<maps;++map){
                    const auto* field=section.find(map?"TextureMap_2":"TextureMap_1");
                    auto image=field?field->text:std::string{};
                    if(image.empty())image=name.substr(name.find_last_of('\\')+1);
                    if(image.find_first_of("\\/.:\"")==std::string::npos)appendTextureInput("ui\\textures\\"+image);
                }
            }
            sources.push_back(std::move(source));return true;
        };
        // Corpus admission follows the actual14D370 pending-row rule.
        // Garage's earlier aperture owners retain NoTexture, but initial
        // shared Popup construction has no such aperture owner. A registered
        // zero-ref row is not a required resource and must not pull Garage
        // archive bytes into earlier shared UI startup.
        std::int32_t noTextureRow=-1;
        assets::OriginalStandardMaterialRowView50LA noTexture;
        if(!table->find298C80("ui\\textures\\NoTexture",noTextureRow,d.material)||
           !table->row(noTextureRow,noTexture)||!noTexture.references08||
           *noTexture.references08<0)return fail(d,Status::material,0x29B628);
        if(*noTexture.references08>0&&!noTexture.object04&&
           !appendPsm("ui\\textures\\NoTexture"))return fail(d,Status::material,0x29B628);
        if(reuseShared){
            // Source14D370 skips loaded object04. Do not replay missing-PSM
            // selection, bind a new family, or seed this loaded row's texture.
        }else if(lookup&&*lookup==assets::OriginalStandardArchiveProvider50LA::Lookup::present){
            if(!appendPsm(requested))return fail(d,Status::material,0x29B628);
        }else{
            using Proof=assets::OriginalGarageScreenFallback50KG;
            std::shared_ptr<Proof> proof(new Proof);
            proof->row_=row;proof->name_=requested;
            for(char& c:proof->name_)if(c>='A'&&c<='Z')c=char(c-'A'+'a');
            proof->garage_=garage_;proof->provider_=provider;proof->context_=context_;proof->textures_=domain_->textures383400();
            const bool sharedIcon=proof->name_=="ui\\textures\\icon_x"||proof->name_=="ui\\textures\\icon_tri";
            const bool sharedGarageIcon=proof->name_=="ui\\textures\\icon_sqr"||proof->name_=="ui\\textures\\icon_cir";
            proof->pstArchive_=assets::OriginalUiArchiveId50KG::garage;
            const auto rejectFallback=[&](const char* stage)noexcept {
                std::fprintf(stderr,"original_garage_prompt_fallback_failed stage=%s name=%s row=%d material_status=%u material_pc=%u\n",
                    stage,requested.c_str(),row,static_cast<unsigned>(d.material.status),d.material.sourcePC);
                std::fflush(stderr);
                return fail(d,Status::missingPsmFallbackPending,0x29B638);
            };
            assets::OriginalStandardArchiveProvider50LA::MemberView pst;
            if(provider->lookup132820(proof->name_+".pst",pst)!=assets::OriginalStandardArchiveProvider50LA::Lookup::present||!pst.member||!pst.data)
                return rejectFallback("pst_lookup");
            if(sharedIcon||sharedGarageIcon){
                //132820 retains the first active equal-byte member. Dedicated
                //Main already mounted CONTPOP at real shared-dialog boot;
                //race-hosted UI may have mounted OVERLAYS first. X/Triangle
                //are equal in CONTPOP/OVERLAYS; Square/Circle are equal in
                //Garage/OVERLAYS and current() checks the Garage bytes.
                assets::OriginalStandardArchiveProvider50LA::MountView mount;
                if(!provider->mount(pst.mount,mount)||!mount.archiveSha256)
                    return rejectFallback("pst_mount");
                const auto matches=[&](assets::OriginalUiArchiveId50KG id)noexcept {
                    const auto* expected=assets::originalUiArchiveIdentity50KG(id);
                    return expected&&*mount.archiveSha256==expected->sha256&&mount.archiveBytes==expected->bytes;
                };
                if(sharedIcon&&matches(assets::OriginalUiArchiveId50KG::controllerPopup))
                    proof->pstArchive_=assets::OriginalUiArchiveId50KG::controllerPopup;
                else if(matches(assets::OriginalUiArchiveId50KG::overlays))
                    proof->pstArchive_=assets::OriginalUiArchiveId50KG::overlays;
                else if(sharedGarageIcon&&matches(assets::OriginalUiArchiveId50KG::garage))
                    proof->pstArchive_=assets::OriginalUiArchiveId50KG::garage;
                else {
                    std::fprintf(stderr,"original_garage_prompt_pst_archive name=%s sha=%s bytes=%zu member=%zu\n",
                        mount.archiveName?mount.archiveName->c_str():"<null>",mount.archiveSha256->c_str(),mount.archiveBytes,pst.ordinal);
                    return rejectFallback("pst_archive");
                }
            }
            proof->pstMember_=*pst.member;proof->pstBytes_.assign(pst.data,pst.data+pst.size);
            if(!proof->current(proof->name_+".psm",context_,proof->textures_))
                return rejectFallback("proof_current");
            if(!table->bindGarageScreenFallbackAfterRetain(row,proof,d.material))
                return rejectFallback("table_bind");
            appendTextureInput(proof->name_);
        }
        if(!domain_->appendOriginalSources(sources,textures,d.material))
            return fail(d,Status::material,d.material.sourcePC);
        if(!domain_->populateGarageResources2DF144(context_,d.material))
            return fail(d,Status::material,d.material.sourcePC?d.material.sourcePC:0x2DF144);
        stage_=Stage::populated;
        if(!completeZeroPosition2DF158(d))return fail(d,d.status,d.sourcePC);
        return true;
    }catch(...){return fail(d,Status::allocation,resource58?0x2DF1D4:material54?0x2DF11C:0x2DF0E8);}
}
bool OriginalUiGarageQuad50KG::completeZeroPosition2DF158(Diagnostic& d)noexcept {
    d={};const auto stop=[&](Status status,std::uint32_t pc)noexcept {d.status=status;d.sourcePC=pc;return false;};
    if(!constructed||(stage_!=Stage::populated&&stage_!=Stage::dimensionsStored)||
       !domain_||!domain_->prepared()||!context_||!context_->live()||!video_||!video_->originalMain||video_->packetZMaximum!=0xffffff||
       !material54||material54!=primitiveOwner_.get()||!material54->materialStored||!material54->materialRetained||
       material54->domain!=domain_||material54->context!=context_||material54->type0C!=15||
       material54->optional04||material54->optional14||(material54->flags0F&1u)||material54->packet10||
       resource58||packetOwner_||dmaData4_||packetRevision_||nativeGarageFifoPins_||nativeMenuWarningPins_||
       std::fegetround()!=FE_TONEAREST||!std::isfinite(field40))return stop(Status::phase,0x2DF158);
    try {
        const auto table=domain_->materials382B38();const auto row=material54->material00;
        assets::OriginalStandardMaterialRowView50LA material;
        assets::OriginalStandardMaterialPassView50LA pass;
        assets::OriginalStandardTextureRowView50LA texture;
        if(!table->row(row,material)||!material.object04||!material.object04->pass4C(0,pass)||
           !pass.texture1A||!pass.mapCount20||!*pass.mapCount20||
           !domain_->textures383400()->row((*pass.texture1A)[0],texture)||!texture.object04||
           !texture.object04->sourceWidth14()||!texture.object04->sourceHeight18())return stop(Status::texture,0x2DF178);
        const auto width=*texture.object04->sourceWidth14(),height=*texture.object04->sourceHeight18();
        if(width<=0||height<=0||width>0x1000000||height>0x1000000)return stop(Status::arithmetic,0x2DF194);
        volatile float w=float(width),h=float(height),reciprocalW=1.0f/w,reciprocalH=1.0f/h;
        if(stage_==Stage::populated){
            scale4C=reciprocalW;scale50=reciprocalH;uv30[1]=w;uv30[2]=h;position10[0]=position10[1]=0;
            stage_=Stage::dimensionsStored;pendingPC_=0x2DF1CC;d.sourceEffectsCommitted=true;
        }else if(scale4C!=reciprocalW||scale50!=reciprocalH||uv30[1]!=w||uv30[2]!=h||position10[0]!=0||position10[1]!=0)
            return stop(Status::phase,0x2DF1CC);
        // The retained stores above survive allocation failure. A retry starts
        // here only after exact current owner/bindings/dimensions validation.
        auto* packet=static_cast<Qword*>(::operator new[](17*sizeof(Qword),std::align_val_t{64}));
        packetOwner_.reset(packet);resource58=packet;stage_=Stage::packetPublished;
        for(std::size_t i=0;i<17;++i)std::construct_at(packet+i);
        packet[0].words={0x30000010,0,0,0};dmaData4_=packet+1;
        material54->packet10=packet;packet[1].words={0x11000000,0x03000000,0x02000200,0x6C0E8000};
        packet[2].words={0x00008004,0x300E4000,0x00000512,0};
        packet[3].words={0,0,0x3F800000,0};
        const std::array<std::uint32_t,4> rgba{(color44>>16&255u)>>1,(color44>>8&255u)>>1,
            (color44&255u)>>1,std::uint32_t(std::int16_t(alpha48)>>2)};
        volatile float z=float(video_->packetZMaximum)-field40;const auto zbits=std::bit_cast<std::uint32_t>(float(z));
        constexpr std::array<std::array<std::uint32_t,4>,4> stq{{{0,0,0x3F800000,0},
            {0x3F800000,0,0x3F800000,0},{0,0x3F800000,0x3F800000,0},{0x3F800000,0x3F800000,0x3F800000,0}}};
        for(std::size_t i=0;i<4;++i){packet[4+3*i].words=rgba;
            packet[5+3*i].words={0,0,zbits,0x3F800000};packet[6+3*i].words=stq[i];}
        packet[16].words={0x140000B0,0x10000000,0,0};flags5CValue|=1;flags5CKnownMask|=1;
        stage_=Stage::loaded;packetRevision_=1;pendingPC_=0;d.sourceEffectsCommitted=true;d.sourcePC=0x2DF54C;return true;
    }catch(...){return stop(Status::allocation,0x2DF1CC);}
}
bool OriginalUiGarageQuad50KG::completeMemoryCardStrip2DF158(
    OriginalUiMemoryCardConstructor50KG& owner,Diagnostic& d,std::size_t role)noexcept {
    d={};std::int32_t row=-1;const auto table=owner.domain_?owner.domain_->materials382B38():nullptr;
    if(role<4||role>6||!table||!owner.stripOccurrenceCurrent(*table,row,role)||
       &owner.quads_[owner.loadRoles_[role].constructionIndex]!=this||!memoryCardPrefixOnly_||
       offset!=owner.loadRoles_[role].offset||garage_||
       (stage_!=Stage::populated&&stage_!=Stage::dimensionsStored)||
       !table->preflightMemoryCardStripBinding(owner,d.material,role)){
        d.status=Status::phase;d.sourcePC=d.material.sourcePC?d.material.sourcePC:0x2DF158;return false;
    }
    for(std::size_t prior=0;prior<4;++prior)
        if(!owner.completedIconStateCurrent(prior)||!table->preflightMemoryCardIconBinding(owner,prior,d.material)){
            d.status=Status::phase;d.sourcePC=d.material.sourcePC?d.material.sourcePC:owner.iconProgressPC(prior);return false;
        }
    return completeZeroPosition2DF158(d);
}
bool OriginalUiGarageQuad50KG::completeMemoryCardFirstIcon2DF158(OriginalUiMemoryCardConstructor50KG& owner,Diagnostic& d)noexcept {
    return completeMemoryCardIcon2DF158(owner,0,d);
}
bool OriginalUiGarageQuad50KG::completeMemoryCardIcon2DF158(
    OriginalUiMemoryCardConstructor50KG& owner,std::size_t role,Diagnostic& d)noexcept {
    d={};std::int32_t row=-1;const auto table=owner.domain_?owner.domain_->materials382B38():nullptr;
    if(role>3||!table||!owner.iconOccurrenceCurrent(role,*table,row)||
       &owner.quads_[owner.loadRoles_[role].constructionIndex]!=this||!memoryCardPrefixOnly_||offset!=owner.loadRoles_[role].offset||garage_||
       (stage_!=Stage::populated&&stage_!=Stage::dimensionsStored)||
       !table->preflightMemoryCardIconBinding(owner,role,d.material)){
        d.status=Status::phase;d.sourcePC=d.material.sourcePC?d.material.sourcePC:0x2DF158;return false;
    }
    for(std::size_t prior=0;prior<role;++prior)
        if(!owner.completedIconStateCurrent(prior)||!table->preflightMemoryCardIconBinding(owner,prior,d.material)){
            d.status=Status::phase;d.sourcePC=d.material.sourcePC?d.material.sourcePC:owner.iconProgressPC(prior);return false;
        }
    return completeZeroPosition2DF158(d);
}
bool OriginalUiGarageQuad50KG::preflightPrimitive246260(const std::unique_ptr<Primitive>& p,Diagnostic& d)noexcept {
    if(!p)return true;
    if(p->optional04||p->optional14||(p->flags0F&1u)||
       (p->materialStored&&p->material00!=-1&&!p->materialRetained)){
        d.status=Status::phase;d.sourcePC=0x246288;return false;
    }
    if(!p->materialRetained)return true;
    assets::OriginalStandardMaterialRowView50LA row;
    if(!p->domain||!p->context||!p->context->live()||
       !p->domain->materials382B38()->canRetireNativeCollector29C300()||
       !p->domain->materials382B38()->row(p->material00,row)||!row.references08||*row.references08<=0){
        d.status=Status::material;d.sourcePC=0x14D500;return false;
    }
    return true;
}
void OriginalUiGarageQuad50KG::releasePrimitive246260(std::unique_ptr<Primitive>& p)noexcept {
    if(!p)return;
    if(p->optional04||p->optional14||(p->flags0F&1u))std::terminate();
    if(p->materialRetained){
        if(!p->domain||!p->domain->materials382B38()->release14D500(p->material00))std::terminate();
        p->materialRetained=false;
    }
    p.reset(); //2462C4->299AC0, then2462DC. Borrowed packet10 is not freed here.
}
bool OriginalUiGarageQuad50KG::preflightRetirement2DEFE0(Diagnostic& d)const noexcept {
    d={};if(stage_==Stage::retired)return true;
    if(nativeGarageFifoPins_||nativeMenuWarningPins_||!constructed||material54!=primitiveOwner_.get()||resource58!=packetOwner_.get()||
       (material54&&material54->packet10&&material54->packet10!=resource58)){
        d.status=Status::phase;d.sourcePC=0x2DEFE0;return false;
    }
    return preflightPrimitive246260(primitiveOwner_,d);
}
bool OriginalUiGarageQuad50KG::retireResources2DF038(Diagnostic& d)noexcept {
    if(!preflightRetirement2DEFE0(d))return false;
    if(stage_==Stage::retired)return true;
    //Only the Page/shared-dialog friends advance this phase, from actual SAME-Display
    //244D60(1) and unconditional245090 completions, in that order.
    if(retirementDrainPhase_!=2){d.status=Status::rendererRetirementPending;d.sourcePC=0x2DF018;return false;}
    packetOwner_.reset(); //2DF044. Source58 remains a stale, never-dereferenced observation.
    if(material54)material54->packet10=nullptr; //2DF058, AFTER its borrowed packet was freed.
    releasePrimitive246260(primitiveOwner_); //2DF070. Source54 is likewise not cleared.
    dmaData4_=nullptr;domain_.reset();context_.reset();video_.reset();garage_.reset();
    constructed=false;stage_=Stage::retired;pendingPC_=0;d.sourceEffectsCommitted=true;d.sourcePC=0x2DF094;return true;
}
}

namespace rr::platform {
namespace {
struct QuadScalar2DF550 {
    bool okay=true;
    float read(float x)noexcept{if(!std::isfinite(x))okay=false;return x;}
    float add(float a,float b)noexcept{volatile float x=read(a)+read(b);return read(x);}
    float sub(float a,float b)noexcept{volatile float x=read(a)-read(b);return read(x);}
    float mul(float a,float b)noexcept{volatile float x=read(a)*read(b);return read(x);}
    float word(std::uint32_t x)noexcept{return std::bit_cast<float>(x);}
    float unsignedWord(std::uint32_t x)noexcept {
        // Exact unsigned conversion branch2DF600/648, including its folded
        // low bit before signed CVT.S.W and separately rounded doubling.
        if(x<=0x7fffffffu){volatile float v=static_cast<float>(static_cast<std::int32_t>(x));return read(v);}
        volatile float v=static_cast<float>(static_cast<std::int32_t>((x>>1u)|(x&1u)));
        return add(v,v);
    }
    void sincos13BFE0(float angle,float& sine,float& cosine)noexcept {
        float x=read(angle),sign=1.f;
        if(x>word(0x3fc90fdbu)){x=sub(word(0x40490fdbu),x);sign=-1.f;}
        else if(x<word(0xbfc90fdbu)){x=sub(word(0xc0490fdbu),x);sign=-1.f;}
        const auto square=mul(x,x);
        float c=mul(word(0xb493f27eu),square);
        c=add(word(0x37d00d01u),c);c=mul(square,c);c=add(word(0xbab60b61u),c);
        c=mul(square,c);c=add(word(0x3d2aaaabu),c);c=mul(square,c);
        c=add(-.5f,c);c=mul(square,c);c=add(1.f,c);cosine=mul(sign,c);
        float s=mul(word(0x3638ef1du),square);
        s=add(word(0xb9500d01u),s);s=mul(square,s);s=add(word(0x3c088889u),s);
        s=mul(square,s);s=add(word(0xbe2aaaabu),s);s=mul(mul(x,square),s);sine=add(x,s);
    }
};
}
bool OriginalUiGarageQuad50KG::packetCurrent2DF550()const noexcept {
    return constructed&&stage_==Stage::loaded&&packetRevision_&&domain_&&domain_->prepared()&&
        context_&&context_->live()&&video_&&video_->originalMain&&garage_&&garage_->loaded()&&
        material54&&material54==primitiveOwner_.get()&&material54->materialStored&&material54->materialRetained&&
        material54->material00>=0&&material54->type0C==15&&material54->domain==domain_&&material54->context==context_&&
        !material54->optional04&&!material54->optional14&&!(material54->flags0F&1u)&&
        resource58&&resource58==packetOwner_.get()&&material54->packet10==resource58&&dmaData4_==resource58+1;
}
bool OriginalUiGarageQuad50KG::canMutatePacket2DF550()const noexcept {
    return packetCurrent2DF550()&&!nativeGarageFifoPins_&&!nativeMenuWarningPins_&&packetRevision_!=std::numeric_limits<std::uint64_t>::max();
}
void OriginalUiGarageQuad50KG::initializeDrawAssertion2DF550()noexcept {
    // One real process-local lazy assertion history, not one copy per Quad.
    static std::atomic<std::uint8_t> guard8F10{0};static std::atomic<std::uint32_t> assertion444770{0};
    if(!guard8F10.load(std::memory_order_acquire)){
        assertion444770.store(1,std::memory_order_relaxed);guard8F10.store(1,std::memory_order_release);
    }
}
bool OriginalUiGarageQuad50KG::regeneratePacket2DF550(std::uint32_t viewportWidth,
    std::uint32_t viewportHeight,Diagnostic& d)noexcept {
    const auto stop=[&](Status status,std::uint32_t pc){d.status=status;d.sourcePC=pc;pendingPC_=pc;return false;};
    if(!canMutatePacket2DF550()||(flags5CKnownMask&3u)!=3u||std::fegetround()!=FE_TONEAREST)
        return stop(Status::phase,0x2df634u);
    QuadScalar2DF550 arithmetic;
    const float width=arithmetic.unsignedWord(viewportWidth);
    const float height=arithmetic.unsignedWord(viewportHeight);
    // These viewport writes happen even when position is not dirty. Their
    // retained packet mutation is visible if a later numeric/collector fails.
    resource58[3].words[0]=std::bit_cast<std::uint32_t>(width);
    resource58[3].words[1]=std::bit_cast<std::uint32_t>(height);
    ++packetRevision_;d.sourceEffectsCommitted=true;
    if(flags5CValue&1u){
        const float x=arithmetic.read(position10[0]),y=arithmetic.read(position10[1]);
        const float right=arithmetic.add(x,arithmetic.mul(uv30[1],uv30[0]));
        const float bottom=arithmetic.add(y,arithmetic.mul(uv30[2],uv30[0]));
        std::array<std::array<float,2>,4> points{{{x,y},{right,y},{x,bottom},{right,bottom}}};
        if(flags5CValue&2u){
            float sine=0,cosine=0;arithmetic.sincos13BFE0(uv30[3],sine,cosine);
            const float left=arithmetic.sub(x,size20[0]),top=arithmetic.sub(y,size20[1]);
            const float r=arithmetic.sub(right,size20[0]),b=arithmetic.sub(bottom,size20[1]);
            const std::array<std::array<float,2>,4> relative{{{left,top},{r,top},{left,b},{r,b}}};
            for(std::size_t i=0;i<4;++i){
                points[i][0]=arithmetic.add(size20[0],arithmetic.sub(arithmetic.mul(relative[i][0],cosine),arithmetic.mul(relative[i][1],sine)));
                points[i][1]=arithmetic.add(size20[1],arithmetic.add(arithmetic.mul(relative[i][0],sine),arithmetic.mul(relative[i][1],cosine)));
            }
        }
        if(!arithmetic.okay)return stop(Status::arithmetic,0x2df6b8u);
        for(std::size_t i=0;i<4;++i){
            resource58[5+3*i].words[0]=std::bit_cast<std::uint32_t>(points[i][0]);
            resource58[5+3*i].words[1]=std::bit_cast<std::uint32_t>(points[i][1]);
        }
        if(flags5CValue&2u)flags5CValue&=std::uint8_t(~2u); //2DF808, only actual rotation branch.
        flags5CValue&=std::uint8_t(~1u); //2DF860, after all actual XY stores.
    }
    return true;
}
bool OriginalUiGarageQuad50KG::draw2DF550(OriginalUiGarageDrawContribution50KG& draw,Diagnostic& d)noexcept {
    d={};const auto stop=[&](Status status,std::uint32_t pc){d.status=status;d.sourcePC=pc;pendingPC_=pc;return false;};
    initializeDrawAssertion2DF550();
    if(!draw.quadInvocation2DF550Current(*this))return stop(Status::phase,0x2df5b4u);
    OriginalUiGarageDrawContribution50KG::QuadRenderInputs input;std::uint32_t pc=0;
    if(!draw.readQuadInputs2DF550(*this,input,pc))return stop(Status::phase,pc?pc:0x2df5e8u);
    if(!input.sourceQuadEnabled383678){d.sourcePC=0x2df87cu;return true;}
    if(!regeneratePacket2DF550(input.viewportWidth1E8,input.viewportHeight1EC,d))return false;
    if(!draw.collectQuadPrimitive246760(*this,d.material)){
        d.sourceEffectsCommitted=d.sourceEffectsCommitted||d.material.sourceEffectsCommitted;
        return stop(Status::material,d.material.sourcePC?d.material.sourcePC:0x2df874u);
    }
    pendingPC_=0;d.sourcePC=0x2df87cu;return true;
}
bool OriginalUiGarageQuad50KG::drawWarning4702DF550(OriginalUiGarageDrawContribution50KG& draw,
    OriginalUiSharedDialogs50KG& shared,Diagnostic& d)noexcept {
    d={};const auto stop=[&](Status status,std::uint32_t pc){d.status=status;d.sourcePC=pc;pendingPC_=pc;return false;};
    initializeDrawAssertion2DF550();
    if(!shared.warning470QuadCurrent(draw,*this))return stop(Status::phase,0x2df5b4u);
    OriginalUiGarageDrawContribution50KG::QuadRenderInputs input;std::uint32_t pc=0;
    if(!draw.readWarning470QuadInputs2DF550(shared,*this,input,pc))return stop(Status::phase,pc?pc:0x2df5e8u);
    // The same source process word, not warning visibility or a page exception.
    if(!input.sourceQuadEnabled383678){d.sourcePC=0x2df87cu;return true;}
    if(!regeneratePacket2DF550(input.viewportWidth1E8,input.viewportHeight1EC,d))return false;
    if(!draw.collectWarning470Quad246760(shared,*this,d.material)){
        d.sourceEffectsCommitted|=d.material.sourceEffectsCommitted;
        return stop(Status::material,d.material.sourcePC?d.material.sourcePC:0x2df874u);
    }
    pendingPC_=0;d.sourcePC=0x2df87cu;return true;
}
bool OriginalUiGarageQuad50KG::lowerPacket327E90(std::uint64_t revision,
    const std::array<std::uint32_t,2>& xyOffset,LoweredPacket327E90& out,Diagnostic& d)const noexcept {
    d={};if(!nativeGarageFifoPins_){d.status=Status::phase;d.sourcePC=0x3281e8u;return false;}
    return lowerRetainedPacketBody327E90(revision,xyOffset,out,d);
}
bool OriginalUiGarageQuad50KG::lowerRetainedPacketBody327E90(std::uint64_t revision,
    const std::array<std::uint32_t,2>& xyOffset,LoweredPacket327E90& out,Diagnostic& d)const noexcept {
    d={};const auto stop=[&](Status status,std::uint32_t pc){d.status=status;d.sourcePC=pc;return false;};
    if(!packetCurrent2DF550()||!revision||packetRevision_!=revision||
       (flags5CValue&1u)||std::fegetround()!=FE_TONEAREST||xyOffset[0]>65535u||xyOffset[1]>65535u)
        return stop(Status::phase,0x3281e8u);
    // Actual constructor packet/giftag, not a newly invented material quad.
    // Source4 vertices and mode1 select the original VU116 strip path.
    if(resource58[0].words!=std::array<std::uint32_t,4>{{0x30000010u,0,0,0}}||
       resource58[1].words!=std::array<std::uint32_t,4>{{0x11000000u,0x03000000u,0x02000200u,0x6c0e8000u}}||
       resource58[2].words!=std::array<std::uint32_t,4>{{0x00008004u,0x300e4000u,0x00000512u,0}}||
       resource58[3].words[2]!=0x3f800000u||resource58[3].words[3]||
       resource58[16].words!=std::array<std::uint32_t,4>{{0x140000b0u,0x10000000u,0,0}})
        return stop(Status::phase,0x327ed8u);
    QuadScalar2DF550 arithmetic;
    const float width=arithmetic.word(resource58[3].words[0]),height=arithmetic.word(resource58[3].words[1]);
    if(!std::isfinite(width)||!std::isfinite(height)||width<=0||height<=0)
        return stop(Status::arithmetic,0x327f40u);
    const std::array<float,2> half{{arithmetic.mul(width,.5f),arithmetic.mul(height,.5f)}};
    std::array<assets::Vertex48B,4> vertices{};
    std::array<std::array<float,3>,4> stq{};
    for(std::size_t i=0;i<4;++i){
        const auto& rgba=resource58[4+3*i].words;const auto& xyz=resource58[5+3*i].words;
        const auto& texture=resource58[6+3*i].words;
        for(std::size_t lane=0;lane<4;++lane){
            if(rgba[lane]>(lane==3?128u:255u))return stop(Status::arithmetic,0x3282f0u);
            vertices[i].color[lane]=static_cast<std::uint8_t>(rgba[lane]);
        }
        for(std::size_t lane=0;lane<2;++lane){
            // VU123..12A ADD2048 then SUBviewport/2, each float32-rounded.
            // FTOI4 is truncation, not host pixel rounding or canvas clipping.
            const float gs=arithmetic.sub(arithmetic.add(arithmetic.word(xyz[lane]),2048.f),half[lane]);
            const float fixed=std::trunc(arithmetic.mul(gs,16.f));
            if(!std::isfinite(fixed)||fixed<0.f||fixed>65535.f)
                return stop(Status::arithmetic,0x328298u);
            vertices[i].position[lane]=(fixed-static_cast<float>(xyOffset[lane]))/16.f;
        }
        const float z=std::trunc(arithmetic.word(xyz[2]));
        if(!std::isfinite(z)||z<0.f||z>16777215.f||xyz[3]!=0x3f800000u)
            return stop(Status::arithmetic,0x3282a0u);
        vertices[i].position[2]=z;
        for(std::size_t lane=0;lane<3;++lane)stq[i][lane]=arithmetic.read(arithmetic.word(texture[lane]));
        // This actual embedded ScreenTexture constructor has copy-only Q1.
        // A foreign perspective packet cannot impersonate its screen UV path.
        if(texture[2]!=0x3f800000u||texture[3]||!arithmetic.okay)
            return stop(Status::arithmetic,0x328200u);
        vertices[i].uv[0]=stq[i][0];vertices[i].uv[1]=stq[i][1];
    }
    LoweredPacket327E90 next;
    // VU136..141 emits TL,BL,TR,BR; first two have ADC. Preserve those TWO
    // actual triangles and the BL--TR diagonal with degenerate native tails.
    constexpr std::array<std::size_t,8> order{{0,2,1,1,1,2,3,3}};
    for(std::size_t i=0;i<order.size();++i){next.vertices[i]=vertices[order[i]];
        next.stq[i]=stq[order[i]];next.rasterW[i]=1.f;next.fog[i]=0.f;}
    next.quadCount=2;out=next;d.sourcePC=0x328378u;return true;
}
}

namespace rr::platform {
bool OriginalUiGarageQuad50KG::drawMenuWarning4702DF550(
    OriginalUiSharedDialogs50KG& shared,Diagnostic& d)noexcept {
    d={};const auto stop=[&](Status status,std::uint32_t pc){d.status=status;d.sourcePC=pc;pendingPC_=pc;return false;};
    initializeDrawAssertion2DF550();
    if(!shared.menuWarning470QuadCurrent(*this))return stop(Status::phase,0x2df5b4u);
    OriginalUiSharedDialogs50KG::MenuWarningQuadInputs input;std::uint32_t pc=0;
    if(!shared.readMenuWarningQuadInputs2DF550(*this,input,pc))return stop(Status::phase,pc?pc:0x2df5e8u);
    if(!input.sourceQuadEnabled383678){d.sourcePC=0x2df87cu;return true;}
    if(!regeneratePacket2DF550(input.viewportWidth1E8,input.viewportHeight1EC,d))return false;
    const bool effects=d.sourceEffectsCommitted;
    if(!shared.collectMenuWarning470Quad(*this,d)){d.sourceEffectsCommitted|=effects;return false;}
    d.sourceEffectsCommitted|=effects;pendingPC_=0;d.sourcePC=0x2df87cu;return true;
}
bool OriginalUiGarageQuad50KG::retainMenuWarningPacket(OriginalUiSharedDialogs50KG& shared,
    OriginalUiMenuWarningPacket50KG& receipt,Diagnostic& d)noexcept {
    d={};
    if(!shared.menuWarning470PacketCaptureCurrent(receipt)||!shared.menuWarning470QuadCurrent(*this)||
       receipt.owner_!=&shared||receipt.kind_!=OriginalUiMenuWarningPacket50KG::Kind::quad||receipt.active_||
       nativeGarageFifoPins_||nativeMenuWarningPins_||!packetCurrent2DF550()||!packetRevision_||
       domain_!=shared.inputs_.domain||context_!=shared.inputs_.context||video_!=shared.inputs_.video||
       garage_!=shared.inputs_.popup||
       (flags5CValue&1u)||!material54||material54!=primitiveOwner_.get()||
       !material54->materialStored||!material54->materialRetained||material54->material00!=3229||
       material54->optional04||material54->optional14||(material54->flags0F&1u)){
        d.status=Status::phase;d.sourcePC=0x2DF8B0u;return false;
    }
    receipt.payload_=this;receipt.primitive_=material54;receipt.packet_=resource58;
    receipt.primitiveIndex_=0;receipt.packetRevision_=packetRevision_;receipt.materialToken_=material54->material00;
    ++nativeMenuWarningPins_;receipt.active_=true;return true;
}
bool OriginalUiGarageQuad50KG::menuWarningPacketMatches(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiMenuWarningPacket50KG& receipt)const noexcept {
    return receipt.active_&&receipt.owner_==&shared&&receipt.kind_==OriginalUiMenuWarningPacket50KG::Kind::quad&&
       receipt.payload_==this&&nativeMenuWarningPins_&&receipt.packetRevision_&&receipt.packetRevision_==packetRevision_&&
       receipt.primitiveIndex_==0&&receipt.primitive_==material54&&material54&&
       receipt.packet_==resource58&&resource58==packetOwner_.get()&&material54->packet10==resource58&&
       receipt.materialToken_==material54->material00;
}
bool OriginalUiGarageQuad50KG::releaseMenuWarningPacket(OriginalUiMenuWarningPacket50KG& receipt)noexcept {
    if(!receipt.owner_||!menuWarningPacketMatches(*receipt.owner_,receipt))return false;
    --nativeMenuWarningPins_;receipt.active_=false;return true;
}
bool OriginalUiGarageQuad50KG::lowerMenuWarningPacket(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiMenuWarningPacket50KG& receipt,const std::array<std::uint32_t,2>& offset,
    OriginalUiMenuWarningGeometry50KG& out,Diagnostic& d)const noexcept {
    d={};if(!shared.menuWarning470PacketOffsetCurrent(receipt,offset)||!menuWarningPacketMatches(shared,receipt)){
        d.status=Status::phase;d.sourcePC=0x3281E8u;return false;
    }
    LoweredPacket327E90 next;if(!lowerRetainedPacketBody327E90(receipt.packetRevision_,offset,next,d))return false;
    try{
        OriginalUiMenuWarningGeometry50KG geometry;
        geometry.vertices.assign(next.vertices.begin(),next.vertices.end());geometry.stq.assign(next.stq.begin(),next.stq.end());
        geometry.rasterW.assign(next.rasterW.begin(),next.rasterW.end());geometry.fog.assign(next.fog.begin(),next.fog.end());
        geometry.quadCount=next.quadCount;out=std::move(geometry);return true;
    }catch(...){d.status=Status::allocation;d.sourcePC=0x328378u;return false;}
}
}

namespace rr::platform {
bool OriginalUiGarageQuad50KG::drawPopup47C2DF550(OriginalUiGarageDrawContribution50KG& draw,
    OriginalUiSharedDialogs50KG& shared,Diagnostic& d)noexcept {
    d={};const auto stop=[&](Status status,std::uint32_t pc){d.status=status;d.sourcePC=pc;pendingPC_=pc;return false;};
    initializeDrawAssertion2DF550();
    if(!shared.popup47CQuadCurrent(draw,*this))return stop(Status::phase,0x2df5b4u);
    OriginalUiGarageDrawContribution50KG::QuadRenderInputs input;std::uint32_t pc=0;
    if(!draw.readPopup47CQuadInputs2DF550(shared,*this,input,pc))return stop(Status::phase,pc?pc:0x2df5e8u);
    // The same source process word, not warning visibility or a page exception.
    if(!input.sourceQuadEnabled383678){d.sourcePC=0x2df87cu;return true;}
    if(!regeneratePacket2DF550(input.viewportWidth1E8,input.viewportHeight1EC,d))return false;
    if(!draw.collectPopup47CQuad246760(shared,*this,d.material)){
        d.sourceEffectsCommitted|=d.material.sourceEffectsCommitted;
        return stop(Status::material,d.material.sourcePC?d.material.sourcePC:0x2df874u);
    }
    pendingPC_=0;d.sourcePC=0x2df87cu;return true;
}
}
