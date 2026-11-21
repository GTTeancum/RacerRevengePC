#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_main_session50kg.h"
#include "original_ui_controller_popup_resources50kg.h"
#include "original_ui_main_exit_game50xg.h"
#include "../../../src/assets/material_texture_assets50ea.h"
#include "../../../src/assets/original_ui_overlay_text50kg.h"
#include "../../../src/assets/original_ui_screen_assets50kg.h"
#include "../../../src/game/original_ui_controller_popup_geometry50kg.h"
#include "../../../src/game/original_ui_main_layout50kg.h"
#include "../../../src/game/original_ui_text50kg.h"
#include "../../../src/game/original_ui_text_geometry50kg.h"
#include "../../../src/game/original_ui_text_position50kg.h"
#include "../../../src/game/original_ui_text_widget50kg.h"
#include <cmath>
#include <cstdio>
#include <exception>
#include <cstring>
#include <new>
#include <utility>

namespace rr::platform {
namespace {
using Status=OriginalUiMainSessionStatus50KG;
using WidgetStatus=game::OriginalUiMainWidgetStatus50KG;
using Phase=game::OriginalUiMainWidgetPhase50KG;
using Notice=game::OriginalUiTransitionNotificationKind50KG;
using Hook=game::OriginalUiApertureHookKind50KG;
using TransitionStatus=game::OriginalUiTransitionStatus50KG;
using ApertureStatus=game::OriginalUiApertureStatus50KG;
bool widgetOkay(WidgetStatus s) noexcept {
    return s==WidgetStatus::ok || s==WidgetStatus::noChange || s==WidgetStatus::hidden || s==WidgetStatus::lockedOut;
}
bool apertureOkay(ApertureStatus s) noexcept {return s==ApertureStatus::ok || s==ApertureStatus::noChange;}
// PC-NATIVE ADDITION (EXIT GAME, original_ui_main_exit_game50xg.h): process-wide host
// status of the Main session that last published it, and the one-shot exit request.
// owner is compared only, never dereferenced.
OriginalUiMainExitGameStatus50XG exitGameStatus50XG{};
const void* exitGameStatusOwner50XG=nullptr;
bool exitGameRequest50XG=false;
Status result(OriginalUiMainSessionDiagnostic50KG& d,Status s,std::uint32_t pc=0) noexcept {
    d.status=s;d.sourcePC=pc;return s;
}
}
// PC-NATIVE ADDITION (EXIT GAME, original_ui_main_exit_game50xg.h).
OriginalUiMainExitGameStatus50XG originalUiMainExitGameStatus50XG() noexcept {return exitGameStatus50XG;}
bool takeOriginalUiMainExitGameRequest50XG() noexcept {
    const bool requested=exitGameRequest50XG;exitGameRequest50XG=false;return requested;
}
void resetOriginalUiMainProfileFields50KG(OriginalUiMainProfile50KG& p) noexcept {
    p.savedSlot6E0=1;
    p.availability11018=static_cast<std::uint8_t>((p.availability11018&0x80u)|4u);
    // Does not mark an otherwise unknown imported profile as wholly known.
}
struct OriginalUiMainSession50KG::Storage {
    OriginalUiMainSessionCold50KG cold;
    OriginalUiMainDraw50KG collector;
    game::OriginalUiTransitionOwner50KG transition;
    game::OriginalUiApertureOwner50KG aperture;
    game::OriginalUiMainSceneOwner50KG scene;
    game::OriginalUiMainFireOwner50KG fire;
    game::OriginalUiMainCameraPreparation50KG camera;
    game::OriginalUiMainParticleRenderState50KG particleRenderState;
    game::OriginalUiMainParticleLighting50KG lighting;
    OriginalUiMainSessionSnapshot50KG state;
    OriginalUiMainSessionDiagnostic50KG callbackFault;
    bool dependenciesPrepared=false,rendering=false,inOperation=false,paused=false;
    bool popupAttached=false,sourceClosing=false,sourceTornDown=false;
    std::uint64_t inputSerial=0;
    OriginalUiMainRouteProposal50KG pendingRoute{};
    bool updateAudioPrepared=false;
    OriginalUiMainSessionUpdate50KG preparedUpdate{};

