#include "original_results104_storage50la.h"
#include "../assets/original_ui_screen_assets50kg.h" // DLC add-on Results icon image (PORT EXTENSION)
#include "../../tests/native/original_results_live_owner_read50la.h"
#include "original_ui_text50kg.h"
#include "original_ui_text_position50kg.h"
#include "../assets/original_standard_material_domain50la.h"
#include "../assets/original_standard_material_table50la.h"
#include "../assets/original_standard_material_object50la.h"
#include "../assets/original_application_localization50kg.h"
#include "../assets/original_standard_texture_resource50la.h"
#include <cstdio>
#include <cmath>

namespace rr::game {
namespace {
// Source scalar instruction boundaries: do not contract multiply/add pairs.
float add(float a,float b)noexcept {volatile float r=a+b;return r;}
float sub(float a,float b)noexcept {volatile float r=a-b;return r;}
float mul(float a,float b)noexcept {volatile float r=a*b;return r;}
float div(float a,float b)noexcept {volatile float r=a/b;return r;}
bool nativeVertex(const OriginalPauseStorage50LA::PacketVertex& in,assets::Vertex48B& out)noexcept {
    out={};
    for(std::size_t i=0;i<3;++i){if(!std::isfinite(in.xyz[i]))return false;out.position[i]=in.xyz[i];}
    if(in.xyz[2]!=16777214.f)return false;
    for(std::size_t i=0;i<2;++i){if(!std::isfinite(in.stq[i]))return false;out.uv[i]=in.stq[i];}
    for(std::size_t i=0;i<4;++i){if(in.rgba[i]>255)return false;out.color[i]=std::uint8_t(in.rgba[i]);}
    return true;
}
} // anonymous scalar/vertex helpers

bool OriginalResults10450LA::demandNativeFinalCamera1C15B0(
    const ::original_main_entry50KG::Application& root,const OriginalRaceConstructor50LA& race,
    const ::original_camera_session50KX::Session& camera,
    const ::original_position_stats_adapter50LA::Owner& observer,OriginalResults104Diagnostic50LA& d)noexcept {
    using Read=::original_results_roster50LA::Read;
    const auto liveRace=Read::raceAccess(race);
    const auto liveRoot=Read::rootAccess(root);
    d={};
    if(liveRoot.garageAudioOwnerThread_!=std::this_thread::get_id()||
       liveRace.resultsFeedbackThread_!=std::this_thread::get_id()||!impl_||
       impl_->nativeCaptureThread!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1c15b0;return false;
    }
    auto& p=*impl_;
    const auto stop=[&](std::uint32_t pc,const char* why){return p.stop(d,Impl::S::owner,pc,why);};
    if(liveRace.results104_.get()!=this||liveRace.application_!=p.application||liveRoot.entitiesOwner_!=p.application||
       liveRace.entity_.sessionGeneration!=p.component.sessionGeneration||!constructed()||!p.loadComplete||
       !retirementCurrent()||p.nativeFrameBorrows||!p.context||!p.context->live()||!p.domain||
       p.domain!=liveRoot.standardMaterialDomainOwner_||!p.fonts||p.fonts!=liveRoot.fonts328Owner_||
       p.fonts->domain_!=p.domain||p.fonts->context_!=p.context||!p.backdropsA0||!p.icons210||
       !(p.component.flags29&1u)||!demandObserverCameraMatches1C15B0(observer,camera))
        return stop(0x1c15b0,"Results slot40 exact same loaded source owners absent");
    original_ui_source_timing50LH::Sample sample;std::uint64_t publication=0;
    if(!liveRoot.nativeTaRootTiming_.readPrepared(sample,publication)||publication!=liveRoot.nativeTaRootPublication_||
       !p.updateCpu||publication!=p.updateCpu->publication||
       p.updateCpu->prefix!=UpdateCpuPrefix::baseReturned||p.updateCpu->nextSourcePC!=0x1c8c1c||
       publication<=p.lastNativeDemandAttemptPublication)
        return stop(0x1c15b0,"Results slot40 exact unreplayed same-Root prepared publication absent");
    const auto materials=p.domain->materials382B38();const auto textures=p.domain->textures383400();
    if(!materials||!textures||!p.context->materialApplication()||
       !materials->screenApplicationMatches(*p.context->materialApplication()))
        return stop(0x1c15b0,"Results slot40 same canonical material/texture tables absent");
    // Actual source first compares App0C against264-1 BEFORE reading B4.
    // This bounded native adapter supports only the actual final single-human
    // selected Camera. It never writes those absent PS2 lanes or assertsmode3.
    if(!liveRoot.finalCameraCurrent(race,camera))
        return stop(0x1c15e4,"Results supported final native Camera observation absent");
    p.lastNativeDemandAttemptPublication=publication;
    const auto demand=[&](std::int16_t token,const assets::OriginalStandardTextureResource50LA* expected,
        std::uint32_t pc){
        assets::OriginalStandardTextureRowView50LA row;assets::OriginalStandardTextureResource50LA* actual=nullptr;
        assets::OriginalStandardTextureDiagnostic50LA td;
        if(!expected||!textures->row(token,row)||!row.references08||*row.references08<=0||row.object04!=expected||
           !textures->resolve298E60(token,actual,td)||actual!=expected)
            return stop(pc,"Results slot40 exact canonical texture reference/identity changed");
        bool written=false;
        if(!actual->demandResults1C15B0(pc,written,td))return stop(td.sourcePC,"Results reached texture demand floor rejected");
        return true;
    };
    const auto texture=[&](const Impl::Texture& t,std::uint32_t pc){
        if(!t.primitive54)return stop(0x2df8a0,"Results slot40 actual texture primitive absent");
        const auto* ref=p.references.reference(t.primitive54->reference);
        if(ref&&ref->absent)return true; // add-on portrait: no canonical texture to demand (PORT EXTENSION)
        const assets::OriginalStandardMaterialObject50LA* material=nullptr;
        assets::OriginalStandardMaterialRowView50LA row;assets::OriginalStandardMaterialPassView50LA pass;
        if(!ref||!ref->retained||!t.buffer58||t.primitive54->packet10!=t.buffer58.get()||
           !materials->row(ref->material00,row)||!row.references08||*row.references08<=0||
           !materials->resolve298E60(ref->material00,material,p.materialDiagnostic)||!material||row.object04!=material||
           !material->constructed29B550()||material->textures383400()!=textures.get()||
           !material->pass4C(0,pass)||!pass.mapCount20||!*pass.mapCount20||!pass.texture1A||(*pass.texture1A)[0]<0)
            return stop(0x2df8a0,"Results slot40 actual primitive/material/pass0 map0 unavailable");
        return demand((*pass.texture1A)[0],t.atlas,pc);
    };
    std::uint8_t flags=0;
    // Existing counted B4 accessor certifies ONLY bit0 and reads the current
    // actual manager byte once. No Profile fallback belongs to this slot40.
    if(!liveRace.managerFlags(camera,flags,d)){d.sourcePC=0x1c15f4;return false;}
    if(flags&1u){
        //1C160C name at3AC9B0 (signed lowhalfC9B0):denmarkbold16ingame.
        //A genuinely absent named entry skips ONLY the font demand.
        const auto font=p.fonts->find282210(assets::OriginalApplicationFonts50LA::Name::denmark16);
        if(font){
            if(!assets::OriginalApplicationFonts50LA::currentResource281F90(*p.domain,*font))
                return stop(0x1c1630,"Results actual registered Denmark16 resource unavailable");
            if(!demand(font->texture408_,font->texture281F90(),0x1c1658))return false;
        }
        //Exactly three icons210/270/2D0, not fourth Circle330. Repeated shared
        //texture aliases remain separate reached2DF8A0/demand occurrences.
        for(std::size_t i=0;i<3;++i){d.object=0x210+0x60*i;if(!texture((*p.icons210)[i],0x1c168c))return false;}
        const auto operation=p.loadedRosterOperation.lock();const auto* bank=demandObserverBankIdentity1C16F8(observer);
        // Native same-Operation current Bank bound, not a full source B0
        // constructor receipt. Read the actual count at EACH reached loop
        // condition1C16F8, never use portraitsCount alone as a live count.
        std::size_t i=0;
        for(;;){
            std::size_t count=0;
            if(!demandRosterCountCurrent1C16F8(race,observer,operation,bank,count))
                return stop(0x1c16f8,"Results slot40 actual loaded roster/live Bank count changed");
            //Actual reached loop-condition count read once by the private out-of-line helper.
            if(count!=p.portraitsCount||!p.published480||!p.portraits480)
                return stop(0x1c16f8,"Results slot40 current count exceeds/differs from owned portrait extent");
            if(i>=count)break;
            d.object=0x480; //Pointer field; element ordinal is the actual loop index.
            if(!texture(p.portraits480[i],0x1c16d8))return false;
            ++i;
        }
    }
    //Independent second last-camera test1C1708..1724, including B4 bit0
    //clear. Do not move these two backdrop demands under the first branch.
    if(!liveRoot.finalCameraCurrent(race,camera))
        return stop(0x1c1724,"Results final native Camera changed before independent backdrop demand");
    for(std::size_t i=0;i<2;++i){d.object=0xa0+0x60*i;if(!texture((*p.backdropsA0)[i],0x1c175c))return false;}
    //141C30 sees genuinely empty Results Entity children (retirementCurrent).
    if(!retirementCurrent())return stop(0x141c30,"Results actual empty child traversal changed");
    d.status=Impl::S::incomplete;d.sourcePC=0x1c177c;
    d.operation="Results slot40 demand body returned under final native Camera adaptation; no mode3 or PS2 last-camera claim";
    return true;
}

OriginalResults10450LA::DisplayCpuPrefix OriginalResults10450LA::captureNativeSingleHumanBaseCpu1B9220(
    const ::original_main_entry50KG::Application& root,const OriginalRaceConstructor50LA& race,
    const ::original_position_stats_adapter50LA::Owner& observer,OriginalResults104Diagnostic50LA& d)noexcept {
    using Read=::original_results_roster50LA::Read;
    const auto liveRace=Read::raceAccess(race);
    const auto liveRoot=Read::rootAccess(root);
    d={};
    if(liveRoot.garageAudioOwnerThread_!=std::this_thread::get_id()||
       liveRace.resultsFeedbackThread_!=std::this_thread::get_id()||!impl_||
       impl_->nativeCaptureThread!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1b9220;d.operation="Results shared capture immutable owner threads";
        return DisplayCpuPrefix::none;
    }
    auto& p=*impl_;
    const auto reject=[&](Impl::S status,std::uint32_t pc,const char* why){
        p.stop(d,status,pc,why);return DisplayCpuPrefix::none;
    };
    // Completed native Root, not a still-entered or abandoned callback chain.
    // Source mode5/last-camera/B4 are NOT inferred from this native idle cut.
    if(liveRoot.sourceRace28COwner_.get()!=&race||liveRoot.entitiesOwner_!=p.application||
       liveRoot.riderProfile!=liveRace.profile_||liveRoot.nativeTaRootRace_!=&race||
       liveRoot.nativeTaRootGeneration_!=p.component.sessionGeneration||liveRoot.nativeTaRootEntered_||
       liveRoot.nativeTaRootAborted_||liveRoot.sourceApplicationMode22C_!=0||liveRoot.sourceUpdateRenderBypass228Bit3_||
       liveRace.results104_.get()!=this||liveRace.application_!=p.application||!liveRace.counted_||liveRace.rolledBack_||
       liveRace.entity_.sessionGeneration!=p.component.sessionGeneration||!constructed()||!p.loadComplete||
       !p.application||!p.base||!p.derived||!p.mode1F0||!p.base180||!p.center40||!p.backdropsA0||
       !p.icons210||!p.titles490||!p.polygons[0]||!p.polygons[1]||!p.domain||!p.context||!p.context->live()||
       liveRoot.standardMaterialDomainOwner_!=p.domain||!liveRoot.videoOwner_||p.context->videoIdentity()!=liveRoot.videoOwner_.get()||
       !p.fonts||p.fonts->terminal()||!p.sourceData||!p.sourceData->sourceLoaded()||p.depth!=0xffffffu||
       !retirementCurrent()||!liveRace.profile_||!Read::profileInitialized(race))
        return reject(Impl::S::owner,0x1b9220,"Results shared capture exact completed Root/Race/resources");
    if(!liveRoot.publishedRoot(race,d))return DisplayCpuPrefix::none;
    const auto* profile=Read::commonProfile(race);
    if(!profile||!Read::profileKnown(*profile))return reject(Impl::S::owner,0x1b9d98,"Results actual Profile1125C unavailable");
    const auto humans=(Read::profilePlayerBits(*profile)>>5)&3u;
    // >=2 runs a distinct115A60 viewport scope and polygon38 BEFORE slots78/74.
    // Never collapse that route into this single-human order, or invent count1.
    if(humans!=1u)return reject(Impl::S::incomplete,0x1b9da4,
        "Results shared capture supports only actual single-human Profile; split viewport route unjoined");
    const auto controllerMode=*p.mode1F0;
    // mode1F0 is optional, unknown until actual1B9FE0 stores it. Additionally
    // require THIS completed update to have taken the bit3-clear producer,
    // not merely a retained native backing value or skipped stale base call.
    if(!p.updateCpu||p.updateCpu->prefix!=UpdateCpuPrefix::baseReturned||
       p.updateCpu->nextSourcePC!=0x1c8c1c||!(liveRace.flags1B0KnownMask_&8u)||(liveRace.flags1B0KnownValue_&8u))
        return reject(Impl::S::incomplete,0x1ba01c,"Results current completed base mode1F0 producer absent");
    const auto timing=liveRoot.nativeTaRootTiming_;original_ui_source_timing50LH::Sample sample;std::uint64_t publication=0;
    if(!timing.readPrepared(sample,publication)||publication!=liveRoot.nativeTaRootPublication_||
       publication!=p.updateCpu->publication||!liveRoot.sourceApplicationElapsed23C_||
       std::bit_cast<std::uint32_t>(sample.updateSeconds)!=std::bit_cast<std::uint32_t>(*liveRoot.sourceApplicationElapsed23C_)||
       !std::isfinite(sample.raw250)||sample.raw250<0.f||publication<=p.lastNativeSharedAttemptPublication)
        return reject(Impl::S::phase,0x1b9324,"Results exact unreplayed same-Root raw250 publication absent");
    const auto stamp=Read::profileStamp(race);const auto playerBits=Read::profilePlayerBits(*profile);
    using Dictionary=assets::OriginalApplicationLocalization50KG;
    std::shared_ptr<Dictionary> dictionary;
    std::shared_ptr<const Dictionary::SelectedText> selectedController;
    std::string_view selectedBytes;
    std::int32_t controllerOrdinal=-1;
    std::uint32_t controllerDevice=0;
    const auto current=[&]()noexcept {
        original_ui_source_timing50LH::Sample now;std::uint64_t serial=0;
        const auto next=Read::profileStamp(race);const auto* currentProfile=Read::commonProfile(race);
        if(!p.mode1F0||*p.mode1F0!=controllerMode)return false;
        if(selectedController){std::string_view text;
            if(!dictionary||liveRoot.sourceLocalization32COwner_!=dictionary||!p.device1F4||*p.device1F4!=controllerDevice||
               !Dictionary::SelectedText::BorrowReader::readCurrent(selectedController,root,*dictionary,controllerOrdinal,text)||
               text.data()!=selectedBytes.data()||text.size()!=selectedBytes.size())return false;
        }
        return liveRoot.sourceRace28COwner_.get()==&race&&!liveRoot.nativeTaRootEntered_&&!liveRoot.nativeTaRootAborted_&&
            liveRoot.entitiesOwner_==p.application&&liveRoot.riderProfile==liveRace.profile_&&
            liveRoot.standardMaterialDomainOwner_==p.domain&&p.context->videoIdentity()==liveRoot.videoOwner_.get()&&
            liveRoot.nativeTaRootRace_==&race&&liveRoot.nativeTaRootGeneration_==p.component.sessionGeneration&&
            !liveRoot.sourceUpdateRenderBypass228Bit3_&&
            liveRoot.sourceApplicationMode22C_==0&&liveRoot.nativeTaRootPublication_==publication&&
            timing.sameOwner(liveRoot.nativeTaRootTiming_)&&timing.readPrepared(now,serial)&&serial==publication&&
            now.timestamp==sample.timestamp&&now.deltaMilliseconds==sample.deltaMilliseconds&&
            std::bit_cast<std::uint32_t>(now.raw250)==std::bit_cast<std::uint32_t>(sample.raw250)&&
            std::bit_cast<std::uint32_t>(now.updateSeconds)==std::bit_cast<std::uint32_t>(sample.updateSeconds)&&
            next.generation==stamp.generation&&next.mode34==stamp.mode34&&currentProfile&&Read::profileKnown(*currentProfile)&&
            Read::profilePlayerBits(*currentProfile)==playerBits&&p.context->live()&&retirementCurrent();
    };
    if(!current())return reject(Impl::S::owner,0x1b9324,"Results shared Root observation changed before effects");
    // Native at-most-once attempt: partial source arithmetic cannot be replayed
    // on failure in this publication. This does not manufacture last-camera admission.
    p.lastNativeSharedAttemptPublication=publication;p.nativeDisplay.reset();p.displayCpu.reset();
    if(!p.advanceBaseGeometryCpu1B9324(sample.raw250,d))return DisplayCpuPrefix::none;
    if(!current())return reject(Impl::S::owner,0x1b9c24,"Results shared Root changed after geometry");
    // Actual1B9C24 precedes all slots78/74. Preserve selected Application32C
    // issuance, not the loader's convenience English table or a cached string.
    if(controllerMode){
        if(!p.device1F4)return reject(Impl::S::incomplete,0x1b9c30,"Results controller device1F4 has no reached producer");
        controllerDevice=*p.device1F4;controllerOrdinal=controllerDevice?88:87;
        dictionary=liveRoot.sourceLocalization32COwner_;
        assets::OriginalApplicationLocalizationDiagnostic50KG localized;
        if(!dictionary||p.application->localization32CNativeOwner_!=dictionary.get()||
           !liveRoot.borrowLocalizationText(*dictionary,controllerOrdinal,selectedController,localized)||
           !Dictionary::SelectedText::BorrowReader::readCurrent(selectedController,root,*dictionary,controllerOrdinal,selectedBytes))
            return reject(Impl::S::incomplete,controllerDevice?0x1b9c48:0x1b9cec,
                "Results actual selected Application32C controller text borrow unavailable");
        auto& text=*p.base180;
        if(!text.string8||text.length5C>text.capacity5B||text.string8[text.length5C])
            return reject(Impl::S::owner,controllerDevice?0x1b9c60:0x1b9d04,"Results owned controller Text180 backing changed");
        // assign preserves full incoming strlen/strcmp equality before clamp,
        // capacity truncation, terminator and dirty bits1/2 in source order.
        p.assign(text,selectedBytes.data(),selectedBytes.size());
        if(!current())return reject(Impl::S::owner,0x1b9d84,"Results selected controller text/root changed after assignment");
    }
    // The concrete slots78 then74 chain follows, with the selected borrow
    // retained through every reached primitive and the final child boundary.
    const auto prefix=captureNativeDerivedCpu1C21B0(race,observer,d);
    if(!p.nativeDisplay||!p.nativeDisplay->boundedDerivedReturned)return prefix;
    auto& capture=*p.nativeDisplay;capture.boundedDerivedReturned=false;
    capture.sharedRootPublication=publication;capture.sharedRootTimestamp=sample.timestamp;capture.sharedRaw250=sample.raw250;
    const auto stop=[&](std::uint32_t pc,const char* why){
        capture.building=false;capture.stoppedPC=pc;p.stop(d,Impl::S::owner,pc,why);return prefix;
    };
    if(!current())return stop(0x1b9ee4,"Results source owners/mode changed after slots78/74");
    capture.building=true;
    // Single human: slots78 textures ->74 pages -> optional180
    // -> polygon38 -> conditional polygon3C. Each projector checks its own
    // actual global BEFORE CPU work/service; dirty208 is never a draw gate.
    if(controllerMode&&!p.projectTextCpu2E0170(*p.base180,0x1b9ef0,d)){
        capture.building=false;capture.stoppedPC=d.sourcePC;return prefix;}
    if(!p.projectPolygonCpu2E2398(0,0x1b9ef8,d)){
        capture.building=false;capture.stoppedPC=d.sourcePC;return prefix;}
    if(p.base->hidden160||!(p.base->transition164<0.f)){
        if(!p.projectPolygonCpu2E2398(1,0x1b9f30,d)){
            capture.building=false;capture.stoppedPC=d.sourcePC;return prefix;}
    }
    capture.building=false;
    if(!current())return stop(0x141d70,"Results same Root/empty Entity children changed before base child traversal");
    // Actual retained Results Entity has firstChild18==0 and descendants24==0
    // (retirementCurrent). Thus141D70 has no child callback to invent or skip.
    capture.boundedDerivedReturned=true;capture.boundedSharedReturned=true;capture.stoppedPC=0x1b9f44;
    p.stop(d,Impl::S::incomplete,0x1b9f44,
        "Results single-human shared CPU body returned; source camera/B4/Frame draw lifetime remains unjoined");
    return prefix;
}

bool OriginalResults10450LA::Impl::retainNativeOccurrence(std::size_t reference,
    std::uint32_t caller,std::vector<assets::Vertex48B>&& vertices,D& d)noexcept {
    if(!nativeDisplay||!nativeDisplay->building||nativeDisplay->thread!=std::this_thread::get_id()||
       !context||!context->live()||!domain||vertices.empty()||vertices.size()%4)
        return stop(d,S::owner,caller,"Results native occurrence custody absent");
    const auto table=domain->materials382B38();const auto* ref=references.reference(reference);
    if(ref&&ref->absent){
        // PORT EXTENSION, NOT ORIGINAL: an add-on portrait (own icon, no stock
        // row). Kept in source order with no leaf yet; its policy and leaf are
        // resolved when this capture's derived body returns
        // (captureNativeDerivedCpu1C21B0), or the packet is dropped there.
        if(!references.addonIcon(ref))return true;
        try {
            NativeDisplayCapture::Packet packet;packet.reference=reference;packet.caller=caller;
            packet.vertices=std::move(vertices);packet.material.finalLeaf=-1;
            nativeDisplay->packets.push_back(std::move(packet));return true;
        }catch(...){return stop(d,S::allocation,caller,"Results native add-on occurrence allocation");}
    }
    if(!table||!ref||!ref->retained||!table->coldCollectorCurrent(nativeDisplay->receipt))
        return stop(d,S::material,caller,"Results originating primitive collector changed");
    // prepareNativePrimitive owns its collector pin and exactly one originating
    // service per reached primitive, not per cached image. Do not double-service
    // here. Clean Text reaches this same path.
    NativeDisplayCapture::Packet packet;
    if(!references.prepareNativePrimitive50LA(reference,nativeDisplay->receipt,packet.material,materialDiagnostic)){
        std::fprintf(stderr,"results_primitive_rejected reference=%zu caller=%08x status=%u pc=%08x field=%s\n",reference,caller,
            unsigned(materialDiagnostic.status),materialDiagnostic.sourcePC,materialDiagnostic.field?materialDiagnostic.field:"");
        return stop(d,S::material,materialDiagnostic.sourcePC,"Results current FINAL native primitive projection incomplete");}
    const auto maximum=packet.material.hasTexture?packet.material.textured.maximumVertexAlpha:
        packet.material.untextured.maximumVertexAlpha;
    for(const auto& v:vertices)if(v.color[3]>maximum)
        return stop(d,S::source,caller,"Results native joint vertex/material alpha bound");
    try {
        packet.reference=reference;packet.caller=caller;packet.vertices=std::move(vertices);
        // Retain this occurrence's material policy even if another occurrence
        // shares its PST. DenmarkPause and Denmark16 need not share samplers.
        nativeDisplay->packets.push_back(std::move(packet));return true;
    }catch(...){return stop(d,S::allocation,caller,"Results native occurrence allocation");}
}

bool OriginalResults10450LA::Impl::copyNativeTexture2DF550(const Texture& t,
    std::uint32_t caller,D& d)noexcept {
    if(!t.primitive54||!t.buffer58||t.primitive54->packet10!=t.buffer58.get())
        return stop(d,S::owner,caller,"Results native texture packet owner");
    try {
        std::vector<assets::Vertex48B> vertices(4);constexpr std::size_t order[]{1,0,2,3};
        for(std::size_t i=0;i<4;++i)if(!nativeVertex(t.buffer58->vertices[order[i]],vertices[i]))
            return stop(d,S::source,caller,"Results native texture vertex domain");
        return retainNativeOccurrence(t.primitive54->reference,caller,std::move(vertices),d);
    }catch(...){return stop(d,S::allocation,caller,"Results native texture copy allocation");}
}

bool OriginalResults10450LA::Impl::copyNativeText2E0170(const Text& t,
    std::uint32_t caller,D& d)noexcept {
    if(t.active5E!=(std::size_t(t.length5C)+35)/36||t.active5E>t.packetCount5D||
       !t.packets64||!t.primitives4)
        return stop(d,S::source,caller,"Results native text active packet domain");
    try {
        for(std::size_t p=0;p<t.active5E;++p){
            const auto count=std::min<std::size_t>(36,std::size_t(t.length5C)-p*36);
            const auto& packet=t.packets64[p];
            if(!t.primitives4[p]||t.primitives4[p]->packet10!=&packet||!packet.vertices||packet.count<count*2)
                return stop(d,S::owner,caller,"Results native text occurrence packet owner");
            std::vector<assets::Vertex48B> vertices(count*4);
            for(std::size_t q=0;q<count;++q){const auto& a=packet.vertices[q*2];const auto& b=packet.vertices[q*2+1];
                auto* out=vertices.data()+q*4;
                if(a.rgba!=b.rgba||a.xyz[2]!=b.xyz[2]||!nativeVertex(a,out[0])||!nativeVertex(b,out[2]))
                    return stop(d,S::source,caller,"Results native glyph pair domain");
                out[1]=out[0];out[1].position[0]=b.xyz[0];out[1].uv[0]=b.stq[0];
                out[3]=out[2];out[3].position[0]=a.xyz[0];out[3].uv[0]=a.stq[0];
            }
            if(!retainNativeOccurrence(t.primitives4[p]->reference,caller,std::move(vertices),d))return false;
        }
        return true; // Empty text has no active primitive; not a renderer receipt.
    }catch(...){return stop(d,S::allocation,caller,"Results native text copy allocation");}
}

bool OriginalResults10450LA::Impl::copyNativePolygon2E21D0(std::size_t index,
    std::uint32_t caller,D& d)noexcept {
    if(index>=polygons.size()||!polygons[index])return stop(d,S::owner,caller,"Results native polygon owner");
    const auto& p=*polygons[index];
    if(p.count34!=1||!p.primitives30||!p.primitives30[0]||!p.packets44||
       p.primitives30[0]->packet10!=p.packets44.get())return stop(d,S::owner,caller,"Results native polygon packet owner");
    try {
        std::vector<assets::Vertex48B> vertices(4);constexpr std::size_t order[]{1,0,2,3};
        for(std::size_t i=0;i<4;++i){auto in=p.packets44[0].vertices[order[i]];in.xyz=in.stq;in.stq={};
            if(!nativeVertex(in,vertices[i]))return stop(d,S::source,caller,"Results native polygon STQ position domain");}
        return retainNativeOccurrence(p.primitives30[0]->reference,caller,std::move(vertices),d);
    }catch(...){return stop(d,S::allocation,caller,"Results native polygon copy allocation");}
}

OriginalResults10450LA::DisplayCpuPrefix OriginalResults10450LA::captureNativeDerivedCpu1C21B0(
    const OriginalRaceConstructor50LA& race,const ::original_position_stats_adapter50LA::Owner& observer,
    OriginalResults104Diagnostic50LA& d)noexcept {
    using Read=::original_results_roster50LA::Read;
    const auto liveRace=Read::raceAccess(race);
    d={};
    // Immutable owner threads FIRST, including the first-ever invocation.
    // No capture reset, source field mutation or collector rebuild on rejection.
    if(liveRace.resultsFeedbackThread_!=std::this_thread::get_id()||!impl_||
       impl_->nativeCaptureThread!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1c21b0;
        d.operation="Results native capture immutable owner thread";return DisplayCpuPrefix::none;}
    auto& p=*impl_;
    if(liveRace.results104_.get()!=this||!liveRace.counted_||liveRace.rolledBack_||
       liveRace.application_!=p.application||liveRace.entity_.sessionGeneration!=p.component.sessionGeneration||
       !p.application||!liveResultsRaceAuthorityCurrent(race,p.application)||
       !constructed()||!p.loadComplete||!p.base||!p.derived||!p.mode1F0||
       !p.icons210||!p.backdropsA0||!p.center40||!p.titles490||
       !p.domain||!p.context||!p.context->live()||!p.fonts||p.fonts->terminal()||
       !p.sourceData||!p.sourceData->sourceLoaded()||p.depth!=0xffffffu||!retirementCurrent()||
       !liveRace.profile_||!Read::profileInitialized(race)){
        p.stop(d,Impl::S::owner,0x1c21b0,"Results native capture exact Race/Application/session/resources");
        return DisplayCpuPrefix::none;}
    const auto* profile=Read::commonProfile(race);
    if(!profile||!Read::profileKnown(*profile)||!std::isfinite(p.base->opening30)||!std::isfinite(p.base->transition164)){
        p.stop(d,Impl::S::source,0x1c21b0,"Results native capture current Profile/layout");return DisplayCpuPrefix::none;}
    p.nativeDisplay.reset();
    const auto table=p.domain->materials382B38();
    try {p.nativeDisplay=std::make_unique<Impl::NativeDisplayCapture>();}
    catch(...){p.stop(d,Impl::S::allocation,0x1c21b0,"Results native capture allocation");return DisplayCpuPrefix::none;}
    auto& capture=*p.nativeDisplay;
    // Publish the REAL rebuild into this same Application Domain's existing
    // collector slot. Later F8/other native borrowers must observe the SAME
    // issued generation, not an invalidated older cold seal. No source
    // population/history is replayed and no pin validity rule is relaxed.
    // The original builds the collector once per race load (1A7B14 14D440);
    // Results does not rebuild it. While the Domain's sealed collector is
    // still current (no row gained or lost an object since), it is that same
    // registry and every earlier receipt (the course world's among them)
    // stays valid; only an expired seal is rebuilt here.
    const bool collectorCurrent=table&&table->coldCollectorCurrent(p.domain->coldCollector_);
    {static int last=-1;if(last!=int(collectorCurrent)){last=int(collectorCurrent);std::fprintf(stderr,"results_collector reused=%d\n",int(collectorCurrent));}}
    if(!collectorCurrent&&(!table||!table->rebuildColdCollector14D440(p.domain->coldCollector_,p.materialDiagnostic))){
        std::fprintf(stderr,"results_collector_rebuild_rejected table=%d status=%u pc=%08x row=%d field=%s\n",table?1:0,
            unsigned(p.materialDiagnostic.status),p.materialDiagnostic.sourcePC,p.materialDiagnostic.row,
            p.materialDiagnostic.field?p.materialDiagnostic.field:"");
        p.stop(d,Impl::S::material,0x14d440,"Results native capture collector snapshot");return DisplayCpuPrefix::none;}
    capture.receipt=p.domain->coldCollector14D440();
    capture.race=&race;capture.generation=p.component.sessionGeneration;capture.building=true;
    p.references.beginNativeCapture50DL(); // PORT EXTENSION: add-on icon policy is per capture
    // The existing private chain traverses each reached derived call once;
    // never replay its borrowed TextCall/TextureCall arrays after mutation.
    const auto prefix=prepareCareerSummaryCpuPrefix(race,observer,d);
    capture.building=false;capture.stoppedPC=d.sourcePC;
    // PORT EXTENSION, NOT ORIGINAL (DLC menu-art step D): each add-on portrait
    // packet takes its policy and leaf now (PortraitPolicy), or is dropped
    // (not drawn). Stock packets are untouched; with no add-on icon this loop
    // changes nothing.
    try {
        for(std::size_t i=0;i<capture.packets.size();){auto& packet=capture.packets[i];
            const auto* ref=p.references.reference(packet.reference);
            if(!ref||!ref->absent){++i;continue;}
            const auto* icon=p.references.addonIcon(ref);
            assets::OriginalStandardMaterialDiagnostic50LA md;Impl::References::NativePrimitive material;
            bool drawn=icon&&p.references.prepareAddonPortrait50DL(packet.reference,capture.receipt,material,md);
            const char* why=drawn?nullptr:(md.field?md.field:"add-on icon policy refused");
            if(drawn)for(const auto& v:packet.vertices)if(v.color[3]>material.textured.maximumVertexAlpha){
                drawn=false;why="vertex alpha above the policy bound";break;}
            const std::uint8_t outcome=drawn?1u:2u;
            if(icon&&icon->logged!=outcome){icon->logged=outcome;
                const auto* image=icon->images->decodedTexture(0);
                if(drawn)std::fprintf(stderr,"dlc_results_portrait racer=%u key=%s icon=%s status=drawn width=%u height=%u leaf=%d policy=stock_portrait_reference_%zu\n",
                    icon->racer60,icon->key.c_str(),icon->member.c_str(),image&&!image->mips.empty()?image->mips.back().width:0u,
                    image&&!image->mips.empty()?image->mips.back().height:0u,material.finalLeaf,p.references.portraitPolicy_.reference);
                else std::fprintf(stderr,"dlc_results_portrait racer=%u key=%s icon=%s status=refused reason=\"%s\"\n",
                    icon->racer60,icon->key.c_str(),icon->member.c_str(),why);
            }
            if(drawn){packet.material=std::move(material);++i;}
            else capture.packets.erase(capture.packets.begin()+static_cast<std::ptrdiff_t>(i));
        }
    }catch(...){
        p.stop(d,Impl::S::allocation,0x1c21b0,"Results native add-on portrait resolution");
        capture.boundedDerivedReturned=false;return DisplayCpuPrefix::none;}
    capture.boundedDerivedReturned=p.displayCpu&&p.displayCpu->nextSourcePC==0x1c8a74&&d.status==Impl::S::incomplete;
    return prefix; // Preserve incomplete/error, never turn CPU capture into draw success.
}

bool OriginalResults10450LA::Impl::projectTextureCpu2DF694(Texture& t,
    std::uint32_t caller,D& d)noexcept {
    if(nativeDisplay&&nativeDisplay->building){
        if(nativeDisplay->thread!=std::this_thread::get_id()||!application)
            return stop(d,S::owner,caller,"Results native texture occurrence owner thread");
        if(!application->screenQuadEnabled383678_)return true; //Source gate BEFORE projection/service.
    }
    // PORT EXTENSION, NOT ORIGINAL: an add-on racer's portrait
    // (Reference::absent) is projected only with its own icon loaded (a
    // packet exists); it has no stock atlas or retained row.
    bool addonPortrait=false;
    if(t.primitive54){const auto* absent=references.reference(t.primitive54->reference);
        if(absent&&absent->absent){if(!t.buffer58||!references.addonIcon(absent))return true;addonPortrait=true;}}
    // Only the zero-rotation CPU span2DF694..2DF860. In particular this is
    // NOT2DF550: source phase5/global383678 checks, current camera1E8/1EC
    // packet header writes and246760 submission are not implemented here.
    if(!t.primitive54||!t.buffer58||(!t.atlas&&!addonPortrait)||
       t.primitive54->packet10!=t.buffer58.get()||t.field3C!=0.f||
       (t.flags5C&2u)||t.field30!=1.f||t.field40!=1.f||
       !std::isfinite(t.x10)||!std::isfinite(t.y14)||
       !std::isfinite(t.width34)||!std::isfinite(t.height38))
        return stop(d,S::source,caller,"Results bounded texture CPU geometry");
    const auto* ref=references.reference(t.primitive54->reference);
    if(!ref||(!ref->retained&&!addonPortrait))
        return stop(d,S::material,caller,"Results texture lost its own material association");
    Packet next=*t.buffer58;
    if(t.flags5C&1u){
        const float right=add(t.x10,mul(t.width34,t.field30));
        const float bottom=add(t.y14,mul(t.height38,t.field30));
        if(!std::isfinite(right)||!std::isfinite(bottom))
            return stop(d,S::source,caller,"Results texture CPU arithmetic");
        for(std::size_t i=0;i<4;++i){
            next.vertices[i].xyz[0]=(i&1)?right:t.x10;
            next.vertices[i].xyz[1]=(i&2)?bottom:t.y14;
        }
    }
    for(const auto& v:next.vertices){
        for(float f:v.xyz)if(!std::isfinite(f))return stop(d,S::source,caller,"Results texture XYZ");
        for(float f:v.stq)if(!std::isfinite(f))return stop(d,S::source,caller,"Results texture STQ");
        if(v.xyz[2]!=16777214.f)return stop(d,S::source,caller,"Results texture retained depth");
    }
    *t.buffer58=next;t.flags5C=std::uint8_t(t.flags5C&~1u);
    return !nativeDisplay||!nativeDisplay->building||copyNativeTexture2DF550(t,caller,d);
}

bool OriginalResults10450LA::Impl::projectTextCpu2E0170(Text& t,
    std::uint32_t caller,D& d)noexcept {
    if(nativeDisplay&&nativeDisplay->building){
        if(nativeDisplay->thread!=std::this_thread::get_id()||!application)
            return stop(d,S::owner,caller,"Results native text occurrence owner thread");
        if(!application->screenTextEnabled383688_)return true; //Independent source gate, before dirty work.
    }
    // The existing source-backed glyph/rounding leaves are reused, NOT a
    // Pause100 storage owner or reference ledger. This admits the actual
    // Results capacities, including a short final36-character packet.
    // Renderer checks, packet-header preparation and246760 remain absent.
    const auto fail=[&](const char* operation){return stop(d,S::source,caller,operation);};
    const auto count=(std::size_t(t.capacity5B)+35)/36;
    if(!t.font4C||!t.string8||!t.capacity5B||t.length5C>t.capacity5B||
       t.string8[t.length5C]||t.field44!=0.f||t.field48!=1.f||
       !std::isfinite(t.field40)||t.field40<=0.f||
       !std::isfinite(t.x10)||!std::isfinite(t.y14)||t.justification50>4||
       !t.glyphsC||!t.packets64||!t.primitives4||t.count0!=count||t.packetCount5D!=count)
        return fail("Results bounded text CPU geometry");
    const auto* atlas=t.font4C->texture281F90();
    if(!atlas||!atlas->sourceWidth14()||!atlas->sourceHeight18()||
       *atlas->sourceWidth14()<=0||*atlas->sourceHeight18()<=0)
        return fail("Results actual retained font dimensions");
    for(std::size_t p=0;p<count;++p){
        const auto expected=2*std::min<std::size_t>(36,std::size_t(t.capacity5B)-p*36);
        if(!t.primitives4[p]||t.primitives4[p]->packet10!=&t.packets64[p]||
           !t.packets64[p].vertices||t.packets64[p].count!=expected)
            return fail("Results text packet ownership");
        const auto* ref=references.reference(t.primitives4[p]->reference);
        if(!ref||!ref->retained)return stop(d,S::material,caller,"Results text material association");
    }
    if(!(t.flags5A&3u))return !nativeDisplay||!nativeDisplay->building||copyNativeText2E0170(t,caller,d);
    OriginalUiTextLayout50KG run;
    OriginalUiTextDiagnostic50KG glyph;
    const auto layout=[&](float x,float y){
        OriginalUiTextInput50KG in{&t.font4C->metrics(),nullptr,
            reinterpret_cast<const std::uint8_t*>(t.string8.get()),t.length5C,x,y};
        return layoutOriginalUiTextDimensions50LA(in,
            static_cast<std::uint32_t>(*atlas->sourceWidth14()),
            static_cast<std::uint32_t>(*atlas->sourceHeight18()),run,glyph)&&run.count==t.length5C;
    };
    if(!layout(0,0))return fail("Results source glyph layout");
    float x=t.x20,y=t.y24;
    if(t.flags5A&1u){
        x=t.x10;y=t.y14;
        if(t.length5C&&t.justification50){
            std::int32_t width=0,height=0,coordinate=0;
            if(!roundOriginalUiTextMetric50KG(run.widthBeforeScaleAndRounding,t.field40,width)||
               !roundOriginalUiTextMetric50KG(run.firstGlyphHeightBeforeScaleAndRounding,t.field40,height))
                return fail("Results source text metric rounding");
            const auto j=t.justification50;
            if(j==1||j==3||j==4){
                const float shift=j==4?float(width):mul(float(width),.5f);
                if(!roundOriginalUiTextMetric50KG(sub(x,shift),1,coordinate))return fail("Results text X rounding");
                x=float(coordinate);
            }
            if(j==2||j==3){
                if(!roundOriginalUiTextMetric50KG(sub(y,mul(float(height),.5f)),1,coordinate))
                    return fail("Results text Y rounding");
                y=float(coordinate);
            }
        }
    }
    const bool inverse=(t.flags5A&5u)!=0;
    if(!layout(inverse?div(x,t.field40):x,inverse?div(y,t.field40):y))
        return fail("Results source positioned glyph layout");
    for(std::size_t n=0;n<t.length5C;++n)for(std::size_t v=0;v<2;++v){
        const auto& point=run.quads[n].vertices[v?2:0];
        if(t.glyphsC[n].vertices[v].packet!=&t.packets64[n/36].vertices[(n%36)*2+v]||
           !std::isfinite(mul(point.x,t.field40))||!std::isfinite(mul(point.y,t.field40)))
            return fail("Results glyph packet ownership/arithmetic");
    }
    t.active5E=std::uint8_t((t.length5C+35u)/36u);t.x20=x;t.y24=y;
    const float z=sub(float(depth),t.field48);
    for(std::size_t n=0;n<t.length5C;++n)for(std::size_t v=0;v<2;++v){
        auto& vertex=t.glyphsC[n].vertices[v];const auto& point=run.quads[n].vertices[v?2:0];
        vertex.authored00[0]=point.x;vertex.authored00[1]=point.y;vertex.authored00[2]=z;
        vertex.packet->stq[0]=point.u;vertex.packet->stq[1]=point.v;
        vertex.packet->xyz=vertex.authored00;
        vertex.packet->xyz[0]=mul(point.x,t.field40);vertex.packet->xyz[1]=mul(point.y,t.field40);
    }
    t.flags5A=std::uint8_t(t.flags5A&~7u);
    return !nativeDisplay||!nativeDisplay->building||copyNativeText2E0170(t,caller,d);
}

bool OriginalResults10450LA::Impl::prepareTexturesCpu1C21B0(
    std::uint8_t raceFlags29,std::uint32_t profileMode34,D& d)noexcept {
    auto& out=*displayCpu;
    const auto texture=[&](Texture& t,std::uint32_t offset,std::uint32_t pc){
        d.object=offset;
        if(out.textureCount==out.textures.size()||!projectTextureCpu2DF694(t,pc,d))return false;
        out.textures[out.textureCount++]={&t,pc};return true;
    };
    // Full1C21B0 gate/order, but each reached2DF550 call is represented only
    // by this SAME owner's CPU geometry and typed retained-resource borrow.
    if(*mode1F0==0&&base->opening30<=0.f){
        if(!(raceFlags29&4u)&&!texture((*icons210)[3],0x330,0x1c2204))return false;
        //1C2224 is C.LT.S transition164,0: icons require NEGATIVE164.
        if(!base->hidden160&&base->transition164<0.f){
            if(!texture((*icons210)[0],0x210,0x1c2234))return false;
            if((derived->mode200==1&&profileMode34!=2)||derived->mode200==2)
                if(!texture((*icons210)[2],0x2d0,0x1c2278))return false;
        }
    }
    //1C22A8 BC1T skips backdrops when hidden AND transition164<0.
    // Even mode1F0!=0 / opening30>0 still reaches this independent tail.
    if(!base->hidden160||!(base->transition164<0.f)){
        if(!texture((*backdropsA0)[0],0xa0,0x1c22b4)||
           !texture((*backdropsA0)[1],0x100,0x1c22bc)||
           !texture(*center40,0x40,0x1c22c4))return false;
    }
    out.prefix=DisplayCpuPrefix::textures;return true;
}

bool OriginalResults10450LA::Impl::transitionCpu1C2358(D& d)noexcept {
    if(!(base->opening30<=0.f))return true; //1C2350 directly to1C8A74.
    auto& out=displayCpu->transition;
    //390 is retained as source bits by the existing owner, not an integer
    // countdown. No synthetic timer step is introduced by CPU preparation.
    const float t=std::bit_cast<float>(derived->field390);
    if(!std::isfinite(t))return stop(d,S::source,0x1c2358,"Results transition390 is not finite");
    const auto mode=derived->mode200,previous=derived->previous204;
    const float ratio=div(t,.25f);
    if(t<=0.f){
        if(mode==1){out.offsetF31=1280.f;out.offsetF21=641.f;}
        else if(mode==3||mode==4){out.offsetF20=-1280.f;out.offsetF21=-641.f;}
        else if(mode==2){out.offsetF20=-641.f;out.offsetF31=641.f;}
    }else if(mode==1){
        if(previous==3||previous==4){out.offsetF20=mul(640.f,ratio);out.offsetF21=641.f;
            out.offsetF31=mul(-640.f,sub(1.f,ratio));}
        else if(previous==2){out.offsetF20=mul(-640.f,ratio);out.offsetF31=641.f;
            out.offsetF21=mul(640.f,sub(1.f,ratio));}
    }else if(mode==3||mode==4){
        if(mode==4&&previous==3){
            //1C2524 loads F20;1C2534 MOV.S F21,F20 is the branch delay slot.
            out.offsetF20=641.f;out.offsetF21=641.f;
            if(t>.25f){const float second=div(sub(t,.25f),.25f);
                out.offsetF31=mul(-640.f,sub(1.f,second));}
            else {
                //1C2560 reads unrepresented AD4; if nonzero1C9B10 runs,
                //THEN390 is reloaded. Do not default AD4, skip the callback,
                //or use pre-call390 for the remaining stackD4 arithmetic.
                displayCpu->nextSourcePC=0x1c2560;
                return stop(d,S::incomplete,0x1c2560,"Results AD4/1C9B10 producer and reload join absent");
            }
        }else if(previous==1){out.offsetF20=mul(640.f,sub(1.f,ratio));out.offsetF21=641.f;
            out.offsetF31=mul(-640.f,ratio);}
        else if(previous==2){out.offsetF31=mul(640.f,ratio);out.offsetF20=641.f;
            out.offsetF21=mul(-640.f,sub(1.f,ratio));}
    }else if(mode==2){
        if(previous==1){out.offsetF21=mul(640.f,ratio);out.offsetF31=-641.f;
            out.offsetF20=mul(-640.f,sub(1.f,ratio));}
        else if(previous==3||previous==4){out.offsetF21=mul(-640.f,ratio);out.offsetF20=-641.f;
            out.offsetF31=mul(640.f,sub(1.f,ratio));}
    }
    if(!std::isfinite(out.offsetF20)||!std::isfinite(out.offsetF21)||!std::isfinite(out.offsetF31))
        return stop(d,S::source,0x1c268c,"Results bounded transition arithmetic");
    return true;
}

bool OriginalResults10450LA::Impl::prepareTitlePrefixCpu1C2698(
    std::uint8_t raceFlags29,std::uint32_t profileMode34,D& d)noexcept {
    auto& out=*displayCpu;
    if(!(base->opening30<=0.f)||*mode1F0!=0){
        out.nextSourcePC=0x1c8a74;out.prefix=DisplayCpuPrefix::titles;return true;
    }
    std::size_t first=0,end=3;
    if(base->hidden160||!(base->transition164<0.f)){first=1;end=2;}
    else if((derived->mode200!=1&&derived->mode200!=2)||
            (derived->mode200==1&&profileMode34==2))end=2;
    // This assignment happens even if race.flags29 bit2 skips title1 below.
    if(!localized((*titles490)[1],base->hidden160?427:426,d))return false;
    for(std::size_t i=first;i<end;++i){
        if(i==1&&(raceFlags29&4u))continue;
        d.object=0x490+0x70*i;auto& t=(*titles490)[i];
        if(!projectTextCpu2E0170(t,0x1c28b0,d))return false;
        out.titles[out.titleCount++]={&t,0x1c28b0};
    }
    //1C28CC/1C28EC are independent source early returns. The remaining
    //roster/records/statistics/name-entry pages are NOT an empty callback.
    out.nextSourcePC=base->hidden160||!(base->transition164<0.f)?0x1c8a74:0x1c28f4;
    out.prefix=DisplayCpuPrefix::titles;return true;
}

OriginalResults10450LA::DisplayCpuPrefix OriginalResults10450LA::prepareDisplayCpuPrefix(
    const OriginalRaceConstructor50LA& race,OriginalResults104Diagnostic50LA& d)noexcept {
    using Read=::original_results_roster50LA::Read;
    const auto liveRace=Read::raceAccess(race);
    d={};if(!impl_)return DisplayCpuPrefix::none;auto& p=*impl_;p.displayCpu.reset();
    if(!constructed()||!p.loadComplete||!p.base||!p.derived||!p.mode1F0||
       !p.icons210||!p.backdropsA0||!p.center40||!p.titles490||!p.context||!p.context->live()||
       !p.domain||!p.fonts||p.fonts->terminal()||!p.sourceData||!p.sourceData->sourceLoaded()||
       p.depth!=0xffffffu||!retirementCurrent()||liveRace.results104_.get()!=this||
       !liveRace.counted_||liveRace.rolledBack_||liveRace.application_!=p.application||
       liveRace.entity_.sessionGeneration!=p.component.sessionGeneration||
       !liveResultsRaceAuthorityCurrent(race,p.application)||
       !liveRace.profile_||!Read::profileInitialized(race)){
        p.stop(d,Impl::S::owner,0x1c21b0,"Results CPU preparation requires its exact retained Race104/Profile/resources");
        return DisplayCpuPrefix::none;
    }
    const auto* profile=Read::commonProfile(race);
    if(!profile||!Read::profileKnown(*profile)||!std::isfinite(p.base->opening30)||!std::isfinite(p.base->transition164)){
        p.stop(d,Impl::S::source,0x1c21b0,"Results current profile or finite layout fields absent");
        return DisplayCpuPrefix::none;
    }
    const auto before=Read::profileStamp(race);
    p.displayCpu.emplace();auto& out=*p.displayCpu;
    out.race=&race;out.raceEntity=&liveRace.entity_;out.session=p.component.sessionGeneration;
    // No208 read/clear occurs here.1C17A0 uses208 ONLY for dirty layout;
    //even208==0 reaches1C2108,AE0/1C9B10 and the independent submission gate.
    // No claim is made that1C17A0 or shared1B9220 has run before this private
    //CPU proposal. There is deliberately no production caller yet.
    if(!p.prepareTexturesCpu1C21B0(liveRace.entity_.flags29,Read::profileMode(*profile),d)||
       !p.transitionCpu1C2358(d)||
       !p.prepareTitlePrefixCpu1C2698(liveRace.entity_.flags29,Read::profileMode(*profile),d))return out.prefix;
    const auto after=Read::profileStamp(race);
    if(before.generation!=after.generation||before.mode34!=after.mode34||!p.context->live()){
        p.stop(d,Impl::S::owner,0x1c28c8,"Results owners changed during CPU preparation");
        p.displayCpu.reset();return DisplayCpuPrefix::none;
    }
    p.stop(d,Impl::S::incomplete,out.nextSourcePC,
        out.nextSourcePC==0x1c28f4?"Results title prefix only; text-page tails and real renderer join absent":
        "Results bounded derived CPU path only; real renderer/shared-base join absent");
    return out.prefix;
}
}
