#pragma once
#include <cstdint>
#include <thread>
namespace original_camera_session50KX {class Session;}
namespace rr::game {
class OriginalDirectRaceStartOwner;
class OriginalGcIntroExitOwner;
// Concrete synchronous Session-owned call join, not a callback/phase receipt.
// Only the existing intro cursors can reach their fixed199770 occurrences.
// No arbitrary setter arguments, end-entry call or renderer admission exists.
class OriginalSessionF8Intro50LA final {
    friend class ::original_camera_session50KX::Session;
    friend class OriginalDirectRaceStartOwner;
    friend class OriginalGcIntroExitOwner;
    ::original_camera_session50KX::Session* const session_;
    const std::thread::id ownerThread_=std::this_thread::get_id();
    explicit OriginalSessionF8Intro50LA(::original_camera_session50KX::Session& s)noexcept:session_(&s){}
    bool reveal(std::uint8_t& actualSessionProjection,bool direct)const noexcept;
    bool reveal176330(std::uint8_t& projection)const noexcept{return reveal(projection,true);}
    bool revealIntroExit(std::uint8_t& projection)const noexcept{return reveal(projection,false);}
public:
    OriginalSessionF8Intro50LA(const OriginalSessionF8Intro50LA&)=delete;
    OriginalSessionF8Intro50LA& operator=(const OriginalSessionF8Intro50LA&)=delete;
};
}
