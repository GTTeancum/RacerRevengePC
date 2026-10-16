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
    enum class Kind : std::uint8_t {text,texture};
    struct Command {Kind kind=Kind::text;std::size_t object=0;std::uint32_t sourcePC=0;};
    class Commands final {
        friend class OriginalPauseLayout50LA;
        std::array<Command,15> commands_{};
        std::size_t count_=0;
    public:
        std::size_t count()const noexcept{return count_;}
        const Command* at(std::size_t i)const noexcept{return i<count_?&commands_[i]:nullptr;}
        // No owner pointer or draw-ready receipt. Future lowering must consume
        // these commands synchronously under the same actual Pause owner lease.
    };
    struct Diagnostic {
        std::uint32_t sourcePC=0;
        OriginalPauseTextGeometry50LA::Diagnostic text;
        OriginalPauseTextureGeometry50LA::Diagnostic texture;
    };
private:
    static bool visible(const Storage& p)noexcept {return p.mode1F0_==0&&p.opening30_<=0.f;}
    static void position(Storage::Text& t,float x,float y)noexcept {
        if(t.x10==x&&t.y14==y)return;
        t.x10=x;t.y14=y;t.flags5A|=3u;
    }
    static bool prepare1BB010(Storage& p,Commands& out,Diagnostic& d)noexcept {
        out={};d={};
        if(p.stage_!=Storage::Stage::derivedResources||!p.context_||!p.context_->live()||
           p.selection880_>=11){d.sourcePC=0x1BB010;return false;}
        if(!visible(p))return true;
        Commands next;
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
