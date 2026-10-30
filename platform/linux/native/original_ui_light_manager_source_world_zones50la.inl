#pragma once
#include <new>
namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::bindSourceRaWorldZones272A48(
    const ::original_main_entry50KG::Application& root,::ra_cold_aggregate50LA::Owner& world,
    std::shared_ptr<OriginalUiLightManagerOwner50KG>& output,Diagnostic& d)noexcept {
    d={};const auto manager=root.sourceLightManager426F60Owner_;
    if(!manager||(output&&output!=manager))return fail(d,Status::identity,0x272a48);
    output=manager; //Exact existing singleton pin, even if native admission refuses.
    if(manager->issuer_!=&root||!world.sourceWorldLightZonesInvocationCurrent(*manager)||
       !manager->sourceMutationAvailable()||!manager->nativeProjectionInputsEmpty()||
       !manager->zoneReleasePreflight()||manager->raZones_)
        return fail(d,Status::identity,0x272a48);
    //Native ledger/control allocation and authority capacity, NOT source malloc.
    if(manager->entities_->objectCount423F60_>std::numeric_limits<std::uint32_t>::max()-10u||
       manager->entities_->entityAuthority_.enrollmentSerial_>std::numeric_limits<std::uint64_t>::max()-10u||
       !manager->entities_->entityAuthority_.reserveAdditional(10))return fail(d,Status::allocation,0x148b60);
    std::unique_ptr<RaZones148B60> scope(new(std::nothrow) RaZones148B60);
    if(!scope)return fail(d,Status::allocation,0x148b60);
    scope->sourceMode=true;scope->sourceWorldIdentity=&world;scope->world=&world;
    //Actual default stores precede the genuinely empty148110 call. Admission
    //above proves every old node/List/array absent, not a warm payload reset.
    constexpr Words defaults{{0x42480000u,0x42480000u,0x42480000u,0x42800000u}};
    scope->sourceEntered=true;scope->sourcePC=0x148ba0;manager->default70_=defaults;
    scope->sourcePC=0x148ba8;manager->default50_=defaults;manager->defaultsKnown_=true;
    const bool released=manager->releaseZones148110(d);
    scope->sourcePredecessorReturned=released;
    manager->raZones_=std::move(scope); //Retain even an unexpected predecessor refusal.
    if(!released)return false;
    return manager->setupSourceRaZones148B60(world,d);
}
bool OriginalUiLightManagerOwner50KG::setupSourceRaZones148B60(
    ::ra_cold_aggregate50LA::Owner& world,Diagnostic& d)noexcept {
    d={};if(!raZones_||!raZones_->sourceMode||raZones_->world!=&world||
       !raZones_->sourceEntered||!raZones_->sourcePredecessorReturned||raZones_->sourceArrayMask||
       raZones_->sourceReturned||raZones_->sourceNativeClosed||!current())return fail(d,Status::phase,0x148bb4);
    auto& r=*raZones_;const auto at=[&](std::uint32_t pc){r.sourcePC=pc;};
    const auto stop=[&](Status status,std::uint32_t pc){at(pc);return fail(d,status,pc);};
    at(0x148bb8);count90_=10;zonesAttempted_=true;
    nodes88_.reset(new(std::nothrow) Zone*[10]);at(0x148bbc);r.sourceArrayPublications[0]=nodes88_.get();r.sourceArrayMask|=1;
    lists8C_.reset(new(std::nothrow) L*[10]);at(0x148bcc);r.sourceArrayPublications[1]=lists8C_.get();r.sourceArrayMask|=2;
    ambient60_.reset(new(std::nothrow) Words[10]);at(0x148bd8);r.sourceArrayPublications[2]=ambient60_.get();r.sourceArrayMask|=4;
    ambient80_.reset(new(std::nothrow) Words[10]);at(0x148be4);r.sourceArrayPublications[3]=ambient80_.get();r.sourceArrayMask|=8;
    directional44_.reset(new(std::nothrow) Words[10]);at(0x148bf4);r.sourceArrayPublications[4]=directional44_.get();r.sourceArrayMask|=16;
    at(0x148bfc);r.sourceS0=0;at(0x148c00);r.sourceS1=0;at(0x148c04);r.sourceS2=0;
    for(std::size_t i=0;i<10;++i){
        at(0x148c08);r.sourceNodeAttempted[i]=true;r.zones[i].reset(new(std::nothrow) Zone);
        if(r.zones[i]){auto& z=*r.zones[i];auto& c=z.entity;
            if(!entities_->entityAuthority_.canEnroll(c,key(&c)))return stop(Status::topology,0x148c1c);
            const auto initialized=game::initializeOriginalWreckComponent50LA(c,K::searchNode27F120,
                generation_,key(&c),true,entities_->objectCount423F60_);
            r.sourceNodeCounted[i]=c.reference00!=0;
            if(initialized.status!=game::OriginalWreckComponentStatus50LA::complete)return stop(Status::topology,initialized.sourcePC);
            //The generic helper above represents only Entity141300. These are
            //the separate, ordered SearchNode27F120 stores, not host defaults.
            at(0x27f13c);r.sourceNodeVtable[i]=0x3d2dd0;r.sourceNodeConstructorMask[i]|=1;
            at(0x27f140);z.field30=nullptr;r.sourceNodeConstructorMask[i]|=2;
            at(0x27f144);z.first34=nullptr;r.sourceNodeConstructorMask[i]|=4;
            at(0x27f148);z.second38=nullptr;r.sourceNodeConstructorMask[i]|=8;
            at(0x27f14c);z.index48=0;r.sourceNodeConstructorMask[i]|=16;
            at(0x27f150);z.field3C=nullptr;r.sourceNodeConstructorMask[i]|=32;
            at(0x27f154);z.field40=nullptr;r.sourceNodeConstructorMask[i]|=64;
            at(0x27f158);z.field44=nullptr;r.sourceNodeConstructorMask[i]|=128;
            at(0x27f15c);z.fieldA0=nullptr;r.sourceNodeConstructorMask[i]|=256;
            z.constructed=true;
            if(!entities_->entityAuthority_.enroll(c))return stop(Status::topology,0x148c24);
            r.sourceNodeEnrolled[i]=true;
        }
        at(0x148c38);if(!nodes88_)return stop(Status::allocation,r.sourcePC);
        nodes88_[i]=r.zones[i].get();r.sourceNodeSlotKnown[i]=true;if(r.zones[i])r.zones[i]->published=true;
        //148C38 delay executes BEFORE next actual list allocation148C34.
        at(0x148c34);r.sourceListAttempted[i]=true;r.lists[i].reset(new(std::nothrow) L(*issuer_,entities_));
        if(r.lists[i]){auto& list=*r.lists[i];list.lightManagerOwner_=this;L::D ld;
            const bool built=list.construct27E260(issuer_,entities_,ld,true);
            if(!built&&!(ld.status==L::S::allocation&&ld.sourcePC==0x27ee38&&
                list.lightEmptyCurrent(this,issuer_,entities_)&&list.derivedConstructed_&&
                !list.complete_&&!list.selected1C_&&list.soultreeKnown_&&list.searchableKnown_))
                return stop(Status::list,ld.sourcePC);
            r.sourceListReturned[i]=true;
        }
        at(0x148c58);if(!lists8C_)return stop(Status::allocation,r.sourcePC);
        lists8C_[i]=r.lists[i].get();r.sourceListSlotKnown[i]=true;r.listPublished[i]=bool(r.lists[i]);
        at(0x27f434);if(!r.zones[i])return stop(Status::allocation,r.sourcePC);
        at(0x27f454);if(!r.lists[i])return stop(Status::allocation,r.sourcePC);
        auto& z=*r.zones[i];auto& list=*r.lists[i];
        if(!list.lightEmptyCurrent(this,issuer_,entities_)||!list.derivedConstructed_||
           z.first34||z.second38||list.lightAliases_)return stop(Status::phase,0x27f420);
        at(0x27f454);list.usage10_=1;at(0x27f458);z.first34=&list;at(0x10496c);++list.reference00_;++list.lightAliases_;
        at(0x27f4c4);list.usage10_=2;at(0x27f4c8);z.second38=&list;at(0x10496c);++list.reference00_;++list.lightAliases_;
        at(0x148ca0);*r.sourceS1+=4;
        at(0x148ca4);z.index48=static_cast<std::uint32_t>(i);r.sourceIndexKnown[i]=true;
        at(0x148cac);++*r.sourceS0;
        at(0x148cbc);if(!ambient80_)return stop(Status::allocation,r.sourcePC);
        ambient80_[i]=default70_;r.sourceAmbient80Known[i]=true;
        at(0x148cc8);*r.sourceS2+=16;
        at(0x148cd0);if(!ambient60_)return stop(Status::allocation,r.sourcePC);
        ambient60_[i]=default50_;r.ambientKnown[i]=true;++r.next;
    }
    //No44 dereference or legacy gameplay/UI completion grant occurs.
    r.sourceReturned=true;at(0x148cf4);d={Status::complete,0x148cf4};return true;
}
bool OriginalUiLightManagerOwner50KG::preflightSourceRaZonesNative(
    const ::ra_cold_aggregate50LA::Owner& world)const noexcept {
    //A borrowed candidate may have refused admission because somebody else owns
    //the singleton. It has no payload to retire on this World's behalf.
    if(!raZones_||!raZones_->sourceMode||raZones_->sourceWorldIdentity!=&world){
        const auto& pin=world.sourceTopologyLightManager_;
        if(!issuer_||world.sourceLoadApplication_!=issuer_||world.worldApplication_!=entities_||pin.get()!=this||
           issuer_->sourceLightManager426F60Owner_.get()!=this||
           pin.owner_before(issuer_->sourceLightManager426F60Owner_)||issuer_->sourceLightManager426F60Owner_.owner_before(pin))return false;
        return sourceIdentity(); //Only this exact rejected native lexical borrow.
    }
    const auto& r=*raZones_;
    std::uint32_t typedLiveRegistry=0;std::size_t typedPublishedRegistry=0;
    Words expectedVector30{{0x42ce0000u,0x42ce0000u,0x42b90000u,0}};std::uint32_t expectedA0=0;std::uint32_t expectedCapacity=10;bool sourceGrowthFailed=false;
    for(const auto& typed:world.sourceTypedLights_){if(!typed)return false;const auto& f=*typed;
        if(((f.spotlightConfigEntered||f.laterAmbientConfigEntered)&&f.registryColorMask)||(f.registryIndex&&*f.registryIndex>=190)||f.registryColorMask>15||f.registryA0Stored&&!f.registrationEntered)return false;
        if(f.registryGrowthEntered){
            if(!f.registryIndex||*f.registryIndex!=expectedCapacity||!f.registrationEntered||sourceGrowthFailed)return false;
            expectedCapacity=*f.registryIndex+10u;
            if(!f.registryGrowthReturned)sourceGrowthFailed=true;
        }else if(f.registryGrowthReturned)return false;
        if(f.registrationEntered&&((!f.nativeClosed&&f.registryManager!=this)||(f.nativeClosed&&f.registryManager)||!r.sourceSecondRegistered||(!f.observerCallReturned&&!f.laterAmbientConfigEntered)||!f.registryIndex))return false;
        for(unsigned k=0;k<4;++k){if(bool(f.registryColorWords[k])!=bool(f.registryColorMask&(1u<<k)))return false;
            if(f.registryColorWords[k]){std::uint32_t expected=0;
                if(k<3){if(f.spotlightHalfColor)std::memcpy(&expected,&(*f.spotlightHalfColor)[k],sizeof(expected));else expected=0x42ff0000u;}
                if(*f.registryColorWords[k]!=expected)return false;expectedVector30[k]=*f.registryColorWords[k];}}
        if(f.registryA0Stored){if(f.registryColorMask!=15||f.spotlightConfigEntered||f.laterAmbientConfigEntered)return false;expectedA0=1;}
        if(f.registrySlotPublished){if(!f.registryIndex||*f.registryIndex!=2+typedPublishedRegistry||
            (!f.spotlightConfigEntered&&!f.laterAmbientConfigEntered&&!f.registryA0Stored)||f.registryColorMask!=((f.spotlightConfigEntered||f.laterAmbientConfigEntered)?0u:15u)||!f.registryOwner||!registry9C_||
            registry9C_[*f.registryIndex]!=f.registryOwner||registryPublished_[*f.registryIndex]!=f.registryOwner)return false;++typedPublishedRegistry;}
        else if(f.registryOwner||f.registryCountStored)return false;
        if(f.registryWithdrawn&&!f.registryCountStored)return false;
        if(f.registryCountStored&&!f.registryWithdrawn){if(!f.light||f.registryOwner!=&f.light->entity||!f.light->registered)return false;++typedLiveRegistry;}
    }
    if((r.sourceAmbientAllocation&&capacity98_!=expectedCapacity)||raRegistryGrowthFailed_!=sourceGrowthFailed||
       typedPublishedRegistry>183||typedLiveRegistry>typedPublishedRegistry||(r.sourceReturned&&fieldA0_!=expectedA0))return false;
    if(!sourceIdentity()||r.canonical||r.gameplayGeneration||nativeProjectionUi_||nativeProjectionGameplayGeneration_||
       courseZoneOwner_||uiLightsOwner_||sceneLightsOwner_||zone_||list_||listPublished_||
       count94_!=std::uint32_t(r.sourceAmbientRegistered&&!r.sourceAmbientWithdrawn)+
           std::uint32_t(r.sourceSecondRegistered&&!r.sourceSecondWithdrawn)+typedLiveRegistry||
       directional40Live_||
       (registryFailed_&&!(r.sourceAmbientAllocation&&r.sourceAmbientPublished&&!r.sourceAmbientRegistered))||
       zonesComplete_||ambientKnown_||directional44Known_||
       r.firstAmbient||r.firstDirectional||r.next>10||
       (r.sourceArrayMask&~31u)||(r.sourceArrayWithdrawnMask&~r.sourceArrayMask))return false;
    for(std::size_t i=0;i<10;++i)if(r.sourceDirectionalTailRGB[i]){
        if((i!=1&&i!=2&&i!=8)||!r.sourceSecondRegistered||
           *r.sourceDirectionalTailRGB[i]!=std::array<std::uint32_t,3>{{0x41a00000u,0x41a00000u,0x41a00000u}})return false;
        if((i==2&&!r.sourceDirectionalTailRGB[1])||(i==8&&(!r.sourceDirectionalTailRGB[1]||!r.sourceDirectionalTailRGB[2])))return false;
    }
    if((r.sourceSecondRevisionBefore[1]||r.sourceSecondRevisionBefore[2]||r.sourceSecondRevisionBefore[3])&&!r.sourceSecondRevisionBefore[0])return false;
    std::uint32_t expectedRevision=r.sourceAmbientRevisionBefore?std::uint32_t(*r.sourceAmbientRevisionBefore+1u):field84_;
    for(const auto& before:r.sourceSecondRevisionBefore){
        if(before){if(!r.sourceAmbientRevisionBefore||*before!=expectedRevision)return false;++expectedRevision;}
    }
    std::size_t typedOrdinal=1;unsigned typedStage=0;
    for(const auto& receipt:r.sourceTypedRevisions){
        if(!r.sourceSecondRegistered||receipt.ordinal<2||receipt.ordinal>=185||receipt.stage>3||
           receipt.before!=expectedRevision)return false;
        if(receipt.ordinal!=typedOrdinal){
            if(receipt.ordinal!=typedOrdinal+1||receipt.stage!=0)return false;
        }else if(receipt.stage<=typedStage)return false;
        typedOrdinal=receipt.ordinal;typedStage=receipt.stage;++expectedRevision;
    }
    if(r.sourceAmbientRevisionBefore&&field84_!=expectedRevision)return false;
    if(r.sourceAmbientAttempted){
        if(!r.sourceAmbientRevisionBefore||!r.sourceAmbientColors||!r.sourceAmbientAllocation||!r.sourceAmbientPublished||
           (!capacity98_||capacity98_>190||capacity98_%10)||registryWritten_!=std::size_t(r.sourceAmbientRegistered)+std::size_t(r.sourceSecondRegistered)+typedPublishedRegistry)return false;
        if(r.sourceAmbientRegistered){if(!registry9C_||registryFailed_)return false;}
        else if(registry9C_||!registryFailed_)return false;
    }else if(r.sourceAmbientColors||r.sourceAmbientAllocation||r.sourceAmbientPublished||r.sourceAmbientRegistered||r.sourceAmbient)return false;
    if(r.sourceAmbientRegistered&&!r.sourceAmbientWithdrawn){
        if(!r.sourceAmbient||!world.sourceFirstLight_||r.sourceAmbient!=&world.sourceFirstLight_->entity||
           !world.sourceFirstLight_->registered)return false;
    }else if(r.sourceAmbient)return false;
    if(r.sourceSecondRegistered){
        if(!r.sourceAmbientRegistered||!r.sourceSecondRegistryReceipt||!registry9C_||!capacity98_||capacity98_>190||capacity98_%10||
           registryWritten_!=2+typedPublishedRegistry||registry9C_[1]!=r.sourceSecondRegistryReceipt||registryPublished_[1]!=r.sourceSecondRegistryReceipt||
           field40_!=r.sourceSecondRegistryReceipt||vector30_!=expectedVector30)return false;
        if(!r.sourceSecondWithdrawn&&(!world.sourceSecondLight_||r.sourceSecondRegistryReceipt!=&world.sourceSecondLight_->entity||
           !world.sourceSecondLight_->registered))return false;
        if(!(r.sourceArrayWithdrawnMask&16u)){
            if(!directional44_)return false;
            for(std::size_t i=0;i<10;++i){Words expected{{0x42ce0000u,0x42ce0000u,0x42b90000u,0}};
                if(r.sourceDirectionalTailRGB[i])for(unsigned k=0;k<3;++k)expected[k]=(*r.sourceDirectionalTailRGB[i])[k];
                if(directional44_[i]!=expected)return false;}
        }
    }else if(r.sourceSecondRegistryReceipt||r.sourceSecondWithdrawn)return false;
    for(std::size_t i=0;i<183;++i)if(r.localRegistered[i]||r.localPrimary[i])return false;
    if(!preflightSourceRaPortalLinksNative(world))return false;
    const void* arrays[]={nodes88_.get(),lists8C_.get(),ambient60_.get(),ambient80_.get(),directional44_.get()};
    for(unsigned a=0;a<5;++a){const auto bit=1u<<a;
        if(r.sourceArrayWithdrawnMask&bit){if(arrays[a]||r.sourceArrayPublications[a])return false;}
        else if(r.sourceArrayMask&bit){if(!r.sourceArrayPublications[a]||*r.sourceArrayPublications[a]!=arrays[a])return false;}
        else if(arrays[a]||r.sourceArrayPublications[a])return false;
    }
    if(r.sourceArrayMask){if(r.sourceArrayMask!=31||!r.sourcePredecessorReturned||count90_!=10||!zonesAttempted_)return false;}
    else if(count90_||zonesAttempted_)return false;
    if(r.sourceArrayMask&&(!r.sourceS0||!r.sourceS1||!r.sourceS2||*r.sourceS0>10||*r.sourceS1>40||*r.sourceS2>160||
       *r.sourceS1!=4u**r.sourceS0||*r.sourceS2%16u))return false;
    if(r.sourceNativeClosed){
        if(r.world||r.sourceArrayWithdrawnMask!=r.sourceArrayMask)return false;
        for(std::size_t i=0;i<10;++i)if(r.zones[i]||r.lists[i]||r.sourceNodeCounted[i]||r.sourceNodeEnrolled[i])return false;
        return true; //Historical world pointer is never dereferenced.
    }
    if(r.world!=&world||!r.sourceEntered||!current())return false;
    //Fold actual Ambient history; these values are validation expectations,
    //not a second manager payload or native restoration of previous colors.
    Words expectedDefault50= r.sourceAmbientColors?Words{{0x42480000u,0x42480000u,0x42480000u,0x42800000u}}:Words{{0,0,0,0x42800000u}};
    //Before first Ambient, retain the constructor defaults already owned here.
    if(!r.sourceAmbientColors)expectedDefault50=default50_;
    auto expectedDefault70=expectedDefault50;
    std::array<Words,10> expectedAmbient60,expectedAmbient80;
    expectedAmbient60.fill(expectedDefault50);expectedAmbient80.fill(expectedDefault70);
    for(const auto& typed:world.sourceTypedLights_)if(typed&&typed->laterAmbientStoresReturned){
        if(!typed->laterAmbientConfigEntered||!typed->zone||*typed->zone<0||*typed->zone>=10||!typed->registrationEntered)return false;
        constexpr Words ambient{{0x41480000u,0x41480000u,0x41480000u,0x42800000u}};
        expectedDefault50=expectedDefault70=ambient;
        const auto zone=static_cast<std::size_t>(*typed->zone);
        expectedAmbient60[zone]=expectedAmbient80[zone]=ambient;
        if(!zone){expectedAmbient60.fill(ambient);expectedAmbient80.fill(ambient);}
    }
    if(r.sourceAmbientColors&&(default50_!=expectedDefault50||default70_!=expectedDefault70))return false;
    std::size_t counted=0;
    for(std::size_t i=0;i<10;++i){const auto& zp=r.zones[i];const auto& lp=r.lists[i];
        if(r.sourceNodeWithdrawn[i]){if(zp||r.sourceNodeCounted[i]||r.sourceNodeEnrolled[i])return false;}
        else {
            if(r.sourceNodeSlotKnown[i]&&(!nodes88_||nodes88_[i]!=zp.get()))return false;
            if(zp){const auto& z=*zp;const auto& c=z.entity;
                if(!r.sourceNodeAttempted[i]||z.published!=r.sourceNodeSlotKnown[i]||
                   (r.sourceNodeConstructorMask[i]&~511u))return false;
                if(z.constructed){if(r.sourceNodeConstructorMask[i]!=511||!r.sourceNodeVtable[i]||
                    *r.sourceNodeVtable[i]!=0x3d2dd0||z.field3C||z.field40||z.field44||z.fieldA0||
                    z.index48!=(r.sourceIndexKnown[i]?i:0))return false;}
                else if(r.sourceNodeConstructorMask[i]||r.sourceNodeVtable[i])return false;
                if(r.sourceNodeCounted[i]){
                    ++counted;
                    if(!z.constructed||c.kind!=K::searchNode27F120||c.objectKey!=key(&c)||c.sessionGeneration!=generation_||
                       c.reference00!=1||c.previous10||c.next14||c.firstChild18||c.parent1C||c.mask20||c.descendants24||
                       c.category28!=0xff||(c.flags29&15u)!=3u||c.nativeFlags29)return false;
                }else if(c.reference00||z.constructed)return false;
                if(r.sourceNodeEnrolled[i]){if(!r.sourceNodeCounted[i]||!entities_->entityAuthority_.canRetire(c))return false;}
                else {const auto& a=entities_->entityAuthority_;for(std::size_t j=0;j<a.count_;++j)if(a.entries_[j].node==&c)return false;}
                if(z.first34!=(lp&&lp->lightAliases_>=1?lp.get():nullptr)||z.second38!=(lp&&lp->lightAliases_>=2?lp.get():nullptr))return false;
            }else if(r.sourceNodeCounted[i]||r.sourceNodeEnrolled[i]||r.sourceIndexKnown[i]||
                (lp&&lp->lightAliases_))return false;
        }
        if(r.sourceListWithdrawn[i]){if(lp)return false;}
        else {
            if(r.sourceListSlotKnown[i]&&(!lists8C_||lists8C_[i]!=lp.get()))return false;
            if(r.listPublished[i]!=(r.sourceListSlotKnown[i]&&bool(lp)))return false;
            if(lp){const auto& l=*lp;
                if(!r.sourceListAttempted[i]||l.lightManagerOwner_!=this||!l.same(issuer_,entities_)||
                   l.managerOwner_||l.loadingManagerOwner_||l.courseManagerOwner_||l.lightTable3C_||l.lightBlockPointers_[0])return false;
                if(l.attempted_){if(!l.sourceLightHistoryCurrent(this,issuer_,entities_))return false;}
                else if(l.reference00_||l.released_||l.derivedConstructed_||l.selected1C_||l.lightAliases_||l.lightReleasedAliases_)return false;
                if(r.sourceListReturned[i]&&(!l.derivedConstructed_||!l.soultreeKnown_||!l.searchableKnown_))return false;
            }else if(r.sourceListReturned[i])return false;
        }
        if(r.sourceAmbient80Known[i]&&!(r.sourceArrayWithdrawnMask&8u)){
            if(!ambient80_||ambient80_[i]!=expectedAmbient80[i])return false;}
        if(r.ambientKnown[i]&&!(r.sourceArrayWithdrawnMask&4u)){
            if(!ambient60_||ambient60_[i]!=expectedAmbient60[i])return false;}
        if(i<r.next&&(!r.sourceListReturned[i]||!r.sourceIndexKnown[i]||!r.sourceAmbient80Known[i]||!r.ambientKnown[i]))return false;
    }
    if(counted>entities_->objectCount423F60_)return false;
    return !r.sourceReturned||(r.next==10&&r.sourceS0&&*r.sourceS0==10&&r.sourceS1&&*r.sourceS1==40&&r.sourceS2&&*r.sourceS2==160);
}
bool OriginalUiLightManagerOwner50KG::sourceRaZonesCounted(
    const ::ra_cold_aggregate50LA::Owner& world,std::size_t& count)const noexcept {
    count=0;if(!preflightSourceRaZonesNative(world))return false;
    if(!raZones_||!raZones_->sourceMode||raZones_->sourceWorldIdentity!=&world||raZones_->sourceNativeClosed)return true;
    for(bool counted:raZones_->sourceNodeCounted)if(counted)++count;
    return true;
}
bool OriginalUiLightManagerOwner50KG::abortSourceRaZonesNative(
    const ::ra_cold_aggregate50LA::Owner& world,Diagnostic& d)noexcept {
    d={};
    if(world.sourcePortalGraphPhase_!=::ra_cold_aggregate50LA::Owner::SourcePortalGraphPhase::unreached&&
       world.sourcePortalGraphPhase_!=::ra_cold_aggregate50LA::Owner::SourcePortalGraphPhase::nativeClosed)
        return fail(d,Status::phase,0x148110); //Even an earlier no-manager-link Portal cut retains its manager borrow.
    if(!preflightSourceRaZonesNative(world))return fail(d,Status::phase,0x148110);
    if(!raZones_||!raZones_->sourceMode||raZones_->sourceWorldIdentity!=&world||raZones_->sourceNativeClosed)return true;
    if(!world.preflightSourceWorldTopologyCountNative())return fail(d,Status::phase,0x148110);
    auto& r=*raZones_;
    for(const auto& typed:world.sourceTypedLights_)if(typed&&typed->observerEntered&&!typed->observerWithdrawn)
        return fail(d,Status::phase,0x148110); //D4/node borrow exists even before List selection.
    // Portal owners must withdraw their raw borrows before manager node retirement.
    for(auto* portal:r.portals)if(portal)return fail(d,Status::phase,0x148110);
    for(const auto& link:r.portalLinks)if(link)return fail(d,Status::phase,0x148110);
    for(const auto& list:r.lists)if(list&&(!list->lightMembersEmpty()||list->sourceLightOccurrence_))
        return fail(d,Status::phase,0x148110); //No partial node retirement with live primary/pending borrow.
    //All aliases/nodes precede88 disposal, then all List payloads and8C.
    //Masks permit exact retries without reading stale/unwritten slots.
    for(std::size_t i=0;i<10;++i){if(r.sourceNodeWithdrawn[i])continue;
        if(r.zones[i]){auto& z=*r.zones[i];auto& c=z.entity;L::D ld;
            if(r.lists[i]){auto& list=*r.lists[i];
                if(list.lightAliases_>=1&&list.lightReleasedAliases_<1&&
                   !list.releaseLightAlias(this,issuer_,entities_,1,z.first34,ld))return fail(d,Status::list,ld.sourcePC);
                if(list.lightAliases_>=2&&list.lightReleasedAliases_<2&&
                   !list.releaseLightAlias(this,issuer_,entities_,2,z.second38,ld))return fail(d,Status::list,ld.sourcePC);
            }
            if(r.sourceNodeEnrolled[i]){if(!entities_->entityAuthority_.retire(c))return fail(d,Status::topology,0x1413e0);r.sourceNodeEnrolled[i]=false;}
            if(r.sourceNodeCounted[i]){--c.reference00;--entities_->objectCount423F60_;r.sourceNodeCounted[i]=false;}
            r.zones[i].reset();
        }
        r.sourceNodeWithdrawn[i]=true;
    }
    r.sourceArrayPublications[0].reset();nodes88_.reset();r.sourceArrayWithdrawnMask|=r.sourceArrayMask&1u;
    for(std::size_t i=0;i<10;++i){if(r.sourceListWithdrawn[i])continue;
        if(r.lists[i]){auto& list=*r.lists[i];L::D ld;
            if(list.attempted_&&!list.releaseLightPayload(this,issuer_,entities_,ld))return fail(d,Status::list,ld.sourcePC);
            r.lists[i].reset();
        }
        r.sourceListWithdrawn[i]=true;
    }
    r.sourceArrayPublications[1].reset();lists8C_.reset();r.sourceArrayWithdrawnMask|=r.sourceArrayMask&2u;
    r.sourceArrayPublications[2].reset();ambient60_.reset();r.sourceArrayWithdrawnMask|=r.sourceArrayMask&4u;
    r.sourceArrayPublications[3].reset();ambient80_.reset();r.sourceArrayWithdrawnMask|=r.sourceArrayMask&8u;
    r.sourceArrayPublications[4].reset();directional44_.reset();r.sourceArrayWithdrawnMask|=r.sourceArrayMask&16u;
    r.world=nullptr;r.sourceNativeClosed=true;
    //Keep actual count90/defaults/masks as history, not guest zero/reset.
    //Retained tombstone blocks legacy/source reentry and never dereferences World.
    d={Status::complete,0};return true;
}
}
