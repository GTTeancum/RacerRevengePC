#include "original_race_f8_storage50la.h"
#include <bit>
#include <cfenv>
#include <cmath>
#include <limits>
#include <thread>

namespace rr::game {
bool OriginalRaceF850LA::Impl::currentLeaf()const noexcept {
    // This concrete owner has no constructed child. Do not silently turn a
    // future nonempty141ED0 walk into success, or rely on a remembered flag.
    return constructed&&!closed&&counted&&fields&&application&&application->application10()&&
        application->nativeOwnerThread_==std::this_thread::get_id()&&
        !application->attachmentLock423F58()&&component.reference00==1&&
        component.kind==OriginalWreckComponentKind50LA::entity141300&&
        component.objectKey==std::uint64_t(reinterpret_cast<std::uintptr_t>(&component))&&
        component.sessionGeneration&&component.category28==0xff&&component.mask20==mask&&
        !component.firstChild18&&!component.descendants24&&
        application->entityAuthority_.canRetire(component)&&
        original_entity_chain_detail50LA::forest(application->entityAuthority_.view());
}

bool OriginalRaceF850LA::visibility199770(std::int32_t shown,std::int32_t immediate,
    std::int32_t cross,std::int32_t square,std::int32_t localizedCross)noexcept {
    if(!constructed()||!impl_->loadComplete||impl_->nativeBarBorrows||!impl_->currentLeaf())return false;
    auto& f=*impl_->fields;
    //199770..C4 performs these three stores even if bit0 does not change.
    // Source masks only bit0 of a3/t0/t1; a1/a2 use whole-word zero tests.
    f.flags1E4Low5=std::uint8_t((f.flags1E4Low5&~4u)|((std::uint32_t(cross)&1u)<<2));
    f.flags1E4Low5=std::uint8_t((f.flags1E4Low5&~8u)|((std::uint32_t(square)&1u)<<3));
    f.flags1E4Low5=std::uint8_t((f.flags1E4Low5&~16u)|((std::uint32_t(localizedCross)&1u)<<4));
    const bool wasShown=(f.flags1E4Low5&1u)!=0;
    if((shown!=0)==wasShown)return true;
    f.flags1E4Low5=std::uint8_t((f.flags1E4Low5&~1u)|(shown?1u:0u));
    if(!immediate)f.transition1E0=.25f; //1997F8 /199828, only on a bit0 edge.
    // Immediate leaves the existing transition untouched, not reset to -1.
    // Bit1 and the unrepresented upper lanes are never invented or rewritten.
    return true;
}

bool OriginalRaceF850LA::update199320(float actual23C,std::uint64_t publication,
    OriginalRaceF8Diagnostic50LA& d)noexcept {
    d={};
    if(!loadedCold()||impl_->nativeBarBorrows||!impl_->currentLeaf()){
        d.sourcePC=0x199320;d.operation="F8 complete current empty leaf required";return false;
    }
    auto& p=*impl_;
    if(!publication||p.lastUpdatePublication==std::numeric_limits<std::uint64_t>::max()||
       (p.lastUpdatePublication&&publication!=p.lastUpdatePublication+1))
        return p.stop(d,0x199320,"F8 update must consume the next actual Root publication once");
    // This private leaf is called only by the Application bridge. No public
    // delta setter or elapsed250 substitution exists. These are native finite
    // binary32 bounds, NOT an EE exceptional/FPU-low-bit emulation claim.
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
        std::numeric_limits<float>::digits==24,"F8 native binary32 transition");
#if defined(__FAST_MATH__) || defined(_M_FP_FAST) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
    return p.stop(d,0x19933c,"F8 transition requires strict native floating point");
#else
    const auto ordinary=[](float x)noexcept {
        return std::isfinite(x)&&(x==0.f||std::fpclassify(x)==FP_NORMAL);
    };
    if(std::fegetround()!=FE_TONEAREST||!ordinary(actual23C)||actual23C<0.f||
       actual23C>std::bit_cast<float>(std::uint32_t{0x3d75c28f})||
       !ordinary(p.fields->transition1E0))
        return p.stop(d,0x19933c,"F8 native transition operand/rounding domain");
    //19932C is transition <0, NOT <=0. Zero therefore reaches the SUB.S.
    // Keep one separately rounded subtraction and preserve negative overshoot.
    if(!(p.fields->transition1E0<0.f)){
        volatile float next=p.fields->transition1E0-actual23C;
        if(!ordinary(next))return p.stop(d,0x199340,"F8 native transition result domain");
        p.fields->transition1E0=next;d.sourceEffectsCommitted=true;
    }
    //141ED0 sees the real empty child/descendant state checked above, hence
    // reaches141F78 without a renderer call or fabricated child callback.
    p.lastUpdatePublication=publication;return true;
#endif
}

