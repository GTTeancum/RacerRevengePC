#pragma once
// ============================================================================
// PC ENHANCEMENT, NOT ORIGINAL BEHAVIOUR: N64-style surfaces from track data.
//
// Original Revenge loads TerrainZone1..8 from the course .tob (276880, row
// constructor 276700: +0 SurfaceFriction, +4 SurfaceDrag, +8 SurfaceTraction,
// +C GenerateDust, +10 GenerateDirt) and, when a TerrainZone section exists,
// TerrainZoneCollision / ZoneCollisionObjectN (272520): each collision object
// whose SLT name (cut at the first '.', 50 chars) matches OBJ (cut at the last
// '.', case-insensitive, 308F00) gets row TerrainZone-1 at object +0x180
// (27264C), handed to its body at +0xA8 (27B110). Unlisted objects keep row 0.
// The original reads only GenerateDust (217A90, over the terrain probe's
// collected-body list 348+1E8/+1EC). Friction/Traction are never read.
//
// This module reads the same data on EVERY course (no course names) and, when
// the switch is on, applies N64 Racer's surface rules to EVERY racer (player
// and AI, one path):
//  * grip  (N64 0x80066A80 / 0x80068D04): grip target = SurfaceTraction,
//    eased linearly at 0.5/s, tracks the last surface seen while airborne,
//    a finished racer's target is 1.0. Applied as a multiplier on the pod's
//    own sideslip side force (16CD90 +90/+10C/+118), see lateralScale().
//  * speed factor (N64 0x80068880): target = SurfaceFriction, eased linearly
//    at 0.5/s, applied only while the pod is ground-supported (N64: ground
//    distance < 15). Applied as a multiplier on the engine speed endpoints
//    (2E7720 +54/+58/+60/+64), see speedFactor().
//  * speed bonus / boost strip (N64 "Fast" 0x4, target +200, pod +0x240):
//    target = SurfaceSpeedBonus (NEW KEY, default 0; highest collected value
//    wins), eased linearly at 25/s, applied always (also airborne) as an
//    upward shift of the engine's speed window (2E7720 +58) plus a
//    zero-throttle floor, see speedBonus() and NOTES.md (zero-g) section 4.
//  * zero gravity (N64 ZOn 0x1 / ZOff 0x2 in the surface record's vehicle
//    reaction word): row value ZeroGravity (NEW KEY, 0 none, 1 on, 2 off,
//    3 both; default 0). This module only READS it; the flight latch is
//    native_zero_gravity50zg.h. While a racer is in zero-g flight the N64
//    forces grip = factor = 1 (0x8006B948..968) and reads the surface under
//    the racing line (the caller supplies that owner), see update().
//
// Both new keys follow the original's own key rule (276880 asks for every
// key by name with a default): absent key -> default, absent section ->
// defaults. The original reader ignores them. No stock course has either key
// (zero-g note section 6, stockkeys.py), so they stay at their defaults.
//
// Exactly inert at defaults: when a racer's eased values are exactly 1.0f /
// 0.0f the caller takes the original entry point, so stock courses (every
// stock value is the default) run bit-for-bit as before.
// Host-side state only; no game or guest memory is written. All row and
// identity lookups are bounds-checked; out-of-range data falls back to row 0.
// ============================================================================
#include "../assets/import_decode48c.h"
#include "../assets/import_world48f.h"
#include "../assets/track_start50ad.h"
#include "terrain_query48j.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace rr::game {
// ---- THE ONE DEFAULT. --------------------------------------------------------
// User decision 2026-11-14: N64 surface rules (ice, slow snow, boost strip)
// and zero gravity are canonical for converted N64 tracks, so ON by default.
// RR_SURFACE_TRACTION=0 switches the surface rules off, RR_ZERO_GRAVITY=0 the
// zero-g latch. Stock courses author defaults everywhere and stay bit-for-bit
// original either way. Zero gravity follows this same constant.
inline constexpr bool surfaceRulesDefaultEnabled50ST=true;
inline constexpr bool zeroGravityDefaultEnabled50ST=surfaceRulesDefaultEnabled50ST;

