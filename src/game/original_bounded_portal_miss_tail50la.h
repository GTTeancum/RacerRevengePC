#pragma once
#include "original_bounded_portal_query50la.h"
namespace rr::game {
//Complete miss-only traversal of the already pinned original collision chain.
//No new numeric model, source pointers, hit publication, or enclosing return.
//Each stable frame owns its invocation; the caller's SP+AC remains shared.
template<class PhaseOwner,class Center,class Portal>
struct OriginalBoundedPortalMissTail50LA {
    using History=typename PhaseOwner::PortalMissTail;
    using Next=typename PhaseOwner::PortalNext;
    using Query=OriginalBoundedPortalQuery50LA<PhaseOwner,Center,Portal>;
    using Words=std::array<std::uint32_t,4>;
    using Vertices=std::array<Words,4>;
    struct Frame {
        const void* cursor=nullptr;const void* next=nullptr;
        const Portal* portal=nullptr;const Vertices* vertices=nullptr;
        const Center* center=nullptr;std::uint32_t* output=nullptr;
        Query query;
    };
    std::array<Frame,PhaseOwner::portalChainCapacity> frames{};
    const void* seedNext=nullptr;
    bool invoked=false;
    OriginalBoundedPortalMissTail50LA()=default;
    OriginalBoundedPortalMissTail50LA(const OriginalBoundedPortalMissTail50LA&)=delete;
    OriginalBoundedPortalMissTail50LA& operator=(const OriginalBoundedPortalMissTail50LA&)=delete;
    bool aliasesCleared()const noexcept {
        if(seedNext)return false;
        for(const auto& f:frames)if(f.cursor||f.next||f.portal||f.vertices||f.center||f.output||!f.query.aliasesCleared())return false;
        return true;
    }
    void retire()noexcept {
        for(auto& f:frames){f.query.retire();f.cursor=f.next=nullptr;f.portal=nullptr;f.vertices=nullptr;f.center=nullptr;f.output=nullptr;}
        seedNext=nullptr;
    }
    bool current(const History& h,const void* firstHead,const void* firstNext,const void* secondCursor,
        const Center* center,const std::uint32_t* output,bool withdrawn)const noexcept {
        if(h.entered!=invoked)return false;
        if(!h.entered)return aliasesCleared();
        if(withdrawn){
            if(!aliasesCleared())return false;
        }else if(h.fromSecond?(h.seedNext.step?h.seedNext.nextNonNull!=bool(seedNext):bool(seedNext)):bool(seedNext))return false;
        const void* previous=h.fromSecond?seedNext:firstNext;
        for(unsigned i=0;i<frames.size();++i){const auto& f=frames[i];const auto& o=h.occurrences[i];
            if(!f.query.current(o.query,o.inputs,f.portal,f.vertices,f.center,output,withdrawn))return false;
            if(withdrawn)continue;
            if(i>=h.count){if(f.cursor||f.next||f.portal||f.vertices||f.center||f.output)return false;continue;}
            if(!f.cursor||f.cursor!=previous||f.cursor==firstHead||(h.fromSecond&&f.cursor==secondCursor))return false;
            for(unsigned j=0;j<i;++j)if(frames[j].cursor==f.cursor)return false;
            if(bool(f.portal)!=(o.inputs.step>=1)||bool(f.vertices)!=(o.inputs.step>=1)||
               (o.inputs.step>=5?f.center!=center:bool(f.center))||
               (o.query.portalEdge0.step>=14?bool(f.output):(o.inputs.step>=2?f.output!=output:bool(f.output)))||
               (o.next.step?o.next.nextNonNull!=bool(f.next):bool(f.next)))return false;
            for(unsigned k=0;k<3;++k){
                if(o.inputs.centerWords[k]&&*o.inputs.centerWords[k]!=std::bit_cast<std::uint32_t>(center->v[k]))return false;
                if(o.inputs.vertexWords[k]&&*o.inputs.vertexWords[k]!=(*f.vertices)[0][k])return false;
            }
            previous=f.next;
        }
        return true;
    }
    bool nextReadCurrent(const History& h,const void* cursor,const void* secondCursor)const noexcept {
        if(h.fromSecond&&!h.count)return cursor==secondCursor&&h.seedNext.nativeAdmissionAttempted&&!h.seedNext.step;
        if(!h.count)return false;const auto& o=h.occurrences[h.count-1];
        return cursor==frames[h.count-1].cursor&&o.query.returned()&&o.query.pc==0x280cf8u&&
            o.next.nativeAdmissionAttempted&&!o.next.step;
    }
    template<class ActualPortal,class ActualNext,class ActualOperand,class Sync>
    bool run(History& h,bool second,const void* firstHead,const void* firstNext,const void* secondCursor,
        const Center* center,std::uint32_t* output,std::uint32_t radius,std::uint32_t returnPC,
        ActualPortal&& actualPortal,ActualNext&& actualNext,ActualOperand&& actualOperand,Sync&& sync,std::uint32_t& pc)noexcept {
        if(invoked||h.entered||!firstHead||!center||!output||radius||returnPC!=0x280bc0u||
           (second?pc!=0x280cf8u||!secondCursor:pc!=0x280c40u||!firstNext))return false;
        invoked=h.entered=true;h.fromSecond=second;
        const auto at=[&](std::uint32_t p)noexcept{pc=h.pc=p;sync(p);};
        const auto advance=[&](Next& n,const void* cursor,const void*& result)noexcept {
            at(0x280cf8);n.nativeAdmissionAttempted=true;
            if(!actualNext(cursor,result,pc)){at(pc);return false;}
            n.nextNonNull=bool(result);n.step=1;
            at(0x280cfc);n.branchTaken=n.nextNonNull;n.step=2;
            at(0x280d00);n.branchTarget=*n.branchTaken?0x280c40u:0x280d04u;n.step=3;
            if(*n.branchTaken){at(0x280c40);return true;}
            at(0x280d04);n.step=4;
            for(std::uint32_t p=0x280d08;p<=0x280d2c;p+=4)at(p);n.step=5;
            at(0x280d30);n.step=6;at(0x280d34);n.returnPC=returnPC;n.step=7;
            at(returnPC);return true;
        };
        const void* cursor=firstNext;
        if(second){if(!advance(h.seedNext,secondCursor,seedNext))return false;
            if(h.seedNext.step==7)return true;cursor=seedNext;}
        while(cursor){
            at(0x280c40);
            //Admission bound follows the complete live ten-portal graph; never infer NULL from exhaustion.
            if(h.count+(second?2u:1u)>=PhaseOwner::portalChainCapacity||cursor==firstHead||(second&&cursor==secondCursor))return false;
            for(unsigned j=0;j<h.count;++j)if(frames[j].cursor==cursor)return false;
            const unsigned i=h.count++;auto& f=frames[i];auto& o=h.occurrences[i];f.cursor=cursor;
            auto& input=o.inputs;input.nativeAdmissionAttempted=true;
            const Portal* portal=nullptr;const Vertices* vertices=nullptr;
            if(!actualPortal(cursor,portal,vertices,pc)){at(pc);return false;}
            if(!portal||!vertices)return false;f.portal=portal;f.vertices=vertices;input.step=1;
            at(0x280c44);f.output=output;input.step=2;
            at(0x280c48);input.radiusF12=radius;input.step=3;
            at(0x280c4c);input.callContinuation=0x280c54u;input.step=4;
            at(0x280c50);f.center=center;input.step=5;
            for(unsigned k=0;k<3;++k){
                at(0x231150u+8*k);input.centerWords[k]=std::bit_cast<std::uint32_t>(center->v[k]);input.step=6+2*k;
                at(0x231154u+8*k);input.vertexWords[k]=(*vertices)[0][k];input.step=7+2*k;
            }
            at(0x231168);input.step=12;at(0x23116c);
            const auto operand=[&](unsigned index,unsigned stage,const Portal*& p,const Vertices*& v,
                const Words*& word,std::uint32_t& reached)noexcept {
                h.pc=reached;sync(reached);return actualOperand(cursor,index,stage,p,v,word,reached);
            };
            const bool returned=f.query.run(o.query,input,f.portal,f.vertices,f.center,f.output,operand,pc);
            at(pc);if(o.query.portalEdge0.step>=14)f.output=nullptr;
            if(!returned||pc!=0x280cf8u)return false; //First new hit is pending280C5C, before its distance reload/store.
            if(!advance(o.next,cursor,f.next))return false;
            if(o.next.step==7)return true;cursor=f.next;
        }
        return false; //No synthetic NULL return.
    }
};
} //namespace rr::game
