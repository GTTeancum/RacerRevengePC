#pragma once
// Included only by the private Commands header. Native binary32 admission is
// deliberately narrower than universal EE/FCSR semantics; no target execution.
namespace original_world_sky50LA {
inline bool Commands::buildGeometry12ACD0Or12B0D0(Plain133F80& p,
    const CellInput& in,std::uint32_t& pc) noexcept {
    pc=in.policy36C0F0?0x12acd0:0x12b0d0;
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ != 0)
    (void)p;(void)in;return false;
#else
    if(std::fegetround()!=FE_TONEAREST||p.geometryAttempted||!current(p)||
       in.policy36C0F0>1||in.subdivisionsX!=3||in.subdivisionsY!=2||
       in.face>=6||in.row>=4||in.column>=4)return false;
    // Authenticate the caller's actual produced grid lanes, not the native
    // flat mesh (+1024Y) or a recomputed alternate mesh. All admission arithmetic
    // here is exact bounded integer arithmetic converted to binary32.
    const auto grid=[&](std::size_t column,std::size_t row) noexcept {
        const int right=(int(column)*128-256)*100;
        const int up=(256-int(row)*128)*100;
        switch(in.face) {
        case 0:return std::array<float,3>{{float(right),float(up),25600.f}};
        case 1:return std::array<float,3>{{-25600.f,float(up),float(right)}};
        case 2:return std::array<float,3>{{float(-right),float(up),-25600.f}};
        case 3:return std::array<float,3>{{25600.f,float(up),float(-right)}};
        case 4:return std::array<float,3>{{float(right),25600.f,float(-up)}};
        default:return std::array<float,3>{{float(right),-25600.f,float(up)}};
        }
    };
    const auto match=[](const auto& actual,const auto& expected) noexcept {
        for(unsigned a=0;a<3;++a)if(!std::isfinite(actual[a])||actual[a]!=expected[a])return false;
        return true;
    };
    if(!match(in.topLeft,grid(in.column,in.row))||!match(in.right,grid(in.column+1,in.row))||
       !match(in.below,grid(in.column,in.row+1))||!match(in.diagonal,grid(in.column+1,in.row+1)))return false;
    p.geometryAttempted=true;p.geometryActive=true;p.geometryPolicy=in.policy36C0F0;
    const auto fail=[&](std::uint32_t at) noexcept {
        pc=at;p.failurePC=at;p.geometryActive=false;return false;
    };
    // 1340AC precedes306478 in the delay slot. Source capacity precedes allocation.
    p.field2C=1;p.header.fill(0);p.headerKnown=true;p.field24=128;
    pc=0x1340c4;
    try {p.buffer20.reset(static_cast<std::uint8_t*>(::operator new(512,std::align_val_t(64))));}
    catch(...) {return fail(pc);}
    // Stronger native alignment, not a source305500 call. Untouched tail bytes
    // stay uninitialized; only the exact source stores below initialize backing.
    p.field28=0;
    // Serialized source134xxx scratch is private native local state. Never
    // claim/reset guest3E1C28/30/38/40 or diagnostic-static frontend history.
    std::uint32_t placeholder=0,target=0,blockCount=0,format=0;
    bool blockOpen=false;
    const auto store=[&](std::uint32_t index,std::uint32_t word) noexcept {
        if(index>=128)return false;
        for(unsigned i=0;i<4;++i)p.buffer20.get()[4*index+i]=std::uint8_t(word>>(8*i));
        return true;
    };
    const auto put=[&](std::uint32_t word) noexcept {
        if(p.field28>=128)return false;
        store(p.field28,word);++p.field28;return true;
    };
    const auto reserve=[&](std::int32_t address,std::uint32_t fmt) noexcept {
        if(blockOpen||p.field28>=128)return false;
        if(address!=-1)target=std::uint32_t(address);
        placeholder=p.field28++;format=fmt;blockCount=0;blockOpen=true;
        return true; //1345D0 reserves without writing placeholder yet.
    };
    const auto quad=[&](std::uint32_t a,std::uint32_t b,std::uint32_t c,std::uint32_t d) noexcept {
        if(!blockOpen||p.field28>124)return false;
        put(a);put(b);put(c);put(d);blockCount+=4;return true;
    };
    const auto pair=[&](std::uint32_t a,std::uint32_t b) noexcept {
        if(!blockOpen||p.field28>126)return false;
        put(a);put(b);blockCount+=4; //actual134550 counter, NOT copied-word count2
        return true;
    };
    const auto end=[&]() noexcept {
        if(!blockOpen||!blockCount)return false;
        const auto qwords=(blockCount+3)/4;
        if(qwords>256||target>0x7fff)return false;
        const auto count=qwords==256?0u:qwords;
        if(!store(placeholder,((format|0x60u)<<24)|(count<<16)|0x8000u|target))return false;
        target+=qwords;blockOpen=false;return true;
    };
    const auto bits=[](float f) noexcept {std::uint32_t u;std::memcpy(&u,&f,4);return u;};
    // Force each reached source scalar arithmetic operation to binary32. This
    // explicit FE_TONEAREST native domain does not establish original FCSR.
    const auto add=[](float a,float b) noexcept {volatile float v=a+b;return v;};
    const auto sub=[](float a,float b) noexcept {volatile float v=a-b;return v;};
    const auto mul=[](float a,float b) noexcept {volatile float v=a*b;return v;};
    const auto div=[](float a,float b) noexcept {volatile float v=a/b;return v;};
    float dx=0.f,dy=0.f,dz=0.f;
    std::uint32_t axis=2;
    if(in.face==0||in.face==2) {
        axis=0;dx=div(sub(in.right[0],in.topLeft[0]),3.f);
        dy=div(sub(in.below[1],in.topLeft[1]),2.f);
    } else if(in.face==1||in.face==3) {
        axis=1;dz=div(sub(in.right[2],in.topLeft[2]),3.f);
        dy=div(sub(in.below[1],in.topLeft[1]),2.f);
    } else {
        dx=div(sub(in.right[0],in.topLeft[0]),3.f);
        dz=div(sub(in.below[2],in.topLeft[2]),2.f);
    }
    const float du=div(.9375f,3.f),dv=div(.9375f,2.f);
    if(!std::isfinite(dx)||!std::isfinite(dy)||!std::isfinite(dz))return fail(0x12af40);
    const std::uint64_t prim=0x1cu|((in.face<4&&in.row>=2)?0x20u:0u);
    const std::uint64_t tag=0x8008u|(std::uint64_t(1)<<46)|(prim<<47)|(std::uint64_t(3)<<60);
    const auto gif=[&]() noexcept {
        return quad(std::uint32_t(tag),std::uint32_t(tag>>32),0x412u,0);
    };
    if(in.policy36C0F0) {
        pc=0x12af48;
        if(!put(0x11000000)||!reserve(0,12)||!gif()||!end()||!reserve(-1,12))return fail(pc);
        pc=0x12afc8;
        if(!quad(bits(in.topLeft[0]),bits(in.topLeft[1]),bits(in.topLeft[2]),axis)||
           !quad(bits(in.diagonal[0]),bits(in.diagonal[1]),bits(in.diagonal[2]),axis)||
           !quad(bits(.03125f),bits(.03125f),0,0)||
           !quad(bits(dx),bits(dy),bits(dz),0)||!quad(bits(du),bits(dv),3,2)||
           !end()||!put(0x17000000))return fail(pc);
    } else {
        //12B39C..6D8:2 strips,8XYZ records and8UV pairs per strip. Second UV
        // ordinate is previous ordinate+dv, not a separately recomputed product.
        for(std::uint32_t row=0;row<2;++row) {
            pc=0x12b3a4;
            if(!reserve(0,12)||!gif()||!end()||!reserve(-1,12))return fail(pc);
            const float ya=mul(dy,float(row)),yb=mul(dy,float(row+1));
            const float za=mul(dz,float(row)),zb=mul(dz,float(row+1));
            for(std::uint32_t column=0;column<4;++column) {
                std::array<float,3> a=in.topLeft,b=in.topLeft;
                if(axis==0) {
                    const float x=mul(dx,float(column));a[0]=b[0]=add(in.topLeft[0],x);
                    a[1]=add(in.topLeft[1],ya);b[1]=add(in.topLeft[1],yb);
                } else if(axis==1) {
                    const float z=mul(dz,float(column));a[2]=b[2]=add(in.topLeft[2],z);
                    a[1]=add(in.topLeft[1],ya);b[1]=add(in.topLeft[1],yb);
                } else {
                    const float x=mul(dx,float(column));a[0]=b[0]=add(in.topLeft[0],x);
                    a[2]=add(in.topLeft[2],za);b[2]=add(in.topLeft[2],zb);
                }
                pc=0x12b4a0;
                if(!quad(bits(a[0]),bits(a[1]),bits(a[2]),14)||
                   !quad(bits(b[0]),bits(b[1]),bits(b[2]),14))return fail(pc);
            }
            pc=0x12b608;
            if(!end()||!reserve(17,4))return fail(pc);
            const float v=add(.03125f,mul(dv,float(row)));
            for(std::uint32_t column=0;column<4;++column) {
                const float u=add(.03125f,mul(du,float(column)));
                if(!pair(bits(u),bits(v))||!pair(bits(u),bits(add(v,dv))))return fail(0x12b680);
            }
            if(!end()||!put(0x17000000))return fail(0x12b6c4);
        }
    }
    pc=in.policy36C0F0?0x12b088:0x12b6e4;
    const auto expected=in.policy36C0F0?28u:112u;
    if(blockOpen||p.field28!=expected||p.field28>p.field24)return fail(pc);
    //1342B0:both bounded counts already16-byte aligned. No uninitialized tail
    // is read, padded or exposed. Source finalheader04 native pointer is separate.
    p.header[0]=0x30000000u|(expected/4);p.nativeHeader04=p.buffer20.get();
    p.geometryComplete=true;p.geometryActive=false;p.failurePC=0;
    pc=in.policy36C0F0?0x12b0c4:0x12b73c;return true;
#endif
}
}
