#pragma once
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/original_standard_archive_provider50la.h"
#include "../../src/assets/original_standard_texture_resource50la.h"
#include <array>
#include <memory>
#include <vector>
#include <utility>

namespace ra_tga_source50LA {
// Cold source admission only. The actual resolver must already retain this
// same mounted provider. No hash callback, arbitrary digest or image input.
class Verifier final {
public:
    static bool append(rr::assets::OriginalStandardTextureResolver50LA& resolver,
        const std::shared_ptr<const rr::assets::OriginalStandardArchiveProvider50LA>& provider,
        rr::assets::OriginalStandardTextureDiagnostic50LA& d) noexcept {
        namespace a=rr::assets;
        d={};
        const auto reject=[&](a::OriginalStandardTextureStatus50LA status){
            d.status=status;d.sourcePC=0x132820;return false;};
        if(!provider||!provider->sealed())return reject(a::OriginalStandardTextureStatus50LA::source);
        struct Pin {const char* member;std::size_t bytes;const char* hash;};
        static constexpr std::array<Pin,3> pins={{
            {"textures\\LargeFlag.tga",65580,"352286ac266f59e3d06531cfab7a67ce8ad9e7355fe544ba932550278abd613d"},
            {"textures\\MetalGrate.tga",196652,"20cd5335e1d870295983807467defecbf2c45dd1ee1ceeaa10c8bfd2e8e85fa9"},
            {"textures\\TwiDesigns02.tga",49196,"693031b2ed08b5437c250ea875eb9e8f6f0444b858530d7a0e0c1737630aba15"}
        }};
        try {
            const auto revision=provider->revision();
            std::vector<a::OriginalStandardTextureSourceInput50LA> inputs;inputs.reserve(pins.size());
            for(std::size_t i=0;i<pins.size();++i){
                d.source=i;const auto& pin=pins[i];
                a::OriginalStandardArchiveProvider50LA::MemberView member;
                a::OriginalStandardArchiveProvider50LA::MountView mount;
                if(provider->lookup132820(pin.member,member)!=a::OriginalStandardArchiveProvider50LA::Lookup::present||
                   !provider->mount(member.mount,mount)||!member.data||member.size!=pin.bytes||
                   !mount.archiveSha256||*mount.archiveSha256!=
                     "1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2"||
                   mount.archiveBytes!=4604561||mount.memberCount!=1010)
                    return reject(a::OriginalStandardTextureStatus50LA::source);
                a::OriginalStandardTextureSourceInput50LA input;
                input.member=pin.member;input.bytes.assign(member.data,member.data+member.size);
                if(pod_materials50ed_fixture::detail::sha256(input.bytes)!=pin.hash)
                    return reject(a::OriginalStandardTextureStatus50LA::source);
                inputs.push_back(std::move(input));
            }
            if(!provider->sealed()||provider->revision()!=revision)
                return reject(a::OriginalStandardTextureStatus50LA::phase);
            // Private join rechecks the actual provider identity/bytes before
            // the existing atomic append transaction. No resource is created.
            return resolver.appendRaTgaVerified(provider,inputs,d);
        }catch(...){return reject(a::OriginalStandardTextureStatus50LA::allocation);}
    }
};
}
