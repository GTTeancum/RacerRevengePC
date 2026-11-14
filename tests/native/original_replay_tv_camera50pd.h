#pragma once
// The original Results-replay camera (SLUS_202.68), one path for every course.
// Disassembly: D:\Programming\GitHub\RacerRevengePC\work\scratch\dis_all.txt.
//
// Load   175040..175064 new TV (0xF0, ctor 2D2870, vtable 3D5160) at race48+78.
//        175650..1756A8 sprintf("%s%s", "tracks\\"-style prefix, "<code>TVCam.pos")
//        then 2D29F0; failure deletes it (race48+78 = 0).
//        1752D0 registers it with 115DE0 after 64/68/6C/70 (only when present).
//        1750A0 -> 2D2090(tv, racer340, 3.0f, 70.0f): body-attached, target
//        Y offset +0C = 3.0 (read by 2D2430), +08 = 1.4667*70 (not read by the
//        body-attached 2D3400 branch reproduced in tv_camera_update49f).
// Enter  1AB520 sets race1B0 bit4; 1AC8E0 1ACC24 -> 1ACE14 and the replay loop
//        1A81DC..1A8204: if race48+78: 2D30B0, 177AB0(race48, 6, 0, -1).
// 177AB0 mode 6 (17849C..178538, jump table 3A8080[6]): TV vtable+8 = 2D20C0
//        (empty; no pose import), 115E60 selects the TV child, scene position =
//        TV record[current40].Position, zone = race74+4C0[InitialZone - 1],
//        114770(method 2) with no 1781F8 saved20 overwrite, mode44 = 6. A
//        same-mode 6 request re-runs it (177AE4..177AF8 let 4 and 6 through).
// Frame  175D70 prefix 175D90..17601C, then 17622C: bit4 && race48+78 ->
//        director 176924..176D48 (regardless of the current mode):
//        2D30D0 (vtable+24) with 28B180(racer340->288) local XYZ and dt; it
//        returns 1 only when it switched camera. On a switch: 2D3750 copies the
//        camera's 7 thresholds truncated to int (cvt.w.s); one 308030 draw,
//        r = cvt.w.s(100 * 2^-31 * raw31). s1 = old mode, s2 = 0.
//          r >= tv          177AB0(6)                          s2 = 1
//          else             177AB0(0, no preset); s2 |= old mode != 0;
//                           p = current Follow preset id (2843C0), then
//            r >= follow     preset 0
//            r >= left_side  draw u = 2^-31*raw31: u<0.33 -> 0, u<0.67 -> 3, else 2
//            r >= right_side same three-way draw
//            r >= look_back  preset 3
//            else            preset 2
//                           via Follow vtable+1C = 17BD00(id, 0, 0); +10C = 1;
//                           s2 |= new id != p.
//          then r >= blur && s2: cut90 = 2.0, 2AED10(blur, 100, 0).
//        115CB0 then runs the selected child: mode 6 -> TV 2D3400 (vtable+14).
// Shake  2D38C0 walks race28C+B0 (the racer manager) i < +30 via 1AF330:
//        skips racer+40 != 0, reads 28B180(racer340->288) local XYZ and the
//        racer340 body velocity: the port's 8 banked racers in manager order.
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/tv_camera49e.h"
#include "../../src/game/tv_camera_selection49e.h"
#include "../../src/game/tv_camera_update49f.h"
#include "../../src/game/camera_frame49f.h"
#include "../../src/game/coupled_body_advancement49m.h"
#include "../../src/game/original_shared_rng50iu.h"
#include "../../src/game/original_follow_controller.h"
#include <array>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

