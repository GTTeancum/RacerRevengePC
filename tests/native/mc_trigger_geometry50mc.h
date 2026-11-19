#pragma once
#include "mc_trigger_authoring50mc.h"
#include "ma_trigger_authoring_fixture.h"
// Deliberately the same disclosed native shape6 geometry policy as TB/MA:
// std trig/normalization and full OBB narrowphase, NOT original trig/VU parity.
namespace mc_trigger_geometry50MC {
namespace g=rr::game;
struct Shape {std::uint32_t shapeType=0;g::CollisionSphere48J sphere{};g::NativeTriggerBox50MA box{};};
class Owner final {
 std::shared_ptr<const mc_trigger_authoring50MC::Owner> source_;
 std::array<Shape,mc_trigger_authoring50MC::capacity> shapes_{};std::size_t live_=0;
 const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 bool prepare(std::shared_ptr<const mc_trigger_authoring50MC::Owner> source,std::string& error)noexcept {
  if(thread_!=std::this_thread::get_id()||attempted_){error="MC trigger geometry requires one owner-thread attempt";return false;}attempted_=true;
  try{
   if(!source||!source->ready())throw std::runtime_error("MC complete trigger authoring required");
   std::size_t boxes=0,spheres=0;const auto live=source->liveCount();for(std::size_t i=0;i<live;++i){const auto& s=source->source(i);auto& shape=shapes_[i];shape.shapeType=s.shapeType;shape.sphere.center=s.position;
    if(s.shapeType==6){++boxes;shape.box.half=s.boxHalf;shape.box.world=ma_trigger_authoring::detail::boxWorld(s.position,s.yaw,s.pitch,s.roll);
     if(!g::validNativeTriggerBox50MA(shape.box))throw std::runtime_error("MC source OBB projection rejected");shape.sphere.radius=ma_trigger_authoring::detail::boxRadius(shape.box);
    }else{if(s.shapeType!=5)throw std::runtime_error("MC source shape type");++spheres;shape.sphere.radius=s.radius;}
    if(!std::isfinite(shape.sphere.radius)||shape.sphere.radius<=0.f)throw std::runtime_error("MC invalid coarse sphere");
   }
   // Stock 14 box + 7 sphere is a receipt.
   course_archive_receipt50CI::countReceipt("mc_trigger_geometry","boxes",boxes,14);
   course_archive_receipt50CI::countReceipt("mc_trigger_geometry","spheres",spheres,7);
   live_=live;source_=std::move(source);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}catch(...){error="MC geometry failure";return false;}
 }
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&source_&&source_->ready();}
 const Shape& shape(std::size_t i)const {if(!ready()||i>=live_)throw std::runtime_error("MC geometry outside the live trigger prefix");return shapes_[i];}
 const mc_trigger_authoring50MC::Owner* sourceIdentity()const noexcept{return ready()?source_.get():nullptr;}
 static constexpr std::size_t retainedPayloadBytes()noexcept{return sizeof(Owner);}
};
}
