#pragma once
// Private CPU state/arithmetic support, not a constructible race+D0
// resource owner. No material, primitive, submission or readiness is fabricated.
#include "../assets/original_application_fonts50la.h"
#include "../assets/original_ui_localization50kg.h"
#include "control_presets49d.h"
#include "lap_timing50id.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>

namespace rr::game::time_stats_detail50LA {
// Adapter-owned, synchronous borrows from actual bank/manager fields. These
// types do not certify their provenance; root must construct them only within
// the actual bank read lifetime, after manager gap updates, for one view.
struct Record {
    // Stable scalar snapshot borrows populated coherently from real bank
    // reads. Never bind these references to Bank::read() temporaries.
    const float& timeCB0;
    const float& bestLapCB4;
    const float& timeCB8;
    const float& valueCBC;
    const std::uint32_t& lapCC8;
    const std::int32_t& placeCCC;
    const std::uint8_t& flagsCD8;
};
struct ScreenText {
    std::array<char,41> bytes{};
    std::uint8_t length=0,flags5A=0;
    float x10=0,y14=0;
    std::uint32_t color54=0xffe5d96c,justification50=0;
    // Post2DFD90 default, not constructor2E02F0's -1. RGB color setters do
    // not derive this value from color54's high byte.
    std::int16_t alpha58=255;
};
struct ScreenTexture {
    float x10=0,y14=0;
    // Source dimensions/UVs are loader state, not renderer defaults.
    float width34=0,height38=19,field3C=0;
    float u0=0,v0=.796875f,u1=0,v1=.9375f;
    std::uint8_t flags5C=0;
};
struct ViewAnimation {
    float transition524=0,hold52C=0,offset534=-165;
    std::int32_t direction53C=1,observedLap544=0;
};
inline bool rowVisible(bool predicateActive,const ViewAnimation& view) noexcept {
    return predicateActive||view.offset534!=-165.f;
}
inline std::int32_t signedWord(std::uint32_t bits) noexcept {
    std::int32_t result;std::memcpy(&result,&bits,sizeof(result));return result;
}
inline bool predicate(std::size_t row,const ControlPresetSlot49D& slot,
    std::uint32_t actualVehicleState40) noexcept {
    if(actualVehicleState40!=0)return false;
    const auto f=signedWord(slot.field10F94);
    if(row==0)return f==1||f==2||f==3||f==7;
    if(row==1)return f==2||f==4||f==5||f==7;
    return row==2&&(f==3||f>=5);
}
inline bool displayGate(std::uint32_t view,std::uint8_t raceFlags1B0,
    std::uint8_t raceEntityFlags29,std::uint8_t uiFlags1E4) noexcept {
    return view<2&&(view!=1||(raceFlags1B0&3u)>=2u)&&
        !(raceFlags1B0&0x10u)&&!(raceEntityFlags29&4u)&&!(uiFlags1E4&1u);
}
inline bool time189000(float seconds,std::array<char,9>& out) noexcept {
    // Deliberately the same finite-input admission as existing race HUD.
    // General EE nonfinite behavior/host contraction parity is not claimed.
    if(!std::isfinite(seconds))return false;
    if(seconds<=0||seconds>=21474836.f){std::memcpy(out.data(),"--:--.--",9);return true;}
    const float scaled=seconds*100.f;
    const float rounded=.5f+scaled;
    if(!std::isfinite(rounded)||rounded>=2147483648.f)return false;
    const auto ticks=static_cast<std::int32_t>(rounded);
    const auto minutes=std::min((ticks/100)/60,99),secondsPart=(ticks/100)%60,hundredths=ticks%100;
    out={{char('0'+minutes/10),char('0'+minutes%10),':',char('0'+secondsPart/10),
        char('0'+secondsPart%10),'.',char('0'+hundredths/10),char('0'+hundredths%10),0}};
    return true;
}
inline void assign(ScreenText& text,const char* bytes,std::size_t length) noexcept {
    // Source compares original input length/content BEFORE truncation.
    if(length==text.length&&!std::memcmp(text.bytes.data(),bytes,length))return;
    length=std::min<std::size_t>(length,40);
    std::memcpy(text.bytes.data(),bytes,length);text.bytes[length]=0;
    text.length=static_cast<std::uint8_t>(length);text.flags5A|=3;
}
inline std::size_t decimal(std::int32_t value,char* out) noexcept {
    char reverse[10];std::size_t count=0,n=0;
    const bool negative=value<0;
    std::uint32_t magnitude=negative?0u-static_cast<std::uint32_t>(value):static_cast<std::uint32_t>(value);
    do{reverse[count++]=char('0'+magnitude%10u);magnitude/=10u;}while(magnitude);
    if(negative)out[n++]='-';
    while(count)out[n++]=reverse[--count];
    out[n]=0;return n;
}
inline bool gap1962B4(const Record& selected,const Record* records,std::size_t count,
    std::uint32_t managerActiveCount50,bool predicateActive,std::array<char,10>& out) noexcept {
    if(count&&!records)return false;
    float seconds=0;unsigned char prefix=0;
    if(!(selected.flagsCD8&1u)){
        if(selected.placeCCC>=2){seconds=selected.valueCBC;if(seconds!=0)prefix=0x40;}
        else {
            const Record* second=nullptr;
            for(std::size_t i=0;i<count;++i)if(records[i].placeCCC==2){second=&records[i];break;}
            // Active1962FC delay-slot ownCBC survives BOTH fallback conditions.
            // Sliding1969B4 has neither ownCBC fallback nor active-count check.
            if(predicateActive&&(!second||managerActiveCount50==1))seconds=selected.valueCBC;
            else if(second){seconds=second->valueCBC;if(seconds!=0)prefix=0x26;}
        }
    }
    std::array<char,9> formatted{};if(!time189000(seconds,formatted))return false;
    std::array<char,10> next{};const std::size_t offset=prefix?1:0;
    if(prefix)next[0]=static_cast<char>(prefix);
    std::memcpy(next.data()+offset,formatted.data(),9);out=next;return true;
}
// Six texts in original storage order40,B0,120,190,200,270; textures in
// order2E0,340,3A0,400,460,4C0. One shared set is laid out for the chosen view;
// only animation bookkeeping is doubled. Do not clone six objects per view.
struct State {
    std::array<ScreenText,6> text{};
    std::array<ScreenTexture,6> texture{};
    std::array<ViewAnimation,2> views{};
    // Extra550 is inert constructor/destructor state, never made visible.
    ScreenText extra550{};
    std::uint32_t mode520=0;
    void reset194CC0() noexcept {for(auto& view:views)view=ViewAnimation{};}
    bool advance194D00(float elapsed) noexcept {
        if(!std::isfinite(elapsed)||elapsed<0)return false;
        // No overshoot carry into the closing transition. Newly created hold
        // IS decremented in this same iteration, matching194DD8 onward.
        for(auto& view:views){
            if(view.transition524>0){
                const float ratio=view.transition524/.2f;
                if(view.direction53C)view.offset534=-165.f*ratio;
                else {const float inverse=1.f-ratio;view.offset534=-165.f*inverse;}
                view.transition524=view.transition524-elapsed;
                if(view.transition524<=0&&view.direction53C){view.hold52C=4.5f;view.offset534=0;}
                if(view.transition524<=0&&!view.direction53C)view.offset534=-165.f;
            }
            if(view.hold52C>0){
                view.hold52C=view.hold52C-elapsed;
                if(view.hold52C<=0){view.direction53C=0;view.transition524=.2f;}
            }
        }
        // Enclosing entity owner must separately invoke genuine141ED0 after.
        return true;
    }
    bool layout193BE0(std::uint32_t oneBasedView,const ControlPresets49D& preferences,
        std::uint32_t actualVehicleState40) noexcept {
        if(oneBasedView<1||oneBasedView>2)return false;
        const auto v=oneBasedView-1;const auto& a=views[v];
        const float x=mode520!=0&&mode520!=1&&oneBasedView!=1?-20.f:0.f;
        const float y=mode520==1&&oneBasedView!=1?-27.f:0.f;
        float stagger=a.direction53C?std::min(0.f,a.offset534+50.f):a.offset534;
        constexpr std::size_t label[]={2,0,1},value[]={5,3,4};
        for(std::size_t row=0;row<3;++row){
            if(row==1)stagger=a.direction53C?std::min(0.f,25.f+a.offset534):std::max(-165.f,stagger-25.f);
            if(row==2)stagger=a.direction53C?std::min(0.f,a.offset534):std::max(-165.f,stagger-25.f);
            const bool active=predicate(row,preferences.slots[v],actualVehicleState40);
            const float left=active?x:x+stagger;
            float right=149.f+x,labelX=30.f+x,valueX=167.f+x;
            if(!active){right=right+stagger;labelX=labelX+stagger;valueX=valueX+stagger;}
            const float textureY=(30.f+22.f*static_cast<float>(row))+y;
            const float textY=(31.f+22.f*static_cast<float>(row))+y;
            const auto texturePosition=[](ScreenTexture& t,float px,float py){
                if(t.x10==px&&t.y14==py)return;
                t.x10=px;t.y14=py;t.flags5C|=1;if(t.field3C!=0)t.flags5C|=2;
            };
            const auto textPosition=[](ScreenText& t,float px,float py){
                if(t.x10==px&&t.y14==py)return;
                t.x10=px;t.y14=py;t.flags5A|=3;
            };
            texturePosition(texture[2*row],left,textureY);texturePosition(texture[2*row+1],right,textureY);
            textPosition(text[label[row]],labelX,textY);textPosition(text[value[row]],valueX,textY);
        }
        return true;
    }
    bool prepare194F40(std::uint32_t view,const ControlPresets49D& preferences,
        std::uint32_t actualVehicleState40,const std::uint32_t& actualLapCC8,
        std::uint8_t actualManagerFlags58) noexcept {
        if(!layout193BE0(view+1,preferences,actualVehicleState40))return false;
        auto& a=views[view];
        if(a.observedLap544<signedWord(actualLapCC8)&&!(actualManagerFlags58&1u)){
            a.direction53C=1;a.transition524=.2f;++a.observedLap544;
        }
        return true;
    }
};
// Retained authentic ownership ingredients, deliberately NOT a resource-ready
// owner. Root's eventual race+D0 object needs these AND genuine primitive
// references before it may publish itself. Declare this before screen states
// so normal destruction destroys screen borrows first.
struct ResourceBorrows {
    std::shared_ptr<const assets::OriginalUiArchive50KG> localizationArchive;
    std::shared_ptr<const assets::OriginalUiEnglish50KG> english;
    std::shared_ptr<const assets::OriginalApplicationFonts50LA::Entry> eurostile13,denmark16;
};
// Content preparation is intentionally separate from source text mutation and
//246760 submission: original draws a label before computing/drawing its value.
// Root must not call this an accepted draw or reorder renderer side effects.
struct RowContent {
    const assets::OriginalUiLocalizedEntry50KG* label=nullptr;
    std::array<char,41> value{};
    std::size_t valueLength=0;
};
inline bool prepareRow(std::size_t row,bool predicateActive,const Record& selected,
    const Record* records,std::size_t managerCount30,std::uint32_t managerActiveCount50,
    std::uint32_t subtype11258,std::uint8_t lapCount1125C,
    const assets::OriginalUiEnglish50KG& english,RowContent& out) noexcept {
    if(row>2||!english.loaded())return false;
    RowContent next;
    const std::size_t labelId=row==0?458:row==1?459:subtype11258==1?460:461;
    next.label=english.at(labelId);if(!next.label)return false;
    if(row==0){
        // ADDIU wraps at32 bits; source signed comparison caps only upper end.
        auto lap=signedWord(selected.lapCC8+1u);
        if(subtype11258==1){
            const auto maximum=static_cast<std::int32_t>(lapCount1125C&31u);
            lap=std::min(lap,maximum);
            next.valueLength=decimal(lap,next.value.data());
            next.value[next.valueLength++]='/';
            next.valueLength+=decimal(maximum,next.value.data()+next.valueLength);
        }else next.valueLength=decimal(lap,next.value.data());
    }else if(row==2&&subtype11258==1){
        std::array<char,10> gap{};
        if(!gap1962B4(selected,records,managerCount30,managerActiveCount50,predicateActive,gap))return false;
        next.valueLength=gap[0]==char(0x40)||gap[0]==char(0x26)?9:8;
        std::memcpy(next.value.data(),gap.data(),next.valueLength+1);
    }else {
        const float seconds=row==2?selected.bestLapCB4:
            subtype11258==3?selected.timeCB0:selected.timeCB8;
        std::array<char,9> formatted{};if(!time189000(seconds,formatted))return false;
        std::memcpy(next.value.data(),formatted.data(),9);next.valueLength=8;
    }
    out=next;return true;
}
} // namespace rr::game::time_stats_detail50LA