// ---- N64 constants (work\scratch\racer-n64\notes\n64-ice-physics.md) --------
inline constexpr float n64TractionBlend50ST=0.996666f; // 0x800AD528, section 3.2
// UNSETTLED: N64 per-pod traction stat (table 0x800A368C, 0.30..0.80). Revenge
// pods all share SideSlipSideForce -1000, so one reference stat is used.
inline constexpr float n64ReferenceStat50ST=0.5f;
inline constexpr float surfaceEaseRatePerSecond50ST=0.5f; // grip +0x248 and factor +0x244
inline constexpr float speedBonusEaseRatePerSecond50ST=25.f; // bonus +0x240 (0x80066E58..)
// N64 units are feet (converter scale 1) and Revenge's engine speeds are ft/s
// (authored mph x 1.46667, authored_parameters49n.cpp), so these carry over.
inline constexpr float zeroGravityMinimumSpeed50ST=75.f; // 0x4296: ZON bonus 75-speed, ZOFF floor
inline constexpr float speedBonusLimit50ST=1000.f;       // refuse larger authored values
// User decision 2026-11-14 ("N64's 1.7 s"): on ice (grip 0.2) the sideways
// slide halves in the N64's absolute time, 1.72 s (n64-ice-physics.md: f0 30,
// stat 0.5, T 0.98670, k 0.402/s), not the N64 ratio. Revenge's own sideways
// decay is 1000/(1000/32.2) = 32.2/s on every stock pod (SideSlipSideForce
// -1000, Weight 1000). FLAG: a pod with other values keeps the same multiplier,
// so its ice half-life scales with its own decay rate.
inline constexpr double n64IceGrip50ST=0.2;
inline constexpr double n64IceSlideHalfLifeSeconds50ST=1.72;
inline constexpr double revengeSideDecayPerSecond50ST=32.2;

// ---- Row layout: the ONE place it is defined. -------------------------------
// Rows 0..7 = TerrainZone1..8 (authored). Rows 8..11 = test-only forced rows.
inline constexpr std::size_t terrainZoneRows50ST=8;
inline constexpr std::size_t forcedTestRow50ST=8;        // RR_TEST_FORCE_TRACTION
inline constexpr std::size_t forcedZeroGOnRow50ST=9;     // RR_TEST_FORCE_ZERO_G (on)
inline constexpr std::size_t forcedZeroGOffRow50ST=10;   // RR_TEST_FORCE_ZERO_G (off)
inline constexpr std::size_t forcedSpeedBonusRow50ST=11; // RR_TEST_FORCE_SPEED_BONUS
inline constexpr std::size_t surfaceRowCount50ST=12;
inline constexpr std::size_t surfaceRacerSlots50ST=8;
inline constexpr std::size_t surfaceSltNameCap50ST=50; // 27A8CC
inline constexpr std::uint32_t noSurfaceOwner50ST=std::numeric_limits<std::uint32_t>::max();
// ZeroGravity row bits = N64 vehicle_reaction & 3.
inline constexpr std::uint8_t zeroGravityOnBit50ST=1,zeroGravityOffBit50ST=2;

struct SurfaceRow50ST {
    float friction=1.f, traction=1.f;
    float speedBonus=0.f;        // NEW KEY SurfaceSpeedBonus (N64 Fast -> 200)
    std::uint8_t zeroGravity=0;  // NEW KEY ZeroGravity (0..3)
};
struct SurfaceRacer50ST {
    float grip=1.f, gripTarget=1.f;     // N64 +0x248 and its target
    float factor=1.f, factorTarget=1.f; // N64 +0x244 and its target
    float bonus=0.f, bonusTarget=0.f;   // N64 +0x240 and its target
    std::uint32_t owner=noSurfaceOwner50ST; // last logged owner
    std::uint64_t steps=0, gripChangeStep=0, factorChangeStep=0, bonusChangeStep=0;
    bool gripSettled=true, factorSettled=true, bonusSettled=true;
};
// What the zero-g latch tells the surface rule for this completed step.
struct SurfaceZeroGravityStep50ST {
    bool flight=false;                         // N64 ZON during this step
    std::uint32_t lineOwner=noSurfaceOwner50ST; // static owner under the racing line
    float measuredSpeed=0.f;                   // ft/s, the step's cached body speed
};

