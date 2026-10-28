#pragma once
#include "../../src/game/original_application_entities50la.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include <cstdint>
#include <memory>
#include <string>
#include <exception>
#include <type_traits>

namespace original_racer_manager_entity {
namespace g=rr::game;
// Actual race+B0 owner:1AA548 allocates0x48,1AA55C calls1AD440.
// This is NOT the separate1AF7F0 timing/rank manager. Its genuine base
// Entity supplies the parent consumed by1AEA1C; no Camera/Application alias.
class Owner final {
    std::shared_ptr<g::OriginalApplicationEntities50LA> application_;
    g::OriginalWreckComponent50LA entity_;
    g::OriginalWreckComponent50LA* enrolledRecord_=nullptr;
    g::OriginalWreckComponent50LA* published40_=nullptr;
    std::uint64_t generation_=0;
    std::shared_ptr<rr::platform::OriginalUiRiderProfileOwner50KG> boundProfile_;
    bool constructed_=false,closed_=false,recordAttached_=false,selectionInitialized_=false;
    // Constructor1AD464..474 clears these lanes.40 is NOT source-initialized;
    // published40_ is a native validity envelope until1AEA34 actually writes it.
    std::uint32_t count30_=0,humans34_=0,opponents38_=0;
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    bool live()const noexcept {
        return constructed_&&!closed_&&application_&&application_->application10()&&
            !application_->attachmentLock423F58()&&entity_.reference00==1&&
            entity_.sessionGeneration==generation_&&entity_.objectKey==key(&entity_)&&
            application_->entityAuthority_.canRetire(entity_)&&
            g::original_entity_chain_detail50LA::forest(application_->entityAuthority_.view());
    }
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner();
private:
    bool constructEntity(std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        std::uint64_t generation,std::string& error)noexcept {
        if(constructed_||closed_||!generation||!application||!application->application10()||
           application->attachmentLock423F58()||
           !application->entityAuthority_.canEnroll(entity_,key(&entity_))){
            error="Racer manager requires a fresh actual Application Entity owner";return false;}
        application_=std::move(application);generation_=generation;
        const auto result=g::initializeOriginalWreckComponent50LA(entity_,
            g::OriginalWreckComponentKind50LA::entity141300,generation_,key(&entity_),true,
            application_->objectCount423F60()); //1AD450 a1=1.
        if(result.status!=g::OriginalWreckComponentStatus50LA::complete){
            error="Racer manager1AD440 Entity construction failed";return false;}
        constructed_=true;
        if(!application_->entityAuthority_.enroll(entity_))std::terminate();
        return true;
    }
public:
    // Bind the real Application's pair, not independently supplied matching
    // pointers. The dependent body avoids a circular include of Application.
    template<class Application> bool construct(const Application& actual,
        std::uint64_t generation,std::string& error)noexcept {
        static_assert(std::is_same_v<Application,original_main_entry50KG::Application>,
            "Racer manager requires the actual native Application");
        if(!actual.enabled||!actual.riderProfile){error="Racer manager requires the actual retained profile";return false;}
        if(!constructEntity(actual.entities(),generation,error))return false;
        boundProfile_=actual.riderProfile;return true;
    }
    // First entry: after original Course matches(), before runtime roster
    // derivation. Restart: validate its current retained runtime snapshot.
    // No broader counted Race constructor or forest is claimed here. A fresh
    // manager reexecutes the real subtype3 clear on an already-clear profile.
    bool initializeSelection(rr::platform::OriginalUiRiderProfileOwner50KG& profile,
        const rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection& expected,
        rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection& actual)noexcept {
        if(!live()||selectionInitialized_||enrolledRecord_||boundProfile_.get()!=&profile)return false;
        rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection next;
        if(!profile.commitRacerManager1AD520(expected,next))return false;
        humans34_=(next.packed1125C>>5u)&3u;
        opponents38_=(next.packed1125C>>7u)&15u;count30_=humans34_+opponents38_;
        selectionInitialized_=true;actual=next;return true;
    }
    bool enrollRecord(g::OriginalWreckComponent50LA& actor)noexcept {
        if(!live()||enrolledRecord_||published40_||actor.sessionGeneration!=generation_||
           actor.reference00!=1||actor.parent1C||actor.previous10||actor.next14||actor.firstChild18||
           !application_->entityAuthority_.canEnroll(actor,actor.objectKey))return false;
        if(!application_->entityAuthority_.enroll(actor))return false;
        enrolledRecord_=&actor;return true;
    }
    bool attachRecord(g::OriginalWreckComponent50LA& actor)noexcept {
        if(!live()||enrolledRecord_!=&actor||recordAttached_||published40_)return false;
        const auto result=application_->entityAuthority_.attachChildren(entity_,actor,2u,
            application_->attachmentLock423F58()); //1AEA1C, source mask2.
        if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
        recordAttached_=true;return true;
    }
    bool publishRecord(g::OriginalWreckComponent50LA& actor)noexcept {
        if(!live()||enrolledRecord_!=&actor||!recordAttached_||published40_||
           actor.parent1C!=&entity_||actor.mask20!=2u||(actor.flags29&3u))return false;
        published40_=&actor;return true; //1AEA34, after real141520.
    }
    bool ownsRecord(const g::OriginalWreckComponent50LA& actor,std::uint64_t generation)const noexcept {
        return live()&&generation==generation_&&enrolledRecord_==&actor&&published40_==&actor&&
            recordAttached_&&actor.parent1C==&entity_&&actor.mask20==2u&&
            actor.sessionGeneration==generation_&&actor.reference00==1;
    }
    const g::OriginalWreckComponent50LA* recordEntity()const noexcept{return published40_;}
    const auto& application()const noexcept{return application_;}
    std::uint64_t generation()const noexcept{return generation_;}
    std::uint32_t count30()const noexcept{return count30_;}
    std::uint32_t humans34()const noexcept{return humans34_;}
    std::uint32_t opponents38()const noexcept{return opponents38_;}
    bool retireRecord(g::OriginalWreckComponent50LA& actor)noexcept {
        if(!live())return false;
        // Failure before enrollment leaves the actor owner solely responsible
        // for its constructed reference. Never retire another owner's registry row.
        if(!enrolledRecord_)return !published40_&&!recordAttached_&&
            actor.sessionGeneration==generation_&&!actor.parent1C&&!actor.previous10&&!actor.next14;
        if(enrolledRecord_!=&actor||actor.firstChild18||actor.reference00!=1||
           !application_->entityAuthority_.canRetire(actor))return false;
        if(recordAttached_){
            if(actor.parent1C!=&entity_)return false;
            if(application_->entityAuthority_.detach(actor,application_->attachmentLock423F58()).status!=
               g::OriginalWreckComponentStatus50LA::complete)return false;
            recordAttached_=false;
        }
        if(!application_->entityAuthority_.retire(actor))std::terminate();
        published40_=nullptr;enrolledRecord_=nullptr;return true;
    }
    bool close()noexcept {
        if(closed_)return true;
        if(constructed_){
            if(!live()||enrolledRecord_||published40_||recordAttached_||entity_.parent1C||
               entity_.previous10||entity_.next14||entity_.firstChild18||entity_.descendants24||
               !application_->objectCount423F60())return false;
            //1AD490 has no44/3C allocations in this cold prefix; its real
            //1413E0 suffix releases this counted Entity after the child owner.
            if(!application_->entityAuthority_.retire(entity_))std::terminate();
            --entity_.reference00;--application_->objectCount423F60();
        }
        closed_=true;boundProfile_.reset();application_.reset();return true;
    }
};
}
