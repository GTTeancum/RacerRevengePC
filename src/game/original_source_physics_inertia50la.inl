#pragma once
namespace rr::game {
inline OriginalSourcePhysicsBody50LA::Result OriginalSourcePhysicsBody50LA::configureSourceInertia166558(
    const std::array<std::uint32_t,4>& actualHalf50,std::array<std::uint32_t,16>& actualWorking60,
    std::uint32_t rawInertia288)noexcept {
    auto& h=inertia_;
    if(h.entered||nativeWithdrawn_||nativeRetired_||detachedNative_||!attachReturned_||!body_||
       !dimensionsMass_.returned||dimensionsMass_.pc!=0x166558u||!preflightNative()||!modelBorrowCurrentNative()||
       dimensionsMass_.actualHalfIdentity!=&actualHalf50||actualHalf50!=dimensionsMass_.halfAfter||
       !modelBorrow_->preflightNative()||!modelBorrow_->model_||modelBorrow_->ledgerWithdrawn_||modelBorrow_->modelRetired_||
       !modelBorrow_->model_->preflightSourceRenderableReturn26E820Native()||
       modelBorrow_->model_->fields_.packets1F0!=&modelBorrow_->sourcePackets_||
       modelBorrow_->model_->sourcePacketBorrower_!=&modelBorrow_->sourcePackets_)
        return {Status::owner,0x166558u};
    for(auto word:actualWorking60)if(word)return {Status::owner,0x166558u}; //Actual first14D7F0 scratch, no invented identity.
    h.entered=true;h.rawInertia=rawInertia288;h.workingBefore=actualWorking60;h.actualWorkingIdentity=&actualWorking60;
    const auto stop=[&](std::uint32_t pc){h.pc=pc;return Result{Status::bounded,pc};};
    const auto supported=[](std::uint32_t bits){const auto magnitude=bits&0x7fffffffu,exponent=bits&0x7f800000u;
        return !magnitude||(exponent&&exponent!=0x7f800000u);};
    const auto binary=[&](char op,std::uint32_t a,std::uint32_t b,std::uint32_t& result)->bool {
        if(std::fegetround()!=FE_TONEAREST||!supported(a)||!supported(b))return false;
        float x,y,value;std::memcpy(&x,&a,4);std::memcpy(&y,&b,4);
        if(op=='*'){volatile float rounded=x*y;value=rounded;}
        else if(op=='+'){volatile float rounded=x+y;value=rounded;}
        else if(op=='-'){volatile float rounded=x-y;value=rounded;}
        else {if(!(b&0x7fffffffu))return false;volatile float rounded=x/y;value=rounded;}
        std::uint32_t raw;std::memcpy(&raw,&value,4);if(!supported(raw))return false;result=raw;return true;
    };
    const auto arithmetic=[&](unsigned slot,std::uint32_t pc,char op,std::uint32_t a,std::uint32_t b)->bool {
        h.pc=pc;std::uint32_t result;if(!binary(op,a,b,result))return false;
        h.arithmetic[slot]=result;h.arithmeticKnown|=1u<<slot;return true;
    };
    //Second real14D7F0: all sixteen zero words, not identity. Then four
    //independent SQ publications to actual caller working60, in source order.
    h.pc=0x14d7f0u;for(unsigned i=0;i<16;++i){h.zeroF0[i]=0;h.zeroKnown|=std::uint16_t(1u<<i);}
    h.zeroReturned=true;
    for(unsigned quad=0;quad<4;++quad){h.pc=0x166578u+quad*4;
        for(unsigned lane=0;lane<4;++lane)actualWorking60[quad*4+lane]=h.zeroF0[quad*4+lane];
        h.zeroCopyMask|=std::uint8_t(1u<<quad);h.workingAfter=actualWorking60;}
    for(unsigned i=0;i<3;++i)h.halfReload[i]=actualHalf50[i]; //Source16658C/590/594 reads Y,Z,X.
    h.pc=0x1665a0u;std::memcpy(&h.massReload[0],&body_->physical.parameters.body.mass,4);h.massReadMask=1;
    const auto x=h.halfReload[0],y=h.halfReload[1],z=h.halfReload[2];
    if(!arithmetic(0,0x1665a4u,'*',y,y)||!arithmetic(1,0x1665a8u,'*',z,z)||
       !arithmetic(2,0x1665a8u,'+',h.arithmetic[0],h.arithmetic[1])||
       !arithmetic(3,0x1665acu,'*',x,x)||!arithmetic(4,0x1665b0u,'*',z,z)||
       !arithmetic(5,0x1665b0u,'+',h.arithmetic[3],h.arithmetic[4])||
       !arithmetic(6,0x1665b4u,'*',x,x)||!arithmetic(7,0x1665b8u,'*',y,y)||
       !arithmetic(8,0x1665b8u,'+',h.arithmetic[6],h.arithmetic[7])||
       !arithmetic(9,0x1665bcu,'/',h.massReload[0],0x41400000u)||
       !arithmetic(10,0x1665c4u,'*',h.rawInertia,h.arithmetic[9])||
       !arithmetic(11,0x1665c8u,'*',h.arithmetic[10],h.arithmetic[2]))return stop(h.pc);
    h.pc=0x1665ccu;actualWorking60[0]=h.arithmetic[11];h.workingAfter=actualWorking60;h.diagonalMask=1;
    h.pc=0x1665d4u;std::memcpy(&h.massReload[1],&body_->physical.parameters.body.mass,4);h.massReadMask|=2;
    if(!arithmetic(12,0x1665d8u,'/',h.massReload[1],0x41400000u)||
       !arithmetic(13,0x1665dcu,'*',h.rawInertia,h.arithmetic[12])||
       !arithmetic(14,0x1665e0u,'*',h.arithmetic[13],h.arithmetic[5]))return stop(h.pc);
    h.pc=0x1665e4u;actualWorking60[5]=h.arithmetic[14];h.workingAfter=actualWorking60;h.diagonalMask|=2;
    h.pc=0x1665ecu;std::memcpy(&h.massReload[2],&body_->physical.parameters.body.mass,4);h.massReadMask|=4;
    if(!arithmetic(15,0x1665f0u,'/',h.massReload[2],0x41400000u)||
       !arithmetic(16,0x1665f4u,'*',h.rawInertia,h.arithmetic[15])||
       !arithmetic(17,0x1665f8u,'*',h.arithmetic[16],h.arithmetic[8]))return stop(h.pc);
    h.pc=0x1665fcu;actualWorking60[10]=h.arithmetic[17];h.workingAfter=actualWorking60;h.diagonalMask|=4;
    h.pc=0x166600u;actualWorking60[15]=0x3f800000u;h.workingAfter=actualWorking60;h.diagonalMask|=8;
    h.pc=0x166608u;if(body_->dispatch08!=0x3ce000u)return stop(h.pc); //Actual+7C resolves165340.
    h.setterEntered=true;
    for(unsigned pair=0;pair<8;++pair)for(unsigned lane=0;lane<2;++lane){const auto i=pair*2+lane;
        h.pc=lane?0x165368u:0x165364u;std::memcpy(body_->physical.inertia100.v+i,actualWorking60.data()+i,4);
        h.body100[i]=actualWorking60[i];h.body100Mask|=std::uint16_t(1u<<i);}
    h.pc=0x16537cu;h.inverseEntered=true;
    std::memcpy(h.inverseInput.data(),body_->physical.inertia100.v,64); //14E910 four quads into private stack.
    auto& reg=h.inverseRegisters;
    const auto load=[&](unsigned to,unsigned lane,std::uint32_t pc){h.pc=pc;reg[to]=h.inverseInput[lane];h.inverseRegisterKnown|=1u<<to;};
    const auto step=[&](std::uint32_t pc,char op,unsigned to,unsigned fs,unsigned ft)->bool {
        h.pc=pc;if(!(h.inverseRegisterKnown&(1u<<fs))||(op!='n'&&!(h.inverseRegisterKnown&(1u<<ft))))return false;
        std::uint32_t value,product;
        if(op=='n'){if(!supported(reg[fs]))return false;value=reg[fs]^0x80000000u;}
        else if(op=='M'||op=='N'){
            if(!h.inverseAccumulator||!binary('*',reg[fs],reg[ft],product)||
               !binary(op=='M'?'+':'-',*h.inverseAccumulator,product,value))return false;
        }else if(!binary(op=='P'?'*':op=='S'?'-':op,reg[fs],reg[ft],value))return false;
        if(op=='P'||op=='S')h.inverseAccumulator=value;
        else {reg[to]=value;h.inverseRegisterKnown|=1u<<to;}
        ++h.inverseOperationCount;return true;
    };
    const auto store=[&](unsigned from,unsigned lane,std::uint32_t pc){h.pc=pc;h.inverseLocal[lane]=reg[from];h.inverseLocalKnown|=std::uint16_t(1u<<lane);};
    const auto storeZero=[&](unsigned lane,std::uint32_t pc){h.pc=pc;h.inverseLocal[lane]=0;h.inverseLocalKnown|=std::uint16_t(1u<<lane);};
    h.pc=0x14e928u;reg[16]=0;h.inverseRegisterKnown|=1u<<16;
    load(7,5,0x14e93cu);
    load(6,10,0x14e940u);
    load(8,8,0x14e944u);
    load(9,9,0x14e948u);
    load(5,6,0x14e94cu);
    load(10,4,0x14e950u);
    load(3,0,0x14e954u);
    if(!step(0x14e958u,'P',0,7,6))return stop(h.pc);
    load(13,1,0x14e95cu);
    if(!step(0x14e960u,'*',0,8,7))return stop(h.pc);
    load(12,2,0x14e964u);
    if(!step(0x14e968u,'N',11,9,5))return stop(h.pc);
    if(!step(0x14e96cu,'P',0,10,6))return stop(h.pc);
    if(!step(0x14e970u,'N',14,8,5))return stop(h.pc);
    if(!step(0x14e974u,'*',2,3,11))return stop(h.pc);
    if(!step(0x14e978u,'*',1,13,14))return stop(h.pc);
    if(!step(0x14e97cu,'S',0,2,1))return stop(h.pc);
    if(!step(0x14e980u,'*',1,10,9))return stop(h.pc);
    if(!step(0x14e984u,'-',15,1,0))return stop(h.pc);
    if(!step(0x14e988u,'M',4,12,15))return stop(h.pc);
    h.pc=0x14e98cu;
    if(!(h.inverseRegisterKnown&(1u<<4))||!supported(reg[4]))return stop(h.pc);
    h.determinantZero=(reg[4]&0x7fffffffu)==0;
    if(!*h.determinantZero){
    h.pc=0x14e9a0u;reg[2]=0x3f800000u;h.inverseRegisterKnown|=1u<<2;
    if(!step(0x14e9a4u,'P',0,13,6))return stop(h.pc);
    if(!step(0x14e9a8u,'/',4,2,4))return stop(h.pc);
    if(!step(0x14e9acu,'*',2,4,11))return stop(h.pc);
    if(!step(0x14e9b0u,'N',1,9,12))return stop(h.pc);
    if(!step(0x14e9b4u,'n',1,1,0))return stop(h.pc);
    if(!step(0x14e9b8u,'*',1,4,1))return stop(h.pc);
    store(2,0,0x14e9bcu);
    if(!step(0x14e9c0u,'P',0,13,5))return stop(h.pc);
    if(!step(0x14e9c4u,'N',2,7,12))return stop(h.pc);
    if(!step(0x14e9c8u,'*',2,4,2))return stop(h.pc);
    store(1,1,0x14e9ccu);
    if(!step(0x14e9d0u,'n',0,14,0))return stop(h.pc);
    if(!step(0x14e9d4u,'*',1,4,0))return stop(h.pc);
    store(2,2,0x14e9d8u);
    storeZero(3,0x14e9dcu);
    if(!step(0x14e9e0u,'P',0,3,6))return stop(h.pc);
    store(1,4,0x14e9e4u);
    if(!step(0x14e9e8u,'N',1,8,12))return stop(h.pc);
    if(!step(0x14e9ecu,'*',1,4,1))return stop(h.pc);
    if(!step(0x14e9f0u,'P',0,3,5))return stop(h.pc);
    store(1,5,0x14e9f4u);
    if(!step(0x14e9f8u,'N',1,10,12))return stop(h.pc);
    if(!step(0x14e9fcu,'n',1,1,0))return stop(h.pc);
    if(!step(0x14ea00u,'*',1,4,1))return stop(h.pc);
    store(1,6,0x14ea04u);
    if(!step(0x14ea08u,'P',0,3,9))return stop(h.pc);
    if(!step(0x14ea0cu,'N',1,8,13))return stop(h.pc);
    if(!step(0x14ea10u,'n',1,1,0))return stop(h.pc);
    if(!step(0x14ea14u,'*',0,4,15))return stop(h.pc);
    storeZero(7,0x14ea18u);
    store(0,8,0x14ea1cu);
    if(!step(0x14ea20u,'P',0,3,7))return stop(h.pc);
    if(!step(0x14ea24u,'N',0,10,13))return stop(h.pc);
    if(!step(0x14ea28u,'*',1,4,1))return stop(h.pc);
    if(!step(0x14ea2cu,'*',0,4,0))return stop(h.pc);
    store(1,9,0x14ea30u);
    store(0,10,0x14ea34u);
    storeZero(11,0x14ea38u);
    storeZero(12,0x14ea3cu);
    storeZero(13,0x14ea40u);
    storeZero(14,0x14ea44u);
    storeZero(15,0x14ea4cu);
    }else {
        //Actual14EA50..14EAA8 zero result, including v15. No identity fallback.
        h.pc=0x14ea50u;for(unsigned i=0;i<16;++i){h.inverseLocal[i]=0;h.inverseLocalKnown|=std::uint16_t(1u<<i);}
    }
    for(unsigned pair=0;pair<8;++pair)for(unsigned lane=0;lane<2;++lane){const auto i=pair*2+lane;
        h.pc=lane?0x14eac4u:0x14eac0u;h.inverseOutputMask|=std::uint16_t(1u<<i);}
    h.inverseReturned=true; //Actual14EAD4 JR/14EAD8 delay, caller scratch20 complete.
    for(unsigned pair=0;pair<8;++pair)for(unsigned lane=0;lane<2;++lane){const auto i=pair*2+lane;
        h.pc=lane?0x1653a0u:0x16539cu;std::memcpy(&inverseWordNative(i),h.inverseLocal.data()+i,4);
        h.body140[i]=h.inverseLocal[i];h.body140Mask|=std::uint16_t(1u<<i);}
    h.returned=true;h.pc=0x166618u;return {Status::reached,h.pc};
}
inline bool OriginalSourcePhysicsBody50LA::inertiaHistoryNative()const noexcept {
    const auto& h=inertia_;
    if(!h.entered)return !h.actualWorkingIdentity&&!h.pc&&!h.zeroReturned&&!h.zeroKnown&&!h.zeroCopyMask&&
        !h.diagonalMask&&!h.arithmeticKnown&&!h.setterEntered&&!h.body100Mask&&!h.inverseEntered&&
        !h.inverseLocalKnown&&!h.inverseReturned&&!h.inverseOutputMask&&!h.body140Mask&&!h.returned;
    if(!body_||!dimensionsMass_.returned||!h.actualWorkingIdentity||!h.zeroReturned||h.zeroKnown!=0xffffu||
       h.zeroCopyMask!=15u||h.arithmeticKnown>0x3ffffu||(h.arithmeticKnown&(h.arithmeticKnown+1u))||
       (h.diagonalMask!=0&&h.diagonalMask!=1&&h.diagonalMask!=3&&h.diagonalMask!=7&&h.diagonalMask!=15)||
       (h.massReadMask!=1&&h.massReadMask!=3&&h.massReadMask!=7))return false;
    for(unsigned i=0;i<16;++i)if(h.workingBefore[i]||h.zeroF0[i])return false;
    for(unsigned i=0;i<3;++i){if(h.halfReload[i]!=dimensionsMass_.halfAfter[i])return false;
        if((h.massReadMask&(1u<<i))&&h.massReload[i]!=(dimensionsMass_.massStored?dimensionsMass_.arithmetic[10]:0u))return false;}
    if((h.diagonalMask&1u)&&!(h.arithmeticKnown&(1u<<11)))return false;
    if((h.diagonalMask&2u)&&(!(h.arithmeticKnown&(1u<<14))||!(h.massReadMask&2u)))return false;
    if((h.diagonalMask&4u)&&(!(h.arithmeticKnown&(1u<<17))||!(h.massReadMask&4u)))return false;
    for(unsigned i=0;i<16;++i){const auto expected=i==0&&(h.diagonalMask&1u)?h.arithmetic[11]:
        i==5&&(h.diagonalMask&2u)?h.arithmetic[14]:i==10&&(h.diagonalMask&4u)?h.arithmetic[17]:
        i==15&&(h.diagonalMask&8u)?0x3f800000u:0u;
        if(h.workingAfter[i]!=expected)return false;
        if((h.body100Mask&(1u<<i))&&h.body100[i]!=expected)return false;
        if((h.body140Mask&(1u<<i))&&h.body140[i]!=h.inverseLocal[i])return false;
    }
    if(h.setterEntered&&h.diagonalMask!=15)return false;
    if(h.body100Mask&&(!h.setterEntered||(h.body100Mask&(h.body100Mask+1u))))return false;
    if(h.inverseEntered){
        if(h.body100Mask!=0xffffu||h.inverseInput!=h.body100||!(h.inverseRegisterKnown&(1u<<16))||
           h.inverseRegisters[16]||h.inverseOperationCount>37u||(h.inverseLocalKnown&(h.inverseLocalKnown+1u)))return false;
        if(h.determinantZero&&h.inverseOperationCount<11)return false;
        if(h.determinantZero&&*h.determinantZero){
            if(h.inverseOperationCount!=11||h.inverseLocalKnown!=0xffffu)return false;
            for(auto word:h.inverseLocal)if(word)return false;
        }
    }else if(h.inverseRegisterKnown||h.inverseOperationCount||h.inverseAccumulator||h.determinantZero||
        h.inverseLocalKnown||h.inverseReturned||h.inverseOutputMask||h.body140Mask)return false;
    if(h.inverseReturned&&(!h.determinantZero||h.inverseLocalKnown!=0xffffu||h.inverseOutputMask!=0xffffu||
        h.inverseOperationCount!=(*h.determinantZero?11u:37u)))return false;
    if(h.body140Mask&&(!h.inverseReturned||(h.body140Mask&(h.body140Mask+1u))))return false;
    return !h.returned||(h.inverseReturned&&h.body140Mask==0xffffu&&h.pc==0x166618u);
}
}