class SurfaceRules50ST {
public:
    SurfaceRules50ST() {
        tractionEnabled_=surfaceRulesDefaultEnabled50ST;
        zeroGravityEnabled_=zeroGravityDefaultEnabled50ST;
        if(const char* v=std::getenv("RR_SURFACE_TRACTION")){
            if(!std::strcmp(v,"0"))tractionEnabled_=false;else if(!std::strcmp(v,"1"))tractionEnabled_=true;
        }
        if(const char* v=std::getenv("RR_ZERO_GRAVITY")){
            if(!std::strcmp(v,"0"))zeroGravityEnabled_=false;else if(!std::strcmp(v,"1"))zeroGravityEnabled_=true;
        }
        // TEST ONLY: RR_TEST_FORCE_TRACTION="<traction>[,<friction>][/<every>]".
        // Every <every>-th static collision owner (default 1 = all) uses row 8
        // with these values, so a stock course can show enter/leave and easing
        // in muted logs-only runs. Implies the enhancement is on.
        if(const char* v=std::getenv("RR_TEST_FORCE_TRACTION")){
            const std::string t(v);const auto slash=t.find('/');
            const std::string values=t.substr(0,slash);const auto comma=values.find(',');
            const float traction=static_cast<float>(std::atof(values.substr(0,comma).c_str()));
            const float friction=comma==std::string::npos?1.f:
                static_cast<float>(std::atof(values.substr(comma+1).c_str()));
            const long every=slash==std::string::npos?1:std::atol(t.substr(slash+1).c_str());
            if(validValue(traction)&&validValue(friction)&&every>=1&&every<=100000){
                forced_=true;tractionEnabled_=true;forcedEvery_=static_cast<std::size_t>(every);
                forcedRow_.friction=friction;forcedRow_.traction=traction;
            }else std::cerr<<"surface_rules test_force rejected value="<<t<<'\n';
        }
        // TEST ONLY: RR_TEST_FORCE_ZERO_G="<every>[/<offset>]". Static owner k
        // with k%every==0 gets row 9 (ZeroGravity 1), k%every==offset (default
        // every/2) row 10 (ZeroGravity 2), so a stock course gets entry and
        // exit strips along its own line for muted logs-only runs. Implies
        // zero gravity on. Takes priority over the other forced rows.
        if(const char* v=std::getenv("RR_TEST_FORCE_ZERO_G")){
            const std::string t(v);const auto slash=t.find('/');
            const long every=std::atol(t.substr(0,slash).c_str());
            const long offset=slash==std::string::npos?every/2:std::atol(t.substr(slash+1).c_str());
            if(every>=2&&every<=100000&&offset>=1&&offset<every){
                forcedZeroG_=true;zeroGravityEnabled_=true;
                forcedZeroGEvery_=static_cast<std::size_t>(every);forcedZeroGOffset_=static_cast<std::size_t>(offset);
            }else std::cerr<<"surface_rules test_force_zero_g rejected value="<<t<<'\n';
        }
        // TEST ONLY: RR_TEST_FORCE_SPEED_BONUS="<bonus>[/<every>]": every
        // <every>-th static owner (default 1) gets row 11 with that bonus (and
        // a forced zero-g "on" row also carries it, like N64 ZOn+Fast).
        // Implies the surface rules on.
        if(const char* v=std::getenv("RR_TEST_FORCE_SPEED_BONUS")){
            const std::string t(v);const auto slash=t.find('/');
            const float bonus=static_cast<float>(std::atof(t.substr(0,slash).c_str()));
            const long every=slash==std::string::npos?1:std::atol(t.substr(slash+1).c_str());
            if(validBonus(bonus)&&bonus>0.f&&every>=1&&every<=100000){
                forcedBonus_=true;tractionEnabled_=true;forcedBonusEvery_=static_cast<std::size_t>(every);
                forcedBonusValue_=bonus;
            }else std::cerr<<"surface_rules test_force_speed_bonus rejected value="<<t<<'\n';
        }
    }
    // Reader needed at all (either feature on).
    bool enabled()const noexcept {return tractionEnabled_||zeroGravityEnabled_;}
    bool tractionEnabled()const noexcept {return tractionEnabled_;}
    bool zeroGravityEnabled()const noexcept {return zeroGravityEnabled_;}
    bool loaded()const noexcept {return loaded_;}

