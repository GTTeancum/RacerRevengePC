#pragma once
#include "original_standard_texture_resource50la.h"
#include <memory>
#include <array>
#include <vector>
#include <utility>

namespace ra_crowd_image50LA {class Verifier;}
namespace rr::assets {
// One authenticated host-image lease, separate from canonical texture demand,
// compressed source storage, child registration and GPU publication. This does
// not construct resources. A complete set and actual context are additionally
// required by canonical CPU adoption. Only the private cold verifier can
// mint six fixed-pin RA frame/mip leases. No generic hash callback/setter.
class OriginalStandardIpumImageLease50LA final {
    friend class ::ra_crowd_image50LA::Verifier;
    friend class OriginalStandardTextureResource50LA;
    friend class OriginalStandardTextureResolver50LA;
    std::shared_ptr<const OriginalStandardIpumCompressedFrames50LA> compressed_;
    OriginalStandardIpumCompressedFrames50LA::FrameView frame_{};
    std::vector<std::uint8_t> rrtex_;
    OriginalStandardIpumImageLease50LA()=default;
public:
    OriginalStandardIpumImageLease50LA(const OriginalStandardIpumImageLease50LA&)=delete;
    OriginalStandardIpumImageLease50LA& operator=(const OriginalStandardIpumImageLease50LA&)=delete;
    OriginalStandardIpumImageLease50LA(OriginalStandardIpumImageLease50LA&&)=delete;
    OriginalStandardIpumImageLease50LA& operator=(OriginalStandardIpumImageLease50LA&&)=delete;
    ~OriginalStandardIpumImageLease50LA()=default;
    const OriginalStandardTextureResolver50LA* resolverIdentity()const noexcept{return compressed_->resolverIdentity();}
    std::size_t sourceIndex()const noexcept{return compressed_->sourceIndex();}
    const OriginalStandardIpumCompressedFrames50LA* compressedIdentity()const noexcept{return compressed_.get();}
    const OriginalStandardIpumCompressedFrames50LA::FrameView& sourceFrame()const noexcept{return frame_;}
    // Pointer remains valid through this immutable lease. Native host RGBA is
    // NOT the original resource's encoded1555/IPUM storage or level+18 pointer.
    const std::uint8_t* rgba()const noexcept{return rrtex_.data()+16;}
    std::size_t rgbaBytes()const noexcept{return rrtex_.size()-16;}
    const std::vector<std::uint8_t>& authenticatedRrtexBytes()const noexcept{return rrtex_;}
};

// Complete immutable six-frame admission; identical pixels retain distinct
// frame/lease objects. This object alone does not construct any resource.
class OriginalStandardIpumImageSet50LA final {
    friend class ::ra_crowd_image50LA::Verifier;
    std::array<std::shared_ptr<const OriginalStandardIpumImageLease50LA>,6> frames_{};
    OriginalStandardIpumImageSet50LA()=default;
public:
    OriginalStandardIpumImageSet50LA(const OriginalStandardIpumImageSet50LA&)=delete;
    OriginalStandardIpumImageSet50LA& operator=(const OriginalStandardIpumImageSet50LA&)=delete;
    const std::shared_ptr<const OriginalStandardIpumImageLease50LA>& frame(std::size_t i)const noexcept {
        static const std::shared_ptr<const OriginalStandardIpumImageLease50LA> absent;
        return i<frames_.size()?frames_[i]:absent;
    }
    std::size_t count()const noexcept{return frames_.size();}
};
}
