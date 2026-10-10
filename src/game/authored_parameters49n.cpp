#include "authored_parameters49n.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
using K=AuthoredKey49N;
float fromBits(std::uint32_t u) noexcept { float f; std::memcpy(&f,&u,4); return f; }
float negative(float f) noexcept {
    std::uint32_t u; std::memcpy(&u,&f,4); return fromBits(u^0x80000000u);
}
bool nearest() noexcept { return std::fegetround()==FE_TONEAREST; }
bool validClass(AuthoredClass49N c) noexcept {
    return c==AuthoredClass49N::A || c==AuthoredClass49N::B || c==AuthoredClass49N::C;
}
struct Math {
    bool valid=true;
    float keep(float f) noexcept { valid=valid&&std::isfinite(f); return f; }
    float add(float a,float b) noexcept { volatile float f=a+b; return keep(f); }
    float sub(float a,float b) noexcept { volatile float f=a-b; return keep(f); }
    float mul(float a,float b) noexcept { volatile float f=a*b; return keep(f); }
    float div(float a,float b) noexcept { volatile float f=a/b; return keep(f); }
    float get(const AuthoredParameters49N& p,K k) noexcept {
        const auto& x=p.values[static_cast<std::size_t>(k)];
        if(!x.present){valid=false;return 0.0f;}
        return keep(x.value);
    }
    float interpolate(float minimum,float maximum,float ability) noexcept {
        const float difference=sub(maximum,minimum);
        return add(minimum,mul(ability,difference));
    }
    float authoredAbility(const AuthoredParameters49N& p,K base,K maximum,float upgrade) noexcept {
        const float b=get(p,base), m=get(p,maximum);
        return add(b,mul(upgrade,sub(m,b)));
    }
    float clampedProfile(float minimum,float maximum,float ability) noexcept {
        //216108..2161A0 /2162A0..216338. Preserve subtract0/divide1 and
        // comparisons for both increasing and decreasing endpoint order.
        const float d=sub(maximum,minimum);
        const float a=sub(ability,0.0f);
        float v=add(minimum,div(mul(a,d),1.0f));
        if(maximum<=minimum) {
            if(v<maximum) v=maximum;
            else if(!(v<=minimum)) v=minimum;
        } else {
            if(!(v<=maximum)) v=maximum;
            else if(v<minimum) v=minimum;
        }
        return v;
    }
    BodyMatrix48G inverse(const BodyMatrix48G& m) noexcept {
        //14E910. Full original scalar-ACC cofactor order, not 1/diagonal.
        const float a=m.v[0],b=m.v[4],c=m.v[8];
        const float d=m.v[1],e=m.v[5],f=m.v[9];
        const float g=m.v[2],h=m.v[6],i=m.v[10];
        const float c00=sub(mul(e,i),mul(f,h));
        const float ci=sub(mul(b,i),mul(c,h));
        const float c02=sub(mul(b,f),mul(c,e));
        float determinant=sub(mul(a,c00),mul(d,ci));
        determinant=add(determinant,mul(g,c02));
        BodyMatrix48G o{};
        if(determinant==0.0f) return o;
        const float u=div(1.0f,determinant);
        o.v[0]=mul(u,c00);
        o.v[1]=mul(u,negative(sub(mul(d,i),mul(f,g))));
        o.v[2]=mul(u,sub(mul(d,h),mul(e,g)));
        o.v[4]=mul(u,negative(ci));
        o.v[5]=mul(u,sub(mul(a,i),mul(c,g)));
        o.v[6]=mul(u,negative(sub(mul(a,h),mul(b,g))));
        o.v[8]=mul(u,c02);
        o.v[9]=mul(u,negative(sub(mul(a,f),mul(c,d))));
        o.v[10]=mul(u,sub(mul(a,e),mul(b,d)));
        //All remaining lanes, including15, are original +0.
        return o;
    }
};
K classKey(K a,AuthoredClass49N c) noexcept {
    return static_cast<K>(static_cast<std::uint32_t>(a)+static_cast<std::uint32_t>(c)-1u);
}
void copyAero(const AeroParameters48E& source,AeroParameters48E& target) noexcept {
    //Explicit field list deliberately excludes the unresolved130 writer.
    target.offset68=source.offset68;target.lift6c=source.lift6c;target.drag78=source.drag78;
    target.pitch84=source.pitch84;target.pitchDamping88=source.pitchDamping88;target.lateral90=source.lateral90;
    target.rollDampingB0=source.rollDampingB0;target.timerYawB8=source.timerYawB8;target.yawDampingC8=source.yawDampingC8;
    target.driveDragF8=source.driveDragF8;target.brakeDragFc=source.brakeDragFc;target.steering100=source.steering100;
    target.steeringRamp104=source.steeringRamp104;target.timerSteering108=source.timerSteering108;
    target.timerLateral10c=source.timerLateral10c;target.timerYaw110=source.timerYaw110;target.timerDrag114=source.timerDrag114;
    target.tiltedLateral118=source.tiltedLateral118;target.tiltedSteering11c=source.tiltedSteering11c;target.tiltedYaw120=source.tiltedYaw120;
}
bool finiteMapped(const AuthoredBinding49N& b) noexcept {
    const auto& e=b.main.engine; const auto& a=b.main.aero;
    if(b.massSetterWrote&&(!std::isfinite(b.main.body.mass)||!std::isfinite(b.main.body.inverseMass))) return false;
    const float scalars[]={e.maxThrust,e.boostThrust,e.repairThrust,e.rampUpFilter,e.rampDownFilter,
        e.rampDownSpeed,e.theoreticalTopSpeed,e.boostSpeed,e.repairSpeed,
        a.offset68,a.lift6c,a.drag78,a.pitch84,a.pitchDamping88,a.lateral90,a.rollDampingB0,a.timerYawB8,a.yawDampingC8,
        a.driveDragF8,a.brakeDragFc,a.steering100,a.steeringRamp104,a.timerSteering108,a.timerLateral10c,a.timerYaw110,
        a.timerDrag114,a.tiltedLateral118,a.tiltedSteering11c,a.tiltedYaw120};
    for(float x:scalars) if(!std::isfinite(x)) return false;
    for(const auto& v:e.torqueBasis) for(float x:v.v) if(!std::isfinite(x)) return false;
    for(float x:b.inverseInertia140.v) if(!std::isfinite(x)) return false;
    return true;
}
}

