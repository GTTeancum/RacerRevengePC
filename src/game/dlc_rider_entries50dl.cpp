#include "dlc_rider_entries50dl.h"
// PORT EXTENSION, NOT ORIGINAL BEHAVIOUR: see dlc_rider_entries50dl.h.
#include "../assets/dlc_registry50dl.h"
#include "original_ui_course_race_domain50kg.h"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <mutex>

namespace rr::game::dlc50DL {
namespace reg=rr::assets::dlc50DL;
namespace {
// The port-owned add-on selections and Rider-page highlights, one per human
// player (0 = P1, 1 = P2); 0 = stock. Every index is checked against the size.
std::atomic<unsigned> selected[addonRiderPlayers]{};
std::atomic<unsigned> highlighted[addonRiderPlayers]{};
std::once_flag testOnce;
// Remembered add-on per player, packed: racer | anchorRider<<8 | anchorLast<<16; 0 = none.
std::atomic<unsigned> remembered[addonRiderPlayers]{};
// Race latch: the add-on that races this session and the stock rider the
// profile keeps. Host memory only; {0,0} = no add-on (stock unchanged).
std::atomic<unsigned> raceAddon{0},raceProfile{0};
}

std::size_t addonRiderCount() noexcept {return reg::addonPodCount();}
const reg::AddonPod* addonRiderAt(std::size_t index) noexcept {return reg::addonPodAt(index);}
const reg::AddonPod* addonRider(unsigned racer) noexcept {return reg::addonPod(racer);}

bool selectAddonRider(unsigned player,unsigned racer) noexcept {
    if(player>=addonRiderPlayers||!reg::addonPod(racer))return false; // bounds: P1/P2, admitted add-on numbers only
    selected[player].store(racer);return true;
}
void clearAddonRider(unsigned player) noexcept {if(player<addonRiderPlayers)selected[player].store(0);}
unsigned selectedAddonRider(unsigned player) noexcept {
    if(player>=addonRiderPlayers)return 0;
    const unsigned racer=selected[player].load();
    return reg::addonPod(racer)?racer:0u;
}
bool selectAddonRider(unsigned racer) noexcept {return selectAddonRider(0u,racer);}
void clearAddonRider() noexcept {clearAddonRider(0u);}
unsigned selectedAddonRider() noexcept {return selectedAddonRider(0u);}
bool setHighlightedAddonRider(unsigned player,unsigned racer) noexcept {
    if(player>=addonRiderPlayers||(racer&&!reg::addonPod(racer)))return false; // bounds: 0 or an admitted add-on number
    highlighted[player].store(racer);return true;
}
unsigned highlightedAddonRider(unsigned player) noexcept {
    if(player>=addonRiderPlayers)return 0;
    const unsigned racer=highlighted[player].load();
    return reg::addonPod(racer)?racer:0u;
}
bool setHighlightedAddonRider(unsigned racer) noexcept {return setHighlightedAddonRider(0u,racer);}
unsigned highlightedAddonRider() noexcept {return highlightedAddonRider(0u);}
bool rememberAddonRider(unsigned player,unsigned racer,unsigned anchorRider,bool anchorLast) noexcept {
    // bounds: P1/P2, an admitted add-on, a stock anchor rider 1..26
    if(player>=addonRiderPlayers||!reg::addonPod(racer)||racer>255u||anchorRider<1||anchorRider>26)return false;
    remembered[player].store(racer|(anchorRider<<8)|(anchorLast?1u<<16:0u));return true;
}
void forgetAddonRider(unsigned player) noexcept {if(player<addonRiderPlayers)remembered[player].store(0);}
bool rememberedAddonRider(unsigned player,RememberedAddonRider50DL& out) noexcept {
    if(player>=addonRiderPlayers)return false;
    const unsigned packed=remembered[player].load(),racer=packed&0xFFu;
    if(!packed||!reg::addonPod(racer))return false;
    out={racer,(packed>>8)&0xFFu,((packed>>16)&1u)!=0};return true;
}
std::string riderKey(unsigned racer){
    if(rr::game::nativeCourseRacerOwned50KG(racer)){
        const char nn[3]={char('0'+racer/10),char('0'+racer%10),0};
        return std::string("stock:")+nn;
    }
    const auto* pod=reg::addonPod(racer);
    return pod?pod->key:std::string();
}
bool addonRiderMayBeAi(unsigned racer,bool addonCourse) noexcept {
    const auto* pod=reg::addonPod(racer);
    if(!pod)return true;                       // stock racers: the stock field rules decide
    return addonCourse&&pod->aiOnAddonCourses; // never on a stock course (D6)
}
void applyTestAddonRider() noexcept {
    try{std::call_once(testOnce,[]{
        const char* key=std::getenv("RR_TEST_ADDON_RIDER");
        if(!key||!*key)return;
        for(std::size_t i=0;i<reg::addonPodCount();++i){
            const auto* pod=reg::addonPodAt(i);
            if(pod->key==key&&selectAddonRider(pod->number)){
                std::fprintf(stderr,"dlc_rider_test key=%s number=%u status=selected\n",key,pod->number);return;
            }
        }
        std::fprintf(stderr,"dlc_rider_test key=%s status=refused reason=\"not an admitted add-on pod\"\n",key);
    });}catch(...){}
}
unsigned latchRaceRider(unsigned profileRider,bool freePlay,const char* track) noexcept {
    const unsigned addon=freePlay?selectedAddonRider():0u;
    const auto* pod=addon?reg::addonPod(addon):nullptr;
    if(!pod){raceAddon.store(0);raceProfile.store(0);return profileRider;}
    raceProfile.store(profileRider);raceAddon.store(addon);
    std::fprintf(stderr,"dlc_race_player racer=%u key=%s stem=%s pod=%s profile_rider=%u track=%s mode=free_play\n",
        addon,pod->key.c_str(),pod->stem.c_str(),pod->podId.c_str(),profileRider,track?track:"-");
    return addon;
}
void clearRaceRider() noexcept {raceAddon.store(0);raceProfile.store(0);}
unsigned profileRider(unsigned raced) noexcept {
    const unsigned addon=raceAddon.load();
    return addon&&raced==addon?raceProfile.load():raced;
}
unsigned racedRider(unsigned profileRider) noexcept {
    const unsigned addon=raceAddon.load();
    return addon&&profileRider==raceProfile.load()?addon:profileRider;
}

} // namespace rr::game::dlc50DL
