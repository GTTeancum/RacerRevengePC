// One implementation of the existing GB/MC cold-source authentication and
// payload arithmetic. Existing typed owners keep every allocation and phase.
#include "track_cold_source_services50la.h"
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/indexed_resource48c.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/original_standard_archive_provider50la.h"
#include <limits>
namespace track_cold_source_services50LA {
namespace a=rr::assets;
void add(std::size_t& n,std::size_t x)noexcept {
  n=x>std::numeric_limits<std::size_t>::max()-n?std::numeric_limits<std::size_t>::max():n+x;
 }
void vectorPayload(std::size_t& n,std::size_t capacity,std::size_t elementBytes)noexcept {
  if(capacity>std::numeric_limits<std::size_t>::max()/elementBytes)n=std::numeric_limits<std::size_t>::max();
  else add(n,capacity*elementBytes);
 }
void textPayload(std::size_t& n,const std::string& s)noexcept{add(n,s.capacity());add(n,1);}
void parametersPayload(std::size_t& n,const a::ParameterDocument48F& p)noexcept {
  vectorPayload(n,p.original_bytes.capacity(),sizeof(std::uint8_t));vectorPayload(n,p.blocks.capacity(),sizeof(a::ParameterBlock48F));
  for(const auto& b:p.blocks){textPayload(n,b.name);vectorPayload(n,b.fields.capacity(),sizeof(a::ParameterField48F));
   for(const auto& f:b.fields){textPayload(n,f.name);textPayload(n,f.value);}}
 }
bool authenticateMember(const a::IndexedResArchive48C& archive,
    const MemberPinView& pin,std::vector<std::uint8_t>& out,a::DecodeError48C& decode) {
  const a::ResourceEntry48C* match=nullptr;
  for(const auto& e:archive.entries())if(a::OriginalStandardArchiveProvider50LA::sameName(e.name,pin.name)){
   if(match)return false;match=&e;}
  return match&&match->logical_offset==pin.offset&&match->bytes==pin.bytes&&
   archive.extractMember(pin.name,out,decode,pin.bytes)&&out.size()==pin.bytes&&
   pod_materials50ed_fixture::detail::sha256(out)==pin.sha;
 }
}