    // Cold, once per course load, on every course. archiveBytes = the course
    // RES; worldMember = its SCB member ("tracks\xx.scb"); the TOB is the same
    // member with extension .tob (data-derived, no course name). owners = the
    // course's static collision owners in candidate order (each has .zone and
    // .ordinal); candidate identity = firstIdentity + owner index
    // (track_static_candidates50IG::prepareSpan).
    // Stock TOBs (TB, TC) carry duplicate labels: retain them; the keys read
    // here are looked up per block.
    static assets::WorldLimits48F tobLimits(std::size_t bytes)noexcept{
        assets::WorldLimits48F l;l.retainDuplicateLabels=true;if(bytes>l.input_bytes)l.input_bytes=bytes;return l;}
    template<class Owners>
    void loadCourse(const std::vector<std::uint8_t>& archiveBytes,const char* worldMember,
        const assets::World48F& world,const Owners& owners,std::uint32_t firstIdentity)noexcept {
        resetCourse();
        if(!enabled())return;
        try {
            rows_.fill({});ownerRow_.assign(owners.size(),0);ownerName_.assign(owners.size(),{});
            firstIdentity_=firstIdentity;
            // Owner SLT names, as 27A8C4->309348 builds descriptor +0.
            for(std::size_t k=0;k<owners.size();++k){
                const assets::WorldInstance48F* found=nullptr;bool unique=true;
                for(const auto& in:world.instances)
                    if(in.zone==owners[k].zone&&in.ordinal==owners[k].ordinal){if(found)unique=false;found=&in;}
                if(!found||!unique||found->parameter_block>=world.parameters.blocks.size())continue;
                const auto* slt=field(world.parameters.blocks[found->parameter_block],"slt");
                if(!slt)continue;
                const auto dot=slt->find('.');
                std::string name=slt->substr(0,dot==std::string::npos?slt->size():dot);
                if(name.size()>surfaceSltNameCap50ST)name.resize(surfaceSltNameCap50ST);
                ownerName_[k]=lower(name);
            }
            std::size_t sections=0,mapped=0;
            const std::string tobMember=tobName(worldMember);
            assets::ResArchive48C archive;assets::DecodeError48C error;std::vector<std::uint8_t> tob;
            assets::ParameterDocument48F doc;
            const bool haveTob=!tobMember.empty()&&assets::decodeResource48C(archiveBytes,archive,error)&&
                hasMember(archive,tobMember)&&assets::extractResMember48C(archive,tobMember,tob,error)&&
                assets::decodeParameters48F(tob,doc,error,nullptr,tobLimits(tob.size()));
            if(haveTob){
                for(std::size_t i=0;i<terrainZoneRows50ST;++i){
                    const auto* b=block(doc,"terrainzone"+std::to_string(i+1));
                    if(!b)continue; // 276880: missing sections keep defaults
                    ++sections;
                    // 158B80-style: every key by name, absent -> default.
                    rows_[i].friction=number(*b,"surfacefriction",i);
                    rows_[i].traction=number(*b,"surfacetraction",i);
                    rows_[i].speedBonus=bonusNumber(*b,"surfacespeedbonus",i);
                    rows_[i].zeroGravity=zeroGravityValue(*b,"zerogravity",i);
                }
                // 272520 runs only when some TerrainZone section exists.
                const auto* zc=sections?block(doc,"terrainzonecollision"):nullptr;
                const auto* countText=zc?field(*zc,"numcollisionobjects"):nullptr;
                const long count=countText?std::strtol(countText->c_str(),nullptr,0):0;
                for(long n=1;count>0&&n<=count&&n<=4096;++n){
                    const auto* z=block(doc,"zonecollisionobject"+std::to_string(n));
                    const auto* obj=z?field(*z,"obj"):nullptr;
                    const auto* tz=z?field(*z,"terrainzone"):nullptr;
                    if(!obj||!tz)continue;
                    const long zone=std::strtol(tz->c_str(),nullptr,0);
                    // Original has no clamp (reads outside its 8 rows): refuse here.
                    if(zone<1||zone>static_cast<long>(terrainZoneRows50ST)){
                        std::cerr<<"surface_rules zone_object_refused index="<<n<<" terrainzone="<<*tz<<'\n';continue;}
                    const auto dot=obj->rfind('.');
                    const std::string name=lower(obj->substr(0,dot==std::string::npos?obj->size():dot));
                    for(std::size_t k=0;k<ownerName_.size();++k)
                        if(!ownerName_[k].empty()&&ownerName_[k]==name){
                            ownerRow_[k]=static_cast<std::uint8_t>(zone-1);++mapped;}
                }
            }
            if(forced_){
                rows_[forcedTestRow50ST]=forcedRow_;
                for(std::size_t k=0;k<ownerRow_.size();++k)if(k%forcedEvery_==0)ownerRow_[k]=forcedTestRow50ST;
            }
            if(forcedBonus_){
                rows_[forcedSpeedBonusRow50ST].speedBonus=forcedBonusValue_;
                for(std::size_t k=0;k<ownerRow_.size();++k)if(k%forcedBonusEvery_==0)ownerRow_[k]=forcedSpeedBonusRow50ST;
            }
            if(forcedZeroG_){
                rows_[forcedZeroGOnRow50ST].zeroGravity=zeroGravityOnBit50ST;
                if(forcedBonus_)rows_[forcedZeroGOnRow50ST].speedBonus=forcedBonusValue_;
                rows_[forcedZeroGOffRow50ST].zeroGravity=zeroGravityOffBit50ST;
                for(std::size_t k=0;k<ownerRow_.size();++k){
                    if(k%forcedZeroGEvery_==0)ownerRow_[k]=forcedZeroGOnRow50ST;
                    else if(k%forcedZeroGEvery_==forcedZeroGOffset_)ownerRow_[k]=forcedZeroGOffRow50ST;
                }
            }
            std::size_t low=0;for(const auto& r:rows_)if(nonDefault(r))++low;
            for(std::size_t k=0;k<ownerRow_.size();++k){
                const auto row=ownerRow_[k];if(row>=surfaceRowCount50ST)continue;
                if(rows_[row].zeroGravity&zeroGravityOnBit50ST)++zeroGravityOnOwners_;
                if(rows_[row].zeroGravity&zeroGravityOffBit50ST)++zeroGravityOffOwners_;
            }
            loaded_=true;
            std::cerr<<"surface_rules_load tob="<<(haveTob?tobMember:std::string("absent"))
                <<" zone_sections="<<sections<<" owners="<<owners.size()<<" mapped="<<mapped
                <<" non_default_rows="<<low<<" zero_g_on_owners="<<zeroGravityOnOwners_
                <<" zero_g_off_owners="<<zeroGravityOffOwners_
                <<" forced="<<(forced_?1:0)<<(forcedBonus_?1:0)<<(forcedZeroG_?1:0)<<'\n';
            for(std::size_t i=0;i<surfaceRowCount50ST;++i)
                if(nonDefault(rows_[i]))
                    std::cerr<<"surface_rules_row row="<<i<<" traction="<<rows_[i].traction
                        <<" friction="<<rows_[i].friction<<" speed_bonus="<<rows_[i].speedBonus
                        <<" zero_gravity="<<unsigned(rows_[i].zeroGravity)<<'\n';
        }catch(...){resetCourse();std::cerr<<"surface_rules_load failed; course stays original\n";}
    }

