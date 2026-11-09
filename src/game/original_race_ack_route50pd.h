#pragma once
// Native typed form of the original Results acknowledgement branch 1AB5BC
// (inside race vtable+78 1AB520, taken when Results+20C is nonzero and
// App2E0 is nonzero). It chooses the 17DE10(App, flags, screen, page)
// request; the caller performs the reached side effects in source order.
#include <cstdint>

namespace rr::game {
struct OriginalRaceAckFacts50PD {
    std::uint32_t mode34=0;          // P+34
    std::uint32_t order187B10=0;     // s1, read before 1875C0
    std::uint32_t boundary102CC=0;   // P+102CC after 1875C0
    bool record7E4=false;            // P+7E4 bit0 (1ABFC0 store at 1AB598)
    std::uint8_t finalMedal=0;       // M[C][C][12], P+1938+C*0x894+C*0x50+s1 at 1AB730
    bool unlockPending187E30=false;  // 187E30
};
enum class OriginalRaceAckRoute50PD:std::uint8_t {hall,win,ending,unlock,garage,course,assertion};
struct OriginalRaceAckDestination50PD {
    OriginalRaceAckRoute50PD route=OriginalRaceAckRoute50PD::assertion;
    std::uint32_t flags=0,screen=0,page=0;   // 17DE10 a1..a3
    bool tournament=false;                   // 1875C0/187D80, 100BC, 26D070 were reached
    bool insertRecords1AC440=false;          // 1AC440(App28C) precedes 17DE10
    bool savePending1AB648=false;            // 100BC bit0 store (tournament: 1AB648)
    std::uint32_t sourcePC=0;                // 17DE10 call site
};
inline const char* originalRaceAckRouteName50PD(OriginalRaceAckRoute50PD r)noexcept {
    switch(r){
    case OriginalRaceAckRoute50PD::hall:return "hall";
    case OriginalRaceAckRoute50PD::win:return "win";
    case OriginalRaceAckRoute50PD::ending:return "ending";
    case OriginalRaceAckRoute50PD::unlock:return "unlock";
    case OriginalRaceAckRoute50PD::garage:return "garage";
    case OriginalRaceAckRoute50PD::course:return "course";
    default:return "assertion";
    }
}
inline OriginalRaceAckDestination50PD originalRaceAckDestination1AB5BC(const OriginalRaceAckFacts50PD& f)noexcept {
    using R=OriginalRaceAckRoute50PD;
    OriginalRaceAckDestination50PD d;
    const auto request=[&](R route,std::uint32_t flags,std::uint32_t screen,std::uint32_t page,std::uint32_t pc)noexcept {
        d.route=route;d.flags=flags;d.screen=screen;d.page=page;d.sourcePC=pc;return d;};
    if(f.mode34==2u){
        d.tournament=true;d.savePending1AB648=true; // 1AB600..1AB648 precede every row
        if(f.record7E4){d.insertRecords1AC440=true;return request(R::hall,6,11,1,0x1ab698);}
        if(f.boundary102CC){
            const auto next=f.order187B10+1u;
            if(next==4u||next==8u)return request(R::win,6,7,1,0x1ab6fc);
            if(next==13u){
                if(f.finalMedal==1u)return request(R::ending,0x24,3,0,0x1ab744);
                return request(R::win,6,7,1,0x1ab758);
            }
            d.route=R::assertion;d.sourcePC=0x1ab7c8; // PodRaceEvent.cpp:2244, no request
            return d;
        }
        if(f.unlockPending187E30)return request(R::unlock,6,10,1,0x1ab7f8);
        return request(R::garage,6,13,1,0x1ab818);
    }
    if(f.record7E4){d.insertRecords1AC440=true;d.savePending1AB648=true;return request(R::hall,6,11,1,0x1ab884);}
    if(f.unlockPending187E30){d.savePending1AB648=true;return request(R::unlock,6,10,1,0x1ab8e8);}
    return request(R::course,6,6,1,0x1ab908); // a2 copied from a1 at 1AB90C
}
}
