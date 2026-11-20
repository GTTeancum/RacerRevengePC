#pragma once
namespace original_main_entry50KG { struct Application; }
#include "track_session_assets50ig_fixture.h"
#include "original_course_roster50kg_fixture.h"
#include "../../src/game/original_course_load_selection50kg.h"
#include "../../src/game/dlc_rider_entries50dl.h"
#include "../../src/game/dlc_course_entries50dl.h"
#include "runtime_startup50fl_fixture.h"
#include "pod_materials50ed_fixture.h"
#include "course_archive_receipt50ci.h"
#include "course_cut_identity50ci.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include "../../platform/linux/native/host_track_selection50im.h"
#include <array>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace original_course_targets50KX {
using Selection=rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection;
using Track=rr::platform::HostTrack50IM;
// Common rewrite (I1, PLAN B-C02): every course is admitted by ONE rule. The
// original launches a race from the Course screen for any course (17C16C /
// 17D678 -> 1A65C0); there is no admission list. A course is available when its
// archive resolves (stock disc or the registry dlc:/ overlay) and its integrity
// receipt is logged (course_archive_receipt50ci.h, receipt only). Course and
// pod differ only in data: no target below selects code.
// RETIRING (kept only for the R11 frontend / I3 publication readers until they
// drop the call): every host selection is a direct human course now.
inline constexpr bool directHumanTrack(Track track) noexcept {
    return static_cast<unsigned>(track)<=static_cast<unsigned>(Track::Course);
}
struct Target {
    Track track;    // host menu/harness selection key only (W0-HANDOFF 3.1); race code reads the code
    const track_session_assets50IG::SessionAssets* assets;
    std::uint32_t course,planet;
};
// The stock Course-screen table: UICourse course/planet number -> course row.
// Data, like the original's course table (file selection by code, A4); the
// rows carry no behaviour. Every row is admitted by the one rule above.
inline const std::array<Target,13> targets{{
    {Track::TA,&track_session_assets50IG::retainedTA,0,0},
    {Track::SA,&track_session_assets50IG::retainedSA,11,4},
    {Track::MA,&track_session_assets50IG::retainedMA,4,5},
    {Track::GA,&track_session_assets50IG::retainedGA,8,1},
    {Track::RA,&track_session_assets50IG::retainedRA,7,2},
    {Track::MB,&track_session_assets50IG::retainedMB,5,5},
    {Track::SC,&track_session_assets50IG::retainedSC,12,4},
    {Track::GC,&track_session_assets50IG::retainedGC,10,1},
    {Track::TC,&track_session_assets50IG::retainedTC,2,0},
    {Track::RC,&track_session_assets50IG::retainedRC,3,2},
    {Track::TB,&track_session_assets50IG::retainedTB,1,0},
    {Track::GB,&track_session_assets50IG::retainedGB,9,1},
    {Track::MC,&track_session_assets50IG::retainedMC,6,5}}};