    // Grip multiplier for this racer's next force step. Exactly 1.0f unless the
    // eased grip is below 1. Mapping (ice NOTES.md section 3): ratio of the N64
    // per-frame blend rates, ln(T(stat*grip))/ln(T(stat)), T(g)=c(1-g^2).
    float lateralScale(std::size_t slot)const noexcept {
        if(!tractionEnabled_||!loaded_||slot>=surfaceRacerSlots50ST)return 1.f;
        const float g=racers_[slot].grip;
        if(g==1.f)return 1.f;
        const double s=n64ReferenceStat50ST,c=n64TractionBlend50ST;
        const double num=std::log(c*(1.0-(s*g)*(s*g))),den=std::log(c*(1.0-s*s));
        const double ratio=num/den;
        // Same curve shape as the N64 blend ratio, raised to the power that
        // puts grip 0.2 exactly at the absolute 1.72 s half-life. ratio(1)=1,
        // so full grip stays exactly 1 (and is skipped above). Continuous-time
        // rate: frame-rate independent.
        static const double exponent=[](){
            const double s0=n64ReferenceStat50ST,c0=n64TractionBlend50ST,g0=n64IceGrip50ST;
            const double iceRatio=std::log(c0*(1.0-(s0*g0)*(s0*g0)))/std::log(c0*(1.0-s0*s0));
            const double target=(std::log(2.0)/n64IceSlideHalfLifeSeconds50ST)/revengeSideDecayPerSecond50ST;
            return std::log(target)/std::log(iceRatio);
        }();
        const double m=std::pow(ratio,exponent);
        if(!std::isfinite(m))return 1.f;
        return static_cast<float>(std::clamp(m,1e-4,1.0));
    }
    // Speed multiplier for this racer's next force step. N64 applies the factor
    // only near the ground; Revenge's own "near ground" is terrain probe
    // 348+104 (the same bit that selects ground vs airborne gravity at 219810).
    float speedFactor(std::size_t slot,bool groundSupported)const noexcept {
        if(!tractionEnabled_||!loaded_||slot>=surfaceRacerSlots50ST||!groundSupported)return 1.f;
        const float f=racers_[slot].factor;
        return f==1.f?1.f:std::clamp(f,1e-3f,1.f);
    }
    // Eased N64 speed bonus in ft/s (N64 adds it always, also airborne).
    // Exactly 0 unless a boost strip or the zero-g minimum is active.
    float speedBonus(std::size_t slot)const noexcept {
        if(!enabled()||!loaded_||slot>=surfaceRacerSlots50ST)return 0.f;
        const float b=racers_[slot].bonus;
        return b==0.f?0.f:std::clamp(b,0.f,speedBonusLimit50ST);
    }

