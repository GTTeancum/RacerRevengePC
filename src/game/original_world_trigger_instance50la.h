#pragma once
#include "original_collision_base_owner50la.h"
#include "trigger_sphere50in.h"
#include "original_source_collision_observer50la.h"
#include "original_collision_membership_guards50la.h"
#include "original_source_portal_overlap231150.h"
#include "original_bounded_portal_query50la.h"
#include "original_bounded_portal_miss_tail50la.h"
#include <bit>
#include <cfenv>
#include <cmath>
#include <memory>
#include <new>
namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game {
//Pointer-free history of the distinct Fog-derived2A0A80 geometry prefix.
//Narrow VU value proof and11F680 return are distinct from2A0A80/membership readiness.
struct OriginalTriggerFogGeometryPrefix50LA {
    std::uint8_t step=0,matrixWords=0;
    std::optional<std::uint32_t> attachment1A8,shape88,dispatchTarget,radius50;
    std::optional<std::array<std::uint32_t,16>> matrix50;
    std::optional<std::array<std::uint32_t,4>> center00,transformedCenter;
    std::optional<std::uint32_t> resultW;
    std::array<std::optional<std::uint32_t>,3> coordinateProducts;
    //Derived bounded domain, not a general VU multiplier: positive normal
    //with low16 significand bits zero. See v39 evidence and pinned PS2Float.
    static bool unitProductDomain(std::uint32_t bits)noexcept {
        return bits>=0x00800000u&&bits<0x7f800000u&&(bits&0xffffu)==0;
    }
    bool identityMatrix()const noexcept {
        if(!matrix50)return false;
        for(unsigned k=0;k<16;++k)if((*matrix50)[k]!=(k%5u==0?0x3f800000u:0u))return false;
        return true; //Positive-zero identity, including the actual zero translation.
    }
    bool operator==(const OriginalTriggerFogGeometryPrefix50LA&)const=default;
    bool historyCurrent()const noexcept {
        if(step>22||bool(attachment1A8)!=(step>=2)||bool(matrix50)!=(step>=5)||
           bool(shape88)!=(step>=7)||bool(dispatchTarget)!=(step>=8)||
           bool(center00)!=(step>=11)||bool(radius50)!=(step>=12)||
           (step>=4&&attachment1A8!=0u)||(step>=8&&(shape88!=5u||dispatchTarget!=0x11fc80u))||
           (step<9&&matrixWords)||(step==9&&matrixWords>=16)||(step>=10&&matrixWords!=16)||
           bool(resultW)!=(step>=14)||(resultW&&*resultW!=0x3f800000u)||
           bool(transformedCenter)!=(step>=19)||(step>=15&&!identityMatrix()))return false;
        for(unsigned k=0;k<3;++k){const auto& product=coordinateProducts[k];
            if(bool(product)!=(step>=15u+k))return false;
            if(product&&(!center00||!unitProductDomain((*center00)[k])||*product!=(*center00)[k]))return false;
        }
        if(transformedCenter){
            for(unsigned k=0;k<3;++k)if((*transformedCenter)[k]!=*coordinateProducts[k])return false;
            if((*transformedCenter)[3]!=*resultW)return false;
        }
        return true;
    }
};
//Distinct Fog-derived caller and120070 construction/publication history.
//No pointers, registration, bounds, assertion, destructor or return readiness.
struct OriginalTriggerFogMembershipPrefix50LA {
    std::uint8_t step=0; //Caller1..4, stack5..6, flags7, oldE0=8, heap9..10, publication11.
    std::optional<std::uint32_t> mode,stackConstructorPC,heapConstructorPC;
    std::optional<std::uint8_t> flagsF8;
    std::optional<bool> oldE0NonNull,heapNonNull;
    bool operator==(const OriginalTriggerFogMembershipPrefix50LA&)const=default;
    static bool constructorBoundary(std::uint32_t pc)noexcept {
        return pc==0x280890u||pc==0x2808acu||pc==0x2808fcu||pc==0x280918u||pc==0x280920u;
    }
    bool historyCurrent()const noexcept {
        if(step>11||bool(mode)!=(step>=2)||(mode&&mode!=1u)||
           bool(stackConstructorPC)!=(step>=5)||bool(flagsF8)!=(step>=7)||
           oldE0NonNull.has_value()!=(step>=8)||heapNonNull.has_value()!=(step>=9)||
           bool(heapConstructorPC)!=heapNonNull.value_or(false))return false;
        if(stackConstructorPC&&(!constructorBoundary(*stackConstructorPC)||
           (*stackConstructorPC==0x280920u)!=(step>=6)))return false;
        if(step>=8&&(*flagsF8&8u))return false;
        if(step>=9&&*oldE0NonNull)return false;
        if(heapConstructorPC&&(!constructorBoundary(*heapConstructorPC)||
           (*heapConstructorPC==0x280920u)!=(step>=10)))return false;
        return true;
    }
};
//Own1200F0 assertion and bounded model-backed scalar1202E0 shape prefix.
//Shared guard history is immutable occurrence evidence, not a copied process state.
struct OriginalTriggerFogBoundsPrefix50LA {
    std::uint8_t step=0,localVF0Stores=0;
    OriginalCollisionMembershipGuards50LA::History allocation;
    std::optional<bool> selectedNonNull;
    std::optional<std::uint32_t> shape88,dispatchTarget,centerScale5C;
    std::optional<std::array<std::uint32_t,3>> centerXYZ,unitXYZ;
    std::array<std::optional<std::uint32_t>,3> centerProducts,halfProducts;
    std::optional<std::uint32_t> radius50,radiusScale58,radiusProduct;
    std::uint8_t unitStores=0,matrixWords=0;
    std::array<std::uint32_t,4> unitQuad{}; //Only unitStores prefix is source-known.
    std::array<std::uint32_t,16> matrix10{}; //Only matrixWords prefix is source-known.
    std::uint8_t commonStep=0,axisStoredMask=0,commonMatrixLoads=0;
    std::array<std::optional<std::uint32_t>,3> halfReads,commonProducts;
    std::array<std::array<std::uint32_t,4>,3> axisTemps{}; //2F0/300/310; known per word mask.
    std::array<std::uint8_t,3> axisTempMasks{};
    std::array<std::optional<std::array<std::uint32_t,4>>,3> axisRegisters;
    std::array<std::uint32_t,16> commonMatrix{}; //Four words known per actual LQC2.
    std::optional<std::array<std::uint32_t,4>> commonCenter,commonResult;
    std::optional<std::uint32_t> commonW;
    //Own contiguous120CD8..120E64 prefix; no primary-registration readiness.
    std::uint8_t tailStep=0,directionStoredMask=0,halfStoredMask=0;
    struct Direction {
        std::optional<std::array<std::uint32_t,4>> copyRegister,snapshot,input,result;
        std::array<std::uint32_t,12> matrix{};
        std::uint8_t matrixLoads=0;
        std::array<std::optional<std::array<std::uint32_t,3>>,3> fmacXYZ; //ACC after X/Y; VF9.XYZ after Z, not an ACC write.
        std::optional<std::uint32_t> wBefore;
        bool operator==(const Direction&)const=default;
    };
    std::array<Direction,3> directions;
    //Per coordinate, slot order is the actual scalar source Zaxis/Xaxis/Yaxis.
    std::array<std::array<std::optional<std::uint32_t>,3>,3> scalarReads,scalarAbs,scalarTemps,scalarReloads;
    std::array<std::optional<std::uint32_t>,3> scalarInner,scalarOuter;
    std::optional<std::uint32_t> tailReturnPC;
    static constexpr std::uint32_t directionW[4]={0x3f800000u,0x40000000u,0x40400000u,0x40800000u};
    static constexpr std::uint32_t scalarReadPC[3][3]={{0x120d74,0x120d84,0x120d98},{0x120dc0,0x120dcc,0x120de0},{0x120e08,0x120e14,0x120e28}};
    static constexpr std::uint32_t scalarAbsPC[3][3]={{0x120d7c,0x120d90,0x120da4},{0x120dc4,0x120dd8,0x120dec},{0x120e0c,0x120e20,0x120e34}};
    static constexpr std::uint32_t scalarTempPC[3][3]={{0x120d80,0x120d94,0x120da8},{0x120dc8,0x120ddc,0x120df0},{0x120e10,0x120e24,0x120e38}};
    static constexpr std::uint32_t scalarReloadPC[3][3]={{0x120d88,0x120d9c,0x120dac},{0x120dd0,0x120de4,0x120df4},{0x120e18,0x120e2c,0x120e3c}};
    static constexpr std::uint32_t scalarInnerPC[3]={0x120db4,0x120dfc,0x120e40};
    static constexpr std::uint32_t scalarOuterPC[3]={0x120db8,0x120e00,0x120e44};
    static constexpr std::uint32_t halfStorePC[3]={0x120dbc,0x120e04,0x120e48};
    bool tailReached(std::uint32_t pc)const noexcept {return tailStep>=(pc-0x120cd8u)/4u+1u;}
    bool directionDomain(unsigned axis)const noexcept {
        const auto& d=directions[axis];if(!d.input||d.matrixLoads!=3)return false;
        for(unsigned k=0;k<12;++k)if(d.matrix[k]!=(k%5u==0?0x3f800000u:0u))return false;
        for(unsigned k=0;k<3;++k){const auto bits=(*d.input)[k];
            if(k==axis){if(!OriginalTriggerFogGeometryPrefix50LA::unitProductDomain(bits))return false;}
            else if(bits)return false;
        }
        return (*d.input)[3]==0x3f800000u;
    }
    //Only +zero plus +zero or the already-qualified positive-normal domain.
    //Operand order is retained. This is a value lemma, not a general EE ADD.
    static std::optional<std::uint32_t> positiveZeroSum(std::uint32_t fs,std::uint32_t ft)noexcept {
        if(fs==0u&&(ft==0u||OriginalTriggerFogGeometryPrefix50LA::unitProductDomain(ft)))return ft;
        if(ft==0u&&OriginalTriggerFogGeometryPrefix50LA::unitProductDomain(fs))return fs;
        return std::nullopt;
    }
    bool commonIdentity()const noexcept {
        if(commonMatrixLoads!=4)return false;
        for(unsigned k=0;k<16;++k)if(commonMatrix[k]!=(k%5u==0?0x3f800000u:0u))return false;
        return true;
    }
    bool operator==(const OriginalTriggerFogBoundsPrefix50LA&)const=default;
    bool historyCurrent()const noexcept {
        if(step>30||!OriginalCollisionMembershipGuards50LA::allocationHistory(allocation)||
           allocation.entered!=(step>=1)||selectedNonNull.has_value()!=(step>=2)||
           allocation.finishAttempted!=(step>=2)||allocation.returned!=(step>=3)||
           (selectedNonNull&&allocation.nonnull!=*selectedNonNull)||
           (step>=3&&!selectedNonNull.value_or(false))||
           (step<7&&localVF0Stores)||(step==7&&localVF0Stores>=9)||(step>=8&&localVF0Stores!=9)||
           bool(shape88)!=(step>=9)||bool(dispatchTarget)!=(step>=10)||
           (step>=10&&(shape88!=5u||dispatchTarget!=0x1208e4u))||
           bool(centerScale5C)!=(step>=12)||bool(centerXYZ)!=(step>=13)||
           bool(radius50)!=(step>=20)||bool(radiusScale58)!=(step>=20)||bool(radiusProduct)!=(step>=22)||
           (step<20&&unitStores)||(step==20&&unitStores>=4)||(step>=21&&unitStores!=4)||
           bool(unitXYZ)!=(step>=23)||(step<28&&matrixWords)||(step==28&&matrixWords>=16)||(step>=29&&matrixWords!=16))return false;
        constexpr unsigned centerStep[3]={14,15,17},halfStep[3]={24,25,23};
        for(unsigned k=0;k<3;++k){
            if(bool(centerProducts[k])!=(step>=centerStep[k])||bool(halfProducts[k])!=(step>=halfStep[k]))return false;
            if(centerProducts[k]&&(centerScale5C!=0x3f800000u||!centerXYZ||
               !OriginalTriggerFogGeometryPrefix50LA::unitProductDomain((*centerXYZ)[k])||*centerProducts[k]!=(*centerXYZ)[k]))return false;
            if(halfProducts[k]&&(!radiusProduct||*halfProducts[k]!=*radiusProduct))return false;
            if(unitXYZ&&(*unitXYZ)[k]!=0x3f800000u)return false;
        }
        for(unsigned k=0;k<unitStores;++k)if(unitQuad[k]!=0x3f800000u)return false;
        if(radiusProduct&&(radiusScale58!=0x3f800000u||!radius50||
           !OriginalTriggerFogGeometryPrefix50LA::unitProductDomain(*radius50)||*radiusProduct!=*radius50))return false;
        if(commonStep>35||(commonStep&&step!=30)||
           commonMatrixLoads!=(commonStep<25?0u:commonStep<28?commonStep-24u:4u)||
           bool(commonCenter)!=(commonStep>=29)||bool(commonW)!=(commonStep>=30)||
           (commonW&&commonW!=0x3f800000u)||bool(commonResult)!=(commonStep>=35))return false;
        constexpr unsigned readAt[3]={1,3,5},loadAt[3]={16,18,19},storeAt[3]={21,23,24};
        constexpr unsigned tempAt[3][4]={{7,8,11,12},{4,9,13,15},{2,6,14,17}};
        unsigned stored=0;
        for(unsigned axis=0;axis<3;++axis){
            if(bool(halfReads[axis])!=(commonStep>=readAt[axis])||
               (halfReads[axis]&&halfReads[axis]!=halfProducts[axis])||
               bool(axisRegisters[axis])!=(commonStep>=loadAt[axis]))return false;
            unsigned mask=0;for(unsigned lane=0;lane<4;++lane)if(commonStep>=tempAt[axis][lane]){
                mask|=1u<<lane;const auto value=axisTemps[axis][lane];
                if(lane==3){if(value!=0x3f800000u)return false;}
                else if(lane==axis){if(!halfReads[axis]||value!=*halfReads[axis])return false;}
                else if(value)return false;
            }
            if(axisTempMasks[axis]!=mask||(axisRegisters[axis]&&*axisRegisters[axis]!=axisTemps[axis]))return false;
            if(commonStep>=storeAt[axis])stored|=1u<<axis;
            if(bool(commonProducts[axis])!=(commonStep>=31u+axis))return false;
            if(commonProducts[axis]&&(!commonCenter||!commonIdentity()||
               !OriginalTriggerFogGeometryPrefix50LA::unitProductDomain((*commonCenter)[axis])||
               *commonProducts[axis]!=(*commonCenter)[axis]))return false;
        }
        if(axisStoredMask!=stored)return false;
        for(unsigned k=0;k<4u*commonMatrixLoads;++k)if(commonMatrix[k]!=matrix10[k])return false;
        if(commonCenter){for(unsigned k=0;k<3;++k)if((*commonCenter)[k]!=*centerProducts[k])return false;
            if((*commonCenter)[3]!=0x3f800000u)return false;}
        if(commonResult){for(unsigned k=0;k<3;++k)if((*commonResult)[k]!=*commonProducts[k])return false;
            if((*commonResult)[3]!=*commonW)return false;}
        if(tailStep>100||(tailStep&&commonStep!=35)||bool(tailReturnPC)!=(tailStep==100)||
           (tailReturnPC&&tailReturnPC!=0x120170u))return false;
        unsigned directionsStored=0,halvesStored=0;
        for(unsigned axis=0;axis<3;++axis){const auto& d=directions[axis];
            const unsigned start=13u*axis,ds=tailStep<=start?0u:tailStep<start+13u?tailStep-start:13u;
            if(bool(d.copyRegister)!=(ds>=2)||bool(d.snapshot)!=(ds>=4)||bool(d.input)!=(ds>=8)||
               d.matrixLoads!=(ds<5?0u:ds<7?ds-4u:3u)||bool(d.wBefore)!=(ds>=12)||bool(d.result)!=(ds>=12))return false;
            if((d.copyRegister&&(!axisRegisters[axis]||d.copyRegister!=axisRegisters[axis]))||
               (d.snapshot&&d.snapshot!=d.copyRegister)||(d.input&&d.input!=d.snapshot))return false;
            for(unsigned k=0;k<4u*d.matrixLoads;++k)if(d.matrix[k]!=matrix10[k])return false;
            for(unsigned term=0;term<3;++term){
                if(bool(d.fmacXYZ[term])!=(ds>=9u+term))return false;
                if(d.fmacXYZ[term]){if(!directionDomain(axis))return false;
                    for(unsigned lane=0;lane<3;++lane)if((*d.fmacXYZ[term])[lane]!=
                       (lane==axis&&axis<=term?(*d.input)[axis]:0u))return false;}
            }
            if(d.result){
                if(d.wBefore!=directionW[axis]||*d.wBefore!=(axis?(*directions[axis-1].result)[3]:*commonW))return false;
                for(unsigned lane=0;lane<3;++lane)if((*d.result)[lane]!=(*d.fmacXYZ[2])[lane])return false;
                if((*d.result)[3]!=directionW[axis+1])return false;
            }
            if(ds==13)directionsStored|=1u<<axis;
            constexpr unsigned sourceAxis[3]={2,0,1};
            for(unsigned slot=0;slot<3;++slot){
                const auto& r=scalarReads[axis][slot];const auto& a=scalarAbs[axis][slot];
                const auto& t=scalarTemps[axis][slot];const auto& reload=scalarReloads[axis][slot];
                if(bool(r)!=tailReached(scalarReadPC[axis][slot])||bool(a)!=tailReached(scalarAbsPC[axis][slot])||
                   bool(t)!=tailReached(scalarTempPC[axis][slot])||bool(reload)!=tailReached(scalarReloadPC[axis][slot]))return false;
                if(r&&(!directions[sourceAxis[slot]].result||*r!=(*directions[sourceAxis[slot]].result)[axis]))return false;
                if((a&&(!r||*a!=(*r&0x7fffffffu)))||(t&&t!=a)||(reload&&reload!=t))return false;
            }
            if(bool(scalarInner[axis])!=tailReached(scalarInnerPC[axis])||
               bool(scalarOuter[axis])!=tailReached(scalarOuterPC[axis]))return false;
            if(scalarInner[axis]&&scalarInner[axis]!=positiveZeroSum(*scalarReloads[axis][1],*scalarReloads[axis][2]))return false;
            if(scalarOuter[axis]&&scalarOuter[axis]!=positiveZeroSum(*scalarReloads[axis][0],*scalarInner[axis]))return false;
            if(tailReached(halfStorePC[axis]))halvesStored|=1u<<axis;
        }
        return directionStoredMask==directionsStored&&halfStoredMask==halvesStored;
    }
};
//Distinct caller/primary history; typed entry custody is separate from guest address bits and registration success.
struct OriginalTriggerFogPrimaryPrefix50LA {
    struct Assertion {
        bool entered=false,ready=false,conditionRead=false,passed=false;
        std::uint8_t guard=0,stores=0;std::int32_t enabled=0;
        std::optional<bool> predicate;
        std::optional<std::uint32_t> returnV0;
        bool operator==(const Assertion&)const=default;
        bool historyCurrent(bool began,bool complete)const noexcept {
            if(entered!=began||passed!=complete||guard>1||stores>3||(stores&&guard)||
               (ready&&!guard&&stores!=3)||(conditionRead&&(!ready||!enabled))||
               predicate.has_value()!=conditionRead||bool(returnV0)!=(complete&&conditionRead))return false;
            if(!entered)return !ready&&!conditionRead&&!passed&&!guard&&!enabled&&!stores&&!predicate&&!returnV0;
            if(complete&&(!ready||(enabled&&!conditionRead)))return false;
            if(predicate&&*predicate!=passed)return false;
            return !returnV0||*returnV0==0u;
        }
    };
    std::uint8_t step=0;
    std::optional<std::uint32_t> radiusF12,mode,callContinuation,lookupContinuation,table08,target20,preferredA3,preferredA1;
    //Native operand fingerprints only; publication.centerV0 separately records actual LQ280BA0.
    std::optional<std::array<std::uint32_t,4>> nativeCenter,nativeHalf;
    std::optional<bool> methodNonNull;
    Assertion empty,method;
    bool nativePinAttempted=false,nativePinAcquired=false;
    struct Selector {
        std::uint8_t step=0; //1..6 shared prefix;7..9 allocation boundary OR10..21 existing-block return.
        std::optional<bool> freeHeadNull,appendBranch,tableNonNull;
        std::optional<std::uint32_t> lastUsed38,perBlock30,blockCount34,newBlockCount,
            freshLastUsed38,tableOffsetBytes,wordOffsetBytes,nativeOrdinal,returnPC;
        bool blockRead=false,entryAliasResolved=false,s4Retained=false;
        bool operator==(const Selector&)const=default;
        bool historyCurrent()const noexcept {
            if(step>21||freeHeadNull.has_value()!=(step>=3)||(freeHeadNull&&!*freeHeadNull)||
               bool(lastUsed38)!=(step>=4)||bool(perBlock30)!=(step>=5)||
               appendBranch.has_value()!=(step>=6)||(step>=6&&perBlock30!=1024u)||
               (appendBranch&&*appendBranch!=(*lastUsed38!=*perBlock30)))return false;
            const bool append=appendBranch.value_or(false);
            if((step>=7&&step<=9&&append)||(step>=10&&!append))return false;
            if(bool(blockCount34)!=(append?step>=10:step>=7)||
               tableNonNull.has_value()!=(append?step>=11:step>=8)||
               bool(newBlockCount)!=(!append&&step>=9)||bool(freshLastUsed38)!=(step>=12)||
               bool(tableOffsetBytes)!=(step>=13)||bool(wordOffsetBytes)!=(step>=15)||
               blockRead!=(step>=16)||entryAliasResolved!=(step>=17)||bool(nativeOrdinal)!=(step>=17)||
               bool(returnPC)!=(step>=20)||s4Retained!=(step>=21))return false;
            if(lastUsed38&&*lastUsed38>1024u)return false;
            if(blockCount34&&(*blockCount34>1u||(append&&*blockCount34!=1u)))return false;
            if(newBlockCount&&*newBlockCount!=*blockCount34+1u)return false;
            if(append&&step>=11&&tableNonNull!=true)return false;
            if(freshLastUsed38&&(freshLastUsed38!=lastUsed38||*freshLastUsed38>=1024u))return false;
            if((tableOffsetBytes&&*tableOffsetBytes!=4u* *blockCount34)||
               (wordOffsetBytes&&*wordOffsetBytes!=4u* *freshLastUsed38)||
               (nativeOrdinal&&*nativeOrdinal!=*freshLastUsed38+1u)||(returnPC&&returnPC!=0x280b04u))return false;
            return true; //No raw guest address is stored in this pointer-free history.
        }
    } selector;
    struct EntryCall {
        std::uint8_t step=0; //1 assertion attempt,2 ready,3 passed,4..11 original insertion-call setup.
        std::optional<bool> selectedNonNull; //Native typed-address witness, not a guest DWORD or source predicate read.
        Assertion slot;
        std::optional<std::uint32_t> table08,target24,callContinuation;
        bool operator==(const EntryCall&)const=default;
        bool historyCurrent()const noexcept {
            if(step>11||selectedNonNull.has_value()!=(step>=1)||(selectedNonNull&&!*selectedNonNull)||
               !slot.historyCurrent(step>=1,step>=3)||(step>=2&&!slot.ready)||
               (slot.predicate&&slot.predicate!=selectedNonNull)||
               bool(table08)!=(step>=4)||(table08&&table08!=0x3d2d30u)||
               bool(target24)!=(step>=9)||(target24&&target24!=0x27e570u)||
               bool(callContinuation)!=(step>=10)||(callContinuation&&callContinuation!=0x280b90u))return false;
            return true; //Continuation is caller RA, never a claimed insertion return.
        }
    } entryCall;
    struct Insertion {
        std::uint8_t step=0; //Free-head load,branch,old-count load,add,count store,JR,member store in delay.
        std::optional<std::uint32_t> freeHead40,lastUsed38,incremented38,returnPC;
        bool counterStored=false,memberStored=false;
        bool operator==(const Insertion&)const=default;
        bool historyCurrent()const noexcept {
            if(step>7||bool(freeHead40)!=(step>=1)||(freeHead40&&*freeHead40)||
               bool(lastUsed38)!=(step>=3)||(lastUsed38&&*lastUsed38>=1024u)||
               bool(incremented38)!=(step>=4)||(incremented38&&*incremented38!=*lastUsed38+1u)||
               counterStored!=(step>=5)||memberStored!=(step>=7)||
               bool(returnPC)!=(step>=7)||(returnPC&&returnPC!=0x280b90u))return false;
            return true; //No V0 result exists in27E570; member pointer bits stay unknown.
        }
    } insertion;
    struct Publication {
        std::uint8_t step=0,stores=0; //Twelve original instructions280B90..280BBC.
        std::optional<std::array<std::uint32_t,4>> centerV0;
        std::optional<std::uint32_t> radiusF12,callContinuation;
        bool operator==(const Publication&)const=default;
        bool historyCurrent()const noexcept {
            const unsigned expected=(step>=1?1u:0u)|(step>=2?2u:0u)|(step>=3?4u:0u)|
                (step>=4?8u:0u)|(step>=12?16u:0u);
            return step<=12&&stores==expected&&bool(centerV0)==(step>=5)&&
                bool(radiusF12)==(step>=8)&&(!radiusF12||!*radiusF12)&&
                bool(callContinuation)==(step>=11)&&(!callContinuation||callContinuation==0x280bc0u);
        }
    } publication;
    struct PortalHead {
        std::uint8_t step=0; //Prologue,zone,method,three retained operands,F20,head,branch,restores,JR,delay.
        std::optional<std::uint32_t> radiusF20,returnPC;
        std::optional<bool> headNull;
        bool nativePinAttempted=false,nativePinAcquired=false;
        bool operator==(const PortalHead&)const=default;
        bool historyCurrent()const noexcept {
            return step<=12&&bool(radiusF20)==(step>=7)&&(!radiusF20||!*radiusF20)&&
                headNull.has_value()==(step>=8)&&(!nativePinAttempted||step>=7)&&
                (!nativePinAcquired||nativePinAttempted)&&(step<8||nativePinAcquired)&&
                (step<10||headNull==true)&&bool(returnPC)==(step>=12)&&(!returnPC||returnPC==0x280bc0u);
        }
    } portalHead;
    struct PortalInputs {
        std::uint8_t step=0; //Edge08, output address, F12, JAL, delay, six LWC1 loads, stack adjustment.
        bool nativeAdmissionAttempted=false;
        std::optional<std::uint32_t> radiusF12,callContinuation;
        std::array<std::optional<std::uint32_t>,3> centerWords,vertexWords;
        bool operator==(const PortalInputs&)const=default;
        bool helperEntered()const noexcept{return step==12;}
        bool historyCurrent()const noexcept {
            if(step>12||(step&&!nativeAdmissionAttempted)||bool(radiusF12)!=(step>=3)||
               (radiusF12&&*radiusF12!=0u)||bool(callContinuation)!=(step>=4)||
               (callContinuation&&*callContinuation!=0x280c54u))return false;
            for(unsigned k=0;k<3;++k)if(bool(centerWords[k])!=(step>=6+2*k)||
                bool(vertexWords[k])!=(step>=7+2*k))return false;
            return true;
        }
    } portalInputs;
    struct PortalRelative {
        using Words=std::array<std::uint32_t,4>;
        std::uint8_t step=0; //Contiguous original23116C..23119C, stopping before normal LQC2.
        std::array<std::optional<std::uint32_t>,3> differences;
        std::optional<std::uint32_t> oneV0,normalOffset;
        std::array<std::optional<std::uint32_t>,4> temporary10;
        std::optional<Words> quadV0,local00,vf4;
        //Value-only inference from the pinned PS2Float Sub/DoAdd intended
        //signed model. No portable-source execution or EE hardware claim.
        static std::optional<std::uint32_t> difference(std::uint32_t fs,std::uint32_t ft)noexcept {
            if(!OriginalTriggerFogGeometryPrefix50LA::unitProductDomain(fs))return std::nullopt;
            if((ft&0x7fffffffu)==0u)return fs; //Both signed zero operands; source sign is retained in the input receipt.
            const auto exponent=(fs>>23)&255u;
            if(!OriginalTriggerFogGeometryPrefix50LA::unitProductDomain(ft)||exponent<8u||
               ((fs^ft)&0x7f800000u))return std::nullopt;
            const auto delta=static_cast<std::int32_t>((fs&0x7fffffu)>>16)-static_cast<std::int32_t>((ft&0x7fffffu)>>16);
            if(!delta)return 0u; //Pinned exact cancellation is positive zero.
            const auto magnitude=static_cast<std::uint32_t>(delta<0?-delta:delta); //1..127, no signed overflow.
            unsigned q=0;for(auto v=magnitude;v>1u;v>>=1u)++q;
            const auto resultExponent=exponent+q-7u; //1..253; no subnormal, underflow or exponent255 result.
            return (delta<0?0x80000000u:0u)|(resultExponent<<23)|((magnitude<<(23u-q))&0x7fffffu);
        }
        bool operator==(const PortalRelative&)const=default;
        template<class Inputs>
        bool historyCurrent(const Inputs& inputs)const noexcept {
            if(step>13||(step&&!inputs.helperEntered())||bool(oneV0)!=(step>=2)||(oneV0&&*oneV0!=0x3f800000u)||
               bool(normalOffset)!=(step>=6)||(normalOffset&&*normalOffset!=0x70u)||
               bool(quadV0)!=(step>=11)||bool(local00)!=(step>=12)||bool(vf4)!=(step>=13))return false;
            constexpr unsigned resultStep[3]={1,4,7},storeStep[4]={5,8,9,10};
            for(unsigned k=0;k<3;++k){
                if(bool(differences[k])!=(step>=resultStep[k]))return false;
                if(differences[k]&&(!inputs.centerWords[k]||!inputs.vertexWords[k]||
                   differences[k]!=difference(*inputs.centerWords[k],*inputs.vertexWords[k])))return false;
            }
            for(unsigned k=0;k<4;++k){
                if(bool(temporary10[k])!=(step>=storeStep[k]))return false;
                if(temporary10[k]&&temporary10[k]!=(k<3?differences[k]:oneV0))return false;
            }
            if(quadV0)for(unsigned k=0;k<4;++k)if((*quadV0)[k]!=*temporary10[k])return false;
            return (!local00||local00==quadV0)&&(!vf4||vf4==local00);
        }
    } portalRelative;
    struct PortalDot {
        std::uint8_t step=0;
        bool nativeAdmissionAttempted=false;
        //Unknown W remains absent across LQC2, masked VMUL and full QMFC2.
        std::array<std::optional<std::uint32_t>,4> normalVF5,productsVF5,gprV0;
        std::optional<std::uint32_t> oneVF6,accX,sumVF5X,scalarF1,absoluteF8,temporary3C,reloadF0,outputDistance;
        std::optional<bool> outputNonNull;
        bool operator==(const PortalDot&)const=default;
        static bool signedDomain(std::uint32_t w)noexcept {
            return !(w&0x7fffffffu)||OriginalTriggerFogGeometryPrefix50LA::unitProductDomain(w&0x7fffffffu);
        }
        static std::optional<std::uint32_t> unitProduct(std::uint32_t fs,std::uint32_t ft)noexcept {
            if(!signedDomain(fs)||!signedDomain(ft))return std::nullopt;
            const auto a=fs&0x7fffffffu,b=ft&0x7fffffffu,sign=(fs^ft)&0x80000000u;
            if(!a||!b)return sign;
            if(a==0x3f800000u)return b|sign;
            if(b==0x3f800000u)return a|sign;
            return std::nullopt; //No general VU multiplier.
        }
        static std::optional<std::uint32_t> zeroSum(std::uint32_t fs,std::uint32_t ft)noexcept {
            if(!signedDomain(fs)||!signedDomain(ft))return std::nullopt;
            const auto a=fs&0x7fffffffu,b=ft&0x7fffffffu;
            if(!a&&!b)return (fs&ft)&0x80000000u;
            if(!a)return ft;if(!b)return fs;
            return std::nullopt; //At most one nonzero term in this axis-normal domain.
        }
        bool axisNormal()const noexcept {
            unsigned units=0;
            for(unsigned k=0;k<3;++k){if(!normalVF5[k])return false;
                const auto magnitude=*normalVF5[k]&0x7fffffffu;
                if(magnitude==0x3f800000u)++units;else if(magnitude)return false;}
            return units==1&&!normalVF5[3];
        }
        bool historyCurrent(const PortalRelative& relative)const noexcept {
            if(step>14||(step&&!nativeAdmissionAttempted)||(nativeAdmissionAttempted&&relative.step!=13)||
               normalVF5[3]||productsVF5[3]||gprV0[3]||bool(oneVF6)!=(step>=3)||
               (oneVF6&&*oneVF6!=0x3f800000u)||bool(accX)!=(step>=4)||bool(sumVF5X)!=(step>=5)||
               bool(scalarF1)!=(step>=7)||outputNonNull.has_value()!=(step>=8)||
               (outputNonNull&&!*outputNonNull)||bool(absoluteF8)!=(step>=11)||
               bool(temporary3C)!=(step>=12)||bool(reloadF0)!=(step>=13)||bool(outputDistance)!=(step>=14))return false;
            for(unsigned k=0;k<3;++k){
                if(bool(normalVF5[k])!=(step>=1)||bool(productsVF5[k])!=(step>=2)||bool(gprV0[k])!=(step>=6))return false;
                if(productsVF5[k]&&(!relative.vf4||productsVF5[k]!=unitProduct((*relative.vf4)[k],*normalVF5[k])))return false;
                if(gprV0[k]&&gprV0[k]!=(k?productsVF5[k]:sumVF5X))return false;
            }
            if(step>=2&&!axisNormal())return false;
            if(accX&&accX!=zeroSum(*productsVF5[0],*productsVF5[1]))return false;
            if(sumVF5X){const auto product=unitProduct(*oneVF6,*productsVF5[2]);
                if(!product||sumVF5X!=zeroSum(*accX,*product))return false;}
            return (!scalarF1||scalarF1==gprV0[0])&&(!absoluteF8||*absoluteF8==(*scalarF1&0x7fffffffu))&&
                (!temporary3C||temporary3C==absoluteF8)&&(!reloadF0||reloadF0==temporary3C)&&
                (!outputDistance||outputDistance==reloadF0);
        }
    } portalDot;
    struct PortalSlab {
        using Words=std::array<std::uint32_t,4>;
        std::uint8_t prefixStep=0,negativeStep=0,missStep=0;
        std::optional<bool> firstCondition,secondCondition,conditionBit,callerBranchTaken;
        std::optional<Words> delayV0,secondZeroV0,returnV0; //POR establishes all128 bits, including the caller BEQ's whole operand.
        std::optional<std::uint32_t> negativeRadius,returnPC,callerTarget;
        bool operator==(const PortalSlab&)const=default;
        static std::optional<bool> lessEqualZero(std::uint32_t fs)noexcept {
            if(!PortalDot::signedDomain(fs))return std::nullopt;
            return !(fs&0x7fffffffu)||bool(fs&0x80000000u);
        }
        static std::optional<bool> lessNegativeZero(std::uint32_t fs)noexcept {
            if(!PortalDot::signedDomain(fs))return std::nullopt;
            return bool(fs&0x80000000u)&&bool(fs&0x7fffffffu);
        }
        template<class Inputs>
        bool historyCurrent(const PortalDot& dot,const Inputs& inputs)const noexcept {
            if(prefixStep>4||negativeStep>5||missStep>6||(prefixStep&&dot.step!=14)||
               firstCondition.has_value()!=(prefixStep>=1)||conditionBit.has_value()!=(prefixStep>=1)||
               bool(delayV0)!=(prefixStep>=4)||(delayV0&&*delayV0!=Words{})||
               bool(negativeRadius)!=(negativeStep>=1)||(negativeRadius&&*negativeRadius!=0x80000000u)||
               secondCondition.has_value()!=(negativeStep>=2)||bool(returnV0)!=(missStep>=4)||
               (returnV0&&*returnV0!=Words{})||bool(returnPC)!=(missStep>=4)||
               (returnPC&&returnPC!=inputs.callContinuation)||callerBranchTaken.has_value()!=(missStep>=5)||
               (callerBranchTaken&&!*callerBranchTaken)||bool(callerTarget)!=(missStep>=6)||
               (callerTarget&&*callerTarget!=0x280cf8u))return false;
            if(firstCondition&&(!dot.scalarF1||inputs.radiusF12!=0u||firstCondition!=lessEqualZero(*dot.scalarF1)))return false;
            if(negativeStep&&(prefixStep!=4||firstCondition!=true))return false;
            if(secondCondition&&(!dot.scalarF1||secondCondition!=lessNegativeZero(*dot.scalarF1)))return false;
            if(conditionBit!=(negativeStep>=2?secondCondition:firstCondition))return false;
            const bool secondMiss=negativeStep==5&&secondCondition==true;
            if(secondZeroV0&&(!secondMiss||*secondZeroV0!=Words{}))return false;
            if(missStep){
                if(prefixStep!=4)return false;
                if(firstCondition==false){if(negativeStep||secondZeroV0)return false;}
                else if(!secondMiss||!secondZeroV0)return false;
            }
            return true;
        }
    } portalSlab;
    struct PortalEdge0 {
        using Words=std::array<std::uint32_t,4>;
        std::uint8_t step=0,advanceStep=0,missStep=0;
        bool nativeAdmissionAttempted=false;
        std::optional<std::uint32_t> edgeOffset,oneVF6,accX,sumVF5X,scalarF0,returnPC,target;
        std::optional<Words> relativeVF4,zeroV0,returnV0;
        std::array<std::optional<std::uint32_t>,4> edgeVF5,productsVF5,gprV0;
        std::optional<std::uint64_t> counterA2,oneV1,loopTestV0,loopOffsetV0; //Known low64, not invented high64.
        std::optional<bool> conditionBit,callerBranchTaken;
        bool operator==(const PortalEdge0&)const=default;
        bool axisEdge()const noexcept {
            unsigned units=0;for(unsigned k=0;k<3;++k){if(!edgeVF5[k])return false;
                const auto magnitude=*edgeVF5[k]&0x7fffffffu;
                if(magnitude==0x3f800000u)++units;else if(magnitude)return false;}
            return units==1&&!edgeVF5[3];
        }
        template<class Inputs>
        bool historyCurrent(const PortalSlab& slab,const PortalRelative& relative,const Inputs& inputs)const noexcept {
            if(step>14||advanceStep>5||missStep>7||
               (step&&(slab.prefixStep!=4||slab.negativeStep!=5||slab.secondCondition!=false||slab.missStep))||
               (nativeAdmissionAttempted&&step<2)||(step>=3&&!nativeAdmissionAttempted)||
               bool(edgeOffset)!=(step>=1)||(edgeOffset&&*edgeOffset!=0x80u)||bool(relativeVF4)!=(step>=2)||
               (relativeVF4&&relativeVF4!=relative.local00)||edgeVF5[3]||productsVF5[3]||gprV0[3]||
               bool(oneVF6)!=(step>=5)||(oneVF6&&*oneVF6!=0x3f800000u)||bool(accX)!=(step>=6)||
               bool(sumVF5X)!=(step>=7)||bool(scalarF0)!=(step>=9)||conditionBit.has_value()!=(step>=11)||
               bool(counterA2)!=(step>=14)||(counterA2&&*counterA2!=1u))return false;
            for(unsigned k=0;k<3;++k){
                if(bool(edgeVF5[k])!=(step>=3)||bool(productsVF5[k])!=(step>=4)||bool(gprV0[k])!=(step>=8))return false;
                if(productsVF5[k]&&productsVF5[k]!=PortalDot::unitProduct((*relativeVF4)[k],*edgeVF5[k]))return false;
                if(gprV0[k]&&gprV0[k]!=(k?productsVF5[k]:sumVF5X))return false;
            }
            if(step>=4&&!axisEdge())return false;
            if(accX&&accX!=PortalDot::zeroSum(*productsVF5[0],*productsVF5[1]))return false;
            if(sumVF5X){const auto product=PortalDot::unitProduct(*oneVF6,*productsVF5[2]);
                if(!product||sumVF5X!=PortalDot::zeroSum(*accX,*product))return false;}
            if(scalarF0&&scalarF0!=gprV0[0])return false;
            if(conditionBit&&(inputs.radiusF12!=0u||conditionBit!=PortalSlab::lessEqualZero(*scalarF0)))return false;
            if((advanceStep&&(step!=14||conditionBit!=true||missStep))||
               (missStep&&(step!=14||conditionBit!=false||advanceStep)))return false;
            if(bool(oneV1)!=(advanceStep>=2)||(oneV1&&*oneV1!=0x3f800000u)||
               bool(loopTestV0)!=(advanceStep>=3)||(loopTestV0&&*loopTestV0!=1u)||
               bool(loopOffsetV0)!=(advanceStep>=5)||(loopOffsetV0&&*loopOffsetV0!=16u)||
               bool(zeroV0)!=(missStep>=2)||(zeroV0&&*zeroV0!=Words{})||bool(returnV0)!=(missStep>=5)||
               (returnV0&&returnV0!=zeroV0)||bool(returnPC)!=(missStep>=5)||
               (returnPC&&returnPC!=inputs.callContinuation)||callerBranchTaken.has_value()!=(missStep>=6)||
               (callerBranchTaken&&!*callerBranchTaken)||bool(target)!=(advanceStep>=5||missStep>=7))return false;
            return !target||*target==(advanceStep?0x23125cu:0x280cf8u);
        }
    } portalEdge0;
    struct PortalLoop {
        using Words=std::array<std::uint32_t,4>;
        struct Iteration {
            std::uint8_t step=0,advanceStep=0,missStep=0; //23125C..2312CC; taken2312D0 is step30.
            bool addressAttempted=false,edgeAddressAttempted=false,edgeLoadAttempted=false;
            std::optional<std::uint64_t> counterIn,vertexOffset,counterOut,loopTestV0,loopOffsetV0;
            std::optional<std::uint32_t> edgeOffset,oneVF6,accX,sumVF5X,scalarF0,target;
            std::array<std::optional<std::uint32_t>,3> centerWords,vertexWords,differences;
            std::array<std::optional<std::uint32_t>,4> temporary20,edgeVF5,productsVF5,gprV0;
            std::optional<Words> quadV0,local00,relativeVF4,zeroV0;
            std::optional<bool> conditionBit,loopBranchTaken;
            bool operator==(const Iteration&)const=default;
            bool axisEdge()const noexcept {
                unsigned units=0;for(unsigned k=0;k<3;++k){if(!edgeVF5[k])return false;
                    const auto m=*edgeVF5[k]&0x7fffffffu;if(m==0x3f800000u)++units;else if(m)return false;}
                return units==1&&!edgeVF5[3];
            }
            template<class Inputs>
            bool historyCurrent(unsigned index,const Inputs& inputs,const PortalEdge0& edge0)const noexcept {
                if(index<1||index>3||step>30||advanceStep>3||missStep>2||(step&&!addressAttempted)||
                   (edgeAddressAttempted&&step<7)||(step>=8&&!edgeAddressAttempted)||
                   (edgeLoadAttempted&&step<18)||(step>=19&&!edgeLoadAttempted)||
                   bool(counterIn)!=(step>=1)||(counterIn&&*counterIn!=index)||
                   bool(vertexOffset)!=(step>=1)||(vertexOffset&&*vertexOffset!=16u*index)||
                   bool(edgeOffset)!=(step>=8)||(edgeOffset&&*edgeOffset!=0x80u+16u*index)||
                   bool(quadV0)!=(step>=16)||bool(local00)!=(step>=17)||bool(relativeVF4)!=(step>=18)||
                   bool(oneVF6)!=(step>=21)||(oneVF6&&*oneVF6!=0x3f800000u)||bool(accX)!=(step>=22)||
                   bool(sumVF5X)!=(step>=23)||bool(scalarF0)!=(step>=25)||conditionBit.has_value()!=(step>=27)||
                   edgeVF5[3]||productsVF5[3]||gprV0[3])return false;
                constexpr unsigned centerStep[3]={2,5,6},vertexStep[3]={3,4,7},diffStep[3]={9,10,12},storeStep[4]={11,13,14,15};
                for(unsigned k=0;k<3;++k){
                    if(bool(centerWords[k])!=(step>=centerStep[k])||bool(vertexWords[k])!=(step>=vertexStep[k])||
                       bool(differences[k])!=(step>=diffStep[k])||bool(edgeVF5[k])!=(step>=19)||
                       bool(productsVF5[k])!=(step>=20)||bool(gprV0[k])!=(step>=24))return false;
                    if(centerWords[k]&&centerWords[k]!=inputs.centerWords[k])return false;
                    if(differences[k]&&differences[k]!=PortalRelative::difference(*centerWords[k],*vertexWords[k]))return false;
                    if(productsVF5[k]&&productsVF5[k]!=PortalDot::unitProduct((*relativeVF4)[k],*edgeVF5[k]))return false;
                    if(gprV0[k]&&gprV0[k]!=(k?productsVF5[k]:sumVF5X))return false;
                }
                for(unsigned k=0;k<4;++k){if(bool(temporary20[k])!=(step>=storeStep[k]))return false;
                    if(temporary20[k]&&(k<3?temporary20[k]!=differences[k]:
                       (!edge0.oneV1||*temporary20[k]!=static_cast<std::uint32_t>(*edge0.oneV1))))return false;
                    if(quadV0&&(*quadV0)[k]!=*temporary20[k])return false;}
                if((local00&&local00!=quadV0)||(relativeVF4&&relativeVF4!=local00)||(step>=20&&!axisEdge()))return false;
                if(accX&&accX!=PortalDot::zeroSum(*productsVF5[0],*productsVF5[1]))return false;
                if(sumVF5X){const auto p=PortalDot::unitProduct(*oneVF6,*productsVF5[2]);
                    if(!p||sumVF5X!=PortalDot::zeroSum(*accX,*p))return false;}
                if((scalarF0&&scalarF0!=gprV0[0])||(conditionBit&&
                   (inputs.radiusF12!=0u||conditionBit!=PortalSlab::lessEqualZero(*scalarF0))))return false;
                if(bool(counterOut)!=(step>=30)||(counterOut&&(*counterOut!=index+1u||conditionBit!=true)))return false;
                if((advanceStep&&(step!=30||missStep))||(missStep&&(step!=29||conditionBit!=false||advanceStep)))return false;
                if(bool(loopTestV0)!=(advanceStep>=1)||(loopTestV0&&*loopTestV0!=(index<3?1u:0u))||
                   loopBranchTaken.has_value()!=(advanceStep>=2)||(loopBranchTaken&&*loopBranchTaken!=(index<3))||
                   bool(loopOffsetV0)!=(advanceStep>=3)||(loopOffsetV0&&*loopOffsetV0!=16u*(index+1u))||
                   bool(zeroV0)!=(missStep>=2)||(zeroV0&&*zeroV0!=Words{})||
                   bool(target)!=(advanceStep>=3||missStep>=2))return false;
                return !target||*target==(missStep?0x2312f0u:(index<3?0x23125cu:0x2312ecu));
            }
        };
        std::array<Iteration,3> iterations;
        std::uint8_t successStep=0,returnStep=0;
        std::optional<std::uint64_t> successLow64,returnLow64;
        std::array<std::optional<std::uint32_t>,2> returnUpper64;
        std::optional<std::uint32_t> returnPC,target;
        std::optional<bool> callerBranchTaken;
        bool operator==(const PortalLoop&)const=default;
        template<class Inputs>
        bool historyCurrent(const Inputs& inputs,const PortalEdge0& edge0)const noexcept {
            const Iteration* terminal=nullptr;
            for(unsigned i=0;i<3;++i){const auto& it=iterations[i];if(!it.historyCurrent(i+1,inputs,edge0))return false;
                if(it.addressAttempted){
                    if(edge0.advanceStep!=5||edge0.target!=0x23125cu||edge0.conditionBit!=true||edge0.missStep)return false;
                    if(i&&(iterations[i-1].advanceStep!=3||iterations[i-1].target!=0x23125cu))return false;}
                if(it.missStep==2||(i==2&&it.advanceStep==3))terminal=&it;
            }
            const bool hit=terminal&&terminal==&iterations[2]&&terminal->advanceStep==3;
            if(successStep>1||returnStep>5||(successStep&&!hit)||(returnStep&&(!terminal||(hit&&!successStep)))||
               bool(successLow64)!=(successStep==1)||(successLow64&&*successLow64!=1u)||
               bool(returnLow64)!=(returnStep>=3)||(returnLow64&&*returnLow64!=(hit?1u:0u))||
               bool(returnUpper64[0])!=(returnStep>=3)||bool(returnUpper64[1])!=(returnStep>=3&&!hit)||
               bool(returnPC)!=(returnStep>=3)||(returnPC&&returnPC!=inputs.callContinuation)||
               callerBranchTaken.has_value()!=(returnStep>=4)||(callerBranchTaken&&*callerBranchTaken==hit)||
               bool(target)!=(returnStep>=5))return false;
            if(returnStep>=3&&(returnUpper64[0]!=(hit?terminal->gprV0[2]:std::optional<std::uint32_t>(0u))||
                (!hit&&returnUpper64[1]!=0u)))return false;
            return !target||*target==(hit?0x280c5cu:0x280cf8u);
        }
    } portalLoop;
    struct PortalHit {
        std::uint8_t step=0,storeAttemptMask=0;
        std::optional<std::uint32_t> reloadF0,stored10,stored0C;
        bool operator==(const PortalHit&)const=default;
        bool historyCurrent(const PortalLoop& loop,const PortalDot& dot)const noexcept {
            if(step>3||(storeAttemptMask&~3u)||(step&&(loop.returnStep!=5||loop.target!=0x280c5cu||loop.returnLow64!=1u))||
               ((storeAttemptMask&1u)&&step<1)||((storeAttemptMask&2u)&&step<2)||
               (step>=2&&!(storeAttemptMask&1u))||(step>=3&&storeAttemptMask!=3u)||
               bool(reloadF0)!=(step>=1)||(reloadF0&&reloadF0!=dot.outputDistance)||
               bool(stored10)!=(step>=2)||(stored10&&stored10!=reloadF0)||
               bool(stored0C)!=(step>=3)||(stored0C&&stored0C!=reloadF0))return false;
            return true;
        }
    } portalHit;
    bool portalMissAt280CF8()const noexcept {
        const unsigned sources=(portalSlab.missStep==6&&portalSlab.callerTarget==0x280cf8u?1u:0u)+
            (portalEdge0.missStep==7&&portalEdge0.target==0x280cf8u?1u:0u)+
            (portalLoop.returnStep==5&&portalLoop.target==0x280cf8u&&portalLoop.returnLow64==0u?1u:0u);
        return sources==1u;
    }
    struct PortalNext {
        std::uint8_t step=0; //Next LW,branch,delay,fallthrough NOP,restores,JR,return delay.
        bool nativeAdmissionAttempted=false;
        std::optional<bool> nextNonNull,branchTaken;
        std::optional<std::uint32_t> branchTarget,returnPC;
        bool operator==(const PortalNext&)const=default;
        bool historyCurrent(bool miss,const Publication& pub)const noexcept {
            if(step>7||(nativeAdmissionAttempted&&!miss)||(step&&!nativeAdmissionAttempted)||
               nextNonNull.has_value()!=(step>=1)||branchTaken.has_value()!=(step>=2)||
               (branchTaken&&branchTaken!=nextNonNull)||bool(branchTarget)!=(step>=3)||
               (branchTarget&&*branchTarget!=(nextNonNull==true?0x280c40u:0x280d04u))||
               (step>=4&&nextNonNull!=false)||bool(returnPC)!=(step>=7)||
               (returnPC&&returnPC!=pub.callContinuation))return false;
            return true;
        }
    } portalNext;
    struct PortalNeighbor {
        using Words=std::array<std::uint32_t,4>;
        std::uint8_t step=0;
        bool neighborAdmissionAttempted=false,methodAdmissionAttempted=false,nativePinAttempted=false,nativePinAcquired=false;
        std::optional<std::uint32_t> usage10,lookupContinuation,lookupReturnPC,table08,target20,callContinuation,shimTarget;
        std::optional<std::uint64_t> usageBit1V0,usageBit2V0;
        std::optional<bool> lookupBranchTaken;
        std::optional<Words> preferredA3,preferredA1;
        bool operator==(const PortalNeighbor&)const=default;
        bool historyCurrent(const PortalHit& hit)const noexcept {
            if(step>21||(neighborAdmissionAttempted&&hit.step!=3)||(step&&!neighborAdmissionAttempted)||
               (methodAdmissionAttempted&&step<8)||(step>=9&&!methodAdmissionAttempted)||
               (nativePinAttempted&&step<11)||(nativePinAcquired&&!nativePinAttempted)||(step>=12&&!nativePinAcquired)||
               bool(usage10)!=(step>=2)||(usage10&&*usage10!=1u)||
               bool(lookupContinuation)!=(step>=3)||(lookupContinuation&&*lookupContinuation!=0x280c78u)||
               bool(usageBit1V0)!=(step>=5)||(usageBit1V0&&*usageBit1V0!=(*usage10&1u))||
               lookupBranchTaken.has_value()!=(step>=6)||(lookupBranchTaken&&*lookupBranchTaken)||
               bool(usageBit2V0)!=(step>=7)||(usageBit2V0&&*usageBit2V0!=(*usage10&2u))||
               bool(lookupReturnPC)!=(step>=11)||(lookupReturnPC&&lookupReturnPC!=lookupContinuation)||
               bool(table08)!=(step>=12)||(table08&&*table08!=0x3d2d30u)||
               bool(target20)!=(step>=17)||(target20&&*target20!=0x27e3b0u)||
               bool(callContinuation)!=(step>=18)||(callContinuation&&*callContinuation!=0x280c98u)||
               bool(preferredA3)!=(step>=19)||(preferredA3&&*preferredA3!=Words{})||
               bool(shimTarget)!=(step>=20)||(shimTarget&&*shimTarget!=0x27e3c0u)||
               bool(preferredA1)!=(step>=21)||(preferredA1&&preferredA1!=preferredA3))return false;
            return true; //No selected slot, return value, pending reservation or secondary observer exists yet.
        }
    } portalNeighbor;
    struct PortalSelector {
        std::uint8_t step=0; //Own secondary selection:1..6 prefix;7..9 allocator boundary OR10..23 typed return/caller branch.
        bool nativeReservationAttempted=false,nativeReservationAcquired=false;
        std::optional<bool> selectedNonNull,callerBranchTaken;
        std::optional<std::uint32_t> callerTarget;
        std::optional<bool> freeHeadNull,appendBranch,tableNonNull;
        std::optional<std::uint32_t> lastUsed38,perBlock30,blockCount34,newBlockCount,
            freshLastUsed38,tableOffsetBytes,wordOffsetBytes,nativeOrdinal,returnPC;
        bool blockRead=false,entryAliasResolved=false,s3Retained=false;
        bool operator==(const PortalSelector&)const=default;
        bool historyCurrent()const noexcept {
            if(step>23||(step&&!nativeReservationAcquired)||(nativeReservationAcquired&&!nativeReservationAttempted)||
               selectedNonNull.has_value()!=(step>=21)||(selectedNonNull&&!*selectedNonNull)||
               callerBranchTaken.has_value()!=(step>=22)||(callerBranchTaken&&*callerBranchTaken)||
               bool(callerTarget)!=(step>=23)||(callerTarget&&*callerTarget!=0x280ca4u)||freeHeadNull.has_value()!=(step>=3)||(freeHeadNull&&!*freeHeadNull)||
               bool(lastUsed38)!=(step>=4)||bool(perBlock30)!=(step>=5)||
               appendBranch.has_value()!=(step>=6)||(step>=6&&perBlock30!=1024u)||
               (appendBranch&&*appendBranch!=(*lastUsed38!=*perBlock30)))return false;
            const bool append=appendBranch.value_or(false);
            if((step>=7&&step<=9&&append)||(step>=10&&!append))return false;
            if(bool(blockCount34)!=(append?step>=10:step>=7)||
               tableNonNull.has_value()!=(append?step>=11:step>=8)||
               bool(newBlockCount)!=(!append&&step>=9)||bool(freshLastUsed38)!=(step>=12)||
               bool(tableOffsetBytes)!=(step>=13)||bool(wordOffsetBytes)!=(step>=15)||
               blockRead!=(step>=16)||entryAliasResolved!=(step>=17)||bool(nativeOrdinal)!=(step>=17)||
               bool(returnPC)!=(step>=20)||s3Retained!=(step>=21))return false;
            if(lastUsed38&&*lastUsed38>1024u)return false;
            if(blockCount34&&(*blockCount34>1u||(append&&*blockCount34!=1u)))return false;
            if(newBlockCount&&*newBlockCount!=*blockCount34+1u)return false;
            if(append&&step>=11&&tableNonNull!=true)return false;
            if(freshLastUsed38&&(freshLastUsed38!=lastUsed38||*freshLastUsed38>=1024u))return false;
            if((tableOffsetBytes&&*tableOffsetBytes!=4u* *blockCount34)||
               (wordOffsetBytes&&*wordOffsetBytes!=4u* *freshLastUsed38)||
               (nativeOrdinal&&*nativeOrdinal!=*freshLastUsed38+1u)||(returnPC&&returnPC!=0x280c98u))return false;
            return true; //No raw guest address is stored in this pointer-free history.
        }
    } portalSelector;
    struct PortalEntryCall {
        std::uint8_t step=0; //Eight actual secondary setup instructions280CA4..280CC0; no primary assertion.
        std::optional<std::uint32_t> table08,target24,callContinuation;
        bool operator==(const PortalEntryCall&)const=default;
        bool historyCurrent()const noexcept {
            return step<=8&&bool(table08)==(step>=1)&&(!table08||table08==0x3d2d30u)&&
                bool(target24)==(step>=6)&&(!target24||target24==0x27e570u)&&
                bool(callContinuation)==(step>=7)&&(!callContinuation||callContinuation==0x280cc4u);
        }
    } portalEntryCall;
    struct PortalInsertion {
        std::uint8_t step=0; //Free-head load,branch,old-count load,add,count store,JR,member store in delay.
        std::optional<std::uint32_t> freeHead40,lastUsed38,incremented38,returnPC;
        bool counterStored=false,memberStored=false;
        bool operator==(const PortalInsertion&)const=default;
        bool historyCurrent()const noexcept {
            if(step>7||bool(freeHead40)!=(step>=1)||(freeHead40&&*freeHead40)||
               bool(lastUsed38)!=(step>=3)||(lastUsed38&&*lastUsed38>=1024u)||
               bool(incremented38)!=(step>=4)||(incremented38&&*incremented38!=*lastUsed38+1u)||
               counterStored!=(step>=5)||memberStored!=(step>=7)||
               bool(returnPC)!=(step>=7)||(returnPC&&returnPC!=0x280cc4u))return false;
            return true; //No V0 result exists in27E570; member pointer bits stay unknown.
        }
    } portalInsertion;
    struct PortalPool {
        std::uint8_t callStep=0,bodyStep=0; //Caller3 instructions; body prefix through tail/free/cold branch cuts only.
        bool poolReadAttempted=false;
        std::optional<std::uint32_t> addressHighAT,callContinuation,used20,perBlock10;
        std::optional<bool> less,fullBranch,free2CNull,freeBranch,cached1CNull,cachedBranch;
        bool operator==(const PortalPool&)const=default;
        bool historyCurrent()const noexcept {
            if(callStep>3||bodyStep>12||bool(addressHighAT)!=(callStep>=1)||(addressHighAT&&addressHighAT!=0x00440000u)||
               bool(callContinuation)!=(callStep>=2)||(callContinuation&&callContinuation!=0x280cd0u)||
               (poolReadAttempted&&callStep<2)||(callStep>=3&&!poolReadAttempted)||(bodyStep&&callStep!=3)||
               bool(used20)!=(bodyStep>=2)||(used20&&*used20>169u)||
               bool(perBlock10)!=(bodyStep>=3)||(perBlock10&&*perBlock10!=169u)||
               less.has_value()!=(bodyStep>=4)||(less&&*less!=(*used20<*perBlock10))||
               fullBranch.has_value()!=(bodyStep>=5)||(fullBranch&&*fullBranch==*less)||
               (bodyStep>=7&&less!=false)||free2CNull.has_value()!=(bodyStep>=7)||
               freeBranch.has_value()!=(bodyStep>=8)||(freeBranch&&freeBranch!=free2CNull)||
               (bodyStep>=10&&free2CNull!=true)||cached1CNull.has_value()!=(bodyStep>=10)||
               (cached1CNull&&!*cached1CNull)||cachedBranch.has_value()!=(bodyStep>=11)||
               (cachedBranch&&cachedBranch!=cached1CNull))return false;
            return true; //Read/branch history only; no pool count/store, allocation result or source return.
        }
    } portalPool;
    struct PortalPoolMutation {
        bool admissionAttempted=false,admitted=false,poolStored=false,resultAddressRetained=false;
        std::optional<bool> fromFree,nextNull;
        std::uint8_t step=0;
        std::optional<std::uint32_t> itemSize24,incremented20,productV1,boundaryPC;
        bool operator==(const PortalPoolMutation&)const=default;
        bool historyCurrent(const PortalPool& p)const noexcept {
            if(fromFree.has_value()!=admissionAttempted||(admitted&&!admissionAttempted)||(step&&!admitted))return false;
            const bool free=fromFree.value_or(false);
            if(admissionAttempted&&(p.callStep!=3||(free?(p.bodyStep!=9||p.free2CNull!=false):(p.bodyStep!=6||p.less!=true))))return false;
            if(step>(free?3u:8u)||bool(itemSize24)!=(!free&&step>=1)||(itemSize24&&*itemSize24!=24u)||
               bool(incremented20)!=(!free&&step>=3)||(incremented20&&*incremented20!=*p.used20+1u)||
               bool(productV1)!=(!free&&step>=4)||(productV1&&*productV1!=*p.used20*24u)||
               nextNull.has_value()!=(free&&step>=1)||poolStored!=(step>=(free?3u:6u))||
               resultAddressRetained!=(step>=(free?3u:8u))||bool(boundaryPC)!=resultAddressRetained||
               (boundaryPC&&*boundaryPC!=0x105804u))return false;
            return true; //Typed result identity only. Unknown34 accounting and allocator return remain pending.
        }
    } portalPoolMutation;
    struct PortalPoolReturn {
        bool admissionAttempted=false,admitted=false,input34Unknown=false,sum34Unknown=false,accountingStored=false;
        std::uint8_t step=0;
        std::optional<bool> resultNonNull,branchTaken;
        std::optional<std::uint32_t> itemSize24,returnPC;
        bool operator==(const PortalPoolReturn&)const=default;
        bool historyCurrent(const PortalPool& p,const PortalPoolMutation& m)const noexcept {
            return step<=10&&(!admitted||admissionAttempted)&&(!step||admitted)&&
                (!admissionAttempted||(m.resultAddressRetained&&m.boundaryPC==0x105804u))&&
                resultNonNull.has_value()==(step>=1)&&(!resultNonNull||*resultNonNull)&&
                branchTaken.has_value()==(step>=1)&&(!branchTaken||!*branchTaken)&&
                input34Unknown==(step>=3)&&bool(itemSize24)==(step>=4)&&(!itemSize24||*itemSize24==24u)&&
                sum34Unknown==(step>=5)&&accountingStored==(step>=6)&&bool(returnPC)==(step>=10)&&
                (!returnPC||(returnPC==p.callContinuation&&*returnPC==0x280cd0u));
        }
    } portalPoolReturn;
    struct PortalPoolSlot14 {
        bool storeAttempted=false,stored=false;
        std::optional<std::uint32_t> boundaryPC;
        bool operator==(const PortalPoolSlot14&)const=default;
        bool historyCurrent(const PortalPoolReturn& r)const noexcept {
            return (!storeAttempted||(r.step==10&&r.returnPC==0x280cd0u))&&(!stored||storeAttempted)&&
                bool(boundaryPC)==stored&&(!boundaryPC||*boundaryPC==0x280cd4u);
        }
    } portalPoolSlot14; //Pointer-free reached-store history, not numeric slot14 bits or observer-head publication.
    struct PortalPoolPayload {
        bool entered=false,portalLoadAttempted=false,headLoadAttempted=false;
        std::uint8_t step=0,stores=0;
        std::optional<bool> portalNonNull,oldHeadNull;
        std::optional<std::uint32_t> distanceF0,boundaryPC;
        bool operator==(const PortalPoolPayload&)const=default;
        bool historyCurrent(const PortalPoolSlot14& slot)const noexcept {
            const unsigned expected=step>=9?63u:step>=8?31u:step>=6?15u:step>=4?7u:step>=2?3u:step>=1?1u:0u;
            return step<=9&&(!entered||(slot.stored&&slot.boundaryPC==0x280cd4u))&&(!step||entered)&&stores==expected&&
                (!portalLoadAttempted||(entered&&step>=2))&&(step<3||portalLoadAttempted)&&
                portalNonNull.has_value()==(step>=3)&&(!portalNonNull||*portalNonNull)&&
                bool(distanceF0)==(step>=5)&&(!headLoadAttempted||(entered&&step>=6))&&(step<7||headLoadAttempted)&&
                oldHeadNull.has_value()==(step>=7)&&(!oldHeadNull||*oldHeadNull)&&
                bool(boundaryPC)==(step>=9)&&(!boundaryPC||*boundaryPC==0x280cf8u);
        }
    } portalPoolPayload;
    struct PortalHitNext {
        std::uint8_t step=0;
        bool nativeAdmissionAttempted=false,portalReadAttempted=false,portalReadCompleted=false;
        std::optional<bool> nextNonNull,branchTaken,portalNonNull;
        std::optional<std::uint32_t> branchTarget,returnPC,inputBoundary;
        bool operator==(const PortalHitNext&)const=default;
        bool historyCurrent(const PortalPoolPayload& payload,const Publication& pub)const noexcept {
            return step<=7&&(!nativeAdmissionAttempted||(payload.step==9&&payload.stores==63u&&payload.boundaryPC==0x280cf8u))&&
                (!step||nativeAdmissionAttempted)&&nextNonNull.has_value()==(step>=1)&&branchTaken.has_value()==(step>=2)&&
                (!branchTaken||branchTaken==nextNonNull)&&bool(branchTarget)==(step>=3)&&
                (!branchTarget||*branchTarget==(nextNonNull==true?0x280c40u:0x280d04u))&&
                (step<4||nextNonNull==false)&&bool(returnPC)==(step>=7)&&(!returnPC||returnPC==pub.callContinuation)&&
                (!portalReadAttempted||(step==3&&branchTarget==0x280c40u))&&(!portalReadCompleted||portalReadAttempted)&&
                portalNonNull.has_value()==portalReadCompleted&&(!portalNonNull||*portalNonNull)&&
                bool(inputBoundary)==portalReadCompleted&&(!inputBoundary||*inputBoundary==0x280c44u);
        }
    } portalHitNext; //Distinct published-hit exit and fresh next portal08 read; never reuses first/miss occurrence receipts.
    struct PortalNextInputs {
        std::uint8_t step=0;
        bool nativeAdmissionAttempted=false,nativeInputsAcquired=false;
        std::optional<std::uint32_t> radiusF12,callTarget,callContinuation,stackBytes,boundaryPC;
        std::array<std::optional<std::uint32_t>,3> centerWords,vertexWords;
        bool operator==(const PortalNextInputs&)const=default;
        bool helperEntered()const noexcept{return step==11;}
        bool historyCurrent(const PortalHitNext& next,const PortalHead& head)const noexcept {
            if(step>11||(nativeAdmissionAttempted&&(!next.portalReadCompleted||next.inputBoundary!=0x280c44u))||
               (nativeInputsAcquired&&!nativeAdmissionAttempted)||(step&&!nativeInputsAcquired)||
               bool(radiusF12)!=(step>=2)||(radiusF12&&(radiusF12!=head.radiusF20||*radiusF12))||
               bool(callTarget)!=(step>=3)||(callTarget&&*callTarget!=0x231150u)||
               bool(callContinuation)!=(step>=3)||(callContinuation&&*callContinuation!=0x280c54u)||
               bool(stackBytes)!=(step>=11)||(stackBytes&&*stackBytes!=0x40u)||
               bool(boundaryPC)!=(step>=11)||(boundaryPC&&*boundaryPC!=0x23116cu))return false;
            for(unsigned k=0;k<3;++k)if(bool(centerWords[k])!=(step>=5+2*k)||bool(vertexWords[k])!=(step>=6+2*k))return false;
            return true; //New occurrence's raw loads only; no arithmetic, local/output store, overlap or return receipt.
        }
    } portalNextInputs;
    // One whole, separately owned 231150 activation. The same proven phase
    // types are reused as fresh values; no first-call receipt is copied.
    struct PortalQuery {
        bool entered=false;
        std::uint32_t pc=0;
        PortalRelative portalRelative;
        PortalDot portalDot;
        PortalSlab portalSlab;
        PortalEdge0 portalEdge0;
        PortalLoop portalLoop;
        bool operator==(const PortalQuery&)const=default;
        bool returned()const noexcept {
            return (portalSlab.missStep==6&&portalSlab.callerTarget==0x280cf8u)||
                (portalEdge0.missStep==7&&portalEdge0.target==0x280cf8u)||
                (portalLoop.returnStep==5&&(portalLoop.target==0x280c5cu||portalLoop.target==0x280cf8u));
        }
        template<class Inputs>
        bool historyCurrent(const Inputs& inputs)const noexcept {
            if(!entered)return !pc&&portalRelative==PortalRelative{}&&portalDot==PortalDot{}&&
                portalSlab==PortalSlab{}&&portalEdge0==PortalEdge0{}&&portalLoop==PortalLoop{};
            if(!inputs.helperEntered()||!pc||!portalRelative.historyCurrent(inputs)||
               !portalDot.historyCurrent(portalRelative)||!portalSlab.historyCurrent(portalDot,inputs)||
               !portalEdge0.historyCurrent(portalSlab,portalRelative,inputs)||!portalLoop.historyCurrent(inputs,portalEdge0))return false;
            const unsigned terminal=(portalSlab.missStep==6?1u:0u)+(portalEdge0.missStep==7?1u:0u)+(portalLoop.returnStep==5?1u:0u);
            if(terminal>1u)return false;
            if(terminal)return pc==(portalLoop.returnStep==5?*portalLoop.target:0x280cf8u);
            return (pc>=0x23116cu&&pc<=0x2312f8u&&(pc&3u)==0u)||pc==0x280c54u||pc==0x280c58u;
        }
    } portalNextQuery;
    //Native cohort bound: ten source portals, each with distinct endpoints,
    //contribute at most one collision link to the authenticated original root.
    static constexpr unsigned portalChainCapacity=10u;
    struct PortalMissTail {
        struct Occurrence {
            PortalInputs inputs;
            PortalQuery query;
            PortalNext next;
            bool operator==(const Occurrence&)const=default;
        };
        bool entered=false,fromSecond=false;
        std::uint8_t count=0; //Native retained activations, not a source count store.
        std::uint32_t pc=0;
        PortalNext seedNext; //Only the existing second query's own next00 read.
        std::array<Occurrence,portalChainCapacity> occurrences{};
        bool operator==(const PortalMissTail&)const=default;
        bool historyCurrent(const PortalNext& first,const PortalQuery& second,const Publication& pub,const PortalHead& head)const noexcept {
            if(!entered)return *this==PortalMissTail{};
            if(count+(fromSecond?2u:1u)>portalChainCapacity)return false;
            if(fromSecond){
                if(!second.returned()||second.pc!=0x280cf8u||!seedNext.historyCurrent(true,pub)||
                   (count&&(seedNext.step!=3||seedNext.branchTarget!=0x280c40u)))return false;
            }else if(first.step!=3||first.branchTarget!=0x280c40u||seedNext!=PortalNext{})return false;
            for(unsigned i=0;i<occurrences.size();++i){const auto& o=occurrences[i];
                if(i>=count){if(o!=Occurrence{})return false;continue;}
                if(!o.inputs.nativeAdmissionAttempted||!o.inputs.historyCurrent()||!o.query.historyCurrent(o.inputs)||
                   !o.next.historyCurrent(o.query.returned()&&o.query.pc==0x280cf8u,pub)||
                   (o.inputs.radiusF12&&o.inputs.radiusF12!=head.radiusF20))return false;
                if(i+1<count&&(o.next.step!=3||o.next.branchTarget!=0x280c40u))return false;
            }
            if(!count)return fromSecond&&seedNext.nativeAdmissionAttempted&&
                (seedNext.step==7?pc==seedNext.returnPC:pc==0x280cf8u);
            const auto& last=occurrences[count-1];
            if(last.next.step==7)return pc==last.next.returnPC;
            if(last.next.step==3)return pc==0x280c40u;
            if(last.next.nativeAdmissionAttempted)return !last.next.step&&pc==0x280cf8u;
            if(last.query.entered)return pc==last.query.pc;
            return pc==0x280c40u; //Only portal-owner admission can refuse within the raw input prefix.
        }
    } portalMissTail;
    unsigned portalNullReturns()const noexcept {
        unsigned n=(portalHead.step==12&&portalHead.returnPC==0x280bc0u?1u:0u)+
            (portalNext.step==7&&portalNext.returnPC==0x280bc0u?1u:0u)+
            (portalHitNext.step==7&&portalHitNext.returnPC==0x280bc0u?1u:0u);
        if(portalMissTail.entered){
            if(portalMissTail.seedNext.step==7&&portalMissTail.seedNext.returnPC==0x280bc0u)++n;
            if(portalMissTail.count&&portalMissTail.count<=portalMissTail.occurrences.size()){
                const auto& last=portalMissTail.occurrences[portalMissTail.count-1].next;
                if(last.step==7&&last.returnPC==0x280bc0u)++n;
            }
        }
        return n; //Only actualNULL epilogues, never a hit, capacity refusal or generic portal receipt.
    }
    struct ReturnFlow {
        bool entered=false,zoneRead=false,stackAttempted=false,stackReturned=false;
        std::uint8_t primaryStep=0,callStep=0,membershipStep=0,triggerStep=0;
        std::uint32_t pc=0;
        std::optional<std::uint32_t> primaryReturn,stackTarget,stackContinuation,stackCountBefore,stackCountAfter,membershipReturn,triggerReturn;
        std::optional<bool> zoneNonNull;
        std::optional<std::int32_t> stackArgument;
        OriginalCollisionMembershipGuards50LA::History zoneGuard;
        bool operator==(const ReturnFlow&)const=default;
        bool triggerReturned()const noexcept{return triggerStep==5&&triggerReturn==0x2dc2acu;}
        bool historyCurrent(unsigned portalReturns)const noexcept {
            if(!entered)return *this==ReturnFlow{};
            if(portalReturns!=1u||primaryStep!=11||primaryReturn!=0x120190u||
               !OriginalCollisionMembershipGuards50LA::zoneHistory(zoneGuard)||!zoneGuard.entered||
               zoneNonNull.has_value()!=zoneRead||(zoneRead&&!zoneGuard.processAdmitted)||
               zoneRead!=zoneGuard.finishAttempted||(zoneRead&&zoneNonNull!=zoneGuard.nonnull)||
               callStep>3||(callStep&&!zoneGuard.returned)||bool(stackTarget)!=(callStep>=2)||
               (stackTarget&&*stackTarget!=0x280920u)||bool(stackContinuation)!=(callStep>=2)||
               (stackContinuation&&*stackContinuation!=0x120208u)||bool(stackArgument)!=(callStep>=3)||
               (stackArgument&&*stackArgument!=-1)||stackAttempted!=(callStep==3)||
               (stackReturned&&!stackAttempted)||bool(stackCountBefore)!=stackReturned||bool(stackCountAfter)!=stackReturned||
               (stackCountBefore&&(*stackCountBefore<3u||*stackCountAfter!=*stackCountBefore-1u))||
               membershipStep>7||(membershipStep&&!stackReturned)||bool(membershipReturn)!=(membershipStep==7)||
               (membershipReturn&&*membershipReturn!=0x2a0b1cu)||triggerStep>5||(triggerStep&&membershipStep!=7)||
               bool(triggerReturn)!=(triggerStep==5)||(triggerReturn&&*triggerReturn!=0x2dc2acu))return false;
            if(!zoneGuard.returned)return !callStep&&!stackReturned&&!membershipStep&&!triggerStep&&pc==zoneGuard.pc;
            if(!stackReturned)return callStep==3&&pc==0x280920u;
            return membershipStep==7&&triggerReturned()&&pc==0x2dc2acu;
        }
    } returnFlow;
    bool operator==(const OriginalTriggerFogPrimaryPrefix50LA&)const=default;
    bool historyCurrent()const noexcept {
        for(unsigned k=0;k<3;++k)if(portalInputs.centerWords[k]&&
            (!nativeCenter||*portalInputs.centerWords[k]!=(*nativeCenter)[k]))return false;
        if(step>21||!selector.historyCurrent()||!entryCall.historyCurrent()||!insertion.historyCurrent()||!publication.historyCurrent()||
           !portalHead.historyCurrent()||!portalInputs.historyCurrent()||!portalRelative.historyCurrent(portalInputs)||
           !portalDot.historyCurrent(portalRelative)||!portalSlab.historyCurrent(portalDot,portalInputs)||
           !portalEdge0.historyCurrent(portalSlab,portalRelative,portalInputs)||!portalLoop.historyCurrent(portalInputs,portalEdge0)||
           !portalHit.historyCurrent(portalLoop,portalDot)||!portalNext.historyCurrent(portalMissAt280CF8(),publication)||
           (portalHit.step&&portalNext.nativeAdmissionAttempted)||!portalNeighbor.historyCurrent(portalHit)||
           !portalSelector.historyCurrent()||(portalSelector.nativeReservationAttempted&&portalNeighbor.step!=21)||
           (portalSelector.returnPC&&portalSelector.returnPC!=portalNeighbor.callContinuation)||
           !portalEntryCall.historyCurrent()||!portalInsertion.historyCurrent()||
           (portalEntryCall.step&&(portalSelector.step!=23||portalSelector.callerTarget!=0x280ca4u))||
           (portalInsertion.step&&portalEntryCall.step!=8)||
           (portalInsertion.lastUsed38&&portalInsertion.lastUsed38!=portalSelector.freshLastUsed38)||
           (portalInsertion.incremented38&&portalInsertion.incremented38!=portalSelector.nativeOrdinal)||
           (portalInsertion.returnPC&&portalInsertion.returnPC!=portalEntryCall.callContinuation)||
           !portalPool.historyCurrent()||!portalPoolMutation.historyCurrent(portalPool)||!portalPoolReturn.historyCurrent(portalPool,portalPoolMutation)||
           !portalPoolSlot14.historyCurrent(portalPoolReturn)||!portalPoolPayload.historyCurrent(portalPoolSlot14)||
           !portalHitNext.historyCurrent(portalPoolPayload,publication)||(portalHitNext.nativeAdmissionAttempted&&portalNext.nativeAdmissionAttempted)||
           !portalNextInputs.historyCurrent(portalHitNext,portalHead)||!portalNextQuery.historyCurrent(portalNextInputs)||
           !portalMissTail.historyCurrent(portalNext,portalNextQuery,publication,portalHead)||
           !returnFlow.historyCurrent(portalNullReturns())||
           (portalPoolPayload.distanceF0&&portalPoolPayload.distanceF0!=portalDot.outputDistance)||
           (portalPool.callStep&&(portalInsertion.step!=7||portalInsertion.returnPC!=0x280cc4u))||
           (portalHead.step&&publication.step!=12)||
           (portalInputs.nativeAdmissionAttempted&&(portalHead.step!=9||portalHead.headNull!=false))||
           (portalInputs.radiusF12&&portalInputs.radiusF12!=portalHead.radiusF20)||
           (portalHead.radiusF20&&portalHead.radiusF20!=publication.radiusF12)||
           (portalHead.returnPC&&portalHead.returnPC!=publication.callContinuation)||
           (publication.step&&insertion.step!=7)||(publication.centerV0&&publication.centerV0!=nativeCenter)||
           (publication.radiusF12&&publication.radiusF12!=radiusF12)||
           (selector.step&&step!=21)||(entryCall.step&&selector.step!=21)||(insertion.step&&entryCall.step!=11)||
           (insertion.lastUsed38&&insertion.lastUsed38!=selector.freshLastUsed38)||
           (insertion.incremented38&&insertion.incremented38!=selector.nativeOrdinal)||
           bool(radiusF12)!=(step>=1)||(radiusF12&&*radiusF12)||
           bool(mode)!=(step>=6)||(mode&&mode!=1u)||bool(callContinuation)!=(step>=7)||
           (callContinuation&&callContinuation!=0x120190u)||bool(nativeCenter)!=(step>=8)||bool(nativeHalf)!=(step>=8)||
           methodNonNull.has_value()!=(step>=13)||bool(lookupContinuation)!=(step>=13)||
           (lookupContinuation&&lookupContinuation!=0x280a7cu)||bool(table08)!=(step>=17)||
           (table08&&table08!=0x3d2d30u)||bool(target20)!=(step>=18)||(target20&&target20!=0x27e3b0u)||
           bool(preferredA3)!=(step>=19)||(preferredA3&&*preferredA3)||
           bool(preferredA1)!=(step>=21)||(preferredA1&&preferredA1!=preferredA3)||
           !empty.historyCurrent(step>=10,step>=12)||!method.historyCurrent(step>=14,step>=16)||
           (step>=11&&!empty.ready)||(step>=15&&!method.ready))return false;
        if((empty.predicate&&!*empty.predicate)||(method.predicate&&method.predicate!=methodNonNull)||
           (nativePinAttempted&&(step<13||methodNonNull!=true))||(nativePinAcquired&&!nativePinAttempted)||
           (step>=14&&methodNonNull.value_or(false)&&!nativePinAcquired))return false;
        return step<17||methodNonNull==true;
    }
};
// One actual 2A0290 source object. This is not a ready TriggerActions owner.
// Only the World source descriptor owns it. All source aliases are separate
// from native allocation custody, including allocations not yet published.
class OriginalWorldTriggerInstance50LA final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner; //Same World loader validates retained trigger membership.
    using C=OriginalWreckComponent50LA;
    using B=OriginalCollisionBaseOwner50LA;
    using S=OriginalWreckComponentStatus50LA;
    C entity_;
    C* serviceParent_=nullptr;
    bool serviceRegistrationAttempted_=false,serviceRegistered_=false,serviceTopologyWithdrawn_=false;
    OriginalApplicationEntities50LA* application_=nullptr;
    bool counted_=false,enrolled_=false,colliderEnrolled_=false;
    bool constructed_=false,setupAttempted_=false,setupReturned_=false,nativeClosed_=false;
    std::uint32_t pc_=0,vtable08_=0;
    SpringVector4_48N position30_{},box40_{};
    BodyMatrix48G matrix50_{};
    std::uint8_t flags1B8_=0,flagsKnown_=0;
    std::optional<std::uint32_t> random90_,capacity94_,count98_,field1A8_,field1B4_;
    std::optional<float> radius1AC_;
    std::array<std::optional<char>,260> parentA4_{}; //Unknown until actual2DC068 strcpy.
    bool oneTimeWritten_=false,parentWritten_=false;
    std::unique_ptr<const void*[]> callbacks9C_,argumentsA0_;
    std::optional<const void*> source9C_,sourceA0_;
    std::unique_ptr<B> collider_;
    std::optional<B*> source1B0_; // Constructor does not write 1B0.
    struct Shape {
        TriggerSphereShape50IN value;
        std::uint8_t known=0;
        std::optional<const void*> owner60;
    };
    struct Result {
        std::optional<std::uint32_t> capacity00,count04;
        // Source never writes 08. Native zero backing is not knowledge.
        std::optional<float> distance08;
        SpringVector4_48N center20{};
        std::array<SpringVector4_48N,3> normals30{},points60{};
    };
    std::unique_ptr<Shape> shape_;
    std::unique_ptr<Result> result_;
    bool shapeAttempted_=false,shapeReturned_=false,refreshReturned_=false;
    bool colliderAllocationReturned_=false,shapeAllocationReturned_=false,resultAllocationReturned_=false;
    bool callbacksAllocationReturned_=false,argumentsAllocationReturned_=false;
    // Code-address identity, not a cast host callback or execution receipt.
    std::optional<std::uint32_t> callbackE8PC_;
    // Distinct 2DC2A4 -> 2A0A80 occurrence. No membership, callback,
    // registration or 2A0A80 return is implied by the geometry prefix.
    bool serviceEntered_=false,serviceMatrixCopied_=false,serviceCenterStored_=false;
    bool serviceRadiusStored_=false,serviceGeometryReturned_=false;
    std::optional<std::uint32_t> serviceAttachment1A8_;
    std::optional<BodyMatrix48G> serviceMatrix50_;
    OriginalTriggerFogGeometryPrefix50LA fogGeometry_;
    bool refreshFogService2A0A80(std::uint32_t& pc)noexcept {
        if(pc!=0x2a0a80||fogGeometry_.step||serviceEntered_||!setupReturned_||nativeClosed_||!preflightNative())return false;
        auto& h=fogGeometry_;const auto at=[&](std::uint32_t p){pc=pc_=p;};
        at(0x2a0a80);serviceEntered_=true;
        for(std::uint32_t p=0x2a0a84;p<=0x2a0a90;p+=4)at(p);h.step=1;
        at(0x2a0a94);if(!field1A8_)return false;serviceAttachment1A8_=h.attachment1A8=*field1A8_;h.step=2;
        at(0x2a0a98);at(0x2a0a9c);h.step=3; //BEQ delay retains the authentic collision-node argument in S0.
        if(*h.attachment1A8){at(0x2a0aa0);return false;} //Actual attached-model transform is not joined.
        at(0x2a0b00);if(!source1B0_||!*source1B0_||*source1B0_!=collider_.get())return false;h.step=4;
        at(0x2a0b04);at(0x2a0b08); //JAL11F680; delay passes actual nonNULL this+50, not the setup NULL matrix.
        h.matrix50.emplace();std::memcpy(h.matrix50->data(),matrix50_.v,sizeof matrix50_.v);
        serviceMatrix50_.emplace();std::memcpy(serviceMatrix50_->v,matrix50_.v,sizeof matrix50_.v);h.step=5;
        at(0x11f680);for(std::uint32_t p=0x11f684;p<=0x11f694;p+=4)at(p);h.step=6;
        auto& b=*collider_;at(0x11f698);h.shape88=b.shape88_;h.step=7;
        at(0x11f69c);at(0x11f6a0);if(h.shape88!=5u)return false; //Other dispatch cases retain the actual tag read.
        for(std::uint32_t p=0x11f6a4;p<=0x11f6b8;p+=4)at(p);
        at(0x11f6bc);h.dispatchTarget=0x11fc80u;h.step=8; //LUI39 + signed ADDIU8A00 gives table388A00; slot5=388A14.
        at(0x11f6c0);at(0x11f6c4);at(0x11fc80);at(0x11fc84); //JR/NOP then nonNULL-matrix BNE/NOP.
        at(0x11fcc8);
        if(!shape_||shape_->known!=63||!b.shape8C_||*b.shape8C_!=shape_.get())return false;h.step=9;
        at(0x11fccc);at(0x11fcd0);
        for(unsigned pair=0;pair<8;++pair){
            std::uint32_t words[2];at(0x11fcd4);std::memcpy(words,matrix50_.v+2*pair,4);
            at(0x11fcd8);std::memcpy(words+1,matrix50_.v+2*pair+1,4);at(0x11fcdc);
            at(0x11fce0);std::memcpy(shape_->value.world.v+2*pair,words,4);++h.matrixWords;
            at(0x11fce4);std::memcpy(shape_->value.world.v+2*pair+1,words+1,4);++h.matrixWords;
            at(0x11fce8);at(0x11fcec);at(0x11fcf0); //Pointer updates; BGTZ delay runs even after pair8.
        }
        serviceMatrixCopied_=true;h.step=10;
        at(0x11fcf4);at(0x11fcf8);std::array<std::uint32_t,4> center;
        at(0x11fcfc);std::memcpy(center.data(),shape_->value.localCenter.v,sizeof center);
        at(0x11fd00);std::memcpy(b.cachedCenter60_.v,center.data(),sizeof center);
        h.center00=center;serviceCenterStored_=true;h.step=11;
        at(0x11fd04);std::uint32_t radius;at(0x11fd08);std::memcpy(&radius,&shape_->value.radius50,4);
        at(0x11fd0c);b.radius80_=0.f;std::memcpy(&*b.radius80_,&radius,4);
        h.radius50=radius;serviceRadiusStored_=true;h.step=12;
        for(std::uint32_t p=0x11fd10;p<=0x11fd28;p+=4)at(p);h.step=13;
        at(0x11fd2c);h.resultW=0x3f800000u;h.step=14; //VMULw.w VF9,VF0,VF0: architectural1*1, only W.
        //Pinned integer-multiplier derivation: all eight Booth selectors vanish
        //for low16=0; the unit product has no bit15 correction. This does NOT
        //admit general finite identity arithmetic or claim full VU flags/pipeline emulation.
        constexpr std::uint32_t multiplyPC[3]={0x11fd30,0x11fd34,0x11fd38};
        for(unsigned axis=0;axis<3;++axis){at(multiplyPC[axis]);
            if(!h.identityMatrix()||!OriginalTriggerFogGeometryPrefix50LA::unitProductDomain((*h.center00)[axis]))return false;
            h.coordinateProducts[axis]=(*h.center00)[axis];h.step=static_cast<std::uint8_t>(15u+axis);
            //Actual ACC/VF9 XYZ sequence: (X,+0,+0), (X,Y,+0), (X,Y,Z).
            //All other products and additions are +0 in this exact domain.
        }
        at(0x11fd3c);h.step=18; //VADD.xyz of exact +0 translation, no input-W multiply.
        at(0x11fd40);std::array<std::uint32_t,4> transformed;
        for(unsigned axis=0;axis<3;++axis)transformed[axis]=*h.coordinateProducts[axis];transformed[3]=*h.resultW;
        at(0x11fd44);std::memcpy(b.cachedCenter60_.v,transformed.data(),sizeof transformed);
        h.transformedCenter=transformed;h.step=19; //SQC2 in unconditional-branch delay, including output W=1.
        for(std::uint32_t p=0x11fe28;p<=0x11fe38;p+=4)at(p);h.step=20;
        at(0x11fe3c);h.step=21;at(0x11fe40);h.step=22;serviceGeometryReturned_=true;
        at(0x2a0b0c);return true; //Only11F680 returned.2A0A80/120070 and descriptor2DC2AC remain unjoined.
    }
    bool refreshService2A0A80(std::uint32_t& pc)noexcept {
        if(serviceEntered_||!setupReturned_||!preflightNative())return false;
        serviceEntered_=true;pc=pc_=0x2a0a94;
        if(!field1A8_)return false;serviceAttachment1A8_=*field1A8_;
        pc=pc_=0x2a0a98;
        if(*serviceAttachment1A8_){pc=pc_=0x2a0aa0;return false;} //Actual model branch unjoined.
        pc=pc_=0x2a0b00;
        if(!source1B0_||*source1B0_!=collider_.get()||!collider_)return false;
        pc=pc_=0x2a0b04;
        // 2A0B08 passes this actual, NONNULL retained matrix, unlike setup.
        serviceMatrix50_=matrix50_;pc=pc_=0x11f680;
        auto& b=*collider_;
        if(b.shape88_!=5||!shape_||shape_->known!=63||!b.shape8C_||*b.shape8C_!=shape_.get())return false;
        pc=pc_=0x11fcc8;
        for(unsigned pair=0;pair<8;++pair){
            std::uint32_t words[2];std::memcpy(words,matrix50_.v+2*pair,sizeof words);
            pc=pc_=0x11fce0;std::memcpy(shape_->value.world.v+2*pair,words,4);
            pc=pc_=0x11fce4;std::memcpy(shape_->value.world.v+2*pair+1,words+1,4);
        }
        serviceMatrixCopied_=true;
        pc=pc_=0x11fd00;b.cachedCenter60_=shape_->value.localCenter;serviceCenterStored_=true;
        pc=pc_=0x11fd0c;b.radius80_=shape_->value.radius50;serviceRadiusStored_=true;
        pc=pc_=0x11fd2c;
        // Bounded identity VU adaptation, not a general matrix implementation.
        // All source copies/stores above survive an unsupported transform.
        BodyMatrix48G id;identity(id);
        if(std::memcmp(matrix50_.v,id.v,sizeof id.v))return false;
        for(unsigned k=0;k<3;++k)if(!std::isfinite(b.cachedCenter60_.v[k])||!(b.cachedCenter60_.v[k]>0.f))return false;
        if(std::bit_cast<std::uint32_t>(b.cachedCenter60_.v[3])!=0x3f800000u)return false;
        pc=pc_=0x11fd44; //Exact identity result retains the already copied center.
        pc=pc_=0x11fe40;serviceGeometryReturned_=true;
        pc=pc_=0x2a0b0c;return true; //Geometry only; 120070 mode1 has NOT run.
    }
    using Membership=OriginalSourceCollisionObserver50LA;
    std::unique_ptr<Membership> serviceStackMembership_,serviceHeapMembership_;
    bool membershipEntered_=false,membershipStackReturned_=false,membershipHeapAllocationReturned_=false;
    bool membershipHeapReturned_=false,membershipE0Published_=false,membershipNativeWithdrawn_=false;
    std::optional<std::uint8_t> membershipFlagsF8_;
    bool membershipOldE0NullRead_=false;
    OriginalTriggerFogMembershipPrefix50LA fogMembership_;
    const void* fogMembershipNode_=nullptr; //Actual retained2A0B18 identity; opaque after native withdrawal.
    bool fogMembershipCurrentNative()const noexcept {
        const auto& h=fogMembership_;if(!h.historyCurrent())return false;
        if(!h.step&&!fogGeometry_.step)return true; //Existing generic occurrence stays independent.
        if((h.step&&fogGeometry_.step!=22)||bool(fogMembershipNode_)!=(h.step>=4))return false;
        if(membershipEntered_!=(h.step>=5)||bool(serviceStackMembership_)!=(h.step>=5)||
           membershipStackReturned_!=(h.step>=6)||membershipFlagsF8_!=h.flagsF8||
           membershipOldE0NullRead_!=(h.step>=8&&!h.oldE0NonNull.value_or(true))||
           membershipHeapAllocationReturned_!=(h.step>=9)||bool(serviceHeapMembership_)!=h.heapNonNull.value_or(false)||
           membershipHeapReturned_!=(h.step>=10&&h.heapNonNull.value_or(false))||membershipE0Published_!=(h.step==11))return false;
        //Constructor history remains constructor history after its own genuine destructor.
        if(h.stackConstructorPC&&serviceStackMembership_->ctorPC_!=*h.stackConstructorPC)return false;
        if(h.heapConstructorPC&&serviceHeapMembership_->pc()!=*h.heapConstructorPC)return false;
        //Only own separately mirrored Fog bounds/primary prefixes may follow publication.
        return fogBoundsCurrentNative()&&fogPrimaryCurrentNative()&&!serviceReturn_.entered;
    }
    bool beginFogServiceMembership120070(OriginalGarageObserverShared50KG& shared,
        const void* actualNode,std::uint32_t& pc)noexcept {
        if(pc!=0x2a0b0c||!actualNode||fogGeometry_.step!=22||fogMembership_.step||
           !serviceGeometryReturned_||membershipEntered_||membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=fogMembership_;const auto at=[&](std::uint32_t p){pc=pc_=p;};
        at(0x2a0b0c);if(!source1B0_||!*source1B0_||*source1B0_!=collider_.get())return false;h.step=1;
        at(0x2a0b10);h.mode=1;h.step=2;
        at(0x2a0b14);h.step=3; //Own JAL continuation2A0B1C, not a return receipt.
        at(0x2a0b18);fogMembershipNode_=actualNode;h.step=4; //Actual retained node, not a later substituted owner.
        at(0x120070);
        //Native backing for actual stack+50, not a nullable guest allocation.
        try{serviceStackMembership_.reset(new Membership);}catch(...){return false;}
        membershipEntered_=true;
        for(std::uint32_t p=0x120074;p<=0x120090;p+=4)at(p);
        at(0x120094);at(0x120098);h.step=5;h.stackConstructorPC=0x280890u;
        const bool stack=serviceStackMembership_->construct280890(shared);
        h.stackConstructorPC=serviceStackMembership_->pc();
        if(!stack){at(*h.stackConstructorPC);return false;}
        membershipStackReturned_=true;h.step=6;
        at(0x12009c);membershipFlagsF8_=h.flagsF8=collider_->flagsF8_;h.step=7;
        at(0x1200a0);at(0x1200a4);at(0x1200a8);at(0x1200ac);
        if(*h.flagsF8&8u){at(0x1200b0);return false;} //Before temporary-observer selection/branch delay.
        at(0x1200b8);if(!collider_->membershipE0_)return false;
        h.oldE0NonNull=bool(*collider_->membershipE0_);h.step=8;
        at(0x1200bc);
        if(*h.oldE0NonNull){at(0x1200c4);return false;} //BEQL annulsC0; old-owner destructor is unjoined.
        at(0x1200c0);membershipOldE0NullRead_=true; //Taken delay sets allocation bytes30; skipsC4..CC.
        at(0x1200d0);at(0x1200d4);
        //Actual2E8D90 nullable30-byte allocation, represented by separate native backing.
        try{serviceHeapMembership_.reset(new Membership);}catch(...){serviceHeapMembership_.reset();}
        membershipHeapAllocationReturned_=true;h.heapNonNull=bool(serviceHeapMembership_);h.step=9;
        at(0x1200d8);at(0x1200dc);
        if(serviceHeapMembership_){at(0x1200e0);at(0x1200e4);h.heapConstructorPC=0x280890u;
            const bool heap=serviceHeapMembership_->construct280890(shared);
            h.heapConstructorPC=serviceHeapMembership_->pc();
            if(!heap){at(*h.heapConstructorPC);return false;}
            membershipHeapReturned_=true;
        }
        h.step=10;at(0x1200e8);
        collider_->membershipE0_=serviceHeapMembership_?serviceHeapMembership_->identity():nullptr;
        membershipE0Published_=true;at(0x1200ec);h.step=11;
        at(0x1200f0);return true; //Only E0 publication/selection. No assertion, bounds, registration or120070 return.
    }
    bool beginServiceMembership120070(OriginalGarageObserverShared50KG& shared,std::uint32_t& pc)noexcept {
        if(fogGeometry_.step||!serviceGeometryReturned_||membershipEntered_||!preflightNative())return false;
        pc=pc_=0x120070;
        // Native backing for actual callee stack50, not a guest allocation.
        try{serviceStackMembership_.reset(new Membership);}catch(...){return false;}
        membershipEntered_=true;pc=pc_=0x120094;
        if(!serviceStackMembership_->construct280890(shared)){pc=pc_=serviceStackMembership_->pc();return false;}
        membershipStackReturned_=true;pc=pc_=0x12009c;
        membershipFlagsF8_=collider_->flagsF8_;
        pc=pc_=0x1200a8;
        if(*membershipFlagsF8_&8u){pc=pc_=0x1200b4;return false;} //Temporary-list branch unjoined.
        pc=pc_=0x1200b8;
        if(!collider_->membershipE0_||*collider_->membershipE0_)return false;
        membershipOldE0NullRead_=true;pc=pc_=0x1200d0;
        // Native backing for the original nullable30-byte guest allocation.
        try{serviceHeapMembership_.reset(new Membership);}catch(...){serviceHeapMembership_.reset();}
        membershipHeapAllocationReturned_=true;
        if(serviceHeapMembership_){pc=pc_=0x1200e0;
            if(!serviceHeapMembership_->construct280890(shared)){pc=pc_=serviceHeapMembership_->pc();return false;}
            membershipHeapReturned_=true;
        }
        pc=pc_=0x1200e8;
        collider_->membershipE0_=serviceHeapMembership_?serviceHeapMembership_->identity():nullptr;
        membershipE0Published_=true;pc=pc_=0x1200f0;
        return true; //Only allocation/publication prefix, NOT120070 return.
    }
    bool withdrawServiceMembershipNative()noexcept {
        if(membershipNativeWithdrawn_)return true;
        if(!preflightNative())return false;
        if(!withdrawServicePrimaryNative())return false;
        // Preserve source E0 history/backing until instance disposal. Native
        // withdrawal does not fabricate a source NULL store or280920 return.
        if(serviceHeapMembership_&&(!(serviceHeapMembership_->primary_.entered?
           serviceHeapMembership_->canWithdrawActiveRegistrationNative():serviceHeapMembership_->canWithdrawUnregisteredNative())||
           !serviceHeapMembership_->withdrawNative()))return false;
        if(serviceStackMembership_&&(!serviceStackMembership_->canWithdrawUnregisteredNative()||
           !serviceStackMembership_->withdrawNative()))return false;
        membershipNativeWithdrawn_=true;return true;
    }
    OriginalCollisionMembershipGuards50LA::History membershipAllocationGuard_;
    bool boundsScratchCenter_=false,boundsScratchHalf_=false,boundsEntered_=false,boundsReturned_=false;
    // Native fingerprint only, not a claim that all source loads occurred.
    std::optional<TriggerSphereShape50IN> boundsNativeShape_;
    std::array<std::optional<std::uint32_t>,3> boundsProducts_;
    std::uint8_t boundsLocalCenterStores_=0,boundsLocalHalfStores_=0,boundsHalfStores_=0;
    bool boundsCenterStored_=false;
    std::optional<std::uint32_t> boundsRadiusProduct_;
    std::uint8_t boundsMatrixWords_=0;
    BodyMatrix48G boundsMatrix_{};
    SpringVector4_48N boundsLocalCenter_{{0,0,0,1}},boundsLocalHalf_{{0,0,0,1}};
    SpringVector4_48N boundsCenter_{},boundsHalf_{};
    OriginalTriggerFogBoundsPrefix50LA fogBounds_;
    std::array<std::array<std::uint32_t,4>,9> fogBoundsLocalQuads_{}; //Unknown until each own SQC2.
    bool fogBoundsCurrentNative()const noexcept {
        const auto& h=fogBounds_;if(!h.historyCurrent())return false;
        if(!h.step&&!fogGeometry_.step)return true; //Unchanged generic occurrence.
        if((h.step&&(fogGeometry_.step!=22||fogMembership_.step!=11))||
           h.allocation!=membershipAllocationGuard_||boundsScratchCenter_!=(h.step>=4)||
           boundsScratchHalf_!=(h.step>=5)||boundsEntered_!=(h.step>=7)||
           bool(boundsNativeShape_)!=(h.step>=7)||boundsReturned_!=(h.tailStep==100)||boundsCenterStored_!=(h.commonStep==35)||boundsHalfStores_!=h.halfStoredMask||
           boundsLocalCenterStores_!=(h.step>=18?7u:h.step>=16?1u:0u)||boundsLocalHalfStores_!=(h.step>=26?7u:0u)||
           boundsMatrixWords_!=h.matrixWords||boundsRadiusProduct_!=h.radiusProduct||boundsProducts_!=h.centerProducts)return false;
        if(h.selectedNonNull&&*h.selectedNonNull!=fogMembership_.heapNonNull.value_or(false))return false;
        constexpr std::array<std::uint32_t,4> vf0={{0,0,0,0x3f800000u}};
        auto halfExpected=vf0;
        for(unsigned k=0;k<3;++k)if(h.halfStoredMask&(1u<<k))halfExpected[k]=*h.scalarOuter[k];
        if((h.step>=4&&std::memcmp(boundsCenter_.v,h.commonResult?h.commonResult->data():vf0.data(),sizeof vf0))||
           (h.step>=5&&std::memcmp(boundsHalf_.v,halfExpected.data(),sizeof halfExpected)))return false;
        for(unsigned k=0;k<h.localVF0Stores;++k){auto expected=vf0;
            if(k<3&&(h.axisStoredMask&(1u<<k)))expected=*h.axisRegisters[k];
            if(k<3&&(h.directionStoredMask&(1u<<k)))expected=*h.directions[k].result;
            if(k==3&&h.step>=19)for(unsigned axis=0;axis<3;++axis)expected[axis]=*h.centerProducts[axis];
            if(k==4&&h.step>=27)for(unsigned axis=0;axis<3;++axis)expected[axis]=*h.halfProducts[axis];
            if(fogBoundsLocalQuads_[k]!=expected)return false;
        }
        if((h.step>=18&&std::bit_cast<std::uint32_t>(boundsLocalCenter_.v[3])!=0x3f800000u)||
           (h.step>=26&&std::bit_cast<std::uint32_t>(boundsLocalHalf_.v[3])!=0x3f800000u))return false;
        for(unsigned k=0;k<h.matrixWords;++k)if(h.matrix10[k]!=std::bit_cast<std::uint32_t>(boundsMatrix_.v[k]))return false;
        if(h.shape88&&(!collider_||*h.shape88!=collider_->shape88_))return false;
        if(h.step>=11&&(!shape_||shape_->known!=63||!collider_||!collider_->shape8C_||*collider_->shape8C_!=shape_.get()))return false;
        if(h.centerScale5C&&*h.centerScale5C!=std::bit_cast<std::uint32_t>(shape_->value.centerScale5C))return false;
        if(h.centerXYZ&&std::memcmp(h.centerXYZ->data(),shape_->value.localCenter.v,sizeof *h.centerXYZ))return false;
        if(h.radius50&&*h.radius50!=std::bit_cast<std::uint32_t>(shape_->value.radius50))return false;
        if(h.radiusScale58&&*h.radiusScale58!=std::bit_cast<std::uint32_t>(shape_->value.radiusScale58))return false;
        return true;
    }
    bool continueFogBoundsPrefix1200F0(std::uint32_t& pc)noexcept {
        if(pc!=0x1200f0||fogMembership_.step!=11||fogBounds_.step||membershipNativeWithdrawn_||
           nativeClosed_||!membershipE0Published_||!preflightNative())return false;
        auto& h=fogBounds_;const auto at=[&](std::uint32_t p){pc=pc_=p;};
        at(0x1200f0);h.step=1;
        const bool admitted=OriginalCollisionMembershipGuards50LA::beginAllocation(membershipAllocationGuard_);
        h.allocation=membershipAllocationGuard_;
        if(!admitted){at(h.allocation.pc);return false;}
        //The actual process-known-enabled owner supplies these reached cold/warm
        //guard effects. No reset, speculative enabled=0 branch or per-Fog globals.
        at(0x120114);at(0x120118);at(0x12011c);at(0x120120);at(0x120124);
        at(0x120128);at(0x12012c);h.selectedNonNull=fogMembership_.heapNonNull;h.step=2;
        const bool passed=OriginalCollisionMembershipGuards50LA::finishAllocation(membershipAllocationGuard_,*h.selectedNonNull);
        h.allocation=membershipAllocationGuard_;
        at(0x133660);at(0x133664);at(0x133668);
        if(!passed){at(0x13366c);return false;} //Unknown debug36C100, not a claim that NULL necessarily traps.
        //Both genuine debug branches return0 for actual nonNULL A0. Do not
        //invent the debug read or claim either individual branch was taken.
        at(0x1336a8);at(0x1336ac);at(0x1336b0);at(0x120130);at(0x120134);at(0x120158);h.step=3;
        constexpr std::array<std::uint32_t,4> vf0={{0,0,0,0x3f800000u}};
        at(0x12015c);std::memcpy(boundsCenter_.v,vf0.data(),sizeof vf0);boundsScratchCenter_=true;h.step=4;
        at(0x120160);at(0x120164);std::memcpy(boundsHalf_.v,vf0.data(),sizeof vf0);boundsScratchHalf_=true;h.step=5;
        at(0x120168);at(0x12016c);
        if(!source1B0_||!*source1B0_||*source1B0_!=collider_.get())return false;h.step=6;
        //Actual retained colliderS3 in call delay. Own return120170 is still ahead.
        at(0x1202e0);if(!shape_||shape_->known!=63||!collider_->shape8C_||*collider_->shape8C_!=shape_.get())return false;
        boundsNativeShape_.emplace();std::memcpy(&*boundsNativeShape_,&shape_->value,sizeof shape_->value);
        boundsEntered_=true; //Native fingerprint only; source shape/tag loads are still ahead.
        for(std::uint32_t p=0x1202e4;p<=0x120304;p+=4)at(p);h.step=7;
        constexpr std::uint32_t stores[9]={0x120308,0x120310,0x120318,0x120320,0x120328,0x120330,0x120338,0x120340,0x120348};
        for(unsigned k=0;k<9;++k){if(k)at(stores[k]-4);at(stores[k]);fogBoundsLocalQuads_[k]=vf0;++h.localVF0Stores;}
        h.step=8; //Only offsets50/60/70/C0/D0/E0/F0/100/110. Matrix80 and product260 remain unknown.
        at(0x12034c);h.shape88=collider_->shape88_;h.step=9;
        at(0x120350);if(h.shape88!=5u)return false; //Native unsupported-tag boundary, NOT original rejection/default return.
        for(std::uint32_t p=0x120354;p<=0x120368;p+=4)at(p);
        at(0x12036c);h.dispatchTarget=0x1208e4u;h.step=10; //Actual388AB0 table, tag5 word388AC4.
        at(0x120370);at(0x120374);at(0x1208e4);
        if(!shape_||shape_->known!=63||!collider_->shape8C_||*collider_->shape8C_!=shape_.get())return false;h.step=11;
        at(0x1208e8);at(0x1208ec);at(0x1208f0);
        at(0x1208f4);h.centerScale5C=std::bit_cast<std::uint32_t>(shape_->value.centerScale5C);h.step=12;
        std::array<std::uint32_t,3> center;
        for(unsigned k=0;k<3;++k){at(0x1208f8+4u*k);std::memcpy(center.data()+k,shape_->value.localCenter.v+k,4);}
        h.centerXYZ=center;h.step=13;
        //Pinned scalarEE PS2Float model, not hardware certification. Preserve
        //actual fs/ft ordering; no host FP or broad positive-normal shortcut.
        const auto centerProduct=[&](unsigned axis,std::uint32_t sourcePC,unsigned step)noexcept {
            at(sourcePC);const auto bits=(*h.centerXYZ)[axis];
            if(h.centerScale5C!=0x3f800000u||!OriginalTriggerFogGeometryPrefix50LA::unitProductDomain(bits))return false;
            boundsProducts_[axis]=h.centerProducts[axis]=bits;h.step=static_cast<std::uint8_t>(step);return true;
        };
        if(!centerProduct(0,0x120904,14)||!centerProduct(1,0x120908,15))return false;
        at(0x12090c);std::memcpy(boundsLocalCenter_.v,&*h.centerProducts[0],4);boundsLocalCenterStores_=1;h.step=16;
        if(!centerProduct(2,0x120910,17))return false;
        at(0x120914);std::memcpy(boundsLocalCenter_.v+1,&*h.centerProducts[1],4);boundsLocalCenterStores_|=2;
        at(0x120918);std::memcpy(boundsLocalCenter_.v+2,&*h.centerProducts[2],4);boundsLocalCenterStores_|=4;
        constexpr std::uint32_t one=0x3f800000u;
        at(0x12091c);std::memcpy(boundsLocalCenter_.v+3,&one,4);h.step=18;
        std::array<std::uint32_t,4> quad;at(0x120920);std::memcpy(quad.data(),boundsLocalCenter_.v,sizeof quad);
        at(0x120924);fogBoundsLocalQuads_[3]=quad;h.step=19; //Actual local260 temporary -> localC0, not caller output.
        at(0x120928);if(!shape_||shape_->known!=63||!collider_->shape8C_||*collider_->shape8C_!=shape_.get())return false;
        at(0x12092c);h.radius50=std::bit_cast<std::uint32_t>(shape_->value.radius50);
        at(0x120930);h.radiusScale58=std::bit_cast<std::uint32_t>(shape_->value.radiusScale58);h.step=20;
        for(unsigned k=0;k<4;++k){at(0x120934+4u*k);h.unitQuad[k]=one;++h.unitStores;}h.step=21;
        at(0x120944); //MUL.S f3,f1,f0: radius LEFT, actual scale RIGHT.
        if(h.radiusScale58!=one||!OriginalTriggerFogGeometryPrefix50LA::unitProductDomain(*h.radius50))return false;
        boundsRadiusProduct_=h.radiusProduct=*h.radius50;h.step=22;
        std::array<std::uint32_t,3> units;
        at(0x120948);units[2]=h.unitQuad[2];at(0x12094c);units[0]=h.unitQuad[0];at(0x120950);units[1]=h.unitQuad[1];
        h.unitXYZ=units; //Actual literal-unit temporary, not RNG or substituted source data.
        at(0x120954);h.halfProducts[2]=*h.radiusProduct;h.step=23;
        at(0x120958);h.halfProducts[0]=*h.radiusProduct;h.step=24;
        at(0x12095c);h.halfProducts[1]=*h.radiusProduct;h.step=25; //Each product LEFT, literal1 RIGHT in the same bounded scalar model.
        for(unsigned k=0;k<3;++k){at(0x120960+4u*k);std::memcpy(boundsLocalHalf_.v+k,&*h.halfProducts[k],4);boundsLocalHalfStores_|=1u<<k;}
        at(0x12096c);std::memcpy(boundsLocalHalf_.v+3,&one,4);h.step=26;
        at(0x120970);std::memcpy(quad.data(),boundsLocalHalf_.v,sizeof quad);
        at(0x120974);fogBoundsLocalQuads_[4]=quad;h.step=27; //Actual local270 temporary -> localD0.
        at(0x120978);if(!shape_||shape_->known!=63||!collider_->shape8C_||*collider_->shape8C_!=shape_.get())return false;
        at(0x12097c);h.step=28; //Retained A1=8 from1208F0, source shape+10, local matrix+80.
        for(unsigned pair=0;pair<8;++pair){std::uint32_t words[2];
            at(0x120980);std::memcpy(words,shape_->value.world.v+2u*pair,4);
            at(0x120984);std::memcpy(words+1,shape_->value.world.v+2u*pair+1u,4);at(0x120988);
            at(0x12098c);std::memcpy(boundsMatrix_.v+2u*pair,words,4);h.matrix10[2u*pair]=words[0];++h.matrixWords;++boundsMatrixWords_;
            at(0x120990);std::memcpy(boundsMatrix_.v+2u*pair+1u,words+1,4);h.matrix10[2u*pair+1u]=words[1];++h.matrixWords;++boundsMatrixWords_;
            at(0x120994);at(0x120998);at(0x12099c); //BGTZ delay executes on final pair too.
        }
        h.step=29;at(0x1209a0);at(0x1209a4);h.step=30;
        //Common-tail original instructions120C4C..120CD4, one receipt per
        //instruction. Local axes and caller output remain distinct owners.
        const auto done=[&](unsigned step)noexcept {h.commonStep=static_cast<std::uint8_t>(step);};
        const auto axisWord=[&](unsigned axis,unsigned lane,std::uint32_t bits,std::uint32_t p,unsigned step)noexcept {
            at(p);h.axisTemps[axis][lane]=bits;h.axisTempMasks[axis]|=1u<<lane;done(step);
        };
        at(0x120c4c);h.halfReads[0]=fogBoundsLocalQuads_[4][0];done(1);
        axisWord(2,0,0,0x120c50,2);
        at(0x120c54);h.halfReads[1]=fogBoundsLocalQuads_[4][1];done(3);
        axisWord(1,0,0,0x120c58,4);
        at(0x120c5c);h.halfReads[2]=fogBoundsLocalQuads_[4][2];done(5);
        axisWord(2,1,0,0x120c60,6);axisWord(0,0,*h.halfReads[0],0x120c64,7);
        axisWord(0,1,0,0x120c68,8);axisWord(1,1,*h.halfReads[1],0x120c6c,9);
        at(0x120c70);done(10); //Actual LUI3F80 supplies the following W stores.
        axisWord(0,2,0,0x120c74,11);axisWord(0,3,one,0x120c78,12);
        axisWord(1,2,0,0x120c7c,13);axisWord(2,2,*h.halfReads[2],0x120c80,14);
        axisWord(1,3,one,0x120c84,15);
        at(0x120c88);h.axisRegisters[0]=h.axisTemps[0];done(16);
        axisWord(2,3,one,0x120c8c,17); //Z.W is stored AFTER the X LQ.
        at(0x120c90);h.axisRegisters[1]=h.axisTemps[1];done(18);
        at(0x120c94);h.axisRegisters[2]=h.axisTemps[2];done(19);
        at(0x120c98);done(20); //Actual localC0 center operand.
        at(0x120c9c);fogBoundsLocalQuads_[0]=*h.axisRegisters[0];h.axisStoredMask|=1;done(21);
        at(0x120ca0);done(22); //Actual local80 matrix operand.
        at(0x120ca4);fogBoundsLocalQuads_[1]=*h.axisRegisters[1];h.axisStoredMask|=2;done(23);
        at(0x120ca8);fogBoundsLocalQuads_[2]=*h.axisRegisters[2];h.axisStoredMask|=4;done(24);
        for(unsigned column=0;column<4;++column){at(0x120cac+4u*column);
            std::memcpy(h.commonMatrix.data()+4u*column,boundsMatrix_.v+4u*column,16);++h.commonMatrixLoads;done(25u+column);}
        at(0x120cbc);h.commonCenter=fogBoundsLocalQuads_[3];done(29);
        at(0x120cc0);h.commonW=one;done(30); //Same architectural VMULw.w1*1 as v39.
        //Byte-identical v39 FMAC/VADD.xyz sequence and the same bounded
        //matrix/coordinate domain, still model-qualified, not hardware-certified.
        for(unsigned axis=0;axis<3;++axis){at(0x120cc4+4u*axis);const auto bits=(*h.commonCenter)[axis];
            if(!h.commonIdentity()||!OriginalTriggerFogGeometryPrefix50LA::unitProductDomain(bits))return false;
            h.commonProducts[axis]=bits;done(31u+axis);
        }
        at(0x120cd0);done(34); //Actual positive-zero translation, XYZ only; W remains1.
        std::array<std::uint32_t,4> result;
        for(unsigned axis=0;axis<3;++axis)result[axis]=*h.commonProducts[axis];result[3]=*h.commonW;
        at(0x120cd4);std::memcpy(boundsCenter_.v,result.data(),sizeof result);h.commonResult=result;boundsCenterStored_=true;done(35);
        //Three original direction chains. Full input copies precede local
        //overwrites. Their XYZ-only FMAC leaves the liveVF9.W untouched.
        const auto tailDone=[&](std::uint32_t p)noexcept {h.tailStep=static_cast<std::uint8_t>((p-0x120cd8u)/4u+1u);};
        const auto tailInstruction=[&](std::uint32_t p)noexcept {at(p);tailDone(p);};
        for(unsigned axis=0;axis<3;++axis){auto& d=h.directions[axis];const auto p=0x120cd8u+0x34u*axis;
            tailInstruction(p); //A1 selects the actual local50/60/70.
            at(p+4);d.copyRegister=fogBoundsLocalQuads_[axis];tailDone(p+4);
            tailInstruction(p+8); //A0 selects own local320/330/340 snapshot.
            at(p+12);d.snapshot=d.copyRegister;tailDone(p+12);
            for(unsigned column=0;column<3;++column){at(p+16u+4u*column);
                std::memcpy(d.matrix.data()+4u*column,boundsMatrix_.v+4u*column,16);++d.matrixLoads;tailDone(p+16u+4u*column);}
            at(p+28);d.input=d.snapshot;tailDone(p+28);
            for(unsigned term=0;term<3;++term){at(p+32u+4u*term);
                if(!h.directionDomain(axis))return false;
                //Pinned model: matrixONE LEFT,R RIGHT, remaining products +0.
                //ACC after X/Y andVF9.XYZ after Z have these separate receipts.
                std::array<std::uint32_t,3> value{};
                if(axis<=term)value[axis]=(*d.input)[axis];
                d.fmacXYZ[term]=value;tailDone(p+32u+4u*term);
            }
            at(p+44); //VADD.xyzw VF9,VF9,VF0, NOT the center's VADD.xyz.
            const auto priorW=axis?(*h.directions[axis-1].result)[3]:*h.commonW;
            if(priorW!=h.directionW[axis])return false;
            std::array<std::uint32_t,4> value;
            for(unsigned lane=0;lane<3;++lane){const auto sum=h.positiveZeroSum((*d.fmacXYZ[2])[lane],0u);
                if(!sum)return false;value[lane]=*sum;}
            //Only the proven model cases1+1,2+1,3+1. No host FP evaluation.
            value[3]=h.directionW[axis+1];d.wBefore=priorW;d.result=value;tailDone(p+44);
            at(p+48);fogBoundsLocalQuads_[axis]=*d.result;h.directionStoredMask|=1u<<axis;tailDone(p+48);
        }
        //Original scalar ABS temporaries378..398 and reloads. Preserve source
        //Z/X/Y order and inner X+Y before outer Z+inner. No reassociation.
        constexpr unsigned sourceAxis[3]={2,0,1};
        const auto scalarRead=[&](unsigned coordinate,unsigned slot)noexcept {
            const auto p=h.scalarReadPC[coordinate][slot];at(p);
            h.scalarReads[coordinate][slot]=fogBoundsLocalQuads_[sourceAxis[slot]][coordinate];tailDone(p);
        };
        const auto scalarAbsStore=[&](unsigned coordinate,unsigned slot)noexcept {
            at(h.scalarAbsPC[coordinate][slot]);h.scalarAbs[coordinate][slot]=*h.scalarReads[coordinate][slot]&0x7fffffffu;
            tailDone(h.scalarAbsPC[coordinate][slot]);at(h.scalarTempPC[coordinate][slot]);
            h.scalarTemps[coordinate][slot]=h.scalarAbs[coordinate][slot];tailDone(h.scalarTempPC[coordinate][slot]);
        };
        const auto scalarReload=[&](unsigned coordinate,unsigned slot)noexcept {
            const auto p=h.scalarReloadPC[coordinate][slot];at(p);
            h.scalarReloads[coordinate][slot]=h.scalarTemps[coordinate][slot];tailDone(p);
        };
        for(unsigned coordinate=0;coordinate<3;++coordinate){
            scalarRead(coordinate,0);if(coordinate==0)tailInstruction(0x120d78); //V1=378; later V1 carried from prior coordinate.
            scalarAbsStore(coordinate,0);scalarRead(coordinate,1);scalarReload(coordinate,0);
            tailInstruction(h.scalarAbsPC[coordinate][1]-4u); //V1=37C/388/394.
            scalarAbsStore(coordinate,1);scalarRead(coordinate,2);scalarReload(coordinate,1);
            tailInstruction(h.scalarAbsPC[coordinate][2]-4u); //V1=380/38C/398.
            scalarAbsStore(coordinate,2);scalarReload(coordinate,2);
            if(coordinate<2)tailInstruction(h.scalarInnerPC[coordinate]-4u); //V1=384/390 for the NEXT coordinate.
            at(h.scalarInnerPC[coordinate]);const auto inner=h.positiveZeroSum(*h.scalarReloads[coordinate][1],*h.scalarReloads[coordinate][2]);
            if(!inner)return false;h.scalarInner[coordinate]=inner;tailDone(h.scalarInnerPC[coordinate]);
            at(h.scalarOuterPC[coordinate]);const auto outer=h.positiveZeroSum(*h.scalarReloads[coordinate][0],*inner);
            if(!outer)return false;h.scalarOuter[coordinate]=outer;tailDone(h.scalarOuterPC[coordinate]);
            at(h.halfStorePC[coordinate]);std::memcpy(boundsHalf_.v+coordinate,&*outer,4);
            boundsHalfStores_|=1u<<coordinate;h.halfStoredMask|=1u<<coordinate;tailDone(h.halfStorePC[coordinate]);
        }
        //Real callee restores, JR and stack-delay. Caller W stays its earlier1.
        //No guest RA/stack backing is fabricated by this native history.
        for(std::uint32_t p=0x120e4c;p<=0x120e60;p+=4)tailInstruction(p);
        at(0x120e64);boundsReturned_=true;h.tailReturnPC=0x120170u;tailDone(0x120e64);
        at(0x120170);return true; //Bounds only. No caller setup, primary/portal registration or120070 return.
    }
    static bool positiveNormalBits(std::uint32_t b)noexcept {
        const auto exponent=b&0x7f800000u;return !(b&0x80000000u)&&exponent&&exponent!=0x7f800000u;
    }
    bool continueServiceBounds1200F0(std::uint32_t& pc)noexcept {
        if(fogGeometry_.step||!membershipE0Published_||membershipAllocationGuard_.entered||membershipNativeWithdrawn_||!preflightNative())return false;
        if(!OriginalCollisionMembershipGuards50LA::beginAllocation(membershipAllocationGuard_)){
            pc=pc_=membershipAllocationGuard_.pc;return false;}
        if(!OriginalCollisionMembershipGuards50LA::finishAllocation(membershipAllocationGuard_,bool(serviceHeapMembership_))){
            pc=pc_=membershipAllocationGuard_.pc;return false;}
        pc=pc_=0x12015c;boundsCenter_={{0,0,0,1}};boundsScratchCenter_=true;
        pc=pc_=0x120164;boundsHalf_={{0,0,0,1}};boundsScratchHalf_=true;
        pc=pc_=0x1202e0;
        if(!shape_||shape_->known!=63||!collider_||collider_->shape88_!=5||
           !collider_->shape8C_||*collider_->shape8C_!=shape_.get())return false;
        boundsEntered_=true;boundsNativeShape_=shape_->value;
        const auto& v=shape_->value;pc=pc_=0x1208f4;
        const auto scale=std::bit_cast<std::uint32_t>(v.centerScale5C);
        constexpr std::uint32_t multiplyPC[3]={0x120904,0x120908,0x120910};
        // Unit-scale, positive-normal exact subset; no host arithmetic or
        // denormal/overflow assumptions are used to supply original results.
        for(unsigned axis=0;axis<3;++axis){pc=pc_=multiplyPC[axis];
            const auto bits=std::bit_cast<std::uint32_t>(v.localCenter.v[axis]);
            if(scale!=0x3f800000u||!positiveNormalBits(bits))return false;
            boundsProducts_[axis]=bits;
            if(axis==1){pc=pc_=0x12090c;boundsLocalCenter_.v[0]=std::bit_cast<float>(*boundsProducts_[0]);boundsLocalCenterStores_|=1;}
        }
        pc=pc_=0x120914;boundsLocalCenter_.v[1]=std::bit_cast<float>(*boundsProducts_[1]);boundsLocalCenterStores_|=2;
        pc=pc_=0x120918;boundsLocalCenter_.v[2]=std::bit_cast<float>(*boundsProducts_[2]);boundsLocalCenterStores_|=4;
        pc=pc_=0x12091c;boundsLocalCenter_.v[3]=1;
        pc=pc_=0x120944;
        const auto radius=std::bit_cast<std::uint32_t>(v.radius50);
        if(std::bit_cast<std::uint32_t>(v.radiusScale58)!=0x3f800000u||!positiveNormalBits(radius))return false;
        boundsRadiusProduct_=radius;
        // 120954/958/95C multiply this product by source(1,1,1).
        constexpr std::uint32_t halfPC[3]={0x120960,0x120964,0x120968};
        for(unsigned axis=0;axis<3;++axis){pc=pc_=halfPC[axis];boundsLocalHalf_.v[axis]=std::bit_cast<float>(radius);boundsLocalHalfStores_|=1u<<axis;}
        pc=pc_=0x12096c;boundsLocalHalf_.v[3]=1;
        for(unsigned i=0;i<16;++i){pc=pc_=(i&1)?0x120990:0x12098c;
            std::memcpy(boundsMatrix_.v+i,v.world.v+i,4);++boundsMatrixWords_;}
        pc=pc_=0x120cc4;BodyMatrix48G id;identity(id);
        if(std::memcmp(boundsMatrix_.v,id.v,sizeof id.v))return false;
        pc=pc_=0x120cd4;boundsCenter_=boundsLocalCenter_;boundsCenterStored_=true;
        // Identity axes retain the radius; abs+zero sums are exact in this
        // positive-normal domain. Source writes only XYZ; caller's W survives.
        constexpr std::uint32_t outPC[3]={0x120dbc,0x120e04,0x120e48};
        for(unsigned axis=0;axis<3;++axis){pc=pc_=outPC[axis];boundsHalf_.v[axis]=boundsLocalHalf_.v[axis];boundsHalfStores_|=1u<<axis;}
        pc=pc_=0x120e64;boundsReturned_=true;pc=pc_=0x120188;return true;
    }
    bool serviceBoundsCurrentNative()const noexcept {
        if(!OriginalCollisionMembershipGuards50LA::allocationHistory(membershipAllocationGuard_)||
           (membershipAllocationGuard_.entered&&!membershipE0Published_)||
           (boundsScratchCenter_&&!membershipAllocationGuard_.returned)||(boundsScratchHalf_&&!boundsScratchCenter_)||
           (boundsEntered_&&(!boundsScratchHalf_||!boundsNativeShape_))||
           (boundsNativeShape_&&!boundsEntered_)||boundsLocalCenterStores_>7||boundsLocalHalfStores_>7||boundsHalfStores_>7||
           (boundsLocalCenterStores_&(boundsLocalCenterStores_+1u))||
           (boundsLocalHalfStores_&(boundsLocalHalfStores_+1u))||(boundsHalfStores_&(boundsHalfStores_+1u))||
           (boundsProducts_[1]&&!boundsProducts_[0])||(boundsProducts_[2]&&!boundsProducts_[1])||
           (boundsRadiusProduct_&&boundsLocalCenterStores_!=7)||(boundsLocalHalfStores_&&!boundsRadiusProduct_)||
           boundsMatrixWords_>16||(boundsMatrixWords_&&boundsLocalHalfStores_!=7)||
           (boundsCenterStored_&&boundsMatrixWords_!=16)||(boundsHalfStores_&&!boundsCenterStored_)||
           (boundsReturned_&&boundsHalfStores_!=7))return false;
        if(boundsNativeShape_){
            if(!shape_)return false;const auto& old=*boundsNativeShape_;const auto& now=shape_->value;
            if(std::memcmp(old.localCenter.v,now.localCenter.v,sizeof old.localCenter.v)||
               std::memcmp(old.world.v,now.world.v,sizeof old.world.v)||
               std::bit_cast<std::uint32_t>(old.radius50)!=std::bit_cast<std::uint32_t>(now.radius50)||
               std::bit_cast<std::uint32_t>(old.radiusScale58)!=std::bit_cast<std::uint32_t>(now.radiusScale58)||
               std::bit_cast<std::uint32_t>(old.centerScale5C)!=std::bit_cast<std::uint32_t>(now.centerScale5C))return false;
            for(unsigned k=0;k<3;++k){
                if(boundsProducts_[k]&&*boundsProducts_[k]!=std::bit_cast<std::uint32_t>(old.localCenter.v[k]))return false;
                if((boundsLocalCenterStores_&(1u<<k))&&(!boundsProducts_[k]||
                   std::bit_cast<std::uint32_t>(boundsLocalCenter_.v[k])!=*boundsProducts_[k]))return false;
                if((boundsLocalHalfStores_&(1u<<k))&&(!boundsRadiusProduct_||
                   std::bit_cast<std::uint32_t>(boundsLocalHalf_.v[k])!=*boundsRadiusProduct_))return false;
                if(boundsScratchCenter_&&std::bit_cast<std::uint32_t>(boundsCenter_.v[k])!=
                   (boundsCenterStored_?std::bit_cast<std::uint32_t>(boundsLocalCenter_.v[k]):0u))return false;
                if(boundsScratchHalf_&&std::bit_cast<std::uint32_t>(boundsHalf_.v[k])!=
                   ((boundsHalfStores_&(1u<<k))?std::bit_cast<std::uint32_t>(boundsLocalHalf_.v[k]):0u))return false;
            }
            if(boundsRadiusProduct_&&*boundsRadiusProduct_!=std::bit_cast<std::uint32_t>(old.radius50))return false;
            for(unsigned k=0;k<boundsMatrixWords_;++k)if(std::bit_cast<std::uint32_t>(boundsMatrix_.v[k])!=std::bit_cast<std::uint32_t>(old.world.v[k]))return false;
        }
        if((boundsScratchCenter_&&std::bit_cast<std::uint32_t>(boundsCenter_.v[3])!=0x3f800000u)||
           (boundsScratchHalf_&&std::bit_cast<std::uint32_t>(boundsHalf_.v[3])!=0x3f800000u))return false;
        return true;
    }
    struct SourceMembershipPortalEdge {
        const void* edge=nullptr;const void* next=nullptr;
        const OriginalWreckComponent50LA* neighbor=nullptr;const OriginalWreckComponent50LA* portal=nullptr;
        OriginalNativeListMethodCustody50LA* previousMethod=nullptr;
        OriginalNativeListMethodCustody50LA* method=nullptr;
        std::optional<std::uint32_t> slot,distance;
        source_portal_overlap231150::History overlap;
        unsigned edgeIndex=0;std::uint8_t distanceStoreMask=0,linkStoreMask=0;
        bool entered=false,overlapReturned=false,hit=false,methodReturned=false,selectionPending=false;
        bool methodPinned=false,methodPinWithdrawn=false,selectionAttempted=false,selectionReturned=false,selectionCanceled=false;
        bool counterAdvanced=false,inserted=false,slotWithdrawn=false,allocationAttempted=false,allocationRecorded=false,allocationReturned=false;
        bool headPublished=false,linkReleased=false,complete=false;
        std::uint32_t freeHeadBefore=0,lastUsedBefore=0,freeHeadAfter=0,lastUsedAfter=0;
        Membership::SecondaryLink* link=nullptr;const void* oldHead=nullptr;
    };
    struct SourceMembershipPortals280BF0 {
        bool entered=false,helperEntered=false,rootRead=false,returned=false,nativeWithdrawn=false;
        std::uint32_t pc=0;std::size_t count=0;
        const void* root=nullptr;const void* cursor=nullptr;
        OriginalNativeListMethodCustody50LA* cursorMethod=nullptr;
        std::shared_ptr<const void> graphPin;
        std::array<SourceMembershipPortalEdge,20> edges{};
    };
    struct ServicePrimary2809C0 {
        bool entered=false,methodResolved=false,methodPinned=false,methodUnpinned=false;
        bool selectionAttempted=false,selectionReturned=false,selectionPending=false,selectionCanceled=false;
        bool counterAdvanced=false,inserted=false,published=false,slotWithdrawn=false,nativeWithdrawn=false;
        const void* zone=nullptr;
        const ::ra_pod_scene50LA::Owner* world=nullptr;
        OriginalNativeListMethodCustody50LA* method=nullptr;
        std::optional<std::uint32_t> slot;
        std::uint32_t pc=0,freeBefore=0,lastBefore=0,freeAfter=0,lastAfter=0;
        std::array<std::uint32_t,4> center{};
        SourceMembershipPortals280BF0 portals;
    } servicePrimary_;
    OriginalTriggerFogPrimaryPrefix50LA fogPrimary_;
    const void* fogPortalZone_=nullptr;
    const OriginalNativeListMethodCustody50LA* fogPortalMethod_=nullptr;
    const void* fogPortalHead_=nullptr;
    std::shared_ptr<const void> fogPortalGraphPin_;
    bool fogPortalCustodyWithdrawn_=false;
    using FogPortalVertices=std::array<std::array<std::uint32_t,4>,4>;
    const C* fogPortalOperandA0_=nullptr;
    const FogPortalVertices* fogPortalVertices_=nullptr;
    const SpringVector4_48N* fogPortalCenterA1_=nullptr;
    //Native backing for caller sp+AC. Initial backing bits are NOT source
    //knownness; portalDot.outputDistance witnesses the reached2311D4 store.
    std::uint32_t fogPortalDistanceBacking_=0;
    std::uint32_t* fogPortalDistanceA2_=nullptr;
    std::array<std::uint32_t,4> fogPortalRelative10_{},fogPortalRelative00_{};
    std::array<std::uint32_t,4>* fogPortalRelativeA3_=nullptr;
    const std::array<std::uint32_t,4>* fogPortalNormal_=nullptr;
    std::uint32_t fogPortalTemporary3C_=0;
    std::uint32_t* fogPortalTemporaryV0_=nullptr;
    const std::array<std::uint32_t,4>* fogPortalEdgeRelativeT0_=nullptr;
    const std::array<std::uint32_t,4>* fogPortalFirstEdge_=nullptr;
    //Per-iteration aliases retain actual source storage, never guest-address bits.
    std::array<const std::array<std::uint32_t,4>*,3> fogPortalLoopVertices_{},fogPortalLoopEdges_{};
    std::array<std::uint32_t,4> fogPortalRelative20_{};
    const void* fogPortalNextCursor_=nullptr; //Miss-only actual next00 read, distinct from the immutable initial-head alias.
    const void* fogPortalHitNextCursor_=nullptr; //Separate published-hit next00; actual graph owner validates its same-root link.
    const C* fogPortalNextPortalA0_=nullptr; //Fresh next-occurrence280C40 portal08 only; no first-occurrence alias is overwritten.
    const std::array<std::array<std::uint32_t,4>,4>* fogPortalNextVertices_=nullptr;
    const SpringVector4_48N* fogPortalNextCenterA1_=nullptr;
    std::uint32_t* fogPortalNextDistanceA2_=nullptr; //Own argument alias to SAME caller sp+AC backing; no second location or initialization.
    OriginalBoundedPortalQuery50LA<OriginalTriggerFogPrimaryPrefix50LA,SpringVector4_48N,C> fogPortalNextQuery_;
    OriginalBoundedPortalMissTail50LA<OriginalTriggerFogPrimaryPrefix50LA,SpringVector4_48N,C> fogPortalMissTail_;
    const void* fogPortalNeighborNode_=nullptr;
    const C* fogPortalNeighborS0_=nullptr; //Actual neighbor04 Entity, never cast to the native SearchNode owner.
    OriginalNativeListMethodCustody50LA* fogPortalNeighborMethod_=nullptr;
    OriginalNativeListMethodCustody50LA* fogPortalCurrentMethodS2_=nullptr;
    OriginalNativeListMethodCustody50LA* fogPortalSelectA0_=nullptr;
    const SpringVector4_48N* fogPortalSelectCenterA1_=nullptr;
    const SpringVector4_48N* fogPortalSelectHalfA2_=nullptr;
    std::uint8_t fogPortalNeighborPinIdentity_=0; //Stable opaque native occurrence identity, never a guest token.
    bool fogPortalNeighborUnpinned_=false;
    std::uint32_t* const* fogPortalSelectorTable_=nullptr;
    std::uint32_t* const* fogPortalSelectorTableEnd_=nullptr;
    std::uint32_t* fogPortalSelectorBlock_=nullptr;
    std::uint32_t* fogPortalSelectorEntry_=nullptr;
    std::uint32_t* fogPortalSelectorS3_=nullptr;
    bool fogPortalSelectorWithdrawn_=false,fogPortalSelectorCanceled_=false;
    bool fogPortalSelectorCommitted_=false; //Successful native reservation handoff, not cancellation or source readiness.
    OriginalNativeListMethodCustody50LA* fogPortalInsertionA0_=nullptr;
    const B* fogPortalInsertionA1_=nullptr;
    std::uint32_t* fogPortalInsertionA2_=nullptr;
    const SpringVector4_48N* fogPortalInsertionA3_=nullptr;
    const SpringVector4_48N* fogPortalInsertionT0_=nullptr;
    bool fogPortalInsertionMemberWithdrawn_=false,fogPortalInsertionAbandoned_=false;
    bool fogPortalSelectorCurrentNative()const noexcept {
        const auto& f=fogPrimary_.portalSelector;const auto& call=fogPrimary_.portalEntryCall;const auto& inserted=fogPrimary_.portalInsertion;
        if(!f.historyCurrent()||!call.historyCurrent()||!inserted.historyCurrent()||
           (f.nativeReservationAttempted&&fogPrimary_.portalNeighbor.step!=21)||
           (call.step&&(f.step!=23||f.callerTarget!=0x280ca4u))||(inserted.step&&call.step!=8)||
           (inserted.lastUsed38&&inserted.lastUsed38!=f.freshLastUsed38)||
           (inserted.incremented38&&inserted.incremented38!=f.nativeOrdinal)||
           (inserted.returnPC&&inserted.returnPC!=call.callContinuation)||
           (fogPortalSelectorCommitted_&&(!fogSelectorCommitted_||!f.nativeReservationAcquired||inserted.step!=7||
            fogPrimary_.portalPoolPayload.stores!=63u||!fogPrimary_.returnFlow.triggerReturned()||!serviceRegistered_)))return false;
        const bool noAliases=!fogPortalSelectorTable_&&!fogPortalSelectorTableEnd_&&!fogPortalSelectorBlock_&&
            !fogPortalSelectorEntry_&&!fogPortalSelectorS3_&&!fogPortalInsertionA0_&&!fogPortalInsertionA1_&&
            !fogPortalInsertionA2_&&!fogPortalInsertionA3_&&!fogPortalInsertionT0_;
        if(fogPortalCustodyWithdrawn_)return noAliases&&fogPortalSelectorWithdrawn_==f.nativeReservationAttempted&&
            fogPortalSelectorCanceled_==(f.nativeReservationAcquired&&!fogPortalSelectorCommitted_)&&
            fogPortalInsertionMemberWithdrawn_==inserted.memberStored&&
            fogPortalInsertionAbandoned_==(inserted.counterStored&&!inserted.memberStored);
        if(fogPortalSelectorWithdrawn_||fogPortalSelectorCanceled_||fogPortalInsertionMemberWithdrawn_||fogPortalInsertionAbandoned_)return false;
        if((call.step>=2?fogPortalInsertionA0_!=fogPortalNeighborMethod_:bool(fogPortalInsertionA0_))||
           (call.step>=3?fogPortalInsertionA1_!=collider_.get():bool(fogPortalInsertionA1_))||
           (call.step>=4?fogPortalInsertionA2_!=fogPortalSelectorS3_:bool(fogPortalInsertionA2_))||
           (call.step>=5?fogPortalInsertionA3_!=&boundsCenter_:bool(fogPortalInsertionA3_))||
           (call.step>=8?fogPortalInsertionT0_!=&boundsHalf_:bool(fogPortalInsertionT0_)))return false;
        if(!f.nativeReservationAcquired)return noAliases&&(!fogPortalNeighborMethod_||
            fogPortalNeighborMethod_->sourceEntries_.pending!=&fogPortalNeighborPinIdentity_);
        if(!fogPortalNeighborMethod_||!fogPrimary_.portalNeighbor.nativePinAcquired||fogPortalNeighborUnpinned_)return false;
        const auto& c=*fogPortalNeighborMethod_;const auto& s=c.source_;const auto& e=c.sourceEntries_;
        const auto expectedLast=inserted.counterStored?inserted.incremented38:f.lastUsed38;
        if(!c.sourceCollisionEntriesCurrentNative()||(!fogPortalSelectorCommitted_&&e.terminal)||
           (f.perBlock30&&s.perBlock30!=f.perBlock30)||(f.blockCount34&&s.blockCount34!=f.blockCount34))return false;
        if(fogPortalSelectorCommitted_){
            if(e.pending&&(e.pending==&fogPortalNeighborPinIdentity_||e.pendingIdentity==collider_.get()||
               e.pendingSlot==f.nativeOrdinal.value_or(0u)))return false;
        }else if(e.pending!=&fogPortalNeighborPinIdentity_||e.pendingIdentity!=collider_.get()||
           e.pendingConsumed!=inserted.counterStored||e.pendingSlot!=f.nativeOrdinal.value_or(0u)||
           (f.freeHeadNull&&s.field40!=0u)||(expectedLast&&s.lastUsed38!=expectedLast))return false;
        //Completed reads of lastUsed/freeHead stay historical; exact table/cell/borrower custody remains live.
        if(f.tableNonNull){if(!s.pointer3C||*s.pointer3C!=e.table.get()||fogPortalSelectorTable_!=e.table.get()||
            *f.tableNonNull!=bool(fogPortalSelectorTable_))return false;}
        else if(fogPortalSelectorTable_)return false;
        if(f.step>=14){if(!fogPortalSelectorTable_||fogPortalSelectorTableEnd_!=fogPortalSelectorTable_+1)return false;}
        else if(fogPortalSelectorTableEnd_)return false;
        if(f.blockRead){if(!e.ready||!e.words||!fogPortalSelectorTableEnd_||fogPortalSelectorBlock_!=fogPortalSelectorTableEnd_[-1]||
            fogPortalSelectorBlock_!=e.words.get())return false;}
        else if(fogPortalSelectorBlock_)return false;
        if(f.entryAliasResolved){const auto index=*f.nativeOrdinal-1u;
            if(index>=1024u||!fogPortalSelectorBlock_||fogPortalSelectorEntry_!=fogPortalSelectorBlock_+index||
               e.members[index]!=(inserted.memberStored?collider_.get():nullptr)||
               e.borrowers[index]!=(inserted.memberStored?&fogPortalNeighborPinIdentity_:nullptr)||e.wordKnown[index])return false;}
        else if(fogPortalSelectorEntry_)return false;
        if(f.s3Retained)return fogPortalSelectorS3_==fogPortalSelectorEntry_&&f.selectedNonNull==bool(fogPortalSelectorS3_);
        return !fogPortalSelectorS3_;
    }
    bool withdrawFogPortalSelectorNative()noexcept {
        const auto& f=fogPrimary_.portalSelector;
        if(!f.nativeReservationAttempted)return true;
        if(!fogPortalSelectorCurrentNative()||
           (fogPortalSelectorCommitted_&&fogPortalNeighborMethod_->sourceEntries_.pending))return false;
        const auto& inserted=fogPrimary_.portalInsertion;
        fogPortalInsertionA0_=nullptr;fogPortalInsertionA1_=nullptr;fogPortalInsertionA2_=nullptr;
        fogPortalInsertionA3_=fogPortalInsertionT0_=nullptr;
        fogPortalSelectorS3_=fogPortalSelectorEntry_=fogPortalSelectorBlock_=nullptr;
        fogPortalSelectorTableEnd_=fogPortalSelectorTable_=nullptr;
        //All usable aliases die before cancellation or member unlink. Never clear a later occurrence's pending reservation.
        if(f.nativeReservationAcquired){auto& c=*fogPortalNeighborMethod_;auto& e=c.sourceEntries_;
            if(!fogPortalSelectorCommitted_){
                if(e.pendingConsumed){
                    if(!inserted.memberStored){e.abandonedConsumed=e.pendingSlot;e.terminal=true;fogPortalInsertionAbandoned_=true;}
                    e.pendingConsumed=false;
                }
                e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;fogPortalSelectorCanceled_=true;
            }
            if(inserted.memberStored){const auto index=*f.nativeOrdinal-1u; //Exact cell was checked before any custody mutation.
                e.nativeFreeNext[index]=*c.source_.field40;e.members[index]=e.borrowers[index]=nullptr;
                e.wordKnown[index]=false;c.source_.field40=*f.nativeOrdinal;fogPortalInsertionMemberWithdrawn_=true;
            }
        }
        fogPortalSelectorWithdrawn_=true;return true; //Native unlink only. No original return, count rollback or fabricated guest free address.
    }
    Membership::FogPoolAllocator* fogPortalPoolA0_=nullptr; //Retained native call-argument custody, not live architectural A0 after scalar loads.
    Membership::FogPoolAllocator* fogPortalPoolS0_=nullptr;
    Membership::SecondaryLink* fogPortalPoolFreeV0_=nullptr;
    bool fogPortalPoolWithdrawn_=false,fogPortalPoolRecordWithdrawn_=false,fogPortalPoolSlot14Withdrawn_=false;
    Membership::FogPoolChunk* fogPortalPoolChunkA0_=nullptr;
    Membership::SecondaryLink* fogPortalPoolBaseA0_=nullptr;
    Membership::SecondaryLink* fogPortalPoolNextV1_=nullptr;
    Membership::SecondaryLink* fogPortalPoolResultV0_=nullptr;
    const C* fogPortalPayloadV1_=nullptr; //Actual280CDC portal08 load; replaced by genuineNULL head at280CEC.
    bool fogPortalPoolCurrentNative()const noexcept {
        const auto& f=fogPrimary_.portalPool;const auto& m=fogPrimary_.portalPoolMutation;const auto& r=fogPrimary_.portalPoolReturn;
        const auto& slot=fogPrimary_.portalPoolSlot14;const auto& payload=fogPrimary_.portalPoolPayload;
        if(!f.historyCurrent()||!m.historyCurrent(f)||!r.historyCurrent(f,m)||!slot.historyCurrent(r)||!payload.historyCurrent(slot)||
           (f.callStep&&(fogPrimary_.portalInsertion.step!=7||fogPrimary_.portalInsertion.returnPC!=0x280cc4u)))return false;
        const bool noAliases=!fogPortalPoolA0_&&!fogPortalPoolS0_&&!fogPortalPoolFreeV0_&&!fogPortalPoolChunkA0_&&
            !fogPortalPoolBaseA0_&&!fogPortalPoolNextV1_&&!fogPortalPoolResultV0_&&!fogPortalPayloadV1_;
        if(fogPortalCustodyWithdrawn_)return noAliases&&fogPortalPoolWithdrawn_==bool(f.callStep)&&
            fogPortalPoolRecordWithdrawn_==m.poolStored&&fogPortalPoolSlot14Withdrawn_==slot.stored;
        if(fogPortalPoolWithdrawn_||fogPortalPoolRecordWithdrawn_||fogPortalPoolSlot14Withdrawn_)return false;
        if(f.callStep<3)return noAliases;
        if(!serviceHeapMembership_||!serviceHeapMembership_->fogPoolAliasesCurrentNative(&servicePrimary_,fogPortalPoolA0_,fogPortalPoolFreeV0_)||
           (r.step>=8?bool(fogPortalPoolS0_):(f.bodyStep>=6?fogPortalPoolS0_!=fogPortalPoolA0_:bool(fogPortalPoolS0_)))||
           (f.free2CNull?*f.free2CNull!=!fogPortalPoolFreeV0_:bool(fogPortalPoolFreeV0_)))return false;
        const bool free=m.fromFree.value_or(false);
        if(!free&&m.step>=2){if(!serviceHeapMembership_->fogPoolChunkCurrentNative(&servicePrimary_,fogPortalPoolA0_,fogPortalPoolChunkA0_))return false;}
        else if(fogPortalPoolChunkA0_)return false;
        if(!free&&m.step>=5){if(fogPortalPoolBaseA0_!=&fogPortalPoolChunkA0_->records[0])return false;}
        else if(fogPortalPoolBaseA0_)return false;
        if(m.nextNull){if(*m.nextNull!=!fogPortalPoolNextV1_||
            !serviceHeapMembership_->fogPoolAliasesCurrentNative(&servicePrimary_,fogPortalPoolA0_,fogPortalPoolNextV1_))return false;}
        else if(fogPortalPoolNextV1_)return false;
        const auto& a=serviceHeapMembership_->fogPoolAllocation_;
        if(a.entered!=m.poolStored||a.accountingStored!=r.accountingStored||a.slot14Stored!=slot.stored||a.slot14AliasWithdrawn||
           a.slot14ReservationCommitted!=fogPortalSelectorCommitted_||
           a.payloadMask!=payload.stores||a.oldHeadRead!=payload.oldHeadNull.has_value())return false;
        if(m.poolStored&&(a.nativeWithdrawn||a.primaryOccurrence!=&servicePrimary_||a.secondaryOccurrence!=&fogPortalNeighborPinIdentity_||
           a.pool!=fogPortalPoolA0_||a.fromFree!=free||!serviceHeapMembership_->fogPoolAllocationHistoryNative()||
           a.record!=(free?fogPortalPoolFreeV0_:&fogPortalPoolChunkA0_->records[*f.used20])))return false;
        if(m.resultAddressRetained){if(fogPortalPoolResultV0_!=a.record)return false;}
        else if(fogPortalPoolResultV0_)return false;
        if(slot.stored&&(a.slot14Method!=fogPortalNeighborMethod_||a.slot14Member!=collider_.get()||
           a.slot14NativeOrdinal!=fogPrimary_.portalSelector.nativeOrdinal||a.record->fogSlot14!=fogPortalSelectorS3_))return false;
        if((payload.step>=3&&payload.step<7?fogPortalPayloadV1_!=fogPortalOperandA0_:bool(fogPortalPayloadV1_))||
           ((payload.stores&1u)&&a.child04!=fogPortalNeighborS0_)||
           ((payload.stores&4u)&&a.portal0C!=fogPortalOperandA0_)||
           (payload.distanceF0&&payload.distanceF0!=fogPrimary_.portalDot.outputDistance)||
           ((payload.stores&8u)&&a.distance10!=payload.distanceF0))return false;
        return true; //Read snapshots remain historical; only this exact consumed record is continuously owned.
    }
    bool fogPortalPoolApplicationCurrentNative(const OriginalGarageObserverShared50KG& actualShared)const noexcept {
        return !fogPrimary_.portalPool.callStep||fogPortalCustodyWithdrawn_||
            (serviceHeapMembership_&&serviceHeapMembership_->shared_==&actualShared);
    }
    bool withdrawFogPortalPoolNative()noexcept {
        if(!fogPrimary_.portalPool.callStep)return true;
        if(!fogPortalPoolCurrentNative())return false;
        if(fogPrimary_.portalPoolSlot14.stored){
            if(!serviceHeapMembership_->withdrawFogPoolSlot14AliasNative(&servicePrimary_,&fogPortalNeighborPinIdentity_))return false;
            fogPortalPoolSlot14Withdrawn_=true;
        }
        fogPortalPoolResultV0_=fogPortalPoolNextV1_=fogPortalPoolBaseA0_=nullptr;fogPortalPoolChunkA0_=nullptr;fogPortalPayloadV1_=nullptr;
        fogPortalPoolFreeV0_=nullptr;fogPortalPoolS0_=fogPortalPoolA0_=nullptr;fogPortalPoolWithdrawn_=true;
        return true; //Usable frame aliases die first. The exact consumed record is released after the own List member.
    }
    bool fogPortalNeighborCurrentNative()const noexcept {
        const auto& f=fogPrimary_.portalNeighbor;
        if(!f.historyCurrent(fogPrimary_.portalHit)||!fogPortalSelectorCurrentNative()||!fogPortalPoolCurrentNative())return false;
        if(fogPortalCustodyWithdrawn_)return !fogPortalNeighborNode_&&!fogPortalNeighborS0_&&!fogPortalNeighborMethod_&&
            !fogPortalCurrentMethodS2_&&!fogPortalSelectA0_&&!fogPortalSelectCenterA1_&&!fogPortalSelectHalfA2_&&
            (!f.nativePinAcquired||fogPortalNeighborUnpinned_);
        if(fogPortalNeighborUnpinned_||(f.neighborAdmissionAttempted&&!fogPortalGraphPin_)||
           bool(fogPortalNeighborNode_)!=(f.step>=1)||bool(fogPortalNeighborS0_)!=(f.step>=1)||
           bool(fogPortalNeighborMethod_)!=(f.step>=9)||
           (f.step>=13?fogPortalCurrentMethodS2_!=fogPortalNeighborMethod_:bool(fogPortalCurrentMethodS2_))||
           (f.step>=14?fogPortalSelectA0_!=fogPortalNeighborMethod_:bool(fogPortalSelectA0_))||
           (f.step>=15&&f.step<21?fogPortalSelectCenterA1_!=&boundsCenter_:bool(fogPortalSelectCenterA1_))||
           (f.step>=16?fogPortalSelectHalfA2_!=&boundsHalf_:bool(fogPortalSelectHalfA2_)))return false;
        if(fogPortalNeighborMethod_){const auto& c=*fogPortalNeighborMethod_;
            if(fogPortalNeighborMethod_==servicePrimary_.method||!c.sourceMode_||c.world_!=servicePrimary_.world||
               c.application_!=application_||c.ordinal_<10u||c.ordinal_>=20u||!c.source_.usageKnown||c.usage10_!=1u||
               !c.source_.constructorReturned||c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())return false;
            unsigned pins=0;for(const auto* id:c.sourceCollisionRegistrationBorrowers_)if(id==&fogPortalNeighborPinIdentity_)++pins;
            if(pins!=(f.nativePinAcquired?1u:0u))return false; //Own pending/count/member custody is checked separately, never borrowed from primary.
            unsigned members=0;for(const auto* id:c.sourceEntries_.borrowers)if(id==&fogPortalNeighborPinIdentity_)++members;
            if(members!=(fogPrimary_.portalInsertion.memberStored?1u:0u))return false;
        }
        return true;
    }
    bool withdrawFogPortalNeighborNative()noexcept {
        if(!fogPortalNeighborCurrentNative()||
           (fogPortalSelectorCommitted_&&fogPortalNeighborMethod_->sourceEntries_.pending))return false;
        //Refuse before any observer/head alias changes if unlink would invalidate another selection's free-head read.
        if(!withdrawFogPortalPoolNative()||!withdrawFogPortalSelectorNative())return false;
        if(fogPrimary_.portalPoolMutation.poolStored){
            if(!serviceHeapMembership_||!serviceHeapMembership_->releaseFogPoolAllocationNative(&servicePrimary_,&fogPortalNeighborPinIdentity_))return false;
            fogPortalPoolRecordWithdrawn_=true;
        }
        const auto clearAliases=[&]()noexcept {
            fogPortalSelectCenterA1_=fogPortalSelectHalfA2_=nullptr;
            fogPortalSelectA0_=fogPortalCurrentMethodS2_=fogPortalNeighborMethod_=nullptr;
            fogPortalNeighborS0_=nullptr;fogPortalNeighborNode_=nullptr;
        };
        if(fogPrimary_.portalNeighbor.nativePinAcquired&&!fogPortalNeighborUnpinned_){
            auto& pins=fogPortalNeighborMethod_->sourceCollisionRegistrationBorrowers_;
            auto it=pins.begin();while(it!=pins.end()&&*it!=&fogPortalNeighborPinIdentity_)++it;
            if(it==pins.end())return false;
            clearAliases();pins.erase(it);fogPortalNeighborUnpinned_=true;
        }else clearAliases();
        return true; //Usable aliases die before the own native pin; no source reference decrement or primary-slot withdrawal.
    }
    bool fogPortalSharedOutputCurrentNative()const noexcept {
        // Actual caller SP+AC has one backing. Source-reached stores choose its
        // current writer; earlier reads and published records keep their own value.
        const auto& second=fogPrimary_.portalNextQuery.portalDot.outputDistance;
        auto latest=second?second:fogPrimary_.portalDot.outputDistance;
        for(const auto& o:fogPrimary_.portalMissTail.occurrences)
            if(o.query.portalDot.outputDistance)latest=o.query.portalDot.outputDistance;
        return !latest||fogPortalDistanceBacking_==*latest;
    }
    bool fogPortalNextInputsCurrentNative()const noexcept {
        const auto& f=fogPrimary_.portalNextInputs;
        if(!f.historyCurrent(fogPrimary_.portalHitNext,fogPrimary_.portalHead)||
           !fogPortalNextQuery_.current(fogPrimary_.portalNextQuery,f,fogPortalNextPortalA0_,fogPortalNextVertices_,
                fogPortalNextCenterA1_,&fogPortalDistanceBacking_,fogPortalCustodyWithdrawn_)||
           !fogPortalSharedOutputCurrentNative())return false;
        if(fogPortalCustodyWithdrawn_)return !fogPortalNextVertices_&&!fogPortalNextCenterA1_&&!fogPortalNextDistanceA2_;
        if(bool(fogPortalNextVertices_)!=f.nativeInputsAcquired||
           (fogPrimary_.portalNextQuery.portalEdge0.step>=14?bool(fogPortalNextDistanceA2_):
             (f.step>=1?fogPortalNextDistanceA2_!=&fogPortalDistanceBacking_:bool(fogPortalNextDistanceA2_)))||
           (f.step>=4?fogPortalNextCenterA1_!=&boundsCenter_:bool(fogPortalNextCenterA1_))||
           (f.nativeInputsAcquired&&(!fogPortalGraphPin_||!fogPortalHitNextCursor_||!fogPortalNextPortalA0_)))return false;
        for(unsigned k=0;k<3;++k){
            if(f.centerWords[k]&&*f.centerWords[k]!=std::bit_cast<std::uint32_t>(boundsCenter_.v[k]))return false;
            if(f.vertexWords[k]&&*f.vertexWords[k]!=(*fogPortalNextVertices_)[0][k])return false;
        }
        return true; //Historical first values remain fixed; only a reached second output store changes SP+AC.
    }
    bool fogPortalAfterCurrentNative()const noexcept {
        const auto& hit=fogPrimary_.portalHit;const auto& next=fogPrimary_.portalNext;const auto& afterHit=fogPrimary_.portalHitNext;
        if(!hit.historyCurrent(fogPrimary_.portalLoop,fogPrimary_.portalDot)||
           !next.historyCurrent(fogPrimary_.portalMissAt280CF8(),fogPrimary_.publication)||
           !afterHit.historyCurrent(fogPrimary_.portalPoolPayload,fogPrimary_.publication)||
           (next.nativeAdmissionAttempted&&afterHit.nativeAdmissionAttempted)||!fogPortalNeighborCurrentNative()||
           !fogPortalNextInputsCurrentNative()||
           !fogPortalMissTail_.current(fogPrimary_.portalMissTail,fogPortalHead_,fogPortalNextCursor_,fogPortalHitNextCursor_,
                &boundsCenter_,&fogPortalDistanceBacking_,fogPortalCustodyWithdrawn_))return false;
        if(fogPortalCustodyWithdrawn_)return !fogPortalNextCursor_&&!fogPortalHitNextCursor_&&!fogPortalNextPortalA0_;
        if((hit.step||next.nativeAdmissionAttempted||afterHit.nativeAdmissionAttempted)&&!fogPortalGraphPin_)return false;
        if(hit.reloadF0&&hit.reloadF0!=fogPrimary_.portalDot.outputDistance)return false;
        if((afterHit.step?afterHit.nextNonNull!=bool(fogPortalHitNextCursor_):bool(fogPortalHitNextCursor_))||
           (fogPortalHitNextCursor_&&fogPortalHitNextCursor_==fogPortalHead_)||
           bool(fogPortalNextPortalA0_)!=afterHit.portalReadCompleted)return false;
        return next.step?next.nextNonNull==bool(fogPortalNextCursor_):!fogPortalNextCursor_;
    }
    const std::array<std::uint32_t,4>* fogPortalLiveLocal00History()const noexcept {
        const auto* latest=fogPrimary_.portalRelative.local00?&*fogPrimary_.portalRelative.local00:nullptr;
        for(const auto& it:fogPrimary_.portalLoop.iterations)if(it.local00)latest=&*it.local00;
        return latest; //Latest reached SQ, not an older historical snapshot.
    }
    bool fogPortalLoopCurrentNative()const noexcept {
        const auto& f=fogPrimary_.portalLoop;
        if(!f.historyCurrent(fogPrimary_.portalInputs,fogPrimary_.portalEdge0)||!fogPortalAfterCurrentNative())return false;
        if(fogPortalCustodyWithdrawn_){for(unsigned i=0;i<3;++i)if(fogPortalLoopVertices_[i]||fogPortalLoopEdges_[i])return false;return true;}
        std::array<std::optional<std::uint32_t>,4> latest20;
        for(unsigned i=0;i<3;++i){const auto& it=f.iterations[i];
            if(bool(fogPortalLoopVertices_[i])!=(it.step>=1)||bool(fogPortalLoopEdges_[i])!=(it.step>=8))return false;
            if(it.step&&(!fogPortalGraphPin_||!fogPortalVertices_||fogPortalLoopVertices_[i]!=&(*fogPortalVertices_)[i+1]))return false;
            for(unsigned k=0;k<3;++k){
                if(it.centerWords[k]&&*it.centerWords[k]!=std::bit_cast<std::uint32_t>(boundsCenter_.v[k]))return false;
                if(it.vertexWords[k]&&*it.vertexWords[k]!=(*fogPortalLoopVertices_[i])[k])return false;
                if(it.edgeVF5[k]&&*it.edgeVF5[k]!=(*fogPortalLoopEdges_[i])[k])return false;
            }
            for(unsigned k=0;k<4;++k)if(it.temporary20[k])latest20[k]=it.temporary20[k];
        }
        for(unsigned k=0;k<4;++k)if(latest20[k]&&fogPortalRelative20_[k]!=*latest20[k])return false;
        const auto* latest=fogPortalLiveLocal00History();return !latest||fogPortalRelative00_==*latest;
    }
    bool fogPortalFirstEdgeCurrentNative()const noexcept {
        const auto& f=fogPrimary_.portalEdge0;
        if(!f.historyCurrent(fogPrimary_.portalSlab,fogPrimary_.portalRelative,fogPrimary_.portalInputs)||!fogPortalLoopCurrentNative())return false;
        if(fogPortalCustodyWithdrawn_)return !fogPortalFirstEdge_;
        if(bool(fogPortalFirstEdge_)!=(f.step>=3))return false;
        if(f.step&&!fogPortalGraphPin_)return false;
        //relativeVF4 is the edge0 historical reload; later loop SQ may overwrite local00.
        if(fogPortalFirstEdge_)for(unsigned k=0;k<3;++k)if(f.edgeVF5[k]!=(*fogPortalFirstEdge_)[k])return false;
        return true;
    }
    bool fogPortalSlabCurrentNative()const noexcept {
        const auto& f=fogPrimary_.portalSlab;
        if(!f.historyCurrent(fogPrimary_.portalDot,fogPrimary_.portalInputs)||!fogPortalFirstEdgeCurrentNative())return false;
        if(fogPortalCustodyWithdrawn_)return !fogPortalEdgeRelativeT0_;
        return f.negativeStep>=5?fogPortalEdgeRelativeT0_==&fogPortalRelative00_:!fogPortalEdgeRelativeT0_;
    }
    bool fogPortalDotCurrentNative()const noexcept {
        const auto& f=fogPrimary_.portalDot;
        if(!f.historyCurrent(fogPrimary_.portalRelative)||!fogPortalSlabCurrentNative())return false;
        if(fogPortalCustodyWithdrawn_)return !fogPortalNormal_&&!fogPortalTemporaryV0_;
        if(bool(fogPortalNormal_)!=(f.step>=1)||
           (f.step>=10?fogPortalTemporaryV0_!=&fogPortalTemporary3C_:bool(fogPortalTemporaryV0_)))return false;
        if(f.step&&!fogPortalGraphPin_)return false;
        if(fogPortalNormal_)for(unsigned k=0;k<3;++k)if(f.normalVF5[k]!=(*fogPortalNormal_)[k])return false;
        return (!f.temporary3C||fogPortalTemporary3C_==*f.temporary3C)&&
            fogPortalSharedOutputCurrentNative();
    }
    bool fogPortalRelativeCurrentNative()const noexcept {
        const auto& f=fogPrimary_.portalRelative;
        if(!f.historyCurrent(fogPrimary_.portalInputs))return false;
        if(fogPortalCustodyWithdrawn_)return !fogPortalRelativeA3_;
        if(fogPrimary_.portalLoop.iterations[0].step>=8?bool(fogPortalRelativeA3_):
           (f.step>=3?fogPortalRelativeA3_!=&fogPortalRelative00_:bool(fogPortalRelativeA3_)))return false;
        for(unsigned k=0;k<4;++k)if(f.temporary10[k]&&fogPortalRelative10_[k]!=*f.temporary10[k])return false;
        const auto* latest=fogPortalLiveLocal00History();return !latest||fogPortalRelative00_==*latest;
    }
    bool fogPortalInputsCurrentNative()const noexcept {
        const auto& f=fogPrimary_.portalInputs;
        if(!f.historyCurrent()||!fogPortalRelativeCurrentNative()||!fogPortalDotCurrentNative())return false;
        if(fogPortalCustodyWithdrawn_||!f.step)return !fogPortalOperandA0_&&!fogPortalVertices_&&
            !fogPortalCenterA1_&&!fogPortalDistanceA2_;
        if(!fogPortalGraphPin_||!fogPortalHead_||fogPrimary_.portalHead.step!=9||
           fogPrimary_.portalHead.headNull!=false||!fogPortalOperandA0_||!fogPortalVertices_)return false;
        if((fogPrimary_.portalEdge0.step>=14?bool(fogPortalDistanceA2_):
            (f.step>=2?fogPortalDistanceA2_!=&fogPortalDistanceBacking_:bool(fogPortalDistanceA2_)))||
           (f.step>=5?fogPortalCenterA1_!=&boundsCenter_:bool(fogPortalCenterA1_)))return false;
        for(unsigned k=0;k<3;++k){
            if(f.centerWords[k]&&*f.centerWords[k]!=std::bit_cast<std::uint32_t>(boundsCenter_.v[k]))return false;
            if(f.vertexWords[k]&&*f.vertexWords[k]!=(*fogPortalVertices_)[0][k])return false;
        }
        return true;
    }
    bool fogPortalHeadCurrentNative()const noexcept {
        const auto& f=fogPrimary_.portalHead;const auto& h=servicePrimary_;
        if(!f.historyCurrent()||!fogPortalInputsCurrentNative()||(f.step&&fogPrimary_.publication.step!=12))return false;
        if(!f.step)return !fogPortalZone_&&!fogPortalMethod_&&!fogPortalHead_&&!fogPortalGraphPin_&&!fogPortalCustodyWithdrawn_;
        if(fogPortalCustodyWithdrawn_)return !fogPortalZone_&&!fogPortalMethod_&&!fogPortalHead_&&!fogPortalGraphPin_;
        if(!h.published||h.nativeWithdrawn||h.methodUnpinned||!serviceHeapMembership_||
           bool(fogPortalGraphPin_)!=f.nativePinAcquired)return false;
        const auto& record=serviceHeapMembership_->record_;
        if(f.step>=2){if(!fogPortalZone_||fogPortalZone_!=record.zone04||fogPortalZone_!=h.zone)return false;}
        else if(fogPortalZone_)return false;
        if(f.step>=3){if(!fogPortalMethod_||fogPortalMethod_!=record.method08||fogPortalMethod_!=h.method)return false;}
        else if(fogPortalMethod_)return false;
        return f.headNull?*f.headNull==!fogPortalHead_:!fogPortalHead_;
    }
    bool withdrawFogPortalHeadNative()noexcept {
        if(!fogPrimary_.portalHead.step||fogPortalCustodyWithdrawn_)return fogPortalHeadCurrentNative();
        if(!fogPortalHeadCurrentNative()||!withdrawFogPortalNeighborNative())return false;
        fogPortalNextCursor_=fogPortalHitNextCursor_=nullptr;fogPortalNextPortalA0_=nullptr;
        fogPortalNextQuery_.retire(); //Own activation aliases die before the complete-chain graph pin.
        fogPortalMissTail_.retire(); //Every later occurrence and next cursor dies before the same graph token.
        fogPortalNextVertices_=nullptr;fogPortalNextCenterA1_=nullptr;fogPortalNextDistanceA2_=nullptr;
        fogPortalLoopVertices_.fill(nullptr);fogPortalLoopEdges_.fill(nullptr);
        fogPortalFirstEdge_=nullptr;fogPortalEdgeRelativeT0_=nullptr;fogPortalNormal_=nullptr;fogPortalTemporaryV0_=nullptr;fogPortalRelativeA3_=nullptr;
        fogPortalOperandA0_=nullptr;fogPortalVertices_=nullptr;fogPortalCenterA1_=nullptr;fogPortalDistanceA2_=nullptr;
        fogPortalHead_=fogPortalZone_=nullptr;fogPortalMethod_=nullptr;
        fogPortalGraphPin_.reset();fogPortalCustodyWithdrawn_=true;
        return true; //Native aliases die before graph token release; immutable source reads/return remain.
    }
    template<class ActualHead>
    bool continueFogPortalHead280BF0(ActualHead&& actualHead,std::uint32_t& pc)noexcept {
        if(pc!=0x280bf0||fogPrimary_.publication.step!=12||fogPrimary_.portalHead.step||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& f=fogPrimary_.portalHead;auto& observer=*serviceHeapMembership_;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        for(std::uint32_t p=0x280bf0;p<=0x280c18;p+=4)at(p);f.step=1;
        at(0x280c1c);if(observer.record_.zone04!=h.zone||!h.zone)return false;
        fogPortalZone_=observer.record_.zone04;f.step=2;
        at(0x280c20);if(observer.record_.method08!=h.method||!h.method)return false;
        fogPortalMethod_=static_cast<const OriginalNativeListMethodCustody50LA*>(observer.record_.method08);f.step=3;
        at(0x280c24);f.step=4; //S7=the actual collider A1.
        at(0x280c28);f.step=5; //S5=the retained center A2.
        at(0x280c2c);f.step=6; //S4=the retained half A3, not selectorS4 from the caller frame.
        at(0x280c30);f.radiusF20=fogPrimary_.publication.radiusF12;f.step=7;
        at(0x280c34);f.nativePinAttempted=true;
        const bool read=actualHead(&h,collider_.get(),fogPortalZone_,fogPortalMethod_,fogPortalHead_,fogPortalGraphPin_,pc);
        f.nativePinAcquired=bool(fogPortalGraphPin_);
        if(!read||!f.nativePinAcquired){at(pc);return false;}
        f.headNull=fogPortalHead_==nullptr;f.step=8; //Actual owned node30 load only after graph custody succeeds.
        at(0x280c38);at(0x280c3c);f.step=9; //Branch plus delay retaining actual observerS6.
        if(!*f.headNull){at(0x280c40);return false;} //Before edge08 read or231150 overlap.
        for(std::uint32_t p=0x280d08;p<=0x280d2c;p+=4)at(p);f.step=10;
        at(0x280d30);f.step=11;at(0x280d34);f.returnPC=fogPrimary_.publication.callContinuation;f.step=12;
        at(0x280bc0);return true; //Actual portal return only.280D38 is padding; primary epilogue is not reached.
    }
    template<class ActualPortal>
    bool continueFogPortalInputs280C40(ActualPortal&& actualPortal,std::uint32_t& pc)noexcept {
        if(pc!=0x280c40||fogPrimary_.portalHead.step!=9||fogPrimary_.portalHead.headNull!=false||
           fogPrimary_.portalInputs.nativeAdmissionAttempted||membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& f=fogPrimary_.portalInputs;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x280c40);f.nativeAdmissionAttempted=true;
        if(!actualPortal(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,
            fogPortalOperandA0_,fogPortalVertices_,pc)){at(pc);return false;}
        f.step=1; //Actual edge08 portal read, with retained typed geometry owner.
        at(0x280c44);fogPortalDistanceA2_=&fogPortalDistanceBacking_;f.step=2;
        at(0x280c48);f.radiusF12=fogPrimary_.portalHead.radiusF20;f.step=3;
        at(0x280c4c);f.callContinuation=0x280c54u;f.step=4;
        at(0x280c50);fogPortalCenterA1_=&boundsCenter_;f.step=5; //JAL delay: same retained center S5.
        for(unsigned k=0;k<3;++k){
            at(0x231150u+8*k);f.centerWords[k]=std::bit_cast<std::uint32_t>(fogPortalCenterA1_->v[k]);f.step=6+2*k;
            at(0x231154u+8*k);f.vertexWords[k]=(*fogPortalVertices_)[0][k];f.step=7+2*k;
        }
        at(0x231168);f.step=12; //Actual callee stack decrement, not a guest address fabrication.
        at(0x23116c);return continueFogPortalRelative23116C(pc);
    }
    bool continueFogPortalRelative23116C(std::uint32_t& pc)noexcept {
        if(pc!=0x23116c||fogPrimary_.portalInputs.step!=12||fogPrimary_.portalRelative.step||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& f=fogPrimary_.portalRelative;const auto& in=fogPrimary_.portalInputs;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x23116c);const auto x=f.difference(*in.centerWords[0],*in.vertexWords[0]);if(!x)return false;
        f.differences[0]=x;f.step=1;
        at(0x231170);f.oneV0=0x3f800000u;f.step=2;
        at(0x231174);fogPortalRelativeA3_=&fogPortalRelative00_;f.step=3;
        at(0x231178);const auto y=f.difference(*in.centerWords[1],*in.vertexWords[1]);if(!y)return false;
        f.differences[1]=y;f.step=4;
        at(0x23117c);fogPortalRelative10_[0]=*x;f.temporary10[0]=x;f.step=5;
        at(0x231180);f.normalOffset=0x70u;f.step=6; //Symbolic offset from the SAME retained portal A0; no normal read yet.
        at(0x231184);const auto z=f.difference(*in.centerWords[2],*in.vertexWords[2]);if(!z)return false;
        f.differences[2]=z;f.step=7;
        at(0x231188);fogPortalRelative10_[1]=*y;f.temporary10[1]=y;f.step=8;
        at(0x23118c);fogPortalRelative10_[2]=*z;f.temporary10[2]=z;f.step=9;
        at(0x231190);fogPortalRelative10_[3]=*f.oneV0;f.temporary10[3]=f.oneV0;f.step=10;
        at(0x231194);f.quadV0=fogPortalRelative10_;f.step=11;
        at(0x231198);*fogPortalRelativeA3_=*f.quadV0;f.local00=*f.quadV0;f.step=12;
        at(0x23119c);f.vf4=*fogPortalRelativeA3_;f.step=13;
        at(0x2311a0);return false; //Before normal LQC2/VU dot; output distance and all enclosing returns remain unknown.
    }
    template<class ActualNormal>
    bool continueFogPortalDot2311A0(ActualNormal&& actualNormal,std::uint32_t& pc)noexcept {
        if(pc!=0x2311a0||fogPrimary_.portalRelative.step!=13||fogPrimary_.portalDot.nativeAdmissionAttempted||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& f=fogPrimary_.portalDot;const auto& relative=*fogPrimary_.portalRelative.vf4;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x2311a0);f.nativeAdmissionAttempted=true;
        if(!actualNormal(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,fogPortalNormal_,pc)){at(pc);return false;}
        for(unsigned k=0;k<3;++k)f.normalVF5[k]=(*fogPortalNormal_)[k];f.step=1; //XYZ known; W stays unknown, never read as known.
        at(0x2311a4);if(!f.axisNormal())return false;
        std::array<std::uint32_t,3> products;
        for(unsigned k=0;k<3;++k){const auto product=f.unitProduct(relative[k],*f.normalVF5[k]);if(!product)return false;products[k]=*product;}
        for(unsigned k=0;k<3;++k)f.productsVF5[k]=products[k];f.step=2;
        at(0x2311a8);f.oneVF6=0x3f800000u;f.step=3; //VADDw.x VF6,VF0,VF0; other VF6 lanes remain unknown.
        at(0x2311ac);const auto acc=f.zeroSum(products[0],products[1]);if(!acc)return false;f.accX=acc;f.step=4;
        at(0x2311b0);const auto product=f.unitProduct(*f.oneVF6,products[2]);if(!product)return false;
        const auto sum=f.zeroSum(*f.accX,*product);if(!sum)return false;f.sumVF5X=sum;f.step=5;
        at(0x2311b4);f.gprV0[0]=sum;f.gprV0[1]=products[1];f.gprV0[2]=products[2];f.step=6; //Full QMFC2 retains unknown W.
        at(0x2311b8);f.scalarF1=f.gprV0[0];f.step=7; //MTC1 consumes only the known low word.
        at(0x2311bc);if(fogPortalDistanceA2_!=&fogPortalDistanceBacking_)return false;
        f.outputNonNull=true;f.step=8;at(0x2311c0);f.step=9; //Actual nonNULL caller output address; branch not taken, delay still executes.
        at(0x2311c4);fogPortalTemporaryV0_=&fogPortalTemporary3C_;f.step=10;
        at(0x2311c8);f.absoluteF8=*f.scalarF1&0x7fffffffu;f.step=11;
        at(0x2311cc);*fogPortalTemporaryV0_=*f.absoluteF8;f.temporary3C=f.absoluteF8;f.step=12;
        at(0x2311d0);f.reloadF0=fogPortalTemporary3C_;f.step=13;
        at(0x2311d4);*fogPortalDistanceA2_=*f.reloadF0;f.outputDistance=f.reloadF0;f.step=14;
        at(0x2311d8);return continueFogPortalSlab2311D8(pc);
    }
    bool continueFogPortalSlab2311D8(std::uint32_t& pc)noexcept {
        if(pc!=0x2311d8||fogPrimary_.portalDot.step!=14||fogPrimary_.portalSlab.prefixStep||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& f=fogPrimary_.portalSlab;
        const auto distance=*fogPrimary_.portalDot.scalarF1; //Signed F1, never the ABS output.
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x2311d8);const auto le=f.lessEqualZero(distance);if(!le||fogPrimary_.portalInputs.radiusF12!=0u)return false;
        f.firstCondition=f.conditionBit=le;f.prefixStep=1;
        at(0x2311dc);f.prefixStep=2;at(0x2311e0);f.prefixStep=3;
        at(0x2311e4);f.delayV0=decltype(f.delayV0)::value_type{};f.prefixStep=4; //Ordinary BC1F: POR V0,zero,zero delay always executes.
        if(*le){
            at(0x2311e8);f.negativeRadius=*fogPrimary_.portalInputs.radiusF12^0x80000000u;f.negativeStep=1;
            at(0x2311ec);const auto lt=f.lessNegativeZero(distance);if(!lt)return false;
            f.secondCondition=f.conditionBit=lt;f.negativeStep=2;
            at(0x2311f0);f.negativeStep=3;at(0x2311f4);f.negativeStep=4;
            at(0x2311f8);fogPortalEdgeRelativeT0_=&fogPortalRelative00_;f.negativeStep=5; //Ordinary BC1F delay, on both outcomes.
            if(!*lt){at(0x23120c);return false;} //Either signed zero: before first edge setup, no hit/miss result.
            at(0x2311fc);f.secondZeroV0=decltype(f.secondZeroV0)::value_type{}; //Negative nonzero miss, before the common branch.
        }
        at(0x231200);f.missStep=1;
        at(0x231204);f.missStep=2; //Unconditional branch delay restores SP by0x40 before JR.
        at(0x2312f4);f.missStep=3;
        at(0x2312f8);f.returnV0=f.secondZeroV0?f.secondZeroV0:f.delayV0;
        f.returnPC=fogPrimary_.portalInputs.callContinuation;f.missStep=4; //Actual JR delay; no padding-as-return shortcut.
        at(0x280c54);if(f.returnV0!=decltype(f.returnV0)::value_type{}||f.returnPC!=0x280c54u)return false;
        f.callerBranchTaken=true;f.missStep=5;
        at(0x280c58);f.callerTarget=0x280cf8u;f.missStep=6;
        at(0x280cf8);return false; //Before next-link LW; no edge distance store, traversal, portal or primary return.
    }
    template<class ActualEdge>
    bool continueFogPortalEdge023120C(ActualEdge&& actualEdge,std::uint32_t& pc)noexcept {
        if(pc!=0x23120c||fogPrimary_.portalSlab.negativeStep!=5||fogPrimary_.portalSlab.secondCondition!=false||
           fogPrimary_.portalSlab.missStep||fogPrimary_.portalEdge0.step||membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& f=fogPrimary_.portalEdge0;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x23120c);f.edgeOffset=0x80u;f.step=1; //Symbolic offset from same actual portal A0.
        at(0x231210);if(fogPortalEdgeRelativeT0_!=&fogPortalRelative00_)return false;
        f.relativeVF4=*fogPortalEdgeRelativeT0_;f.step=2;
        at(0x231214);f.nativeAdmissionAttempted=true;
        if(!actualEdge(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,fogPortalFirstEdge_,pc)){at(pc);return false;}
        for(unsigned k=0;k<3;++k)f.edgeVF5[k]=(*fogPortalFirstEdge_)[k];f.step=3; //W remains unknown.
        at(0x231218);if(!f.axisEdge())return false;std::array<std::uint32_t,3> products;
        for(unsigned k=0;k<3;++k){const auto product=decltype(fogPrimary_.portalDot)::unitProduct((*f.relativeVF4)[k],*f.edgeVF5[k]);
            if(!product)return false;products[k]=*product;}
        for(unsigned k=0;k<3;++k)f.productsVF5[k]=products[k];f.step=4;
        at(0x23121c);f.oneVF6=0x3f800000u;f.step=5;
        at(0x231220);const auto acc=decltype(fogPrimary_.portalDot)::zeroSum(products[0],products[1]);if(!acc)return false;f.accX=acc;f.step=6;
        at(0x231224);const auto product=decltype(fogPrimary_.portalDot)::unitProduct(*f.oneVF6,products[2]);if(!product)return false;
        const auto sum=decltype(fogPrimary_.portalDot)::zeroSum(*f.accX,*product);if(!sum)return false;f.sumVF5X=sum;f.step=7;
        at(0x231228);f.gprV0[0]=sum;f.gprV0[1]=products[1];f.gprV0[2]=products[2];f.step=8;
        at(0x23122c);f.scalarF0=f.gprV0[0];f.step=9;
        at(0x231230);f.step=10;at(0x231234);
        const auto le=decltype(fogPrimary_.portalSlab)::lessEqualZero(*f.scalarF0);if(!le||fogPrimary_.portalInputs.radiusF12!=0u)return false;
        f.conditionBit=le;f.step=11;at(0x231238);f.step=12;at(0x23123c);f.step=13;
        at(0x231240);f.counterA2=1u;fogPortalDistanceA2_=nullptr;f.step=14; //Ordinary BC1T delay replaces output pointer with low64 counter1.
        if(*le){
            at(0x231250);f.advanceStep=1;
            at(0x231254);f.oneV1=0x3f800000u;f.advanceStep=2;
            at(0x2312e0);f.loopTestV0=1u;f.advanceStep=3; //Signed1<4; SLTI writes known low64.
            at(0x2312e4);f.advanceStep=4;
            at(0x2312e8);f.loopOffsetV0=16u;f.target=0x23125cu;f.advanceStep=5; //SLL delay;231258 duplicate SLL is skipped.
            at(0x23125c);return false; //Before ADDU forms vertex1 address or any fresh vertex loads.
        }
        at(0x231244);f.missStep=1;
        at(0x231248);f.zeroV0=decltype(f.zeroV0)::value_type{};f.missStep=2;
        at(0x2312f0);f.missStep=3; //This distinct miss route restores SP here, not at231204.
        at(0x2312f4);f.missStep=4;
        at(0x2312f8);f.returnV0=f.zeroV0;f.returnPC=fogPrimary_.portalInputs.callContinuation;f.missStep=5;
        at(0x280c54);if(f.returnV0!=decltype(f.returnV0)::value_type{}||f.returnPC!=0x280c54u)return false;
        f.callerBranchTaken=true;f.missStep=6;
        at(0x280c58);f.target=0x280cf8u;f.missStep=7;
        at(0x280cf8);return false; //Before next-link read; no portal/primary return or hit-path distance stores.
    }
    template<class ActualLoopOperand>
    bool continueFogPortalLoop23125C(ActualLoopOperand&& actualOperand,std::uint32_t& pc)noexcept {
        auto& f=fogPrimary_.portalLoop;auto& edge0=fogPrimary_.portalEdge0;auto& h=servicePrimary_;
        if(pc!=0x23125c||edge0.advanceStep!=5||edge0.target!=0x23125cu||f.iterations[0].addressAttempted||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        const OriginalTriggerFogPrimaryPrefix50LA::PortalLoop::Iteration* terminal=nullptr;
        for(unsigned n=0;n<3;++n){auto& it=f.iterations[n];const unsigned index=n+1;
            const auto counter=n?f.iterations[n-1].counterOut:edge0.counterA2;
            const auto offset=n?f.iterations[n-1].loopOffsetV0:edge0.loopOffsetV0;
            at(0x23125c);if(!counter||*counter!=index||!offset||*offset!=16u*index||
                (n&&(f.iterations[n-1].advanceStep!=3||f.iterations[n-1].target!=0x23125cu)))return false;
            it.addressAttempted=true;
            const C* portal=nullptr;const FogPortalVertices* vertices=nullptr;const std::array<std::uint32_t,4>* edge=nullptr;
            if(!actualOperand(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,index,1u,portal,vertices,edge,pc)){at(pc);return false;}
            if(portal!=fogPortalOperandA0_||vertices!=fogPortalVertices_||!vertices||edge)return false;
            fogPortalLoopVertices_[n]=&(*vertices)[index];it.counterIn=counter;it.vertexOffset=offset;it.step=1;
            at(0x231260);if(fogPortalCenterA1_!=&boundsCenter_)return false;
            it.centerWords[0]=std::bit_cast<std::uint32_t>(fogPortalCenterA1_->v[0]);it.step=2;
            at(0x231264);it.vertexWords[0]=(*fogPortalLoopVertices_[n])[0];it.step=3;
            at(0x231268);it.vertexWords[1]=(*fogPortalLoopVertices_[n])[1];it.step=4;
            at(0x23126c);it.centerWords[1]=std::bit_cast<std::uint32_t>(fogPortalCenterA1_->v[1]);it.step=5;
            at(0x231270);it.centerWords[2]=std::bit_cast<std::uint32_t>(fogPortalCenterA1_->v[2]);it.step=6;
            at(0x231274);it.vertexWords[2]=(*fogPortalLoopVertices_[n])[2];it.step=7;
            at(0x231278);it.edgeAddressAttempted=true;portal=nullptr;vertices=nullptr;
            if(!actualOperand(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,index,8u,portal,vertices,edge,pc)){at(pc);return false;}
            if(portal!=fogPortalOperandA0_||vertices!=fogPortalVertices_||!edge)return false;
            fogPortalLoopEdges_[n]=edge;fogPortalRelativeA3_=nullptr;it.edgeOffset=0x80u+16u*index;it.step=8;
            at(0x23127c);auto result=decltype(fogPrimary_.portalRelative)::difference(*it.centerWords[0],*it.vertexWords[0]);
            if(!result)return false;it.differences[0]=result;it.step=9;
            at(0x231280);result=decltype(fogPrimary_.portalRelative)::difference(*it.centerWords[1],*it.vertexWords[1]);
            if(!result)return false;it.differences[1]=result;it.step=10;
            at(0x231284);fogPortalRelative20_[0]=*it.differences[0];it.temporary20[0]=it.differences[0];it.step=11;
            at(0x231288);result=decltype(fogPrimary_.portalRelative)::difference(*it.centerWords[2],*it.vertexWords[2]);
            if(!result)return false;it.differences[2]=result;it.step=12;
            at(0x23128c);fogPortalRelative20_[1]=*it.differences[1];it.temporary20[1]=it.differences[1];it.step=13;
            at(0x231290);fogPortalRelative20_[2]=*it.differences[2];it.temporary20[2]=it.differences[2];it.step=14;
            at(0x231294);if(edge0.oneV1!=0x3f800000u)return false;
            fogPortalRelative20_[3]=static_cast<std::uint32_t>(*edge0.oneV1);it.temporary20[3]=fogPortalRelative20_[3];it.step=15;
            at(0x231298);it.quadV0=fogPortalRelative20_;it.step=16;
            at(0x23129c);fogPortalRelative00_=*it.quadV0;it.local00=fogPortalRelative00_;it.step=17;
            at(0x2312a0);if(fogPortalEdgeRelativeT0_!=&fogPortalRelative00_)return false;
            it.relativeVF4=*fogPortalEdgeRelativeT0_;it.step=18;
            at(0x2312a4);it.edgeLoadAttempted=true;portal=nullptr;vertices=nullptr;edge=nullptr;
            if(!actualOperand(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,index,19u,portal,vertices,edge,pc)){at(pc);return false;}
            if(portal!=fogPortalOperandA0_||vertices!=fogPortalVertices_||edge!=fogPortalLoopEdges_[n])return false;
            for(unsigned k=0;k<3;++k)it.edgeVF5[k]=(*edge)[k];it.step=19;
            at(0x2312a8);if(!it.axisEdge())return false;std::array<std::uint32_t,3> products;
            for(unsigned k=0;k<3;++k){const auto p=decltype(fogPrimary_.portalDot)::unitProduct((*it.relativeVF4)[k],*it.edgeVF5[k]);
                if(!p)return false;products[k]=*p;}
            for(unsigned k=0;k<3;++k)it.productsVF5[k]=products[k];it.step=20;
            at(0x2312ac);it.oneVF6=0x3f800000u;it.step=21;
            at(0x2312b0);const auto acc=decltype(fogPrimary_.portalDot)::zeroSum(products[0],products[1]);if(!acc)return false;it.accX=acc;it.step=22;
            at(0x2312b4);const auto p=decltype(fogPrimary_.portalDot)::unitProduct(*it.oneVF6,products[2]);if(!p)return false;
            const auto sum=decltype(fogPrimary_.portalDot)::zeroSum(*it.accX,*p);if(!sum)return false;it.sumVF5X=sum;it.step=23;
            at(0x2312b8);it.gprV0[0]=sum;it.gprV0[1]=products[1];it.gprV0[2]=products[2];it.step=24;
            at(0x2312bc);it.scalarF0=it.gprV0[0];it.step=25;at(0x2312c0);it.step=26;
            at(0x2312c4);const auto le=decltype(fogPrimary_.portalSlab)::lessEqualZero(*it.scalarF0);
            if(!le||fogPrimary_.portalInputs.radiusF12!=0u)return false;it.conditionBit=le;it.step=27;
            at(0x2312c8);it.step=28;at(0x2312cc);it.step=29;
            if(!*le){ //BC1TL annuls2312D0: counter remains index, never index+1.
                at(0x2312d4);it.missStep=1;at(0x2312d8);it.zeroV0=decltype(it.zeroV0)::value_type{};
                it.target=0x2312f0u;it.missStep=2;terminal=&it;break;
            }
            at(0x2312d0);it.counterOut=*it.counterIn+1u;it.step=30;
            at(0x2312e0);it.loopTestV0=*it.counterOut<4u?1u:0u;it.advanceStep=1;
            at(0x2312e4);it.loopBranchTaken=*it.loopTestV0!=0u;it.advanceStep=2;
            at(0x2312e8);it.loopOffsetV0=*it.counterOut<<4;it.target=*it.loopBranchTaken?0x23125cu:0x2312ecu;it.advanceStep=3;
            if(!*it.loopBranchTaken){terminal=&it;break;} //Ordinary BNE executes its delay even on final fallthrough.
        }
        if(!terminal)return false;const bool hit=terminal==&f.iterations[2]&&terminal->advanceStep==3;
        if(hit){at(0x2312ec);f.successLow64=1u;f.successStep=1;} //ADDIU changes low64 only; final QMFC2 upper64 survives.
        at(0x2312f0);f.returnStep=1;at(0x2312f4);f.returnStep=2;
        at(0x2312f8);f.returnLow64=hit?1u:0u;f.returnUpper64[0]=hit?terminal->gprV0[2]:std::optional<std::uint32_t>(0u);
        if(!hit)f.returnUpper64[1]=0u;f.returnPC=fogPrimary_.portalInputs.callContinuation;f.returnStep=3;
        at(0x280c54);if(f.returnPC!=0x280c54u)return false;f.callerBranchTaken=*f.returnLow64==0u;f.returnStep=4;
        at(0x280c58);f.target=*f.callerBranchTaken?0x280cf8u:0x280c5cu;f.returnStep=5;
        at(*f.target);return false; //Before first hit-distance reload or next-link LW; no enclosing registration return.
    }
    template<class ActualHitStore>
    bool continueFogPortalHit280C5C(ActualHitStore&& actualStore,std::uint32_t& pc)noexcept {
        auto& f=fogPrimary_.portalHit;auto& h=servicePrimary_;
        if(pc!=0x280c5c||fogPrimary_.portalLoop.returnStep!=5||fogPrimary_.portalLoop.target!=0x280c5cu||f.step||
           f.storeAttemptMask||membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x280c5c);if(!fogPrimary_.portalDot.outputDistance||fogPortalDistanceBacking_!=*fogPrimary_.portalDot.outputDistance)return false;
        f.reloadF0=fogPortalDistanceBacking_;f.step=1; //Caller sp+AC backing, not any edge-dot result or retired A2 alias.
        at(0x280c60);f.storeAttemptMask|=1u;
        if(!actualStore(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,1u,*f.reloadF0,pc)){at(pc);return false;}
        f.stored10=f.reloadF0;f.step=2;
        at(0x280c64);f.storeAttemptMask|=2u;
        if(!actualStore(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,2u,*f.reloadF0,pc)){at(pc);return false;}
        f.stored0C=f.reloadF0;f.step=3;
        at(0x280c68);return false; //Before neighbor04 load, method lookup, secondary selection or publication.
    }
    template<class ActualNext>
    bool continueFogPortalNext280CF8(ActualNext&& actualNext,std::uint32_t& pc)noexcept {
        auto& f=fogPrimary_.portalNext;auto& h=servicePrimary_;
        if(pc!=0x280cf8||!fogPrimary_.portalMissAt280CF8()||f.nativeAdmissionAttempted||fogPrimary_.portalHit.step||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x280cf8);f.nativeAdmissionAttempted=true;const void* next=nullptr;
        if(!actualNext(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,next,pc)){at(pc);return false;}
        fogPortalNextCursor_=next;f.nextNonNull=bool(next);f.step=1; //S1 changes; historical initial head does not.
        at(0x280cfc);f.branchTaken=f.nextNonNull;f.step=2;
        at(0x280d00);f.branchTarget=*f.branchTaken?0x280c40u:0x280d04u;f.step=3;
        if(*f.branchTaken){at(0x280c40);return false;} //Before fresh next-portal08 read or a new helper occurrence.
        at(0x280d04);f.step=4; //Real fallthrough NOP, not padding after return.
        //Saved RA,S7..S0,F20 restored in original order; do not fabricate register values.
        for(std::uint32_t p=0x280d08;p<=0x280d2c;p+=4)at(p);f.step=5;
        at(0x280d30);f.step=6;
        at(0x280d34);f.returnPC=fogPrimary_.publication.callContinuation;f.step=7; //JR delay restores SP byB0.
        at(*f.returnPC);return false; //Actual portal helper return280BC0, before the primary epilogue.
    }
    template<class ActualNext,class ActualPortal>
    bool continueFogPortalHitNext280CF8(ActualNext&& actualNext,ActualPortal&& actualPortal,std::uint32_t& pc)noexcept {
        auto& f=fogPrimary_.portalHitNext;auto& h=servicePrimary_;const auto& payload=fogPrimary_.portalPoolPayload;
        if(pc!=0x280cf8u||payload.step!=9||payload.stores!=63u||payload.boundaryPC!=pc||
           f.nativeAdmissionAttempted||fogPrimary_.portalNext.nativeAdmissionAttempted||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x280cf8);f.nativeAdmissionAttempted=true;const void* next=nullptr;
        if(!actualNext(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,next,pc)){at(pc);return false;}
        fogPortalHitNextCursor_=next;f.nextNonNull=bool(next);f.step=1;
        at(0x280cfc);f.branchTaken=f.nextNonNull;f.step=2;
        at(0x280d00);f.branchTarget=*f.branchTaken?0x280c40u:0x280d04u;f.step=3; //Ordinary NOP delay on both paths.
        if(*f.branchTaken){
            at(0x280c40);f.portalReadAttempted=true;const C* portal=nullptr;
            if(!actualPortal(&h,collider_.get(),fogPortalZone_,fogPortalHitNextCursor_,fogPortalGraphPin_,portal,pc)){at(pc);return false;}
            if(!portal)return false;fogPortalNextPortalA0_=portal;f.portalNonNull=true;f.portalReadCompleted=true;
            f.inputBoundary=0x280c44u;at(*f.inputBoundary);return false; //Only a new portal08 LW. No old helper/backing/history reuse.
        }
        at(0x280d04);f.step=4;
        for(std::uint32_t p=0x280d08;p<=0x280d2c;p+=4)at(p);f.step=5; //Saved RA,S7..S0,F20 restored; no fabricated register values.
        at(0x280d30);f.step=6;
        at(0x280d34);f.returnPC=fogPrimary_.publication.callContinuation;f.step=7;
        at(*f.returnPC);return false; //GenuineNULL helper return280BC0 only; first published record and native owner custody remain.
    }
    template<class ActualInputs>
    bool continueFogPortalNextInputs280C44(ActualInputs&& actualInputs,std::uint32_t& pc)noexcept {
        auto& f=fogPrimary_.portalNextInputs;auto& h=servicePrimary_;const auto& next=fogPrimary_.portalHitNext;
        if(pc!=0x280c44u||!next.portalReadCompleted||next.inputBoundary!=pc||f.nativeAdmissionAttempted||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x280c44);f.nativeAdmissionAttempted=true;
        const std::array<std::array<std::uint32_t,4>,4>* vertices=nullptr;
        if(!actualInputs(&h,collider_.get(),fogPortalZone_,fogPortalHitNextCursor_,fogPortalGraphPin_,
            fogPortalNextPortalA0_,vertices,pc)){at(pc);return false;}
        if(!vertices)return false;fogPortalNextVertices_=vertices;f.nativeInputsAcquired=true; //Native backing admission, not a source vertex load.
        at(0x280c44);fogPortalNextDistanceA2_=&fogPortalDistanceBacking_;f.step=1;
        at(0x280c48);f.radiusF12=fogPrimary_.portalHead.radiusF20;f.step=2; //Fresh MOV.S from the retained outer F20.
        at(0x280c4c);f.callTarget=0x231150u;f.callContinuation=0x280c54u;f.step=3;
        at(0x280c50);fogPortalNextCenterA1_=&boundsCenter_;f.step=4; //JAL delay, actual retained S5 center.
        for(unsigned k=0;k<3;++k){
            at(0x231150u+8*k);f.centerWords[k]=std::bit_cast<std::uint32_t>(fogPortalNextCenterA1_->v[k]);f.step=5+2*k;
            at(0x231154u+8*k);f.vertexWords[k]=(*fogPortalNextVertices_)[0][k];f.step=6+2*k;
        }
        at(0x231168);f.stackBytes=0x40u;f.step=11; //Same caller sp+AC is now callee sp+EC; no native stack address or value fabricated.
        f.boundaryPC=0x23116cu;at(*f.boundaryPC);return false; //Before this occurrence's first SUB.S; no first relative/dot/helper history reuse.
    }
    template<class ActualOperand>
    bool continueFogPortalNextQuery23116C(ActualOperand&& actualOperand,std::uint32_t& pc)noexcept {
        auto& q=fogPrimary_.portalNextQuery;
        if(pc!=0x23116cu||!fogPrimary_.portalNextInputs.helperEntered()||q.entered||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;
        const auto read=[&](unsigned index,unsigned stage,const C*& portal,const FogPortalVertices*& vertices,
            const std::array<std::uint32_t,4>*& operand,std::uint32_t& reachedPC)noexcept {
            pc_=h.pc=reachedPC;
            return actualOperand(&h,collider_.get(),fogPortalZone_,fogPortalHitNextCursor_,fogPortalGraphPin_,
                index,stage,portal,vertices,operand,reachedPC);
        };
        const bool returned=fogPortalNextQuery_.run(q,fogPrimary_.portalNextInputs,fogPortalNextPortalA0_,
            fogPortalNextVertices_,fogPortalNextCenterA1_,fogPortalNextDistanceA2_,read,pc);
        pc_=h.pc=pc;
        if(q.portalEdge0.step>=14)fogPortalNextDistanceA2_=nullptr; //Actual231240 replaces A2 with integer1.
        return returned; //Only the query/caller branch, never second publication or enclosing registration.
    }
    template<class ActualPortal,class ActualNext,class ActualOperand>
    bool continueFogPortalMissTailNative(ActualPortal&& actualPortal,ActualNext&& actualNext,
        ActualOperand&& actualOperand,std::uint32_t& pc)noexcept {
        const bool first=pc==0x280c40u&&fogPrimary_.portalNext.step==3&&fogPrimary_.portalNext.branchTarget==pc;
        const bool second=pc==0x280cf8u&&fogPrimary_.portalNextQuery.returned()&&fogPrimary_.portalNextQuery.pc==pc;
        if((!first&&!second)||fogPrimary_.portalMissTail.entered||membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;
        const auto sync=[&](std::uint32_t p)noexcept{pc_=h.pc=p;};
        const auto portal=[&](const void* cursor,const C*& p,const FogPortalVertices*& v,std::uint32_t& at)noexcept{
            sync(at);return actualPortal(&h,collider_.get(),fogPortalZone_,cursor,fogPortalGraphPin_,p,v,at);
        };
        const auto next=[&](const void* cursor,const void*& n,std::uint32_t& at)noexcept{
            sync(at);return actualNext(&h,collider_.get(),fogPortalZone_,cursor,fogPortalGraphPin_,n,at);
        };
        const auto operand=[&](const void* cursor,unsigned index,unsigned stage,const C*& p,
            const FogPortalVertices*& v,const std::array<std::uint32_t,4>*& word,std::uint32_t& at)noexcept{
            sync(at);return actualOperand(&h,collider_.get(),fogPortalZone_,cursor,fogPortalGraphPin_,index,stage,p,v,word,at);
        };
        const bool returned=fogPortalMissTail_.run(fogPrimary_.portalMissTail,second,fogPortalHead_,fogPortalNextCursor_,
            fogPortalHitNextCursor_,&boundsCenter_,&fogPortalDistanceBacking_,*fogPrimary_.portalHead.radiusF20,
            *fogPrimary_.publication.callContinuation,portal,next,operand,sync,pc);
        sync(pc);return returned; //Only genuine NULL helper return, never primary/registration completion or a later hit publication.
    }
    template<class ActualNeighbor>
    bool continueFogPortalNeighbor280C68(ActualNeighbor&& actualNeighbor,std::uint32_t& pc)noexcept {
        auto& f=fogPrimary_.portalNeighbor;auto& h=servicePrimary_;
        if(pc!=0x280c68||fogPrimary_.portalHit.step!=3||f.neighborAdmissionAttempted||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        const void* node=nullptr;const C* neighbor=nullptr;OriginalNativeListMethodCustody50LA* method=nullptr;
        at(0x280c68);f.neighborAdmissionAttempted=true;
        if(!actualNeighbor(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,1u,node,neighbor,method,pc)){at(pc);return false;}
        if(!node||!neighbor||method)return false;fogPortalNeighborNode_=node;fogPortalNeighborS0_=neighbor;f.step=1;
        at(0x280c6c);if(!fogPortalMethod_||fogPortalMethod_!=h.method||!fogPortalMethod_->source_.usageKnown||fogPortalMethod_->usage10_!=1u)return false;
        f.usage10=fogPortalMethod_->usage10_;f.step=2; //Actual current S2 usage read, not neighbor usage or a fabricated mode.
        at(0x280c70);f.lookupContinuation=0x280c78u;f.step=3;
        at(0x280c74);f.step=4; //POR A0,S0: exact retained neighbor Entity identity.
        at(0x27f590);f.usageBit1V0=*f.usage10&1u;f.step=5;
        at(0x27f594);f.lookupBranchTaken=*f.usageBit1V0==0u;f.step=6;
        at(0x27f598);f.usageBit2V0=*f.usage10&2u;f.step=7; //Ordinary branch delay runs even though mode1 branch is not taken.
        at(0x27f59c);f.step=8;
        at(0x27f5a0);f.methodAdmissionAttempted=true;node=nullptr;neighbor=nullptr;
        if(!actualNeighbor(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,9u,node,neighbor,method,pc)){at(pc);return false;}
        if(node!=fogPortalNeighborNode_||neighbor!=fogPortalNeighborS0_||!method||method==h.method)return false;
        fogPortalNeighborMethod_=method;f.step=9; //LW neighbor34 in the unconditional branch delay; never node38.
        at(0x27f5bc);f.step=10;at(0x27f5c0);f.lookupReturnPC=f.lookupContinuation;f.step=11;
        at(0x280c78);f.nativePinAttempted=true;
        for(const auto* pin:method->sourceCollisionRegistrationBorrowers_)if(pin==&fogPortalNeighborPinIdentity_)return false;
        try{method->sourceCollisionRegistrationBorrowers_.push_back(&fogPortalNeighborPinIdentity_);}catch(...){return false;}
        f.nativePinAcquired=true; //Native lifetime only; allocation refusal retains the authentic method return, before vtable load.
        if(method->source_.vtable08!=0x3d2d30u)return false;f.table08=method->source_.vtable08;f.step=12;
        at(0x280c7c);fogPortalCurrentMethodS2_=method;f.step=13; //Original primary method/head history remains unchanged.
        at(0x280c80);fogPortalSelectA0_=fogPortalCurrentMethodS2_;f.step=14;
        at(0x280c84);fogPortalSelectCenterA1_=&boundsCenter_;f.step=15;
        at(0x280c88);fogPortalSelectHalfA2_=&boundsHalf_;f.step=16;
        at(0x280c8c);f.target20=0x27e3b0u;f.step=17; //Actual source table word3D2D50.
        at(0x280c90);f.callContinuation=0x280c98u;f.step=18;
        at(0x280c94);f.preferredA3=decltype(f.preferredA3)::value_type{};f.step=19; //POR zero supplies all128 bits.
        at(0x27e3b0);f.shimTarget=0x27e3c0u;f.step=20;
        at(0x27e3b4);f.preferredA1=f.preferredA3;fogPortalSelectCenterA1_=nullptr;f.step=21; //J delay overwrites A1, not the retained center S5.
        at(0x27e3c0);return false; //Before fresh neighbor selection; no slot, pending reservation, S3 or return280C98.
    }
    bool continueFogPortalSelector27E3C0(std::uint32_t& pc)noexcept {
        auto& f=fogPrimary_.portalSelector;const auto& neighbor=fogPrimary_.portalNeighbor;auto& h=servicePrimary_;
        if(pc!=0x27e3c0||neighbor.step!=21||neighbor.preferredA1!=decltype(neighbor.preferredA1)::value_type{}||
           f.nativeReservationAttempted||membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        if(!fogPortalNeighborMethod_||fogPortalSelectA0_!=fogPortalNeighborMethod_||!neighbor.nativePinAcquired||fogPortalNeighborUnpinned_)return false;
        auto& c=*fogPortalNeighborMethod_;auto& e=c.sourceEntries_;auto& s=c.source_;
        f.nativeReservationAttempted=true;
        if(!c.sourceCollisionEntriesCurrentNative()||e.pending||e.terminal||!c.sourceMode_||c.application_!=application_||
           c.world_!=h.world||c.usage10_!=1u||c.reference00_!=2u||c.ordinal_/10u!=1u)return false;
        //Separate native reservation protects this neighbor's actual backing. It is not a source slot/result or insertion.
        e.entered=true;e.pending=&fogPortalNeighborPinIdentity_;e.pendingIdentity=collider_.get();f.nativeReservationAcquired=true;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        for(std::uint32_t p=0x27e3c0;p<=0x27e3cc;p+=4)at(p);f.step=1;
        at(0x27e3d0);at(0x27e3d4);f.step=2;
        at(0x27e3e0);if(!s.field40||*s.field40)return false; //A native free ordinal is not authentic guest entry-address custody.
        f.freeHeadNull=true;f.step=3;at(0x27e3e4);at(0x27e3e8);
        at(0x27e3f4);if(!s.lastUsed38)return false;f.lastUsed38=s.lastUsed38;f.step=4;
        at(0x27e3f8);if(!s.perBlock30)return false;f.perBlock30=s.perBlock30;f.step=5;
        at(0x27e3fc);if(f.perBlock30!=1024u)return false;
        at(0x27e400);f.appendBranch=*f.lastUsed38!=*f.perBlock30;f.step=6; //Original inequality, not an invented less-than branch.
        if(!*f.appendBranch){
            at(0x27e404);if(!s.blockCount34)return false;f.blockCount34=s.blockCount34;f.step=7;
            at(0x27e408);if(!s.pointer3C||*s.pointer3C!=e.table.get())return false;
            fogPortalSelectorTable_=e.table.get();f.tableNonNull=bool(fogPortalSelectorTable_);f.step=8;
            at(0x27e40c);f.newBlockCount=*f.blockCount34+1u;f.step=9;
            at(0x27e410);return false; //Before original allocator JAL and its size delay; no new[] or ready-block substitution.
        }
        at(0x27e534);if(!s.blockCount34)return false;f.blockCount34=s.blockCount34;f.step=10;
        at(0x27e538);if(f.blockCount34!=1u||!e.ready||!e.tablePublished||!e.blockPublished||!e.table||!e.words||
            !s.pointer3C||*s.pointer3C!=e.table.get()||e.table[0]!=e.words.get())return false;
        fogPortalSelectorTable_=e.table.get();f.tableNonNull=true;f.step=11;
        at(0x27e53c);f.freshLastUsed38=s.lastUsed38;f.step=12;
        if(!f.freshLastUsed38||*f.freshLastUsed38>=1024u||f.freshLastUsed38!=f.lastUsed38)return false;
        at(0x27e540);f.tableOffsetBytes=*f.blockCount34<<2;f.step=13;
        at(0x27e544);fogPortalSelectorTableEnd_=fogPortalSelectorTable_+*f.blockCount34;f.step=14;
        at(0x27e548);f.wordOffsetBytes=*f.freshLastUsed38<<2;f.step=15;
        at(0x27e54c);fogPortalSelectorBlock_=fogPortalSelectorTableEnd_[-1];f.blockRead=true;f.step=16;
        at(0x27e550);const auto index=*f.freshLastUsed38;
        if(fogPortalSelectorBlock_!=e.words.get()||e.members[index]||e.borrowers[index]||e.wordKnown[index])return false;
        fogPortalSelectorEntry_=fogPortalSelectorBlock_+index;f.nativeOrdinal=index+1u;f.entryAliasResolved=true;f.step=17;
        e.pendingSlot=*f.nativeOrdinal; //Native sidecar index only; source V0 is the separately held typed address, not this ordinal.
        at(0x27e554);at(0x27e558);at(0x27e55c);f.step=18;at(0x27e560);f.step=19;
        at(0x27e564);f.returnPC=neighbor.callContinuation;f.step=20;
        at(0x280c98);fogPortalSelectorS3_=fogPortalSelectorEntry_;f.s3Retained=true;f.selectedNonNull=bool(fogPortalSelectorS3_);f.step=21;
        at(0x280c9c);f.callerBranchTaken=!*f.selectedNonNull;f.step=22;
        at(0x280ca0);f.callerTarget=*f.callerBranchTaken?0x280cf8u:0x280ca4u;f.step=23;
        at(*f.callerTarget);return false; //Before secondary insertion-call setup; no source counter/member/pool/observer effects.
    }
    bool continueFogPortalInsertion280CA4(std::uint32_t& pc)noexcept {
        const auto& selected=fogPrimary_.portalSelector;auto& call=fogPrimary_.portalEntryCall;auto& f=fogPrimary_.portalInsertion;
        if(pc!=0x280ca4||selected.step!=23||selected.callerTarget!=pc||call.step||f.step||
           !fogPortalSelectorS3_||membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& c=*fogPortalNeighborMethod_;auto& e=c.sourceEntries_;auto& s=c.source_;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x280ca4);if(s.vtable08!=0x3d2d30u)return false;call.table08=s.vtable08;call.step=1;
        at(0x280ca8);fogPortalInsertionA0_=fogPortalCurrentMethodS2_;call.step=2;
        at(0x280cac);fogPortalInsertionA1_=collider_.get();call.step=3; //Retained actual collider S7.
        at(0x280cb0);fogPortalInsertionA2_=fogPortalSelectorS3_;call.step=4;
        at(0x280cb4);fogPortalInsertionA3_=&boundsCenter_;call.step=5; //Retained center S5, no new vector load.
        at(0x280cb8);call.target24=0x27e570u;call.step=6; //Actual source table word3D2D54.
        at(0x280cbc);call.callContinuation=0x280cc4u;call.step=7;
        at(0x280cc0);fogPortalInsertionT0_=&boundsHalf_;call.step=8; //JALR delay retains half S4, not the primary caller's slot S4.
        at(0x27e570);if(s.field40!=0u)return false;f.freeHead40=s.field40;f.step=1;
        at(0x27e574);at(0x27e578);f.step=2; //BEQ taken, ordinary NOP delay, skip nonzero free-head pop.
        at(0x27e598);if(!s.lastUsed38||s.lastUsed38!=selected.freshLastUsed38||*s.lastUsed38>=1024u)return false;
        f.lastUsed38=s.lastUsed38;f.step=3;
        at(0x27e59c);f.incremented38=*f.lastUsed38+1u;f.step=4; //Exact bounded0..1023 ->1..1024.
        at(0x27e5a0);s.lastUsed38=f.incremented38;e.pendingConsumed=true;f.counterStored=true;f.step=5;
        at(0x27e5a4);f.step=6;
        at(0x27e5a8);const auto index=*selected.nativeOrdinal-1u;
        //Revalidate after the original counter store; refusal retains the consumed reservation and count.
        if(!fogPortalSelectorCurrentNative()||index>=1024u||!e.words||fogPortalInsertionA2_!=e.words.get()+index||
           e.members[index]||e.borrowers[index]||e.wordKnown[index])return false;
        e.members[index]=fogPortalInsertionA1_;e.borrowers[index]=&fogPortalNeighborPinIdentity_;e.wordKnown[index]=false;
        f.memberStored=true;f.returnPC=call.callContinuation;f.step=7;
        //SW's guest pointer bits stay unknown. This own typed member and consumed pending reservation remain live.
        at(*f.returnPC);return false; //Before280CC4 pool setup. No allocation, secondary observer publication or portal return.
    }
    bool continueFogPortalPool280CC4(OriginalGarageObserverShared50KG& actualApplicationShared,std::uint32_t& pc)noexcept {
        auto& f=fogPrimary_.portalPool;const auto& inserted=fogPrimary_.portalInsertion;
        if(pc!=0x280cc4||inserted.step!=7||inserted.returnPC!=pc||f.callStep||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x280cc4);f.addressHighAT=0x00440000u;f.callStep=1;
        at(0x280cc8);f.callContinuation=0x280cd0u;f.callStep=2;
        at(0x280ccc);f.poolReadAttempted=true;
        if(!serviceHeapMembership_->readFogPool443138Native(actualApplicationShared,&h,fogPortalPoolA0_))return false;
        f.callStep=3; //Actual shared443138 load in JAL delay, not the List occurrence's native identity.
        at(0x105720);at(0x105724);at(0x105728);f.bodyStep=1;
        at(0x10572c);f.used20=fogPortalPoolA0_->used20;f.bodyStep=2;
        at(0x105730);f.perBlock10=fogPortalPoolA0_->perBlock10;f.bodyStep=3;
        at(0x105734);if(*f.used20>169u||*f.perBlock10!=169u)return false;
        f.less=*f.used20<*f.perBlock10;f.bodyStep=4; //Original signed SLT, equal to this bounded nonnegative comparison.
        at(0x105738);f.fullBranch=!*f.less;f.bodyStep=5;
        at(0x10573c);fogPortalPoolS0_=fogPortalPoolA0_;f.bodyStep=6; //Ordinary branch delay POR retains actual pool.
        if(!*f.fullBranch){at(0x105740);return false;} //Before item-size/current-block reads, multiply or used20 store.
        at(0x105760);fogPortalPoolFreeV0_=fogPortalPoolS0_->free2C;f.free2CNull=!fogPortalPoolFreeV0_;f.bodyStep=7;
        at(0x105764);f.freeBranch=*f.free2CNull;f.bodyStep=8;at(0x105768);f.bodyStep=9;
        if(!*f.freeBranch){at(0x10576c);return false;} //Before reading next00 or popping this historical free-record alias.
        at(0x105778);if(fogPortalPoolS0_->cached1C)return false;f.cached1CNull=true;f.bodyStep=10;
        at(0x10577c);f.cachedBranch=true;f.bodyStep=11;at(0x105780);f.bodyStep=12;
        at(0x1057d8);return false; //Before JAL105870 and its delay. No allocation, pool mutation or return280CD0.
    }
    bool continueFogPortalPoolMutationNative(std::uint32_t& pc)noexcept {
        auto& m=fogPrimary_.portalPoolMutation;const auto& p=fogPrimary_.portalPool;
        const bool free=pc==0x10576cu;
        if((pc!=0x105740u&&!free)||m.admissionAttempted||
           (free?(p.bodyStep!=9||p.free2CNull!=false):(p.bodyStep!=6||p.less!=true))||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& observer=*serviceHeapMembership_;m.admissionAttempted=true;m.fromFree=free;
        if(!observer.admitFogPoolMutationNative(&h,&fogPortalNeighborPinIdentity_,fogPortalPoolS0_,*p.used20,free,free?fogPortalPoolFreeV0_:nullptr))return false;
        m.admitted=true;const auto at=[&](std::uint32_t v){pc=pc_=h.pc=v;};
        if(free){
            at(0x10576c);fogPortalPoolNextV1_=fogPortalPoolFreeV0_->next00;m.nextNull=!fogPortalPoolNextV1_;m.step=1;
            at(0x105770);m.step=2;
            at(0x105774);if(!observer.consumeFogPoolFree105774(&h,&fogPortalNeighborPinIdentity_,fogPortalPoolS0_,
                fogPortalPoolFreeV0_,fogPortalPoolNextV1_,*p.used20))return false;
            m.poolStored=true;fogPortalPoolResultV0_=fogPortalPoolFreeV0_;m.resultAddressRetained=true;m.boundaryPC=0x105804u;m.step=3;
        }else{
            at(0x105740);m.itemSize24=fogPortalPoolS0_->itemSize24;m.step=1;
            at(0x105744);fogPortalPoolChunkA0_=fogPortalPoolS0_->current18;m.step=2;
            at(0x105748);m.incremented20=*p.used20+1u;m.step=3;
            at(0x10574c);m.productV1=*p.used20* *m.itemSize24;m.step=4; //Exact positive MULT low result, at most168*24=4032.
            at(0x105750);fogPortalPoolBaseA0_=&fogPortalPoolChunkA0_->records[0];m.step=5; //Guest header+24 mapped to actual first record, not host byte arithmetic.
            at(0x105754);if(!observer.consumeFogPoolTail105754(&h,&fogPortalNeighborPinIdentity_,fogPortalPoolS0_,fogPortalPoolChunkA0_,*p.used20))return false;
            m.poolStored=true;m.step=6; //Native record custody starts with the count store; source result V0 is still the increment.
            at(0x105758);m.step=7;
            at(0x10575c);fogPortalPoolResultV0_=fogPortalPoolBaseA0_+*p.used20;
            m.resultAddressRetained=true;m.boundaryPC=0x105804u;m.step=8; //Original branch-delay address result, never a guest ordinal.
        }
        at(0x105804);return false; //Before result-null test, unknown34 accounting, JR or caller slot14 store.
    }
    bool continueFogPortalPoolReturn105804(std::uint32_t& pc)noexcept {
        auto& r=fogPrimary_.portalPoolReturn;const auto& p=fogPrimary_.portalPool;const auto& m=fogPrimary_.portalPoolMutation;
        if(pc!=0x105804||!m.resultAddressRetained||m.boundaryPC!=pc||r.admissionAttempted||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& observer=*serviceHeapMembership_;r.admissionAttempted=true;
        if(!observer.admitFogPoolAccountingNative(&h,&fogPortalNeighborPinIdentity_,fogPortalPoolS0_,fogPortalPoolResultV0_))return false;
        r.admitted=true;const auto at=[&](std::uint32_t v){pc=pc_=h.pc=v;};
        at(0x105804);r.resultNonNull=bool(fogPortalPoolResultV0_);r.branchTaken=!*r.resultNonNull;r.step=1;
        at(0x105808);r.step=2; //Ordinary NOP delay. This authentic existing-record result takes the nonnull path.
        at(0x10580c);if(fogPortalPoolS0_->allocatedBytes34)return false;r.input34Unknown=true;r.step=3;
        at(0x105810);r.itemSize24=fogPortalPoolS0_->itemSize24;r.step=4; //Fresh source read for either branch, not tail's earlier stride receipt.
        at(0x105814);if(r.itemSize24!=24u)return false;r.sum34Unknown=true;r.step=5; //ADDU: unknown32 +24 modulo2^32 remains unknown.
        at(0x105818);if(!observer.storeFogUnknownAccounting105818(&h,&fogPortalNeighborPinIdentity_,fogPortalPoolS0_,
            fogPortalPoolResultV0_,*r.itemSize24))return false;
        r.accountingStored=true;r.step=6; //Distinct source store and own observer receipt; no numeric zero or invented account balance.
        at(0x10581c);r.step=7; //Restore the actual saved caller RA280CD0.
        at(0x105820);fogPortalPoolS0_=nullptr;r.step=8; //Callee pool S0 retires; caller's separate neighbor S0 custody is unchanged.
        at(0x105824);r.step=9;
        at(0x105828);r.returnPC=p.callContinuation;r.step=10; //JR delay restores SP+20; V0 retains the exact owned record identity.
        at(*r.returnPC);return true; //Actual105720 return only. Before the unconditional caller slot14 store280CD0.
    }
    bool continueFogPortalSlot14280CD0(std::uint32_t& pc)noexcept {
        auto& slot=fogPrimary_.portalPoolSlot14;const auto& r=fogPrimary_.portalPoolReturn;
        if(pc!=0x280cd0u||r.step!=10||r.returnPC!=pc||slot.storeAttempted||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        slot.storeAttempted=true;pc=pc_=servicePrimary_.pc=0x280cd0u;
        if(!serviceHeapMembership_->storeFogPoolSlot14280CD0(&servicePrimary_,&fogPortalNeighborPinIdentity_,
            fogPortalPoolA0_,fogPortalPoolResultV0_,fogPortalNeighborMethod_,collider_.get(),fogPortalSelectorS3_))return false;
        slot.stored=true;slot.boundaryPC=0x280cd4u;pc=pc_=servicePrimary_.pc=*slot.boundaryPC;
        return false; //Only the authentic slot14 SW completed. Before neighbor04/method08 or any observer-head publication.
    }
    template<class ActualPortal>
    bool continueFogPortalPayload280CD4(ActualPortal&& actualPortal,std::uint32_t& pc)noexcept {
        auto& f=fogPrimary_.portalPoolPayload;auto& h=servicePrimary_;const auto& slot=fogPrimary_.portalPoolSlot14;
        if(pc!=0x280cd4u||!slot.stored||slot.boundaryPC!=pc||f.entered||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        f.entered=true;auto& observer=*serviceHeapMembership_;
        const auto at=[&](std::uint32_t v){pc=pc_=h.pc=v;};
        const auto store=[&](std::uint32_t atPC,const C* child,const OriginalNativeListMethodCustody50LA* method,const C* portal,
            std::optional<std::uint32_t> distance)noexcept {
            return observer.publishFogPoolPayloadNative(atPC,&h,&fogPortalNeighborPinIdentity_,fogPortalPoolResultV0_,child,method,portal,distance);
        };
        at(0x280cd4);if(!store(pc,fogPortalNeighborS0_,nullptr,nullptr,std::nullopt))return false;f.stores=1u;f.step=1;
        at(0x280cd8);if(!store(pc,nullptr,fogPortalCurrentMethodS2_,nullptr,std::nullopt))return false;f.stores=3u;f.step=2;
        at(0x280cdc);f.portalLoadAttempted=true;const C* portal=nullptr;
        if(!actualPortal(&h,collider_.get(),fogPortalZone_,fogPortalHead_,fogPortalGraphPin_,portal,pc)){at(pc);return false;}
        if(!portal||portal!=fogPortalOperandA0_)return false;
        fogPortalPayloadV1_=portal;f.portalNonNull=true;f.step=3;
        at(0x280ce0);if(!store(pc,nullptr,nullptr,fogPortalPayloadV1_,std::nullopt))return false;f.stores=7u;f.step=4;
        at(0x280ce4);f.distanceF0=fogPortalDistanceBacking_;f.step=5; //Fresh actual sp+AC load, no arithmetic or zero inference.
        at(0x280ce8);if(!store(pc,nullptr,nullptr,nullptr,f.distanceF0))return false;f.stores=15u;f.step=6;
        at(0x280cec);f.headLoadAttempted=true;bool headNull=false;
        if(!observer.readFogPoolHead280CEC(&h,&fogPortalNeighborPinIdentity_,headNull))return false;
        fogPortalPayloadV1_=nullptr;f.oldHeadNull=headNull;f.step=7; //V1 now carries actualNULL, not the earlier portal identity.
        at(0x280cf0);if(!store(pc,nullptr,nullptr,nullptr,std::nullopt))return false;f.stores=31u;f.step=8;
        at(0x280cf4);if(!store(pc,nullptr,nullptr,nullptr,std::nullopt))return false;f.stores=63u;f.step=9;
        f.boundaryPC=0x280cf8u;at(*f.boundaryPC);return false; //One genuine observer head published; before next-link LW or any enclosing return.
    }
    //Native typed aliases of actual owned table/block/entry storage. Never
    //cast to a guest DWORD or equate these addresses with a1-based ordinal.
    std::uint32_t* const* fogSelectorTable_=nullptr;
    std::uint32_t* const* fogSelectorTableEnd_=nullptr;
    std::uint32_t* fogSelectorBlock_=nullptr;
    std::uint32_t* fogSelectorEntry_=nullptr;
    std::uint32_t* fogSelectorS4_=nullptr;
    std::uint32_t* fogInsertionA2_=nullptr; //Same live entry, retained by actual280B7C.
    bool fogSelectorCustodyWithdrawn_=false;
    bool fogSelectorCommitted_=false; //A persistent registered member, no longer an exclusive pending selector.
    bool fogSelectorCurrentNative()const noexcept {
        const auto& f=fogPrimary_.selector;const auto& h=servicePrimary_;
        const auto& call=fogPrimary_.entryCall;const auto& inserted=fogPrimary_.insertion;const auto& pub=fogPrimary_.publication;
        if(!f.historyCurrent()||!call.historyCurrent()||!inserted.historyCurrent()||!pub.historyCurrent()||
           (pub.step&&inserted.step!=7)||(call.step&&f.step!=21)||
           (inserted.step&&call.step!=11)||(inserted.lastUsed38&&inserted.lastUsed38!=f.freshLastUsed38)||
           (inserted.incremented38&&inserted.incremented38!=f.nativeOrdinal))return false;
        if(!f.step)return !fogSelectorTable_&&!fogSelectorTableEnd_&&!fogSelectorBlock_&&!fogSelectorEntry_&&!fogSelectorS4_&&
            !fogInsertionA2_&&!fogSelectorCustodyWithdrawn_&&!fogSelectorCommitted_;
        if(fogSelectorCommitted_&&(inserted.step!=7||pub.stores!=31u||!fogPrimary_.returnFlow.triggerReturned()||
           !serviceAction_.returned||!serviceRegistered_||
           fogPortalSelectorCommitted_!=fogPrimary_.portalSelector.nativeReservationAcquired))return false;
        if(h.selectionAttempted!=true||h.selectionReturned||h.slot||
           unsigned(h.selectionPending)+unsigned(h.selectionCanceled)+unsigned(fogSelectorCommitted_)!=1u||
           h.counterAdvanced!=inserted.counterStored||h.inserted!=inserted.memberStored||h.published!=(pub.stores==31u)||
           (h.slotWithdrawn&&(!inserted.memberStored||!fogSelectorCustodyWithdrawn_)))return false;
        if(pub.stores&4u){
            if(!serviceHeapMembership_)return false;const auto& observer=*serviceHeapMembership_;
            if(observer.fogSlot14AliasWithdrawn_){if(observer.fogSlot14_)return false;}
            else if(fogSelectorCustodyWithdrawn_||observer.fogSlot14_!=fogSelectorS4_)return false;
        }
        if(fogSelectorCustodyWithdrawn_)return !fogSelectorTable_&&!fogSelectorTableEnd_&&!fogSelectorBlock_&&!fogSelectorEntry_&&!fogSelectorS4_&&!fogInsertionA2_;
        if(h.methodUnpinned||h.nativeWithdrawn||!h.methodPinned||!h.method||(!h.selectionPending&&!fogSelectorCommitted_))return false;
        const auto& c=*h.method;const auto& e=c.sourceEntries_;const auto& s=c.source_;
        if(!c.sourceCollisionEntriesCurrentNative()||
           (f.perBlock30&&s.perBlock30!=f.perBlock30)||(f.blockCount34&&s.blockCount34!=f.blockCount34))return false;
        const auto expectedLast=inserted.counterStored?inserted.incremented38:f.lastUsed38;
        if(fogSelectorCommitted_){
            if(e.pending&&(e.pending==&h||e.pendingIdentity==collider_.get()||e.pendingSlot==f.nativeOrdinal.value_or(0u)))return false;
        }else if(e.pending!=&h||e.pendingIdentity!=collider_.get()||
           e.pendingSlot!=f.nativeOrdinal.value_or(0u)||e.pendingConsumed!=inserted.counterStored||
           (f.freeHeadNull&&s.field40!=0u)||(expectedLast&&s.lastUsed38!=expectedLast))return false;
        //After successful handoff, only the original read history freezes; live shared counters may advance.
        if(f.tableNonNull){if(!s.pointer3C||*s.pointer3C!=e.table.get()||fogSelectorTable_!=e.table.get()||
            *f.tableNonNull!=bool(fogSelectorTable_))return false;}
        else if(fogSelectorTable_)return false;
        if(f.step>=14){if(!fogSelectorTable_||fogSelectorTableEnd_!=fogSelectorTable_+1)return false;}
        else if(fogSelectorTableEnd_)return false;
        if(f.blockRead){if(!e.ready||!e.words||!fogSelectorTableEnd_||fogSelectorBlock_!=fogSelectorTableEnd_[-1]||fogSelectorBlock_!=e.words.get())return false;}
        else if(fogSelectorBlock_)return false;
        if(f.entryAliasResolved){const auto index=*f.nativeOrdinal-1u;
            if(index>=1024||!fogSelectorBlock_||fogSelectorEntry_!=fogSelectorBlock_+index||e.wordKnown[index]||
               e.members[index]!=(inserted.memberStored?collider_.get():nullptr)||
               e.borrowers[index]!=(inserted.memberStored?&h:nullptr))return false;}
        else if(fogSelectorEntry_)return false;
        if(call.selectedNonNull&&*call.selectedNonNull!=bool(fogSelectorS4_))return false;
        if(call.step>=7){if(fogInsertionA2_!=fogSelectorS4_)return false;}
        else if(fogInsertionA2_)return false;
        return f.s4Retained?fogSelectorS4_==fogSelectorEntry_:!fogSelectorS4_;
    }
    bool commitFogRegistrationReservationsNative()noexcept {
        auto& h=servicePrimary_;const auto& f=fogPrimary_;
        if(fogSelectorCommitted_||fogPortalSelectorCommitted_||nativeClosed_||membershipNativeWithdrawn_||
           !serviceRegistered_||serviceTopologyWithdrawn_||!serviceParent_||!fogGeometry_.step||
           !f.returnFlow.triggerReturned()||!serviceAction_.returned||serviceAction_.nativeWithdrawn||
           f.insertion.step!=7||f.publication.stores!=31u||!h.selectionPending||h.selectionCanceled||
           !serviceHeapMembership_||!preflightNative())return false;
        auto& observer=*serviceHeapMembership_;
        const bool secondary=f.portalSelector.nativeReservationAcquired;
        if(f.portalSelector.nativeReservationAttempted!=secondary||
           (secondary?(f.portalInsertion.step!=7||f.portalPoolPayload.stores!=63u||
             !observer.fogPoolAllocation_.slot14Stored||observer.fogPoolAllocation_.slot14ReservationCommitted||
             observer.fogPoolAllocation_.slot14NativeOrdinal!=f.portalSelector.nativeOrdinal||
             observer.fogPoolAllocation_.record!=fogPortalPoolResultV0_):
             (observer.fogPoolAllocation_.entered||f.portalPoolPayload.stores)))return false;
        //Both pending selections and every live observer/table/cell/pin were admitted by preflightNative.
        //All following operations are non-failing native bookkeeping. No guest store, counter change, pointer conversion,
        //source return/PC or generic selection receipt is added. Exact registered members and their lifetime pins remain.
        auto& primary=h.method->sourceEntries_;
        if(secondary){auto& e=fogPortalNeighborMethod_->sourceEntries_;
            e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;
            observer.fogPoolAllocation_.slot14ReservationCommitted=true;fogPortalSelectorCommitted_=true;
        }
        primary.pending=primary.pendingIdentity=nullptr;primary.pendingSlot=0;primary.pendingConsumed=false;
        h.selectionPending=false;fogSelectorCommitted_=true;return true;
    }
    bool fogReturnCurrentNative()const noexcept {
        const auto& r=fogPrimary_.returnFlow;
        if(!r.historyCurrent(fogPrimary_.portalNullReturns()))return false;
        if(!r.entered)return !serviceStackMembership_||!serviceStackMembership_->destroyEntered_;
        if(!serviceStackMembership_||!serviceHeapMembership_||serviceReturn_.entered||
           serviceStackMembership_.get()==serviceHeapMembership_.get())return false;
        const auto& stack=*serviceStackMembership_;const auto& heap=*serviceHeapMembership_;
        if(r.zoneRead&&(r.zoneNonNull!=bool(heap.record_.zone04)||heap.record_.zone04!=servicePrimary_.zone||
           heap.record_.zone04!=fogMembershipNode_))return false;
        if(!r.stackReturned)return !stack.destroyEntered_&&!stack.countDecremented_&&!stack.destroyReturned_;
        return stack.destroyEntered_&&stack.destroyReturned_&&stack.countDecremented_&&!stack.lease_&&
            stack.destructionArgument_==-1&&!stack.sourceDeleted_&&stack.destroyPC_==0x2809b8u&&
            r.stackCountBefore==stack.countBeforeDestroy_&&!heap.destroyEntered_;
    }
    bool finishFogServiceMembership280BC0(std::uint32_t& pc)noexcept {
        auto& r=fogPrimary_.returnFlow;
        if(pc!=0x280bc0u||r.entered||fogPrimary_.portalNullReturns()!=1u||membershipNativeWithdrawn_||
           nativeClosed_||!serviceStackMembership_||!serviceHeapMembership_||!preflightNative())return false;
        auto& stack=*serviceStackMembership_;auto& heap=*serviceHeapMembership_;
        if(&stack==&heap||stack.primary_.entered||!stack.lease_||!heap.lease_||
           !stack.shared_||stack.shared_!=heap.shared_||stack.group_!=heap.group_||
           stack.identity()==heap.identity()||stack.group_==stack.identity()||stack.group_==heap.identity()||heap.record_.zone04!=servicePrimary_.zone||
           heap.record_.zone04!=fogMembershipNode_)return false;
        const auto at=[&](std::uint32_t p)noexcept{pc=pc_=servicePrimary_.pc=r.pc=p;};
        r.entered=true;
        //Actual RA,S6..S0,F20 restores, JR280BE4 and SP+90 delay280BE8.
        //280BEC is padding, not this call's continuation.
        for(std::uint32_t p=0x280bc0;p<=0x280be8;p+=4){at(p);++r.primaryStep;}
        r.primaryReturn=0x120190u;at(*r.primaryReturn);
        if(!OriginalCollisionMembershipGuards50LA::beginZone(r.zoneGuard)){at(r.zoneGuard.pc);return false;}
        at(0x1201bc);r.zoneNonNull=bool(heap.record_.zone04);r.zoneRead=true;
        if(!OriginalCollisionMembershipGuards50LA::finishZone(r.zoneGuard,*r.zoneNonNull)){
            at(r.zoneGuard.pc);return false;}
        at(0x1201fc);r.callStep=1; //Actual caller stack+50, not selected heapE0.
        at(0x120200);r.stackTarget=0x280920u;r.stackContinuation=0x120208u;r.callStep=2;
        at(0x120204);r.stackArgument=-1;r.callStep=3; //JAL delay: signed low16 remains -1.
        at(*r.stackTarget);r.stackAttempted=true;
        if(!stack.destroy280920(*r.stackArgument))return false;
        r.stackReturned=true;r.stackCountBefore=stack.countBeforeDestroy_;r.stackCountAfter=*r.stackCountBefore-1u;
        //The reused native destructor retains end boundary2809B8. Its actual
        //JR2809B0/delay2809B4 returns here, not to that padding. V0 is the
        //same stack identity; no scalar result or heap death is fabricated.
        at(*r.stackContinuation);
        for(std::uint32_t p=0x120208;p<=0x120220;p+=4){at(p);++r.membershipStep;}
        r.membershipReturn=0x2a0b1cu;at(*r.membershipReturn);
        for(std::uint32_t p=0x2a0b1c;p<=0x2a0b2c;p+=4){at(p);++r.triggerStep;}
        r.triggerReturn=0x2dc2acu;at(*r.triggerReturn);
        return true; //Whole void geometry callback returned; action-loop POR2DC2AC remains unexecuted.
    }
    bool fogPrimaryCurrentNative()const noexcept {
        const auto& f=fogPrimary_;const auto& h=servicePrimary_;
        if(!f.historyCurrent())return false;
        if(!f.step&&!fogGeometry_.step)return true;
        if(!fogReturnCurrentNative())return false;
        if((f.step&&(fogBounds_.tailStep!=100||fogMembership_.step!=11))||h.entered!=(f.step>=9)||
           (!f.selector.step&&(h.selectionAttempted||h.selectionReturned||h.selectionPending||h.selectionCanceled||h.slot))||
           h.published!=(f.publication.stores==31u)||h.portals.entered||serviceReturn_.entered||!fogSelectorCurrentNative())return false;
        if(f.nativeCenter){
            if(std::memcmp(f.nativeCenter->data(),boundsCenter_.v,16)||std::memcmp(f.nativeHalf->data(),boundsHalf_.v,16))return false;
        }
        if(!h.entered)return !h.world&&!h.zone&&!h.method&&!h.pc&&!h.methodPinned&&!h.nativeWithdrawn;
        if(!h.world||h.zone!=fogMembershipNode_||!serviceHeapMembership_||
           h.methodResolved!=(f.step>=13)||h.methodPinned!=f.nativePinAcquired||
           (f.methodNonNull&&*f.methodNonNull!=bool(h.method)))return false;
        const auto& o=serviceHeapMembership_->primary_;
        if(!o.entered||o.occurrence!=&h||o.zone!=h.zone||o.center!=*f.nativeCenter||h.center!=*f.nativeCenter||
           o.methodRead!=(f.step>=13)||(o.methodRead&&o.method!=h.method)||o.slotRead||o.stores!=f.publication.stores||
           o.fogSelectedAddressNonNull!=f.entryCall.selectedNonNull||
           o.nativeWithdrawn!=h.nativeWithdrawn)return false;
        const auto same=[](const auto& a,const auto& b)noexcept {
            return a.entered==b.entered&&a.ready==b.ready&&a.conditionRead==b.conditionRead&&a.passed==b.passed&&
                a.guard==b.guard&&a.enabled==b.enabled&&a.stores==b.stores;
        };
        return same(f.empty,o.emptyAssertion)&&same(f.method,o.methodAssertion)&&same(f.entryCall.slot,o.slotAssertion)&&servicePrimaryCurrentNative();
    }
    bool continueFogPublication280B90(std::uint32_t& pc)noexcept {
        if(pc!=0x280b90||fogPrimary_.insertion.step!=7||fogPrimary_.insertion.returnPC!=pc||
           fogPrimary_.publication.step||!fogSelectorS4_||membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& f=fogPrimary_.publication;auto& observer=*serviceHeapMembership_;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x280b90);if(!observer.publishFogScalar280B90(0x280b90u))return false;f.stores=1;f.step=1;
        at(0x280b94);if(!observer.publishFogScalar280B90(0x280b94u))return false;f.stores=3;f.step=2;
        at(0x280b98);if(!fogSelectorCurrentNative()||!observer.publishFogScalar280B90(0x280b98u,fogSelectorS4_))return false;
        f.stores=7;f.step=3; //Native typed alias, with unknown numeric guest slot14 bits.
        at(0x280b9c);if(!observer.publishFogScalar280B90(0x280b9cu))return false;f.stores=15;f.step=4;
        at(0x280ba0);f.centerV0.emplace();std::memcpy(f.centerV0->data(),boundsCenter_.v,16);f.step=5;
        //This is the actual retained destination read, not the earlier native fingerprint.
        at(0x280ba4);f.step=6; //A1=retained colliderS1.
        at(0x280ba8);f.step=7; //A3=retained halfS5.
        at(0x280bac);f.radiusF12=fogPrimary_.radiusF12;f.step=8; //MOV.S from retainedF20; exact bits, no arithmetic.
        at(0x280bb0);f.step=9; //A2=retained centerS6.
        at(0x280bb4);f.step=10; //A0=actual observerS2.
        at(0x280bb8);f.callContinuation=0x280bc0u;f.step=11;
        at(0x280bbc);if(!observer.publishFogCenter280BBC(*f.centerV0))return false;
        f.stores=31;f.step=12;h.published=true; //All five field stores, not2809C0 return.
        at(0x280bf0);return false; //Before portal body, with the same consumed pending reservation still live.
    }
    bool continueFogInsertion27E570(std::uint32_t& pc)noexcept {
        if(pc!=0x27e570||fogPrimary_.entryCall.step!=11||fogPrimary_.insertion.step||
           !fogInsertionA2_||membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& f=fogPrimary_.insertion;auto& c=*h.method;
        auto& e=c.sourceEntries_;auto& s=c.source_;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        at(0x27e570);if(s.field40!=0u)return false;
        f.freeHead40=s.field40;h.freeBefore=*f.freeHead40;f.step=1; //Actual NULL address, not a nonnull native ordinal.
        at(0x27e574);at(0x27e578);f.step=2;
        at(0x27e598);
        if(!s.lastUsed38||s.lastUsed38!=fogPrimary_.selector.freshLastUsed38||*s.lastUsed38>=1024u)return false;
        f.lastUsed38=s.lastUsed38;h.lastBefore=*f.lastUsed38;f.step=3;
        at(0x27e59c);f.incremented38=*f.lastUsed38+1u;f.step=4; //Bounded0..1023 gives1..1024; no wrap.
        at(0x27e5a0);s.lastUsed38=f.incremented38;e.pendingConsumed=true;
        h.freeAfter=0;h.lastAfter=*f.incremented38;h.counterAdvanced=f.counterStored=true;f.step=5;
        at(0x27e5a4);f.step=6;
        at(0x27e5a8);const auto index=*fogPrimary_.selector.nativeOrdinal-1u;
        //Revalidate this exact live entry after the original count store. A
        //refusal leaves consumed pending custody; it never rolls back lastUsed.
        if(!fogSelectorCurrentNative()||index>=1024||!e.words||fogInsertionA2_!=e.words.get()+index||
           e.members[index]||e.borrowers[index]||e.wordKnown[index])return false;
        e.members[index]=collider_.get();e.borrowers[index]=&h;e.wordKnown[index]=false;
        h.inserted=f.memberStored=true;f.returnPC=0x280b90u;f.step=7;
        //The source SW stores an unknown numeric guest pointer. Typed member
        //custody records its actual identity without writing fabricated DWORDs.
        //Keep the native reservation through this bounded caller boundary so
        //no competing selector/cleanup can replace the live entry backing.
        at(0x280b90);return continueFogPublication280B90(pc);
    }
    bool continueFogSlotAssertion280B08(std::uint32_t& pc)noexcept {
        if(pc!=0x280b08||fogPrimary_.selector.step!=21||fogPrimary_.entryCall.step||
           !fogSelectorS4_||membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& f=fogPrimary_.entryCall;auto& observer=*serviceHeapMembership_;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        const auto copyAssertion=[](auto& dst,const auto& src)noexcept {
            dst.entered=src.entered;dst.ready=src.ready;dst.conditionRead=src.conditionRead;dst.passed=src.passed;
            dst.guard=src.guard;dst.enabled=src.enabled;dst.stores=src.stores;
        };
        at(0x280b08);f.selectedNonNull=bool(fogSelectorS4_); //Native snapshot of already retainedS4; no guest pointer bits.
        const bool ready=observer.beginFogSlotAssertion280B08(fogSelectorS4_);
        copyAssertion(f.slot,observer.primary_.slotAssertion);f.step=1;
        if(!ready)return false; //Unknown process state refuses before a guard read or any cold store receipt.
        at(0x280b0c);at(0x280b10);
        if(!f.slot.guard)for(std::uint32_t p=0x280b14;p<=0x280b20;p+=4)at(p);
        for(std::uint32_t p=0x280b24;p<=0x280b30;p+=4)at(p);f.step=2;
        if(f.slot.enabled){
            for(std::uint32_t p=0x280b34;p<=0x280b44;p+=4)at(p);
            const bool predicate=fogSelectorS4_!=nullptr; //Actual SLTU in JAL delay280B44.
            const bool passed=observer.finishFogSlotAssertion280B40(fogSelectorS4_);
            copyAssertion(f.slot,observer.primary_.slotAssertion);if(f.slot.conditionRead)f.slot.predicate=predicate;
            at(0x133660);at(0x133664);at(0x133668);
            if(!passed){at(0x13366c);return false;} //No diagnostic/BREAK behavior invented.
            //The genuine nonnull predicate yields V0=0 on either debug36C100 branch.
            at(0x1336a8);at(0x1336ac);at(0x1336b0);f.slot.returnV0=0;
            at(0x280b48);at(0x280b4c);
        }else{
            if(!observer.finishFogSlotAssertion280B40(fogSelectorS4_))return false;
            copyAssertion(f.slot,observer.primary_.slotAssertion); //Disabled branch reads no predicate and calls no assertion.
        }
        f.step=3;at(0x280b70);
        if(!h.method||h.method->source_.vtable08!=0x3d2d30u)return false;
        f.table08=h.method->source_.vtable08;f.step=4;
        at(0x280b74);f.step=5; //A0=the same retained methodS3.
        at(0x280b78);f.step=6; //A1=the same colliderS1 authenticated by the pending reservation.
        at(0x280b7c);fogInsertionA2_=fogSelectorS4_;f.step=7;
        at(0x280b80);f.step=8; //A3=retained centerS6, not a fresh vector read.
        at(0x280b84);f.target24=0x27e570u;f.step=9; //Original table3D2D54.
        at(0x280b88);f.callContinuation=0x280b90u;f.step=10;
        at(0x280b8c);f.step=11; //JALR delay T0=retained halfS5, no fresh vector read.
        at(0x27e570);return continueFogInsertion27E570(pc);
    }
    bool continueFogEntrySelector27E3C0(std::uint32_t& pc)noexcept {
        if(pc!=0x27e3c0||fogPrimary_.step!=21||fogPrimary_.selector.step||fogPrimary_.preferredA1!=0u||
           membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& h=servicePrimary_;auto& f=fogPrimary_.selector;
        if(!h.method||!h.methodPinned||h.methodUnpinned||h.nativeWithdrawn)return false;
        auto& c=*h.method;auto& e=c.sourceEntries_;auto& s=c.source_;
        if(!c.sourceCollisionEntriesCurrentNative()||e.pending||e.terminal||!c.sourceMode_||
           c.application_!=application_||c.world_!=h.world||c.usage10_!=1||c.reference00_!=2||c.ordinal_/10u!=1u)return false;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        //Exclusive native reservation before exposing an entry alias. This is
        //not a source insertion, counter store, allocation or word-knownness.
        e.entered=true;e.pending=&h;e.pendingIdentity=collider_.get();h.selectionAttempted=h.selectionPending=true;
        for(std::uint32_t p=0x27e3c0;p<=0x27e3cc;p+=4)at(p);f.step=1;
        at(0x27e3d0);at(0x27e3d4);f.step=2; //Actual preferredNULL branch, retained methodS1.
        at(0x27e3e0);
        if(!s.field40||*s.field40)return false; //Native free-head ordinal is NOT an authenticated source entry address.
        f.freeHeadNull=true;f.step=3;at(0x27e3e4);at(0x27e3e8);
        at(0x27e3f4);f.lastUsed38=s.lastUsed38;f.step=4;
        at(0x27e3f8);if(!s.perBlock30)return false;f.perBlock30=s.perBlock30;f.step=5;
        if(f.perBlock30!=1024u)return false;
        at(0x27e3fc);at(0x27e400);f.appendBranch=*f.lastUsed38!=*f.perBlock30;f.step=6;
        if(!*f.appendBranch){
            at(0x27e404);f.blockCount34=s.blockCount34;f.step=7;
            at(0x27e408);if(!s.pointer3C||*s.pointer3C!=e.table.get())return false;
            fogSelectorTable_=e.table.get();f.tableNonNull=bool(fogSelectorTable_);f.step=8;
            at(0x27e40c);f.newBlockCount=*f.blockCount34+1u;f.step=9;
            at(0x27e410);return false; //Actual3080C0 allocator/old-table ownership not joined, no new[] substitution.
        }
        at(0x27e534);f.blockCount34=s.blockCount34;f.step=10;
        at(0x27e538);
        if(f.blockCount34!=1u||!e.ready||!e.tablePublished||!e.blockPublished||!e.table||!e.words||
           !s.pointer3C||*s.pointer3C!=e.table.get()||e.table[0]!=e.words.get())return false;
        fogSelectorTable_=e.table.get();f.tableNonNull=true;f.step=11;
        at(0x27e53c);f.freshLastUsed38=s.lastUsed38;f.step=12;
        if(!f.freshLastUsed38||*f.freshLastUsed38>=1024u||f.freshLastUsed38!=f.lastUsed38)return false;
        at(0x27e540);f.tableOffsetBytes=*f.blockCount34<<2;f.step=13;
        at(0x27e544);fogSelectorTableEnd_=fogSelectorTable_+*f.blockCount34;f.step=14;
        //Guest arithmetic uses four-byte entries. Native pointer-width is
        //handled with the actual table index, never byte-offset host casts.
        at(0x27e548);f.wordOffsetBytes=*f.freshLastUsed38<<2;f.step=15;
        at(0x27e54c);fogSelectorBlock_=fogSelectorTableEnd_[-1];f.blockRead=true;f.step=16;
        at(0x27e550);const auto index=*f.freshLastUsed38;
        if(fogSelectorBlock_!=e.words.get()||e.members[index]||e.borrowers[index]||e.wordKnown[index])return false;
        fogSelectorEntry_=fogSelectorBlock_+index;f.nativeOrdinal=index+1u;f.entryAliasResolved=true;f.step=17;
        e.pendingSlot=*f.nativeOrdinal; //Native reservation only. Legacy h.slot/selectionReturned and observer.slot14 remain untouched.
        at(0x27e554);at(0x27e558);at(0x27e55c);f.step=18;
        at(0x27e560);f.step=19;
        at(0x27e564);f.returnPC=0x280b04u;f.step=20; //Dedicated typed-address return, not the legacy ordinal-selector receipt.
        at(0x280b04);fogSelectorS4_=fogSelectorEntry_;f.s4Retained=true;f.step=21;
        at(0x280b08);return continueFogSlotAssertion280B08(pc);
    }
    template<class ActualMethod>
    bool continueFogPrimary120170(const ::ra_pod_scene50LA::Owner* world,const void* zone,
        ActualMethod&& actualMethod,std::uint32_t& pc)noexcept {
        if(pc!=0x120170||fogPrimary_.step||fogBounds_.tailStep!=100||!boundsReturned_||!world||
           !zone||zone!=fogMembershipNode_||membershipNativeWithdrawn_||nativeClosed_||!preflightNative())return false;
        auto& f=fogPrimary_;auto& h=servicePrimary_;
        const auto at=[&](std::uint32_t p){pc=pc_=p;if(h.entered)h.pc=p;};
        const auto copyAssertion=[](auto& dst,const auto& src)noexcept {
            dst.entered=src.entered;dst.ready=src.ready;dst.conditionRead=src.conditionRead;dst.passed=src.passed;
            dst.guard=src.guard;dst.enabled=src.enabled;dst.stores=src.stores;
        };
        at(0x120170);f.radiusF12=0;f.step=1; //Actual +zero overlap radius, not shape radius.
        at(0x120174);f.step=2;at(0x120178);f.step=3; //Actual caller output pointers; no source vector read yet.
        at(0x12017c);if(!source1B0_||*source1B0_!=collider_.get())return false;f.step=4;
        at(0x120180);f.step=5; //Same retainedS2 node authenticated at entry, no fresh source slot read.
        at(0x120184);if(fogMembership_.mode!=1u)return false;f.mode=fogMembership_.mode;f.step=6;
        at(0x120188);f.callContinuation=0x120190u;f.step=7;
        at(0x12018c);
        if(!serviceHeapMembership_||!membershipHeapReturned_||!collider_->membershipE0_||
           *collider_->membershipE0_!=serviceHeapMembership_->identity())return false;
        f.nativeCenter.emplace();f.nativeHalf.emplace();
        std::memcpy(f.nativeCenter->data(),boundsCenter_.v,16);std::memcpy(f.nativeHalf->data(),boundsHalf_.v,16);f.step=8;
        //The fingerprints authenticate pointer destinations, not original LQ280BA0.
        at(0x2809c0);auto& observer=*serviceHeapMembership_;
        if(!observer.enterFogRegistration2809C0(&h,zone,*f.nativeCenter))return false;
        h.entered=true;h.world=world;h.zone=zone;h.center=*f.nativeCenter;h.pc=0x2809c0u;
        for(std::uint32_t p=0x2809c4;p<=0x2809e4;p+=4)at(p);f.step=9;
        at(0x2809e8);const bool emptyReady=observer.beginFogEmptyAssertion2809E8();
        copyAssertion(f.empty,observer.primary_.emptyAssertion);f.step=10;
        if(!emptyReady)return false; //Actual process unknown-mask refusal, before adopting a guard read.
        for(std::uint32_t p=0x2809ec;p<=0x280a08;p+=4)at(p); //Retained operands and F20=F12 in branch delay.
        if(!f.empty.guard)for(std::uint32_t p=0x280a0c;p<=0x280a18;p+=4)at(p);
        for(std::uint32_t p=0x280a1c;p<=0x280a28;p+=4)at(p);f.step=11;
        if(f.empty.enabled){
            at(0x280a2c);const bool predicate=observer.record_.zone04==nullptr;
            for(std::uint32_t p=0x280a30;p<=0x280a44;p+=4)at(p);
            const bool passed=observer.finishFogEmptyAssertion280A40();copyAssertion(f.empty,observer.primary_.emptyAssertion);
            if(f.empty.conditionRead)f.empty.predicate=predicate;
            at(0x133660);at(0x133664);at(0x133668);
            if(!passed){at(0x13366c);return false;}
            //True predicate yields V0=0 on both actual debug36C100 branches.
            at(0x1336a8);at(0x1336ac);at(0x1336b0);f.empty.returnV0=0;
            at(0x280a48);at(0x280a4c);at(0x280a70);
        }else{
            if(!observer.finishFogEmptyAssertion280A40())return false;
            copyAssertion(f.empty,observer.primary_.emptyAssertion); //Actual disabled branch skips predicate load and133660.
        }
        f.step=12;at(0x280a74);at(0x280a78); //JAL27F590, delay passes actual retained node.
        at(0x27f590);at(0x27f594);at(0x27f598);at(0x27f59c);at(0x27f5a0);
        //The callback validates the exact node34/World4CC owner with leaf checks.
        //No global/root preflight may run while its outer history copy is stale.
        if(!actualMethod(&h,collider_.get(),h.method,pc)){at(pc);return false;}
        at(0x27f5bc);at(0x27f5c0);at(0x280a7c);
        if(!observer.retainFogMethod280A7C(h.method))return false;h.methodResolved=true;
        f.methodNonNull=bool(h.method);f.lookupContinuation=0x280a7cu;f.step=13;
        at(0x280a80);
        if(h.method){
            f.nativePinAttempted=true;
            for(const auto* pin:h.method->sourceCollisionRegistrationBorrowers_)if(pin==&h)return false;
            try{h.method->sourceCollisionRegistrationBorrowers_.push_back(&h);}catch(...){return false;}
            h.methodPinned=f.nativePinAcquired=true;
        }
        const bool methodReady=observer.beginFogMethodAssertion280A80();
        copyAssertion(f.method,observer.primary_.methodAssertion);f.step=14;
        if(!methodReady)return false;
        at(0x280a84);at(0x280a88);
        if(!f.method.guard)for(std::uint32_t p=0x280a8c;p<=0x280a98;p+=4)at(p);
        for(std::uint32_t p=0x280a9c;p<=0x280aa8;p+=4)at(p);f.step=15;
        if(f.method.enabled){
            for(std::uint32_t p=0x280aac;p<=0x280abc;p+=4)at(p);
            const bool passed=observer.finishFogMethodAssertion280AB8();copyAssertion(f.method,observer.primary_.methodAssertion);
            if(f.method.conditionRead)f.method.predicate=bool(h.method);
            at(0x133660);at(0x133664);at(0x133668);
            if(!passed){at(0x13366c);return false;}
            at(0x1336a8);at(0x1336ac);at(0x1336b0);f.method.returnV0=0;
            at(0x280ac0);at(0x280ac4);
        }else{
            if(!observer.finishFogMethodAssertion280AB8())return false;
            copyAssertion(f.method,observer.primary_.methodAssertion);
        }
        f.step=16;at(0x280ae8);if(!h.method)return false; //Disabled assertion does not make NULL dereference safe.
        const auto table=h.method->source_.vtable08;if(table!=0x3d2d30u)return false;f.table08=table;f.step=17;
        at(0x280aec);at(0x280af0);at(0x280af4);at(0x280af8);
        f.target20=0x27e3b0u;f.step=18; //Actual3D2D50, not direct27E3C0.
        at(0x280afc);at(0x280b00);f.preferredA3=0;f.step=19;
        at(0x27e3b0);f.step=20;at(0x27e3b4);f.preferredA1=f.preferredA3;f.step=21;
        at(0x27e3c0);return continueFogEntrySelector27E3C0(pc); //Only the exact owned selector continuation may follow.
    }
    template<class ActualMethod>
    bool continueServicePrimary2809C0(const ::ra_pod_scene50LA::Owner* world,const void* zone,
        ActualMethod&& actualMethod,std::uint32_t& pc)noexcept {
        auto& h=servicePrimary_;
        //Fog bounds return120170 alone does not supply the later caller setup/JAL.
        if(fogGeometry_.step||!boundsReturned_||!world||!zone||h.entered||membershipNativeWithdrawn_||!serviceHeapMembership_||!preflightNative())return false;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        h.entered=true;h.world=world;h.zone=zone;at(0x2809c0);
        std::memcpy(h.center.data(),boundsCenter_.v,16);
        auto& observer=*serviceHeapMembership_;
        if(!observer.beginRegistration2809C0(&h,zone,h.center)){at(observer.registrationPC()?observer.registrationPC():0x2809c0u);return false;}
        at(0x27f5a0);
        if(!actualMethod(&h,collider_.get(),h.method,h.slot,h.selectionPending,h.pc,false)){at(h.pc);return false;}
        h.methodResolved=true;at(0x280a80);
        if(h.method){auto& c=*h.method;
            if(c.application_!=application_||c.world_!=world||!c.sourceMode_||c.usage10_!=1||
               c.ordinal_/10u!=1u||c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())return false;
            for(const auto* pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&h)return false;
            try{c.sourceCollisionRegistrationBorrowers_.push_back(&h);}catch(...){return false;}
            h.methodPinned=true;
        }
        if(!observer.recordMethod280A80(h.method)){at(observer.registrationPC());return false;}
        at(0x280afc);h.selectionAttempted=true;
        if(!actualMethod(&h,collider_.get(),h.method,h.slot,h.selectionPending,h.pc,true)){at(h.pc);return false;}
        h.selectionReturned=true;at(0x27e568);
        if(!h.slot||!observer.recordSelection280B08(*h.slot)){at(h.slot?observer.registrationPC():0x280b08u);return false;}
        at(0x280b88);auto& c=*h.method;auto& e=c.sourceEntries_;auto& state=c.source_;const auto slot=*h.slot;
        if(!slot||slot>1024||!state.field40||!state.lastUsed38||e.pending!=&h||
           e.pendingIdentity!=collider_.get()||e.pendingSlot!=slot||!h.selectionPending)return false;
        h.freeBefore=*state.field40;h.lastBefore=*state.lastUsed38;
        if(*state.field40==slot){at(0x27e594);if(!e.words||e.members[slot-1])return false;state.field40=e.nativeFreeNext[slot-1];}
        else{at(0x27e5a0);state.lastUsed38=*state.lastUsed38+1u;}
        h.freeAfter=*state.field40;h.lastAfter=*state.lastUsed38;e.pendingConsumed=true;h.counterAdvanced=true;
        at(0x27e5a8);if(!e.words)return false;
        e.members[slot-1]=collider_.get();e.borrowers[slot-1]=&h;e.wordKnown[slot-1]=false;
        e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;
        h.selectionPending=false;h.inserted=true;at(0x27e5ac);
        if(!observer.publishPrimary280B90()){at(observer.registrationPC());return false;}
        h.published=true;at(0x280bf0);return true; //Portal custody remains next, not2809C0 return.
    }
    template<class ActualPortal>
    bool continueServicePortals280BF0(ActualPortal&& actualPortal)noexcept {
        if(fogGeometry_.step)return false; //Typed Fog portal custody is unjoined; do not mutate even entry/PC history.
        auto& h=servicePrimary_;auto& p=h.portals;
        auto& observer=*serviceHeapMembership_;
        const auto stop=[&](std::uint32_t pc){h.pc=p.pc=pc;return false;};
        if(!h.published||p.entered)return stop(0x280bf0u);
        p.entered=true;p.pc=0x280bf0u;
        if(!observer.beginSecondary280BF0(&h))return stop(p.pc);
        p.helperEntered=true;p.cursorMethod=h.method;
        SourceMembershipPortalEdge initial;
        if(!actualPortal(0u,&h,collider_.get(),h.zone,h.center,nullptr,initial,p.graphPin,p.pc))return stop(p.pc);
        p.rootRead=true;p.root=p.cursor=initial.next;
        while(p.cursor){
            if(p.count==p.edges.size())return stop(0x280c40u);
            auto& s=p.edges[p.count++];s.previousMethod=p.cursorMethod;
            const bool edgeOkay=actualPortal(1u,&h,collider_.get(),h.zone,h.center,p.cursor,s,p.graphPin,p.pc);
            if(s.methodReturned)p.cursorMethod=s.method;
            if(!edgeOkay)return stop(p.pc);
            if(s.hit){
                p.cursorMethod=s.method;
                if(!s.methodReturned||!s.method)return stop(0x280c78u);
                auto& c=*s.method;
                if(c.application_!=application_||c.world_!=h.world||!c.sourceMode_||c.usage10_!=1u||c.ordinal_/10u!=1u||
                   c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())return stop(0x280c90u);
                try{c.sourceCollisionRegistrationBorrowers_.push_back(&s);}catch(...){return stop(0x280c90u);}
                s.methodPinned=true;s.selectionAttempted=true;
                if(!actualPortal(2u,&h,collider_.get(),h.zone,h.center,p.cursor,s,p.graphPin,p.pc))return stop(p.pc);
                s.selectionReturned=true;
                if(!s.slot)return stop(0x280c9cu);
                auto& e=c.sourceEntries_;
                if(!*s.slot){
                    if(e.pending!=&s||e.pendingIdentity!=collider_.get()||e.pendingSlot)return stop(0x280c9cu);
                    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;s.selectionPending=false;s.selectionCanceled=true;
                }else{
                    const auto slot=*s.slot;
                    if(slot>1024||!s.selectionPending||e.pending!=&s||e.pendingIdentity!=collider_.get()||
                       e.pendingSlot!=slot||!c.source_.field40||!c.source_.lastUsed38)return stop(0x280cbcu);
                    s.freeHeadBefore=*c.source_.field40;s.lastUsedBefore=*c.source_.lastUsed38;
                    if(*c.source_.field40==slot){if(!e.words||e.members[slot-1])return stop(0x27e594u);
                        c.source_.field40=e.nativeFreeNext[slot-1];}
                    else c.source_.lastUsed38=*c.source_.lastUsed38+1u;
                    s.freeHeadAfter=*c.source_.field40;s.lastUsedAfter=*c.source_.lastUsed38;
                    e.pendingConsumed=true;s.counterAdvanced=true;
                    if(!e.words)return stop(0x27e5a8u);
                    e.members[slot-1]=collider_.get();e.borrowers[slot-1]=&s;e.wordKnown[slot-1]=false;
                    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;
                    s.selectionPending=false;s.inserted=true;s.allocationAttempted=true;
                    const auto allocated=observer.allocateSecondary105720(&s);
                    s.allocationRecorded=observer.secondaryAllocationRecordedNative(&s);
                    if(allocated.status==Membership::SecondaryLinkStatus::nativeRefused)return stop(allocated.sourcePC);
                    s.allocationReturned=true;s.link=allocated.record;
                    if(!s.link)return stop(0x280cd0u); //Inserted membership survives NULL pool result.
                    s.link->slot14=slot;s.linkStoreMask|=1u;
                    s.link->child04=s.neighbor;s.linkStoreMask|=2u;
                    s.link->collisionMethod08=s.method;s.link->method08=nullptr;s.linkStoreMask|=4u;
                    s.link->portal0C=s.portal;s.linkStoreMask|=8u;
                    if(!s.distance)return stop(0x280ce8u);
                    s.link->distance10=*s.distance;s.linkStoreMask|=16u;
                    s.oldHead=observer.secondaryHeadNative();
                    s.link->next00=static_cast<Membership::SecondaryLink*>(const_cast<void*>(s.oldHead));s.linkStoreMask|=32u;
                    if(!observer.publishSecondary280CF4(&s,s.link,s.oldHead))return stop(0x280cf4u);
                    s.linkStoreMask|=64u;s.headPublished=true;
                }
            }
            if(!actualPortal(4u,&h,collider_.get(),h.zone,h.center,p.cursor,s,p.graphPin,p.pc))return stop(p.pc);
            s.complete=true;p.cursor=s.next;
        }
        if(!observer.finishSecondary280D30(&h))return stop(0x280d30u);
        p.returned=true;h.pc=p.pc=0x280d38u;return true; //Portal return only;120070 suffix not claimed.
    }
    bool servicePortalsCurrentNative()const noexcept {
        const auto& h=servicePrimary_;const auto& p=h.portals;
        if(!p.entered)return !p.helperEntered&&!p.rootRead&&!p.returned&&!p.nativeWithdrawn&&!p.count&&!p.pc&&!p.graphPin;
        if(!h.published||p.count>20||!p.pc||(p.returned&&(!p.rootRead||p.cursor||p.pc!=0x280d38u))||
           (p.rootRead&&!p.nativeWithdrawn&&!p.graphPin))return false;
        const auto* observer=serviceHeapMembership_.get();if(!observer)return false;
        if(observer->secondary_.entered!=p.helperEntered||observer->secondary_.returned!=p.returned||
           (p.helperEntered&&observer->secondary_.nativeWithdrawn!=p.nativeWithdrawn))return false;
        if(!p.helperEntered)return !p.rootRead&&!p.graphPin&&!p.count&&!p.returned;
        const void* cursor=p.root;auto* method=h.method;
        for(std::size_t i=0;i<p.count;++i){const auto& s=p.edges[i];
            if(s.previousMethod!=method||!source_portal_overlap231150::historyCurrent(s.overlap)||
               s.overlapReturned!=s.overlap.returned||s.hit!=s.overlap.hit||s.distanceStoreMask>3u||
               (s.entered&&(s.edge!=cursor||!(s.edgeIndex&1u)||s.edgeIndex>=40))||
               (s.distanceStoreMask&&(!s.hit||!s.distance||s.distance!=s.overlap.outputDistance))||
               (s.methodReturned&&(!s.hit||s.distanceStoreMask!=3u))||
               (s.methodPinned&&(!s.methodReturned||!s.method))||(s.methodPinWithdrawn&&!s.methodPinned)||
               (s.selectionAttempted&&!s.methodPinned)||(s.selectionReturned&&(!s.selectionAttempted||!s.slot))||
               (s.counterAdvanced&&!s.selectionReturned)||(s.inserted&&!s.counterAdvanced)||
               (s.allocationAttempted&&!s.inserted)||(s.allocationRecorded&&!s.allocationAttempted)||(s.allocationReturned&&!s.allocationRecorded)||
               (s.headPublished&&(!s.link||s.linkStoreMask!=127u))||(s.slotWithdrawn&&!s.inserted)||
               (s.linkReleased&&!s.allocationAttempted)||(s.selectionCanceled&&s.selectionPending))return false;
            if(s.counterAdvanced){
                if(!s.slot||!*s.slot||*s.slot>1024)return false;
                if(s.freeHeadBefore==*s.slot){if(s.lastUsedAfter!=s.lastUsedBefore||s.freeHeadAfter>1024)return false;}
                else if(s.freeHeadBefore||*s.slot!=s.lastUsedBefore+1u||s.lastUsedAfter!=s.lastUsedBefore+1u||s.freeHeadAfter)return false;
            }
            if(s.methodReturned)method=s.method;
            if(s.methodPinned&&!s.methodPinWithdrawn){const auto& c=*s.method;const auto& e=c.sourceEntries_;
                if(c.application_!=application_||c.world_!=h.world||!c.sourceMode_||!c.sourceCollisionEntriesCurrentNative()||c.usage10_!=1u||c.ordinal_/10u!=1u)return false;
                unsigned pins=0;for(auto pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&s)++pins;
                if(pins!=1||s.selectionPending!=(e.pending==&s))return false;
                if(s.selectionPending&&(e.pendingIdentity!=collider_.get()||e.pendingConsumed!=s.counterAdvanced||
                    (s.slot&&e.pendingSlot!=*s.slot)))return false;
                if(s.inserted&&!s.slotWithdrawn&&(!s.slot||!*s.slot||*s.slot>1024||
                    e.members[*s.slot-1]!=collider_.get()||e.borrowers[*s.slot-1]!=&s||e.wordKnown[*s.slot-1]))return false;
            }
            if(s.link&&!s.linkReleased){if(!observer->ownsSecondaryLinkNative(s.link))return false;
                const auto& l=*s.link;
                if(((s.linkStoreMask&1u)&&(!s.slot||l.slot14!=*s.slot))||((s.linkStoreMask&2u)&&l.child04!=s.neighbor)||
                   ((s.linkStoreMask&4u)&&(l.collisionMethod08!=s.method||l.method08))||
                   ((s.linkStoreMask&8u)&&l.portal0C!=s.portal)||((s.linkStoreMask&16u)&&(!s.distance||l.distance10!=*s.distance))||
                   ((s.linkStoreMask&32u)&&l.next00!=s.oldHead))return false;
            }
            if(s.complete){if(!s.overlapReturned||(s.hit&&(!s.selectionReturned||(*s.slot&&!s.headPublished))))return false;cursor=s.next;}
            else if(i+1!=p.count)return false;
            if(p.nativeWithdrawn&&(s.selectionPending||(s.methodPinned&&!s.methodPinWithdrawn)||
               (s.inserted&&!s.slotWithdrawn)||(s.allocationRecorded&&!s.linkReleased)))return false;
        }
        return cursor==p.cursor&&method==p.cursorMethod&&(!p.nativeWithdrawn||!p.graphPin);
    }
    bool withdrawServicePortalsNative()noexcept {
        auto& h=servicePrimary_;auto& p=h.portals;
        if(!p.entered||p.nativeWithdrawn)return true;
        if(!servicePortalsCurrentNative())return false;
        auto& observer=*serviceHeapMembership_;
        for(std::size_t i=p.count;i>0;--i){auto& s=p.edges[i-1];
            if(s.methodPinned&&!s.methodPinWithdrawn){auto& c=*s.method;auto& e=c.sourceEntries_;
                if(s.selectionPending){if(e.pending!=&s)return false;
                    if(e.pendingConsumed){e.abandonedConsumed=e.pendingSlot;e.terminal=true;e.pendingConsumed=false;}
                    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;s.selectionPending=false;s.selectionCanceled=true;}
                if(s.inserted&&!s.slotWithdrawn){if(!s.slot||!e.words||e.pending||!c.source_.field40||
                    e.members[*s.slot-1]!=collider_.get()||e.borrowers[*s.slot-1]!=&s)return false;
                    e.nativeFreeNext[*s.slot-1]=*c.source_.field40;e.members[*s.slot-1]=e.borrowers[*s.slot-1]=nullptr;
                    e.wordKnown[*s.slot-1]=false;c.source_.field40=*s.slot;s.slotWithdrawn=true;}
            }
            if(s.allocationRecorded&&!s.linkReleased){if(!observer.releaseSecondaryNative(&s))return false;s.linkReleased=true;}
            if(s.methodPinned&&!s.methodPinWithdrawn){auto& pins=s.method->sourceCollisionRegistrationBorrowers_;
                auto j=pins.begin();while(j!=pins.end()&&*j!=&s)++j;if(j==pins.end())return false;
                pins.erase(j);s.methodPinWithdrawn=true;}
        }
        if(p.helperEntered&&!observer.withdrawSecondaryNative(&h))return false;
        //All graph-derived addresses now historical opaque tokens; never dereferenced after this reset.
        p.graphPin.reset();p.nativeWithdrawn=true;return true;
    }
    bool servicePrimaryCurrentNative()const noexcept {
        const auto& h=servicePrimary_;const auto& fogInsert=fogPrimary_.insertion;
        const auto nativeSlot=fogInsert.counterStored?fogPrimary_.selector.nativeOrdinal:h.slot;
        if(!h.entered)return !h.world&&!h.zone&&!h.method&&!h.pc&&!h.methodPinned&&!h.nativeWithdrawn;
        if(!boundsReturned_||!h.world||!h.zone||!serviceHeapMembership_||!h.pc||
           (h.methodPinned&&(!h.methodResolved||!h.method))||(h.methodUnpinned&&!h.methodPinned)||
           (h.selectionAttempted&&!h.methodPinned)||(h.selectionReturned&&(!h.selectionAttempted||!h.slot))||
           (h.counterAdvanced&&!h.selectionReturned&&!fogInsert.counterStored)||(h.inserted&&!h.counterAdvanced)||
           (h.published&&!h.inserted)||(h.slotWithdrawn&&!h.inserted)||
           (h.selectionCanceled&&h.selectionPending))return false;
        const auto& o=serviceHeapMembership_->primary_;
        if(o.entered){std::array<std::uint32_t,4> center;std::memcpy(center.data(),boundsCenter_.v,16);
            if(o.occurrence!=&h||o.zone!=h.zone||o.center!=center||h.center!=center||o.nativeWithdrawn!=h.nativeWithdrawn||
               (o.methodRead&&(!h.methodResolved||o.method!=h.method))||
               (o.slotRead&&(!h.selectionReturned||!h.slot||o.slot!=*h.slot))||
               (h.published&&o.stores!=31))return false;
        }else if(h.methodResolved||h.methodPinned||h.selectionAttempted)return false;
        if(h.counterAdvanced){if(!nativeSlot||!*nativeSlot||*nativeSlot>1024)return false;
            if(fogInsert.counterStored){
                if(h.selectionReturned||h.slot||h.freeBefore||h.freeAfter||
                   fogInsert.lastUsed38!=h.lastBefore||fogInsert.incremented38!=h.lastAfter||*nativeSlot!=h.lastAfter)return false;
            }else if(h.freeBefore==*nativeSlot){if(h.lastAfter!=h.lastBefore||h.freeAfter>1024)return false;}
            else if(h.freeBefore||*nativeSlot!=h.lastBefore+1u||h.lastAfter!=h.lastBefore+1u||h.freeAfter)return false;}
        if(h.methodPinned&&!h.methodUnpinned){
            const auto& c=*h.method;const auto& e=c.sourceEntries_;
            if(c.application_!=application_||c.world_!=h.world||!c.sourceMode_||c.usage10_!=1||
               c.ordinal_/10u!=1u||c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())return false;
            unsigned pins=0;for(const auto* pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&h)++pins;
            if(pins!=1||h.selectionPending!=(e.pending==&h))return false;
            if(h.selectionPending&&(e.pendingIdentity!=collider_.get()||
               (h.selectionReturned&&e.pendingSlot!=*h.slot)||e.pendingConsumed!=h.counterAdvanced))return false;
            if(h.inserted&&!h.slotWithdrawn&&(!nativeSlot||!*nativeSlot||*nativeSlot>1024||!e.words||
               e.members[*nativeSlot-1]!=collider_.get()||e.borrowers[*nativeSlot-1]!=&h||e.wordKnown[*nativeSlot-1]))return false;
        }
        if(h.nativeWithdrawn&&((h.methodPinned&&!h.methodUnpinned)||h.selectionPending||(h.inserted&&!h.slotWithdrawn)))return false;
        if(fogPrimary_.selector.step&&!fogSelectorCurrentNative())return false;
        return fogPortalHeadCurrentNative()&&servicePortalsCurrentNative();
    }
    bool withdrawServicePrimaryNative()noexcept {
        auto& h=servicePrimary_;if(!h.entered||h.nativeWithdrawn)return true;
        if(!servicePrimaryCurrentNative()||!serviceHeapMembership_||!serviceHeapMembership_->preflightNative())return false;
        if(serviceHeapMembership_->primary_.entered&&!serviceHeapMembership_->canWithdrawActiveRegistrationNative())return false;
        if((fogSelectorCommitted_&&!h.slotWithdrawn&&h.method->sourceEntries_.pending)||
           (fogPortalSelectorCommitted_&&!fogPortalInsertionMemberWithdrawn_&&fogPortalNeighborMethod_->sourceEntries_.pending))return false;
        //A completed registration may coexist with later pending work, but unlink must wait for its reverse cleanup.
        //Check both methods before changing any observer, graph, payload, selector or member alias.
        if(!withdrawServicePortalsNative()||!withdrawFogPortalHeadNative())return false;
        if((fogPrimary_.publication.stores&4u)&&!serviceHeapMembership_->withdrawFogSlot14AliasNative(&h))return false;
        if(fogPrimary_.selector.step&&!fogSelectorCustodyWithdrawn_){
            fogInsertionA2_=fogSelectorS4_=fogSelectorEntry_=fogSelectorBlock_=nullptr;
            fogSelectorTableEnd_=fogSelectorTable_=nullptr;fogSelectorCustodyWithdrawn_=true;
        } //Native aliases die before pending release/unpin; original return history is unchanged.
        if(h.methodPinned&&!h.methodUnpinned){auto& c=*h.method;auto& e=c.sourceEntries_;
            if(h.selectionPending){if(e.pending!=&h)return false;
                if(e.pendingConsumed){
                    //A counter-only gap is abandoned. A genuine Fog member
                    //store instead proceeds to native unlink below, not abandonment.
                    if(!fogPrimary_.insertion.memberStored){e.abandonedConsumed=e.pendingSlot;e.terminal=true;}
                    e.pendingConsumed=false;
                }
                e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;h.selectionPending=false;h.selectionCanceled=true;}
            const auto nativeSlot=fogPrimary_.insertion.counterStored?fogPrimary_.selector.nativeOrdinal:h.slot;
            if(h.inserted&&!h.slotWithdrawn){if(!nativeSlot||!*nativeSlot||*nativeSlot>1024||!e.words||e.pending||!c.source_.field40||
                   e.members[*nativeSlot-1]!=collider_.get()||e.borrowers[*nativeSlot-1]!=&h)return false;
                e.nativeFreeNext[*nativeSlot-1]=*c.source_.field40;e.members[*nativeSlot-1]=e.borrowers[*nativeSlot-1]=nullptr;
                e.wordKnown[*nativeSlot-1]=false;c.source_.field40=*nativeSlot;h.slotWithdrawn=true;}
        }
        if(serviceHeapMembership_->primary_.entered&&!serviceHeapMembership_->withdrawRegistrationNative(&h))return false;
        if(h.methodPinned&&!h.methodUnpinned){auto& pins=h.method->sourceCollisionRegistrationBorrowers_;
            auto i=pins.begin();while(i!=pins.end()&&*i!=&h)++i;if(i==pins.end())return false;pins.erase(i);h.methodUnpinned=true;}
        h.nativeWithdrawn=true;return true; //Historical source fields/pointers not reset.
    }
    struct ServiceReturn120190 {
        bool entered=false,observerReturned=false,zoneRead=false,stackDestroyCalled=false;
        bool stackDestroyReturned=false,membershipReturned=false,triggerReturned=false;
        const void* selectedZone04=nullptr;
        std::uint32_t pc=0;
        OriginalCollisionMembershipGuards50LA::History zoneGuard;
    } serviceReturn_;
    bool finishServiceMembership120190(std::uint32_t& pc)noexcept {
        auto& h=serviceReturn_;
        if(h.entered||!servicePrimary_.portals.returned||servicePrimary_.portals.cursor||
           membershipNativeWithdrawn_||!serviceStackMembership_||!serviceHeapMembership_||!preflightNative())return false;
        const auto at=[&](std::uint32_t p){pc=pc_=h.pc=p;};
        h.entered=true;at(0x280bc0);at(0x280bec);h.observerReturned=true;
        at(0x120190);
        if(!OriginalCollisionMembershipGuards50LA::beginZone(h.zoneGuard)){at(h.zoneGuard.pc);return false;}
        at(0x1201bc);h.selectedZone04=serviceHeapMembership_->record_.zone04;h.zoneRead=true;
        if(!OriginalCollisionMembershipGuards50LA::finishZone(h.zoneGuard,h.selectedZone04!=nullptr)){
            at(h.zoneGuard.pc);return false;}
        at(0x1201fc);at(0x120200);h.stackDestroyCalled=true;
        // Actual stack50, NOT selected persistent heapE0. Source argument -1
        // suppresses allocation free; the heap's registration/pool links stay.
        at(0x280920);
        if(!serviceStackMembership_->destroy280920(-1))return false;
        h.stackDestroyReturned=true;at(0x2809b8);
        at(0x120224);h.membershipReturned=true;
        at(0x2a0b1c);at(0x2a0b2c);h.triggerReturned=true;at(0x2dc2ac);
        return true; //Void caller return only; no manufactured V0 or action service.
    }
    bool serviceReturnCurrentNative()const noexcept {
        const auto& h=serviceReturn_;
        if(!OriginalCollisionMembershipGuards50LA::zoneHistory(h.zoneGuard))return false;
        if(!h.entered)return !h.observerReturned&&!h.zoneRead&&!h.selectedZone04&&!h.pc&&
            !h.stackDestroyCalled&&!h.stackDestroyReturned&&!h.membershipReturned&&!h.triggerReturned&&!h.zoneGuard.entered;
        if(!servicePrimary_.portals.returned||!serviceHeapMembership_||!serviceStackMembership_||
           !h.observerReturned||(h.zoneRead&&h.selectedZone04!=serviceHeapMembership_->record_.zone04)||
           (h.stackDestroyCalled&&(!h.zoneRead||!h.selectedZone04||!h.zoneGuard.returned))||
           (h.stackDestroyReturned&&!h.stackDestroyCalled)||(h.membershipReturned&&!h.stackDestroyReturned)||
           (h.triggerReturned&&!h.membershipReturned))return false;
        if(!h.zoneGuard.returned)return !h.stackDestroyCalled&&!h.membershipReturned&&h.pc==h.zoneGuard.pc;
        const auto& stack=*serviceStackMembership_;
        if(!h.stackDestroyReturned)return h.stackDestroyCalled&&!stack.destroyEntered_&&h.pc==0x280920u;
        if(!stack.destroyEntered_||!stack.destroyReturned_||!stack.countDecremented_||stack.lease_||
           stack.destructionArgument_!=-1||stack.sourceDeleted_||stack.destroyPC_!=0x2809b8u)return false;
        return h.membershipReturned&&h.triggerReturned&&h.pc==0x2dc2acu;
    }
    bool serviceCallbackReturnedNative()const noexcept {
        return fogGeometry_.step?fogPrimary_.returnFlow.triggerReturned():serviceReturn_.triggerReturned;
    }
    struct ServiceAction2A0A30 {
        bool entered=false,dataStored=false,codeStored=false,countIncremented=false,returned=false,nativeWithdrawn=false;
        const void* payload=nullptr;
        std::optional<std::uint32_t> countRead,capacityRead,codeA0;
        std::optional<std::uint32_t> fogCallReturnPC; //Own JAL1B5AE4 continuation, not padding2A0A7C.
    } serviceAction_;
    bool installServiceAction2A0A30(const void* actualPayload,std::uint32_t& pc)noexcept {
        auto& h=serviceAction_;
        if(h.entered||!actualPayload||!serviceCallbackReturnedNative()||membershipNativeWithdrawn_||!preflightNative())return false;
        h.entered=true;h.payload=actualPayload;pc=pc_=0x2a0a30;
        if(!count98_)return false;h.countRead=*count98_;
        pc=pc_=0x2a0a34;if(!capacity94_)return false;h.capacityRead=*capacity94_;
        pc=pc_=0x2a0a38;
        if(static_cast<std::int32_t>(*h.countRead)<static_cast<std::int32_t>(*h.capacityRead)){
            //Exact source authored action0/capacity1 owner; no unknown slot access.
            pc=pc_=0x2a0a44;
            if(*h.countRead!=0||*h.capacityRead!=1||!source9C_||*source9C_!=callbacks9C_.get()||!callbacks9C_)return false;
            pc=pc_=0x2a0a50;callbacks9C_[0]=actualPayload;h.dataStored=true;
            pc=pc_=0x2a0a54;if(!count98_||*count98_!=*h.countRead)return false;
            pc=pc_=0x2a0a58;if(!sourceA0_||*sourceA0_!=argumentsA0_.get()||!argumentsA0_)return false;
            //Source9C is DATA; sourceA0 is CODE, despite old backing names.
            //Typed source-PC knowledge supersedes unknown native pointer slot;
            //never reinterpret integer1B27C0 as an executable host pointer.
            pc=pc_=0x2a0a64;h.codeA0=0x1b27c0u;h.codeStored=true;
            pc=pc_=0x2a0a68;if(!count98_||*count98_!=*h.countRead)return false;
            pc=pc_=0x2a0a70;count98_=*h.countRead+1u;h.countIncremented=true;
        }
        if(fogGeometry_.step){
            pc=pc_=0x2a0a74;pc=pc_=0x2a0a78; //Actual JR and ordinary return delay.
            h.fogCallReturnPC=0x1b5aecu;pc=pc_=*h.fogCallReturnPC;
        }else pc=pc_=0x2a0a7c; //Existing generic historical end boundary remains unchanged.
        h.returned=true;return true;
    }
    bool serviceActionCurrentNative()const noexcept {
        const auto& h=serviceAction_;
        if(!h.entered)return !h.payload&&!h.countRead&&!h.capacityRead&&!h.codeA0&&!h.dataStored&&
            !h.codeStored&&!h.countIncremented&&!h.returned&&!h.nativeWithdrawn&&!h.fogCallReturnPC;
        if(!serviceCallbackReturnedNative()||!h.payload||(h.capacityRead&&!h.countRead)||
           bool(h.fogCallReturnPC)!=(bool(fogGeometry_.step)&&h.returned)||
           (h.fogCallReturnPC&&*h.fogCallReturnPC!=0x1b5aecu)||
           (h.dataStored&&(!h.countRead||!h.capacityRead||*h.countRead!=0||*h.capacityRead!=1))||
           (h.codeStored!=h.codeA0.has_value())||(h.codeStored&&(!h.dataStored||*h.codeA0!=0x1b27c0u))||
           (h.countIncremented&&!h.codeStored)||(h.returned&&(!h.countRead||!h.capacityRead||
               (static_cast<std::int32_t>(*h.countRead)<static_cast<std::int32_t>(*h.capacityRead)&&!h.countIncremented))))return false;
        if(h.countRead&&(!count98_||*count98_!=*h.countRead+(h.countIncremented?1u:0u)))return false;
        if(h.capacityRead&&capacity94_!=h.capacityRead)return false;
        if(h.dataStored&&!h.nativeWithdrawn&&(!callbacks9C_||callbacks9C_[0]!=h.payload))return false;
        return true;
    }
    bool withdrawServiceActionNative(const void* actualPayload)noexcept {
        auto& h=serviceAction_;if(!h.entered||h.nativeWithdrawn)return true;
        if(h.payload!=actualPayload||!serviceActionCurrentNative())return false;
        if(h.dataStored)callbacks9C_[0]=nullptr; //Native alias withdrawal, not a source9C clear receipt.
        h.nativeWithdrawn=true;return true;
    }
    static void identity(BodyMatrix48G& m)noexcept {
        std::memset(m.v,0,sizeof m.v);m.v[15]=1;m.v[10]=1;m.v[5]=1;m.v[0]=1;
    }
    bool construct2A0290(OriginalApplicationEntities50LA& app,std::uint64_t generation,
        std::uint64_t key,const std::array<std::uint32_t,4>& actual4438D0,std::uint32_t& pc)noexcept {
        if(application_||counted_||constructed_||nativeClosed_)return false;
        application_=&app;pc=pc_=0x2a029c;
        const auto r=initializeOriginalWreckComponent50LA(entity_,OriginalWreckComponentKind50LA::trigger2A0290,
            generation,key,true,app.objectCount423F60());
        if(r.status!=S::complete){pc=pc_=r.sourcePC;return false;}counted_=true;
        pc_=0x2a02b0;vtable08_=0x3d3930;
        position30_={{0,0,0,1}};box40_={{0,0,0,1}}; // 2A02BC/2A02C8
        flags1B8_|=1;flagsKnown_|=1; //2A02FC
        flags1B8_|=2;flagsKnown_|=2; //2A032C
        source9C_=nullptr;sourceA0_=nullptr;field1A8_=0; //2A0338/340/348
        for(unsigned bit=2;bit<6;++bit){flags1B8_&=static_cast<std::uint8_t>(~(1u<<bit));flagsKnown_|=1u<<bit;}
        random90_=100; //2A03A0; upper two flag bits remain unknown.
        std::memcpy(&position30_,actual4438D0.data(),16); //2A03AC
        radius1AC_=0.f; //2A03B8 delay slot before genuine identity14D840.
        identity(matrix50_); // source eight 8-byte copies2A03D8/DC.
        capacity94_=0;count98_=0;field1B4_=0;
        constructed_=true;pc=pc_=0x2dbffc;return true;
    }
    // Native finite/FE_TONEAREST adaptation explicitly authorized for this
    // authored cohort. NOT an EE bit-exact MUL.S emulation claim. Refusal at
    // the operation retains every preceding allocation/publication/store.
    bool sphere11C0B0(const SpringVector4_48N& position,float radius,std::uint32_t& pc)noexcept {
        auto& b=*collider_;if(!b.freshShapeNull()||shapeAttempted_)return false;
        shapeAttempted_=true;pc=pc_=0x11c0dc; //11B600 genuine fresh-NULL branch.
        b.shape8C_=nullptr;b.result90_=nullptr;
        pc=pc_=0x11c0e4;try{shape_.reset(new Shape);}catch(...){shape_.reset();}shapeAllocationReturned_=true;
        if(shape_){shape_->value.localCenter={{0,0,0,1}};shape_->known|=1;} //11C0F4
        b.shape8C_=shape_.get(); //11C0F8 even NULL.
        pc=pc_=0x11c104;b.shape88_=5; //11C108 delay slot BEFORE malloc160.
        try{result_.reset(new Result);}catch(...){result_.reset();}resultAllocationReturned_=true;
        if(result_){const SpringVector4_48N vf0{{0,0,0,1}};result_->center20=vf0;
            for(auto& v:result_->normals30)v=vf0;for(auto& v:result_->points60)v=vf0;}
        pc=pc_=0x11c160;
        if(std::fegetround()!=FE_TONEAREST||!std::isfinite(radius))return false;
        volatile float squared=radius*radius;if(!std::isfinite(squared))return false;
        pc=pc_=0x11c164;if(!result_)return false;
        result_->capacity00=3;result_->count04=0;b.result90_=result_.get(); //164/168/16C
        pc=pc_=0x11c180;if(!shape_)return false;
        shape_->owner60=nullptr;
        auto& s=shape_->value;s.localCenter=position;shape_->known|=1; //11C18C
        s.radius50=radius;shape_->known|=2;s.radiusSquared54=squared;shape_->known|=4;
        s.radiusScale58=1;shape_->known|=8;s.centerScale5C=1;shape_->known|=16;
        identity(s.world);shape_->known|=32;shapeReturned_=true;return true;
    }
    bool setup2A04B0(const std::array<std::uint32_t,4>& position,const std::array<std::uint32_t,4>& box,
        const std::array<std::uint32_t,4>& look,const std::array<std::uint32_t,4>& up,
        std::uint32_t radiusBits,std::int32_t capacity,std::int32_t random,
        const std::array<std::uint32_t,4>& actualSentinel,std::uint32_t actualCategory,
        std::uint8_t& actualCollisionValid,std::uint32_t& pc)noexcept {
        if(!constructed_||setupAttempted_||nativeClosed_)return false;
        setupAttempted_=true;
        // Four actual stack vector copies precede no-op141C20. They are
        // consumed immediately; look/up affect only the unjoined box branch.
        SpringVector4_48N p,bx,lk,u;std::memcpy(&p,position.data(),16);std::memcpy(&bx,box.data(),16);
        std::memcpy(&lk,look.data(),16);std::memcpy(&u,up.data(),16);
        pc=pc_=0x2a0514;position30_=p;box40_=bx;radius1AC_=std::bit_cast<float>(radiusBits);
        pc=pc_=0x2a0524;random90_=static_cast<std::uint32_t>(random); //2A0528 delay slot.
        try{collider_.reset(new B);}catch(...){collider_.reset();}colliderAllocationReturned_=true;
        if(collider_){SpringVector4_48N sentinel;std::memcpy(&sentinel,actualSentinel.data(),16);
            const auto r=collider_->construct11B190(*application_,entity_.sessionGeneration,static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&collider_->entity_)),sentinel);
            if(r.status!=S::complete){pc=pc_=r.sourcePC;return false;}}
        source1B0_=collider_.get();pc=pc_=0x11b574;if(!collider_)return false; //2A053C then11B570.
        auto& cb=*collider_;
        cb.flagsF8_&=static_cast<std::uint8_t>(~8u);cb.flagsF8_|=2;cb.flagsF8_|=4;actualCollisionValid=0;
        pc=pc_=0x2a055c;cb.categoryA4_=actualCategory;
        pc=pc_=0x2a0564;
        if(!std::isfinite(*radius1AC_)||!(*radius1AC_>0.f))return false; //Box branch not implemented.
        if(!sphere11C0B0(p,*radius1AC_,pc))return false;
        cb.modeC8_=1; //124004 return delay reached by2A0704.
        cb.owner9C_=&entity_;cb.callbacksE4_[1].reset();callbackE8PC_=0x2a00a0; //2A0718/720. Typed code-PC lane supersedes base null pointer knowledge; no callback invocation.
        // Actual11F570 shape5 with source owner60 NULL ->11ED60(NULL).
        // Both11F3B0/B4 and11FCAC/B0 write identity. VU identity-transform
        // result is exact for the admitted finite, positive authored XYZ,W=1;
        // there is no general matrix/bounds projection or query readiness.
        pc=pc_=0x11f3a4;identity(shape_->value.world);
        cb.cachedCenter60_=shape_->value.localCenter;cb.radius80_=shape_->value.radius50; //11F404/410
        pc=pc_=0x11f430;
        for(unsigned k=0;k<3;++k)if(!std::isfinite(cb.cachedCenter60_.v[k])||!(cb.cachedCenter60_.v[k]>0.f))return false;
        if(std::bit_cast<std::uint32_t>(cb.cachedCenter60_.v[3])!=0x3f800000u)return false;
        //11F444 exact identity result already equals cached60.
        identity(shape_->value.world); //11F680 shape5 NULL, second copy.
        cb.cachedCenter60_=shape_->value.localCenter;cb.radius80_=shape_->value.radius50; //11FD00/0C
        refreshReturned_=true;
        C* nodes[]{&entity_,&cb.entity_};OriginalWreckComponentDomain50LA domain{entity_.sessionGeneration,nodes,2};
        pc=pc_=0x2a073c;const auto r=cb.append141960(domain,entity_,2);
        if(r.status!=S::complete){pc=pc_=r.sourcePC;return false;}
        if(capacity<=0)capacity=1;
        pc=pc_=0x2a0754;try{callbacks9C_.reset(new const void*[static_cast<std::size_t>(capacity)]);}catch(...){callbacks9C_.reset();}
        callbacksAllocationReturned_=true;source9C_=callbacks9C_.get(); //2A075C; elements unknown.
        pc=pc_=0x2a0760;try{argumentsA0_.reset(new const void*[static_cast<std::size_t>(capacity)]);}catch(...){argumentsA0_.reset();}
        argumentsAllocationReturned_=true;sourceA0_=argumentsA0_.get(); //2A0768; elements unknown.
        capacity94_=static_cast<std::uint32_t>(capacity);count98_=0; //2A076C/770
        setupReturned_=true;pc=pc_=0x2dc024;return true;
    }
    C* colliderEntity()noexcept{return collider_?&collider_->entity_:nullptr;}
    const C* colliderEntity()const noexcept{return collider_?&collider_->entity_:nullptr;}
    std::size_t countedEntities()const noexcept{return std::size_t(counted_)+(collider_&&collider_->entity_.reference00?1u:0u);}
    bool preflightNative()const noexcept {
        if(nativeClosed_)return !counted_&&!collider_&&!shape_&&!result_&&!callbacks9C_&&!argumentsA0_;
        if(!application_||application_->attachmentLock423F58()||!serviceBoundsCurrentNative()||!servicePrimaryCurrentNative()||!serviceReturnCurrentNative()||!serviceActionCurrentNative())return false;
        if(!fogGeometry_.historyCurrent()||!fogMembershipCurrentNative()||!fogBoundsCurrentNative()||!fogPrimaryCurrentNative())return false;
        if(fogGeometry_.step){const auto& h=fogGeometry_;
            if(!serviceEntered_||serviceAttachment1A8_!=h.attachment1A8||
               bool(serviceMatrix50_)!=bool(h.matrix50)||serviceMatrixCopied_!=(h.step>=10)||
               serviceCenterStored_!=(h.step>=11)||serviceRadiusStored_!=(h.step>=12)||serviceGeometryReturned_!=(h.step==22))return false;
            if(h.matrix50&&(std::memcmp(h.matrix50->data(),matrix50_.v,sizeof matrix50_.v)||
               std::memcmp(h.matrix50->data(),serviceMatrix50_->v,sizeof matrix50_.v)))return false;
            if(h.shape88&&(!collider_||*h.shape88!=collider_->shape88_))return false;
            if(h.step>=9&&(!collider_||!shape_||shape_->known!=63||!collider_->shape8C_||*collider_->shape8C_!=shape_.get()))return false;
            if(h.matrixWords&&(!shape_||!h.matrix50||
               std::memcmp(h.matrix50->data(),shape_->value.world.v,4u*h.matrixWords)))return false;
            if(h.center00&&(!shape_||!collider_||
               std::memcmp(h.center00->data(),shape_->value.localCenter.v,sizeof *h.center00)||
               std::memcmp(h.transformedCenter?h.transformedCenter->data():h.center00->data(),
                   collider_->cachedCenter60_.v,sizeof *h.center00)))return false;
            if(h.radius50&&(!shape_||!collider_||!collider_->radius80_||
               *h.radius50!=std::bit_cast<std::uint32_t>(shape_->value.radius50)||
               *h.radius50!=std::bit_cast<std::uint32_t>(*collider_->radius80_)))return false;
        }
        if((membershipEntered_&&(!serviceGeometryReturned_||!serviceStackMembership_))||
           (membershipStackReturned_&&!membershipEntered_)||(membershipFlagsF8_&&!membershipStackReturned_)||
           (membershipOldE0NullRead_&&(!membershipFlagsF8_||(*membershipFlagsF8_&8u)))||
           (membershipHeapAllocationReturned_&&!membershipOldE0NullRead_)||
           (serviceHeapMembership_&&!membershipHeapAllocationReturned_)||
           (membershipHeapReturned_&&!serviceHeapMembership_)||
           (membershipE0Published_&&(!membershipHeapAllocationReturned_||(serviceHeapMembership_&&!membershipHeapReturned_))))return false;
        if(serviceStackMembership_&&(!serviceStackMembership_->preflightNative()||
           serviceStackMembership_->returned()!=membershipStackReturned_))return false;
        if(serviceHeapMembership_&&(!serviceHeapMembership_->preflightNative()||
           serviceHeapMembership_->returned()!=membershipHeapReturned_))return false;
        if(membershipNativeWithdrawn_&&((serviceStackMembership_&&!serviceStackMembership_->nativeWithdrawn_)||
           (serviceHeapMembership_&&!serviceHeapMembership_->nativeWithdrawn_)))return false;
        if(membershipFlagsF8_&&(!collider_||*membershipFlagsF8_!=collider_->flagsF8_))return false;

        if((serviceEntered_&&!setupReturned_)||(serviceAttachment1A8_&&!serviceEntered_)||
           (serviceMatrix50_&&(!serviceAttachment1A8_||*serviceAttachment1A8_))||
           (serviceMatrixCopied_&&!serviceMatrix50_)||(serviceCenterStored_&&!serviceMatrixCopied_)||
           (serviceRadiusStored_&&!serviceCenterStored_)||(serviceGeometryReturned_&&!serviceRadiusStored_))return false;
        if(serviceAttachment1A8_&&serviceAttachment1A8_!=field1A8_)return false;
        if(serviceMatrix50_&&std::memcmp(serviceMatrix50_->v,matrix50_.v,sizeof matrix50_.v))return false;
        if(serviceMatrixCopied_&&(!shape_||std::memcmp(shape_->value.world.v,matrix50_.v,sizeof matrix50_.v)))return false;
        if(serviceCenterStored_&&(!collider_||!shape_||std::memcmp(collider_->cachedCenter60_.v,
           fogGeometry_.transformedCenter?static_cast<const void*>(fogGeometry_.transformedCenter->data()):
               static_cast<const void*>(shape_->value.localCenter.v),sizeof shape_->value.localCenter.v)))return false;
        if(serviceRadiusStored_&&(!collider_||!shape_||!collider_->radius80_||
           std::bit_cast<std::uint32_t>(*collider_->radius80_)!=std::bit_cast<std::uint32_t>(shape_->value.radius50)))return false;

        if((serviceRegistrationAttempted_&&(!serviceCallbackReturnedNative()||!serviceParent_))||
           (serviceParent_&&!serviceRegistrationAttempted_)||(serviceRegistered_&&!serviceRegistrationAttempted_)||
           (serviceTopologyWithdrawn_&&!serviceRegistered_))return false;
        if(counted_&&(entity_.reference00!=1||entity_.nativeFlags29||entity_.mask20!=(serviceRegistered_?0x402u:0u)))return false;
        if(serviceRegistered_&&!serviceTopologyWithdrawn_){
            if(entity_.parent1C!=serviceParent_)return false; //Exact siblings are checked by source list owner.
        }else if(entity_.previous10||entity_.next14||entity_.parent1C)return false;
        if(!counted_&&entity_.reference00)return false;
        if(setupReturned_&&(!constructed_||!refreshReturned_||!capacity94_||*capacity94_!=1||!count98_||*count98_!=(serviceAction_.countIncremented?1u:0u)||
            !callbacksAllocationReturned_||!argumentsAllocationReturned_))return false;
        if(source9C_&&*source9C_!=callbacks9C_.get())return false;
        if(sourceA0_&&*sourceA0_!=argumentsA0_.get())return false;
        if(source1B0_&&*source1B0_!=collider_.get())return false;
        if(!collider_)return !entity_.firstChild18&&!entity_.descendants24&&!shape_&&!result_;
        const auto& b=*collider_;const auto& c=b.entity_;
        if(c.reference00&&c.reference00!=1)return false;
        if(c.firstChild18||c.previous10||c.next14||c.nativeFlags29)return false;
        if(b.appended_){if(c.parent1C!=&entity_||entity_.firstChild18!=&c||c.mask20!=2||entity_.descendants24!=2)return false;}
        else if(c.parent1C||entity_.firstChild18||entity_.descendants24||c.mask20)return false;
        if(b.phase_==B::Phase::complete){
            if(membershipE0Published_){
                const void* expected=serviceHeapMembership_?serviceHeapMembership_->identity():nullptr;
                if(!b.membershipE0_||*b.membershipE0_!=expected||b.listC0_||b.listCC_||b.listD8_||
                   b.countC4_||b.countD0_||b.countDC_)return false;
            }else if(!b.constructorNullListsCurrent())return false;
            if(!b.bodyA0_||*b.bodyA0_)return false;
            if(b.owner9C_&&*b.owner9C_&&*b.owner9C_!=&entity_)return false;
            for(unsigned i=0;i<4;++i){if(i==1&&callbackE8PC_){
                if(*callbackE8PC_!=0x2a00a0||b.callbacksE4_[i])return false;
            }else if(!b.callbacksE4_[i]||*b.callbacksE4_[i])return false;}
            if(b.shape8C_&&*b.shape8C_!=shape_.get())return false;
            //Result can be allocated but unpublished before11C16C.
            if(b.result90_&&*b.result90_&&*b.result90_!=result_.get())return false;
        }else if(shape_||result_||b.appended_)return false;
        return application_->objectCount423F60()>=countedEntities();
    }
    // Called only after all actual authority entries have been withdrawn by
    // the same World preflight. No fallible source destructor/dispatch claim.
    void closeNativeAfterAuthority()noexcept {
        if(serviceRegistered_&&!serviceTopologyWithdrawn_)std::terminate();
        if(serviceAction_.entered&&!serviceAction_.nativeWithdrawn){
            if(!withdrawServiceActionNative(serviceAction_.payload))std::terminate();
        }
        if(fogPortalZone_||fogPortalMethod_||fogPortalHead_||fogPortalGraphPin_||fogPortalOperandA0_||
           fogPortalVertices_||fogPortalCenterA1_||fogPortalDistanceA2_||fogPortalRelativeA3_||
           fogPortalNormal_||fogPortalTemporaryV0_||fogPortalEdgeRelativeT0_||fogPortalFirstEdge_)std::terminate();
        if(fogPortalSelectorTable_||fogPortalSelectorTableEnd_||fogPortalSelectorBlock_||fogPortalSelectorEntry_||fogPortalSelectorS3_||
           fogPortalInsertionA0_||fogPortalInsertionA1_||fogPortalInsertionA2_||fogPortalInsertionA3_||fogPortalInsertionT0_||
           (fogPrimary_.portalSelector.nativeReservationAcquired&&!fogPortalSelectorCanceled_&&!fogPortalSelectorCommitted_)||
           (fogPrimary_.portalInsertion.memberStored&&!fogPortalInsertionMemberWithdrawn_)||
           (fogPrimary_.portalInsertion.counterStored&&!fogPrimary_.portalInsertion.memberStored&&!fogPortalInsertionAbandoned_))std::terminate();
        if(fogPortalPoolA0_||fogPortalPoolS0_||fogPortalPoolFreeV0_||fogPortalPoolChunkA0_||fogPortalPoolBaseA0_||
           fogPortalPoolNextV1_||fogPortalPoolResultV0_||fogPortalPayloadV1_||(fogPrimary_.portalPool.callStep&&!fogPortalPoolWithdrawn_)||
           (fogPrimary_.portalPoolMutation.poolStored&&!fogPortalPoolRecordWithdrawn_)||
           (fogPrimary_.portalPoolSlot14.stored&&!fogPortalPoolSlot14Withdrawn_))std::terminate();
        if(fogPortalNextCursor_||fogPortalHitNextCursor_||fogPortalNextPortalA0_||fogPortalNextVertices_||
           fogPortalNextCenterA1_||fogPortalNextDistanceA2_||fogPortalNeighborNode_||fogPortalNeighborS0_||fogPortalNeighborMethod_||
           fogPortalCurrentMethodS2_||fogPortalSelectA0_||fogPortalSelectCenterA1_||fogPortalSelectHalfA2_||
           (fogPrimary_.portalNeighbor.nativePinAcquired&&!fogPortalNeighborUnpinned_))std::terminate();
        if(!fogPortalNextQuery_.aliasesCleared()||!fogPortalMissTail_.aliasesCleared())std::terminate();
        for(unsigned i=0;i<3;++i)if(fogPortalLoopVertices_[i]||fogPortalLoopEdges_[i])std::terminate();
        // World withdraws both observer leases before authority/count disposal.
        if((serviceStackMembership_&&!serviceStackMembership_->nativeWithdrawn_)||
           (serviceHeapMembership_&&!serviceHeapMembership_->nativeWithdrawn_))std::terminate();
        if(collider_)collider_->membershipE0_.reset(); //Native alias withdrawal only.
        serviceHeapMembership_.reset();serviceStackMembership_.reset();
        source9C_.reset();sourceA0_.reset();source1B0_.reset();
        callbacks9C_.reset();argumentsA0_.reset();shape_.reset();result_.reset();
        if(collider_){auto& b=*collider_;if(b.appended_){entity_.firstChild18=nullptr;entity_.descendants24=0;}
            if(b.entity_.reference00){--b.entity_.reference00;--application_->objectCount423F60();}
            b.phase_=B::Phase::released;collider_.reset();}
        if(counted_){--entity_.reference00;--application_->objectCount423F60();counted_=false;}
        nativeClosed_=true;
    }
public:
    OriginalWorldTriggerInstance50LA()=default;
    OriginalWorldTriggerInstance50LA(const OriginalWorldTriggerInstance50LA&)=delete;
    OriginalWorldTriggerInstance50LA& operator=(const OriginalWorldTriggerInstance50LA&)=delete;
    ~OriginalWorldTriggerInstance50LA(){if(entity_.reference00||collider_)std::terminate();}
};
} //namespace rr::game
