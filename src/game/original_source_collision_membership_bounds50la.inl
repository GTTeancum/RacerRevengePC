//Included inside OriginalSourcePhysicsCollision50LA. One genuine1202E0
//Type0 occurrence under the existing live post-pose update custody. No saved
//output pointers: caller-owned scratch is copied out only at source stores.
    struct SourceMembershipBounds1202E0 {
        bool attempted=false,returned=false;
        const void* updateOccurrence=nullptr;
        const void* outerIdentity=nullptr;
        const void* shapeIdentity=nullptr;
        const void* nodeIdentity=nullptr;
        std::uint32_t pc=0;
        struct Read {std::uint32_t pc=0,offset=0,bits=0;std::uint8_t known=0;bool readable=false;};
        std::array<Read,6> reads{};
        unsigned readCount=0,matrixWords=0;
        std::array<std::uint32_t,4> localCenter{{0,0,0,0x3f800000u}},localHalf{{0,0,0,0x3f800000u}};
        std::array<std::uint32_t,16> composed{};
        std::uint16_t composedKnown=0;
        std::array<std::uint32_t,4> centerBefore{},halfBefore{},centerAfter{},halfAfter{};
        bool centerStored=false;
        std::uint8_t halfStoreMask=0;
    } membershipBounds_;
    bool boundsHistoryNative()const noexcept {
        const auto& h=membershipBounds_;const auto& u=sourceType0_.sourceUpdate_;
        if(!h.attempted)return !h.returned&&!h.updateOccurrence&&!h.outerIdentity&&!h.shapeIdentity&&
            !h.nodeIdentity&&!h.pc&&!h.readCount&&!h.matrixWords&&!h.composedKnown&&!h.centerStored&&!h.halfStoreMask;
        if(!h.updateOccurrence||!h.outerIdentity||!h.shapeIdentity||h.updateOccurrence!=postUpdate_.updateOccurrence||
           h.outerIdentity!=postUpdate_.outerIdentity||h.shapeIdentity!=u.shape||!postUpdate_.returned||
           !u.returned||h.readCount>6||h.matrixWords>16||h.halfStoreMask>7||
           (h.halfStoreMask&(h.halfStoreMask+1u))||h.localCenter[3]!=0x3f800000u||h.localHalf[3]!=0x3f800000u||
           (h.halfStoreMask&&!h.centerStored)||h.halfAfter[3]!=h.halfBefore[3])return false;
        for(unsigned i=0;i<h.readCount;++i){const auto& r=h.reads[i];const unsigned axis=i%3;
            if(r.offset!=(i<3?0x0cu:0x18u)+4u*axis||r.pc!=(i<3?0x12038cu:0x1203b0u)+8u*axis||
               r.known>15||(!r.readable&&r.known)||
               (i+1<h.readCount&&(!r.readable||r.known!=15)))return false;
            if(r.known==15&&(i<3?h.localCenter[axis]:h.localHalf[axis])!=r.bits)return false;
        }
        if(h.matrixWords&&(h.readCount!=6||h.reads[5].known!=15))return false;
        const auto wordMask=h.matrixWords==16?0xffffu:((1u<<h.matrixWords)-1u);
        if(h.composedKnown&~wordMask)return false;
        for(unsigned i=0;i<h.matrixWords;++i){
            std::uint32_t bits;std::memcpy(&bits,&u.composed00.v[i],4);
            if(h.composed[i]!=bits||bool(h.composedKnown&(1u<<i))!=bool(u.composedKnown&(1u<<i)))return false;
        }
        if(h.centerStored&&(h.matrixWords!=16||h.composedKnown!=0xffffu||h.centerAfter[3]!=0x3f800000u))return false;
        if(!h.centerStored&&h.centerAfter!=h.centerBefore)return false;
        for(unsigned i=0;i<3;++i)if(!(h.halfStoreMask&(1u<<i))&&h.halfAfter[i]!=h.halfBefore[i])return false;
        if(h.returned&&(!h.centerStored||h.halfStoreMask!=7||h.pc!=0x120e68u))return false;
        if(!u.nativeWithdrawn){
            const auto& n=sourceType0_.sourceNodes_.nodes;
            if(!u.live||!collision_||h.outerIdentity!=collision_.get()||h.nodeIdentity!=n.raw.get())return false;
            for(unsigned i=0;i<h.readCount;++i){const auto& r=h.reads[i];
                const bool readable=n.raw&&n.bytes>=std::size_t(r.offset)+4&&n.known.size()==n.bytes;
                if(readable!=r.readable)return false;
                if(readable){std::uint32_t bits;std::memcpy(&bits,n.raw.get()+r.offset,4);std::uint8_t known=0;
                    for(unsigned k=0;k<4;++k)if(n.known[r.offset+k])known|=std::uint8_t(1u<<k);
                    if(bits!=r.bits||known!=r.known)return false;}
            }
        }
        return true; //After withdrawal only immutable copied history is consulted.
    }
    Result boundsSourceType0_1202E0(const void* updateOccurrence,
        std::array<std::uint32_t,4>& center,std::array<std::uint32_t,4>& half)noexcept {
        auto& h=membershipBounds_;const auto& u=sourceType0_.sourceUpdate_;
        //Native custody/output-alias admission, not original source branching.
        if(h.attempted||!updateOccurrence||&center==&half||nativeWithdrawn_||!collision_||
           !postUpdate_.returned||postUpdate_.pc!=0x11ff5cu||postUpdate_.updateOccurrence!=updateOccurrence||
           !u.live||u.nativeWithdrawn||!u.returned||!postUpdateHistoryNative()||
           !sourceType0_.preflightSourceType0UpdateNative()||collision_->shape88_!=0||
           !collision_->shape8C_||*collision_->shape8C_!=u.shape)return {Status::owner,0x1202e0u};
        h.attempted=true;h.updateOccurrence=updateOccurrence;h.outerIdentity=collision_.get();h.shapeIdentity=u.shape;
        h.centerBefore=h.centerAfter=center;h.halfBefore=h.halfAfter=half;h.pc=0x120378u;
        const auto stop=[&](std::uint32_t pc)noexcept {h.pc=pc;return Result{Status::bounded,pc};};
        const auto& n=sourceType0_.sourceNodes_.nodes;
        h.pc=0x120388u;h.nodeIdentity=n.raw.get(); //Actual shape114 first-node association, not semantic mesh.
        for(unsigned i=0;i<6;++i){const unsigned axis=i%3;auto& r=h.reads[h.readCount++];
            r.pc=(i<3?0x12038cu:0x1203b0u)+8u*axis;r.offset=(i<3?0x0cu:0x18u)+4u*axis;h.pc=r.pc;
            r.readable=n.raw&&n.bytes>=std::size_t(r.offset)+4&&n.known.size()==n.bytes;
            if(!r.readable)return stop(r.pc);
            std::memcpy(&r.bits,n.raw.get()+r.offset,4);
            for(unsigned k=0;k<4;++k)if(n.known[r.offset+k])r.known|=std::uint8_t(1u<<k);
            if(r.known!=15)return stop(r.pc);
            (i<3?h.localCenter[axis]:h.localHalf[axis])=r.bits;
        }
        //Eight raw pairs copied from current shape00. world80/cached60 are not inputs.
        for(unsigned i=0;i<16;++i){h.pc=(i&1)?0x1203e4u:0x1203e0u;
            std::memcpy(&h.composed[i],&u.composed00.v[i],4);++h.matrixWords;
            h.composedKnown|=std::uint16_t(u.composedKnown&(1u<<i));}
        //Native finite-nearest binary32 adaptation, same separate products/adds
        //as collection_bounds48s. Its atomic whole-result facade cannot be used:
        //center SQ precedes the three half transforms and partial half stores.
        float m[16],c[4],extent[4];std::memcpy(m,h.composed.data(),64);
        std::memcpy(c,h.localCenter.data(),16);std::memcpy(extent,h.localHalf.data(),16);
        const auto transform=[&](const float* v,unsigned axis,bool point,std::uint32_t base,float& out)noexcept {
            h.pc=base;
            if(std::fegetround()!=FE_TONEAREST)return false;
            float value=0;
            for(unsigned term=0;term<3;++term){h.pc=base+4u*term;
                if(!(h.composedKnown&(1u<<(4u*term+axis)))||!std::isfinite(m[4u*term+axis])||!std::isfinite(v[term]))return false;
                volatile float product=m[4u*term+axis]*v[term];if(!std::isfinite(product))return false;
                if(!term)value=product;
                else {volatile float sum=value+product;if(!std::isfinite(sum))return false;value=sum;}
            }
            h.pc=base+12u;
            if(point&&(!(h.composedKnown&(1u<<(12u+axis)))||!std::isfinite(m[12u+axis])))return false;
            volatile float sum=value+(point?m[12u+axis]:0.f);if(!std::isfinite(sum))return false;
            out=sum;return true;
        };
        float projected[4]={0,0,0,1};
        for(unsigned axis=0;axis<3;++axis)if(!transform(c,axis,true,0x120cc4u,projected[axis]))return stop(h.pc);
        h.pc=0x120cd4u;std::memcpy(center.data(),projected,16);h.centerAfter=center;h.centerStored=true;
        //Actual three vector transforms finish before the first halfXYZ store.
        float axes[3][3];
        for(unsigned term=0;term<3;++term){float v[3]={0,0,0};v[term]=extent[term];
            for(unsigned axis=0;axis<3;++axis)if(!transform(v,axis,false,0x120cf8u+0x34u*term,axes[term][axis]))return stop(h.pc);}
        constexpr std::uint32_t sums[3]={0x120db4u,0x120dfcu,0x120e40u};
        constexpr std::uint32_t stores[3]={0x120dbcu,0x120e04u,0x120e48u};
        for(unsigned axis=0;axis<3;++axis){h.pc=sums[axis];
            volatile float xy=std::fabs(axes[0][axis])+std::fabs(axes[1][axis]);if(!std::isfinite(xy))return stop(h.pc);
            h.pc+=4;volatile float sum=std::fabs(axes[2][axis])+xy;if(!std::isfinite(sum))return stop(h.pc);
            h.pc=stores[axis];const float value=sum;std::memcpy(&half[axis],&value,4);
            h.halfAfter=half;h.halfStoreMask|=std::uint8_t(1u<<axis);
        }
        h.returned=true;h.pc=0x120e68u;return {Status::reached,h.pc};
    }
