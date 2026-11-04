#pragma once
#include <array>
#include <bit>
#include <cstdint>
#include <optional>

namespace rr::game {
//One invocation of the bounded231150 helper, from its reached23116C
//continuation through the caller BEQ and its NOP. The phase types retain the
//existing value lemmas; this engine adds no floating-point implementation.
//The callee slots are retained native backing for that invocation's symbolic
//frame, not newly assigned guest addresses. Initial bits confer no knownness.
template<class PhaseOwner,class Center,class Portal>
struct OriginalBoundedPortalQuery50LA {
    using History=typename PhaseOwner::PortalQuery;
    using Words=std::array<std::uint32_t,4>;
    using Vertices=std::array<Words,4>;
    using Relative=typename PhaseOwner::PortalRelative;
    using Dot=typename PhaseOwner::PortalDot;
    using Slab=typename PhaseOwner::PortalSlab;
    using Loop=typename PhaseOwner::PortalLoop;

    OriginalBoundedPortalQuery50LA()=default;
    OriginalBoundedPortalQuery50LA(const OriginalBoundedPortalQuery50LA&)=delete;
    OriginalBoundedPortalQuery50LA& operator=(const OriginalBoundedPortalQuery50LA&)=delete;
    OriginalBoundedPortalQuery50LA(OriginalBoundedPortalQuery50LA&&)=delete;
    OriginalBoundedPortalQuery50LA& operator=(OriginalBoundedPortalQuery50LA&&)=delete;

    const Portal* portal=nullptr;
    const Vertices* vertices=nullptr;
    const Center* center=nullptr;
    const Words* normal=nullptr;
    const Words* firstEdge=nullptr;
    std::array<const Words*,3> loopVertices{},loopEdges{};
    Words callee00{},callee10{},callee20{};
    std::uint32_t callee3C=0;
    Words* relativeA3=nullptr;
    const Words* edgeRelativeT0=nullptr;
    std::uint32_t* temporaryV0=nullptr;
    std::uint32_t* outputA2=nullptr;
    //Native storage identity, not an extra guest register or output copy.
    std::uint32_t* sharedOutputStorage=nullptr;
    bool invoked=false;

    bool aliasesCleared()const noexcept {
        if(portal||vertices||center||normal||firstEdge||relativeA3||edgeRelativeT0||
           temporaryV0||outputA2||sharedOutputStorage)return false;
        for(unsigned n=0;n<3;++n)if(loopVertices[n]||loopEdges[n])return false;
        return true;
    }
    void retire()noexcept {
        portal=nullptr;vertices=nullptr;center=nullptr;normal=nullptr;firstEdge=nullptr;
        loopVertices.fill(nullptr);loopEdges.fill(nullptr);
        relativeA3=nullptr;edgeRelativeT0=nullptr;temporaryV0=nullptr;
        outputA2=nullptr;sharedOutputStorage=nullptr;
        //Histories and source-written scratch remain; this object is not reusable.
    }

