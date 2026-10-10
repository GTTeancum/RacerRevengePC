#include "original_ui_main_particle_lighting50kg.h"
#include <cfenv>
#include <cmath>
#include <limits>

namespace rr::game {
namespace {
float sourceDiv(float a,float b) noexcept {volatile float r=a/b;return r;}
float sourceMul(float a,float b) noexcept {volatile float r=a*b;return r;}
}
OriginalUiMainParticleLightingStatus50KG prepareOriginalUiMainParticleLighting50KG(
    const OriginalUiMainSceneOwner50KG& scene,
    OriginalUiMainParticleLighting50KG& output) noexcept {
    using S=OriginalUiMainParticleLightingStatus50KG;
    const auto* definition=scene.definition();
    if(!definition)return S::unprepared;
    // This bounded leaf relies on the exact authenticated Main scene. A
    // nonzero directional color needs the full original four-normal path.
    for(unsigned lane=0;lane<3;++lane)
        if(definition->ambientColor[lane]!=50.f||definition->directionalColor[lane]!=0.f)
            return S::unsupportedScene;
    if(!std::numeric_limits<float>::is_iec559||sizeof(float)!=4||
       std::fegetround()!=FE_TONEAREST)return S::unsupportedArithmetic;
    std::array<std::uint32_t,3> rgb{};
    for(unsigned lane=0;lane<3;++lane){
        // TextConfig ColorRGB is raw50. Light146FDC..147014 first divides
        // it by2;146C40 stores ambient,148F90 registers it in manager+50.
        const float managerAmbient=sourceDiv(definition->ambientColor[lane],2.f);
        //149310: directional RGBzero eliminates the dot-normal term.
        //Both normal-sign paths reduce to this division and multiplication.
        //The positive-normal path's clamp at1 does not bind for25/128.
        const float normalized=sourceDiv(managerAmbient,128.f);
        const float scaled=sourceMul(255.f,normalized);
        if(!std::isfinite(scaled)||scaled<0.f||scaled>255.f)
            return S::unsupportedArithmetic;
        //EE CVT.W.S truncation; range has been proved safe for uint32.
        rgb[lane]=static_cast<std::uint32_t>(scaled);
    }
    OriginalUiMainParticleLighting50KG prepared;
    const auto packed=(rgb[0]<<16)|(rgb[1]<<8)|rgb[2];
    for(unsigned corner=0;corner<4;++corner){
        prepared.sourceRgbPacked[corner]=packed;
        prepared.sourceLightingRgb46To49[corner]={{float(rgb[0]),float(rgb[1]),float(rgb[2])}};
        prepared.sourceLightingWBits46To49[corner]=0x40u;
    }
    output=prepared;
    return S::complete;
}
}