bool selectAuthoredClass49N(const char* text,std::size_t length,AuthoredClass49N& out) noexcept {
    if(length>255 || (length && !text)) return false;
    bool a=false,b=false;
    for(std::size_t i=0;i<length;++i) {
        if(text[i]=='\0') return false;
        a=a||text[i]=='a';b=b||text[i]=='b';
    }
    auto c=a?AuthoredClass49N::A:b?AuthoredClass49N::B:AuthoredClass49N::C;
    if(length==2) {
        if(text[0]=='s'&&text[1]=='b') c=AuthoredClass49N::C;
        else if((text[0]=='r'&&text[1]=='a')||(text[0]=='s'&&text[1]=='c')) c=AuthoredClass49N::B;
    }
    out=c;return true;
}

bool bindAuthoredParameters49N(const AuthoredParameters49N& p,const AuthoredSelectionContext49N& context,
    const AuthoredBinding49N& previous,AuthoredBinding49N& out) noexcept {
    if(!nearest() || !validClass(context.inferredClass)) return false;
    Math m; auto result=previous;
    auto get=[&](K k) noexcept {return m.get(p,k);};
    const float weight=get(K::Weight),roll=get(K::RollInertiaScale),pitch=get(K::PitchInertiaScale),yaw=get(K::YawInertiaScale);
    const float gravity=get(K::Gravity),airborne=get(K::AirborneGravity),g=fromBits(0x4200cccdu);
    result.gravity378=m.mul(gravity,g);result.airborneGravity37C=m.mul(airborne,g);
    const float mass=m.div(weight,g);
    float t=m.div(mass,12.0f);t=m.mul(2.0f,t);t=m.mul(t,10.0f);t=m.mul(10.0f,t);
    result.inertia100={};result.inertia100.v[0]=m.mul(pitch,t);result.inertia100.v[5]=m.mul(yaw,t);
    result.inertia100.v[10]=m.mul(roll,t);result.inertia100.v[15]=1.0f;
    result.massSetterWrote=mass>0.0f;
    if(result.massSetterWrote) {result.main.body.mass=mass;result.main.body.inverseMass=m.div(1.0f,mass);}
    result.inverseInertia140=m.inverse(result.inertia100);
    for(unsigned c=0;c<3;++c) for(unsigned r=0;r<3;++r)
        result.main.body.inverseInertiaColumns[c].v[r]=result.inverseInertia140.v[c*4+r];

    auto selected=context.inferredClass;bool useValues=true;float upgrade=0.0f;
    if(p.tuning.sectionPresent) {
        if(p.tuning.useValues.present&&p.tuning.useValues.value==0) useValues=false;
        if(useValues) {
            if(p.tuning.useAValues.present&&p.tuning.useAValues.value==1) selected=AuthoredClass49N::A;
            if(p.tuning.useBValues.present&&p.tuning.useBValues.value==1) selected=AuthoredClass49N::B;
            if(p.tuning.useCValues.present&&p.tuning.useCValues.value==1) selected=AuthoredClass49N::C;
        } else if(p.tuning.upgradePercent.present) upgrade=m.keep(p.tuning.upgradePercent.value);
    }
    result.selectedClass=selected;result.usedAuthoredClassValues=useValues;
    auto& a=result.main.aero;
    a.offset68=get(K::DefaultAngleOfAttack);a.lift6c=get(K::Lift);a.drag78=get(K::BasicDrag);
    a.pitch84=get(K::PitchMoment);a.pitchDamping88=get(K::PitchRatePitchMoment);a.lateral90=get(K::SideSlipSideForce);
    a.rollDampingB0=get(K::RollRateRollMoment);a.timerYawB8=get(K::SideSlipYawMoment);a.yawDampingC8=get(K::YawRateYawMoment);
    a.driveDragF8=get(K::SpoilerDrag);a.brakeDragFc=get(K::SpoilerBrakeDrag);a.steeringRamp104=get(K::SpoilerYawMomentScale);
    a.timerSteering108=get(K::SpoilerSideForce);a.timerLateral10c=get(K::PowerslideSideForce);
    a.timerYaw110=get(K::PowerslideSpoilerYawMoment);a.timerDrag114=get(K::PowerslideThrust);
    a.tiltedLateral118=get(K::InvertedSideSlipSideForce);a.tiltedSteering11c=get(K::InvertedSpoilerSideForce);
    a.tiltedYaw120=get(K::InvertedSideSlipYawMoment);
    if(useValues) a.steering100=get(classKey(K::SpoilerYawMomentA,selected));
    else {
        float low=get(K::MinSpoilerYawMoment),high=get(K::MaxSpoilerYawMoment);
        const float offset=context.mode58Nonzero?0.0f:get(K::SpoilerYawMomentOffset);
        low=m.add(low,offset);high=m.add(high,offset);
        const float ability=m.authoredAbility(p,K::BaseAbilitySpoilerYawMoment,K::MaxAbilitySpoilerYawMoment,upgrade);
        a.steering100=m.interpolate(low,high,ability);
    }
    auto& e=result.main.engine; float topSpeed;
    if(useValues) {
        e.maxThrust=get(classKey(K::MaxThrustA,selected));topSpeed=get(classKey(K::TheoreticalTopSpeedA,selected));
    } else {
        const float minThrust=get(K::MinMaxThrust),maxThrust=get(K::MaxMaxThrust);
        const float minSpeed=get(K::MinTheoreticalTopSpeed),maxSpeed=get(K::MaxTheoreticalTopSpeed);
        const float thrustAbility=m.authoredAbility(p,K::BaseAbilityMaxThrust,K::MaxAbilityMaxThrust,upgrade);
        const float speedAbility=m.authoredAbility(p,K::BaseAbilityTheoreticalTopSpeed,K::MaxAbilityTheoreticalTopSpeed,upgrade);
        e.maxThrust=m.interpolate(minThrust,maxThrust,thrustAbility);topSpeed=m.interpolate(minSpeed,maxSpeed,speedAbility);
    }
    e.boostThrust=get(K::BoostThrust);e.repairThrust=get(K::RepairThrust);
    e.rampUpFilter=get(K::RampUpFilter);e.rampDownFilter=get(K::RampDownFilter);
    const float units=fromBits(0x3fbbbb84u);
    e.rampDownSpeed=m.mul(get(K::RampDownSpeed),units);e.theoreticalTopSpeed=m.mul(topSpeed,units);
    const float actualTopSpeed=m.mul(get(K::ActualTopSpeed),units);
    e.boostSpeed=m.mul(get(K::BoostSpeed),units);e.repairSpeed=m.mul(get(K::RepairSpeed),units);
    //Vectors are looked up before the configuration branch, but an absent or
    //unusable vector is outside admission only when it actually survives it.
    if(!context.mode58Nonzero&&context.configurationEntryNonzero) {
        a.steering100=m.mul(a.steering100,0.5f);
        e.torqueBasis[0]={{0.0f,negative(a.steering100),0.0f}};
        e.torqueBasis[1]={{0.0f,a.steering100,0.0f}};
    } else {
        if(!p.leftEnginePosition.present||!p.rightEnginePosition.present) return false;
        e.torqueBasis[0]=p.leftEnginePosition.value;e.torqueBasis[1]=p.rightEnginePosition.value;
        for(const auto& v:e.torqueBasis) for(float x:v.v) m.keep(x);
    }
    result.controls.speed1B84=get(K::SteerSpeed1);result.controls.scale1B88=get(K::SteerScale1);
    result.controls.speed2B8C=get(K::SteerSpeed2);result.controls.scale2B90=get(K::SteerScale2);
    result.rates.x=get(K::XAxisRateLimit);result.rates.y=get(K::YAxisRateLimit);

    if(context.applyProfileOverwrite) {
        if(!context.profileAbilitiesPresent) return false;
        const auto& q=context.profile;
        m.keep(q.thrust1920);m.keep(q.speed1924);m.keep(q.steering1928);
        const float minThrust=get(K::MinMaxThrust),maxThrust=get(K::MaxMaxThrust);
        const float minSpeed=get(K::MinTheoreticalTopSpeed),maxSpeed=get(K::MaxTheoreticalTopSpeed);
        e.maxThrust=m.clampedProfile(minThrust,maxThrust,q.thrust1920);
        //216204..216210: no clamp and reversed MUL operand order for units.
        e.theoreticalTopSpeed=m.mul(units,m.interpolate(minSpeed,maxSpeed,q.speed1924));
        const float low=get(K::MinSpoilerYawMoment),high=get(K::MaxSpoilerYawMoment),offset=get(K::SpoilerYawMomentOffset);
        const float minimum=m.add(low,offset),maximum=m.add(high,offset);
        a.steering100=m.clampedProfile(minimum,maximum,q.steering1928);
        //The already copied engine torque bases are intentionally NOT rebuilt.
    }
    result.engineDifference5C=m.sub(e.theoreticalTopSpeed,actualTopSpeed);
    if(!m.valid) return false;
    out=result;return true;
}