    // Zero-g reader queries (native_zero_gravity50zg.h). Bounds-checked.
    std::size_t ownerCount()const noexcept {return ownerRow_.size();}
    std::uint8_t ownerZeroGravity(std::uint32_t owner)const noexcept {
        const auto* r=ownerRowValues(owner);return r?r->zeroGravity:0;}
    const std::string& ownerName(std::uint32_t owner)const noexcept {
        static const std::string none("none");return owner<ownerName_.size()?ownerName_[owner]:none;}
    bool courseHasZeroGravity()const noexcept {return loaded_&&zeroGravityOnOwners_>0;}
    std::uint32_t firstIdentity()const noexcept {return firstIdentity_;}
    // Static owner index of a collected identity, or noSurfaceOwner50ST.
    std::uint32_t ownerOfIdentity(std::uint32_t id)const noexcept {
        if(id<firstIdentity_)return noSurfaceOwner50ST;
        const std::uint64_t k=static_cast<std::uint64_t>(id)-firstIdentity_;
        return k<ownerRow_.size()?static_cast<std::uint32_t>(k):noSurfaceOwner50ST;
    }
    // OR of the ZeroGravity bits of every collected static owner (the probe's
    // own list, as 217A90 lets any listed body decide). first = an owner that
    // carries the bit, for logs.
    std::uint8_t collectedZeroGravity(const PreparedTerrainState48J& terrain,std::uint32_t& first)const noexcept {
        first=noSurfaceOwner50ST;std::uint8_t bits=0;
        const auto count=std::min<std::uint32_t>(terrain.collectedCount,
            static_cast<std::uint32_t>(terrain.collectedIdentities.size()));
        for(std::uint32_t i=0;i<count;++i){
            const auto k=ownerOfIdentity(terrain.collectedIdentities[i]);
            const auto z=ownerZeroGravity(k);
            if(z&&first==noSurfaceOwner50ST)first=k;bits=static_cast<std::uint8_t>(bits|z);
        }
        return bits;
    }

