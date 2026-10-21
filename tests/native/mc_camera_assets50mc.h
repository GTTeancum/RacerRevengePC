#pragma once
#include "mc_cold_sources50la.h"
#include "original_camera_session50kx.h"
#include "runtime_startup50fl_fixture.h"

namespace mc_camera_assets50MC {
// Stable cold authoring leaf. This owns CAM and retains the actual MC source.
// PHY is a borrow from the actual selected racer rig, exactly like existing
// AuthoredStorage; root must retain that unchanged rig through Session teardown.
// This leaf neither constructs Session nor suppresses authored scene cuts.
class Owner final {
 std::shared_ptr<const mc_cold_sources50LA::Owner> source_;
 std::vector<std::uint8_t> cam_;
 const std::vector<std::uint8_t>* phy_=nullptr;
 original_camera_session50KX::AuthoredInputs inputs_{};
 const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 bool prepare(std::shared_ptr<const mc_cold_sources50LA::Owner> source,
   const std::filesystem::path& discPods,const std::string& pod,
   const std::vector<std::uint8_t>& actualPhy,std::string& error){
  if(thread_!=std::this_thread::get_id()||attempted_){error="MC camera requires fresh same-thread storage";return false;}attempted_=true;
  try{
   using pod_materials50ed_fixture::detail::need;
   need(source&&source->ready(),"MC camera requires actual cold source");
   need(pod.size()==5&&pod[0]=='P'&&pod[1]=='O'&&pod[2]=='D'&&pod[3]>='0'&&pod[3]<='9'&&pod[4]>='0'&&pod[4]<='9'&&!actualPhy.empty(),
    "MC camera requires actual selected rig authoring");
   const auto& doc=source->triggerParameters();const auto& tob=doc.original_bytes;
   need(tob.size()==186642&&pod_materials50ed_fixture::detail::sha256(tob)==
    "7070700a884c24a98a0a89d4ea58e61e34f9a89e19af3ca239779c56c0dd14b0","MC camera original TOB differs");
   const rr::assets::ParameterBlock48F* cameras=nullptr;
   for(const auto& b:doc.blocks){need(b.name!="321_anims","MC unexpected countdown section");if(b.name=="cscameras"){need(!cameras,"MC duplicate camera section");cameras=&b;}}
   need(cameras&&cameras->offset==36822&&cameras->fields.size()==1&&cameras->fields[0].name=="numberofcameras"&&
    cameras->fields[0].value=="4"&&cameras->fields[0].value_offset==36869,"MC authored camera count differs");
   auto cam=runtime_startup50fl::readBytes(discPods/pod/(pod+".CAM"));
   need(!cam.empty(),"MC selected original CAM absent");
   // Existing native loader authenticates exact selected POD CAM before parsing.
   // No renderer/camera placement occurs in this cold check.
   // Do not call the native loader here: the real Session owns preset population.
   cam_=std::move(cam);source_=std::move(source);phy_=&actualPhy;
   inputs_={cam_.data(),cam_.size(),reinterpret_cast<const char*>(actualPhy.data()),actualPhy.size(),
    static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&actualPhy)),tob.data(),tob.size(),rr::game::OriginalCameraStartCourse::mc,source_.get()};
   error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool prepared()const noexcept{return thread_==std::this_thread::get_id()&&source_&&source_->ready()&&phy_&&
   inputs_.phy==reinterpret_cast<const char*>(phy_->data())&&inputs_.phyBytes==phy_->size()&&inputs_.tob==source_->triggerParameters().original_bytes.data();}
 std::size_t camBytes()const noexcept{return cam_.size();}
 std::size_t camCapacity()const noexcept{return cam_.capacity();}
 const mc_cold_sources50LA::Owner* sourceIdentity()const noexcept{return prepared()?source_.get():nullptr;}
 const original_camera_session50KX::AuthoredInputs* inputs()const noexcept{return prepared()?&inputs_:nullptr;}
};
}
