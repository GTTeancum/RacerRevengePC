#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <optional>
#include <vector>
// Isolated protocol fixture sharing the actual file-read methods and state. This does not
// instantiate the production archive service or prove its runtime ownership.
enum class E {phase,allocation,unsupported};
struct D {std::uint32_t sourcePC=0;};
struct ServiceObservation {std::uint32_t memberCursor10=0;std::vector<std::uint32_t> devices;};
class OriginalWaterPoolSourceFile50LA {
public:
    struct PriorRead {bool resultAdmitted=true,afterObserved=true;std::size_t copied=4;
        std::array<std::uint8_t,4> bytes{};ServiceObservation after;};
    struct Prior {const void* dataIdentity=nullptr;const void* objectIdentity=nullptr;
        const void* rowsIdentity=nullptr;std::uint64_t dataEpoch=1;PriorRead read;} innerRowCount_;
#include "../../src/assets/original_water_pool_row_input_state50la.inl"
    struct Resolver {
        OriginalWaterPoolSourceFile50LA* file;
        bool readSourceBankMember12FB30(const void*,const void*,std::int32_t,void* output,
            std::size_t requested,std::size_t& copied,D& d)noexcept {
            auto& f=*file;const auto amount=std::min<std::size_t>(requested,f.selectedLength_-f.cursor);
            copied=f.failAfter?std::min(amount,*f.failAfter):amount;
            for(std::size_t i=0;i<copied;++i)static_cast<std::uint8_t*>(output)[i]=std::uint8_t((f.cursor+i)%251u+1u);
            f.deviceCursor+=std::uint32_t(copied);f.readCalled=true;
            if(f.failAfter){d.sourcePC=0x133580u;return false;}
            f.cursor+=std::uint32_t(copied)+(f.badAdvance?1u:0u);return true;
        }
    } service{this};
    Resolver* resolver_=&service;
    const void* selectedRoot_=this;const void* poolIdentity_=this;
    std::int32_t nativeHandle_=0,deviceCount_=1;
    std::uint32_t selectedLength_=0,cursor=4,deviceCursor=4;
    bool nativeClosed_=false,nativeCloseAttempted_=false,sourceCloseAttempted_=false;
    bool readCalled=false,missingAfter=false,badAdvance=false;
    std::optional<std::size_t> failAfter;
    static bool fail(D& d,E,std::uint32_t pc)noexcept {d.sourcePC=pc;return false;}
    bool preflightNative(const void*)const noexcept {return rowInputsHistoryNative();}
    bool observeServiceNative(ServiceObservation& s)const noexcept {
        if(readCalled&&missingAfter)return false;
        s.memberCursor10=cursor;s.devices[0]=deviceCursor;return true;
    }
    void setup(std::uint32_t count,std::uint32_t available){
        innerRowCount_.dataIdentity=&cursor;innerRowCount_.objectIdentity=&deviceCursor;
        innerRowCount_.rowsIdentity=&selectedLength_;innerRowCount_.read.after.memberCursor10=4;
        for(unsigned i=0;i<4;++i)innerRowCount_.read.bytes[i]=std::uint8_t(count>>(8u*i));
        selectedLength_=4u+available;
    }
    bool prepare(std::uint32_t count,D& d){return prepareRowInputsNative(&cursor,&deviceCursor,&selectedLength_,this,1,count,d);}
};
#include "../../src/assets/original_water_pool_row_inputs50la.inl"
int main(){
    using F=OriginalWaterPoolSourceFile50LA;D d;
    {
        F f;f.setup(3,132);assert(f.prepare(3,d)&&f.rowInputsHistoryNative());
        assert(!f.readRowInput12FB30(true,0,d)); //No normals before bulk.
        assert(f.readRowInput12FB30(false,0,d)&&f.rowInputs_.vertices.copied==96);
        assert(f.rowInputs_.vertices.sourceItems==3&&f.rowInputsHistoryNative());
        assert(!f.readRowInput12FB30(true,1,d)); //No skipped normal.
        for(unsigned i=0;i<3;++i){assert(f.readRowInput12FB30(true,i,d));
            assert(f.rowInputs_.normals[i].copied==12&&f.rowInputsHistoryNative());}
        assert(f.latestRowInputReadNative()==&f.rowInputs_.normals[2]);
        assert(f.cursor==136&&!f.readRowInput12FB30(true,3,d));
        assert(!f.readRowInput12FB30(false,0,d)&&!f.prepare(3,d));
    }
    //Every bulk prefix, including less than one complete32-byte item.
    for(unsigned available=0;available<=96u;++available){
        F f;f.setup(3,available);assert(f.prepare(3,d)&&f.readRowInput12FB30(false,0,d));
        const auto& r=f.rowInputs_.vertices;
        assert(r.copied==available&&r.sourceItems==(available?3:0)&&f.rowInputsHistoryNative());
        for(unsigned i=available;i<96u;++i)assert(r.bytes[i]==0); //Bookkeeping only.
        for(unsigned i=0;i<3u;++i){assert(f.readRowInput12FB30(true,i,d));assert(f.rowInputs_.normals[i].sourceItems==0);}
        assert(f.rowInputsHistoryNative());
    }
    for(unsigned length=0;length<=12u;++length){
        F f;f.setup(3,96u+length);assert(f.prepare(3,d)&&f.readRowInput12FB30(false,0,d));
        assert(f.readRowInput12FB30(true,0,d));const auto& r=f.rowInputs_.normals[0];
        assert(r.copied==length&&r.sourceItems==(length?1:0)&&f.rowInputsHistoryNative());
        for(unsigned j=length;j<12u;++j)assert(r.bytes[j]==0);
    }
    for(unsigned count : {16385u,0xffffffffu}){
        F f;f.setup(count,0);assert(!f.prepare(count,d)&&!f.rowInputs_.entered&&f.rowInputsHistoryNative());
    }
    {
        F f;f.setup(3,132);assert(!f.prepare(2,d)&&!f.rowInputs_.entered);
        f.innerRowCount_.read.copied=3;assert(!f.prepare(3,d)&&!f.rowInputs_.entered);
    }
    {
        F f;f.setup(3,132);f.failAfter=5;assert(f.prepare(3,d));
        assert(!f.readRowInput12FB30(false,0,d));const auto& r=f.rowInputs_.vertices;
        assert(r.copied==5&&r.after.memberCursor10==4&&r.after.devices[0]==9&&r.bytes[4]==9);
        assert(!r.sourceItems&&f.rowInputsHistoryNative()&&!f.readRowInput12FB30(true,0,d));
    }
    {
        F f;f.setup(3,132);f.missingAfter=true;assert(f.prepare(3,d));
        assert(!f.readRowInput12FB30(false,0,d)&&!f.rowInputs_.vertices.afterObserved&&f.rowInputsHistoryNative());
    }
    {
        F f;f.setup(3,132);f.badAdvance=true;assert(f.prepare(3,d));
        assert(!f.readRowInput12FB30(false,0,d)&&!f.rowInputs_.vertices.sourceItems&&f.rowInputsHistoryNative());
    }
    {
        F f;f.setup(3,132);assert(f.prepare(3,d)&&!f.latestRowInputReadNative());
        assert(f.readRowInput12FB30(false,0,d)&&f.latestRowInputReadNative()==&f.rowInputs_.vertices);
        f.readCalled=false;f.missingAfter=true;
        assert(!f.readRowInput12FB30(true,0,d));
        assert(f.latestRowInputReadNative()==&f.rowInputs_.normals[0]);
        assert(!f.latestRowInputReadNative()->afterObserved&&f.rowInputsHistoryNative());
    } //Missing normal after-state must not select the older bulk cursor.
    {
        F f;f.setup(0,0);assert(f.prepare(0,d)&&f.readRowInput12FB30(false,0,d));
        assert(f.rowInputs_.vertices.sourceItems==0&&f.rowInputsHistoryNative());
    }
    std::puts("Row read protocol: bulk prefixes0..96, item returns, sequencing, partial failure, cursor mismatch and missing observation passed");
}
