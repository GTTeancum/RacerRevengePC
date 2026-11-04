#pragma once
#include "original_collision_base_owner50la.h"
#include "trigger_sphere50in.h"
#include "original_source_collision_observer50la.h"
#include "original_collision_membership_guards50la.h"
#include "original_source_portal_overlap231150.h"
#include <bit>
#include <cfenv>
#include <cmath>
#include <memory>
#include <new>
namespace ra_pod_scene50LA {class Owner;}
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
        bool historyCurrent()const noexcept {
            if(step>12||(step&&!nativeAdmissionAttempted)||bool(radiusF12)!=(step>=3)||
               (radiusF12&&*radiusF12!=0u)||bool(callContinuation)!=(step>=4)||
               (callContinuation&&*callContinuation!=0x280c54u))return false;
            for(unsigned k=0;k<3;++k)if(bool(centerWords[k])!=(step>=6+2*k)||
                bool(vertexWords[k])!=(step>=7+2*k))return false;
            return true;
        }
    } portalInputs;
    bool operator==(const OriginalTriggerFogPrimaryPrefix50LA&)const=default;
    bool historyCurrent()const noexcept {
        for(unsigned k=0;k<3;++k)if(portalInputs.centerWords[k]&&
            (!nativeCenter||*portalInputs.centerWords[k]!=(*nativeCenter)[k]))return false;
        if(step>21||!selector.historyCurrent()||!entryCall.historyCurrent()||!insertion.historyCurrent()||!publication.historyCurrent()||
           !portalHead.historyCurrent()||!portalInputs.historyCurrent()||(portalHead.step&&publication.step!=12)||
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
        if(h.stackConstructorPC&&serviceStackMembership_->pc()!=*h.stackConstructorPC)return false;
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
    //knownness; no source output word is written or adopted by this prefix.
    std::uint32_t fogPortalDistanceBacking_=0;
    std::uint32_t* fogPortalDistanceA2_=nullptr;
    bool fogPortalInputsCurrentNative()const noexcept {
        const auto& f=fogPrimary_.portalInputs;
        if(!f.historyCurrent())return false;
        if(fogPortalCustodyWithdrawn_||!f.step)return !fogPortalOperandA0_&&!fogPortalVertices_&&
            !fogPortalCenterA1_&&!fogPortalDistanceA2_;
        if(!fogPortalGraphPin_||!fogPortalHead_||fogPrimary_.portalHead.step!=9||
           fogPrimary_.portalHead.headNull!=false||!fogPortalOperandA0_||!fogPortalVertices_)return false;
        if((f.step>=2?fogPortalDistanceA2_!=&fogPortalDistanceBacking_:bool(fogPortalDistanceA2_))||
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
        if(!fogPortalHeadCurrentNative())return false;
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
        at(0x23116c);return false; //Before SUB.S; no difference, output distance, hit or return is claimed.
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
    bool fogSelectorCurrentNative()const noexcept {
        const auto& f=fogPrimary_.selector;const auto& h=servicePrimary_;
        const auto& call=fogPrimary_.entryCall;const auto& inserted=fogPrimary_.insertion;const auto& pub=fogPrimary_.publication;
        if(!f.historyCurrent()||!call.historyCurrent()||!inserted.historyCurrent()||!pub.historyCurrent()||
           (pub.step&&inserted.step!=7)||(call.step&&f.step!=21)||
           (inserted.step&&call.step!=11)||(inserted.lastUsed38&&inserted.lastUsed38!=f.freshLastUsed38)||
           (inserted.incremented38&&inserted.incremented38!=f.nativeOrdinal))return false;
        if(!f.step)return !fogSelectorTable_&&!fogSelectorTableEnd_&&!fogSelectorBlock_&&!fogSelectorEntry_&&!fogSelectorS4_&&!fogInsertionA2_&&!fogSelectorCustodyWithdrawn_;
        if(h.selectionAttempted!=true||h.selectionReturned||h.slot||h.selectionPending==h.selectionCanceled||
           h.counterAdvanced!=inserted.counterStored||h.inserted!=inserted.memberStored||h.published!=(pub.stores==31u)||
           (h.slotWithdrawn&&(!inserted.memberStored||!fogSelectorCustodyWithdrawn_)))return false;
        if(pub.stores&4u){
            if(!serviceHeapMembership_)return false;const auto& observer=*serviceHeapMembership_;
            if(observer.fogSlot14AliasWithdrawn_){if(observer.fogSlot14_)return false;}
            else if(fogSelectorCustodyWithdrawn_||observer.fogSlot14_!=fogSelectorS4_)return false;
        }
        if(fogSelectorCustodyWithdrawn_)return !fogSelectorTable_&&!fogSelectorTableEnd_&&!fogSelectorBlock_&&!fogSelectorEntry_&&!fogSelectorS4_&&!fogInsertionA2_;
        if(h.methodUnpinned||h.nativeWithdrawn||!h.methodPinned||!h.method||!h.selectionPending)return false;
        const auto& c=*h.method;const auto& e=c.sourceEntries_;const auto& s=c.source_;
        if(!c.sourceCollisionEntriesCurrentNative()||e.pending!=&h||e.pendingIdentity!=collider_.get()||
           e.pendingSlot!=f.nativeOrdinal.value_or(0u)||e.pendingConsumed!=inserted.counterStored)return false;
        const auto expectedLast=inserted.counterStored?inserted.incremented38:f.lastUsed38;
        if((f.freeHeadNull&&s.field40!=0u)||(expectedLast&&s.lastUsed38!=expectedLast)||
           (f.perBlock30&&s.perBlock30!=f.perBlock30)||(f.blockCount34&&s.blockCount34!=f.blockCount34))return false;
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
    bool fogPrimaryCurrentNative()const noexcept {
        const auto& f=fogPrimary_;const auto& h=servicePrimary_;
        if(!f.historyCurrent())return false;
        if(!f.step&&!fogGeometry_.step)return true;
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
    struct ServiceAction2A0A30 {
        bool entered=false,dataStored=false,codeStored=false,countIncremented=false,returned=false,nativeWithdrawn=false;
        const void* payload=nullptr;
        std::optional<std::uint32_t> countRead,capacityRead,codeA0;
    } serviceAction_;
    bool installServiceAction2A0A30(const void* actualPayload,std::uint32_t& pc)noexcept {
        auto& h=serviceAction_;
        if(h.entered||!actualPayload||!serviceReturn_.triggerReturned||membershipNativeWithdrawn_||!preflightNative())return false;
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
        pc=pc_=0x2a0a7c;h.returned=true;return true;
    }
    bool serviceActionCurrentNative()const noexcept {
        const auto& h=serviceAction_;
        if(!h.entered)return !h.payload&&!h.countRead&&!h.capacityRead&&!h.codeA0&&!h.dataStored&&
            !h.codeStored&&!h.countIncremented&&!h.returned&&!h.nativeWithdrawn;
        if(!serviceReturn_.triggerReturned||!h.payload||(h.capacityRead&&!h.countRead)||
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

        if((serviceRegistrationAttempted_&&(!serviceReturn_.triggerReturned||!serviceParent_))||
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
           fogPortalVertices_||fogPortalCenterA1_||fogPortalDistanceA2_)std::terminate();
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