    // After a COMPLETED racer step only (retry-safe; N64 also updates its
    // targets after the motion step, second per-frame pass). terrain = this
    // step's committed probe result (348 +1E8 count, +1EC identities).
    // zeroG = the latch state during this step (FLIGHT reads the floor under
    // the racing line instead of the probe list; N64 0x8006B948..968).
    void update(std::size_t slot,const PreparedTerrainState48J& terrain,bool finished,float dt,
        const SurfaceZeroGravityStep50ST& zeroG={})noexcept {
        if(!enabled()||!loaded_||slot>=surfaceRacerSlots50ST)return;
        if(!std::isfinite(dt)||!(dt>0.f)||dt>0.25f)return;
        auto& r=racers_[slot];++r.steps;
        float traction=r.gripTarget,friction=r.factorTarget,bonus=r.bonusTarget;std::uint32_t owner=r.owner;
        const bool flight=zeroGravityEnabled_&&zeroG.flight;
        if(flight){
            // N64 ZON: grip and factor are SET to 1 (not eased), and the
            // bonus target comes from the surface under the line: Fast -> its
            // bonus, otherwise 75-speed below 75 (0x80066A88..AC8).
            traction=1.f;friction=1.f;owner=zeroG.lineOwner;
            const auto* row=ownerRowValues(zeroG.lineOwner);
            const float strip=tractionEnabled_&&row?row->speedBonus:0.f;
            const float speed=std::isfinite(zeroG.measuredSpeed)?zeroG.measuredSpeed:0.f;
            bonus=strip>0.f?strip:std::max(0.f,zeroGravityMinimumSpeed50ST-speed);
            // UNSETTLED (as ice note item 5): N64 Slow under the line also
            // cancels boost in ZON; Revenge boost is racer state (+64/engine
            // +34) with no host-side owner, so it is not mapped.
        }else if(tractionEnabled_){
            const auto count=std::min<std::uint32_t>(terrain.collectedCount,
                static_cast<std::uint32_t>(terrain.collectedIdentities.size()));
            if(count){ // airborne (empty list): targets hold the last surface
                traction=1.f;friction=1.f;bonus=0.f;owner=noSurfaceOwner50ST;
                for(std::uint32_t i=0;i<count;++i){
                    const auto k=ownerOfIdentity(terrain.collectedIdentities[i]);
                    const auto* row=ownerRowValues(k);
                    if(!row)continue; // triggers, pods, other bodies: no row
                    // Lowest penalty / highest bonus wins, as 217A90's dust
                    // gate lets any collected body decide.
                    if(row->traction<traction){traction=row->traction;owner=k;}
                    if(row->friction<friction){friction=row->friction;if(owner==noSurfaceOwner50ST)owner=k;}
                    if(row->speedBonus>bonus){bonus=row->speedBonus;if(owner==noSurfaceOwner50ST)owner=k;}
                }
            }
        }else bonus=0.f; // traction rules off and not in flight: no bonus source
        if(finished)traction=1.f; // N64 0x80066D70: finished pod gets grip target 1.0
        if(traction!=r.gripTarget){log(slot,"grip",r.gripTarget,traction,r.grip,owner);r.gripTarget=traction;r.gripChangeStep=r.steps;r.gripSettled=false;}
        if(friction!=r.factorTarget){log(slot,"speed",r.factorTarget,friction,r.factor,owner);r.factorTarget=friction;r.factorChangeStep=r.steps;r.factorSettled=false;}
        // The ZON 75-speed target changes every step: log only zero <-> non-zero
        // and strip changes, never the per-step drift.
        if(bonus!=r.bonusTarget){
            if((bonus==0.f)!=(r.bonusTarget==0.f)||(!flight&&bonus!=r.bonusTarget))
                log(slot,flight?"bonus_zero_g":"bonus",r.bonusTarget,bonus,r.bonus,owner);
            r.bonusTarget=bonus;r.bonusChangeStep=r.steps;r.bonusSettled=false;}
        r.owner=owner;
        if(flight){r.grip=1.f;r.factor=1.f;}
        const float step=surfaceEaseRatePerSecond50ST*dt;
        ease(r.grip,r.gripTarget,step);ease(r.factor,r.factorTarget,step);
        ease(r.bonus,r.bonusTarget,speedBonusEaseRatePerSecond50ST*dt);
        if(!r.gripSettled&&r.grip==r.gripTarget){r.gripSettled=true;settled(slot,"grip",r.grip,r.steps-r.gripChangeStep);}
        if(!r.factorSettled&&r.factor==r.factorTarget){r.factorSettled=true;settled(slot,"speed",r.factor,r.steps-r.factorChangeStep);}
        if(!r.bonusSettled&&r.bonus==r.bonusTarget){r.bonusSettled=true;
            if(!flight)settled(slot,"bonus",r.bonus,r.steps-r.bonusChangeStep);}
    }
    // Race start / restart: N64 pod init (0x80073010) sets grip = factor = 1,
    // bonus = 0.
    void resetRacers()noexcept {racers_.fill({});}

private:
    using Block=assets::ParameterBlock48F;
    static bool validValue(float v)noexcept {return std::isfinite(v)&&v>0.f&&v<=1.f;}
    static bool validBonus(float v)noexcept {return std::isfinite(v)&&v>=0.f&&v<=speedBonusLimit50ST;}
    static bool nonDefault(const SurfaceRow50ST& r)noexcept {
        return r.traction!=1.f||r.friction!=1.f||r.speedBonus!=0.f||r.zeroGravity!=0;}
    const SurfaceRow50ST* ownerRowValues(std::uint32_t owner)const noexcept {
        if(owner>=ownerRow_.size())return nullptr;
        const auto row=ownerRow_[owner];return row<surfaceRowCount50ST?&rows_[row]:nullptr;
    }
    static std::string lower(std::string s){for(auto& ch:s)if(ch>='A'&&ch<='Z')ch=static_cast<char>(ch-'A'+'a');return s;}
    static std::string tobName(const char* scb){
        if(!scb)return {};std::string s(scb);const auto dot=s.rfind('.');
        if(dot==std::string::npos)return {};return s.substr(0,dot)+".tob";
    }
    static bool hasMember(const assets::ResArchive48C& a,const std::string& m){
        for(const auto& e:a.entries)if(lower(e.name)==lower(m))return true;return false;}
    static const Block* block(const assets::ParameterDocument48F& d,const std::string& name){
        const Block* out=nullptr;for(const auto& b:d.blocks)if(lower(b.name)==name){if(out)return nullptr;out=&b;}return out;}
    static const std::string* field(const Block& b,const char* name){
        const std::string* out=nullptr;for(const auto& f:b.fields)if(lower(f.name)==name){if(out)return nullptr;out=&f.value;}return out;}
    // 158B80 default 1.0 when absent. Values outside (0,1] are refused (1.0).
    float number(const Block& b,const char* key,std::size_t row)const{
        const auto* v=field(b,key);if(!v)return 1.f;float x=1.f;
        if(!assets::parseTrackNumber50AD(v->data(),v->size(),x)||!validValue(x)){
            std::cerr<<"surface_rules value_refused row="<<row<<' '<<key<<'='<<*v<<'\n';return 1.f;}
        return x;
    }
    // New key, same float rule, default 0. Values outside [0,1000] refused (0).
    float bonusNumber(const Block& b,const char* key,std::size_t row)const{
        const auto* v=field(b,key);if(!v)return 0.f;float x=0.f;
        if(!assets::parseTrackNumber50AD(v->data(),v->size(),x)||!validBonus(x)){
            std::cerr<<"surface_rules value_refused row="<<row<<' '<<key<<'='<<*v<<'\n';return 0.f;}
        return x;
    }
    // New key, integer 0..3 (one decimal digit, as the converter writes it),
    // default 0. Anything else is refused (0) and logged.
    std::uint8_t zeroGravityValue(const Block& b,const char* key,std::size_t row)const{
        const auto* v=field(b,key);if(!v)return 0;
        std::string t=*v;while(!t.empty()&&(t.back()==' '||t.back()=='\t'||t.back()=='\r'))t.pop_back();
        std::size_t at=0;while(at<t.size()&&(t[at]==' '||t[at]=='\t'))++at;t=t.substr(at);
        if(t.size()==1&&t[0]>='0'&&t[0]<='3')return static_cast<std::uint8_t>(t[0]-'0');
        std::cerr<<"surface_rules value_refused row="<<row<<' '<<key<<'='<<*v<<'\n';return 0;
    }
    static void ease(float& x,float target,float step)noexcept {
        const float d=target-x;
        if(std::fabs(d)<=step)x=target; // lands exactly on the target (and on 1.0f / 0.0f)
        else x+=d>0.f?step:-step;
    }
    void log(std::size_t slot,const char* rule,float from,float to,float now,std::uint32_t owner)const{
        const bool enter=std::string(rule).rfind("bonus",0)==0?to>0.f:to<1.f;
        std::cerr<<"surface_rules "<<(enter?"enter":"leave")<<" slot="<<slot<<" rule="<<rule
            <<" target_from="<<from<<" target_to="<<to<<" current="<<now<<" step="<<racers_[slot].steps
            <<" owner="<<ownerName(owner)<<'\n';
    }
    void settled(std::size_t slot,const char* rule,float value,std::uint64_t steps)const{
        std::cerr<<"surface_rules settled slot="<<slot<<" rule="<<rule<<" value="<<value
            <<" steps="<<steps<<'\n';
    }
    void resetCourse()noexcept {loaded_=false;rows_.fill({});ownerRow_.clear();ownerName_.clear();firstIdentity_=0;
        zeroGravityOnOwners_=zeroGravityOffOwners_=0;racers_.fill({});}

    bool tractionEnabled_=false,zeroGravityEnabled_=false,loaded_=false;
    bool forced_=false,forcedBonus_=false,forcedZeroG_=false;
    std::size_t forcedEvery_=1,forcedBonusEvery_=1,forcedZeroGEvery_=2,forcedZeroGOffset_=1;
    float forcedBonusValue_=0.f;SurfaceRow50ST forcedRow_{};
    std::array<SurfaceRow50ST,surfaceRowCount50ST> rows_{};
    std::vector<std::uint8_t> ownerRow_;     // per static collision owner: row index
    std::vector<std::string> ownerName_;     // per owner: SLT prefix (lower case), logs and matching
    std::uint32_t firstIdentity_=0;
    std::size_t zeroGravityOnOwners_=0,zeroGravityOffOwners_=0;
    std::array<SurfaceRacer50ST,surfaceRacerSlots50ST> racers_{};
};
inline SurfaceRules50ST surfaceRules50ST;
} // namespace rr::game
