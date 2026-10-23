#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "original_ui_garage_scene_lighting50kg.h"
#include "original_ui_light_manager_owner50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../src/game/original_ui_aperture_owner50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>

namespace {
using V=std::array<float,3>;
float add(float a,float b)noexcept {volatile float v=a+b;return v;}
float sub(float a,float b)noexcept {volatile float v=a-b;return v;}
float mul(float a,float b)noexcept {volatile float v=a*b;return v;}
float div(float a,float b)noexcept {volatile float v=a/b;return v;}
float dot(const V& a,const V& b)noexcept {
    return add(add(mul(a[0],b[0]),mul(a[1],b[1])),mul(a[2],b[2]));
}
V cross(const V& a,const V& b)noexcept {
    return {sub(mul(a[1],b[2]),mul(a[2],b[1])),
        sub(mul(a[2],b[0]),mul(a[0],b[2])),
        sub(mul(a[0],b[1]),mul(a[1],b[0]))};
}
bool normalize(V& v)noexcept {
    const float square=dot(v,v);
    if(!std::isfinite(square)||square<=0)return false;
    const float reciprocal=div(1.f,std::sqrt(square));
    for(float& f:v){f=mul(f,reciprocal);if(!std::isfinite(f))return false;}
    return true;
}
}
namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::lightParticleCorners149310(
    OriginalUiGarageSceneParticles50KG& pool,Diagnostic& d)noexcept {
    if(!identityCurrent()||phase_!=Phase::firstPass||callback_!=&pool.entity||
       !page_||!page_->scene4C0_||page_->sceneBacking_.get()!=page_->scene4C0_||
       page_->scenePageChildren_[9]!=&pool.entity||pool.projectionPublication!=publication_||
       !publication_||!pool.field12C||pool.projectionTerminalPC)return fail(d,0x162AEC);
    auto& scene=*page_->scene4C0_;
    if(scene.sceneParticles_.get()!=&pool||!scene.sceneLighting_||
       !scene.sceneLighting_->manager_||!pool.prepared||pool.retiring||pool.retired)
        return fail(d,0x162AEC);
    const auto terminal=[&](std::uint32_t pc)noexcept {
        pool.projectionTerminalPC=pc;sourcePC_=pc;return fail(d,pc);
    };
    if(std::fegetround()!=FE_TONEAREST)return terminal(0x162C18);
    if(pool.projectionSourcePC==0x162AEC){
        OriginalUiGarageCameraFrame50KG::Fields camera;std::uint32_t pc=0;
        if(!cameraFrame_.read(camera,pc))return fail(d,pc?pc:0x162AF8);
        V back{},up{},position{};
        for(unsigned c=0;c<3;++c){
            if(!std::isfinite(camera.look190[c])||!std::isfinite(camera.up1A0[c])||
               !std::isfinite(camera.position180[c]))return terminal(0x162B00);
            back[c]=-camera.look190[c];up[c]=camera.up1A0[c];
            //106790 multiplies actual look by100 before the position addition.
            position[c]=add(camera.position180[c],mul(100.f,camera.look190[c]));
        }
        V right=cross(up,back);
        up=cross(back,right); //162C18..30, before either normalization.
        if(!normalize(up)||!normalize(back))return terminal(0x162C5C);
        right=cross(up,back); //162C88/8C; source does NOT normalize right again.
        std::array<float,16> facing{};
        for(unsigned c=0;c<3;++c){
            facing[c]=right[c];facing[4+c]=up[c];facing[8+c]=back[c];
            facing[12+c]=position[c];
        }
        facing[15]=1.f;
        for(float f:facing)if(!std::isfinite(f))return terminal(0x162CA8);
        pool.facingMatrix162CA8=facing; //actual local source matrix supplied to149310
        OriginalUiLightManagerOwner50KG::ParticleLight149310 light;
        OriginalUiLightManagerOwner50KG::Diagnostic ld;
        if(!scene.sceneLighting_->manager_->readGarageParticleLight149310(*issuer_,light,ld))
            return fail(d,ld.sourcePC?ld.sourcePC:0x1493CC);
        for(float f:light.ambient50)if(!std::isfinite(f))return terminal(0x1494D4);
        for(float f:light.diffuse3C)if(!std::isfinite(f))return terminal(0x1493EC);
        V direction{};
        if(light.lastType2){
            V source{};
            for(unsigned c=0;c<3;++c){
                if(!std::isfinite(light.positionA0[c]))return terminal(0x149408);
                source[c]=light.positionA0[c];
            }
            const V basis[3]={right,up,back};
            //149310's rigid inverse transposes the basis and forms its
            // negative translated dot. Keep that translation separately
            // rounded, rather than changing this to dot(source-position,basis).
            const V negativePosition{-position[0],-position[1],-position[2]};
            for(unsigned c=0;c<3;++c)direction[c]=add(dot(source,basis[c]),dot(negativePosition,basis[c]));
            if(!normalize(direction))return terminal(0x149438);
        }
        pool.particleLightDirection=direction;pool.particleLightDiffuse=light.diffuse3C;
        pool.particleLightAmbient=light.ambient50;pool.lightingRow=0;pool.lightingAlphaRow=0;
        pool.projectionSourcePC=0x14949C;
    }
    if(pool.projectionSourcePC==0x14949C){
        for(;pool.lightingRow<4;++pool.lightingRow){
            const auto& normal=pool.normals50[pool.lightingRow];
            const V n{normal[0],normal[1],normal[2]};
            for(float f:n)if(!std::isfinite(f))return terminal(0x14949C);
            const float contribution=-dot(pool.particleLightDirection,n);
            if(!std::isfinite(contribution))return terminal(0x1494C4);
            std::array<std::int32_t,3> rgb{};
            for(unsigned c=0;c<3;++c){
                float unit=div(pool.particleLightAmbient[c],128.f);
                if(contribution>0)
                    unit=std::min(1.f,div(add(pool.particleLightAmbient[c],
                        mul(contribution,pool.particleLightDiffuse[c])),128.f));
                //149588 ambient branch has NO invented upper/lower clamp.
                if(!game::convertOriginalUiApertureCvtWS50KG(mul(255.f,unit),rgb[c]))
                    return terminal(contribution>0?0x149540:0x1495B4);
            }
            const std::uint32_t packed=0xff000000u|
                (static_cast<std::uint32_t>(rgb[0])<<16)|
                (static_cast<std::uint32_t>(rgb[1])<<8)|static_cast<std::uint32_t>(rgb[2]);
            pool.quadRemainder[pool.lightingRow][1]=packed; //149584/1495EC: actual+10 only.
        }
        pool.projectionSourcePC=0x162CD8;
    }
    if(pool.projectionSourcePC==0x162CD8){
        for(;pool.lightingAlphaRow<4;++pool.lightingAlphaRow){
            auto& color=pool.quadRemainder[pool.lightingAlphaRow][1];
            if(!color)return terminal(0x162CDC);
            *color&=0x00ffffffu; //162CDC..162D14, actual per-corner source alpha clear.
        }
        pool.projectionSourcePC=0x162D1C;
    }
    if(pool.projectionSourcePC!=0x162D1C)return fail(d,pool.projectionSourcePC);
    sourcePC_=0x162D1C;d={};return true;
}
}