bool OriginalRaceF850LA::demand199350(OriginalRaceF8Diagnostic50LA& d)noexcept {
    d={};
    if(!loadedCold()||impl_->nativeBarBorrows||!impl_->currentLeaf()){
        d.sourcePC=0x199350;d.operation="F8 complete current empty leaf required";return false;
    }
    auto& p=*impl_;const auto& f=*p.fields;
    // Order from19935C..B0: shown, transition<=0, loaded-content bit1.
    // The source leaf returns1 directly; it does not call141C30 children.
    if(!(f.flags1E4Low5&1u))return true;
    if(!std::isfinite(f.transition1E0))return p.stop(d,0x199378,"F8 native transition domain");
    if(!(f.transition1E0<=0.f)||!(f.flags1E4Low5&2u))return true;
    if(!(f.flags1E4Low5&12u))return true;
    if(!p.domain||!p.domain->prepared()||!p.context||!p.context->live()||!p.fonts||
       p.fonts->domain_!=p.domain||p.fonts->context_!=p.context||!p.texts100||!p.textures40)
        return p.stop(d,0x1993b8,"F8 retained canonical material/font/context owners");
    const auto materials=p.domain->materials382B38();const auto textures=p.domain->textures383400();
    if(!materials||!textures||!p.context->materialApplication()||
       !materials->screenApplicationMatches(*p.context->materialApplication()))
        return p.stop(d,0x1993b8,"F8 same canonical tables");

    const auto demand=[&](std::int16_t token,const assets::OriginalStandardTextureResource50LA* expected,
        std::uint32_t pc)noexcept {
        assets::OriginalStandardTextureResource50LA* actual=nullptr;
        assets::OriginalStandardTextureRowView50LA row;
        assets::OriginalStandardTextureDiagnostic50LA td;
        if(!expected||!textures->row(token,row)||!row.references08||*row.references08<=0||
           row.object04!=expected||!textures->resolve298E60(token,actual,td)||actual!=expected)
            return p.stop(d,pc,"F8 actual canonical texture identity/reference");
        bool written=false;
        if(!actual->demandRaceF8199350(pc,written,td))
            return p.stop(d,td.sourcePC,"F8 canonical texture demand rejected");
        d.sourceEffectsCommitted=d.sourceEffectsCommitted||written;return true;
    };
    //1993B8..414 resolves Application328's actual named entry first. The
    // source genuinely permits an absent entry; no synthetic font is created.
    const auto font=p.fonts->find282210(assets::OriginalApplicationFonts50LA::Name::denmark16);
    if(font){
        d.object=0x100;
        if(font!=(*p.texts100)[0].font4C||font!=(*p.texts100)[1].font4C||
           !assets::OriginalApplicationFonts50LA::currentResource281F90(*p.domain,*font))
            return p.stop(d,0x1993ec,"F8 same registered denmark16 font resource");
        if(!demand(font->texture408_,font->texture281F90(),0x199414))return false;
    }
    //2DF8A0 re-resolves each ScreenTexture's actual primitive/material/pass0
    //map0. Do not demand its cached atlas by name or substitute a frame/image.
    for(std::size_t i=0;i<2;++i){
        if(!(f.flags1E4Low5&(4u<<i)))continue;
        d.object=0x40+0x60*i;const auto pc=i?0x199494u:0x199454u;
        const auto& t=(*p.textures40)[i];const auto reference=4+i;
        const auto& r=p.references[reference];
        const assets::OriginalStandardMaterialObject50LA* material=nullptr;
        assets::OriginalStandardMaterialRowView50LA row;
        assets::OriginalStandardMaterialPassView50LA pass;
        if(!r.retained||!t.primitive54||t.primitive54->reference!=reference||
           !t.buffer58||t.primitive54->packet10!=t.buffer58.get()||
           !materials->row(r.row,row)||!row.references08||*row.references08<=0||
           !materials->resolve298E60(r.row,material,p.materialDiagnostic)||!material||row.object04!=material||
           !material->constructed29B550()||material->textures383400()!=textures.get()||
           !material->pass4C(0,pass)||!pass.mapCount20||!*pass.mapCount20||
           !pass.texture1A||(*pass.texture1A)[0]<0)
            return p.stop(d,0x2df8a0,"F8 actual icon primitive/material/pass0 map0");
        if(!demand((*pass.texture1A)[0],t.atlas,pc))return false;
    }
    return true;
}