    // ---- PC-NATIVE ADDITION, NOT ORIGINAL: EXIT GAME row and its yes/no dialog. ----
    // Host memory only (see original_ui_main_exit_game50xg.h). The original Main page has
    // five Group 1 rows and no exit; nothing below adds to ui.dat, the widget bank or the
    // navigation arrays. The row sits after the last enabled row in the wrap ring
    // (last -> EXIT -> first). It is drawn from a host copy of the slot-5 row descriptor
    // (Main's own font, material, colours and fade) one row pitch lower, with Main's own
    // selection arrows. The dialog copies the shared 47C popup: its materials, its layout
    // constants (2055B0/2064C0 in original_ui_controller_popup_geometry50kg.cpp), its
    // original state machine run on a HOST copy of the state (never APP UI+47C) and its
    // sounds (1 open/Confirm, 5 Cancel). Message text "EXIT GAME?" is PC-native; the
    // Confirm/Cancel labels are the original LOC 9/10 strings the 47C popup prepared.
    struct ExitGame {
        bool prepared=false,rowSelected=false,confirmed=false,requested=false;
        assets::OriginalUiOverlayText50KG rowDescriptor;
        game::OriginalUiTextGeometry50KG row;
        std::uint32_t rowMaterial=0,arrowRight=0,arrowLeft=0;
        float rowZ=0,arrowZ=0;
        // 0 message, 1 Confirm label, 2 Cancel label (2055B0 ScreenTexts +270/+2E0/+350).
        std::array<game::OriginalUiTextLayout50KG,3> text{};
        std::uint32_t dim=0,overlay=0,font=0,cross=0,triangle=0;
        float crossWidth=0,crossHeight=0,triangleWidth=0,triangleHeight=0;
        float overlayReciprocalWidth=0,overlayReciprocalHeight=0;
        game::OriginalUiControllerPopupState50KG dialog=game::OriginalUiControllerPopup50KG::initialState();
    } exitGame;
    ~Storage(){
        clearOriginalUiMainExitRowOwner50XG(&collector);
        if(exitGameStatusOwner50XG==this){exitGameStatus50XG={};exitGameStatusOwner50XG=nullptr;}
    }
    void publishExitGame() noexcept {
        const auto& e=exitGame;exitGameStatusOwner50XG=this;
        exitGameStatus50XG={e.prepared,e.rowSelected,e.dialog.active3CC,
            e.dialog.active3CC&&e.dialog.result3D8==1&&e.dialog.timer3C4<=0,e.requested};
    }
    // Page entry/opening/closing callbacks reselect the original row themselves
    // (openingAC groupDeselect + selectSlot(savedSlot6E0)); the host row simply lets go.
    void resetExitGame() noexcept {
        if(exitGame.requested)return; // exit already handed to the frontend loop
        exitGame.rowSelected=false;exitGame.confirmed=false;
        exitGame.dialog=game::OriginalUiControllerPopup50KG::initialState();
        clearOriginalUiMainExitRowOwner50XG(&collector);publishExitGame();
    }
    static float exitRounded(float v) noexcept {volatile float out=v;return out;}
    // The 47C ScreenText setup (2E08D0 capacity, 2E1504 centring for justification 3).
    static bool exitText(game::OriginalUiTextLayout50KG& out,const std::string& bytes,std::size_t capacity,
        float x,float y,bool centred,const assets::OriginalUiFont50KG& font,const assets::Texture48B& atlas) noexcept {
        if(bytes.empty()||bytes.size()>capacity||bytes.find('\0')!=std::string::npos)return false;
        game::OriginalUiTextInput50KG input{&font,&atlas,reinterpret_cast<const std::uint8_t*>(bytes.data()),bytes.size(),0,0};
        game::OriginalUiTextLayout50KG measured;game::OriginalUiTextDiagnostic50KG td;
        std::int32_t width=0,height=0;
        if(!game::layoutOriginalUiText50KG(input,measured,td)||
           !game::roundOriginalUiTextMetric50KG(measured.widthBeforeScaleAndRounding,1,width)||
           !game::roundOriginalUiTextMetric50KG(measured.firstGlyphHeightBeforeScaleAndRounding,1,height))return false;
        float px=x,py=y;
        if(centred){std::int32_t cx=0,cy=0;
            if(!game::roundOriginalUiTextMetric50KG(exitRounded(x-exitRounded(float(width)/2.f)),1,cx)||
               !game::roundOriginalUiTextMetric50KG(exitRounded(y-exitRounded(float(height)/2.f)),1,cy))return false;
            px=float(cx);py=float(cy);}
        input.localX=px;input.localY=py;
        return game::layoutOriginalUiText50KG(input,out,td);
    }
    static bool exitDepth(const OriginalUiMainVideoOwner50KG& video,float assigned,float& z) noexcept {
        // Same SUB.S/FTOI0 rule as the Main draw's packetDepth (integer depths only).
        if(!std::isfinite(assigned)||(assigned>=0&&std::floor(assigned)!=assigned)||video.packetZMaximum<=0)return false;
        const float value=assigned<0?-assigned:float(video.packetZMaximum)-assigned;
        if(!std::isfinite(value)||value<0||value>16777215.f)return false;
        z=float(static_cast<std::uint32_t>(value));return true;
    }
    // Cold preparation from Main's own loaded resources. Any refusal leaves the row
    // unavailable (logged once); it never faults or changes the original Main.
    void prepareExitGame() noexcept {
        auto& e=exitGame;const char* why=nullptr;
        try {
            auto& r=*cold.resources;const auto& m=manager();
            const auto* materials=r.materials();const auto* images=r.images();const auto* textures=r.textures();
            const auto* video=r.video();
            const auto* d8=m.textDescriptor(8);const auto* d9=m.textDescriptor(9);
            const auto* g9=r.textGeometry(9);
            const auto material=[&](const char* name,bool textured)->std::uint32_t {
                const auto i=r.materialIndex(name);
                if(!materials||i>=materials->size()||(*materials)[i].animated||
                   (*materials)[i].images.size()!=(textured?1u:0u))return ~0u;
                return i;};
            const auto image=[&](std::uint32_t m)->const assets::Texture48B* {
                const auto* t=textures?textures->texture((*materials)[m].images[0]):nullptr;
                return t&&t->texture.width&&t->texture.height?&t->texture:nullptr;};
            if(!materials||!images||!textures||!video)why="resources";
            else if(!d8||!d9||!g9||d9->lines.size()!=1||g9->lines().size()!=1||
                    d9->textSource==assets::OriginalUiTextSource50KG::dynamic)why="row_template";
            else if(d9->y-d8->y<=0||d9->y-d8->y>64)why="row_pitch";
            if(!why){
                // Row: host copy of the slot-5 row descriptor, one row pitch lower.
                const auto pitch=d9->y-d8->y;
                e.rowMaterial=r.widgetMaterial(9);
                if(e.rowMaterial>=materials->size()||(*materials)[e.rowMaterial].images.size()!=1||
                   (*materials)[e.rowMaterial].animated)why="row_material";
                else {
                    const auto index=(*materials)[e.rowMaterial].images[0];
                    const auto* font=images->font(index);const auto* atlas=image(e.rowMaterial);
                    e.rowDescriptor=*d9;e.rowDescriptor.lines={"EXIT GAME"};e.rowDescriptor.y=d9->y+pitch;
                    game::OriginalUiTextGeometryDiagnostic50KG gd;
                    if(!font||!atlas||!e.row.prepare(e.rowDescriptor,*font,*atlas,gd)||e.row.lines().size()!=1)why="row_glyphs";
                    else if(e.row.lines()[0].position.positionedY+float(e.row.lines()[0].measuredHeight)>=378.f)
                        why="row_below_prompts"; // keep clear of the base prompt icons at y378
                    else if(!exitDepth(*video,d9->depth,e.rowZ)||!exitDepth(*video,1,e.arrowZ))why="row_depth";
                }
            }
            if(!why){
                e.arrowRight=r.materialIndex("ui/textures/arrwrght");e.arrowLeft=r.materialIndex("ui/textures/arrwleft");
                if(e.arrowRight>=materials->size()||e.arrowLeft>=materials->size()||
                   (*materials)[e.arrowRight].images.empty()||(*materials)[e.arrowLeft].images.empty())why="arrows";
            }
            if(!why){
                // Dialog: the 47C popup's own materials (dim, overlay, font, cross, triangle).
                e.dim=material("ui/textures/dimpopup",false);e.overlay=material("ui/textures/overlay1popup",true);
                e.font=material("font/venus14popup",true);e.cross=material("ui/textures/icon_x_popup",true);
                e.triangle=material("ui/textures/icon_tri_popup",true);
                const auto popup=r.controllerPopup();
                const auto* geometry=popup?popup->geometry():nullptr;
                if(e.dim==~0u||e.overlay==~0u||e.font==~0u||e.cross==~0u||e.triangle==~0u)why="dialog_materials";
                else if(!geometry||!geometry->prepared())why="dialog_labels";
                else {
                    const auto* font=images->font((*materials)[e.font].images[0]);const auto* atlas=image(e.font);
                    const auto* cross=image(e.cross);const auto* triangle=image(e.triangle);const auto* overlay=image(e.overlay);
                    // garagePurchaseTexts: [1] = LOC9, [2] = LOC10, the 47C's prepared labels.
                    const auto& labels=geometry->garagePurchaseTexts();
                    if(!font||!atlas||!cross||!triangle||!overlay)why="dialog_images";
                    else if(!exitText(e.text[0],"EXIT GAME?",60,320,204,true,*font,*atlas)||
                            !exitText(e.text[1],labels[1].text,40,412,257,false,*font,*atlas)||
                            !exitText(e.text[2],labels[2].text,40,232,257,false,*font,*atlas))why="dialog_glyphs";
                    else {
                        e.crossWidth=float(cross->width);e.crossHeight=float(cross->height);
                        e.triangleWidth=float(triangle->width);e.triangleHeight=float(triangle->height);
                        e.overlayReciprocalWidth=exitRounded(1.f/float(overlay->width));
                        e.overlayReciprocalHeight=exitRounded(1.f/float(overlay->height));
                    }
                }
            }
        }catch(...){why="allocation";}
        e.prepared=!why;
        if(why)std::fprintf(stderr,"pc_native_exit_game row=unavailable reason=%s\n",why);
        else std::fprintf(stderr,"pc_native_exit_game row=ready x=%.0f y=%.0f\n",
            double(e.row.lines()[0].position.positionedX),double(e.row.lines()[0].position.positionedY));
        publishExitGame();
    }
    struct ExitEnabledSlots {std::uint16_t first=0,last=0;};
    ExitEnabledSlots exitEnabledSlots(const game::OriginalUiNavigation50KG& nav) const noexcept {
        ExitEnabledSlots out;const auto& controls=nav.page->groups[1].controls;
        for(std::size_t i=0;i<controls.size()&&i<5;++i){const auto c=controls[i];
            if(c<0||static_cast<std::size_t>(c)>=nav.controls.size())continue;
            const auto& state=nav.controls[static_cast<std::size_t>(c)];
            if(!state.enabled||state.blocked)continue;
            if(!out.first)out.first=static_cast<std::uint16_t>(i+1);
            out.last=static_cast<std::uint16_t>(i+1);
        }
        return out;
    }
    bool exitControl(std::uint16_t slot,game::OriginalUiControlOperation50KG op,std::uint32_t pc) noexcept {
        // One original command on one existing Group 1 control, through the manager's own
        // preflighted batch (the same deselect/select the original movement issues).
        const auto* nav=manager().navigation(1);
        if(!nav||!nav->page||nav->page->groups.size()<2||!slot||slot>nav->page->groups[1].controls.size())return false;
        const auto control=nav->page->groups[1].controls[slot-1];
        if(control<0||static_cast<std::size_t>(control)>=nav->page->controls.size())return false;
        game::OriginalUiCommands50KG commands;
        commands.values[0]={static_cast<std::uint16_t>(control),op,false,pc};commands.count=1;
        return widgetOkay(manager().applyCommands(1,commands,callbackFault.widget));
    }
    bool exitSound(std::uint8_t id,Status& status) noexcept {
        // Refused before any effect (not a fault): the coordinator maps this to "unavailable".
        if(cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&(!cold.audio||!cold.audio->canPlayMenuEffect(id))){
            status=Status::unavailableDestination;return false;}
        return true;
    }
    // Returns true when the EXIT GAME addition consumed this input (status is the result).
    bool exitInput(const OriginalUiMainSessionInput50KG& in,const game::OriginalUiMainContext50KG& context,
        Status& status) noexcept {
        auto& e=exitGame;
        if(!e.prepared)return false;
        if(e.confirmed||e.requested){status=Status::noChange;return true;} // closing to exit
        if(e.dialog.active3CC){
            // Modal, like the 47C: only Confirm4 and Back6 on controller 0 act.
            game::OriginalUiControllerPopupPlan50KG plan;
            if(game::OriginalUiControllerPopup50KG::input(e.dialog,in.controller,in.originalInput,true,plan)!=
               game::OriginalUiControllerPopupStatus50KG::ok){fault(Status::unsupportedState,0x2062D0);status=Status::unsupportedState;return true;}
            status=Status::noChange;
            if(!plan.accepted)return true;
            if(plan.soundRequested&&!exitSound(plan.soundId,status))return true;
            e.dialog=plan.state;
            if(plan.soundRequested&&!audioEffect(plan.soundId)){status=Status::audio;return true;}
            e.confirmed=e.dialog.result3D8==2;
            std::fprintf(stderr,"pc_native_exit_game dialog=%s\n",e.confirmed?"confirm":"cancel");
            publishExitGame();status=Status::ok;return true;
        }
        if(in.controller!=0||context.popupActive||context.sourceInputBlocked||
           cold.application->controllerPopupState.active3CC)return false;
        if(in.originalInput!=2&&in.originalInput!=3&&in.originalInput!=4)return false;
        const auto* nav=manager().navigation(1);
        if(!nav||!nav->page||nav->page->groups.size()<2||!nav->groups[1].enabled)return false;
        const auto slots=exitEnabledSlots(*nav);const auto selected=nav->groups[1].selectedSlot;
        if(!slots.first)return false;
        if(e.rowSelected){
            if(in.originalInput==4){
                // Open the dialog exactly as a Confirm+Cancel 47C Start would (2060A0).
                game::OriginalUiControllerPopupPlan50KG plan;
                if(game::OriginalUiControllerPopup50KG::startController2(e.dialog,plan)!=game::OriginalUiControllerPopupStatus50KG::ok){
                    fault(Status::unsupportedState,0x2060A0);status=Status::unsupportedState;return true;}
                plan.state.hasCancel3D4=true;
                if(!game::OriginalUiControllerPopup50KG::valid(plan.state)){fault(Status::unsupportedState,0x2060A0);status=Status::unsupportedState;return true;}
                if(!exitSound(1,status))return true;
                e.dialog=plan.state;
                if(!audioEffect(1)){status=Status::audio;return true;}
                std::fprintf(stderr,"pc_native_exit_game dialog=open\n");
                publishExitGame();status=Status::ok;return true;
            }
            // Leave the row: Next wraps to the first enabled row, Previous to the last.
            const bool forward=in.originalInput==3;const auto target=forward?slots.first:slots.last;
            if(!exitSound(forward?4:3,status))return true;
            bool okay=false;
            if(target==selected)okay=exitControl(selected,game::OriginalUiControlOperation50KG::select,forward?0x2A518C:0x2A5244);
            else okay=widgetOkay(manager().moveControl(1,1,forward,callbackFault.widget));
            if(!okay){fault(Status::widget,0x2A5110);status=Status::widget;return true;}
            e.rowSelected=false;clearOriginalUiMainExitRowOwner50XG(&collector);
            std::fprintf(stderr,"pc_native_exit_game select=original slot=%u\n",unsigned(manager().navigation(1)->groups[1].selectedSlot));
            publishExitGame();status=audioEffect(forward?4:3)?Status::ok:Status::audio;return true;
        }
        if(in.originalInput==4)return false;
        const bool forward=in.originalInput==3;
        if(selected!=(forward?slots.last:slots.first))return false; // ordinary original move
        if(!exitSound(forward?4:3,status))return true;
        if(!exitControl(selected,game::OriginalUiControlOperation50KG::deselect,forward?0x2A5110:0x2A51D0)){
            fault(Status::widget,0x2A5110);status=Status::widget;return true;}
        e.rowSelected=true;setOriginalUiMainExitRowOwner50XG(&collector);
        std::fprintf(stderr,"pc_native_exit_game select=row from_slot=%u\n",unsigned(selected));
        publishExitGame();status=audioEffect(forward?4:3)?Status::ok:Status::audio;return true;
    }
    static game::OriginalUiOverlayQuad50KG exitQuad(const char* name,float x,float y,float w,float h,
        float u0,float v0,float u1,float v1,std::array<std::uint8_t,4> color,std::uint32_t pc) noexcept {
        game::OriginalUiOverlayQuad50KG q;q.materialName=name;q.sourcePC=pc;q.authoredDepth=1;q.sourcePacketColor=color;
        const float right=exitRounded(x+exitRounded(w*1.f)),bottom=exitRounded(y+exitRounded(h*1.f));
        q.vertices={{{x,y,u0,v0},{right,y,u1,v0},{x,bottom,u0,v1},{right,bottom,u1,v1}}};return q;
    }
    bool exitGlyphs(const game::OriginalUiTextLayout50KG& glyphs,std::uint32_t material,float z,
        std::array<std::uint8_t,4> color,OriginalUiMainDrawSegment50KG segment,std::uint32_t pc,
        OriginalUiMainDrawDiagnostic50KG& d) noexcept {
        if(glyphs.count>glyphs.quads.size())return false;
        for(std::size_t i=0;i<glyphs.count;++i){const auto& g=glyphs.quads[i].vertices; // TL,TR,BR,BL
            game::OriginalUiOverlayQuad50KG q;q.sourcePC=pc;q.authoredDepth=1;q.sourcePacketColor=color;
            q.vertices={{{g[0].x,g[0].y,g[0].u,g[0].v},{g[1].x,g[1].y,g[1].u,g[1].v},
                         {g[3].x,g[3].y,g[3].u,g[3].v},{g[2].x,g[2].y,g[2].u,g[2].v}}}; // ScreenTexture TL,TR,BL,BR
            if(!collector.appendOverlay(q,material,z,segment,0,d))return false;
        }
        return true;
    }
    // After projectMainPage (same point as the original row arrows).
    bool drawExitRow(OriginalUiMainDrawDiagnostic50KG& d) noexcept {
        const auto& e=exitGame;
        if(!e.prepared||!state.mainPageShown)return true;
        // Mirror the Options row (widget 8): visibility, fade alpha and colours.
        const auto* widget=manager().textWidget(8);const auto* line=widget?widget->line(0):nullptr;
        const auto* descriptor=widget?widget->descriptor():nullptr;
        if(!widget||!line||!descriptor||!widget->shown()||!line->overlayEnabled||!line->blinkOn)return true;
        const auto color=e.rowSelected?(descriptor->hasHighlightColor?descriptor->highlightColor:line->color):
            (descriptor->hasHighlightColor?widget->widgetColor():line->color);
        const auto packet=game::originalUiTextPacketColor50KG(color,static_cast<std::int16_t>(line->alphaCurrent));
        if(packet[3]>128)return true;
        const auto& row=e.row.lines()[0];
        if(!exitGlyphs(row.glyphs,e.rowMaterial,e.rowZ,packet,OriginalUiMainDrawSegment50KG::mainPage,0x2e0a30,d))return false;
        if(!e.rowSelected)return true;
        const auto& materials=*cold.resources->materials();
        const auto* right=cold.resources->textures()->texture(materials[e.arrowRight].images[0]);
        const auto* left=cold.resources->textures()->texture(materials[e.arrowLeft].images[0]);
        if(!right||!left)return false;
        // Slot5's arrow rule (1E6410..684) placed around this row's own line.
        const game::OriginalUiMainArrowInput50KG input{5,row.position.positionedX,row.position.positionedY,row.measuredWidth,
            right->texture.width,right->texture.height,left->texture.width,left->texture.height};
        game::OriginalUiMainArrows50KG arrows;
        return game::layoutOriginalUiMainArrows50KG(input,arrows)&&
            collector.appendOverlay(arrows.beforeText,e.arrowRight,e.arrowZ,OriginalUiMainDrawSegment50KG::mainPage,0,d)&&
            collector.appendOverlay(arrows.afterText,e.arrowLeft,e.arrowZ,OriginalUiMainDrawSegment50KG::mainPage,0,d);
    }
    // After the Main particles, before drain: the same packets in the same order as 2064C0.
    bool drawExitDialog(game::OriginalUiControllerPopupState50KG& next,OriginalUiMainDrawDiagnostic50KG& d) noexcept {
        auto& e=exitGame;next=e.dialog;
        if(!e.prepared||!e.dialog.active3CC)return true;
        game::OriginalUiControllerPopupRender50KG render;
        if(game::OriginalUiControllerPopup50KG::render(e.dialog,render)!=game::OriginalUiControllerPopupStatus50KG::ok)return false;
        next=render.state;
        constexpr float z=16777214.f; // 47C packet depth (Video+60 minus depth1)
        const auto text=game::originalUiTextPacketColor50KG(0x00cccccc,255);
        const auto segment=OriginalUiMainDrawSegment50KG::controllerPopup;
        if(render.drawDim&&!collector.appendOverlay(exitQuad("ui/textures/dimpopup",0,0,640,448,0,0,0,0,{{12,12,12,79}},0x206DCC),
            e.dim,z,segment,0,d))return false;
        if(render.drawMessage&&!exitGlyphs(e.text[0],e.font,z,text,segment,0x206DFC,d))return false;
        if(render.drawConfirm&&(!collector.appendOverlay(exitQuad("ui/textures/icon_x_popup",380,247,e.crossWidth,e.crossHeight,
            0,0,1,1,{{127,127,127,63}},0x2072AC),e.cross,z,segment,0,d)||!exitGlyphs(e.text[1],e.font,z,text,segment,0x2072B4,d)))return false;
        if(render.drawCancel&&(!collector.appendOverlay(exitQuad("ui/textures/icon_tri_popup",200,247,e.triangleWidth,e.triangleHeight,
            0,0,1,1,{{127,127,127,63}},0x2072C8),e.triangle,z,segment,0,d)||!exitGlyphs(e.text[2],e.font,z,text,segment,0x2072D0,d)))return false;
        if(render.drawStrips){
            const float u32=exitRounded(32.f*e.overlayReciprocalWidth),v32=exitRounded(32.f*e.overlayReciprocalHeight);
            const float v18=exitRounded(18.f*e.overlayReciprocalHeight);
            for(std::size_t i=0;i<3;++i){const auto& s=render.strips[i];
                if(!collector.appendOverlay(exitQuad("ui/textures/overlay1popup",s.x,s.y,s.width,s.height,
                    0,i==0?0:i==1?v32:v18,i==0?1:u32,i==1?0:i==0?1:v32,{{121,66,13,63}},0x2072F4),
                    e.overlay,z,segment,0,d))return false;
            }
        }
        return true;
    }
    // ---- end PC-NATIVE ADDITION ----

