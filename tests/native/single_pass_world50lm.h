#pragma once
// Common rewrite (R3, PLAN 2.2 "instances, standard models, materials"): the ONE static
// world owner of a race, for every course - the single instantiation of
// original_single_pass_world50lm.inl. Replaces ma_original_world50LM and
// common_route_world50LM (ma_sa_original_world50lm.h) and the per-course
// *_original_world50l{e,h,m} owners. 274080 instances / standard models and the
// single-pass material decoder (274160, 14D370/29E340; R7 reads its results).
//
// There is no stock row here: every course - stock or registry - is admitted from its own
// data (identity built from the Track). The stock pins and counts the per-course owners
// carried are receipts (stock_course_receipts50cr.h), logged by the Track, never gates.
#include "track_materials50fa_fixture.h"
#include "original_world_material_admission50cm.h"
#include "material_alpha50gx_fixture.h"
#include "source_texture50fc_fixture.h"
#include "course_archive_receipt50ci.h"
#include "mc_world_semantics50mc.h"   // tolerant SCB reader (duplicate unused labels), shared by the Track

#include "../../src/assets/import_world48f.h"
#include "../../src/assets/original_ipu_decoder.h"
#include "../../src/assets/original_standard_ipum_framing50la.h"
#include "../../src/assets/original_track_cut_identity50la.h"
#include "../../src/assets/original_ui_atm50kg.h"
#include "../../src/assets/world_normals50hk.h"
#include <algorithm>
#include <array>
#include <charconv>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <functional>
#include <locale>
#include <sstream>
#include <cmath>

namespace single_pass_world50LM {
namespace a=rr::assets;
namespace shared=pod_materials50ed_fixture::detail;
namespace tm=track_materials50fa_fixture;
using Bytes=std::vector<std::uint8_t>;
using Json=nlohmann::json;
// No stock row (Identity is always built from the Track; see identityForTrack in
// common_track_load50cr.cpp). Empty defaults keep the shared .inl's Identity shape.
inline constexpr const char* archiveSha="";
inline constexpr const char* scbMember="";
inline constexpr const char* archiveMember="";
inline constexpr std::uint32_t archiveBytes=0u;
inline constexpr std::uint64_t assetIdentity=0ull;
inline constexpr const char* stockCode="";
namespace detail {
struct SourceImage { const char* member;const char* sourceSha256;std::uint32_t sourceBytes,width,height,mipIndex,kind,frameCount,initialFrame; };
// No per-course pin tables: every image is decoded and admitted from its own data.
inline constexpr std::array<SourceImage,0> sourceImages{};
inline constexpr std::array<source_texture50fc_fixture::detail::Entry,0> indexedPins{};
inline constexpr std::array<source_texture50fc_fixture::detail::Entry,0> exceptionalImages{};
// ProjectedPrelit TexGenMatrixRow1..4 of an authored pass (moved here from the retired
// track_materials50fa_fixture::detail with the legacy loader; R7).
inline std::array<double,16> projection(const Json& fields){
    std::array<double,16> matrix{};
    for(unsigned row=0;row<4;++row){
        const std::string name="TexGenMatrixRow"+std::to_string(row+1);
        const Json* found=nullptr;
        for(const auto& field:fields.at("authored"))if(field.at("name")==name){
            shared::need(!found,"Duplicate authored texture matrix row");found=&field;
        }
        shared::need(found,"Missing authored texture matrix row");
        std::istringstream values(found->at("value").get<std::string>());
        values.imbue(std::locale::classic());
        for(unsigned col=0;col<4;++col){
            values>>matrix[row*4+col];
            shared::need(bool(values)&&std::isfinite(matrix[row*4+col]),"Invalid authored texture matrix scalar");
            if(col!=3){char comma=0;values>>comma;shared::need(comma==',',"Authored texture matrix separator");}
        }
        values>>std::ws;shared::need(values.eof(),"Authored texture matrix suffix");
    }
    shared::need(matrix[3]==0&&matrix[7]==0&&matrix[11]==0&&matrix[15]==1,"Authored projected texture matrix must be affine");
    return matrix;
}
}
#include "original_single_pass_world50lm.inl"
} // namespace single_pass_world50LM