bool OriginalRaceF850LA::layout198CF0(const std::array<std::int32_t,4>& rectangle,
    OriginalRaceF8Diagnostic50LA& d)noexcept {
    d={};
    if(!loadedCold()||impl_->nativeBarBorrows||!impl_->currentLeaf()){
        d.sourcePC=0x198cf0;d.operation="F8 current loaded empty leaf required for layout";return false;
    }
    auto& p=*impl_;
    // Authenticate all owned source destinations before the first store.
    for(std::size_t i=0;i<4;++i){
        d.object=0x30+4*i;
        if(!p.polygonCurrent(i)||p.polygons30[i]->nativePacketPins||
           p.polygons30[i]->packetRevision==std::numeric_limits<std::uint64_t>::max())
            return p.stop(d,0x198de8,"F8 mutable actual type1 polygon/packet ownership");
    }
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
        std::numeric_limits<float>::digits==24,"F8 native binary32 layout");
#if defined(__FAST_MATH__) || defined(_M_FP_FAST) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
    return p.stop(d,0x198d30,"F8 layout requires strict native floating point");
#else
    const auto ordinary=[](float x)noexcept {
        return std::isfinite(x)&&(x==0.f||std::fpclassify(x)==FP_NORMAL);
    };
    // Bound signed integer conversion to exact binary32 values. The retained
    // one-human640x448 camera lies inside this domain; no guessed rectangle.
    for(const auto word:rectangle)if(word < -16777216||word > 16777216)
        return p.stop(d,0x198d30,"F8 viewport outside exact native conversion domain");
    const float transition=p.fields->transition1E0;
    if(std::fegetround()!=FE_TONEAREST||!ordinary(transition)||transition>.25f)
        return p.stop(d,0x198d3c,"F8 native layout transition/rounding domain");
    // Source conversion order: left, right, top, bottom (delay slot198D48).
    // Volatile binary32 stores keep each operation separately rounded; this
    // is a bounded native adaptation, not EE FPU exceptional/low-bit parity.
    volatile float left=static_cast<float>(rectangle[0]);
    volatile float right=static_cast<float>(rectangle[2]);
    volatile float top=static_cast<float>(rectangle[1]);
    volatile float bottom=static_cast<float>(rectangle[3]);
    volatile float topInner=0.f,bottomInner=0.f;
    const bool shown=(p.fields->flags1E4Low5&1u)!=0;
    if(!(transition<0.f)){
        volatile float fraction=transition/.25f;
        if(shown){volatile float reversed=1.f-fraction;fraction=reversed;}
        volatile float margin=65.f*fraction;
        topInner=top+margin;bottomInner=bottom-margin;
    }else{
        const float margin=shown?65.f:2.f;
        topInner=margin+top;bottomInner=bottom-margin;
    }
    if(!ordinary(topInner)||!ordinary(bottomInner))
        return p.stop(d,0x198de8,"F8 native layout arithmetic result domain");
    const auto store=[&](std::size_t polygon,std::size_t vertex,float x,float y)noexcept {
        auto& object=*p.polygons30[polygon];
        if(!vertex)++object.packetRevision; //Native mutation history only.
        auto& v=object.vertices00[vertex];
        v.authored00[0]=x;v.authored00[1]=y;
        *v.stq14=v.authored00; //Whole original vector; preserve existing Z/W.
        d.object=0x30+4*polygon;d.sourceEffectsCommitted=true;
    };
    //198E0C..19905C: ascending30/34, four triangle-strip vertices each.
    store(0,0,left,top);store(0,1,left,topInner);
    store(0,2,right,top);store(0,3,right,topInner);
    store(1,0,left,bottomInner);store(1,1,left,bottom);
    store(1,2,right,bottomInner);store(1,3,right,bottom);
    //199090/1990F0 repeat SUB.S. Keep the source arithmetic/store order.
    volatile float rightInner=right-2.f;store(2,0,rightInner,top);
    rightInner=right-2.f;store(2,1,rightInner,bottom);
    store(2,2,right,top);store(2,3,right,bottom);
    store(3,0,left,top);store(3,1,left,bottom);
    volatile float leftInner=2.f+left;
    store(3,2,leftInner,top);store(3,3,leftInner,bottom);
    //198CF0 does NOT dirty3C, regenerate2E21D0, change color/depth, demand
    // textures, submit primitives, or recurse. All those fields stay intact.
    d.sourcePC=0x19930c;return true;
#endif
}
}