    template<class Inputs>
    bool current(const History& q,const Inputs& inputs,const Portal* actualPortal,
                 const Vertices* actualVertices,const Center* actualCenter,
                 const std::uint32_t* sharedOutput,bool withdrawn)const noexcept {
        if(q.entered!=invoked||!q.historyCurrent(inputs))return false;
        const auto& relative=q.portalRelative;const auto& dot=q.portalDot;
        const auto& slab=q.portalSlab;const auto& edge0=q.portalEdge0;const auto& loop=q.portalLoop;
        //Each lane follows its most recent reached SW. A later iteration can
        //replace only a prefix of20; do not compare it to one whole old quad.
        for(unsigned k=0;k<4;++k)if(relative.temporary10[k]&&callee10[k]!=*relative.temporary10[k])return false;
        std::array<std::optional<std::uint32_t>,4> latest20;
        const Words* latest00=relative.local00?&*relative.local00:nullptr;
        for(const auto& it:loop.iterations){
            for(unsigned k=0;k<4;++k)if(it.temporary20[k])latest20[k]=it.temporary20[k];
            if(it.local00)latest00=&*it.local00;
        }
        for(unsigned k=0;k<4;++k)if(latest20[k]&&callee20[k]!=*latest20[k])return false;
        if((latest00&&callee00!=*latest00)||(dot.temporary3C&&callee3C!=*dot.temporary3C))return false;
        if(withdrawn||!q.entered)return aliasesCleared();
        if(!inputs.helperEntered()||!actualPortal||!actualVertices||!actualCenter||!sharedOutput||
           portal!=actualPortal||vertices!=actualVertices||center!=actualCenter||
           sharedOutputStorage!=sharedOutput)return false;
        if(edge0.step>=14?bool(outputA2):outputA2!=sharedOutput)return false;
        if(loop.iterations[0].step>=8?bool(relativeA3):
           (relative.step>=3?relativeA3!=&callee00:bool(relativeA3)))return false;
        if(slab.negativeStep>=5?edgeRelativeT0!=&callee00:bool(edgeRelativeT0))return false;
        if(dot.step>=10?temporaryV0!=&callee3C:bool(temporaryV0))return false;
        if(bool(normal)!=(dot.step>=1)||bool(firstEdge)!=(edge0.step>=3))return false;
        for(unsigned k=0;k<3;++k){
            if(inputs.centerWords[k]&&*inputs.centerWords[k]!=std::bit_cast<std::uint32_t>(center->v[k]))return false;
            if(inputs.vertexWords[k]&&*inputs.vertexWords[k]!=(*vertices)[0][k])return false;
            if(normal&&dot.normalVF5[k]!=(*normal)[k])return false;
            if(firstEdge&&edge0.edgeVF5[k]!=(*firstEdge)[k])return false;
        }
        for(unsigned n=0;n<3;++n){const auto& it=loop.iterations[n];
            if(bool(loopVertices[n])!=(it.step>=1)||bool(loopEdges[n])!=(it.step>=8))return false;
            if(loopVertices[n]&&loopVertices[n]!=&(*vertices)[n+1])return false;
            for(unsigned k=0;k<3;++k){
                if(it.centerWords[k]&&*it.centerWords[k]!=std::bit_cast<std::uint32_t>(center->v[k]))return false;
                if(it.vertexWords[k]&&(!loopVertices[n]||*it.vertexWords[k]!=(*loopVertices[n])[k]))return false;
                if(it.edgeVF5[k]&&(!loopEdges[n]||*it.edgeVF5[k]!=(*loopEdges[n])[k]))return false;
            }
        }
        //The caller owns latest-writer validation of shared SP+AC. An older
        //query's outputDistance is a snapshot, not a claim about its live bits.
        return true;
    }

