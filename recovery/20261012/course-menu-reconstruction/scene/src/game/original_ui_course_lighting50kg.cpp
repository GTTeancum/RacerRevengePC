// Reconstructed source-only candidate, 2026-10-12; not byte-identical to lost candidate.
#include "original_ui_course_lighting50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>
namespace rr::game {
namespace {
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
bool normalize(std::array<float,3>& v) noexcept {
    const float square=add(add(mul(v[0],v[0]),mul(v[1],v[1])),mul(v[2],v[2]));
    if (!std::isfinite(square)||square<=0) return false;
    volatile float inverse=1.0f/std::sqrt(square);
    for (float& f:v) {f=mul(f,inverse);if (!std::isfinite(f)) return false;}
    return true;
}

}
bool prepareOriginalUiCourseLighting50KG(const assets::OriginalUiArchive50KG& archive,
    OriginalUiCourseLighting50KG& output,OriginalUiCourseLightingDiagnostic50KG& d) noexcept {
    d={};const auto fail=[&](std::uint32_t pc){d.sourcePC=pc;return false;};
    if (!archive.loaded()||archive.identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course)||
        std::fegetround()!=FE_TONEAREST) return fail(0x1CDB10);
    try {
        assets::OriginalUiDat50KG ui;
        if (!assets::readOriginalUiDat50KG(archive,"UI/UICourse/ui.dat",ui,d.parse,
            assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)) return fail(0x1CDB10);
        const auto* lights=ui.section("Lights");const auto* first=ui.section("Light1");
        const auto* second=ui.section("Light2");std::int32_t count=0;
        if (!lights||!first||!second||!assets::originalUiInteger50KG(*lights,"NumberOfLights",2,count,d.parse,0x1CDB4C)||count!=2)
            return fail(0x1CDB4C);
        const auto* type1=first->find("Type");const auto* type2=second->find("Type");
        if (!type1||!type2||type1->text!="Ambient"||type2->text!="Directional") return fail(0x1CE014);
        std::array<float,3> ambient{},directional{},position{},target{};
        if (!first->find("ColorRGB")||!second->find("ColorRGB")||!second->find("Position")||
            !second->find("TargetPosition")||second->find("LookVector")||
            !assets::originalUiTriple50KG(*first,"ColorRGB",{},ambient,d.parse,0x1CE094)||
            !assets::originalUiTriple50KG(*second,"ColorRGB",{},directional,d.parse,0x1CE094)||
            !assets::originalUiTriple50KG(*second,"Position",{},position,d.parse,0x1CE0D4)||
            !assets::originalUiTriple50KG(*second,"TargetPosition",{},target,d.parse,0x1CE2C0)) return fail(0x1CE094);
        OriginalUiCourseLighting50KG result;result.uiIdentity=ui.identity();
        for (unsigned i=0;i<3;++i) {
            if (ambient[i]<0||ambient[i]>255||directional[i]<0||directional[i]>255) return fail(0x146FDC);
            result.ambient50[i]=mul(ambient[i],.5f);
            result.directional3C[i]=mul(directional[i],.5f);
            result.directionalLook78[i]=sub(target[i],position[i]);
        }
        result.ambient50[3]=64; //148FF4..148FFC, independent of authored alpha.
        result.ambientSlot60=result.ambient50;result.directional3C[3]=127.5f;
        if (!normalize(result.directionalLook78)||!normalize(result.directionalLook78)) return fail(0x147200);
        output=result;return true;
    } catch (...) {return fail(0x1CDB10);}
}
}
