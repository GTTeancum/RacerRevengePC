#include "original_ui_garage_fog_color_lease50kg.h"
#include "original_ui_garage_manager_fog50kg.h"
#include "original_ui_garage_scene_lighting50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
namespace rr::platform {
namespace {
template<class A,class B>bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
    return a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
}
OriginalUiGarageFogColorLease50KG::OriginalUiGarageFogColorLease50KG(
    const ::original_main_entry50KG::Application& root,
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> display)noexcept
    :display_(std::move(display)),video_(display_->video_),issuer_(&root),
     applicationLifetime_(root.animationClock()),generation_(display_->generation_) {display_->pin();}
std::shared_ptr<OriginalUiGarageFogColorLease50KG> OriginalUiGarageFogColorLease50KG::retain(
    const ::original_main_entry50KG::Application& root,
    const std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA>& display)noexcept {
    std::uint32_t pause=0;
    if(!display||!display->canRetainSourceFogField()||!sameOwner(display,display->weak_from_this().lock())||
       !root.animationClock().readPause11C(pause)||
       display->application_!=&root||!display->generation_||display->generation_!=root.raceDisplayGeneration50LA_||
       !sameOwner(display,root.raceDisplayOwner50LA_.lock())||!sameOwner(display->video_,root.videoOwner_))return {};
    try{auto owner=std::shared_ptr<OriginalUiGarageFogColorLease50KG>(new OriginalUiGarageFogColorLease50KG(root,display));
        owner->issuedOwner_=owner;return owner;}
    catch(...){return {};}
}
bool OriginalUiGarageFogColorLease50KG::currentFor(const ::original_main_entry50KG::Application* root)const noexcept {
    std::uint32_t pause=0;
    // Thread and retained lifetime gate precede every raw Application access.
    return display_&&display_->ownerThread()&&applicationLifetime_.readPause11C(pause)&&
        root&&root==issuer_&&display_->sourceFogFieldLifetimeCurrent()&&display_->application_==root&&
        generation_&&generation_==display_->generation_&&generation_==root->raceDisplayGeneration50LA_&&
        sameOwner(display_,display_->weak_from_this().lock())&&
        sameOwner(display_,root->raceDisplayOwner50LA_.lock())&&sameOwner(video_,display_->video_)&&
        sameOwner(video_,root->videoOwner_)&&applicationLifetime_.sameOwner(root->animationClock());
}
bool OriginalUiGarageFogColorLease50KG::currentOwnedBy(
    const std::shared_ptr<OriginalUiGarageFogColorLease50KG>& owner,
    const ::original_main_entry50KG::Application* root)const noexcept {
    return owner.get()==this&&sameOwner(owner,issuedOwner_.lock())&&currentFor(root);
}
bool OriginalUiGarageFogColorLease50KG::store13E8A8(const OriginalUiGarageManagerFog50KG& fog)noexcept {
    if(!currentFor(fog.issuer_)||!fog.colorWriteCurrentFor(issuer_))return false;
    // Literal source SW only. No GL call, image/draw completion, previous-color
    // save or invented restore. Unknown source28 becomes known at this write.
    display_->sourceFogColor28_=fog.fog_->color30;
    display_->sourceFogColor28Known_=true;return true;
}
bool OriginalUiGarageFogColorLease50KG::storeScene13E8A8(const OriginalUiGarageSceneLighting50KG& scene)noexcept {
    if(!currentFor(scene.issuer_)||!scene.colorWriteCurrentFor(issuer_))return false;
    display_->sourceFogColor28_=*scene.fog3EC_->color30;
    display_->sourceFogColor28Known_=true;
    //Same actual13E8A8 scalar field as manager Fog. No previous-color restore.
    return true;
}
OriginalUiGarageFogColorLease50KG::~OriginalUiGarageFogColorLease50KG()noexcept {if(display_)display_->unpin();}
}
namespace original_main_entry50KG {
std::shared_ptr<rr::platform::OriginalUiGarageFogColorLease50KG> Application::retainGarageFogColor13E8A8()const noexcept {
    return rr::platform::OriginalUiGarageFogColorLease50KG::retain(*this,raceDisplayOwner50LA_.lock());
}
}
