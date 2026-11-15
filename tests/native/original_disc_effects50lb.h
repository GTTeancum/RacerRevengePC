#pragma once
#include "../../src/assets/dlc_files50dl.h"
#include "runtime_asset_paths50lb.h"
#include "../../src/assets/import_decode48c.h"
#include <array>
#include <fstream>
#include <stdexcept>
#include <vector>

// Cold, owned original data. No extracted evidence directory or generated
// metadata is needed. Callers still authenticate every EMT's exact SHA256
// before constructing native effect owners.
namespace original_disc_effects50LB {
inline std::array<std::vector<std::uint8_t>,6> load() {
    const auto path=rr::assets::dlc50DL::host(runtime_asset_paths50LB::resolve("work/native/project/disc/PODS/POD01/POD01.RES"));
    std::ifstream file(path,std::ios::binary|std::ios::ate);
    if(!file)throw std::runtime_error("original POD01 effect archive unavailable");
    const auto size=file.tellg();
    if(size<=0||size>16*1024*1024)throw std::runtime_error("original POD01 effect archive exceeds bound");
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
    file.seekg(0);
    if(!file.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size())))
        throw std::runtime_error("original POD01 effect archive truncated");
    rr::assets::ResArchive48C archive;rr::assets::DecodeError48C error;
    if(!rr::assets::decodeResource48C(bytes,archive,error))
        throw std::runtime_error("original POD01 effect archive framing rejected");
    constexpr const char* names[]={"particle\\podLeftEngineDust.emt","particle\\podRightEngineDust.emt",
        "particle\\podLeftEngineSmoke.emt","particle\\podRightEngineSmoke.emt",
        "particle\\podSparks.emt","particle\\podFire.emt"};
    std::array<std::vector<std::uint8_t>,6> result;
    for(unsigned i=0;i<result.size();++i)
        if(!rr::assets::extractResMember48C(archive,names[i],result[i],error)||
           result[i].empty()||result[i].size()>65536)
            throw std::runtime_error("original shared effect member unavailable or exceeds bound");
    return result;
}
} // namespace original_disc_effects50LB
