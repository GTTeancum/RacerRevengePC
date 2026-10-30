#pragma once
namespace rr::game {
inline OriginalSourcePhysicsBody50LA::Result OriginalSourcePhysicsBody50LA::configureSourceDimensionsMass1664A0(
    OriginalSourcePhysicsModel50LA& nested,std::array<std::uint32_t,4>& actualHalf50,std::uint32_t rawWeight284)noexcept {
    auto& h=dimensionsMass_;
    if(h.entered||nativeWithdrawn_||nativeRetired_||detachedNative_||!attachReturned_||!body_||
       !preflightNative()||!modelBorrowCurrentNative()||modelBorrow_!=&nested||nested.base_!=base_||
       !nested.preflightNative()||nested.ledgerWithdrawn_||nested.modelRetired_||nested.nativeWithdrawn_||
       !nested.model_||!nested.localBounds_.returned||!nested.localBounds_.halfStored||
       !nested.localBoundsHistoryNative()||actualHalf50!=nested.localBounds_.half||
       !nested.model_->preflightSourceRenderableReturn26E820Native()||
       nested.model_->fields_.packets1F0!=&nested.sourcePackets_||
       nested.model_->sourcePacketBorrower_!=&nested.sourcePackets_||
       !base_->object_->bodyD0||*base_->object_->bodyD0!=&body_->entity||
       base_->object_->model14C!=static_cast<const void*>(&nested.model_->model_)||
       base_->sourceInverseExtent118_.value)return {Status::owner,0x1664a0u};
    h.entered=true;h.rawWeight=rawWeight284;h.halfBefore=actualHalf50;h.actualHalfIdentity=&actualHalf50;
    std::memcpy(&h.beforeMass,&body_->physical.parameters.body.mass,4);
    std::memcpy(&h.beforeInverseMass,&body_->physical.parameters.body.inverseMass,4);
    const auto stop=[&](std::uint32_t pc){h.pc=pc;return Result{Status::bounded,pc};};
    const auto supported=[](std::uint32_t bits){const auto magnitude=bits&0x7fffffffu;const auto exponent=bits&0x7f800000u;
        return !magnitude||(exponent&&exponent!=0x7f800000u);};
    //Native admitted normal/zero binary32 lane. No subnormal/NaN/Inf policy
    //is silently supplied; refusal precedes this operation's destination store.
    const auto operation=[&](unsigned slot,std::uint32_t pc,char op,std::uint32_t a,std::uint32_t b)->bool {
        h.pc=pc;if(std::fegetround()!=FE_TONEAREST||!supported(a)||(op!='s'&&!supported(b)))return false;
        float x,y,value;std::memcpy(&x,&a,4);std::memcpy(&y,&b,4);
        if(op=='*'){volatile float rounded=x*y;value=rounded;}
        else if(op=='+'){volatile float rounded=x+y;value=rounded;}
        else if(op=='/'){if((b&0x7fffffffu)==0)return false;volatile float rounded=x/y;value=rounded;}
        else {if(x<0.f)return false;volatile float rounded=static_cast<float>(std::sqrt(static_cast<double>(x)));value=rounded;}
        std::uint32_t raw;std::memcpy(&raw,&value,4);if(!supported(raw))return false;
        h.arithmetic[slot]=raw;h.arithmeticKnown|=std::uint16_t(1u<<slot);return true;
    };
    if(!operation(0,0x1664b8u,'*',0x40000000u,h.halfBefore[0]))return stop(h.pc);
    if(!operation(1,0x1664c0u,'*',0x40000000u,h.halfBefore[1]))return stop(h.pc);
    h.pc=0x1664c4u;h.temporaryE0[0]=h.arithmetic[0];h.temporaryKnown=1;
    if(!operation(2,0x1664c8u,'*',0x40000000u,h.halfBefore[2]))return stop(h.pc);
    h.pc=0x1664ccu;h.temporaryE0[1]=h.arithmetic[1];h.temporaryKnown|=2;
    h.pc=0x1664d0u;h.temporaryE0[2]=h.arithmetic[2];h.temporaryKnown|=4;
    h.pc=0x1664d4u;h.temporaryE0[3]=0x3f800000u;h.temporaryKnown|=8;
    h.pc=0x1664dcu;actualHalf50=h.temporaryE0;h.halfAfter=actualHalf50;h.halfDoubledStored=true;
    h.halfReload=actualHalf50; //Actual1664E0/E4/E8 reloads, not the old half extents.
    if(!operation(3,0x1664ecu,'*',h.halfReload[0],h.halfReload[0])||
       !operation(4,0x1664f0u,'*',h.halfReload[1],h.halfReload[1])||
       !operation(5,0x1664f4u,'+',h.arithmetic[3],h.arithmetic[4])||
       !operation(6,0x1664f8u,'*',h.halfReload[2],h.halfReload[2])||
       !operation(7,0x1664f8u,'+',h.arithmetic[5],h.arithmetic[6])||
       !operation(8,0x1664fcu,'s',h.arithmetic[7],0u))return stop(h.pc);
    h.pc=0x166500u;h.lengthStored=true; //Actual stack13C contains arithmetic[8].
    float length;std::memcpy(&length,&h.arithmetic[8],4);h.pc=0x166510u;h.lengthNonpositive=length<=0.f;
    if(!*h.lengthNonpositive){
        if(!operation(9,0x166528u,'/',0x3f800000u,h.arithmetic[8]))return stop(h.pc);
        auto& extent=base_->sourceInverseExtent118_;h.pc=0x16652cu;
        extent.before=base_->object_->fields.words[0x118/4];extent.occurrence=this;extent.value=h.arithmetic[9];
        base_->object_->fields.word(0x118,h.arithmetic[9]);h.inverseExtentStored=true;
    }
    if(!operation(10,0x166540u,'/',h.rawWeight,0x4200cccdu))return stop(h.pc);
    h.requestedMassReturned=true;h.pc=0x166548u;
    if(body_->dispatch08!=0x3ce000u)return stop(h.pc); //Actual table+78 is165300, not a host function pointer.
    h.massCallEntered=true;h.pc=0x165308u;
    float requested;std::memcpy(&requested,&h.arithmetic[10],4);h.massNonpositive=requested<=0.f;
    if(!*h.massNonpositive){
        h.pc=0x165318u;std::memcpy(&body_->physical.parameters.body.mass,&h.arithmetic[10],4);h.massStored=true;
        std::uint32_t actual;std::memcpy(&actual,&body_->physical.parameters.body.mass,4);h.massReload=actual; //16531C.
        if(!operation(11,0x16532cu,'/',0x3f800000u,actual))return stop(h.pc);
        h.pc=0x165330u;std::memcpy(&body_->physical.parameters.body.inverseMass,&h.arithmetic[11],4);h.inverseMassStored=true;
    }
    h.massCallReturned=true;h.returned=true;h.pc=0x166558u;return {Status::reached,h.pc};
    //Actual165334 JR/165338 delay completed. Second zero-scratch and inertia remain unissued.
}
inline bool OriginalSourcePhysicsBody50LA::dimensionsMassHistoryNative()const noexcept {
    const auto& h=dimensionsMass_;
    if(!h.entered)return !h.actualHalfIdentity&&!h.pc&&!h.arithmeticKnown&&!h.temporaryKnown&&
        !h.halfDoubledStored&&!h.lengthStored&&!h.inverseExtentStored&&!h.requestedMassReturned&&
        !h.massCallEntered&&!h.massStored&&!h.inverseMassStored&&!h.massCallReturned&&!h.returned;
    if(!body_||!base_||!base_->object_||!h.actualHalfIdentity||h.beforeMass||h.beforeInverseMass||
       h.arithmeticKnown>4095u||(h.temporaryKnown!=0&&h.temporaryKnown!=1&&h.temporaryKnown!=15))return false;
    for(unsigned i=1;i<9;++i)if((h.arithmeticKnown&(1u<<i))&&!(h.arithmeticKnown&(1u<<(i-1))))return false;
    if(h.temporaryKnown){if((h.arithmeticKnown&3u)!=3u||h.temporaryE0[0]!=h.arithmetic[0])return false;}
    if(h.temporaryKnown==15){if((h.arithmeticKnown&7u)!=7u||h.temporaryE0[1]!=h.arithmetic[1]||
        h.temporaryE0[2]!=h.arithmetic[2]||h.temporaryE0[3]!=0x3f800000u)return false;}
    if(h.halfDoubledStored!=(h.temporaryKnown==15))return false;
    if(h.halfDoubledStored&&(h.halfAfter!=h.temporaryE0||h.halfReload!=h.halfAfter))return false;
    if(h.lengthStored!=bool(h.arithmeticKnown&(1u<<8))||h.lengthNonpositive.has_value()!=h.lengthStored)return false;
    const auto& extent=base_->sourceInverseExtent118_;
    if(h.inverseExtentStored){if(!h.lengthNonpositive||*h.lengthNonpositive||!(h.arithmeticKnown&(1u<<9))||
        extent.occurrence!=this||extent.nativeWithdrawn||extent.value!=std::optional<std::uint32_t>(h.arithmetic[9]))return false;}
    else if(extent.value||extent.occurrence||(h.arithmeticKnown&(1u<<9)))return false;
    if(h.requestedMassReturned!=bool(h.arithmeticKnown&(1u<<10)))return false;
    if(h.requestedMassReturned&&(!h.lengthStored||(!*h.lengthNonpositive&&!h.inverseExtentStored)))return false;
    if(h.massCallEntered&&(!h.requestedMassReturned||!h.massNonpositive))return false;
    if(h.massNonpositive&&!h.massCallEntered)return false;
    if(h.massStored){if(!h.massNonpositive||*h.massNonpositive||h.massReload!=std::optional<std::uint32_t>(h.arithmetic[10]))return false;}
    else if(h.massReload||h.inverseMassStored||(h.arithmeticKnown&(1u<<11)))return false;
    if(h.inverseMassStored!=bool(h.arithmeticKnown&(1u<<11)))return false;
    if(h.massCallReturned&&(!h.massCallEntered||(!*h.massNonpositive&&!h.inverseMassStored)))return false;
    return h.returned==h.massCallReturned&&(!h.returned||h.pc==0x166558u);
}
}
