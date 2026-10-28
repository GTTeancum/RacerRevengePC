#include "source_acos49t.h"

namespace rr::game {
namespace {
// Typed source operations. Evidence/chariotAnimation49T/acos authenticates
// instruction ranges, operation order, constants and the bounded contract.
using U=std::uint32_t; using D=std::uint64_t; using I=std::int32_t;
constexpr D fracmask=(D(1)<<52)-1, one=0x3ff0000000000000ull;
struct Number { U kind=0,sign=0; I exponent=0; D fraction=0; };
// 30F8D0: exponent-zero inputs, including subnormals, are classified as zero.
static Number unpack(D x) {
 Number n; n.sign=U(x>>63); U e=U(x>>52)&2047; D f=x&fracmask;
 if(!e)n.kind=2;
 else if(e==2047){n.kind=f?((f&(D(1)<<51))?1:0):4;n.fraction=f;}
 else {n.kind=3;n.exponent=I(e)-1023;n.fraction=(f<<8)|(D(1)<<60);}
 return n;
}
// 30F7A0: source NaN marker is bit51; preserve the source payload.
static D pack(Number n) {
 D f=n.fraction; U e=0;
 if(n.kind<2){e=2047;f|=D(1)<<51;}
 else if(n.kind==4){e=2047;f=0;}
 else if(n.kind==2)f=0;
 else if(f) {
  if(n.exponent < -1022){U shift=U(-1022-n.exponent);f=shift<57?f>>shift:0;f>>=8;}
  else if(n.exponent>=1024){e=2047;f=0;}
  else {e=U(n.exponent+1023);if((f&255)==128){if(f&256)f+=128;}else f+=127;
   if(f>((D(1)<<61)-1)){f>>=1;++e;} f>>=8;}
 }
 return D(n.sign)<<63 | D(e&2047)<<52 | (f&fracmask);
}
static D sticky(D f){return (f>>1)|(f&1);}
static Number add_records(Number a,Number b) {
 if(a.kind<2)return a;
 if(b.kind<2)return b;
 if(a.kind==4){if(b.kind==4&&a.sign!=b.sign)return {};return a;}
 if(b.kind==4)return b;
 if(b.kind==2){if(a.kind==2)a.sign&=b.sign;return a;}
 if(a.kind==2)return b;
 I distance=a.exponent-b.exponent; I ad=distance<0?-distance:distance;
 if(ad<64){while(a.exponent<b.exponent){a.fraction=sticky(a.fraction);++a.exponent;}
  while(b.exponent<a.exponent){b.fraction=sticky(b.fraction);++b.exponent;}}
 else if(a.exponent>b.exponent)b.fraction=0;else {a.fraction=0;a.exponent=b.exponent;}
 Number r{3,a.sign,a.exponent,0};
 if(a.sign==b.sign)r.fraction=a.fraction+b.fraction;
 else {if(a.fraction>=b.fraction){r.fraction=a.fraction-b.fraction;r.sign=a.sign;}
  else {r.fraction=b.fraction-a.fraction;r.sign=b.sign;}
  if(!r.fraction)r.sign=0;
  while(r.fraction && r.fraction<(D(1)<<60)){r.fraction<<=1;--r.exponent;}}
 if(r.fraction>((D(1)<<61)-1)){r.fraction=sticky(r.fraction);++r.exponent;}
 return r;
}
static D add(D a,D b){return pack(add_records(unpack(a),unpack(b)));}
static D sub(D a,D b){auto y=unpack(b);y.sign^=1;return pack(add_records(unpack(a),y));}
static D mul(D x,D y) {
 auto a=unpack(x),b=unpack(y);U sign=a.sign^b.sign;
 if(a.kind<2){a.sign=sign;return pack(a);}if(b.kind<2){b.sign=sign;return pack(b);}
 if((a.kind==4&&b.kind==2)||(a.kind==2&&b.kind==4))return pack({});
 if(a.kind==4||a.kind==2){a.sign=sign;return pack(a);}
 if(b.kind==4||b.kind==2){b.sign=sign;return pack(b);}
 // Four 32x32 products match 30FC70's calls to 30EC30, and are portable
 // to compilers without a native 128-bit integer type.
 D al=U(a.fraction),ah=a.fraction>>32,bl=U(b.fraction),bh=b.fraction>>32;
 D ll=al*bl,lh=al*bh,hl=ah*bl,hh=ah*bh,cross=lh+hl;
 D lo=ll+(cross<<32),hi=hh+(cross>>32)+(D(cross<lh)<<32)+D(lo<ll);
 I e=a.exponent+b.exponent+4;
 // Preserve source limb normalization, including its conditional low shift.
 while(hi>((D(1)<<61)-1)){if(hi&1)lo=(lo>>1)|(D(1)<<63);hi>>=1;++e;}
 while(hi<(D(1)<<60)){hi=(hi<<1)|(lo>>63);lo<<=1;--e;}
 if((hi&255)==128 && ((hi&256)||lo))hi+=128;
 return pack({3,sign,e,hi});
}
static D div(D x,D y) {
 auto a=unpack(x),b=unpack(y);if(a.kind<2)return pack(a);if(b.kind<2)return pack(b);
 a.sign^=b.sign;
 if(a.kind==4||a.kind==2){if(a.kind==b.kind)return pack({});return pack(a);}
 if(b.kind==4){a.fraction=0;a.exponent=0;return pack(a);}
 if(b.kind==2){a.kind=4;return pack(a);}
 a.exponent-=b.exponent;D rem=a.fraction,den=b.fraction,q=0;
 if(rem<den){rem<<=1;--a.exponent;}
 for(D mask=D(1)<<60;mask;mask>>=1){if(rem>=den){q|=mask;rem-=den;}rem<<=1;}
 if((q&255)==128 && ((q&256)||rem))q+=128;
 a.fraction=q;return pack(a);
}
// 311038 -> 3105A8 -> 310410. Dead zero-record fields cannot affect pack.
static D from_float(U x) {
 Number n;n.sign=x>>31;U e=(x>>23)&255,f=x&0x7fffff;
 if(!e)n.kind=2;
 else if(e==255){n.kind=f?((f&0x100000)?1:0):4;n.fraction=D(f)<<30;}
 else {n.kind=3;n.exponent=I(e)-127;n.fraction=D((f<<7)|0x40000000)<<30;}
 return pack(n);
}
// 310440 -> 311008 -> 310498, preserving sticky compression and source rounding.
static U to_float(D x) {
 auto n=unpack(x);U f=U(n.fraction>>30);if(n.fraction&0x3fffffffull)f|=1;U e=0;
 if(n.kind<2){e=255;f|=0x100000;}
 else if(n.kind==4){e=255;f=0;}
 else if(n.kind==2)f=0;
 else if(f){if(n.exponent < -126){U s=U(-126-n.exponent);f=s<26?f>>s:0;f>>=7;}
  else if(n.exponent>=128){e=255;f=0;}
  else {e=U(n.exponent+127);if((f&127)==64){if(f&128)f+=64;}else f+=63;
   if(f&0x80000000u){f>>=1;++e;}f>>=7;}}
 return n.sign<<31 | (e&255)<<23 | (f&0x7fffff);
}
// 300198: bit-at-a-time fdlibm square-root loop, with the source's nearest
// rounding branch (both source comparisons here have exact operands 1,1).
static D sqrt(D x) {
 U hi=U(x>>32),lo=U(x);U ah=hi&0x7fffffff;
 if((hi&0x7ff00000)==0x7ff00000)return add(mul(x,x),x);
 if(!(ah|lo))return x;
 if(hi>>31)return div(sub(x,x),sub(x,x));
 I e=I(hi>>20); if(!e){while(!hi){e-=21;hi|=lo>>11;lo<<=21;}U i=0;while(!(hi&0x100000)){hi<<=1;++i;}e+=1-I(i);if(i){hi|=lo>>(32-i);lo<<=i;}}
 e-=1023;hi=(hi&0xfffff)|0x100000;if(e&1){hi=hi+hi+(lo>>31);lo+=lo;}
 // Source arithmetic shift rounds negative odd exponents toward minus infinity.
 e=e/2-(e<0 && e%2!=0);
 hi=hi+hi+(lo>>31);lo+=lo;U q=0,q1=0,s0=0,s1=0;
 for(U r=0x200000;r;r>>=1){U t=s0+r;if(t<=hi){s0=t+r;hi-=t;q+=r;}hi=hi+hi+(lo>>31);lo+=lo;}
 for(U r=0x80000000;r;r>>=1){U t1=s1+r,t=s0;if(t<hi||(t==hi&&t1<=lo)){s1=t1+r;if((t1&0x80000000)&&!(s1&0x80000000))++s0;hi-=t;if(lo<t1)--hi;lo-=t1;q1+=r;}hi=hi+hi+(lo>>31);lo+=lo;}
 if(hi|lo){if(q1==0xffffffffu){q1=0;++q;}else q1+=q1&1;}
 hi=(q>>1)+0x3fe00000+U(e*0x100000);lo=q1>>1;if(q&1)lo|=0x80000000;return D(hi)<<32|lo;
}
constexpr D p[]={0x3f023de10dfdf709ull,0x3f49efe07501b288ull,0xbfa48228b5688f3bull,0x3fc9c1550e884455ull,0xbfd4d61203eb6f7dull,0x3fc5555555555555ull};
constexpr D q[]={0x3fb3b8c5b12e9282ull,0xbfe6066c1b8d0159ull,0x40002ae59c598ac8ull,0xc0033a271c8a2d4bull};
constexpr D pi=0x400921fb54442d18ull,halfpi=0x3ff921fb54442d18ull,tail=0x3c91a62633145c07ull,half=0x3fe0000000000000ull;
static D ratio(D z) {D a=mul(z,p[0]);for(U i=1;i<6;++i)a=mul(z,add(a,p[i]));D b=mul(z,q[0]);for(U i=1;i<4;++i)b=mul(z,add(b,q[i]));return div(a,add(b,one));}
static D ieee_acos(D x) {
 U high=U(x>>32),abs=high&0x7fffffff;
 if(abs>=0x3ff00000){if(abs==0x3ff00000&&U(x)==0)return high>>31?pi:0;D d=sub(x,x);return div(d,d);}
 if(abs<0x3fe00000){if(abs<=0x3c600000)return halfpi;D z=mul(x,x);D r=ratio(z);return sub(halfpi,sub(x,sub(tail,mul(x,r))));}
 if(high>>31){D z=mul(add(x,one),half);D r=ratio(z);D s=sqrt(z);D w=sub(mul(r,s),tail);D t=add(s,w);return sub(pi,add(t,t));}
 D z=mul(sub(one,x),half);D s=sqrt(z),df=s&0xffffffff00000000ull;
 D c=div(sub(z,mul(df,df)),add(s,df));D r=ratio(z);D w=add(mul(r,s),c);D t=add(df,w);return add(t,t);
}
// Bounded original302580 sine: nonnegative input in the recovered range below3*pi/4.
// This is the actual legacy-MOT quaternion conversion's half-angle domain.
// All constants are original3C5FE0..3C61A8 words; elementary operations above
// remain the same source soft-double implementation, never host libm.
static D sine_kernel(D x) {
 const U high=U(x>>32)&0x7fffffffu;
 if(high<=0x3e3fffffu)return x;
 const D z=mul(x,x),v=mul(z,x);
 D r=add(mul(z,0x3de5d93a5acfd57cull),0xbe5ae5e68a2b9cebull);
 r=add(mul(z,r),0x3ec71de357b1fe7dull);
 r=add(mul(z,r),0xbf2a01a019c161d5ull);
 r=add(mul(z,r),0x3f8111111110f8a6ull);
 return add(x,mul(v,add(mul(z,r),0xbfc5555555555549ull)));
}
static D cosine_kernel(D x,D y) {
 const U high=U(x>>32)&0x7fffffffu;
 if(high<=0x3e3fffffu)return one;
 const D z=mul(x,x);
 D r=add(mul(z,0xbda8fae9be8838d4ull),0x3e21ee9ebdb4b1c4ull);
 r=add(mul(z,r),0xbe927e4f809c52adull);
 r=add(mul(z,r),0x3efa01a019cb1590ull);
 r=add(mul(z,r),0xbf56c16c16c15177ull);
 r=add(mul(z,r),0x3fa555555555554cull);
 r=mul(z,r);
 if(high<=0x3fd33332u)return sub(one,sub(mul(half,z),sub(mul(z,r),mul(x,y))));
 const D qx=high>0x3fe90000u?0x3fd2000000000000ull:D(high-0x00200000u)<<32;
 const D hz=sub(mul(half,z),qx),a=sub(one,qx);
 return sub(a,sub(hz,sub(mul(z,r),mul(x,y))));
}
static D sine_positive_reduced(D x) {
 const U high=U(x>>32);
 if(high<=0x3fe921fbu)return sine_kernel(x);
 D z=sub(x,0x3ff921fb54400000ull),tail=0x3dd0b4611a626331ull;
 if(high==0x3ff921fbu){z=sub(z,0x3dd0b4611a600000ull);tail=0x3ba3198a2e037073ull;}
 const D first=sub(z,tail),second=sub(sub(z,first),tail);
 return cosine_kernel(first,second);
}
} // namespace

bool sourceAcos49T(std::uint32_t inputBits, SourceLibmState49T& state,
    std::uint32_t& outputBits, SourceAcosTrace49T* trace) noexcept {
    if (state.mode != 1 || !state.defaultMatherr || !state.coldNanRecord)
        return false;
    const D widened = from_float(inputBits);
    const D inner = ieee_acos(widened);
    const D magnitude = widened & 0x7fffffffffffffffull;
    const bool nan = magnitude > 0x7ff0000000000000ull;
    const bool domain = !nan && magnitude > one;
    const D wrapped = domain ? 0 : inner;
    const U result = to_float(wrapped);
    if (domain) state.errnoValue = 33;
    outputBits = result;
    if (trace) *trace = {widened, inner, wrapped};
    return true;
}

bool sourceSinePositiveReduced50IG(std::uint32_t inputBits,std::uint32_t& outputBits) noexcept {
 // Preserve source exponent-zero conversion; reject foreign domains rather
 // than replacing source range reduction with a host sine approximation.
 if(inputBits&0x80000000u)return false;
 const D x=from_float(inputBits);
 if(U(x>>32)>0x4002d97bu)return false;
 outputBits=to_float(sine_positive_reduced(x));return true;
}

std::uint32_t SourceUiLightMath50KG::range146898()noexcept {
    //302BA0's finite positive wrapper reaches300198 with no errno mutation.
    return to_float(sqrt(0x47efffffe0000000ull));
}
bool SourceUiLightMath50KG::luminance146E60(std::uint32_t r,std::uint32_t g,std::uint32_t b,
    std::uint32_t& output)noexcept {
    // This bounded consumer admits only finite RGB in [0,128]. Unknown alpha
    // is deliberately not an input to146E60. False preserves output.
    if(r>0x43000000u||g>0x43000000u||b>0x43000000u)return false;
    const D blue=mul(0x3fbd2f1a9fbe76c9ull,from_float(b));
    const D green=mul(0x3fe2c8b439581062ull,from_float(g));
    const D sum=add(blue,green);
    const D red=mul(0x3fd322d0e5604189ull,from_float(r));
    output=to_float(add(red,sum));return true;
}

bool SourceUiLightMath50KG::luminanceRaRemaining146E60(std::uint32_t r,std::uint32_t g,std::uint32_t b,
    std::uint32_t& output)noexcept {
    //Exact admitted half-RGB triples from retained RA source rows18..185:
    //(-80),(-8),(-50),(-60),(200),(150), and(186,186,165), each /2.
    //No positivity clamp or broader UI/parser admission. False preserves out.
    const bool gray=r==g&&g==b&&(r==0xc2200000u||r==0xc0800000u||r==0xc1c80000u||
        r==0xc1f00000u||r==0x42c80000u||r==0x42960000u);
    if(!gray&&!(r==0x42ba0000u&&g==0x42ba0000u&&b==0x42a50000u))return false;
    //Same signed source software-double widen/multiply/add/narrow operations
    //and original146E60 order/constants as the existing nonnegative caller.
    const D blue=mul(0x3fbd2f1a9fbe76c9ull,from_float(b));
    const D green=mul(0x3fe2c8b439581062ull,from_float(g));
    const D sum=add(blue,green);
    const D red=mul(0x3fd322d0e5604189ull,from_float(r));
    output=to_float(add(red,sum));return true;
}

#ifdef RR_SOURCE_ACOS49T_TEST_API
namespace source_acos49t_test {
std::uint64_t arithmetic(Operation op, std::uint64_t a, std::uint64_t b) noexcept {
    switch (op) {
    case Operation::add: return add(a, b);
    case Operation::subtract: return sub(a, b);
    case Operation::multiply: return mul(a, b);
    case Operation::divide: return div(a, b);
    }
    return 0;
}
std::uint64_t widen(std::uint32_t input) noexcept { return from_float(input); }
std::uint32_t narrow(std::uint64_t input) noexcept { return to_float(input); }
std::uint64_t squareRoot(std::uint64_t input) noexcept { return sqrt(input); }
}
#endif
} // namespace rr::game
