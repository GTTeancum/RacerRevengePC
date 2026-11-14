#pragma once
// One rule for every course archive (TRACKS/xx.RES), every loader.
// The original reads whatever course archive the disc holds: no checksum, byte
// size or member count gates a course (SLUS course load reads the RES and its
// named members only). The shipped stock identity is therefore a RECEIPT here,
// never an admission gate: each site that used to refuse a differing archive
// logs one line "course_archive_identity site=.. code=.. bytes=.. stock=1|0"
// and continues with the bytes it actually read. Every later write is still
// bounded by the native storage it lands in (the decode limits below and each
// owner's own capacities), so a larger archive is refused by capacity, never
// by identity. This is the ONE place that defines the rule and the limits.
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/import_decode48c.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace course_archive_receipt50CI {
// Shared decode capacity for any course archive (DecodeLimits48C defaults:
// 64 MiB file, 128 MiB decoded, 10000 entries). Used wherever a loader had
// set its limits to the stock sizes.
inline rr::assets::DecodeLimits48C limits() noexcept { return {}; }
inline std::size_t maximumArchiveBytes() noexcept { return limits().resource_file_bytes; }
// Receipt only. Returns true when the bytes are the shipped stock archive.
inline bool receipt(const char* site,const char* code,const std::vector<std::uint8_t>& bytes,
    const char* stockSha,std::size_t stockBytes) {
    const bool stock=stockSha&&bytes.size()==stockBytes&&
        pod_materials50ed_fixture::detail::sha256(bytes)==stockSha;
    std::cerr<<"course_archive_identity site="<<(site?site:"?")<<" code="<<(code?code:"?")
             <<" bytes="<<bytes.size()<<" stock="<<(stock?1:0)<<'\n';
    return stock;
}
// Receipt for one named member whose stock bytes were pinned (offset, size,
// SHA). The member is found by name; the pin is reported, not enforced.
inline bool memberReceipt(const char* site,const std::string& name,const std::vector<std::uint8_t>& bytes,
    std::size_t stockOffset,std::size_t actualOffset,std::size_t stockBytes,const char* stockSha) {
    const bool stock=stockSha&&bytes.size()==stockBytes&&actualOffset==stockOffset&&
        pod_materials50ed_fixture::detail::sha256(bytes)==stockSha;
    if(!stock)std::cerr<<"course_member_identity site="<<(site?site:"?")<<" member="<<name
                       <<" bytes="<<bytes.size()<<" stock=0\n";
    return stock;
}
// A stock count kept as a receipt: logs only when the data differs.
inline void countReceipt(const char* site,const char* what,std::size_t actual,std::size_t stock) {
    if(actual!=stock)std::cerr<<"course_count site="<<(site?site:"?")<<" "<<(what?what:"?")
                               <<"="<<actual<<" stock="<<stock<<'\n';
}
}