    template<class Inputs,class ActualOperand>
    bool run(History& q,const Inputs& inputs,const Portal* actualPortal,
             const Vertices* actualVertices,const Center* actualCenter,
             std::uint32_t* sharedOutput,ActualOperand&& actualOperand,std::uint32_t& pc)noexcept {
        if(invoked||q.entered||pc!=0x23116cu||!inputs.helperEntered()||!q.historyCurrent(inputs)||
           !aliasesCleared()||!actualPortal||!actualVertices||!actualCenter||!sharedOutput||
           inputs.radiusF12!=0u||inputs.callContinuation!=0x280c54u)return false;
        for(unsigned k=0;k<3;++k)if(!inputs.centerWords[k]||!inputs.vertexWords[k]||
           *inputs.centerWords[k]!=std::bit_cast<std::uint32_t>(actualCenter->v[k])||
           *inputs.vertexWords[k]!=(*actualVertices)[0][k])return false;
        invoked=q.entered=true;portal=actualPortal;vertices=actualVertices;center=actualCenter;
        sharedOutputStorage=outputA2=sharedOutput;
        const auto at=[&](std::uint32_t p){pc=q.pc=p;};
        //Every source operand request starts with fresh null outputs. Successful
        //requests must identify this same portal and its same vertex storage.
        const auto operand=[&](unsigned index,unsigned stage,const Words*& result){
            const Portal* p=nullptr;const Vertices* v=nullptr;const Words* w=nullptr;
            const bool accepted=actualOperand(index,stage,p,v,w,pc);at(pc);
            if(!accepted)return false;
            if(p!=portal||v!=vertices||(stage==1?bool(w):!w))return false;
            result=w;return true;
        };
        {
            auto& f=q.portalRelative;
            at(0x23116c);const auto x=f.difference(*inputs.centerWords[0],*inputs.vertexWords[0]);if(!x)return false;
            f.differences[0]=x;f.step=1;
            at(0x231170);f.oneV0=0x3f800000u;f.step=2;
            at(0x231174);relativeA3=&callee00;f.step=3;
            at(0x231178);const auto y=f.difference(*inputs.centerWords[1],*inputs.vertexWords[1]);if(!y)return false;
            f.differences[1]=y;f.step=4;
            at(0x23117c);callee10[0]=*x;f.temporary10[0]=x;f.step=5;
            at(0x231180);f.normalOffset=0x70u;f.step=6;
            at(0x231184);const auto z=f.difference(*inputs.centerWords[2],*inputs.vertexWords[2]);if(!z)return false;
            f.differences[2]=z;f.step=7;
            at(0x231188);callee10[1]=*y;f.temporary10[1]=y;f.step=8;
            at(0x23118c);callee10[2]=*z;f.temporary10[2]=z;f.step=9;
            at(0x231190);callee10[3]=*f.oneV0;f.temporary10[3]=f.oneV0;f.step=10;
            at(0x231194);f.quadV0=callee10;f.step=11;
            at(0x231198);*relativeA3=*f.quadV0;f.local00=*f.quadV0;f.step=12;
            at(0x23119c);f.vf4=*relativeA3;f.step=13;
        }
        {
            auto& f=q.portalDot;const auto& relative=*q.portalRelative.vf4;
            at(0x2311a0);f.nativeAdmissionAttempted=true;
            if(!operand(0u,0u,normal))return false;
            for(unsigned k=0;k<3;++k)f.normalVF5[k]=(*normal)[k];f.step=1; //W stays unknown.
            at(0x2311a4);if(!f.axisNormal())return false;
            std::array<std::uint32_t,3> products;
            for(unsigned k=0;k<3;++k){const auto product=f.unitProduct(relative[k],*f.normalVF5[k]);if(!product)return false;products[k]=*product;}
            for(unsigned k=0;k<3;++k)f.productsVF5[k]=products[k];f.step=2;
            at(0x2311a8);f.oneVF6=0x3f800000u;f.step=3;
            at(0x2311ac);const auto acc=f.zeroSum(products[0],products[1]);if(!acc)return false;f.accX=acc;f.step=4;
            at(0x2311b0);const auto product=f.unitProduct(*f.oneVF6,products[2]);if(!product)return false;
            const auto sum=f.zeroSum(*f.accX,*product);if(!sum)return false;f.sumVF5X=sum;f.step=5;
            at(0x2311b4);f.gprV0[0]=sum;f.gprV0[1]=products[1];f.gprV0[2]=products[2];f.step=6;
            at(0x2311b8);f.scalarF1=f.gprV0[0];f.step=7;
            at(0x2311bc);if(outputA2!=sharedOutput)return false;
            f.outputNonNull=true;f.step=8;at(0x2311c0);f.step=9;
            at(0x2311c4);temporaryV0=&callee3C;f.step=10;
            at(0x2311c8);f.absoluteF8=*f.scalarF1&0x7fffffffu;f.step=11;
            at(0x2311cc);*temporaryV0=*f.absoluteF8;f.temporary3C=f.absoluteF8;f.step=12;
            at(0x2311d0);f.reloadF0=callee3C;f.step=13;
            at(0x2311d4);*outputA2=*f.reloadF0;f.outputDistance=f.reloadF0;f.step=14;
        }
        {
            auto& f=q.portalSlab;const auto distance=*q.portalDot.scalarF1;
            at(0x2311d8);const auto le=f.lessEqualZero(distance);if(!le||inputs.radiusF12!=0u)return false;
            f.firstCondition=f.conditionBit=le;f.prefixStep=1;
            at(0x2311dc);f.prefixStep=2;at(0x2311e0);f.prefixStep=3;
            at(0x2311e4);f.delayV0=Words{};f.prefixStep=4; //Ordinary BC1F always executes POR delay.
            bool toEdges=false;
            if(*le){
                at(0x2311e8);f.negativeRadius=*inputs.radiusF12^0x80000000u;f.negativeStep=1;
                at(0x2311ec);const auto lt=f.lessNegativeZero(distance);if(!lt)return false;
                f.secondCondition=f.conditionBit=lt;f.negativeStep=2;
                at(0x2311f0);f.negativeStep=3;at(0x2311f4);f.negativeStep=4;
                at(0x2311f8);edgeRelativeT0=&callee00;f.negativeStep=5;
                if(!*lt)toEdges=true;
                else {at(0x2311fc);f.secondZeroV0=Words{};}
            }
            if(!toEdges){
                at(0x231200);f.missStep=1;at(0x231204);f.missStep=2;
                at(0x2312f4);f.missStep=3;
                at(0x2312f8);f.returnV0=f.secondZeroV0?f.secondZeroV0:f.delayV0;
                f.returnPC=inputs.callContinuation;f.missStep=4;
                at(0x280c54);if(f.returnV0!=Words{}||f.returnPC!=0x280c54u)return false;
                f.callerBranchTaken=true;f.missStep=5;
                at(0x280c58);f.callerTarget=0x280cf8u;f.missStep=6;
                at(0x280cf8);return q.returned();
            }
        }
        {
            auto& f=q.portalEdge0;
            at(0x23120c);f.edgeOffset=0x80u;f.step=1;
            at(0x231210);if(edgeRelativeT0!=&callee00)return false;
            f.relativeVF4=*edgeRelativeT0;f.step=2;
            at(0x231214);f.nativeAdmissionAttempted=true;
            if(!operand(0u,3u,firstEdge))return false;
            for(unsigned k=0;k<3;++k)f.edgeVF5[k]=(*firstEdge)[k];f.step=3;
            at(0x231218);if(!f.axisEdge())return false;std::array<std::uint32_t,3> products;
            for(unsigned k=0;k<3;++k){const auto product=Dot::unitProduct((*f.relativeVF4)[k],*f.edgeVF5[k]);
                if(!product)return false;products[k]=*product;}
            for(unsigned k=0;k<3;++k)f.productsVF5[k]=products[k];f.step=4;
            at(0x23121c);f.oneVF6=0x3f800000u;f.step=5;
            at(0x231220);const auto acc=Dot::zeroSum(products[0],products[1]);if(!acc)return false;f.accX=acc;f.step=6;
            at(0x231224);const auto product=Dot::unitProduct(*f.oneVF6,products[2]);if(!product)return false;
            const auto sum=Dot::zeroSum(*f.accX,*product);if(!sum)return false;f.sumVF5X=sum;f.step=7;
            at(0x231228);f.gprV0[0]=sum;f.gprV0[1]=products[1];f.gprV0[2]=products[2];f.step=8;
            at(0x23122c);f.scalarF0=f.gprV0[0];f.step=9;
            at(0x231230);f.step=10;at(0x231234);
            const auto le=Slab::lessEqualZero(*f.scalarF0);if(!le||inputs.radiusF12!=0u)return false;
            f.conditionBit=le;f.step=11;at(0x231238);f.step=12;at(0x23123c);f.step=13;
            at(0x231240);f.counterA2=1u;outputA2=nullptr;f.step=14;
            if(*le){
                at(0x231250);f.advanceStep=1;
                at(0x231254);f.oneV1=0x3f800000u;f.advanceStep=2;
                at(0x2312e0);f.loopTestV0=1u;f.advanceStep=3;
                at(0x2312e4);f.advanceStep=4;
                at(0x2312e8);f.loopOffsetV0=16u;f.target=0x23125cu;f.advanceStep=5;
            }else{
                at(0x231244);f.missStep=1;
                at(0x231248);f.zeroV0=Words{};f.missStep=2;
                at(0x2312f0);f.missStep=3;at(0x2312f4);f.missStep=4;
                at(0x2312f8);f.returnV0=f.zeroV0;f.returnPC=inputs.callContinuation;f.missStep=5;
                at(0x280c54);if(f.returnV0!=Words{}||f.returnPC!=0x280c54u)return false;
                f.callerBranchTaken=true;f.missStep=6;
                at(0x280c58);f.target=0x280cf8u;f.missStep=7;
                at(0x280cf8);return q.returned();
            }
        }
        auto& f=q.portalLoop;const auto& edge0=q.portalEdge0;
        const typename Loop::Iteration* terminal=nullptr;
        for(unsigned n=0;n<3;++n){auto& it=f.iterations[n];const unsigned index=n+1;
            const auto counter=n?f.iterations[n-1].counterOut:edge0.counterA2;
            const auto offset=n?f.iterations[n-1].loopOffsetV0:edge0.loopOffsetV0;
            at(0x23125c);if(!counter||*counter!=index||!offset||*offset!=16u*index||
                (n&&(f.iterations[n-1].advanceStep!=3||f.iterations[n-1].target!=0x23125cu)))return false;
            it.addressAttempted=true;const Words* edge=nullptr;
            if(!operand(index,1u,edge))return false;
            loopVertices[n]=&(*vertices)[index];it.counterIn=counter;it.vertexOffset=offset;it.step=1;
            at(0x231260);if(center!=actualCenter)return false;
            it.centerWords[0]=std::bit_cast<std::uint32_t>(center->v[0]);it.step=2;
            at(0x231264);it.vertexWords[0]=(*loopVertices[n])[0];it.step=3;
            at(0x231268);it.vertexWords[1]=(*loopVertices[n])[1];it.step=4;
            at(0x23126c);it.centerWords[1]=std::bit_cast<std::uint32_t>(center->v[1]);it.step=5;
            at(0x231270);it.centerWords[2]=std::bit_cast<std::uint32_t>(center->v[2]);it.step=6;
            at(0x231274);it.vertexWords[2]=(*loopVertices[n])[2];it.step=7;
            at(0x231278);it.edgeAddressAttempted=true;
            if(!operand(index,8u,edge))return false;
            loopEdges[n]=edge;relativeA3=nullptr;it.edgeOffset=0x80u+16u*index;it.step=8;
            at(0x23127c);auto result=Relative::difference(*it.centerWords[0],*it.vertexWords[0]);
            if(!result)return false;it.differences[0]=result;it.step=9;
            at(0x231280);result=Relative::difference(*it.centerWords[1],*it.vertexWords[1]);
            if(!result)return false;it.differences[1]=result;it.step=10;
            at(0x231284);callee20[0]=*it.differences[0];it.temporary20[0]=it.differences[0];it.step=11;
            at(0x231288);result=Relative::difference(*it.centerWords[2],*it.vertexWords[2]);
            if(!result)return false;it.differences[2]=result;it.step=12;
            at(0x23128c);callee20[1]=*it.differences[1];it.temporary20[1]=it.differences[1];it.step=13;
            at(0x231290);callee20[2]=*it.differences[2];it.temporary20[2]=it.differences[2];it.step=14;
            at(0x231294);if(edge0.oneV1!=0x3f800000u)return false;
            callee20[3]=static_cast<std::uint32_t>(*edge0.oneV1);it.temporary20[3]=callee20[3];it.step=15;
            at(0x231298);it.quadV0=callee20;it.step=16;
            at(0x23129c);callee00=*it.quadV0;it.local00=callee00;it.step=17;
            at(0x2312a0);if(edgeRelativeT0!=&callee00)return false;
            it.relativeVF4=*edgeRelativeT0;it.step=18;
            at(0x2312a4);it.edgeLoadAttempted=true;
            if(!operand(index,19u,edge))return false;
            if(edge!=loopEdges[n])return false;
            for(unsigned k=0;k<3;++k)it.edgeVF5[k]=(*edge)[k];it.step=19;
            at(0x2312a8);if(!it.axisEdge())return false;std::array<std::uint32_t,3> products;
            for(unsigned k=0;k<3;++k){const auto p=Dot::unitProduct((*it.relativeVF4)[k],*it.edgeVF5[k]);
                if(!p)return false;products[k]=*p;}
            for(unsigned k=0;k<3;++k)it.productsVF5[k]=products[k];it.step=20;
            at(0x2312ac);it.oneVF6=0x3f800000u;it.step=21;
            at(0x2312b0);const auto acc=Dot::zeroSum(products[0],products[1]);if(!acc)return false;it.accX=acc;it.step=22;
            at(0x2312b4);const auto p=Dot::unitProduct(*it.oneVF6,products[2]);if(!p)return false;
            const auto sum=Dot::zeroSum(*it.accX,*p);if(!sum)return false;it.sumVF5X=sum;it.step=23;
            at(0x2312b8);it.gprV0[0]=sum;it.gprV0[1]=products[1];it.gprV0[2]=products[2];it.step=24;
            at(0x2312bc);it.scalarF0=it.gprV0[0];it.step=25;at(0x2312c0);it.step=26;
            at(0x2312c4);const auto le=Slab::lessEqualZero(*it.scalarF0);
            if(!le||inputs.radiusF12!=0u)return false;it.conditionBit=le;it.step=27;
            at(0x2312c8);it.step=28;at(0x2312cc);it.step=29;
            if(!*le){ //BC1TL annuls2312D0; no counterOut is established.
                at(0x2312d4);it.missStep=1;at(0x2312d8);it.zeroV0=Words{};
                it.target=0x2312f0u;it.missStep=2;terminal=&it;break;
            }
            at(0x2312d0);it.counterOut=*it.counterIn+1u;it.step=30;
            at(0x2312e0);it.loopTestV0=*it.counterOut<4u?1u:0u;it.advanceStep=1;
            at(0x2312e4);it.loopBranchTaken=*it.loopTestV0!=0u;it.advanceStep=2;
            at(0x2312e8);it.loopOffsetV0=*it.counterOut<<4;it.target=*it.loopBranchTaken?0x23125cu:0x2312ecu;it.advanceStep=3;
            if(!*it.loopBranchTaken){terminal=&it;break;}
        }
        if(!terminal)return false;const bool hit=terminal==&f.iterations[2]&&terminal->advanceStep==3;
        if(hit){at(0x2312ec);f.successLow64=1u;f.successStep=1;} //ADDIU establishes low64 only.
        at(0x2312f0);f.returnStep=1;at(0x2312f4);f.returnStep=2;
        at(0x2312f8);f.returnLow64=hit?1u:0u;f.returnUpper64[0]=hit?terminal->gprV0[2]:std::optional<std::uint32_t>(0u);
        if(!hit)f.returnUpper64[1]=0u;f.returnPC=inputs.callContinuation;f.returnStep=3;
        at(0x280c54);if(f.returnPC!=0x280c54u)return false;f.callerBranchTaken=*f.returnLow64==0u;f.returnStep=4;
        at(0x280c58);f.target=*f.callerBranchTaken?0x280cf8u:0x280c5cu;f.returnStep=5;
        at(*f.target);return q.returned();
    }
};
} //namespace rr::game