namespace original_replay_tv50PD {
using rr::assets::TVCameraView49E;
constexpr float targetYOffset2D2090=3.0f;   // 1752A0: lui 0x4040
struct Decision {
    bool switched=false;        // 2D30D0 returned 1
    bool tv=false;              // r >= tv: 177AB0(6)
    std::int32_t preset=0;      // Follow preset id when !tv
    bool presetChanged=false;   // s2 contribution of the preset step (needs current id)
    bool blurIfChanged=false;   // r >= blur
    std::int32_t roll=-1;
};

class Owner {
public:
    bool present()const noexcept{return view_.loaded();}
    const TVCameraView49E& view()const noexcept{return view_;}
    std::uint64_t childKey()const noexcept{return reinterpret_cast<std::uintptr_t>(this);}
    // 175650..1756A8, every course, the course's own archive.
    bool load175650(const std::vector<std::uint8_t>& archiveBytes,const char* code,std::size_t archiveSize)noexcept{
        clear();
        // Common course-archive rule: no stock-size gate (archiveSize is the
        // stock receipt value only); the shared decode capacity bounds it.
        (void)archiveSize;
        if(!code||archiveBytes.empty())return false;
        std::string name="tracks\\";
        for(const char* p=code;*p;++p)name.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(*p))));
        name+="TVCam.pos";
        try{
            rr::assets::ResArchive48C archive;rr::assets::DecodeError48C error;
            const rr::assets::DecodeLimits48C limits{}; // shared course-archive capacity
            if(!rr::assets::decodeResource48C(archiveBytes,archive,error,nullptr,limits))return false;
            std::vector<std::uint8_t> member;
            if(!rr::assets::extractResMember48C(archive,name,member,error,rr::assets::max_tv_camera_text_bytes49e))return false;
            text_.assign(member.begin(),member.end());
        }catch(...){clear();return false;}
        const rr::assets::TVCameraSpan49E<const char> text{text_.data(),text_.size()};
        rr::assets::TVCameraRequirements49E need;rr::assets::TVCameraDiagnostic49E diag;
        if(!rr::assets::inspectTVCamera49E(text,need,diag)){lastError_=diag;clear();return false;}
        try{records_.resize(need.cameras);links_.resize(need.links);}catch(...){clear();return false;}
        const rr::assets::TVCameraStorage49E storage{{records_.data(),records_.size()},{links_.data(),links_.size()}};
        if(!rr::assets::loadTVCamera49E(text,storage,view_,diag)){lastError_=diag;clear();return false;}
        return true;
    }
    const rr::assets::TVCameraDiagnostic49E& lastError()const noexcept{return lastError_;}
    void reset2D30B0()noexcept{rr::game::resetTVCameraSelector49E(selector_);}
    const rr::assets::TVCameraRecord49E* current()const noexcept{
        return selector_.current40<0?nullptr:view_.camera(static_cast<std::uint32_t>(selector_.current40));
    }

    // 1769A4..176A28 and the threshold ladder of 176A50..176D48. Draws come from
    // the session's one raw 308030 owner, in source order (selector first).
    bool direct176924(const rr::assets::TVCameraVector49E& playerLocal288,float elapsed,
        rr::game::CoupledNoiseContext49M& shared,Decision& out)noexcept{
        out={};if(!present())return false;
        rr::game::TVCameraRandom49E random{shared.sharedRandomState};
        rr::game::TVCameraSelection49E selection;
        if(!rr::game::selectTVCamera49E(view_,selector_,playerLocal288,elapsed,random,selection))return false;
        shared.sharedRandomState=random.raw;
        out.switched=selection.changed;
        if(!out.switched)return true;
        const auto* camera=current();if(!camera)return false;
        const auto& t=camera->thresholds;    // 2D3750: cvt.w.s of each
        const auto i=[](float f)noexcept{return static_cast<std::int32_t>(f);};
        volatile float unit=4.656612873077392578125e-10f*static_cast<float>(rr::game::drawOriginalSharedRng50IU(shared));
        volatile float scaled=100.f*unit;
        out.roll=static_cast<std::int32_t>(scaled);
        const std::int32_t r=out.roll;
        out.blurIfChanged=r>=i(t.blur);
        if(r>=i(t.tv)){out.tv=true;return true;}
        const auto threeWay=[&]()noexcept{
            volatile float u=4.656612873077392578125e-10f*static_cast<float>(rr::game::drawOriginalSharedRng50IU(shared));
            return u<0.33f?0:(u<0.67f?3:2);};      // 3EA8F5C3, 3F2B851F, c.olt.s
        if(r>=i(t.follow))out.preset=0;
        else if(r>=i(t.left_side))out.preset=threeWay();
        else if(r>=i(t.right_side))out.preset=threeWay();
        else if(r>=i(t.look_back))out.preset=3;
        else out.preset=2;
        return true;
    }
    // 2D3400 + view, for mode 6 only (115CB0's selected TV child).
    bool update2D3400(const rr::game::TVCameraUpdateInput49F& input,rr::game::CoupledNoiseContext49M& shared,
        rr::game::CameraSceneState49F& scene,rr::game::TVCameraUpdateOutput49F& pose,
        rr::game::CameraViewOutput49F& view,rr::game::CameraViewContext49F context)noexcept{
        if(!present())return false;
        rr::game::TVCameraRandom49E random{shared.sharedRandomState};
        if(!rr::game::updateTVCameraView49F(view_,selector_,update_,input,random,scene,pose,view,context))return false;
        shared.sharedRandomState=random.raw;return true;
    }
