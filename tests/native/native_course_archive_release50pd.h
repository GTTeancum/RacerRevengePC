#pragma once
// Course track bank (TRACKS/<course>.RES) released when its race ends, as the
// original does (1316A0 removes the race's archive bank on race unload) and as
// RA already does for its own mounts. Without it, an earlier race's bank stays
// active and a later course sees two different members under one name (for
// example particle\demoprt.pst in GA.RES and RA.RES), which 132820 refuses.
//
// One holder per process; one lease per race session. The lease records only
// mounts this session newly committed or reactivated (provider commit); an
// already-active repeat records nothing. Release goes through the resolver's
// existing 1316A0 preflight, which refuses while any borrower of the bank's
// files is alive; a refusal keeps the bank active (logged), exactly as before.
#include "../../src/assets/original_standard_archive_provider50la.h"
#include "../../src/assets/original_standard_texture_resource50la.h"
#include <cstdio>
#include <memory>
#include <new>
#include <thread>

namespace native_course_archive_release50PD {
struct Holder {
    std::unique_ptr<rr::assets::NativeRaceMountLease50LA> lease;
    std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> resolver;
    // Fresh lease for this race session's course mount. Returns null (no lease,
    // mount behaves as before) if an earlier session's lease is still open.
    rr::assets::NativeRaceMountLease50LA* begin(
        const std::shared_ptr<rr::assets::OriginalStandardArchiveProvider50LA>& provider,
        std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> owner)noexcept {
        if(!provider||!owner)return nullptr;
        if(lease&&!lease->closed_&&lease->count_)return nullptr;
        lease.reset(new(std::nothrow) rr::assets::NativeRaceMountLease50LA());if(!lease)return nullptr;
        lease->provider_=provider;lease->thread_=std::this_thread::get_id();
        resolver=std::move(owner);
        return lease.get();
    }
    bool release()noexcept {
        if(!lease||lease->closed_||!lease->count_||!resolver){lease.reset();resolver.reset();return true;}
        rr::assets::OriginalStandardTextureDiagnostic50LA d;
        const auto count=lease->count_;
        const bool released=resolver->retireNativeRaceMounts1316A0(*lease,d);
        std::fprintf(stderr,"course_archive_release released=%d mounts=%zu status=%u pc=%x\n",
            released?1:0,count,unsigned(d.status),unsigned(d.sourcePC));
        if(released){lease.reset();resolver.reset();}
        return released;
    }
};
inline Holder& holder()noexcept {static Holder h;return h;}
}
