#pragma once
#include "../../src/assets/audio_assets50hc.h"
#include <cstdint>
#include <string>

namespace audio_retained_payload50MC {
// Actual retained container payload only, excluding allocator bookkeeping and
// shared owners. sizeof enclosing objects already accounts for inline strings.
inline std::size_t externalStringBytes(const std::string& value)noexcept{
 const auto data=reinterpret_cast<std::uintptr_t>(value.data());
 const auto object=reinterpret_cast<std::uintptr_t>(&value);
 return data>=object&&data-object<sizeof(value)?0:value.capacity()+1;
}
inline std::size_t bankDynamicBytes(const rr::assets::AudioBank50HC& bank)noexcept{
 std::size_t bytes=bank.clips.capacity()*sizeof(rr::assets::AudioClip50HC);
 for(const auto& clip:bank.clips){
  bytes+=clip.pcm.samples.capacity()*sizeof(std::int16_t);
  bytes+=externalStringBytes(clip.request.name)+externalStringBytes(clip.pcm.sourceSha256)+
      externalStringBytes(clip.sourcePath)+externalStringBytes(clip.sourceSha256)+externalStringBytes(clip.detail);
 }
 return bytes;
}
inline std::size_t bankPcmCapacityBytes(const rr::assets::AudioBank50HC& bank)noexcept{
 std::size_t bytes=0;for(const auto& clip:bank.clips)bytes+=clip.pcm.samples.capacity()*sizeof(std::int16_t);return bytes;
}
}