private:
    void clear()noexcept{view_=TVCameraView49E{};selector_={};update_={};text_.clear();records_.clear();links_.clear();}
    TVCameraView49E view_{};
    rr::game::TVCameraSelectorState49E selector_{};
    rr::game::TVCameraUpdateState49F update_{};
    rr::assets::TVCameraDiagnostic49E lastError_{};
    std::vector<char> text_;
    std::vector<rr::assets::TVCameraRecord49E> records_;
    std::vector<std::uint32_t> links_;
};

// 177AB0 mode 6, 177B48..178538 + 178648. Same admission shape as the existing
// transitionOriginalAttached177AB0; differs only in the source-specific leaf:
// no child activation (2D20C0 is empty), scene point = selected TV record
// Position, zone = InitialZone - 1, and no 1781F8 saved20 overwrite (so the
// rebind gets the point twice, as the existing no-overwrite caller does).
inline rr::game::OriginalFollowTransitionResult transitionOriginalTv177AB0(
    rr::game::OriginalRaceCameraOwner& controller,const Owner& tv,
    rr::game::OriginalCameraChildRegistry& collection,rr::game::OriginalFollowSceneBinding& scene,
    rr::game::ResetVisibilityWorld50EY& visibility,rr::game::OriginalCameraSceneOwner& spatial,
    const rr::game::OriginalFollowVisibilityZones& zones,rr::game::LiveZoneWorkspace50EN& workspace) noexcept {
    using S=rr::game::OriginalFollowControllerStatus;
    rr::game::OriginalFollowTransitionResult result;
    if(!tv.present()||!collection.bound||!controller.bound||
       collection.sessionGeneration!=controller.binding.sessionGeneration||
       collection.sceneKey!=controller.binding.sceneKey||!collection.controllerKey||
       scene.sceneKey!=controller.binding.sceneKey)return result;
    if(controller.flags62&0x60u){result.status=S::requiresModeRestore;return result;}   // 177B48 restore recursion
    const auto* camera=tv.current();
    if(!camera||!collection.registered38||!collection.count30||collection.count30>64||
       !zones.keys4C0||zones.count!=visibility.lifecycle.zoneCount||
       spatial.sceneKey!=scene.sceneKey||!spatial.registration34.attached||
       !spatial.parent38||spatial.parent38>zones.count)return result;
    std::size_t selected=collection.count30;
    for(std::size_t i=0;i<collection.count30;++i){
        if(!collection.registered38[i])return result;
        if(selected==collection.count30&&collection.registered38[i]==tv.childKey())selected=i;
    }
    if(selected==collection.count30)return result;              // 115E60 needs the 1752D0 registration
    // Ordinal (InitialZone-1)+1. STAND-IN: an InitialZone outside the course's
    // zone list (MC's "09" reads 0 through strtol base 0) makes the original
    // index race74+4C0 outside its table; the port keeps the camera scene's
    // current zone instead of reading outside the table.
    const bool zoneListed=camera->initial_zone>=1&&static_cast<std::size_t>(camera->initial_zone)<=zones.count;
    const auto zone=zoneListed?static_cast<rr::game::CollectionRef50BX>(camera->initial_zone):spatial.parent38;
    const auto& p=camera->position;
    const rr::game::SpringVector4_48N point{{p.x,p.y,p.z,1}};
    result.sourcePC=0x178530u;
    result.registration=rr::game::rebindOriginalCameraScene177AB0(visibility,spatial,zone,point,point,workspace);
    if(result.registration.status!=rr::game::LiveZoneStatus50EN::Complete){result.status=S::owner;return result;}
    collection.selected34=selected;controller.mode44=6;
    scene.position180={p.x,p.y,p.z,1};scene.registrationPosition20={p.x,p.y,p.z,1};
    scene.parentZoneKey=zones.keys4C0[zone-1u];scene.parentMethod=2;scene.registrationKey=spatial.allocationSerial;
    result.status=S::complete;result.sourcePC=0x178648u;return result;
}
}
