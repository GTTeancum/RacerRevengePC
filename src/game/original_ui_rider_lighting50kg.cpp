#include "original_ui_rider_lighting50kg.h"
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
bool color(const std::array<float,4>& c) noexcept {
    for (float f:c) if (!std::isfinite(f)||f<0||f>255) return false;
    return true;
}
bool genuine(const OriginalUiRiderLighting50KG& s) noexcept {
    const auto* a=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::rider);
    if (!a||!s.uiIdentity) return false;
    for (std::size_t i=0;i<a->memberCount;++i) {
        const auto* p=assets::originalUiMemberIdentity50KG(a->id,i);
        if (p==s.uiIdentity&&std::strcmp(p->name,"ui\\uirider\\ui.dat")==0) {
            for (float x:s.directionalLook78) if (!std::isfinite(x)) return false;
            return color(s.ambient50)&&color(s.directional3C);
        }
    }
    return false;
}
}
bool prepareOriginalUiRiderLighting50KG(const assets::OriginalUiArchive50KG& archive,
    OriginalUiRiderLighting50KG& output,OriginalUiRiderLightingDiagnostic50KG& d) noexcept {
    d={};const auto fail=[&](std::uint32_t pc){d.sourcePC=pc;return false;};
    if (!archive.loaded()||archive.identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::rider)||
        std::fegetround()!=FE_TONEAREST) return fail(0x1CDB10);
    try {
        assets::OriginalUiDat50KG ui;
        if (!assets::readOriginalUiDat50KG(archive,"UI/UIRider/ui.dat",ui,d.parse,
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
        OriginalUiRiderLighting50KG result;result.uiIdentity=ui.identity();
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
bool captureOriginalUiRiderLighting50KG(const OriginalUiRiderLighting50KG& current,
    OriginalUiRiderLightCapture50KG& capture) noexcept {
    if (!genuine(current)) return false;
    OriginalUiRiderLightCapture50KG next;next.ambient520=current.ambient50;
    // Source getter packs each truncated lane and caller extracts its byte.
    for (unsigned i=0;i<4;++i)
        next.directional530[i]=static_cast<float>(static_cast<std::uint32_t>(current.directional3C[i])&255u);
    next.captured=true;capture=next;return true;
}
bool applyOriginalUiRiderLighting50KG(bool unlocked,const OriginalUiRiderLightCapture50KG& capture,
    OriginalUiRiderLighting50KG& current) noexcept {
    if (!genuine(current)||!capture.captured||!color(capture.ambient520)||!color(capture.directional530)) return false;
    current.ambient50=unlocked?capture.ambient520:std::array<float,4>{{0,0,0,64}};
    current.ambientSlot60=current.ambient50;
    current.directional3C=unlocked?capture.directional530:std::array<float,4>{{0,0,0,255}};
    return true;
}
}
