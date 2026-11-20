#include "original_camera_start_effects.h"
// Compile the private immutable cut authoring foundation; no live caller or admission.
#include "../assets/original_ta_cut_authoring_candidate.h"
// Compile the private TA child foundation only; no live caller or admission.
#include "original_ta_cut_child_candidate.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>
#include "../assets/import_world48f.h"
namespace rr::game {
bool OriginalCourseStartActions::prepare(std::string_view code,const std::uint8_t* bytes,std::size_t size) noexcept {
    if(authenticated_||!bytes||!size||code.size()!=2u)return false;
    // Common course rule (174DF0 / 1B28EC): the cut count is the course TOB's
    // own [CSCameras].NumberOfCameras, what 174E58 reads from World+3E4, for
    // every course (stock or registry). The former stock literal table
    // (TA/SC/TC 3, GC/MA/GB/MC 4, GA/SA/RC/TB/MB 5, RA 7) is a receipt only:
    // the camera factory logs common_track_data role=cuts against it.
    try {
        // Stock TOBs carry duplicate labels (TB, TC), so the decoder retains
        // them; the one field used here must be unique.
        assets::ParameterDocument48F document;assets::DecodeError48C error;
        assets::WorldLimits48F limits;limits.retainDuplicateLabels=true;
        limits.input_bytes=std::max<std::size_t>(limits.input_bytes,size);
        const std::vector<std::uint8_t> tob(bytes,bytes+size);
        if(!assets::decodeParameters48F(tob,document,error,nullptr,limits))return false;
        const assets::ParameterBlock48F* cameras=nullptr;
        for(const auto& b:document.blocks){
            // PORT STAND-IN (flagged, data-keyed): the native start has no
            // 321_anims animation path; a TOB naming one is refused.
            if(b.name=="321_anims")return false;
            if(b.name=="cscameras"){if(cameras)return false;cameras=&b;}
        }
        if(!cameras)return false;
        const std::string* text=nullptr;
        for(const auto& f:cameras->fields)if(f.name=="numberofcameras"){if(text)return false;text=&f.value;}
        if(!text||text->empty()||text->size()>2)return false;
        unsigned count=0;for(const char c:*text){if(c<'0'||c>'9')return false;count=count*10u+unsigned(c-'0');}
        // Native cut storage holds 7 (OriginalTaCutChildrenCandidate); zero is
        // the original zero-cut controller (174F9C skips 174FA4).
        if(count>7u)return false;
        sourceSceneCutCount3E4_=static_cast<std::int32_t>(count);
    }catch(...){return false;}
    code_={code[0],code[1],'\0'};authenticated_=true;return true;
}
}
