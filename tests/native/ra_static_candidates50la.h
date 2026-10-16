#pragma once
#include "ra_cold_sources50la.h"
#include "track_static_candidates50ig_fixture.h"
#include "ra_trigger_candidates50la.h"
namespace ra_cold_aggregate50LA { class Owner; }
#include <memory>
#include <utility>

namespace ra_cold_aggregate50LA { class Owner; }
namespace ra_static_candidates50LA {
// Owns only genuine source-backed static candidate/radii/filter storage.
// It does not create world registries, query identities, pod shapes, membership,
// Course admission or a source parent. Root supplies actual namespace bindings.
class Owner final {
    friend class ra_cold_aggregate50LA::Owner;
    friend class ra_static_registry50LA::ColdStage;
    friend class ra_cold_aggregate50LA::Owner;
    friend class ra_source_camera50LA::OwnershipBridge;
    // Declared first: every candidate/radius borrow dies before source backing.
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::vector<rr::game::Type2StaticCandidate49X> candidates_;
    std::vector<track_static_candidates50IG::Radii> radii_;
    std::vector<track_runtime_storage50IG::Binding> bindings_;
    track_static_candidates50IG::Bindings names_{};
    std::shared_ptr<const ra_trigger_authoring50LA::Owner> triggerSource_;
    std::shared_ptr<ra_trigger_actions50LA::Owner> triggerActions_;
    bool triggerAttempted_=false;
    bool prepareTriggers(std::shared_ptr<const ra_trigger_authoring50LA::Owner> source) {
        if(!prepared_||triggerAttempted_||!source||!source->sources())return false;
        triggerAttempted_=true;
        triggerSource_=std::move(source);triggerActions_=std::make_shared<ra_trigger_actions50LA::Owner>();
        for(const auto& c:candidates_)for(std::uint32_t i=0;i<2;++i)
            if(c.candidate.collision.filter.identity==ra_trigger_actions50LA::firstTriggerIdentity+i)return false;
        return ra_trigger_candidates50LA::prepare(*triggerSource_,*triggerActions_,candidates_,bindings_,
            names_.collisionTypeTag,names_.preservedFlags31,names_.preservedFlagsF8)==178u;
    }
    bool attempted_=false,prepared_=false;
public:
    struct Diagnostic {
        enum class Status {complete,alreadyAttempted,source,inputOrAllocation} status=Status::source;
        const char* message=nullptr;
    };
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    bool prepare(std::shared_ptr<const ra_cold_sources50LA::Owner> source,
        const track_static_candidates50IG::Bindings& actualNames,Diagnostic& d)noexcept {
        d={};
        if(attempted_){d.status=Diagnostic::Status::alreadyAttempted;return false;}
        attempted_=true;
        const auto* decoded=source?source->sourcesDecoded():nullptr;
        if(!decoded||decoded->collisions.size()!=178||!decoded->map.indicesPrepared||
           !decoded->ai.indicesPrepared||!decoded->mapLap.prepared){
            d.message="actual RA decoded collision/navigation owner required";return false;
        }
        // Retain actual source before publishing any pointer into its meshes.
        source_=std::move(source);
        try {
            track_static_candidates50IG::prepareSpan(decoded->collisions.data(),
                decoded->collisions.size(),actualNames,candidates_,radii_,bindings_);
            names_=actualNames;prepared_=true;d.status=Diagnostic::Status::complete;return true;
        }catch(...){
            d.status=Diagnostic::Status::inputOrAllocation;
            d.message="RA static source candidate preparation rejected; private prefix retained";
            return false;
        }
    }
    // Synchronous cold registry input only. Each binding points at this owner's
    // stable candidate and actual source mesh. Candidate mutation belongs only
    // to the future exclusive serial registry/query adapter, never resizing.
    const std::vector<track_runtime_storage50IG::Binding>* staticBindings()const noexcept {
        return prepared_?&bindings_:nullptr;
    }
    const ra_cold_sources50LA::DecodedSources* decodedSource()const noexcept {
        return prepared_?source_->sourcesDecoded():nullptr;
    }
    // The future live registry owner must strongly retain this owner until all
    // registrations/queries retire. A borrowed bindings pointer cannot do that.
};
} // namespace ra_static_candidates50LA
