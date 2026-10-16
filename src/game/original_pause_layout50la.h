#pragma once
#include "original_pause_text_geometry50la.h"
#include "original_pause_texture_geometry50la.h"

namespace rr::game {
// Serial CPU preparation at the original derived callback boundary. These
// commands identify source draw calls; they are not renderer submissions.
class OriginalPauseLayout50LA final {
    friend class original_pause_observation50LA::Owner;
    using Storage=OriginalPauseStorage50LA;
public:
    enum class Kind : std::uint8_t {text,texture,polygon};
    struct Command {Kind kind=Kind::text;std::size_t object=0;std::uint32_t sourcePC=0;};
    class Commands final {
        friend class OriginalPauseLayout50LA;
        std::array<Command,24> commands_{};
        std::size_t count_=0;
        const Storage* owner_=nullptr;
        bool outer_=false,viewportOverride_=false;
    public:
        bool viewportOverride()const noexcept{return viewportOverride_;}
        bool outer()const noexcept{return outer_;}
        const Storage* owner()const noexcept{return owner_;}
        std::size_t count()const noexcept{return count_;}
        const Command* at(std::size_t i)const noexcept{return i<count_?&commands_[i]:nullptr;}
        // Borrowed owner identity only, never a draw-ready receipt. Lowering must consume
        // these commands synchronously under the same actual Pause owner lease.
    };
    struct Diagnostic {
        std::uint32_t sourcePC=0;
        OriginalPauseTextGeometry50LA::Diagnostic text;
        OriginalPauseTextureGeometry50LA::Diagnostic texture;
    };
private:
    // Ordered tail1B9D84..1B9F38, after genuine geometry/layout
    // observations. No timer tick, last-camera admission or Entity callback.
    static bool prepareOuterTail1B9D84(Storage& p,const Commands& derived,
        std::uint8_t actualProfile1125C,Commands& out,Diagnostic& d)noexcept {
        out={};d={};const auto count=(actualProfile1125C>>5)&3u;
        if(p.stage_!=Storage::Stage::derivedResources||(count&&derived.owner_!=&p)||derived.outer_||
           p.mode1F0_!=0){d.sourcePC=0x1B9C24;return false;}
        Commands next;next.owner_=&p;next.outer_=true;next.viewportOverride_=count>=2;
        if(!count){out=next;return true;}
        const auto add=[&](Kind kind,std::size_t object,std::uint32_t pc){
            if(next.count_==next.commands_.size())return false;
            next.commands_[next.count_++]={kind,object,pc};return true;
        };
        if(count>=2&&!add(Kind::polygon,0,0x1B9E14))return false;
        // Derived virtual78: conditional icon FIRST, then the three backdrops.
        if(p.opening30_<=0.f&&!add(Kind::texture,3,0x1BAFA4))return false;
        if(!p.hidden160_||p.transition164_>=0.f){
            if(!add(Kind::texture,0,0x1BAFE0)||!add(Kind::texture,1,0x1BAFE8)||
               !add(Kind::texture,2,0x1BAFF0))return false;
        }
        for(std::size_t i=0;i<derived.count_;++i){const auto& c=derived.commands_[i];
            if(!add(c.kind,c.object,c.sourcePC))return false;
        }
        // mode1F0!=0 needs its real mode1F4 device/localization producer above;
        // reject it rather than drawing a fabricated or stale baseText.
        if(count==1&&!add(Kind::polygon,0,0x1B9EF8))return false;
        if((p.hidden160_||p.transition164_>=0.f)&&!add(Kind::polygon,1,count>=2?0x1B9E80:0x1B9F30))return false;
        out=next;return true;
    }
    static bool visible(const Storage& p)noexcept {return p.mode1F0_==0&&p.opening30_<=0.f;}
    static void position(Storage::Text& t,float x,float y)noexcept {
        if(t.x10==x&&t.y14==y)return;
        t.x10=x;t.y14=y;t.flags5A|=3u;
    }
    static bool prepare1BB010(Storage& p,Commands& out,Diagnostic& d)noexcept {
        out={};d={};
        if(p.stage_!=Storage::Stage::derivedResources||!p.context_||!p.context_->live()||
           p.selection880_>=11){d.sourcePC=0x1BB010;return false;}
        if(!visible(p)){out.owner_=&p;return true;}
        Commands next;next.owner_=&p;
        const auto text=[&](std::size_t object,std::uint32_t pc)->bool {
            if(!OriginalPauseTextGeometry50LA::regenerate2E0170(p,object,0xffffffu,d.text)){
                d.sourcePC=pc;return false;
            }
            next.commands_[next.count_++]={Kind::text,object,pc};return true;
        };
        const auto texture=[&](std::size_t object,std::uint32_t pc)->bool {
            OriginalPauseTextureGeometry50LA::Projection packet;
            if(!OriginalPauseTextureGeometry50LA::regenerate2DF550(p,object,packet,d.texture)){
                d.sourcePC=pc;return false;
            }
            next.commands_[next.count_++]={Kind::texture,object,pc};return true;
        };
        position(p.derivedTexts_[0],320.f,82.f);
        if(!text(1,0x1BB31C))return false;
        for(std::size_t i=0;i<11;++i){
            auto& row=p.derivedTexts_[2+i];
            const float offset=20.f*static_cast<float>(i);
            position(row,320.f,112.f+offset);
            Storage::setTextColor(row,i==p.selection880_?p.selectedColor170_:p.normalColor16C_);
            if(!text(3+i,0x1BD5D0))return false;
        }
        // Source cases0/1/9/10 clear s3; only selected settings rows2..8
        // retain the selected text pointer for the width-dependent arrows.
        if(p.selection880_>=2&&p.selection880_<=8){
            auto& selected=p.derivedTexts_[2+p.selection880_];
            std::int32_t width=0;
            if(!OriginalPauseTextGeometry50LA::selectedWidth2E16F0(p,width,d.text)){
                d.sourcePC=0x1BD618;return false;
            }
            auto& left=p.arrows_[0];
            const float leftX=selected.x20-left.width34;
            Storage::setDimension(left,left.x10,0.f+leftX);
            Storage::setDimension(left,left.y14,selected.y24+2.f);
            if(!texture(4,0x1BD6B8))return false;
            auto& right=p.arrows_[1];
            const float textEnd=selected.x20+static_cast<float>(width);
            Storage::setDimension(right,right.x10,6.f+textEnd); //1BD6CC then1BD6D0.
            Storage::setDimension(right,right.y14,selected.y24+2.f);
            if(!texture(5,0x1BD74C))return false;
        }
        if(!text(2,0x1BD768))return false;
        out=next;return true;
    }
};
}
