#include "control_binding48w.h"
#include "scalar_minmax48o.h"
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace rr::game {
namespace {
bool finite(float v) noexcept {
    std::uint32_t bits;std::memcpy(&bits,&v,4);return (bits&0x7f800000u)!=0x7f800000u;
}
float positiveOrStoredZero(float value) noexcept {
    // 218800..218824 selects the steering word only above zero; its other
    // branch is an integer `sw zero`, including for negative zero. Selecting
    // the representation avoids MSVC turning a float ternary into MAXSS with
    // the input's zero sign on equality, even under /fp:strict.
    std::uint32_t word;
    std::memcpy(&word,&value,4);
    word=value>0.0f?word:0u;
    float result;
    std::memcpy(&result,&word,4);
    return result;
}
// Volatile operands prevent constant-path folding of 1 + (-1) / 1 - 1:
// GCC otherwise loses the negative zero required by FE_DOWNWARD at saturation.
struct Arithmetic {
    bool valid=true;
    float keep(float v) noexcept {valid=valid&&finite(v);return v;}
    float add(float a,float b) noexcept {volatile float left=a,right=b;volatile float v=left+right;return keep(v);}
    float sub(float a,float b) noexcept {volatile float left=a,right=b;volatile float v=left-right;return keep(v);}
    float mul(float a,float b) noexcept {volatile float left=a,right=b;volatile float v=left*right;return keep(v);}
    float div(float a,float b) noexcept {volatile float left=a,right=b;volatile float v=left/right;return keep(v);}
};
bool allFinite(const ControlBindingInput48W& in,const ControlBindingParameters48W& p,
               const ControlModelBindings48W& previous) noexcept {
    for(float value:in.filtered.values)if(!finite(value))return false;
    for(float value:{in.bodyCachedSpeed34,in.actionB7C,in.actionB80,in.source7D0,in.source7D4,
                     p.speed1B84,p.scale1B88,p.speed2B8C,p.scale2B90,p.scale4346B8,p.bias4346C0,
                     previous.engineDrive[0],previous.engineDrive[1],previous.aeroDrive[0],previous.aeroDrive[1],
                     previous.aeroBrakeEC,previous.aeroAux50,previous.aeroAuxF0,previous.aeroAuxF4})
        if(!finite(value))return false;
    return true;
}
}
bool bindVehicleControls48W(const ControlBindingInput48W& in,const ControlBindingParameters48W& p,
                           const ControlModelBindings48W& previous,ControlModelBindings48W& out) noexcept {
    if(!allFinite(in,p,previous))return false;
    Arithmetic f;
    auto result=previous;
    const auto state=in.vehicleState40;
    const bool restricted=state==1||state==2;
    if(state==3||state==4) {
        result.engineDrive[0]=result.engineDrive[1]=0.0f;
        result.aeroDrive[0]=result.aeroDrive[1]=0.0f;
        result.aeroTriggerD8=0;
        // Original leaves EC and auxiliary50 untouched on this route.
    } else if(!in.mode58Nonzero&&in.configurationEntryNonzero) {
        const float left=in.filtered.values[1],right=in.filtered.values[3];
        result.engineDrive[1]=detail::scalarMax48O(-right,0.0f);
        result.engineDrive[0]=detail::scalarMax48O(-left,0.0f);
        if(restricted) {
            result.engineDrive[1]=f.mul(p.scale4346B8,result.engineDrive[1]);
            result.engineDrive[0]=f.mul(p.scale4346B8,result.engineDrive[0]);
        }
        if(left>=0.9f&&right>=0.9f&&!restricted) {
            result.aeroDrive[0]=result.aeroDrive[1]=result.aeroBrakeEC=1.0f;
        } else {
            result.aeroDrive[1]=detail::scalarMax48O(right,0.0f);
            result.aeroDrive[0]=detail::scalarMax48O(left,0.0f);
            if(restricted) {
                result.aeroDrive[1]=f.mul(p.scale4346B8,result.aeroDrive[1]);
                result.aeroDrive[0]=f.mul(p.scale4346B8,result.aeroDrive[0]);
                if(state==1)result.aeroDrive[1]=f.add(p.bias4346C0,result.aeroDrive[1]);
                else result.aeroDrive[0]=f.add(p.bias4346C0,result.aeroDrive[0]);
            }
            result.aeroBrakeEC=0.0f;
        }
        result.aeroTriggerD8=(in.flagsB9C&4u)?1:0;
        // This route does not write auxiliary50.
    } else {
        float scale=0.0f;
        if(p.speed2B8C!=p.speed1B84) {
            const float offset=f.sub(in.bodyCachedSpeed34,p.speed1B84);
            const float span=f.sub(p.speed2B8C,p.speed1B84);
            const float difference=f.sub(p.scale2B90,p.scale1B88);
            scale=f.add(p.scale1B88,f.div(f.mul(offset,difference),span));
            if(!(p.scale2B90<=p.scale1B88)) {
                if(!(scale<=p.scale2B90))scale=p.scale2B90;
                else if(scale<p.scale1B88)scale=p.scale1B88;
            } else {
                if(scale<p.scale2B90)scale=p.scale2B90;
                else if(!(scale<=p.scale1B88))scale=p.scale1B88;
            }
        }
        float steering=f.mul(scale,in.filtered.values[0]);
        if(state==1)steering=f.add(p.bias4346C0,f.mul(p.scale4346B8,steering));
        else if(state==2)steering=f.sub(f.mul(p.scale4346B8,steering),p.bias4346C0);
        steering=detail::scalarMin48O(detail::scalarMax48O(steering,-1.0f),1.0f);
        if(in.actionB7C!=0.0f&&!restricted) {
            result.engineDrive[0]=result.engineDrive[1]=1.0f;
            if(!(steering<=0.0f)) {
                result.engineDrive[1]=f.mul(result.engineDrive[1],f.sub(1.0f,steering));
                result.aeroDrive[1]=steering;
            } else result.aeroDrive[1]=0.0f;
            if(steering<0.0f) {
                const float negative=-steering;
                steering=f.add(1.0f,steering);
                result.engineDrive[0]=f.mul(result.engineDrive[0],steering);
                result.aeroDrive[0]=negative;
            } else result.aeroDrive[0]=0.0f;
            result.aeroBrakeEC=0.0f;
            if(in.actionB80!=0.0f) {
                result.aeroBrakeEC=1.0f;
                result.aeroDrive[0]=detail::scalarMax48O(result.aeroDrive[0],0.5f);
                result.aeroDrive[1]=detail::scalarMax48O(result.aeroDrive[1],0.5f);
            }
        } else if(in.actionB80!=0.0f&&!restricted) {
            result.aeroDrive[0]=!(steering<=0.0f)?f.sub(1.0f,steering):1.0f;
            result.aeroDrive[1]=steering<0.0f?f.add(1.0f,steering):1.0f;
            result.aeroBrakeEC=1.0f;
            result.engineDrive[0]=result.engineDrive[1]=0.0f;
        } else {
            result.engineDrive[0]=result.engineDrive[1]=0.0f;
            result.aeroDrive[1]=positiveOrStoredZero(steering);
            result.aeroDrive[0]=positiveOrStoredZero(-steering);
            result.aeroBrakeEC=0.0f;
        }
        result.aeroAux50=in.filtered.values[1];
        result.aeroTriggerD8=(in.flagsB9C&4u)?1:0;
    }
    result.aeroAuxF0=in.source7D4;
    result.aeroAuxF4=in.source7D0;
    if(!f.valid)return false;
    out=result;
    return true;
}

bool applyControlModelBindings48W(const ControlModelBindings48W& bindings,
                                 const EngineInput48D& previousEngine,const AeroInput48E& previousAero,
                                 EngineInput48D& outEngine,AeroInput48E& outAero) noexcept {
    for(float value:{bindings.engineDrive[0],bindings.engineDrive[1],bindings.aeroDrive[0],bindings.aeroDrive[1],bindings.aeroBrakeEC})
        if(!finite(value))return false;
    auto engine=previousEngine;auto aero=previousAero;
    engine.drive[0]=bindings.engineDrive[0];engine.drive[1]=bindings.engineDrive[1];
    aero.driveE4=bindings.aeroDrive[0];aero.driveE8=bindings.aeroDrive[1];
    aero.brakeEc=bindings.aeroBrakeEC;aero.triggerD8=bindings.aeroTriggerD8!=0;
    outEngine=engine;outAero=aero;
    return true;
}

bool stepAndBindControlInputs48W(const ControlChannels48B& raw,float previousD28,
                                const ControlChannels48B& previousFiltered,ControlRateLimits48B rates,float elapsedFactor,
                                const ControlBindingInput48W& context,const ControlBindingParameters48W& parameters,
                                const ControlModelBindings48W& previousBindings,ControlBoundStep48W& out) noexcept {
    ControlBoundStep48W result{};
    const auto mode=context.mode58Nonzero?ControlShapeMode48V::BypassNonlinear:ControlShapeMode48V::Nonlinear;
    if(!stepControlInputs48V(raw,previousD28,mode,previousFiltered,rates,elapsedFactor,result.control))return false;
    auto input=context;input.filtered=result.control.filtered;
    if(!bindVehicleControls48W(input,parameters,previousBindings,result.bindings))return false;
    out=result;
    return true;
}
}
