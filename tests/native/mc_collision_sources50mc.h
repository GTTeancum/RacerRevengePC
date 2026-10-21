#pragma once
#include "mc_cold_sources50la.h"
#include "../../src/assets/collision_mesh48i.h"
#include "../../src/assets/track_collider50ao.h"
#include <array>
#include <span>
#include <stdexcept>

// Source mesh/placement groundwork, NOT an aggregate, live zone arena or query
// issuer. The four missing UpVectors come only from the proven MC World owner.
// Collision masks must be supplied from the actual application registry.
namespace mc_collision_sources50MC {
namespace a=rr::assets;
struct Pin {
 const char* section;std::size_t blockOffset;std::uint32_t zone,ordinal;
 const char* member;std::size_t archiveOffset,bytes;const char* sha;
 bool hasUp,hasLighting;
};
inline constexpr std::array<Pin,89> pins{{
#include "mc_collision_source_index.inc"
}};
enum class Phase {Empty,Source,Meshes,MeshesReady,Closed};
struct Diagnostic {Phase phase=Phase::Empty;std::size_t placement=0,retainedBytes=0,maximumReservation=0;std::string message;a::DecodeError48C decode{};};
class Owner final : public std::enable_shared_from_this<Owner> {
 std::shared_ptr<const mc_cold_sources50LA::Owner> source_;
 std::unique_ptr<std::array<a::CollisionMesh48I,89>> meshes_;
 const std::thread::id thread_=std::this_thread::get_id();
 std::array<a::TrackCollider50AO,89> placements_{};
 bool attempted_=false,placementAttempted_=false,placementsReady_=false;Phase phase_=Phase::Empty;
 static void need(bool v,const char* why){if(!v)throw std::runtime_error(why);}
 static void add(std::size_t& n,std::size_t v){need(v<=std::numeric_limits<std::size_t>::max()-n,"MC mesh payload overflow");n+=v;}
 template<class T> static void bytes(std::size_t& n,const std::vector<T>& v){need(v.capacity()<=std::numeric_limits<std::size_t>::max()/sizeof(T),"MC mesh capacity overflow");add(n,v.capacity()*sizeof(T));}
 static const a::ParameterField48F* field(const a::ParameterBlock48F& b,const char* key){
  const a::ParameterField48F* result=nullptr;for(const auto& f:b.fields)if(f.name==key){need(!result,"MC duplicate collision placement field");result=&f;}return result;
 }
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
public:
 class Borrow final {
  friend class Owner;std::shared_ptr<const Owner> owner_;
  explicit Borrow(std::shared_ptr<const Owner> owner):owner_(std::move(owner)){}
 public:
  Borrow()=default;Borrow(const Borrow&)=default;Borrow(Borrow&&)=default;
  Borrow&operator=(const Borrow&)=delete;Borrow&operator=(Borrow&&)=delete;
  ~Borrow(){if(owner_&&!owner_->thread())std::terminate();}
  explicit operator bool()const noexcept{return owner_&&owner_->meshesReady();}
  const a::CollisionMesh48I* mesh(std::size_t i)const noexcept{return *this&&i<pins.size()?&(*owner_->meshes_)[i]:nullptr;}
  const a::TrackCollider50AO* collider(std::size_t i)const noexcept{return *this&&owner_->placementsReady()&&i<pins.size()?&owner_->placements_[i]:nullptr;}
  const mc_cold_sources50LA::Owner* sourceIdentity()const noexcept{return *this?owner_->source_.get():nullptr;}
 };
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(!thread())std::terminate();}
 bool meshesReady()const noexcept{return thread()&&phase_==Phase::MeshesReady&&source_&&source_->documentsReady()&&meshes_;}
 static constexpr bool gameplayReady()noexcept{return false;}
 std::size_t retainedPayloadBytes()const noexcept {
  if(!thread())return std::numeric_limits<std::size_t>::max();
  try{std::size_t n=sizeof(Owner);if(meshes_){add(n,sizeof(*meshes_));for(const auto& m:*meshes_){bytes(n,m.vertices);bytes(n,m.vertex_normals);bytes(n,m.nodes);bytes(n,m.faces);}}return n;}
  catch(...){return std::numeric_limits<std::size_t>::max();}
 }
 bool prepare(std::shared_ptr<const mc_cold_sources50LA::Owner> source,std::size_t availableBytes,Diagnostic& d)noexcept {
  d={};if(!thread()){d.message="MC mesh owner thread mismatch";return false;}
  if(attempted_){d.message="MC mesh owner is one-attempt";return false;}attempted_=true;phase_=Phase::Source;
  try{
   need(source&&source->documentsReady()&&!weak_from_this().expired(),"MC meshes require shared authentic source/owner");
   need(availableBytes&&availableBytes<=32u*1024u*1024u,"MC mesh preparation requires bounded existing allowance");
   source_=std::move(source);
   auto reserve=[&](std::size_t extra){auto n=source_->retainedPayloadBytes();add(n,retainedPayloadBytes());add(n,extra);d.maximumReservation=std::max(n,d.maximumReservation);need(n<=availableBytes,"MC source plus mesh reservation exceeds allowance");};
   reserve(sizeof(std::array<a::CollisionMesh48I,89>));meshes_=std::make_unique<std::array<a::CollisionMesh48I,89>>();phase_=Phase::Meshes;
   for(std::size_t i=0;i<pins.size();++i){d.placement=i;const auto& p=pins[i];
    const a::ParameterBlock48F* b=nullptr;for(const auto& block:source_->scb().blocks)if(block.name==p.section){need(!b,"MC duplicate placement section");b=&block;}
    need(b&&b->offset==p.blockOffset&&bool(field(*b,"upvector"))==p.hasUp&&bool(field(*b,"uselighting"))==p.hasLighting,"MC original collision placement provenance differs");
    need(field(*b,"position")&&field(*b,"lookvector")&&field(*b,"slt"),"MC collision placement source incomplete");
    const a::ResourceEntry48C* member=nullptr;for(const auto& e:source_->archive().entries())if(a::OriginalStandardArchiveProvider50LA::sameName(e.name,p.member)){need(!member,"MC duplicate collision member");member=&e;}
    need(member&&member->logical_offset==p.archiveOffset&&member->bytes==p.bytes,"MC collision member provenance differs");
    // Conservative preallocation bound for original, decoded tree, cache,
    // projection and decoder temporaries. Parent must corroborate actual peak.
    need(p.bytes<=(std::numeric_limits<std::size_t>::max()-65536)/32,"MC scratch overflow");reserve(32*p.bytes+65536);
    std::vector<std::uint8_t> raw,cache;a::Collision48G decoded;std::string why;
    need(source_->extractMember(p.member,raw,d.decode,p.bytes)&&raw.size()==p.bytes&&pod_materials50ed_fixture::detail::sha256(raw)==p.sha,"MC authentic collision member rejected");
    need(a::decodeCollision48G(raw,decoded,d.decode)&&decoded.type==0,"MC static COL0 required");
    need(a::encodeCollisionAsset48G(decoded,cache,why)&&a::loadCollisionMesh48I(cache,(*meshes_)[i],why),"MC native collision projection rejected");
   }
   reserve(0);d.retainedBytes=retainedPayloadBytes();phase_=Phase::MeshesReady;d.phase=phase_;return true;
  }catch(const std::exception& e){d.phase=phase_;d.message=e.what();return false;}
   catch(...){d.phase=phase_;d.message="MC mesh preparation allocation failure";return false;}
 }
 bool preparePlacements(a::TrackCollisionMasks50AN actualMasks,Diagnostic& d)noexcept {
  d={};if(!meshesReady()||placementAttempted_||weak_from_this().use_count()>1){d.message="MC placements require one unborrowed mesh owner";return false;}
  placementAttempted_=true;
  try{
   need(source_->ready()&&source_->world().instances.size()==458,"MC complete source-backed World required");
   std::size_t placed=0;
   for(const auto& instance:source_->world().instances){
    const a::ResourceEntry48C* present=nullptr;
    for(const auto& e:source_->archive().entries())if(a::OriginalStandardArchiveProvider50LA::sameName(e.name,instance.collision_member))present=&e;
    if(!present)continue;
    need(placed<pins.size(),"MC collision placement extent overflow");const auto& p=pins[placed];d.placement=placed;
    need(instance.section==p.section&&instance.zone==p.zone&&instance.ordinal==p.ordinal&&instance.source_block_offset==p.blockOffset&&
      a::OriginalStandardArchiveProvider50LA::sameName(instance.collision_member,p.member),"MC complete ordered placement identity differs");
    std::array<float,3> position{},look{},up{};
    for(unsigned k=0;k<3;++k){position[k]=static_cast<float>(instance.position[k]);look[k]=static_cast<float>(instance.look[k]);up[k]=static_cast<float>(instance.up[k]);}
    need(a::prepareTrackCollider50AO((*meshes_)[placed],position,look,up,p.member,std::strlen(p.member),actualMasks,placements_[placed]),"MC source collision placement rejected");
    ++placed;
   }
   need(placed==89,"MC all89 collision-only placements required");placementsReady_=true;d.phase=phase_;return true;
  }catch(const std::exception& e){d.phase=phase_;d.message=e.what();return false;}
   catch(...){d.phase=phase_;d.message="MC collision placement failure";return false;}
 }
 bool placementsReady()const noexcept{return meshesReady()&&placementsReady_;}
 Borrow borrow()const noexcept {auto self=weak_from_this().lock();return meshesReady()&&self?Borrow{std::move(self)}:Borrow{};}
 bool close()noexcept {if(!thread()||weak_from_this().use_count()>1)return false;meshes_.reset();source_.reset();placementsReady_=false;attempted_=true;phase_=Phase::Closed;return true;}
};
}
