#pragma once
#include "../../src/game/original_application_entities50la.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include <cstdint>
#include <memory>
#include <string>
#include <exception>
#include <type_traits>
#include <new>
#include <optional>
#include <thread>
#include <array>
#include "../../src/assets/original_standard_material_domain50la.h"
#include "../../src/assets/original_pod_source_bank50la.h"
#include "../../src/game/original_player_constructor_prefix50la.h"
#include "../../src/game/original_source_world_rows_borrow50la.h"

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
    friend class ::ra_cold_aggregate50LA::Owner;
    //Future source1AD624 issuer only; NOT called by native initializeSelection,
    //prewarm, current Session or a corpus roster. Outer1AA580 Race/World
    //admission remains a separate missing join, never certified by this method.
    enum class SourceHumansPhase {unreached,arrays,zeroing,human,player,selected,returned,blocked};
    SourceHumansPhase sourceHumansPhase_=SourceHumansPhase::unreached;
    struct SourceSlot {g::OriginalWreckComponent50LA* value=nullptr;bool known=false;};
    std::unique_ptr<SourceSlot[]> sourceArray44_,sourceArray3C_;
    std::unique_ptr<g::OriginalPlayerConstructorPrefix50LA> sourceHumanPlayer_;
    std::shared_ptr<rr::platform::OriginalUiMainProfile50KG> sourceHumanCommon_;
    std::thread::id sourceHumansThread_;
    std::uint32_t sourceHumanIndex_=0,sourceZeroed44_=0,sourceZeroed3C_=0,sourceHumansPC_=0;
    std::optional<std::uint32_t> sourceSelectedHumanPod_;
    std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> sourceHumanDomain_;
    std::unique_ptr<rr::assets::OriginalPodSourceBank50LA> sourceHumanBank_;
    std::array<char,6> sourcePodName_{}; // Known prefix incl NUL only; no invented source stack tail.
    std::string sourcePodPath_;
    bool sourcePodNamesWritten_=false,sourcePodMountAttempted_=false,sourcePodMode1_=false;
    bool sourceHumansAttempted_=false,sourcePlayerLoadAttempted_=false;
    std::shared_ptr<const g::OriginalSourceWorldRowsBorrow50LA> sourceWorldRows_;
    const void* sourceIncomingVisibility_=nullptr;
    const void* sourceIncomingCollision_=nullptr;
    rr::platform::OriginalUiRiderProfileStamp50KG selectionStamp_{};
    rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection selectionSnapshot_{};
    bool selectionCurrentForSourceHuman()const noexcept {
        if(!boundProfile_||!selectionInitialized_)return false;
        const auto stamp=boundProfile_->stamp();
        rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection now;
        if(stamp.generation!=selectionStamp_.generation||stamp.mode34!=selectionStamp_.mode34||
           !boundProfile_->courseRaceSnapshot(now))return false;
        const auto& prior=selectionSnapshot_;
        return now.course==prior.course&&now.planet==prior.planet&&now.rider==prior.rider&&
            now.subtype==prior.subtype&&now.laps==prior.laps&&now.entrants==prior.entrants&&
            now.packed1125C==prior.packed1125C&&now.mode34==prior.mode34;
    }
    template<class Application> bool beginSourceHumans1AE280(const Application& actual,std::uint32_t& pc)noexcept {
        static_assert(std::is_same_v<Application,original_main_entry50KG::Application>);
        pc=0x1ae280;
        if(sourceHumansAttempted_||!live()||!selectionCurrentForSourceHuman()||enrolledRecord_||published40_||
           !actual.enabled||actual.entities()!=application_||actual.riderProfile!=boundProfile_||
           !actual.profile||boundProfile_->commonProfile()!=actual.profile.get())return false;
        rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection now;
        if(!boundProfile_->courseRaceSnapshot(now)||humans34_!=((now.packed1125C>>5)&3u)||
           opponents38_!=((now.packed1125C>>7)&15u)||count30_!=humans34_+opponents38_)return false;
        sourceHumansAttempted_=true;sourceHumansThread_=std::this_thread::get_id();sourceHumanCommon_=actual.profile;sourceHumanDomain_=actual.standardMaterialDomain();
        //Native typed slots adapt guest count30*4 allocations. A host default
        //value is never source-known until its corresponding explicit store.
        sourceArray44_.reset(new(std::nothrow) SourceSlot[count30_]); //1AE2B8 malloc;store44 at1AE2C0.
        sourceHumansPhase_=SourceHumansPhase::arrays;sourceHumansPC_=0x1ae2c0;
        sourceArray3C_.reset(new(std::nothrow) SourceSlot[count30_]); //1AE2C8 operator new[];store3C at1AE2D0.
        sourceHumansPhase_=SourceHumansPhase::zeroing;
        for(std::uint32_t i=0;i<count30_;++i){
            //Original has no NULL branch. Preserve second allocation even if
            //first failed; stop BEFORE offending native null dereference.
            if(!sourceArray44_){sourceHumansPhase_=SourceHumansPhase::blocked;pc=sourceHumansPC_=0x1ae2ec;return false;}
            sourceArray44_[i]={nullptr,true};++sourceZeroed44_;
            if(!sourceArray3C_){sourceHumansPhase_=SourceHumansPhase::blocked;pc=sourceHumansPC_=0x1ae2fc;return false;}
            sourceArray3C_[i]={nullptr,true};++sourceZeroed3C_;
        }
        sourceHumanIndex_=0;
        if(!humans34_){sourceHumansPhase_=SourceHumansPhase::returned;pc=sourceHumansPC_=0x1ae364;return true;}
        sourceHumansPhase_=SourceHumansPhase::human;pc=sourceHumansPC_=0x1ae32c;
        return continueSourceHuman1AE32C(pc);
    }
    bool continueSourceHuman1AE32C(std::uint32_t& pc)noexcept {
        pc=sourceHumansPC_;
        if(!live()||!selectionCurrentForSourceHuman()||std::this_thread::get_id()!=sourceHumansThread_||sourceHumansPhase_!=SourceHumansPhase::human||
           sourceHumanIndex_>=humans34_||sourceHumanIndex_>1||sourceZeroed44_!=count30_||sourceZeroed3C_!=count30_||
           !sourceHumanCommon_||boundProfile_->commonProfile()!=sourceHumanCommon_.get())return false;
        //ONE current source loop occurrence, no caller-supplied index/ID.
        sourceHumansPhase_=SourceHumansPhase::player;sourceHumansPC_=0x1adfd4;
        sourceHumanPlayer_.reset(new(std::nothrow) g::OriginalPlayerConstructorPrefix50LA);
        if(!sourceHumanPlayer_){pc=sourceHumansPC_;sourceHumansPhase_=SourceHumansPhase::blocked;return false;}
        const auto result=sourceHumanPlayer_->beginNativePrefix(application_,entity_);
        if(result.status!=g::OriginalPlayerConstructorPrefix50LA::Status::playerConstructorReturned){
            pc=sourceHumansPC_=result.sourcePC;sourceHumansPhase_=SourceHumansPhase::blocked;return false;
        }
        //Only AFTER exact1A02C0 return does1ADFF4 read actual Profile2DC.
        std::uint32_t selected=0;
        if(!boundProfile_->readHumanPodSelection1ADFF4(*sourceHumanCommon_,sourceHumanIndex_,selected)){
            pc=sourceHumansPC_=sourceHumanIndex_?0x1ae040:0x1ae010;
            sourceHumansPhase_=SourceHumansPhase::blocked;return false;
        }
        sourceSelectedHumanPod_=selected;sourceHumansPhase_=SourceHumansPhase::selected;
        // Source-selected ID is five bits. Exact sprintf output for0..31;
        // no locale, host uninitialized stack read, or caller-selected POD.
        sourcePodName_={{'p','o','d',char('0'+selected/10),char('0'+selected%10),0}};
        try{sourcePodPath_="Pods\\";sourcePodPath_+=sourcePodName_.data();
            sourcePodPath_+='\\';sourcePodPath_+=sourcePodName_.data();sourcePodPath_+=".res";}
        catch(...){pc=sourceHumansPC_=0x1ae078;sourceHumansPhase_=SourceHumansPhase::blocked;return false;}
        sourcePodNamesWritten_=true;
        pc=sourceHumansPC_=0x1ae0a4;return false;
    }
    // Private same-manager occurrence continuation, not a public mount API.
    // Native authenticated custody is required, but does not certify source
    // FileManager registration. OuterRace1AA580 admission remains unjoined.
    bool continueSourcePod1AE0A4(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> archive,
        std::uint32_t& pc)noexcept {
        pc=sourceHumansPC_;
        if(!live()||!selectionCurrentForSourceHuman()||std::this_thread::get_id()!=sourceHumansThread_||
           sourceHumansPhase_!=SourceHumansPhase::selected||!sourcePodNamesWritten_||sourcePodMountAttempted_||
           !sourceHumanPlayer_||!sourceSelectedHumanPod_||!sourceHumanDomain_||!archive||!archive->loaded())return false;
        // 1AE0A4 mounts whatever pod 1ADFF4 selected, by its formatted path.
        // The authenticated catalog row is found by that same disc path; a pod
        // without an authenticated gameplay-archive row is refused, never
        // remapped to Garage or another pod.
        const rr::assets::OriginalUiArchiveIdentity50KG* identity=nullptr;
        for(unsigned ordinal=0;ordinal<256u;++ordinal){
            const auto* row=rr::assets::originalUiArchiveIdentity50KG(static_cast<rr::assets::OriginalUiArchiveId50KG>(ordinal));
            if(!row)break;
            const char* path=row->discPath;std::size_t n=0;bool same=true;
            for(;path&&path[n]&&n<sourcePodPath_.size();++n){
                auto a=static_cast<unsigned char>(path[n]),b=static_cast<unsigned char>(sourcePodPath_[n]);
                if(a=='/')a='\\';if(b=='/')b='\\';
                if(a>='A'&&a<='Z')a=static_cast<unsigned char>(a+('a'-'A'));
                if(b>='A'&&b<='Z')b=static_cast<unsigned char>(b+('a'-'A'));
                if(a!=b){same=false;break;}
            }
            if(!path||!same||path[n]||n!=sourcePodPath_.size())continue;
            if(identity)return false;
            identity=row;
        }
        if(!identity||archive->identity()!=identity)return false;
        const auto resolver=sourceHumanDomain_->resolver3E1A70();
        const auto provider=sourceHumanDomain_->archiveProvider3E1A70();
        if(!resolver||!provider)return false;
        sourcePodMountAttempted_=true;
        sourceHumanBank_.reset(new(std::nothrow) rr::assets::OriginalPodSourceBank50LA);
        if(!sourceHumanBank_){sourceHumansPhase_=SourceHumansPhase::blocked;return false;}
        rr::assets::OriginalStandardTextureDiagnostic50LA d;
        if(!sourceHumanBank_->mount12CEA0(*this,resolver,provider,std::move(archive),sourcePodPath_,d)){
            sourceHumansPhase_=SourceHumansPhase::blocked;return false;
        }
        // Only successful original registration reaches1AE0B4..C4.
        pc=sourceHumansPC_=0x1ae0c0;
        if(!sourceHumanBank_->enterArchiveOnly132A20(d)){sourceHumansPhase_=SourceHumansPhase::blocked;return false;}
        sourcePodMode1_=true;
        //STOP before World4C0 load: still no1ADF80 return or slot publication.
        pc=sourceHumansPC_=0x1ae0c8;return false;
    }
    template<class Application> bool continueSourcePlayer1AE0C8(const Application& actual,
        std::uint32_t& pc)noexcept {
        static_assert(std::is_same_v<Application,original_main_entry50KG::Application>);
        pc=sourceHumansPC_;
        if(!live()||!selectionCurrentForSourceHuman()||sourceHumansThread_!=std::this_thread::get_id()||
           sourceHumansPC_!=0x1ae0c8||sourceHumansPhase_!=SourceHumansPhase::selected||
           sourcePlayerLoadAttempted_||!sourcePodMode1_||!sourceHumanPlayer_||!sourceHumanBank_||
           sourceHumanBank_->nativeClosed_||!sourceHumanBank_->joined_||!sourceHumanBank_->mode1Reached_||
           sourceHumanBank_->occurrence_!=this||!sourceWorldRows_||actual.entities()!=application_||
           actual.riderProfile!=boundProfile_||actual.profile!=sourceHumanCommon_)return false;
        //sourceWorldRows_ currently has NO issuer: real outer Race1AA580→
        //manager admission is unresolved. Node availability or this local PC
        //cannot mint it; native Session/prewarm paths cannot populate it.
        if(!sourceWorldRows_->read1AE0C8(actual,sourceIncomingVisibility_,sourceIncomingCollision_))return false;
        sourcePlayerLoadAttempted_=true;pc=sourceHumansPC_=0x1ae0fc;
        std::uint32_t playerPC=0x1a0570;
        const bool returned=sourceHumanPlayer_->beginPlayerAxes1A0570(actual,sourceHumanIndex_,playerPC);
        pc=sourceHumansPC_=playerPC;
        //Current implementation always bounds at/before210770, so no1ADF80
        //return, array44/3C publication, next human, or source resource claim.
        return returned;
    }
    template<class Application> bool continueSourcePlayerBase210770(const Application& actual,std::uint32_t& pc)noexcept {
        static_assert(std::is_same_v<Application,original_main_entry50KG::Application>);
        pc=sourceHumansPC_;
        if(!live()||!selectionCurrentForSourceHuman()||sourceHumansThread_!=std::this_thread::get_id()||
           sourceHumansPC_!=0x1a0848||!sourcePlayerLoadAttempted_||!sourceHumanPlayer_||!sourceWorldRows_||
           !sourceSelectedHumanPod_||!sourceHumanBank_||sourceHumanBank_->nativeClosed_||!sourceHumanBank_->joined_||
           sourceHumanBank_->occurrence_!=this||actual.entities()!=application_||actual.riderProfile!=boundProfile_||
           actual.profile!=sourceHumanCommon_)return false;
        const bool returned=sourceHumanPlayer_->continueBaseLoad210770(actual,sourceWorldRows_,
            *sourceSelectedHumanPod_,sourceHumanIndex_,pc);
        sourceHumansPC_=pc;return returned;
        //Same unissued outer occurrence; no native route obtains this gate.
    }
    template<class Application> bool continueSourcePlayerOverlay210848(const Application& actual,std::uint32_t& pc)noexcept {
        static_assert(std::is_same_v<Application,original_main_entry50KG::Application>);
        pc=sourceHumansPC_;
        if(!live()||!selectionCurrentForSourceHuman()||sourceHumansThread_!=std::this_thread::get_id()||
           sourceHumansPC_!=0x210848||!sourcePlayerLoadAttempted_||!sourceHumanPlayer_||!sourceWorldRows_||
           !sourceHumanBank_||sourceHumanBank_->nativeClosed_||!sourceHumanBank_->joined_||
           sourceHumanBank_->occurrence_!=this||actual.entities()!=application_||actual.riderProfile!=boundProfile_||
           actual.profile!=sourceHumanCommon_)return false;
        const bool returned=sourceHumanPlayer_->continueOverlay210848(actual,sourceWorldRows_,pc);
        sourceHumansPC_=pc;return returned; //No outer issuer/full Player return.
    }
    template<class Application> bool continueSourcePlayerOverlayArrays199990(const Application& actual,std::uint32_t& pc)noexcept {
        static_assert(std::is_same_v<Application,original_main_entry50KG::Application>);
        pc=sourceHumansPC_;
        if(!live()||!selectionCurrentForSourceHuman()||sourceHumansThread_!=std::this_thread::get_id()||
           sourceHumansPC_!=0x199990||!sourcePlayerLoadAttempted_||!sourceHumanPlayer_||!sourceWorldRows_||
           !sourceHumanBank_||sourceHumanBank_->nativeClosed_||!sourceHumanBank_->joined_||
           sourceHumanBank_->occurrence_!=this||actual.entities()!=application_||actual.riderProfile!=boundProfile_||
           actual.profile!=sourceHumanCommon_)return false;
        const bool returned=sourceHumanPlayer_->continueOverlayArrays199990(actual,sourceWorldRows_,pc);
        sourceHumansPC_=pc;return returned; //No outer issuer/full Player return.
    }
    template<class Application> bool continueSourcePlayerPointerArrays210880(const Application& actual,std::uint32_t& pc)noexcept {
        static_assert(std::is_same_v<Application,original_main_entry50KG::Application>);
        pc=sourceHumansPC_;
        if(!live()||!selectionCurrentForSourceHuman()||sourceHumansThread_!=std::this_thread::get_id()||
           sourceHumansPC_!=0x210880||!sourcePlayerLoadAttempted_||!sourceHumanPlayer_||!sourceWorldRows_||
           !sourceHumanBank_||sourceHumanBank_->nativeClosed_||!sourceHumanBank_->joined_||
           sourceHumanBank_->occurrence_!=this||actual.entities()!=application_||actual.riderProfile!=boundProfile_||
           actual.profile!=sourceHumanCommon_)return false;
        const bool returned=sourceHumanPlayer_->continuePointerArrays210880(actual,sourceWorldRows_,pc);
        sourceHumansPC_=pc;return returned; //No outer issuer/full Player return.
    }
    bool cancelSourceHumansNative()noexcept {
        if(!sourceHumansAttempted_)return true;
        if(std::this_thread::get_id()!=sourceHumansThread_)return false;
        //Only the explicitly known-zero prefix is observed; unknown host
        //slot contents are never dereferenced or released as source pointers.
        for(std::uint32_t i=0;i<sourceZeroed44_;++i)
            if(!sourceArray44_||!sourceArray44_[i].known||sourceArray44_[i].value)return false;
        for(std::uint32_t i=0;i<sourceZeroed3C_;++i)
            if(!sourceArray3C_||!sourceArray3C_[i].known||sourceArray3C_[i].value)return false;
        //Cancel Player's typed input/axis backing before releasing retained
        //World rows and POD root. Native partial cleanup, not source destructor.
        if(sourceHumanPlayer_&&!sourceHumanPlayer_->cancelNativePartial())return false;
        sourceHumanPlayer_.reset();sourceWorldRows_.reset();
        sourceIncomingVisibility_=sourceIncomingCollision_=nullptr;
        if(sourceHumanBank_){rr::assets::OriginalStandardTextureDiagnostic50LA d;
            if(!sourceHumanBank_->closeNative(d))return false;sourceHumanBank_.reset();}
        sourceArray3C_.reset();sourceArray44_.reset();sourceHumanCommon_.reset();sourceHumanDomain_.reset();
        sourceZeroed44_=sourceZeroed3C_=0;sourceSelectedHumanPod_.reset();return true;
    }

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
        selectionInitialized_=true;selectionStamp_=profile.stamp();selectionSnapshot_=next;actual=next;return true;
    }
    bool enrollRecord(g::OriginalWreckComponent50LA& actor)noexcept {
        if(sourceHumansAttempted_||!live()||enrolledRecord_||published40_||actor.sessionGeneration!=generation_||
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
        if(sourceHumansAttempted_&&(!live()||enrolledRecord_||published40_||recordAttached_||
           entity_.parent1C||entity_.previous10||entity_.next14||entity_.firstChild18||entity_.descendants24))return false;
        if(!cancelSourceHumansNative())return false;
        if(constructed_){
            if(!live()||enrolledRecord_||published40_||recordAttached_||entity_.parent1C||
               entity_.previous10||entity_.next14||entity_.firstChild18||entity_.descendants24||
               !application_->objectCount423F60())return false;
            //Native source-array/partial-Player cancellation completed above.
            //No full1AD490 array/Player destructor is claimed; the real
            //1413E0 suffix releases this counted Entity after the child owner.
            if(!application_->entityAuthority_.retire(entity_))std::terminate();
            --entity_.reference00;--application_->objectCount423F60();
        }
        closed_=true;boundProfile_.reset();application_.reset();return true;
    }
};
}
