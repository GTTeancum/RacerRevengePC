#pragma once
#include "original_wreck_components50la.h"
namespace rr::game {
// The actual manager101330 instance, not another scheduling proxy. Its owner
// constructs this ONE Entity through the retained Application authority.
// Global3DF400 retains the address of this exact object. Destructor101FE0 does
// NOT clear that source global; native liveness must be checked separately and
// a stale source identity must never be dereferenced after its owner dies.
struct OriginalGarageAnimationManagerFields50GN {
    OriginalWreckComponent50LA entity;
    float delta30=0.f,scale34=1.f;
    // Actual retained ToyList2D9B40 Entity identity, set by101380 only after the
    // ten actor records have been constructed in authored order.
    OriginalWreckComponent50LA* toys38=nullptr;
};
}