    game::OriginalUiTransitionOwner50KG& lifecycle() noexcept {
        return cold.externalTransition?*cold.externalTransition:transition;
    }
    const game::OriginalUiTransitionOwner50KG& lifecycle() const noexcept {
        return cold.externalTransition?*cold.externalTransition:transition;
    }
    bool currentMainLifecycle() const noexcept {
        const auto t=lifecycle().snapshot();return t.bound && t.current.screen==4 && t.current.page==1;
    }
    // Main1E5D10/1E6290's source-only transient debug-message state. The input
    // callbacks supported here cannot start the omitted debug editor.
    float messageTime744=0;
    bool messageActive74C=false;
    std::array<char,64> message704{};

    game::OriginalUiMainWidgetManager50KG& manager() noexcept {return *cold.resources->manager();}
    void fault(Status s,std::uint32_t pc) noexcept {
        if(!state.faulted){state.faulted=true;callbackFault.status=s;callbackFault.sourcePC=pc;}
    }
    bool operational() const noexcept {
        return dependenciesPrepared && !state.faulted && !state.quitRequested && !sourceClosing;
    }
    game::OriginalUiMainWidgetPhaseInput50KG phaseInput() const noexcept {
        return {cold.profile->known,cold.profile->savedSlot6E0,cold.profile->availability11018};
    }
    void widgetPhase(Phase p,std::uint32_t pc) noexcept {
        if(!widgetOkay(manager().applyMainWidgetPhase(p,phaseInput(),callbackFault.widget)))fault(Status::widget,pc);
    }
    bool audioEffect(std::uint32_t id) noexcept {
        if(cold.audioMode==OriginalUiMainAudioMode50KG::muted){++state.audio.suppressedByMutedMode;return true;}
        if(!cold.audio || !cold.audio->ready() || !cold.audio->playMenuEffect(id)){
            ++state.audio.failed;fault(Status::audio,0x292360);return false;
        }
        ++state.audio.submitted;return true;
    }
    void startMusic() noexcept {
        // Main1E68A4..D0 only starts menu loop9 outside these original modes.
        if(cold.application->musicState44==4 || cold.application->musicState48==2){
            ++state.audio.suppressedBySourceGate;return;
        }
        if(cold.audioMode==OriginalUiMainAudioMode50KG::muted){++state.audio.suppressedByMutedMode;return;}
        if(!cold.audio || !cold.audio->ready() || !cold.audio->startMainLoop9()){
            ++state.audio.failed;fault(Status::audio,0x234900);return;
        }
        ++state.audio.submitted;
    }
    static bool apertureAvailable(void* p) noexcept {
        auto& s=*static_cast<Storage*>(p);
        return s.operational() && s.cold.resources->exitText() && s.cold.resources->exitText()->prepared();
    }
    static bool admit(void* p,const game::OriginalUiScreenFactory50KG& s,
        const game::OriginalUiPageFactory50KG& page,game::OriginalUiTransitionAdmission50KG kind) noexcept;
    static game::OriginalUiTransitionCompletion50KG poll(void* p,game::OriginalUiTransitionRoute50KG route,
        game::OriginalUiTransitionPhase50KG phase) noexcept {
        auto& self=*static_cast<Storage*>(p);
        if(!self.operational())return game::OriginalUiTransitionCompletion50KG::unavailable;
        return self.aperture.pollCompletion(route,phase);
    }
    static void notify(void* p,const game::OriginalUiTransitionNotification50KG& event) noexcept {
        auto& s=*static_cast<Storage*>(p);
        if(!s.operational())return;
        if(event.route.screen!=4 || event.route.page!=1){s.fault(Status::unsupportedState,event.sourcePC);return;}
        switch(event.kind) {
        case Notice::requestedPage90:
            s.widgetPhase(Phase::requestedPage90,event.sourcePC);s.state.mainPageShown=true;break;
        case Notice::previousPage94:
            s.resetExitGame(); // PC-NATIVE ADDITION (EXIT GAME)
            s.widgetPhase(Phase::previousPage94,event.sourcePC);s.state.mainPageShown=false;break;
        case Notice::pageEntry74: {
            s.resetExitGame(); // PC-NATIVE ADDITION (EXIT GAME)
            if(!s.popupAttached){
                const auto owner=s.cold.resources->controllerPopup();
                if(!owner||!owner->loaded()||owner->poisoned()||
                   (s.cold.application->controllerPopupResources&&s.cold.application->controllerPopupResources!=owner)||
                   s.cold.application->controllerPopupState.active3CC){s.fault(Status::unsupportedState,0x207330);break;}
                if(const auto shared=s.cold.application->sharedDialogs){OriginalUiSharedDialogs50KG::Diagnostic sd;
                    if(shared->controller47C_.resources!=owner||!shared->reappendMainEntry1D7CF4(sd)){
                        s.fault(Status::unsupportedState,sd.sourcePC?sd.sourcePC:0x1D7CF4);break;}
                }else{
                    game::OriginalUiControllerPopupPlan50KG plan;
                    if(game::OriginalUiControllerPopup50KG::attach(s.cold.application->controllerPopupState,plan)!=
                       game::OriginalUiControllerPopupStatus50KG::ok){s.fault(Status::unsupportedState,0x207330);break;}
                    s.cold.application->controllerPopupResources=owner;s.cold.application->controllerPopupState=plan.state;
                }
                s.popupAttached=true;
            }
            auto& profile=*s.cold.profile;
            if(profile.mode34!=4){profile.previousMode38=profile.mode34;profile.mode34=4;}
            s.state.promptMask42C=0;
            s.widgetPhase(Phase::entry74,event.sourcePC);
            s.message704.fill(0);s.messageTime744=0;s.messageActive74C=false;s.state.debugChord748=false;
            // 1E5DD8 addresses APP->UI+47C's result, not the scene field.
            s.cold.application->controllerPopupState.result3D8=0;
            break;
        }
        case Notice::managerEntry84:
            // Exact Main vtable target2A4330 consists solely of return.
            break;
        case Notice::beginTransition7C:
            if(!apertureOkay(s.aperture.beginTransition7C(event.phase)))s.fault(Status::aperture,event.sourcePC);
            break;
        case Notice::pageAuxiliary1C:s.state.inputContainerShown=false;break;
        case Notice::pageAuxiliary24:s.state.inputContainerShown=true;break;
        }
    }
    static void apertureHook(void* p,const game::OriginalUiApertureHook50KG& event) noexcept {
        auto& s=*static_cast<Storage*>(p);
        if(!s.operational())return;
        switch(event.kind){
        case Hook::openingAC:s.resetExitGame(); // PC-NATIVE ADDITION (EXIT GAME)
            s.widgetPhase(Phase::openingAC,event.callPC);if(!s.state.faulted)s.startMusic();break;
        case Hook::openedB0:s.state.promptMask42C=1;s.widgetPhase(Phase::openedB0,event.callPC);break;
        case Hook::closingB4:
            s.resetExitGame(); // PC-NATIVE ADDITION (EXIT GAME)
            s.state.promptMask42C=0;s.widgetPhase(Phase::closingB4,event.callPC);
            if(s.state.faulted)break;
            // Source1E6A34 ->234440(stream,1.0f), after real widget effects.
            if(s.cold.audioMode==OriginalUiMainAudioMode50KG::muted)++s.state.audio.suppressedByMutedMode;
            else if(!s.cold.audio || !s.cold.audio->ready() || !s.cold.audio->fadeMain(1.f)){
                ++s.state.audio.failed;s.fault(Status::audio,0x1E6A34);
            }else ++s.state.audio.submitted;
            break;
        case Hook::closingThresholdB8: // Main1E6A50 is an actual return.
        case Hook::closedBC: // Shared1D7A00 is an actual return.
            break;
        case Hook::screenExitObject440:
            if(!s.rendering || !s.collector.projectExitText(event,s.callbackFault.draw))
                s.fault(Status::draw,event.callPC);
            break;
        }
    }
    static void overlay(void* p,std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& rectangle) noexcept {
        auto& s=*static_cast<Storage*>(p);
        if(!s.operational())return;
        if(!s.rendering || !s.collector.projectAperture(ordinal,rectangle,s.callbackFault.draw))s.fault(Status::draw,0x1D798C);
    }
    bool updateWidgets(const std::vector<std::uint16_t>& ids) noexcept {
        for(auto id:ids)if(manager().widgetState(id).kind==game::OriginalUiMainWidgetKind50KG::text)
            if(!widgetOkay(manager().updateText(id,true,callbackFault.widget))){fault(Status::widget,0x141ED0);return false;}
        return true;
    }
};
bool OriginalUiMainSession50KG::Storage::admit(void* p,const game::OriginalUiScreenFactory50KG& s,
    const game::OriginalUiPageFactory50KG& page,game::OriginalUiTransitionAdmission50KG kind) noexcept {
    auto& self=*static_cast<Storage*>(p);
    // Exact factory identities prevent a route-number-only readiness claim.
    return self.operational() && kind==game::OriginalUiTransitionAdmission50KG::initialPage &&
        &s==game::findOriginalUiScreenFactory50KG(4) && &page==game::findOriginalUiBoundedPageFactory50KG(4,1);
}
OriginalUiMainSession50KG::OriginalUiMainSession50KG() noexcept=default;
OriginalUiMainSession50KG::~OriginalUiMainSession50KG(){
    // Shared retains this Session facade as part of its exact invocation.
    // Leaking only Storage cannot preserve the facade. Refuse destruction
    // until actual explicit retirement instead of leaving a dangling authority.
    if(storage_&&storage_->collector.menuShared_&&storage_->collector.sourceTeardownPending())std::terminate();
    // Existing non-Shared sealed-source fallback retains its borrowed backing.
    // It is not a retirement receipt and never preserves a Session facade.
    if(storage_&&storage_->collector.sourceTeardownPending())storage_.release();
}
Status OriginalUiMainSession50KG::bind(const OriginalUiMainSessionCold50KG& cold,
    OriginalUiMainSessionDiagnostic50KG& d) noexcept {
    d={};if(storage_)return result(d,Status::alreadyBound);
    // External mode cold-prepares this page before its actual manager exists.
    // Adopting an already-bound owner would skip original initial callbacks
    // and pair fresh page state with an unrelated live/terminal lifecycle.
    if(cold.externalTransition && cold.externalTransition->snapshot().bound)
        return result(d,Status::unsupportedState,0x2A3C10);
    if(!cold.resources || !cold.resources->loaded() || !cold.resources->manager() || !cold.gpu ||
        !cold.mainArchive || !cold.mainArchive->loaded() || !cold.mainArchive->identity() ||
        cold.mainArchive->identity()->id!=assets::OriginalUiArchiveId50KG::mainMenu)
        return result(d,Status::resource);
    if(!cold.profile || !cold.profile->known || cold.profile->savedSlot6E0<1 || cold.profile->savedSlot6E0>5 ||
        !cold.application || !cold.application->known || (cold.application->globalFlags228&4u) ||
        cold.application->transportE00!=-1)return result(d,Status::unsupportedState);
    if(cold.audioMode!=OriginalUiMainAudioMode50KG::enabled && cold.audioMode!=OriginalUiMainAudioMode50KG::muted)
        return result(d,Status::invalidInput);
    if(cold.audioMode==OriginalUiMainAudioMode50KG::enabled && (!cold.audio || !cold.audio->ready()))return result(d,Status::audio);
    // Requiring nullptr for deliberate mute prevents accidental second writer
    // ownership and makes suppressed audio receipts unambiguous.
    if(cold.audioMode==OriginalUiMainAudioMode50KG::muted && cold.audio)return result(d,Status::invalidInput);
    try {
        auto next=std::make_unique<Storage>();next->cold=cold;next->state.audioMode=cold.audioMode;
        if(!next->collector.bind(cold.resources,cold.gpu,d.draw))return result(d,Status::draw);
        assets::OriginalUiMainCameraConfig50KG config;assets::OriginalUiPageDiagnostic50KG parse;
        if(!assets::readOriginalUiMainCameraConfig50KG(*cold.resources->manager()->uiDat(),config,parse) ||
            game::prepareOriginalUiMainCamera50KG(config,next->camera)!=game::OriginalUiMainCameraStatus50KG::ok)
            return result(d,Status::camera);
        if(!next->scene.prepare(*cold.mainArchive,d.scene))return result(d,Status::scene);
        if(next->fire.initialize(next->scene)!=game::OriginalUiMainFireStatus50KG::complete)return result(d,Status::particle);
        if(game::prepareOriginalUiMainParticleLighting50KG(next->scene,next->lighting)!=game::OriginalUiMainParticleLightingStatus50KG::complete)
            return result(d,Status::scene);
        if(game::prepareOriginalUiMainParticleRenderState50KG(next->scene,next->camera,next->lighting,next->particleRenderState)!=
            game::OriginalUiMainParticleRenderStateStatus50KG::complete)return result(d,Status::camera);
        const auto* page=cold.resources->manager()->page(1);
        if(!page)return result(d,Status::resource);
        if(page->useVideoAsTexture || page->background!="*INVALID*")
            return result(d,Status::unsupportedState,0x1D63B0);
        std::uint32_t packed=0;
        for(std::size_t i=0;i<3;++i){std::int32_t channel=0;
            if(!game::convertOriginalUiApertureCvtWS50KG(page->backgroundColor[i],channel) || channel<0 || channel>255)
                return result(d,Status::unsupportedState,0x1D6D24);
            packed|=static_cast<std::uint32_t>(channel)<<(16u-8u*static_cast<unsigned>(i));
        }
        next->state.packedBackground430=packed;
        next->dependenciesPrepared=true;
        game::OriginalUiApertureCallbacks50KG a;
        a.context=next.get();a.available=&Storage::apertureAvailable;a.hook=&Storage::apertureHook;a.submitOverlay=&Storage::overlay;
        if(next->aperture.bindMain(cold.mainArchive,a,d.aperture)!=ApertureStatus::ok)return result(d,Status::aperture);
        game::OriginalUiTransitionCallbacks50KG t;
        t.context=next.get();t.admitDestination=&Storage::admit;t.notify=&Storage::notify;t.pollCompletion=&Storage::poll;
        // Standalone callback context is private stable storage. Explicit
        // external mode emits no initial callbacks and owns no second manager.
        if(!cold.externalTransition){
            const auto bound=next->transition.bindInitialMain(t);
            if(bound!=TransitionStatus::ok)return result(d,Status::transition);
        }
        if(next->state.faulted){
            // Initial source callbacks may already have changed shared widget
            // or profile state. Retain the faulted owner, forbid retry, and
            // require fresh resources instead of implying atomic rollback.
            d=next->callbackFault;storage_=std::move(next);return d.status;
        }
        next->state.bound=true;storage_=std::move(next);
        storage_->prepareExitGame(); // PC-NATIVE ADDITION (EXIT GAME); refusal only disables the row
        if(const auto shared=storage_->cold.application->sharedDialogs){
            if(!storage_->collector.bindSharedMenu470(*this,shared)){
                storage_->fault(Status::draw,0x204A00);d=storage_->callbackFault;return d.status;
            }
        }
        return result(d,Status::ok);
    }catch(const std::bad_alloc&){return result(d,Status::allocation);}catch(...){return result(d,Status::resource);}
}
Status OriginalUiMainSession50KG::input(const OriginalUiMainSessionInput50KG& in,
    OriginalUiMainSessionInputReceipt50KG& receipt,OriginalUiMainSessionDiagnostic50KG& d) noexcept {
    d={};receipt={};auto* s=storage_.get();
    if(!s)return result(d,Status::notBound);
    if(s->cold.application->sharedDialogs&&s->cold.application->sharedDialogs->menu470PendingFor(s->collector))
        return result(d,Status::unsupportedState,0x204A00);
    if(s->state.faulted)return result(d,Status::faulted);
    if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation || s->updateAudioPrepared || in.controller>1 || in.originalInput>15)return result(d,Status::invalidInput);
    s->state.framePublished=false;s->pendingRoute={};
    if(s->cold.externalTransition && !s->currentMainLifecycle())return result(d,Status::noChange);
    ++s->inputSerial;
    // Global popup traversal precedes Main; snapshot-active delivery prevents
    // the opening Confirm from being reused as dismissal.
    if(s->cold.application->controllerPopupState.active3CC){
        game::OriginalUiControllerPopupPlan50KG plan;
        if(game::OriginalUiControllerPopup50KG::input(s->cold.application->controllerPopupState,
            in.controller,in.originalInput,true,plan)!=game::OriginalUiControllerPopupStatus50KG::ok)
            return result(d,Status::unsupportedState,0x2062D0);
        if(plan.accepted){
            if(s->cold.application->transportE00!=-1||(plan.soundId!=1&&plan.soundId!=5))return result(d,Status::unsupportedState,0x267BE0);
            if(plan.soundRequested&&s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&
               (!s->cold.audio||!s->cold.audio->canPlayMenuEffect(plan.soundId)))return result(d,Status::audio,plan.soundPC);
            s->cold.application->controllerPopupState=plan.state;
            if(plan.soundRequested&&!s->audioEffect(plan.soundId)){d=s->callbackFault;return d.status;}
            receipt.popupChanged=true;receipt.popupSourcePC=plan.sourcePC;
        }
    }
    receipt.containerReached=s->state.mainPageShown && s->state.inputContainerShown;
    if(!receipt.containerReached || !s->aperture.snapshot().inputEnabled268)return result(d,Status::noChange);
    if(s->cold.application->transportE00!=-1)return result(d,Status::unsupportedState,0x267BE0);
    s->inOperation=true;
    receipt.pageCallbackRan=true;receipt.pageAccepted=true; //1E6284 unconditional1
    game::OriginalUiMainContext50KG context;
    context.popupActive=in.popupActive||s->cold.application->controllerPopupState.active3CC;context.sourceInputBlocked=in.sourceInputBlocked || s->state.debugChord748;
    context.secondControllerPresent=in.secondControllerObservation==WindowControllerPresence50DQ::Present;
    // Page-only dispatch cannot certify a destination. Actual labels remain selectable.
    // The coordinator may inspect an unaccepted exact proposal in external mode.
    Status status=Status::noChange;
    // PC-NATIVE ADDITION (EXIT GAME): the host row/dialog takes the inputs it owns before
    // the original page callback; everything else reaches the original Main unchanged.
    if(s->exitInput(in,context,status)){
        s->inOperation=false;receipt.status=status;
        if(s->state.faulted){d=s->callbackFault;return d.status;}
        return result(d,status);
    }
    if(in.originalInput==2 || in.originalInput==3 || in.originalInput==4){
        const auto ws=s->manager().dispatchMain(in.controller,static_cast<game::OriginalUiMainInput50KG>(in.originalInput),
            context,receipt.decision,d.widget);
        if(ws==WidgetStatus::unavailable){
            status=Status::unavailableDestination;
            const auto& decision=receipt.decision;
            if(decision.action==game::OriginalUiMainAction50KG::secondControllerRequired&&
               in.secondControllerObservation==WindowControllerPresence50DQ::Absent&&
               decision.selectedSlot==2&&decision.originalGameMode==1&&decision.originalPlayerBits==0x40&&
               decision.targetScreen==5&&decision.routeArgument==1&&decision.sourcePC==0x1E5F2C&&
               !s->cold.application->controllerPopupState.active3CC){
                const auto owner=s->cold.resources->controllerPopup();
                const auto* navigation=s->manager().navigation(1);
                if(navigation&&navigation->groups.size()>1&&owner&&owner->loaded()&&!owner->poisoned()&&owner==s->cold.application->controllerPopupResources&&
                   (s->cold.audioMode==OriginalUiMainAudioMode50KG::muted||
                    (s->cold.audio&&s->cold.audio->canPlayMenuEffect(1)))){
                    game::OriginalUiControllerPopupPlan50KG plan;
                    if(game::OriginalUiControllerPopup50KG::startController2(s->cold.application->controllerPopupState,plan)!=
                       game::OriginalUiControllerPopupStatus50KG::ok)s->fault(Status::unsupportedState,0x1E60DC);
                    else {
                        if(const auto shared=s->cold.application->sharedDialogs){OriginalUiSharedDialogs50KG::Diagnostic sd;
                            if(!shared->writeController47CLabels2060A0(plan.state,sd)){
                                s->fault(Status::unsupportedState,sd.sourcePC);s->inOperation=false;
                                receipt.status=Status::unsupportedState;return result(d,Status::unsupportedState,sd.sourcePC);}}
                        s->cold.application->controllerPopupState=plan.state; //1E60DC
                        s->cold.profile->savedSlot6E0=s->manager().navigation(1)->groups[1].selectedSlot; //1E6184 reread ->1E61A4
                        if(s->audioEffect(1)){receipt.popupChanged=true;receipt.popupSourcePC=0x1E60DC;status=Status::ok;}
                    }
                }
            }
        }else if(!widgetOkay(ws)){s->fault(Status::widget,0x1E5E00);status=Status::widget;}
        else if(receipt.decision.action==game::OriginalUiMainAction50KG::changedSelection){
            status=s->audioEffect(in.originalInput==2?3:4)?Status::ok:Status::audio;
            // Source267BE0 is reached here; admitted transportE00=-1 takes
            // the exact no-work return. Never rename it an idle reset.
        }
    }
    // Snapshot-selected delivery occurs AFTER the actual page callback, even
    // if its own popup/controller/sourceInputBlocked gates suppressed action.
    game::OriginalUiMainMovementDelivery50KG delivery{true,true,true};
    const auto movement=s->manager().dispatchMainMovement(in.controller,
        static_cast<assets::OriginalUiMovementInput50KG>(in.originalInput),delivery,receipt.movement,d.widget);
    if(!widgetOkay(movement)){s->fault(Status::widget,0x2A73F0);status=Status::widget;}
    s->inOperation=false;
    if(s->cold.externalTransition && !s->state.faulted && in.originalInput==4 && receipt.decision.selectedSlot!=2 &&
        (receipt.decision.action==game::OriginalUiMainAction50KG::unavailableMode ||
         receipt.decision.action==game::OriginalUiMainAction50KG::unavailableScreen)){
        const auto t=s->lifecycle().snapshot();
        if(t.bound && t.current.screen==4 && t.current.page==1 &&
            t.lifecycle==game::OriginalUiTransitionLifecycle50KG::idle &&
            t.phase==game::OriginalUiTransitionPhase50KG::none && !t.initialRestartPending && !t.applicationRequestQueued){
            auto proposal=OriginalUiMainRouteProposal50KG{};
            proposal.source=this;proposal.serial=s->inputSerial;proposal.decision=receipt.decision;
            proposal.decision.action=proposal.decision.selectedSlot==5?
                game::OriginalUiMainAction50KG::changePage:game::OriginalUiMainAction50KG::changeScreen;
            proposal.observedLifecycle=t;s->pendingRoute=proposal;receipt.routeProposal=proposal;
            status=Status::routeProposed;
        }
    }
    receipt.status=status;return result(d,status);
}
Status OriginalUiMainSession50KG::prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG& in,
    OriginalUiMainSessionDiagnostic50KG& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->cold.application->sharedDialogs&&s->cold.application->sharedDialogs->menu470PendingFor(s->collector))
        return result(d,Status::unsupportedState,0x204A00);
    if(!s->cold.externalTransition || s->inOperation)return result(d,Status::invalidInput);
    if(s->state.faulted)return result(d,Status::faulted);
    if(s->state.quitRequested)return result(d,Status::quit);
    s->state.framePublished=false;
    if(s->updateAudioPrepared){
        s->fault(Status::unsupportedState,0x234440);d=s->callbackFault;return d.status;
    }
    if(!s->currentMainLifecycle() || !std::isfinite(in.elapsedSeconds) || in.elapsedSeconds<0 ||
        in.elapsedSeconds>1 || (in.paused && in.elapsedSeconds!=0) ||
        (s->cold.application->globalFlags228&4u) || s->cold.application->transportE00!=-1)
        return result(d,Status::invalidInput);
    s->pendingRoute={};s->inOperation=true;s->paused=in.paused;
    s->preparedUpdate=in;s->updateAudioPrepared=true;
    if(s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled){
        s->cold.audio->pause(in.paused);
        if(!s->cold.audio->update(in.elapsedSeconds))s->fault(Status::audio,0x234440);
    }
    s->inOperation=false;
    if(s->state.faulted){d=s->callbackFault;return d.status;}
    return result(d,Status::ok);
}
Status OriginalUiMainSession50KG::update(const OriginalUiMainSessionUpdate50KG& in,
    OriginalUiMainSessionDiagnostic50KG& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->cold.application->sharedDialogs&&s->cold.application->sharedDialogs->menu470PendingFor(s->collector))
        return result(d,Status::unsupportedState,0x204A00);
    if(s->state.faulted)return result(d,Status::faulted);
    if(s->inOperation)return result(d,Status::invalidInput);
    s->state.framePublished=false;
    if(s->cold.externalTransition){
        if(!s->updateAudioPrepared)return result(d,Status::unsupportedState,0x234440);
        // Consume once even when the caller changed arguments or manager. The
        // audio service already observed this frame, so no retry is permitted.
        s->updateAudioPrepared=false;
        if(!s->currentMainLifecycle() || in.elapsedSeconds!=s->preparedUpdate.elapsedSeconds ||
            in.paused!=s->preparedUpdate.paused || in.debugChordHeld!=s->preparedUpdate.debugChordHeld){
            s->fault(Status::invalidInput,0x234440);d=s->callbackFault;return d.status;
        }
    }
    s->pendingRoute={};
    if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation || !std::isfinite(in.elapsedSeconds) || in.elapsedSeconds<0 || in.elapsedSeconds>1 ||
        (in.paused && in.elapsedSeconds!=0))
        return result(d,Status::invalidInput);
    if((s->cold.application->globalFlags228&4u) || s->cold.application->transportE00!=-1){
        if(s->cold.externalTransition){s->fault(Status::unsupportedState,0x267BE0);d=s->callbackFault;return d.status;}
        return result(d,Status::unsupportedState);
    }
    s->inOperation=true;s->paused=in.paused;
    if(const auto shared=s->cold.application->sharedDialogs){
        OriginalUiSharedDialogs50KG::Diagnostic sharedDiagnostic;
        if(!shared->advanceMenu470(*this,s->collector,in.elapsedSeconds,sharedDiagnostic)){
            s->fault(Status::unsupportedState,sharedDiagnostic.sourcePC);s->inOperation=false;d=s->callbackFault;return d.status;
        }
    }else{
        game::OriginalUiControllerPopupPlan50KG popupUpdate;
        if(game::OriginalUiControllerPopup50KG::update(s->cold.application->controllerPopupState,in.elapsedSeconds,popupUpdate)!=
           game::OriginalUiControllerPopupStatus50KG::ok)s->fault(Status::unsupportedState,0x206420);
        else s->cold.application->controllerPopupState=popupUpdate.state;
    }
    // PC-NATIVE ADDITION (EXIT GAME): the host dialog copy saves this tick's delta (206420).
    if(!s->state.faulted&&s->exitGame.dialog.active3CC){
        game::OriginalUiControllerPopupPlan50KG exitUpdate;
        if(game::OriginalUiControllerPopup50KG::update(s->exitGame.dialog,in.elapsedSeconds,exitUpdate)!=
           game::OriginalUiControllerPopupStatus50KG::ok)s->fault(Status::unsupportedState,0x206420);
        else s->exitGame.dialog=exitUpdate.state;
    }
    if(!s->cold.externalTransition && s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled){
        s->cold.audio->pause(in.paused);
        if(!s->cold.audio->update(in.elapsedSeconds)){s->fault(Status::audio,0x234440);}
    }
    // Manager switch runs before child traversal. Each call advances only one
    // switch case; opening geometry itself advances only from render48.
    if(!s->cold.externalTransition){
        const auto transitioned=s->transition.tick();
        if(transitioned!=TransitionStatus::ok && transitioned!=TransitionStatus::noChange &&
            transitioned!=TransitionStatus::pending)s->fault(Status::transition,0x2A3C80);
    }
    const auto* order=s->manager().managerWidgetOrder();
    if(!s->state.faulted)s->updateWidgets(order->ordinaryWidgets);
    // Actual Main entity1E6290 precedes its page-owned input container.
    if(!s->state.faulted && s->state.mainPageShown){
        s->state.debugChord748=in.debugChordHeld[0] && in.debugChordHeld[1] &&
            in.debugChordHeld[2] && in.debugChordHeld[3] && (s->cold.profile->flags11054&1u);
        if(s->messageActive74C)s->messageTime744-=in.elapsedSeconds;
        if(s->messageTime744<=0){s->message704.fill(0);s->messageActive74C=false;s->messageTime744=-1;}
        if(s->aperture.saveMainUpdateDelta38(in.elapsedSeconds)!=ApertureStatus::ok)s->fault(Status::aperture,0x1E63F8);
    }
    if(!s->state.faulted)s->updateWidgets(order->onTopWidgets);
    // 1CF410 appends these actual Main scene subscribers AFTER Pod children:
    // particle manager(-1), borrowed stationary camera(-1), emitter(0x402).
    if(!s->state.faulted){
        s->state.lastParticleAdvance=s->fire.advance(in.elapsedSeconds,in.paused);
        if(!s->state.lastParticleAdvance.okay())s->fault(Status::particle,0x160E40);
    }
    // This bounded camera has no source pose animation or debug override.
    // Pose is invariant. Source render fog changes its clipping planes; the
    // typed particleRenderState is that SAME camera's later render boundary,
    // not another entity. Unconsumed motion metrics are not claimed recovered.
    if(!s->state.faulted){
        s->state.lastFireEmit=s->fire.emitSource15E2A8(in.elapsedSeconds,in.paused,
            s->cold.application->rawRandom308030,s->cold.application->fireSerial433970,
            s->cold.application->fireLazy84C8_);
        if(!s->state.lastFireEmit.okay())s->fault(Status::particle,0x15AEA0);
    }
    s->inOperation=false;
    if(s->state.faulted){d=s->callbackFault;return d.status;}
    ++s->state.updateCount;return result(d,Status::ok);
}
bool OriginalUiMainSession50KG::bindInitialSharedMenu470(OriginalUiSharedDialogs50KG& shared,
    OriginalUiMainSessionDiagnostic50KG& d)noexcept {
    d={};auto* s=storage_.get();
    if(!s||!s->state.bound||!s->operational()||s->inOperation||s->rendering||s->sourceTornDown||
       s->updateAudioPrepared||s->state.updateCount||s->state.renderCount||s->state.framePublished||
       !s->cold.externalTransition||s->cold.externalTransition->snapshot().bound||
       s->cold.application->sharedDialogs.get()!=&shared||!shared.menuTopologyCurrent()||
       shared.update470Count_||shared.update470Pending_||shared.menu470Owner()||shared.menuUpdateFailedOwner_||
       !s->cold.resources||s->cold.resources->controllerPopup()!=shared.controller47C_.resources||
       !s->collector.bindSharedMenu470(*this,s->cold.application->sharedDialogs)){
        result(d,Status::unsupportedState,0x1B7BD0);return false;
    }
    return true;
}
bool OriginalUiMainSession50KG::readSharedMenuRasterInputs(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiMainDraw50KG& draw,OriginalUiMenuRasterInputs50KG& out)const noexcept {
    const auto* s=storage_.get();
    if(!sharedMenuLifetimeCurrent(shared,draw)||s->state.faulted||s->sourceClosing||
       !s->cold.resources->loaded()||!s->cold.gpu||!s->cold.gpu->initialized()||
       !draw.sharedMenuRasterResourcesCurrent(shared,s->cold.resources.get(),s->cold.gpu.get(),
           s->cold.gpu->publicationToken()))return false;
    const auto* video=s->cold.resources->video();
    if(!video||!video->lifetime||video->width!=640||video->height!=448||
       video->colorBits!=32||video->packetZMaximum!=0xFFFFFF||video->zBufferFormat!=0x31)return false;
    OriginalUiMenuRasterInputs50KG next;
    const auto* manager=s->cold.resources->manager();
    const auto* ui=manager?manager->uiDat():nullptr;
    if(!ui||!s->cold.mainArchive||!s->cold.mainArchive->loaded()||
       s->cold.mainArchive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::mainMenu)||
       s->camera.configurationIdentity!=ui->identity()||
       s->camera.originalWidth!=640||s->camera.originalHeight!=448)return false;
    bool memberCurrent=false;
    for(std::size_t i=0;i<s->cold.mainArchive->memberCount();++i){
        const auto member=s->cold.mainArchive->member(i);
        if(member.identity==ui->identity()&&member.identity&&
           std::strcmp(member.identity->name,"UI\\UIMain\\ui.dat")==0){memberCurrent=true;break;}
    }
    if(!memberCurrent)return false;
    next.camera=&s->camera;next.configurationIdentity=s->camera.configurationIdentity;
    next.viewport={{0,0,s->camera.originalWidth,s->camera.originalHeight}};
    next.session=this;next.resources=s->cold.resources.get();next.videoLifetime=video->lifetime;
    next.cpu=s->cold.resources->textures();next.gpu=s->cold.gpu.get();
    next.publication=s->cold.gpu->publicationToken();
    if(!next.cpu||!next.publication)return false;
    next.xyOffsetFixed4={{(2048u-next.viewport[2]/2u-next.viewport[0])*16u,
        (2048u-next.viewport[3]/2u-next.viewport[1])*16u}};
    out=std::move(next);return true;
}
bool OriginalUiMainSession50KG::sharedMenuLifetimeCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiMainDraw50KG& draw)const noexcept {
    const auto* s=storage_.get();
    return s&&s->state.bound&&!s->sourceTornDown&& &s->collector==&draw&&s->cold.application&&
        s->cold.application->sharedDialogs.get()==&shared&&s->cold.resources&&
        s->cold.resources->controllerPopup()==shared.controller47C_.resources;
}
bool OriginalUiMainSession50KG::sharedMenuUnwindEntryCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiMainDraw50KG& draw)const noexcept {
    const auto* s=storage_.get();
    return sharedMenuLifetimeCurrent(shared,draw)&&!s->inOperation&&!s->rendering;
}
bool OriginalUiMainSession50KG::sharedMenuRenderCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiMainDraw50KG& draw)const noexcept {
    const auto* s=storage_.get();
    return sharedMenuLifetimeCurrent(shared,draw)&&s->inOperation&&s->rendering&&s->operational()&&
        !s->updateAudioPrepared&&s->state.updateCount&&s->currentMainLifecycle();
}
bool OriginalUiMainSession50KG::sharedMenuUpdateCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiMainDraw50KG& draw)const noexcept {
    const auto* s=storage_.get();
    return sharedMenuLifetimeCurrent(shared,draw)&&s->inOperation&&!s->rendering&&s->operational()&&
        !s->updateAudioPrepared&&s->currentMainLifecycle();
}
Status OriginalUiMainSession50KG::render(const OriginalUiMainFrameClock50KG& clock,
    OriginalUiMainSessionDiagnostic50KG& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->inOperation)return result(d,Status::invalidInput);
    s->state.framePublished=false;s->pendingRoute={};
    if(s->cold.externalTransition && !s->currentMainLifecycle())return result(d,Status::unsupportedState);
    if(s->state.faulted)return result(d,Status::faulted);
    if(s->state.quitRequested)return result(d,Status::quit);
    if(s->cold.externalTransition && s->updateAudioPrepared){
        s->fault(Status::unsupportedState,0x234440);d=s->callbackFault;return d.status;
    }
    if(!s->state.updateCount || (s->paused && clock.advanceAnimations))return result(d,Status::invalidInput);
    // Source scene fog/proxy follows manager UI fog and sets camera far4000.
    // The cold typed boundary preserves that exact render-time matrix/color;
    // it does not replace the manager's immutable authored pose or page camera.
    s->state.framePublished=false;s->inOperation=true;s->rendering=true;
    game::OriginalUiControllerPopupRender50KG popupRender;
    // PC-NATIVE ADDITION (EXIT GAME): the row owns the arrows only while it is selected.
    if(s->exitGame.rowSelected)setOriginalUiMainExitRowOwner50XG(&s->collector);
    else clearOriginalUiMainExitRowOwner50XG(&s->collector);
    game::OriginalUiControllerPopupState50KG exitDialogNext=s->exitGame.dialog;
    bool okay=s->collector.begin(clock,d.draw);
    if(okay){
        if(const auto shared=s->cold.application->sharedDialogs){
            OriginalUiSharedDialogs50KG::Diagnostic sd;
            okay=shared->renderMenuPopupSibling(popupRender,sd);
            if(!okay)d.draw.sourcePC=sd.sourcePC;
        }else okay=game::OriginalUiControllerPopup50KG::render(s->cold.application->controllerPopupState,popupRender)==
            game::OriginalUiControllerPopupStatus50KG::ok;
    }
    okay=okay&&
        s->collector.projectControllerRequiredPopup(popupRender,d.draw)&&s->collector.projectOrdinaryWidgets(true,d.draw) &&
        s->collector.projectMainPage(s->state.mainPageShown,d.draw) &&
        s->drawExitRow(d.draw); // PC-NATIVE ADDITION (EXIT GAME row)
    if(okay)okay=apertureOkay(s->aperture.render48(s->lifecycle().snapshot().lifecycle));
    okay=okay && !s->state.faulted;
    if(okay)okay=s->collector.projectMainBasePrompts(s->state.promptMask42C,d.draw) &&
        s->collector.projectOnTopWidgets(true,d.draw) &&
        s->collector.projectMainParticles(s->fire,s->scene,s->particleRenderState,d.draw) &&
        s->drawExitDialog(exitDialogNext,d.draw) && // PC-NATIVE ADDITION (EXIT GAME dialog)
        s->collector.drain(d.draw);
    s->rendering=false;s->inOperation=false;
    if(!okay){if(s->state.faulted){d=s->callbackFault;return d.status;}return result(d,Status::draw);}
    // PC-NATIVE ADDITION (EXIT GAME): publish the host dialog's render step only after the
    // frame drained, like the 47C; a confirmed dialog that has finished closing hands the
    // exit to the frontend loop (the window-Close path).
    if(s->exitGame.dialog.active3CC){
        s->exitGame.dialog=exitDialogNext;
        if(!exitDialogNext.active3CC){
            if(s->exitGame.confirmed&&!s->exitGame.requested){
                s->exitGame.requested=true;exitGameRequest50XG=true;
                std::fprintf(stderr,"pc_native_exit_game exit_requested=1\n");
            }else if(!s->exitGame.confirmed)s->exitGame.dialog.result3D8=0; // cancelled: row stays selected
        }
        s->publishExitGame();
    }
    // The retained shared owner has already committed the genuine47C body
    // before its fallible projection. Never replay or roll back that prefix.
    if(!s->cold.application->sharedDialogs)s->cold.application->controllerPopupState=popupRender.state;
    ++s->state.renderCount;s->state.framePublished=true;return result(d,Status::ok);
}
namespace {
bool sameDecision(const game::OriginalUiMainDecision50KG& a,const game::OriginalUiMainDecision50KG& b) noexcept {
    return a.action==b.action && a.selectedSlot==b.selectedSlot && a.originalGameMode==b.originalGameMode &&
        a.originalPlayerBits==b.originalPlayerBits && a.routeArgument==b.routeArgument &&
        a.targetScreen==b.targetScreen && a.sourcePC==b.sourcePC;
}
bool sameRoute(game::OriginalUiTransitionRoute50KG a,game::OriginalUiTransitionRoute50KG b) noexcept {
    return a.screen==b.screen && a.page==b.page;
}
bool selectedControlCurrent(const game::OriginalUiNavigation50KG* nav,std::uint16_t slot) noexcept {
    if(!nav || !nav->page || nav->page->groups.size()<=1 || !slot ||
        slot>nav->page->groups[1].controls.size() || nav->groups[1].selectedSlot!=slot)return false;
    const auto control=nav->page->groups[1].controls[slot-1];
    return control>=0 && static_cast<std::size_t>(control)<nav->controls.size() &&
        nav->controls[static_cast<std::size_t>(control)].enabled &&
        !nav->controls[static_cast<std::size_t>(control)].blocked;
}
}
Status OriginalUiMainSession50KG::notifyTransition(const game::OriginalUiTransitionNotification50KG& event,
    OriginalUiMainSessionDiagnostic50KG& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->cold.application->sharedDialogs&&s->cold.application->sharedDialogs->menu470PendingFor(s->collector))
        return result(d,Status::unsupportedState,0x204A00);
    if(!s->cold.externalTransition || s->inOperation || !s->operational())return result(d,Status::unsupportedState);
    const auto t=s->lifecycle().snapshot();
    if(!t.bound || event.route.screen!=4 || event.route.page!=1 || event.lifecycle!=t.lifecycle || event.phase!=t.phase ||
        (!sameRoute(event.route,t.current) && !sameRoute(event.route,t.requested)))return result(d,Status::invalidInput,event.sourcePC);
    s->state.framePublished=false;s->pendingRoute={};s->inOperation=true;Storage::notify(s,event);s->inOperation=false;
    if(s->state.faulted){d=s->callbackFault;return d.status;}
    return result(d,Status::ok);
}
game::OriginalUiTransitionCompletion50KG OriginalUiMainSession50KG::pollCompletion(
    game::OriginalUiTransitionRoute50KG route,game::OriginalUiTransitionPhase50KG phase) const noexcept {
    const auto* s=storage_.get();
    if(!s || !s->cold.externalTransition || s->inOperation || !s->operational())
        return game::OriginalUiTransitionCompletion50KG::unavailable;
    const auto t=s->lifecycle().snapshot();
    if(!t.bound || !sameRoute(t.current,route) || t.phase!=phase)
        return game::OriginalUiTransitionCompletion50KG::unavailable;
    return s->aperture.pollCompletion(route,phase);
}
const game::OriginalUiTransitionOwner50KG* OriginalUiMainSession50KG::externalLifecycleOwner() const noexcept {
    return storage_?storage_->cold.externalTransition.get():nullptr;
}
bool OriginalUiMainSession50KG::routeProposalCurrent(const OriginalUiMainRouteProposal50KG& p) const noexcept {
    const auto* s=storage_.get();
    if(!s || !s->cold.externalTransition || s->inOperation || s->updateAudioPrepared || !s->operational() || p.source!=this || !p.serial ||
        s->pendingRoute.source!=this || p.serial!=s->pendingRoute.serial ||
        !sameDecision(p.decision,s->pendingRoute.decision))return false;
    const auto t=s->lifecycle().snapshot();const auto a=s->aperture.snapshot();
    const auto* nav=s->cold.resources->manager()->navigation(1);
    return t.bound && t.current.screen==4 && t.current.page==1 && sameRoute(t.requested,t.current) &&
        t.lifecycle==game::OriginalUiTransitionLifecycle50KG::idle && t.phase==game::OriginalUiTransitionPhase50KG::none &&
        !t.initialRestartPending && !t.applicationRequestQueued && s->state.mainPageShown && s->state.inputContainerShown &&
        a.inputEnabled268 && a.state==game::OriginalUiApertureState50KG::open &&
        selectedControlCurrent(nav,p.decision.selectedSlot) && s->cold.application->transportE00==-1 &&
        !(s->cold.application->globalFlags228&4u) && s->cold.profile->known && s->cold.application->known;
}
Status OriginalUiMainSession50KG::notifyRouteAccepted(const OriginalUiMainRouteProposal50KG& p,
    OriginalUiMainSessionDiagnostic50KG& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(!s->cold.externalTransition || s->inOperation || s->updateAudioPrepared || !s->operational() || p.source!=this || !p.serial ||
        p.serial!=s->pendingRoute.serial || !sameDecision(p.decision,s->pendingRoute.decision))
        return result(d,Status::invalidInput);
    const auto t=s->lifecycle().snapshot();const auto* nav=s->cold.resources->manager()->navigation(1);
    const bool page=p.decision.action==game::OriginalUiMainAction50KG::changePage;
    const auto expectedLife=page?game::OriginalUiTransitionLifecycle50KG::requestPageExit:
        game::OriginalUiTransitionLifecycle50KG::requestScreenExit;
    const auto targetScreen=page?4:p.decision.targetScreen;
    if(!t.bound || t.current.screen!=4 || t.current.page!=1 || t.lifecycle!=expectedLife ||
        t.phase!=game::OriginalUiTransitionPhase50KG::none || t.applicationRequestQueued || t.initialRestartPending ||
        t.requested.screen!=targetScreen || t.requested.page!=p.decision.routeArgument ||
        !selectedControlCurrent(nav,p.decision.selectedSlot) ||
        s->cold.profile->savedSlot6E0!=p.decision.selectedSlot || s->cold.application->transportE00!=-1)
        return result(d,Status::unsupportedState,0x1E61A4);
    if(p.decision.selectedSlot<=3 && (s->cold.profile->mode34!=static_cast<std::uint32_t>(p.decision.originalGameMode) ||
        (s->cold.profile->playerBits1125C&0x60u)!=p.decision.originalPlayerBits))
        return result(d,Status::unsupportedState,0x1E5EF8);
    s->state.framePublished=false;s->pendingRoute={};s->inOperation=true;
    const bool okay=s->audioEffect(1);s->inOperation=false;
    if(!okay){d=s->callbackFault;return d.status;}
    return result(d,Status::ok,0x1E61B4);
}
void OriginalUiMainSession50KG::requestQuit() noexcept {
    if(!storage_ || storage_->inOperation)return;
    storage_->state.quitRequested=true;storage_->state.framePublished=false;
    if(storage_->cold.audioMode==OriginalUiMainAudioMode50KG::enabled && storage_->cold.audio)storage_->cold.audio->pause(true);
}
OriginalUiMainSessionSnapshot50KG OriginalUiMainSession50KG::snapshot() const noexcept {
    if(!storage_)return {};
    auto snapshot=storage_->state;snapshot.controllerPopup=storage_->cold.application->controllerPopupState;snapshot.transition=storage_->lifecycle().snapshot();snapshot.aperture=storage_->aperture.snapshot();
    if(const auto* text=storage_->cold.resources->exitText())snapshot.exitText=text->snapshot();
    return snapshot;
}
const OriginalUiNativeDraw50KG* OriginalUiMainSession50KG::draw() const noexcept {
    return storage_ && !storage_->state.faulted && !storage_->state.quitRequested && storage_->state.framePublished &&
        !storage_->inOperation && !storage_->updateAudioPrepared &&
        (!storage_->cold.externalTransition || storage_->currentMainLifecycle())?storage_->collector.draw():nullptr;
}
const OriginalUiSourceFrame50KM* OriginalUiMainSession50KG::sourceFrame() const noexcept {
    return storage_&&storage_->operational()&&storage_->state.framePublished&&!storage_->inOperation&&
        !storage_->updateAudioPrepared&&(!storage_->cold.externalTransition||storage_->currentMainLifecycle())?
        storage_->collector.sourceFrame():nullptr;
}
bool OriginalUiMainSession50KG::sourceTeardownPending() const noexcept {
    return storage_&&!storage_->sourceTornDown&&storage_->collector.sourceTeardownPending();
}
Status OriginalUiMainSession50KG::teardownSource(VehicleWindowGl50DS* renderer,OriginalUiMainSessionDiagnostic50KG& d) noexcept {
    d={};if(!storage_||storage_->sourceTornDown)return result(d,Status::ok);
    auto& s=*storage_;if(s.inOperation||s.rendering)return result(d,Status::invalidInput);
    s.sourceClosing=true;s.state.framePublished=false;
    if(s.collector.sourceTeardownPending()&&(!renderer||!s.collector.retireSourceRenderer(*renderer,d.draw)))return result(d,Status::draw);
    s.sourceTornDown=true;return result(d,Status::ok);
}
const game::OriginalUiMainCameraPreparation50KG* OriginalUiMainSession50KG::camera() const noexcept {return storage_?&storage_->camera:nullptr;}
const game::OriginalUiMainParticleCamera50KG* OriginalUiMainSession50KG::particleCamera() const noexcept {return storage_?&storage_->particleRenderState.particleCamera:nullptr;}
const game::OriginalUiMainParticleRenderState50KG* OriginalUiMainSession50KG::particleRenderState() const noexcept {return storage_?&storage_->particleRenderState:nullptr;}
const game::OriginalUiMainSceneOwner50KG* OriginalUiMainSession50KG::scene() const noexcept {return storage_?&storage_->scene:nullptr;}
const game::OriginalUiMainFireOwner50KG* OriginalUiMainSession50KG::fire() const noexcept {return storage_?&storage_->fire:nullptr;}
const game::OriginalUiMainParticleLighting50KG* OriginalUiMainSession50KG::lighting() const noexcept {return storage_?&storage_->lighting:nullptr;}
}
