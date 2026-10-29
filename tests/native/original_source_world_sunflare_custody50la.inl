#pragma once
#include "original_source_world_sunflare_material_custody50la.inl"
#include <limits>
#include <new>
namespace ra_pod_scene50LA {
inline bool Owner::reserveSourceSunFlareNative()noexcept {
    return worldColdConstructed_&&worldApplication_&&worldApplication_->application10()&&
        worldEntity_.reference00==1&&worldEntity_.sessionGeneration&&
        !worldApplication_->attachmentLock423F58()&&
        worldApplication_->objectCount423F60()<std::numeric_limits<std::uint32_t>::max()&&
        worldApplication_->entityAuthority_.enrollmentSerial_<std::numeric_limits<std::uint64_t>::max()&&
        worldApplication_->entityAuthority_.reserveAdditional(1);
}
inline bool Owner::constructSourceSunFlare2C5730(std::uint32_t& pc)noexcept {
    if(!nativeSunFlares_||!nativeSunFlares_->sourceMode||nativeSunFlares_->sourceOwnerAttempted||!worldApplication_)return false;
    auto& d=*nativeSunFlares_;pc=0x27206c;d.sourceOwnerAttempted=true;
    d.owner.reset(new(std::nothrow) NativeSunFlare);
    if(d.owner){auto& o=*d.owner;o.sourceMode=true;auto& e=o.entity;
        auto& authority=worldApplication_->entityAuthority_;
        //Native admission can refuse without turning a real nonNULL outer
        //allocation into source mallocNULL. Preserve that uncounted owner.
        if(!authority.canEnroll(e,key(&e))){pc=0x2c5740;return false;}
        pc=0x2c5740;
        const auto r=g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::entity141300,
            worldEntity_.sessionGeneration,key(&e),true,worldApplication_->objectCount423F60());
        o.counted=e.reference00!=0;
        if(r.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
        if(!authority.enroll(e)){pc=0x2c5748;return false;}o.sourceEnrolled=true;
        auto at=[&](std::uint32_t p,unsigned bit){pc=p;o.sourceConstructorMask|=1u<<bit;};
        at(0x2c5754,0); //Actual vtable08=3D4D90; const native representation.
        pc=0x2c5760;o.direction50={{0.f,0.f,0.f,1.f}};at(pc,1);
        at(0x245cf8,2); //Embedded base245CF0 vtable3D2040.
        pc=0x245cfc;o.material68.primitive04.reset();at(pc,3);
        pc=0x245d00;o.material68.parameters08.reset();at(pc,4);
        at(0x2c577c,5); //Actual embedded subclass vtable68=3D4E10.
        pc=0x2c5780;o.callback74=nullptr;at(pc,6);
        pc=0x2c5794;o.zones30=1;at(pc,7);
        pc=0x2c57ac;o.field60=0;at(pc,8);
        pc=0x2c57b8;o.field3C=0;at(pc,9);
        pc=0x2c57c0;o.colors44.reset();at(pc,10);
        pc=0x2c57c8;o.colors48.reset();at(pc,11);
        pc=0x2c57d0;o.buffer78.reset();at(pc,12);
        pc=0x2c57d8;o.sprite64.reset();o.sourceSprite64=nullptr;at(pc,13);
        pc=0x2c57e0;o.field7C=0;at(pc,14);
        pc=0x2c57e8;o.indices80[0]=-1;at(pc,15);
        pc=0x2c57f0;o.indices80[2]=-1;at(pc,16);
        pc=0x2c57f8;o.indices80[1]=-1;at(pc,17);
        pc=0x2c5800;o.indices80[3]=-1;at(pc,18);
        //Native defaults are not reached source knowledge. Track each actual
        //low-bit store; upper five bits stay unknown throughout this prefix.
        o.flags98KnownMask=0;o.flags98KnownValue=0;
        pc=0x2c5814;o.flags98KnownMask|=1;at(pc,19);
        pc=0x2c5828;o.flags98KnownMask|=2;at(pc,20);
        pc=0x2c583c;o.flags98KnownMask|=4;at(pc,21);
        o.sourceConstructorReturned=true;
    }
    //An allocated/counted object survives native custody even if the raw
    //pointer array is NULL; its real destination store is the later cut.
    pc=0x27209c;if(!d.pointerBacking)return false;
    d.pointerBacking[0]=d.owner.get();d.sourceSlotPublished=true;return true;
}
inline bool Owner::constructSourceSunFlareSprite2E5E60(std::uint32_t mode,std::uint32_t& pc)noexcept {
    if(!nativeSunFlares_||!nativeSunFlares_->sourceMode||!nativeSunFlares_->owner)return false;
    auto& o=*nativeSunFlares_->owner;if(o.sourceSpriteAttempted)return false;
    pc=0x2c5ab8;o.sourceSpriteAttempted=true;
    //Actual call delay store precedes allocator effects.
    pc=0x2c5abc;o.flags98KnownValue=static_cast<std::uint8_t>((o.flags98KnownValue&~4u)|((mode&1u)<<2u));
    o.sourceModeStored=true;
    o.sprite64.reset(new(std::nothrow) NativeSunFlareSprite);
    if(o.sprite64){auto& s=*o.sprite64;s.sourceMode=true;
        auto at=[&](std::uint32_t p,unsigned bit){pc=p;s.sourceKnownMask|=1u<<bit;};
        pc=0x245cf8;s.sourceVtable00=0x3d2040;at(pc,0);
        pc=0x245cfc;s.material.primitive04.reset();at(pc,1);
        pc=0x245d00;s.material.parameters08.reset();at(pc,2);
        pc=0x2e5e7c;s.sourceVtable00=0x3d56d0;at(pc,3);
        pc=0x2e5e80;s.height18=0;at(pc,4);
        pc=0x2e5e84;s.width14=0;at(pc,5);
        pc=0x2e5e8c;s.scale20=1.f;at(pc,6);
        pc=0x2e5e90;s.scale1C=1.f;at(pc,7);
        pc=0x2e5e94;s.u2C=0.f;at(pc,8);
        pc=0x2e5e98;s.u24=0.f;at(pc,9);
        pc=0x2e5e9c;s.v30=1.f;at(pc,10);
        pc=0x2e5ea0;s.v28=1.f;at(pc,11);
        pc=0x2e5ea8;s.depth34=0;at(pc,12);
        pc=0x2e5eac;s.alpha48=255;at(pc,13);
        pc=0x2e5eb8;s.colors38[3]=0x80808080;at(pc,14);
        pc=0x2e5ebc;s.colors38[2]=0x80808080;at(pc,15);
        pc=0x2e5ec0;s.colors38[1]=0x80808080;at(pc,16);
        pc=0x2e5ec4;s.colors38[0]=0x80808080;at(pc,17);
    }
    pc=0x2c5ad0;o.sourceSprite64=o.sprite64.get();o.sourceSpritePublished=true;
    if(!o.sprite64){pc=0x2c5ad8;return false;}
    pc=0x2c5ae0;return true; //Before actual material load; no row/packet acquired.
}
inline bool Owner::canRetireSourceSunFlareNative()const noexcept {
    if(!nativeSunFlares_||!nativeSunFlares_->sourceMode)return false;
    const auto& d=*nativeSunFlares_;
    if(d.sourceThread!=std::this_thread::get_id())return false;
    if(d.sourceNativeClosed)return !d.owner&&!d.pointerBacking&&!nativeSunFlarePointers4A4_;
    if(!worldApplication_||d.sourceApplication!=worldApplication_.get()||
       d.generation!=worldEntity_.sessionGeneration||worldApplication_->attachmentLock423F58())return false;
    if(d.sourcePointerPublished){
        if(!d.pointerAllocationAttempted||!d.count4A8||*d.count4A8!=1||
           !nativeSunFlarePointers4A4_||*nativeSunFlarePointers4A4_!=d.pointerBacking.get())return false;
    }else if(d.pointerBacking||!nativeSunFlarePointers4A4_||*nativeSunFlarePointers4A4_)return false;
    if(d.sourceSlotPublished&&(!d.sourceOwnerAttempted||!d.pointerBacking||d.pointerBacking[0]!=d.owner.get()))return false;
    //Never read an unpublished raw slot, including native default NULL bytes.
    if(!d.owner)return true;
    const auto& o=*d.owner;
    for(auto mask:o.sourceSaturation44Known)if(mask&~15u)return false;
    for(auto mask:o.sourceSaturation48Known)if(mask&~15u)return false;
    const auto& e=o.entity;
    if(!d.sourceOwnerAttempted||!o.sourceMode||(o.sourceConstructorMask&~0x3fffffu)||
       (o.sourceConstructorReturned&&o.sourceConstructorMask!=0x3fffffu)||
       !canRetireSourceSunFlareMaterialNative(o.material68)||o.context||o.directory36B970||
       o.colorsInitialized||o.colorRowsInitialized||o.nextLoad!=NativeSunFlare::Load::unreached)return false;
    if(bool(o.depth40)!=o.sourceDepthStored)return false;
    if(o.sourceCallbackSelfStored?o.callback74!=&o:o.callback74!=nullptr)return false;
    if(o.material68.sourceCallbackAttempted&&!o.material68.sourceNativeClosed&&o.material68.sourceCallbackOwner!=&o)return false;
    if(o.sourceColors44Attempted){
        if(!o.sourceColors44Count||*o.sourceColors44Count!=10||!o.sourceColors44Published||
           !o.sourceColors44||*o.sourceColors44!=o.colors44.get())return false;
    }else if(o.colors44||o.sourceColors44||o.sourceColors44Published||o.sourceColors44Count)return false;
    if(o.sourceColors48Attempted){
        if(!o.sourceColors48Count||*o.sourceColors48Count!=10||!o.sourceColors48Published||
           !o.sourceColors48||*o.sourceColors48!=o.colors48.get())return false;
    }else if(o.colors48||o.sourceColors48||o.sourceColors48Published||o.sourceColors48Count)return false;
    if(o.sourceReadbackAttempted){
        if(!o.sourceReadbackPublished||!o.sourceReadback78||*o.sourceReadback78!=o.buffer78.get()||
           (o.buffer78&&(reinterpret_cast<std::uintptr_t>(o.buffer78.get())&63u)))return false;
    }else if(o.buffer78||o.sourceReadback78||o.sourceReadbackPublished)return false;
    //Read-only TOTAL reference proof precedes either destructive material close.
    //Two acquired handles on the same canonical row need two live references.
    const auto* first=o.sprite64?o.sprite64->material.primitive04.get():nullptr;
    const auto* second=o.material68.primitive04.get();
    if(first&&second&&first->retained&&second->retained&&first->table==second->table&&first->material00==second->material00){
        rr::assets::OriginalStandardMaterialRowView50LA row;
        if(!first->table||!first->table->row(first->material00,row)||!row.references08||*row.references08<2)return false;
    }
    if(o.sourceSpritePublished&&(!o.sourceSprite64||*o.sourceSprite64!=o.sprite64.get()))return false;
    if(o.sourceConstructorReturned&&!o.sourceSpritePublished&&(!o.sourceSprite64||*o.sourceSprite64))return false;
    if(o.sprite64){const auto& s=*o.sprite64;
        if(!o.sourceSpriteAttempted||!s.sourceMode||(s.sourceKnownMask&~0x3ffffu)||!canRetireSourceSunFlareMaterialNative(s.material)||
           (s.sourceSetterMask&~0xffffu)||
           (bool(s.x0C)!=bool(s.sourceSetterMask&16384u))||
           (bool(s.y10)!=bool(s.sourceSetterMask&32768u))||
           (s.x0C&&*s.x0C!=0)||(s.y10&&*s.y10!=0))return false;}
    if(o.counted){
        if(e.reference00!=1||e.kind!=g::OriginalWreckComponentKind50LA::entity141300||
           e.objectKey!=key(&e)||e.sessionGeneration!=d.generation||e.category28!=0xff||(e.flags29&15u)!=3u||e.parent1C||e.firstChild18||
           e.previous10||e.next14||e.mask20||e.descendants24||e.nativeFlags29||
           !worldApplication_->objectCount423F60())return false;
    }else if(e.reference00)return false;
    const auto& a=worldApplication_->entityAuthority_;
    if(o.sourceEnrolled)return o.counted&&a.canRetire(e);
    for(std::size_t i=0;i<a.count_;++i)if(a.entries_[i].node==&e)return false;
    return true;
}
inline bool Owner::retireSourceSunFlareNative()noexcept {
    if(!canRetireSourceSunFlareNative())return false;
    auto& d=*nativeSunFlares_;if(d.sourceNativeClosed)return true;
    if(d.owner&&d.owner->sprite64&&
       !retireSourceSunFlareMaterialNative(d.owner->sprite64->material))return false;
    if(d.owner&&!retireSourceSunFlareMaterialNative(d.owner->material68))return false;
    if(d.owner&&d.owner->sourceEnrolled){
        if(!worldApplication_->entityAuthority_.retire(d.owner->entity))return false;
        d.owner->sourceEnrolled=false;
    }
    //After exact authority withdrawal, only infallible plain backing disposal
    //and acquired-count decrement remain. Do NOT source-null unknown aliases.
    nativeSunFlarePointers4A4_.reset();
    if(d.owner){auto& o=*d.owner;
        //Withdraw only native knowledge; no source NULL-store/destructor grant.
        o.sourceColors44.reset();o.sourceColors48.reset();o.sourceReadback78.reset();
        o.colors44.reset();o.colors48.reset();o.buffer78.reset();o.callback74=nullptr;
        o.sourceSprite64.reset();o.sprite64.reset();
        if(o.counted){--o.entity.reference00;--worldApplication_->objectCount423F60();o.counted=false;}
        d.owner.reset();}
    d.pointerBacking.reset();d.sourceNativeClosed=true;
    //Retain the SAME domain's sole count4A8 and publication history tombstone.
    return true;
}
}
