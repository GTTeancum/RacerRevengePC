#pragma once
#include "rc_particle_authoring50rc.h"
#include "../../src/assets/model_geometry49o.h"
#include "../../src/game/scene_graph49p.h"
#include <array>
#include <cfenv>
#include <cstring>
#include <memory>
#include <thread>

namespace rc_particle_runtime50RC {class Owner;}
namespace rc_particle_visual50RC {class Window;}
namespace rc_particle_attachments50RC {
namespace a=rr::assets;namespace g=rr::game;namespace source=rc_particle_authoring50RC;
struct Member {const char* name;std::size_t bytes;const char* sha;const char* root;};
inline constexpr std::array<Member,7> members{{
 {"geometry\\rockj06.psg",3216,"ce83a6917c317b210905ee392185cd5818ea84b06735b506e54c9a466928e097","rockj06"},
 {"geometry\\mushroom03_149.psg",2324,"e64d106d6d61af94bfa90edf9ef0402b2e6fd81829905c0d4e6361bcfb925871","Mushroom03_149"},
 {"geometry\\rockf18.psg",1568,"5027b9213e27277d05e80649b53cef0772c0ae8b87e1fcf7306ed51f94397b92","rockf18"},
 {"geometry\\rocka61.psg",3696,"e51365efeeda53e26bc187eb8dd3ab421091937a2cbbc440fadf4b35ce3f16c4","rocka61"},
 {"geometry\\mushroomnewlittle03.psg",2916,"84759cb682319855bf7d6291c0605d1989d622d1ea4f34a7e22de4254d01d34f","MushroomNewLittle03"},
 {"geometry\\firep0t01.psg",4420,"b49a4fb65347b66fa457843b404c0b36b8e12eb8e038ed85ab12a20ab1b5801e","FireP0t01"},
 {"geometry\\cylinder07.psg",1952,"433e88e10d4923879b2e84a0f3d349d2b33af70109770c35b3323c3c7099f4fa","Cylinder07"}
}};
enum class FireGate {Unavailable,Stale,Current};
struct Diagnostic {std::size_t attachment=0,payload=0;std::uint32_t sourcePC=0;std::string message;a::DecodeError48C decode{};a::GeometryDiagnostic49O geometry{};};
// Ten native owned projections of the actual one-root PSG placements. This
// owner establishes graph identity/transform. Its private visual lane accepts
// completed-instance publications only from the actual retained Window; graph
// readiness alone supplies no observer/stamp or counted Entity ancestry.
class Owner final {
 friend class ::rc_particle_runtime50RC::Owner;
 friend class ::rc_particle_visual50RC::Window;
 struct Root {a::PsgNode49O original{};g::SceneNode49P node{};g::SceneGraph49P graph{};const a::WorldInstance48F* placement=nullptr;};
 std::shared_ptr<const source::Owner> source_;
 std::array<Root,source::attachmentCount> roots_{};
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,ready_=false;
 const ::rc_particle_visual50RC::Window* visual_=nullptr;
 std::uint64_t visualGeneration_=0;
 // Native completed-instance policy, not original gp8BE0/zone+A4.
 std::uint64_t renderAttempt_=0;
 std::array<std::uint64_t,4> completedAttempts_{};
 std::array<bool,4> completed_{};
 bool visualAttempted_=false,visualFrame_=false,renderAttemptValid_=false;
 bool renderPublicationRetired_=false;
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 static void need(bool value,const char* message){if(!value)throw std::runtime_error(message);}
 const g::SceneGraph49P* graph(std::size_t i)const noexcept{return ready()&&i<roots_.size()?&roots_[i].graph:nullptr;}
 // Only the concrete live Window can bind and publish. The four pointer
 // inputs must be the SAME immutable source placements, never copied rows.
 bool bindVisualWindow(::rc_particle_visual50RC::Window& window,std::uint64_t generation,
  const rc_cold_sources50LA::Owner& cold,const std::array<const a::WorldInstance48F*,4>& fire)noexcept {
  if(!ready()||visualAttempted_||!generation||source_->sourceOwner()!=&cold)return false;
  for(std::size_t i=0;i<4;++i)if(fire[i]!=roots_[5+i].placement||
    source_->descriptor(source_->attachment(5+i).descriptor).kind!=source::Kind::Fire8)return false;
  visualAttempted_=true;visual_=&window;visualGeneration_=generation;return true;
 }
 bool visualMatches(const ::rc_particle_visual50RC::Window& window,std::uint64_t generation)const noexcept {
  return ready()&&visual_==&window&&generation&&generation==visualGeneration_;
 }
 bool beginVisualFrame(::rc_particle_visual50RC::Window& window,std::uint64_t generation,std::uint64_t actualRenderAttempt)noexcept {
  if(!visualMatches(window,generation)||visualFrame_||!actualRenderAttempt||actualRenderAttempt<=renderAttempt_)return false;
  renderAttempt_=actualRenderAttempt;renderAttemptValid_=false;renderPublicationRetired_=false;visualFrame_=true;return true;
 }
 bool publishCompletedSourceInstance(::rc_particle_visual50RC::Window& window,std::uint64_t generation,
  const a::WorldInstance48F& actualPlacement)noexcept {
  if(!visualMatches(window,generation)||!visualFrame_)return false;
  for(std::size_t i=0;i<4;++i)if(roots_[5+i].placement==&actualPlacement){
   completedAttempts_[i]=renderAttempt_;completed_[i]=true;return true;}
  return false;
 }
 bool endVisualFrame(::rc_particle_visual50RC::Window& window,std::uint64_t generation,bool completed)noexcept {
  if(!visualMatches(window,generation)||!visualFrame_)return false;
  visualFrame_=false;renderAttemptValid_=completed;return true;
 }
 // Renderer retirement revokes eligibility while the Window/graphs remain
 // retained. A later actual draw must use a newer attempt before publication.
 bool invalidateVisualPublication(::rc_particle_visual50RC::Window& window,std::uint64_t generation)noexcept {
  if(!visualMatches(window,generation)||visualFrame_)return false;
  renderPublicationRetired_=true;renderAttemptValid_=false;return true;
 }
 bool unbindVisualWindow(::rc_particle_visual50RC::Window& window,std::uint64_t generation)noexcept {
  if(!visualMatches(window,generation)||visualFrame_)return false;
  visual_=nullptr;visualGeneration_=0;renderAttemptValid_=false;return true;
 }
 FireGate fireGate(std::size_t attachment)const noexcept {
  if(!ready()||!visual_||visualFrame_||renderPublicationRetired_||attachment<5||attachment>=9)return FireGate::Unavailable;
  if(renderAttempt_&&!renderAttemptValid_)return FireGate::Unavailable;
  const auto i=attachment-5;
  return renderAttemptValid_&&completed_[i]&&completedAttempts_[i]==renderAttempt_?FireGate::Current:FireGate::Stale;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){if(!thread()||visual_||visualFrame_)std::terminate();}
 static constexpr std::size_t retainedPayloadBytes()noexcept{return sizeof(Owner);}
 // Source authoring/source payload is charged separately and retained once.
 // The peak includes the bounded extracted member and existing SHA scratch.
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept{return sizeof(Owner)+4*4420u+65536u;}
 bool prepare(std::shared_ptr<const source::Owner> authored,std::size_t availablePayloadBytes,Diagnostic& d)noexcept {
  d={};if(!thread()||attempted_){d.message="RC attachment requires one owner-thread attempt";return false;}attempted_=true;
  try{
   need(authored&&authored->ready()&&availablePayloadBytes>=maximumAdditionalPayloadBytes()&&std::fegetround()==FE_TONEAREST,
        "RC actual attachment source/budget/rounding unavailable");
   source_=std::move(authored);const auto& cold=*source_->sourceOwner();
   std::array<a::PsgNode49O,7> original{};
   for(std::size_t i=0;i<members.size();++i){d.attachment=i;const auto& pin=members[i];std::vector<std::uint8_t> bytes;
    need(cold.extractMember(pin.name,bytes,d.decode,pin.bytes)&&bytes.size()==pin.bytes&&
       pod_materials50ed_fixture::detail::sha256(bytes)==pin.sha,"RC actual attachment PSG identity");
    a::PsgHierarchyView49O hierarchy{};
    need(a::readPsgHierarchy49O(bytes.data(),bytes.size(),hierarchy,d.geometry)&&hierarchy.nodeCount==1&&
       a::readPsgNode49O(hierarchy,0,original[i])&&original[i].parent==-1&&std::strcmp(original[i].name,pin.root)==0,
       "RC actual one-root hierarchy rejected");
    for(unsigned k=0;k<16;++k)need(original[i].local[k]==(k%5==0?1.f:0.f),"RC source root matrix changed");
   }
   for(std::size_t i=0;i<roots_.size();++i){d.attachment=i;const auto& attached=source_->attachment(i);
    const auto& descriptor=source_->descriptor(attached.descriptor);const auto& pin=members.at(attached.descriptor);
    need(attached.worldInstance&&attached.worldIndex<cold.world().instances.size()&&
      attached.worldInstance==&cold.world().instances[attached.worldIndex]&&descriptor.childObject=="NONE"&&
      source::detail::same(attached.worldInstance->geometry_member,pin.name),"RC actual source model/root join changed");
    auto& root=roots_[i];root.original=original[attached.descriptor];root.placement=attached.worldInstance;
    std::memcpy(root.node.localC0.v,root.original.local,sizeof root.original.local);
    std::memcpy(root.node.world100.v,root.original.local,sizeof root.original.local);
    root.node.parent180=root.node.firstChild184=root.node.nextSibling188=g::SceneNodeIndex49P::none;
    // Native projection initializes only the represented cache protocol. It
    // does not manufacture source model flags or a visual observer19C.
    root.node.sceneFlags190=0;root.graph={&root.node,1};g::BodyMatrix48G placement{};
    for(unsigned k=0;k<16;++k){placement.v[k]=static_cast<float>(root.placement->matrix[k]);
      need(std::isfinite(placement.v[k]),"RC placement binary32 projection rejected");}
    d.sourcePC=0x289b20u;
    need(g::setParentlessWorldMatrix49P(root.graph,static_cast<g::SceneNodeIndex49P>(0),placement),"RC actual source placement setter rejected");
    d.sourcePC=0x2897b0u;
    need(g::refreshSceneWorld49P(root.graph,static_cast<g::SceneNodeIndex49P>(0)),"RC actual source graph refresh rejected");
   }
   ready_=true;d.payload=retainedPayloadBytes();d.message.clear();d.sourcePC=0;return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.message="RC attachment allocation or preparation failed";}catch(...){}return false;}
 }
 bool ready()const noexcept{return thread()&&ready_&&source_&&source_->ready();}
 const source::Owner* authoringOwner()const noexcept{return ready()?source_.get():nullptr;}
 const rc_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?source_->sourceOwner():nullptr;}
 const a::WorldInstance48F* placement(std::size_t i)const noexcept{return ready()&&i<roots_.size()?roots_[i].placement:nullptr;}
 // Read-only scalar observation, not a graph borrower or render authority.
 const g::BodyMatrix48G* currentWorld(std::size_t i)const noexcept{return ready()&&i<roots_.size()?&roots_[i].node.world100:nullptr;}
 const a::PsgNode49O* originalRoot(std::size_t i)const noexcept{return ready()&&i<roots_.size()?&roots_[i].original:nullptr;}
};
} // namespace rc_particle_attachments50RC
