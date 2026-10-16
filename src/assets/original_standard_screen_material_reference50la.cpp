#include "original_standard_screen_material_reference50la.h"
#include <exception>
#include <new>
#include <utility>

namespace rr::assets {
namespace {
using D=OriginalStandardMaterialDiagnostic50LA;
using S=OriginalStandardMaterialStatus50LA;
bool failScreen(D& d,S status,std::uint32_t pc) noexcept {
    d.status=status;d.sourcePC=pc;return false;
}
}
OriginalStandardScreenMaterialReference50LA::~OriginalStandardScreenMaterialReference50LA() {
    release299AC0();
}
bool OriginalStandardScreenMaterialReference50LA::acceptsAIDamageDirectory299BE0(
    const char* directory,std::size_t capacity) noexcept {
    if(!directory||!capacity)return false;
    std::size_t n=0;
    while(n<capacity&&n<256u&&directory[n])++n;
    if(n==capacity||n==256u)return false;
    if(n&&directory[n-1]=='\\')--n;
    constexpr char expected[]="textures";
    if(n!=sizeof(expected)-1u)return false;
    for(std::size_t i=0;i<n;++i){
        const unsigned char c=static_cast<unsigned char>(directory[i]);
        const auto lowered=c>='A'&&c<='Z'?static_cast<unsigned char>(c+('a'-'A')):c;
        if(lowered!=static_cast<unsigned char>(expected[i]))return false;
    }
    return true;
}
bool OriginalStandardScreenMaterialReference50LA::acquireAIDamage299BE0(
    std::shared_ptr<OriginalStandardMaterialTable50LA> table,
    const char* directory,std::size_t capacity,D& d) noexcept {
    d={};if(started_)return failScreen(d,S::alreadyPrepared,0x299BE0);
    if(!table||!table->prepared()||!acceptsAIDamageDirectory299BE0(directory,capacity))
        return failScreen(d,S::input,0x299BE0);
    if(table->populationTerminal())return failScreen(d,S::phase,0x14D370);
    if(table->demandTerminal())return failScreen(d,S::phase,0x24A430);
    try {
        // This bounded literal/path has no dot, so299C74's suffix removal
        // genuinely does nothing. Preserve the actual caller's path spelling.
        std::string requested=directory;
        if(requested.back()!='\\')requested+='\\';
        requested+="AIDamage";
        table_=std::move(table);started_=true;
        std::int32_t found=-1;
        const bool lookedUp=table_->find298C80(requested,found,d);
        material00_=found; //299CB4, before the assertion and14D590 retain.
        d.row=material00_;d.storedSlots=1;d.sourceEffectsCommitted=true;
        if(!lookedUp)return false;
        if(!table_->retainScreen14D590(material00_,d))return false;
        retained_=true;d.retainedSlots=1;return true;
    }catch(const std::bad_alloc&){return failScreen(d,S::allocation,0x299BE0);}
    catch(...){return failScreen(d,S::input,0x299BE0);}
}
void OriginalStandardScreenMaterialReference50LA::release299AC0() noexcept {
    if(!retained_)return;
    if(!table_||!table_->release14D500(material00_))std::terminate();
    retained_=false;
    // Keep the source token and table pin for postmortem inspection. Native
    // destruction subsequently releases the pin; no row is reset or erased.
}
}