// PORT EXTENSION, NOT ORIGINAL (A12, 146r): the ONE registry target. A free-play
// launch with an add-on course selected (port-owned, dlc_course_entries50dl.h;
// the profile's packed course/planet keep the last STOCK selection and are still
// checked by the original encodings) races that course; its session row is built
// from the registry at consumption (track_session_assets50IG::registrySession).
// It differs from a stock row only in where the files come from (source=registry).
inline const Target courseRouteTarget{Track::Course,nullptr,0xffffffffu,0xffffffffu};
// Code of a target for log lines: the stock row's code, or "course" for the
// registry target (its code is latched only once the launch is consumed).
inline const char* targetCode(const Target& t) noexcept {return t.assets?t.assets->world->code:"course";}
inline const Target* target(const Selection& s) noexcept {
    if(!rr::game::originalCourseSelectionEncoding50KG(s)||
       !rr::game::nativeSingleHumanCourseMode50KG(s))return nullptr;
    if(rr::game::dlc50DL::addonCourseForLaunch(s.mode34,s.subtype))return &courseRouteTarget;
    for(const auto& t:targets)if(t.course==s.course&&t.planet==s.planet)return &t;
    return nullptr;
}
inline bool same(const Selection& a,const Selection& b) noexcept {
    return a.course==b.course&&a.planet==b.planet&&a.rider==b.rider&&a.subtype==b.subtype&&
        a.laps==b.laps&&a.entrants==b.entrants&&a.packed1125C==b.packed1125C&&
        a.mode34==b.mode34;
}
// The launched selection as the racer manager 1AD520 leaves it (1AD5A4..5DC):
// a Time Trial (subtype 3) clears the opponent bits 1125C 7..10 and races a
// solo field. The port runs 1AD520 at session cold entry, before the counted
// race constructor (the original runs 1A65C0 first, 1AD520 inside its load),
// so the consumed launch is compared with either form. One rule, every course.
inline bool sameLaunch1AD520(const Selection& launched,const Selection& current) noexcept {
    if(same(launched,current))return true;
    if(launched.subtype!=3u)return false;
    auto field=launched;field.packed1125C=static_cast<decltype(field.packed1125C)>(field.packed1125C&~0x780u);field.entrants=1u;
    return same(field,current);
}
// Cold request admission only. Retains each actual authenticated target archive
// before Course input starts. Native mesh/texture/AI/camera construction is NOT
// reported complete here; the unchanged target cold loader must perform it.
// A missing target disables only that destination, with no fallback identity.
class Catalog final {
    std::array<std::vector<std::uint8_t>,targets.size()> originals_;
    inline static constexpr auto riders=[](){
        std::array<std::uint32_t,original_player_identity50IK::originals.size()> ids{};
        for(std::size_t i=0;i<ids.size();++i)ids[i]=original_player_identity50IK::originals[i].racer60;
        return ids;
    }();
    std::array<native_racer50HW::J,riders.size()> authenticatedRosters_;
    // Disc-only admission retains each verified encoded original once. The
    // real post-handoff Rig/model loaders still create and validate their own
    // actual resource owners. This is source availability, never race readiness.
    std::array<std::vector<std::uint8_t>,original_player_identity50IK::originals.size()> originalPods_;
    bool attempted_=false,directDisc_=false;
    void prepareDirectDiscRosters() {
        namespace identities=original_player_identity50IK;
        namespace detail=native_racer50HW::rig_detail;
        const auto disc=runtime_asset_paths50LB::resolve("work/native/project/disc");
        // Common pod admission: each slot's identity is the archive actually on
        // the disc (original_player_identity50IK::find, stock row as receipt).
        for(std::size_t i=0;i<identities::originals.size();++i){const auto& identity=*identities::find(identities::originals[i].racer60);
            try {
                auto raw=runtime_startup50fl::readBytes(identities::archivePath(disc,identity));
                detail::need(raw.size()==identity.archiveBytes&&
                    pod_materials50ed_fixture::detail::sha256(raw)==identity.archiveSha256,
                    "Original disc roster archive identity differs");
                rr::assets::ResArchive48C archive;rr::assets::DecodeError48C diagnostic;
                detail::need(rr::assets::decodeResource48C(raw,archive,diagnostic),
                    "Original disc roster archive decode rejected: "+diagnostic.code);
                const auto lowerPod=detail::lower(identity.podId);
                const auto wanted="pods/"+lowerPod+"/"+lowerPod+".phy";
                const rr::assets::ResourceEntry48C* member=nullptr;
                for(const auto& entry:archive.entries)if(detail::lower(entry.name)==wanted){
                    detail::need(!member,"Original disc roster PHY is ambiguous");member=&entry;}
                detail::need(member,"Original disc roster PHY is absent");
                std::vector<std::uint8_t> phy;
                detail::need(rr::assets::extractResMember48C(archive,member->name,phy,diagnostic)&&
                    pod_materials50ed_fixture::detail::sha256(phy)==identity.phySha256,
                    "Original disc roster PHY identity differs");
                originalPods_[i]=std::move(raw);
            }catch(const std::exception& error){
                std::cerr<<"Course direct original pod unavailable: "<<identity.podId<<" "<<error.what()<<'\n';
            }
        }
        for(std::size_t i=0;i<riders.size();++i){
            auto roster=original_course_roster50KG::directDiscRoster(riders[i]);bool complete=true;
            for(const auto& row:roster.at("racers")){
                bool verified=false;
                for(std::size_t pod=0;pod<identities::originals.size();++pod){const auto& identity=*identities::find(identities::originals[pod].racer60);
                    if(row.at("pod_id")==identity.podId&&row.at("resource_sha256")==identity.archiveSha256&&
                       row.at("phy_sha256")==identity.phySha256&&originalPods_[pod].size()==identity.archiveBytes)
                        verified=true;
                }
                complete=complete&&verified;
            }
            if(complete)authenticatedRosters_[i]=std::move(roster);
            else std::cerr<<"Course direct original field unavailable for rider: "<<riders[i]<<'\n';
        }
    }
    // A supplied catalog must describe the exact original field. Validate its
    // admission-bearing structure/identities before the per-rider asset checks:
    // malformed metadata is fatal, while an absent optional pod stays unavailable.
    // This metadata check creates no authenticated resource or race readiness.
    static void validateRosterMetadata(const native_racer50HW::J& roster) {
        namespace selection=rr::game::original_racer_selection50IJ;
        namespace identities=original_player_identity50IK;
        using native_racer50HW::rig_detail::need;
        need(roster.is_object()&&roster.at("schema")=="RR_ORIGINAL_ROSTER_V1"&&
            roster.at("player_index").is_number_integer()&&roster.at("player_index")==0&&
            roster.at("racers").is_array()&&roster.at("racers").size()==selection::entries.size(),
            "Course roster metadata requires the unchanged eight-racer original field");
        const auto validateRow=[](const native_racer50HW::J& row,const selection::Entry& entry){
            const auto* identity=identities::find(entry.racer60);
            need(identity&&row.is_object()&&row.at("pod_id")==std::string(entry.podId)&&
                row.at("path")==std::string(entry.retainedPodJson)&&
                row.at("resource_sha256")==identity->archiveSha256&&
                row.at("phy_sha256")==identity->phySha256,
                "Course roster metadata original resource/handling identity differs");
        };
        for(std::size_t i=0;i<selection::entries.size();++i)
            validateRow(roster.at("racers").at(i),selection::entries[i]);
        if(!roster.contains("additional_racers"))return;
        const auto& additional=roster.at("additional_racers");
        need(additional.is_array()&&additional.size()<=5,
            "Course roster supplemental metadata extent differs");
        std::array<std::uint32_t,5> seen{};std::size_t count=0;
        for(const auto& row:additional){
            need(row.is_object(),"Course roster supplemental metadata row differs");
            const auto podId=row.at("pod_id").get<std::string>();
            const auto* entry=selection::findByPodId(podId);
            need(entry&&original_course_roster50KG::supportsPreparedRider(entry->racer60),
                "Course roster supplemental original identity is unsupported");
            for(const auto& original:selection::entries)
                need(entry->racer60!=original.racer60,"Course roster repeats an original field racer as supplemental");
            for(std::size_t i=0;i<count;++i)
                need(seen[i]!=entry->racer60,"Course roster supplemental original is duplicated");
            validateRow(row,*entry);seen[count++]=entry->racer60;
        }
    }
public:
    void prepare(const rr::platform::HostTrackAdmission50IM& admission) {
        runtime_startup50fl::require(!attempted_,"Course target catalog is cold one-shot");attempted_=true;
        const auto rosterRoot=runtime_asset_paths50LB::resolve("evidence/originalRoster50HW/raw-imports");
        const auto rosterPath=rosterRoot/"roster.json";
        if(!runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty()||
           !std::filesystem::exists(rosterPath)){
            // Explicit original-disc mode is independent of any leftover
            // generated catalog. Other tracks keep their prepared-data gate.
            directDisc_=true;prepareDirectDiscRosters();
        }else{
            // A supplied legacy roster must still validate. A malformed catalog
            // cannot silently fall through to another readiness mode.
            std::ifstream input(rosterPath);
            native_racer50HW::rig_detail::need(bool(input),"Cannot open Course roster metadata: "+rosterPath.string());
            // Strict parse rejects a JSON prefix followed by trailing content.
            const auto originals=native_racer50HW::J::parse(input);
            validateRosterMetadata(originals);
            for(std::size_t i=0;i<riders.size();++i){
                auto roster=originals;std::string error;
                if(original_course_roster50KG::selectPreparedRider(".",rosterRoot,roster,riders[i],error))
                    authenticatedRosters_[i]=std::move(roster);
                else std::cerr<<"Course cold rider unavailable: "<<riders[i]<<" "<<error<<'\n';
            }
        }
        // One rule for every course (B-C02): the rooted Course race reads the
        // original disc (RR_ORIGINAL_ASSET_ROOT) in mode2; the archive must resolve.
        const bool originalDisc=!runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
        for(std::size_t i=0;i<targets.size();++i){const auto& t=targets[i];
            if(!originalDisc||!admission.gcMode2||!rr::platform::hostTrackAdmitted50IM(t.track,admission))continue;
            try {
                auto bytes=runtime_startup50fl::readBytes(t.assets->archive);
                // Common rule for every course (course_archive_receipt50ci.h):
                // the original reads whatever archive the disc holds. Availability
                // needs a readable RES within the shared decode capacity; the stock
                // identity is a receipt only.
                // The stock archive is known readable; any other is decoded once
                // here so an unreadable file disables only this destination.
                if(!course_archive_receipt50CI::receipt("course_select",t.assets->world->code,bytes,
                    t.assets->archiveSha256,t.assets->archiveBytes)){
                    rr::assets::ResArchive48C archive;rr::assets::DecodeError48C diagnostic;
                    if(bytes.empty()||bytes.size()>course_archive_receipt50CI::maximumArchiveBytes()||
                       !rr::assets::decodeResource48C(bytes,archive,diagnostic,nullptr,course_archive_receipt50CI::limits())){
                        std::cerr<<"Course cold target unavailable: "<<t.assets->world->code<<" archive is not a readable RES ("<<diagnostic.code<<")\n";
                        continue;
                    }
                    // Cut-camera row from this archive's own data (common rule).
                    std::string cutError;
                    if(!course_cut_identity50CI::derive(t.assets->world->code,archive,bytes,cutError))
                        std::cerr<<"course_cut_identity code="<<t.assets->world->code<<" derived=0 ("<<cutError<<")\n";
                }
                originals_[i]=std::move(bytes);
            }catch(const std::exception& e){
                std::cerr<<"Course cold target unavailable: "<<t.assets->world->code<<" "<<e.what()<<'\n';
            }
        }
    }
    rr::game::OriginalCourseLoadAvailability50KG availability(const Selection& s)const noexcept {
        using A=rr::game::OriginalCourseLoadAvailability50KG;
        if(!rr::game::originalCourseSelectionEncoding50KG(s))return A::invalidSelection;
        if(!rr::game::nativeSingleHumanCourseMode50KG(s))return A::unsupportedNativeMode;
        if(!attempted_)return A::hostNotReady;
        const Target* selected=nullptr;
        // Registry target (A12): the registry admitted the package file at startup
        // (listed size/SHA); the route availability is the only other condition.
        const bool addonCourse=rr::game::dlc50DL::addonCourseForLaunch(s.mode34,s.subtype)!=0;
        if(runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty())return A::courseResourcesUnavailable;
        if(addonCourse){
            if(!rr::game::dlc50DL::addonCourseRoute().available)return A::courseResourcesUnavailable;
            selected=&courseRouteTarget;
        }
        else for(const auto& t:targets)if(t.course==s.course&&t.planet==s.planet){selected=&t;break;}
        if(!selected)return A::courseResourcesUnavailable;
        bool rosterPresent=false;
        // PORT EXTENSION, NOT ORIGINAL: a selected free-play add-on rider races
        // instead of the profile's stock rider; its row is the DLC registry's
        // and its archive identity the common pod admission's (read once).
        const unsigned addon=s.mode34==0?rr::game::dlc50DL::selectedAddonRider():0u;
        if(addon)rosterPresent=original_player_identity50IK::find(addon)!=nullptr;
        else for(std::size_t i=0;i<riders.size();++i)
            if(s.rider==riders[i]&&!authenticatedRosters_[i].is_null())rosterPresent=true;
        if(!rosterPresent)return A::riderResourcesUnavailable;
        if(addonCourse)return A::ready;
        for(std::size_t i=0;i<targets.size();++i)
            if(selected==&targets[i]&&!originals_[i].empty())return A::ready;
        return A::courseResourcesUnavailable;
    }
    const Target* admits(const Selection& s)const noexcept {
        return availability(s)==rr::game::OriginalCourseLoadAvailability50KG::ready?target(s):nullptr;
    }
};
// Native application message, not a completed destination or source pointer.
// Publish only after takeCourseLaunch: its original queue9 and renderer drain
// have really completed. The outer owner consumes after old-world destruction,
// rechecking the SAME actual profile before selecting the next cold loader.
// Native retained result of a real Course request consumption. This carries
// no Entity, parent, scheduler or constructor completion. Only ColdLoadRequest
// can mint it, after validating the actual outgoing request and profile.
class ConsumedLaunch final {
    friend class ColdLoadRequest;
    friend struct ::original_main_entry50KG::Application;
    bool raceConstructorClaimed_=false;
    std::shared_ptr<const rr::platform::OriginalUiRiderProfileOwner50KG> profile_;
    Selection selection_{};
    const Target* target_=nullptr;
    std::uint64_t nextGeneration_=0;
    // The launched course code as data (upper case + NUL): the stock row's code or
    // the registry code latched at consumption. Written once, by ColdLoadRequest.
    std::array<char,3> code_{};
    ConsumedLaunch()=default;
    static char upper(char c) noexcept {return c>='a'&&c<='z'?static_cast<char>(c-'a'+'A'):c;}
    // Bounds-checked: exactly two [A-Za-z0-9] characters, else refused (code_ stays clear).
    bool latchCode(const char* code) noexcept {
        code_={};
        if(!code||!code[0]||!code[1]||code[2])return false;
        for(unsigned i=0;i<2u;++i){
            const char c=upper(code[i]);
            if(!((c>='A'&&c<='Z')||(c>='0'&&c<='9'))){code_={};return false;}
            code_[i]=c;
        }
        return true;
    }
public:
    ConsumedLaunch(const ConsumedLaunch&)=delete;
    ConsumedLaunch& operator=(const ConsumedLaunch&)=delete;
    // W0 freeze (I1): the course code of this launch, as data. Empty only for a
    // launch that was never consumed (cannot be observed: only ColdLoadRequest mints).
    std::string_view courseCode()const noexcept {return {code_.data(),code_[0]&&code_[1]?2u:0u};}
    // W0 freeze (I1): true when the course's files come from the DLC registry (A12).
    bool registryCourse()const noexcept {return target_==&courseRouteTarget;}
    // W0 freeze (I1): the one launch match for every course, by course code
    // (case-insensitive two-character compare), stock or registry row. Replaces
    // matches(..., HostTrack50IM, ...). `racer` may be the latched free-play add-on
    // (PORT EXTENSION): the profile's own stock rider is compared, never the add-on number.
    bool matchesCourse(const rr::platform::OriginalUiRiderProfileOwner50KG& actual,
        std::uint64_t session,std::string_view courseCode,std::uint32_t racer)const noexcept {
        Selection current;
        const auto own=this->courseCode();
        return profile_.get()==&actual&&target_&&session==nextGeneration_&&
            own.size()==2u&&courseCode.size()==2u&&upper(courseCode[0])==own[0]&&upper(courseCode[1])==own[1]&&
            selection_.rider==rr::game::dlc50DL::profileRider(racer)&&
            actual.courseRaceSnapshot(current)&&sameLaunch1AD520(selection_,current)&&target(current)==target_;
    }
    // RETIRING: the host-selection form, kept only so the existing caller
    // (original_race_constructor50la.h, R1) compiles until it calls matchesCourse.
    // It adds no rule of its own: the same launch match by the launch's course code.
    bool matches(const rr::platform::OriginalUiRiderProfileOwner50KG& actual,
        std::uint64_t generation,Track loadedTrack,std::uint32_t loadedRacer)const noexcept {
        return target_&&target_->track==loadedTrack&&matchesCourse(actual,generation,courseCode(),loadedRacer);
    }
    // (B-C07: renewForRestart is deleted. Every restart is in place, 1AC8E0
    // through common_race_start50CR; a launch is never renewed for a reload.)
};
class ColdLoadRequest final {
    Selection selection_{};
    const Target* target_=nullptr;
    std::uint64_t outgoingGeneration_=0;
public:
    bool pending()const noexcept{return target_!=nullptr;}
    bool publish(const Catalog& catalog,const Selection& selection,std::uint64_t generation)noexcept {
        if(pending()||!generation)return false;
        const auto* selected=catalog.admits(selection);if(!selected)return false;
        selection_=selection;target_=selected;outgoingGeneration_=generation;return true;
    }
    bool consume(const rr::platform::OriginalUiRiderProfileOwner50KG& actual,
        std::uint64_t destroyedGeneration,Track& nextTrack,std::uint32_t& nextRacer)noexcept {
        Selection current;
        if(!pending()||destroyedGeneration!=outgoingGeneration_||!actual.courseRaceSnapshot(current)||
           !same(selection_,current)||target(current)!=target_)return false;
        // Registry target (A12): latch the add-on code the session row is built from.
        if(target_==&courseRouteTarget){
            rr::game::dlc50DL::AddonCourseLoad load;
            const unsigned number=rr::game::dlc50DL::addonCourseForLaunch(current.mode34,current.subtype);
            if(!number||!rr::game::dlc50DL::addonCourseLoad(number,load)||
               !rr::platform::latchCourseRouteCode50IM(load.codeUpper.c_str()))return false;
            std::cerr<<"course_route_launch key="<<load.key<<" code="<<load.code<<" number="<<number
                <<" source=registry mode34="<<current.mode34<<" subtype="<<current.subtype
                <<" profile_course="<<current.course<<" profile_planet="<<current.planet<<'\n';
        }
        nextTrack=target_->track;nextRacer=current.rider;
        target_=nullptr;outgoingGeneration_=0;return true;
    }
    // Compatibility: the existing consume above retains its exact behavior.
    // Allocate before consuming so native allocation failure cannot lose an
    // authenticated request. Publication below is infallible after consume.
    bool consumeForNextSession(
        std::shared_ptr<const rr::platform::OriginalUiRiderProfileOwner50KG> actual,
        std::uint64_t destroyedGeneration,Track& nextTrack,std::uint32_t& nextRacer,
        std::unique_ptr<ConsumedLaunch>& result)noexcept {
        if(!actual||result||destroyedGeneration==std::numeric_limits<std::uint64_t>::max())return false;
        std::unique_ptr<ConsumedLaunch> retained;
        try {retained.reset(new ConsumedLaunch);}catch(...){return false;}
        const auto selected=selection_;const auto* destination=target_;
        if(!consume(*actual,destroyedGeneration,nextTrack,nextRacer))return false;
        // The course code as data: the stock row's code, or the registry code
        // consume() just latched for the registry target.
        if(!retained->latchCode(destination==&courseRouteTarget?rr::platform::courseRouteCode50IM():
               destination->assets->world->code))return false;
        retained->profile_=std::move(actual);retained->selection_=selected;
        retained->target_=destination;retained->nextGeneration_=destroyedGeneration+1;
        result=std::move(retained);return true;
    }
};
}
