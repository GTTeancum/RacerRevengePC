#pragma once
// Cold RA-only bridge. No resource admission, rendering, or source execution.
#include "assets/import_texture48e.h"
#include "assets/original_tga_conversion50la.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace ra_tga_import {
using Bytes=std::vector<std::uint8_t>;
using Conversion=rr::assets::detail::OriginalTgaConversion50LA;
struct Pin {
    const char* member;
    std::size_t bytes;
    const char* sha256;
    std::uint32_t dimension;
    std::uint8_t depth;
};
inline constexpr std::array<Pin,3> pins={{
    {"textures\\LargeFlag.tga",65580,
     "352286ac266f59e3d06531cfab7a67ce8ad9e7355fe544ba932550278abd613d",128,32},
    {"textures\\MetalGrate.tga",196652,
     "20cd5335e1d870295983807467defecbf2c45dd1ee1ceeaa10c8bfd2e8e85fa9",256,24},
    {"textures\\TwiDesigns02.tga",49196,
     "693031b2ed08b5437c250ea875eb9e8f6f0444b858530d7a0e0c1737630aba15",128,24}
}};
inline void require(bool ok,const std::string& why) {
    if(!ok)throw std::runtime_error("Authenticated RA TGA: "+why);
}
inline const Pin* find(const std::string& member) noexcept {
    for(const auto& pin:pins)if(member==pin.member)return &pin;
    return nullptr;
}
inline void requireSource(const Pin& pin,const Bytes& bytes,const std::string& sha256) {
    require(bytes.size()==pin.bytes&&sha256==pin.sha256,
        std::string("source identity mismatch: ")+pin.member);
    require(Conversion::sourceShape(bytes,pin.dimension,pin.dimension,pin.depth),
        std::string("source shape mismatch: ")+pin.member);
}
// Called only after the existing RA geometry authentication and before mkdir.
// Independently pins container and all three required exact member spellings;
// a missing, duplicate or mismatched required member never becomes retention.
template<class Hash>
inline void authenticate(const Bytes& raw,const rr::assets::ResArchive48C& archive,Hash hash) {
    require(raw.size()==4604561&&hash(raw)==
        "1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2",
        "container identity mismatch");
    require(archive.entries.size()==1010,"member count mismatch");
    for(const auto& pin:pins){
        std::size_t count=0;
        for(const auto& entry:archive.entries)if(entry.name==pin.member){
            ++count;require(entry.bytes==pin.bytes,"member directory extent mismatch");
        }
        require(count==1,std::string("required member missing or repeated: ")+pin.member);
        Bytes bytes;rr::assets::DecodeError48C error;
        require(rr::assets::extractResMember48C(archive,pin.member,bytes,error),
            std::string("source extraction failed: ")+pin.member+" "+error.code);
        requireSource(pin,bytes,hash(bytes));
    }
}
inline std::vector<rr::assets::TextureMip48E> convert(
    const Pin& pin,const Bytes& source,const std::string& sha256) {
    // Reject a foreign caller-supplied descriptor as well as altered bytes.
    require(find(pin.member)==&pin,"unrecognized source pin");
    requireSource(pin,source,sha256);
    std::vector<rr::assets::TextureMip48E> descending;
    Bytes scratch,rgba;
    std::vector<std::uint16_t> packed;
    auto width=pin.dimension;
    if(pin.depth==24){
        require(Conversion::read24_29EA90(source,width,width,scratch),"24-bit reader bounds");
        packed.resize(std::size_t(width)*width);
        require(Conversion::convert1555_16C0D0(scratch,width,width,packed),"1555 conversion bounds");
    }else{
        require(Conversion::read32_29EE40(source,width,width,rgba),"32-bit reader bounds");
    }
    for(;;){
        rr::assets::TextureMip48E mip;mip.width=mip.height=width;
        mip.source_header_offset=width==pin.dimension?0:std::size_t(-1);
        mip.source_indices_offset=width==pin.dimension?18:std::size_t(-1);
        if(pin.depth==24){
            mip.rgba.resize(std::size_t(width)*width*4);
            for(std::size_t i=0;i<packed.size();++i){
                const auto word=packed[i];
                // GS RGB5 expands into the high5 bits, never replication.
                // RA MetalGrate/TwiDesigns02 PSMs inherit DefaultMT's
                // TA0=TA1=255,AEM=0 (24A8FC..24A904), without overrides.
                // TGA clears F8 bit0, so249818..24983C leaves TEXA intact.
                // Thus coverage0 also samples255, including a discarded zero word.
                // Keep bit15 in packed until AFTER all original mip filtering.
                mip.rgba[4*i]=static_cast<std::uint8_t>((word&31u)<<3);
                mip.rgba[4*i+1]=static_cast<std::uint8_t>(((word>>5)&31u)<<3);
                mip.rgba[4*i+2]=static_cast<std::uint8_t>(((word>>10)&31u)<<3);
                mip.rgba[4*i+3]=255;
            }
        }else mip.rgba=rgba; // Full original8888 alpha, no normalization.
        descending.push_back(std::move(mip));
        if(width==16)break;
        const auto nextWidth=width/2;
        if(pin.depth==24){
            std::vector<std::uint16_t> next(std::size_t(nextWidth)*nextWidth);
            require(Conversion::mip1555_168980(packed,width,width,next),"1555 mip bounds");
            packed=std::move(next);
        }else{
            Bytes next(std::size_t(nextWidth)*nextWidth*4);
            // Shared168470 preserves the bottom-right/bottom-left alpha quirk.
            require(Conversion::mip8888_168470(rgba,width,width,next),"8888 mip bounds");
            rgba=std::move(next);
        }
        width=nextWidth;
    }
    std::reverse(descending.begin(),descending.end());
    return descending; // Ascending16..base; never a fabricated PST kind.
}
}
