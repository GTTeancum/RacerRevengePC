#pragma once
#include "mc_collision_sources50mc.h"
#include "track_static_candidates50ig_fixture.h"

// Retained static prefix ONLY: no Storage, topology world, query or lease issuer.
// Trigger and animated-compound candidates must be genuinely joined elsewhere.
namespace mc_static_candidates50MC {
namespace g=rr::game;namespace a=rr::assets;namespace rt=track_runtime_storage50IG;
struct PlacementRef {
 const a::CollisionMesh48I& mesh;const a::TrackCollider50AO& instance;
 std::uint32_t zone,ordinal;
};
struct Diagnostic {std::size_t retainedBytes=0,maximumReservation=0;std::string message;};
class Owner final : public std::enable_shared_from_this<Owner> {
 std::shared_ptr<const mc_cold_sources50LA::Owner> source_;
 std::shared_ptr<const mc_collision_sources50MC::Owner> collision_;
 std::unique_ptr<mc_collision_sources50MC::Owner::Borrow> meshBorrow_;
 std::vector<PlacementRef> placed_;
 std::vector<g::Type2StaticCandidate49X> candidates_;
 std::vector<track_static_candidates50IG::Radii> radii_;
 std::vector<rt::Binding> bindings_;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,ready_=false;
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 static void need(bool v,const char* why){if(!v)throw std::runtime_error(why);}
 static void add(std::size_t& n,std::size_t bytes){need(bytes<=std::numeric_limits<std::size_t>::max()-n,"MC static prefix accounting overflow");n+=bytes;}
 template<class T>static void vectorBytes(std::size_t& n,const std::vector<T>& v){need(v.capacity()<=std::numeric_limits<std::size_t>::max()/sizeof(T),"MC static prefix capacity overflow");add(n,v.capacity()*sizeof(T));}
public:
 class Borrow final {
  friend class Owner;std::shared_ptr<const Owner> owner_;
  explicit Borrow(std::shared_ptr<const Owner> owner):owner_(std::move(owner)){}
 public:
  Borrow()=default;Borrow(const Borrow&)=default;Borrow(Borrow&&)=default;
  Borrow&operator=(const Borrow&)=delete;Borrow&operator=(Borrow&&)=delete;
  ~Borrow(){if(owner_&&!owner_->thread())std::terminate();}
  explicit operator bool()const noexcept{return owner_&&owner_->prefixReady();}
  std::span<const PlacementRef> placements()const noexcept{return *this?std::span<const PlacementRef>{owner_->placed_}:std::span<const PlacementRef>{};}
  std::span<const g::Type2StaticCandidate49X> candidates()const noexcept{return *this?std::span<const g::Type2StaticCandidate49X>{owner_->candidates_}:std::span<const g::Type2StaticCandidate49X>{};}
  // Bindings stay private: they contain mutable candidate pointers and cannot
  // escape this read-only prefix as a mutation/registration capability.
  const mc_cold_sources50LA::Owner* sourceIdentity()const noexcept{return *this?owner_->source_.get():nullptr;}
 };
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(!thread())std::terminate();}
 bool prefixReady()const noexcept{return thread()&&ready_&&source_&&source_->ready()&&collision_&&collision_->placementsReady()&&meshBorrow_&&bool(*meshBorrow_);}
 static constexpr bool gameplayReady()noexcept{return false;}
 std::size_t retainedPayloadBytes()const noexcept {
  if(!thread())return std::numeric_limits<std::size_t>::max();
  try{std::size_t n=sizeof(Owner);if(meshBorrow_)add(n,sizeof(*meshBorrow_));vectorBytes(n,placed_);vectorBytes(n,candidates_);vectorBytes(n,radii_);vectorBytes(n,bindings_);
   for(const auto& r:radii_)vectorBytes(n,r.values);
   for(const auto& c:candidates_){vectorBytes(n,c.candidate.collision.filter.categoryMasks);for(const auto& list:c.candidate.collision.filter.identityLists)vectorBytes(n,list.identities);}return n;
  }catch(...){return std::numeric_limits<std::size_t>::max();}
 }
 bool prepare(std::shared_ptr<const mc_cold_sources50LA::Owner> source,
   std::shared_ptr<const mc_collision_sources50MC::Owner> collision,const track_static_candidates50IG::Bindings& actualNames,
   std::size_t availableBytes,Diagnostic& d)noexcept {
  d={};if(!thread()||attempted_||weak_from_this().expired()){d.message="MC static prefix requires one shared owner-thread attempt";return false;}attempted_=true;
  try{
   need(source&&source->ready()&&collision&&collision->placementsReady(),"MC complete authentic placements required");
   need(availableBytes&&availableBytes<=32u*1024u*1024u,"MC static prefix bounded allowance required");
   auto borrow=collision->borrow();need(bool(borrow)&&borrow.sourceIdentity()==source.get(),"MC static source/mesh ownership mismatch");
   std::size_t reserve=source->retainedPayloadBytes();add(reserve,collision->retainedPayloadBytes());add(reserve,sizeof(Owner)+sizeof(borrow));
   add(reserve,2*89*(sizeof(PlacementRef)+sizeof(g::Type2StaticCandidate49X)+sizeof(track_static_candidates50IG::Radii)+sizeof(rt::Binding)));
   std::size_t scratch=0;
   for(std::size_t i=0;i<89;++i){const auto* m=borrow.mesh(i);need(m&&borrow.collider(i),"MC complete borrowed placement required");
    need(m->nodes.size()<=(std::numeric_limits<std::size_t>::max()/sizeof(float))/2,"MC radii extent overflow");add(reserve,2*m->nodes.size()*sizeof(float));
    std::size_t cells=m->nodes.size();add(cells,m->faces.size());need(cells<=std::numeric_limits<std::size_t>::max()/sizeof(std::uint32_t),"MC topology extent overflow");scratch=std::max(scratch,cells*sizeof(std::uint32_t));}
   add(reserve,scratch);add(reserve,65536);d.maximumReservation=reserve;need(reserve<=availableBytes,"MC shared source/static prefix reservation exhausted");
   source_=std::move(source);collision_=std::move(collision);meshBorrow_=std::make_unique<mc_collision_sources50MC::Owner::Borrow>(std::move(borrow));placed_.reserve(89);
   for(std::size_t i=0;i<89;++i){const auto& pin=mc_collision_sources50MC::pins[i];placed_.push_back({*meshBorrow_->mesh(i),*meshBorrow_->collider(i),pin.zone,pin.ordinal});}
   track_static_candidates50IG::prepareSpan(placed_.data(),placed_.size(),actualNames,candidates_,radii_,bindings_);
   need(candidates_.size()==89&&radii_.size()==89&&bindings_.size()==89,"MC exact static prefix extent");
   d.retainedBytes=retainedPayloadBytes();std::size_t actual=source_->retainedPayloadBytes();add(actual,collision_->retainedPayloadBytes());add(actual,d.retainedBytes);
   need(actual<=reserve&&actual<=availableBytes,"MC retained static prefix exceeds reservation");ready_=true;return true;
  }catch(const std::exception& e){d.message=e.what();return false;}catch(...){d.message="MC static prefix allocation failure";return false;}
 }
 Borrow borrow()const noexcept {auto self=weak_from_this().lock();return prefixReady()&&self?Borrow{std::move(self)}:Borrow{};}
 bool close()noexcept {if(!thread()||weak_from_this().use_count()>1)return false;ready_=false;bindings_.clear();candidates_.clear();radii_.clear();placed_.clear();meshBorrow_.reset();collision_.reset();source_.reset();attempted_=true;return true;}
};
}
