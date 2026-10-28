#pragma once
// Shared native cold-source mechanics only. No world/track owner, lifecycle,
// semantic decoder, or original-game load-order policy is defined here.
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
namespace rr::assets {
class IndexedResArchive48C;
struct DecodeError48C;
struct ParameterDocument48F;
}
namespace track_cold_source_services50LA {
// Borrowed identity for one synchronous call. No input, output or archive
// ownership is acquired; the caller retains its existing phase/error policy.
struct MemberPinView {
 const char* name;
 std::size_t offset,bytes;
 const char* sha;
};
bool authenticateMember(const rr::assets::IndexedResArchive48C& archive,
    const MemberPinView& pin,std::vector<std::uint8_t>& out,
    rr::assets::DecodeError48C& decode);
// Saturating native capacity accounting, not original-game resource limits.
void add(std::size_t& n,std::size_t x)noexcept;
// elementBytes is the caller's unchanged sizeof(T), hence nonzero.
void vectorPayload(std::size_t& n,std::size_t capacity,std::size_t elementBytes)noexcept;
void textPayload(std::size_t& n,const std::string& s)noexcept;
void parametersPayload(std::size_t& n,const rr::assets::ParameterDocument48F& p)noexcept;
}
