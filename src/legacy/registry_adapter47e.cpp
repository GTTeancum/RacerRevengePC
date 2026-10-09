#include "registry_adapter47e.h"
#include "compact_registry.h"
#include "registry_replacements.h"
#include "registry_metadata47e.h"
#include <new>
#include <stdexcept>

extern const rr::RegistryPage rr47e_pages[];
extern const uint8_t rr47e_patterns[];
extern const uint16_t rr47e_owners[];
extern PS2Runtime::RecompiledFunction const rr47e_bindings[];
namespace rr47e {
namespace {
using Replacements=rr::RegistryReplacements<PS2Runtime::RecompiledFunction>;
struct State {
    Replacements::Page* directory[kPages]={};
    rr::CompactRegistry original;
    Replacements replacements;
    static Replacements::Page* allocate(void*) { return new(std::nothrow) Replacements::Page; }
    static void release(void*,Replacements::Page* page) { delete page; }
    State() {
        if(!original.configure(kBase,kSlots,rr47e_pages,kPages,rr47e_patterns,kPatterns,
                               rr47e_owners,kOwners,kSymbols) ||
           !replacements.configure(kBase,kSlots,directory,kPages,allocate,release,nullptr))
            throw std::logic_error("Invalid compact original function registry");
    }
};
State& state() { static State value; return value; }
}
PS2Runtime::RecompiledFunction lookup(uint32_t address) {
    auto& s=state();
    PS2Runtime::RecompiledFunction fn=nullptr;
    if(s.replacements.get(address,fn)) return fn;
    return rr47e_bindings[s.original.lookup(address)];
}
bool replace(uint32_t address,PS2Runtime::RecompiledFunction function) {
    return state().replacements.set(address,function);
}
uint32_t allocatedPages() { return state().replacements.allocatedPages(); }
uint32_t directoryBytes() { return sizeof(state().directory); }
uint32_t replacementBytes() { return allocatedPages()*sizeof(Replacements::Page); }
}
