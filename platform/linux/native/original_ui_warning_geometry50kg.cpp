#include "original_ui_warning_geometry50kg.h"
#include <bit>
#include <cfenv>
#include <limits>

namespace rr::platform {
namespace {
using Kind=OriginalUiWarningOperationKind50KG;
using Status=OriginalUiWarningGeometryStatus50KG;
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
    std::numeric_limits<float>::digits==24&&std::numeric_limits<float>::max_exponent==128);

struct PositionPCs {std::uint32_t x,y,dirty,rotated,compareX,compareY,subtract;};
struct HeightPCs {std::uint32_t height,dirty,rotated,compare;};

class Planner final {
    OriginalUiWarningValues50KG values_;
    OriginalUiWarningGeometryPlan50KG plan_;

    void unsupported(std::uint32_t pc)noexcept {
        if(plan_.status==Status::planned){plan_.status=Status::unsupportedArithmetic;plan_.unsupportedPC=pc;}
    }
    bool scalar(float x,std::uint32_t pc)noexcept {
        // Reject unsupported bit classes without relying on host FTZ/DAZ or
        // compiler assumptions about isfinite. Do not silently flush them.
        const auto bits=std::bit_cast<std::uint32_t>(x)&0x7fffffffu;
        if(bits>=0x7f800000u||(bits!=0&&bits<0x00800000u)){unsupported(pc);return false;}
        return true;
    }
    bool equal(float a,float b,std::uint32_t pc)noexcept {
        if(!scalar(a,pc)||!scalar(b,pc))return false;
        return a==b; // +0 and -0 compare equal, as the source c.eq.s does.
    }
    bool leZero(float x,std::uint32_t pc)noexcept {
        return scalar(x,pc)&&x<=0.f;
    }
    bool arithmetic(float a,float b,std::uint32_t pc)noexcept {
        if(!scalar(a,pc)||!scalar(b,pc))return false;
        if(std::fegetround()!=FE_TONEAREST){unsupported(pc);return false;}
        return true;
    }
    float sub(float a,float b,std::uint32_t pc)noexcept {
        if(!arithmetic(a,b,pc))return 0;
        const volatile float left=a,right=b;
        const volatile float result=left-right;
        const float value=result;
        // Detect a host flushing a nonzero cancellation result to zero.
        if(value==0.f&&a!=b)unsupported(pc);
        scalar(value,pc);return value;
    }
    float add(float a,float b,std::uint32_t pc)noexcept {
        if(!arithmetic(a,b,pc))return 0;
        const volatile float left=a,right=b;
        const volatile float result=left+right;
        const float value=result;
        if(value==0.f&&a!=-b)unsupported(pc);
        scalar(value,pc);return value;
    }
    float mul(float a,float b,std::uint32_t pc)noexcept {
        if(!arithmetic(a,b,pc))return 0;
        const volatile float left=a,right=b;
        const volatile float result=left*right;
        const float value=result;
        if(value==0.f&&a!=0.f&&b!=0.f)unsupported(pc);
        scalar(value,pc);return value;
    }
    float divideQuarter(float a,std::uint32_t pc)noexcept {
        if(!arithmetic(a,.25f,pc))return 0;
        const volatile float left=a,right=.25f;
        const volatile float result=left/right;
        const float value=result;
        scalar(value,pc);return value;
    }
    void emit(Kind kind,std::uint32_t pc,std::uint32_t receiver,
        std::uint32_t bits=0,std::uint8_t asserted=0)noexcept {
        if(plan_.status!=Status::planned)return;
        if(plan_.operationCount==plan_.operations.size()){
            plan_.status=Status::operationCapacity;plan_.unsupportedPC=pc;return;
        }
        plan_.operations[plan_.operationCount++]={kind,pc,receiver,bits,asserted};
    }
    static std::uint32_t offset(std::size_t strip)noexcept {
        return 0x40u+static_cast<std::uint32_t>(strip)*0x60u;
    }
    void writeFloat(Kind kind,std::uint32_t pc,std::size_t strip,float x)noexcept {
        emit(kind,pc,offset(strip),std::bit_cast<std::uint32_t>(x));
    }
    void dirty(std::size_t strip,std::uint32_t pc,std::uint32_t rotatedPC)noexcept {
        auto& q=values_.strips[strip];
        // Each original sb occurs even when its bit was already set. Preserve
        // these repeated stores and every unrelated bit; never coalesce them.
        q.flags5CValue=static_cast<std::uint8_t>((q.flags5CValue&0xfeu)|1u);
        emit(Kind::flags5C,pc,offset(strip),q.flags5CValue,1);
        if(!equal(0.f,q.rotation3C,pc+8u)){
            q.flags5CValue=static_cast<std::uint8_t>((q.flags5CValue&0xfdu)|2u);
            emit(Kind::flags5C,rotatedPC,offset(strip),q.flags5CValue,2);
        }
    }
    bool position(std::size_t strip,float baseY,bool top,const PositionPCs& pc)noexcept {
        auto& q=values_.strips[strip];
        // The top edge's -16 is evaluated for the equality branch and again
        // for the changed-position store, just as in the source blocks.
        if(equal(q.positionX10,0.f,pc.compareX)){
            const float wantedY=top?sub(baseY,16.f,pc.subtract):baseY;
            if(equal(q.positionY14,wantedY,pc.compareY))return false;
        }
        q.positionX10=0.f;writeFloat(Kind::positionX10,pc.x,strip,0.f);
        const float wantedY=top?sub(baseY,16.f,pc.y-12u):baseY;
        q.positionY14=wantedY;writeFloat(Kind::positionY14,pc.y,strip,wantedY);
        dirty(strip,pc.dirty,pc.rotated);
        return true;
    }
    void height(std::size_t strip,float wanted,const HeightPCs& pc)noexcept {
        auto& q=values_.strips[strip];
        if(equal(q.height38,wanted,pc.compare))return;
        q.height38=wanted;writeFloat(Kind::height38,pc.height,strip,wanted);
        dirty(strip,pc.dirty,pc.rotated);
    }
    void animate()noexcept {
        const bool opening=values_.active298==0;
        float fraction=divideQuarter(values_.timer294,opening?0x204B3Cu:0x204B60u);
        if(opening)fraction=sub(1.f,fraction,0x204B50);
        const float scaled=mul(60.f,fraction,0x204B70);
        // 204B80 min.s is an upper cap only. There is NO zero lower clamp.
        const float half=scaled<60.f?scaled:60.f;
        const float low=sub(216.f,half,0x204BB4);
        const float high=add(216.f,half,0x204BC4);
        position(0,low,true,{0x204BF0,0x204C00,0x204C14,0x204C40,0x204BB8,0x204BD8,0x204BD4});
        height(0,16.f,{0x204C64,0x204C80,0x204CAC,0x204C54});
        position(1,high,false,{0x204CE0,0x204CE4,0x204D00,0x204D2C,0x204CBC,0x204CD0,0});
        height(1,16.f,{0x204D50,0x204D6C,0x204D98,0x204D40});
        const bool movedMiddle=position(2,low,false,
            {0x204DCC,0x204DD0,0x204DEC,0x204E18,0x204DA8,0x204DBC,0});
        // Preserve subtraction of independently rounded edges, not 2*half.
        const float middleHeight=sub(high,low,movedMiddle?0x204E1Cu:0x204DC8u);
        height(2,middleHeight,{0x204E34,0x204E50,0x204E7C,0x204E24});

        // 204E8C/EA4 is the RENDER decrement, independent of 2049CC/9D0.
        // Do not fold it into the update or clamp a negative result to zero.
        if(!leZero(values_.timer294,0x204E8C)){
            values_.timer294=sub(values_.timer294,values_.delta290,0x204EA0);
            emit(Kind::timer294,0x204EA4,0,std::bit_cast<std::uint32_t>(values_.timer294));
        }
        if(!leZero(values_.timer294,0x204EB4))return;
        if(opening)snapOpen();else snapClosed();
    }
    void snapOpen()noexcept {
        const float low=sub(216.f,60.f,0x204ED4);
        const float high=add(216.f,60.f,0x204EE4);
        position(2,low,false,{0x204EFC,0x204F00,0x204F1C,0x204F48,0x204ED8,0x204EEC,0});
        const float middleHeight=sub(high,low,0x204F50);
        height(2,middleHeight,{0x204F64,0x204F80,0x204FAC,0x204F54});
        position(0,low,true,{0x204FF4,0x205004,0x205018,0x205044,0x204FBC,0x204FDC,0x204FD8});
        position(1,high,false,{0x205078,0x20507C,0x205098,0x2050C8,0x205054,0x205068,0});
        // The source open snap does not store top/bottom heights again.
    }
    void snapClosed()noexcept {
        const float low=216.f,high=216.f; // 2050D0/2050E0 mov.s.
        position(2,low,false,{0x2050F8,0x2050FC,0x205118,0x205144,0x2050D4,0x2050E8,0});
        const float middleHeight=sub(high,low,0x20514C);
        height(2,middleHeight,{0x205160,0x20517C,0x2051A8,0x205150});
        position(0,low,true,{0x2051F0,0x205200,0x205214,0x205240,0x2051B8,0x2051D8,0x2051D4});
        position(1,high,false,{0x205274,0x205278,0x205294,0x2052C0,0x205250,0x205264,0});
        height(0,0.f,{0x2052E0,0x2052F8,0x205324,0x2052D0});
        height(1,0.f,{0x205344,0x20535C,0x205388,0x205334});
        height(2,0.f,{0x2053A8,0x2053C0,0x2053EC,0x205398});
    }
public:
    explicit Planner(const OriginalUiWarningValues50KG& values)noexcept:values_(values){}
    OriginalUiWarningGeometryPlan50KG run()noexcept {
        const bool primaryMissing=!values_.primaryPresent29C&&values_.primaryEnabled474;
        const bool secondaryMissing=!values_.secondaryPresent2A0&&values_.secondaryEnabled478;
        if(primaryMissing||secondaryMissing){
            // 204B1C jumps directly to dim submission for timer<=0. It does
            // not snap, decrement, normalize dirty flags or touch strip data.
            if(!leZero(values_.timer294,0x204B14))animate();
            emit(Kind::callDim2E21D0,0x2053F4,0x160);
            if(leZero(values_.timer294,0x205408)){
                if(primaryMissing)emit(Kind::callText2E0170,0x205434,0x1B0);
                else if(secondaryMissing)emit(Kind::callText2E0170,0x205460,0x220);
            }
            for(std::size_t i=0;i<3;++i)
                emit(Kind::callStrip2DF550,0x205484,offset(i)); // Includes zero-area strips.
        }
        emit(Kind::callInherited141D70,0x2054A4,0);
        if(plan_.status!=Status::planned){
            const auto status=plan_.status;const auto pc=plan_.unsupportedPC;
            plan_={};plan_.status=status;plan_.unsupportedPC=pc;
        }
        return plan_;
    }
};
}
OriginalUiWarningGeometryPlan50KG planOriginalUiWarning204AC050KG(
    const OriginalUiWarningValues50KG& values)noexcept {
    return Planner(values).run();
}
}
