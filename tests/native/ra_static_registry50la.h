#pragma once
#include "ra_static_candidates50la.h"
#include <memory>
#include <utility>

namespace ra_static_registry50LA {
// Bounded source-backed cold construction only. Deliberately has no Storage
// getter, Bank registration, query publication, membership mutation or runtime
// admission API. The later complete RA collider extent also needs its genuine
// trigger/dynamic owners; this static prefix must not be promoted to that extent.
class ColdStage final {
    // Retain candidates/radii and their source until all registration backing
    // dies. Storage also retains the exclusive lease to that same source world.
    std::shared_ptr<const ra_static_candidates50LA::Owner> candidates_;
    track_runtime_storage50IG::Owner storage_;
    bool attempted_=false,constructed_=false;
public:
    struct Diagnostic {
        enum class Status {input,alreadyAttempted,worldLease,storage,constructed} status=Status::input;
        const char* message=nullptr;
        track_runtime_storage50IG::Diagnostic storage;
    };
    ColdStage()=default;
    ColdStage(const ColdStage&)=delete;ColdStage& operator=(const ColdStage&)=delete;
    ColdStage(ColdStage&&)=delete;ColdStage& operator=(ColdStage&&)=delete;
    bool construct(std::shared_ptr<const ra_static_candidates50LA::Owner> candidates,
        Diagnostic& d)noexcept {
        try {
            d={};
            if(attempted_){d.status=Diagnostic::Status::alreadyAttempted;return false;}
            attempted_=true;
            const auto* source=candidates?candidates->decodedSource():nullptr;
            const auto* bindings=candidates?candidates->staticBindings():nullptr;
            if(!source||!bindings||source->collisions.size()!=178||bindings->size()!=178){
                d.message="actual authenticated RA static candidate owner required";return false;
            }
            // Current stage contains exactly the authenticated SCB COL owners.
            // It cannot smuggle an additive trigger/model or accept a substitute
            // source pointer through the generic span interface.
            for(const auto& b:*bindings)if(b.animated||b.trigger){
                d.message="static prefix cannot include unjoined additive owners";return false;
            }
            candidates_=std::move(candidates);
            auto lease=ra_cold_sources50LA::Owner::leaseLiveWorld(candidates_->source_);
            if(!lease){d.status=Diagnostic::Status::worldLease;
                d.message="same-source RA live world already leased or invalid";return false;}
            const track_runtime_storage50IG::SourceWorldView<ra_cold_sources50LA::CollisionOwner>
                view{source->world,{source->collisions.data(),source->collisions.size()}};
            // Preserve existing storage semantics: three query slots are capacity
            // reservations, with zero/unbound source records. This stage neither
            // invents their objects nor includes them in a live query.
            const track_runtime_storage50IG::Limits limits{};
            if(!storage_.prepareSource(view,bindings->data(),bindings->size(),limits,
                d.storage,&lease->world(),lease)){
                d.status=Diagnostic::Status::storage;return false;
            }
            constructed_=true;d.status=Diagnostic::Status::constructed;return true;
        }catch(...){
            // Terminal native failure. No borrowed world mutation is performed
            // by construction; private retained source/candidate prefixes survive.
            d.status=Diagnostic::Status::storage;
            d.message="RA static registry construction failed; no runtime publication";return false;
        }
    }
    bool staticPrefixConstructed()const noexcept{return constructed_;}
    // Cold accounting only. No pointers to mutable world/registry/query backing.
    track_runtime_storage50IG::Counts reservedCounts()const noexcept {
        const auto* s=storage_.get();return s?s->count:track_runtime_storage50IG::Counts{};
    }
};
} // namespace ra_static_registry50LA