bool projectAuthoredCoupledParameters49N(const AuthoredBinding49N& b,
    const CoupledBodyParameters49M& previous,CoupledBodyParameters49M& out) noexcept {
    if(!nearest()||!finiteMapped(b)) return false;
    auto result=previous;
    if(b.massSetterWrote) {
        result.main.body.mass=b.main.body.mass;result.main.body.inverseMass=b.main.body.inverseMass;
    }
    for(unsigned c=0;c<3;++c) for(unsigned r=0;r<3;++r)
        result.main.body.inverseInertiaColumns[c].v[r]=b.inverseInertia140.v[4*c+r];
    result.main.engine=b.main.engine;copyAero(b.main.aero,result.main.aero);
    constexpr unsigned lanes[]={3,7,11,12,13,14,15};
    for(unsigned i=0;i<7;++i) result.mainSphere.inverseInertiaExtra[i]=b.inverseInertia140.v[lanes[i]];
    out=result;return true;
}
bool projectAuthoredGravity49N(const AuthoredBinding49N& b,
    const SubstepSelectionContext49K& previousSelection,const SecondaryInputs49M& previousSecondary,
    SubstepSelectionContext49K& outSelection,SecondaryInputs49M& outSecondary) noexcept {
    if(!nearest()||!std::isfinite(b.gravity378)||!std::isfinite(b.airborneGravity37C)) return false;
    auto selection=previousSelection;auto secondary=previousSecondary;
    selection.vehicle378=b.gravity378;selection.vehicle37C=b.airborneGravity37C;
    secondary.gravity378=b.gravity378;secondary.gravity37C=b.airborneGravity37C;
    outSelection=selection;outSecondary=secondary;return true;
}
}
