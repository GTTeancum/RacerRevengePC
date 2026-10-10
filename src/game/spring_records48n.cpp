#include "spring_records48n.h"
#include <cstring>
#include <limits>
#include <type_traits>

namespace rr::game {
namespace {
static_assert(sizeof(float)==4,"Record payload requires binary32 storage");
static_assert(std::is_trivially_copyable<SpringRecord48N>::value,"Records must be allocation-free");
static_assert(sizeof(SpringRecord48N)==0x60,"Every original record byte is exposed");
static_assert(offsetof(SpringRecord48N,scene40)==0x40&&offsetof(SpringRecord48N,other44)==0x44&&
    offsetof(SpringRecord48N,margin48)==0x48&&offsetof(SpringRecord48N,material4C)==0x4c&&
    offsetof(SpringRecord48N,flags50)==0x50&&offsetof(SpringRecord48N,preserved51)==0x51,
    "Record field positions must remain explicit");
struct Range { std::uintptr_t begin,end; };
bool range(const void* pointer,std::size_t count,std::size_t width,Range& out) noexcept {
    out={0,0};if(!count)return true;
    if(!pointer||count>std::numeric_limits<std::size_t>::max()/width)return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(pointer),bytes=count*width;
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-begin)return false;
    out={begin,begin+bytes};return true;
}
bool overlap(Range a,Range b) noexcept {return a.begin<b.end&&b.begin<a.end;}
}
SpringRecordStatus48N appendSpringRecord48N(const SpringRecordAppend48N& input,
    SpringRecord48N* records,std::size_t storageCapacity,SpringRecordState48N& state) noexcept {
    Range data{},source{},metadata{};
    if(!range(records,storageCapacity,sizeof(*records),data)||
       !range(&input,1,sizeof(input),source)||!range(&state,1,sizeof(state),metadata)||
       (storageCapacity&&reinterpret_cast<std::uintptr_t>(records)%alignof(SpringRecord48N))||
       overlap(data,source)||overlap(data,metadata)||overlap(source,metadata))
        return SpringRecordStatus48N::InvalidInput;
    if(state.generatedCount84<0||state.generatedCapacity88<0)return SpringRecordStatus48N::InvalidInput;
    if(storageCapacity<static_cast<std::size_t>(state.generatedCapacity88))return SpringRecordStatus48N::InsufficientCapacity;
    if(state.generatedCount84>=state.generatedCapacity88)return SpringRecordStatus48N::Saturated;
    // The original stages all four vectors before its stores. Byte copies
    // preserve NaN payloads (including signaling NaNs), signed zero and W lanes.
    SpringRecordAppend48N staged;
    std::memcpy(&staged,&input,sizeof(staged));
    auto& record=records[static_cast<std::size_t>(state.generatedCount84)];
    std::memcpy(&record.field00,&staged.field00,sizeof(SpringVector4_48N));
    std::memcpy(&record.localPoint10,&staged.localPoint10,sizeof(SpringVector4_48N));
    std::memcpy(&record.normal20,&staged.normal20,sizeof(SpringVector4_48N));
    std::memcpy(&record.surfacePoint30,&staged.surfacePoint30,sizeof(SpringVector4_48N));
    record.other44=staged.other44;record.scene40=staged.scene40;record.material4C=staged.material4C;
    ++state.generatedCount84;state.lastMaterialD4=staged.material4C;
    return SpringRecordStatus48N::Appended;
}
void resetSpringRecords48N(SpringRecordState48N& state) noexcept {
    state.generatedCount84=0;state.pending38=1;state.currentCandidate130=0;
    state.contact3C=0;state.point74=0;state.generated78=0;state.upward40=0;state.other44=0;
}
}
