// Private native custody for the genuine normal Pause row10 receiver.
// This token is not a public destination flag or a reconstructed Rider commit.
#pragma once
#include "original_ui_rider_profile50kg.h"
namespace original_main_entry50KG { struct Application; struct Frontend; }
namespace rr::platform {
class OriginalUiFrontend50KG;
class OriginalUiCourseSession50KG;
class OriginalUiPauseCourseOrigin50LA final {
public:
    ~OriginalUiPauseCourseOrigin50LA()=default;
    OriginalUiPauseCourseOrigin50LA(const OriginalUiPauseCourseOrigin50LA&)=delete;
    OriginalUiPauseCourseOrigin50LA& operator=(const OriginalUiPauseCourseOrigin50LA&)=delete;
private:
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiFrontend50KG;
    friend class OriginalUiCourseSession50KG;
    // Friendship for receiver operations does not grant issuance. Only Root
    // can construct this key, including from the other token friends.
    class Issuance final {
    public:
        ~Issuance()=default;
        Issuance(const Issuance&)=delete;
        Issuance& operator=(const Issuance&)=delete;
    private:
        friend struct ::original_main_entry50KG::Application;
        Issuance()noexcept{}
    };
    // Only the actual Root may issue this after retiring the old Race,
    // Camera, Display, Pause and Scope and binding its retained request to
    // this exact new Frontend. Root retains this same token until acceptance.
    OriginalUiPauseCourseOrigin50LA(Issuance,const ::original_main_entry50KG::Application& root,
        const ::original_main_entry50KG::Frontend& frontend,OriginalUiRiderProfileStamp50KG stamp)noexcept
        :root_(&root),frontend_(&frontend),preimage_(stamp){}
    bool bindSession(const OriginalUiCourseSession50KG&)noexcept;
    bool current(const OriginalUiCourseSession50KG&)const noexcept;
    bool commitEntry(const OriginalUiCourseSession50KG&,OriginalUiRiderProfileStamp50KG before,
        OriginalUiRiderProfileStamp50KG after)noexcept;
    const ::original_main_entry50KG::Application* const root_;
    const ::original_main_entry50KG::Frontend* const frontend_;
    const OriginalUiRiderProfileStamp50KG preimage_;
    const OriginalUiCourseSession50KG* session_=nullptr;
    bool entryCommitted_=false;
};
}
