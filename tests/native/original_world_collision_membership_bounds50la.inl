// Included inside OriginalWorldCollisionObjects50LA. Actual1202E0 type0
// occurrence after the second11F680, not PhysicsE0/post-pose ownership.
    static bool membershipBoundsCurrentNative(const SourceCallback& h)noexcept {
        const auto& m=h.membership;const auto& b=m.bounds;
        constexpr std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
        if(!b.entered)return !b.returned&&!b.shape&&!b.node&&!b.reads&&!b.matrixWords&&
            !b.pc&&!b.centerStored&&!b.halfStores&&m.pc==0x1202e0u&&m.center==vf0&&m.half==vf0;
        if(!h.loaderPath.partialType0_||b.shape!=h.loaderPath.partialType0_.get()||
           b.reads>6||b.matrixWords>16||b.halfStores>7||
           (b.halfStores&(b.halfStores+1u))||b.localCenter[3]!=vf0[3]||b.localHalf[3]!=vf0[3]||
           (b.matrixWords&&b.reads!=6)||(b.centerStored&&b.matrixWords!=16)||
           (b.halfStores&&!b.centerStored))return false;
        const auto& shape=*h.loaderPath.partialType0_;const auto& n=shape.payload.records.nodes;
        if(b.node!=n.raw.get())return false;
        for(unsigned i=0;i<b.reads;++i){const unsigned offset=(i<3?12u:24u)+4u*(i%3);
            if(!n.raw||n.known.size()<offset+4)return false;
            std::uint32_t bits=0;for(unsigned k=0;k<4;++k){if(!n.known[offset+k])return false;
                bits|=std::uint32_t(n.raw[offset+k])<<(8*k);}
            if(bits!=(i<3?b.localCenter:b.localHalf)[i%3])return false;
        }
        for(unsigned i=0;i<b.matrixWords;++i)
            if(shape.matrix00[i].known!=15||shape.matrix00[i].bits()!=b.composed[i])return false;
        if(m.center!=(b.centerStored?b.projectedCenter:vf0)||
           (b.centerStored&&b.projectedCenter[3]!=vf0[3])||m.half[3]!=vf0[3])return false;
        for(unsigned i=0;i<3;++i)if(m.half[i]!=((b.halfStores&(1u<<i))?b.projectedHalf[i]:0u))return false;
        //Only source receipts, identities and integer bits are consulted here.
        return b.returned?(b.centerStored&&b.halfStores==7&&b.pc==0x120e68u&&(m.primaryCalled||m.pc==0x2809c0u)):
            (m.pc==b.pc&&b.pc>=0x120378u&&b.pc<=0x120e48u);
    }
    static bool runSourceMembershipBounds1202E0(SourceCallback& h,std::uint32_t& pc)noexcept {
        auto& m=h.membership;auto& b=m.bounds;
        if(b.entered||m.pc!=0x1202e0u||!m.scratchInitialized||!h.secondSetter.returned||
           !h.loaderPath.partialType0_||h.outer->shape88_!=0)return false;
        auto& shape=*h.loaderPath.partialType0_;const auto& n=shape.payload.records.nodes;
        const auto at=[&](std::uint32_t p){pc=h.pc=m.pc=b.pc=p;};
        b.entered=true;b.shape=&shape;at(0x120378u);
        at(0x120388u);b.node=n.raw.get(); //Actual shape114, not a semantic mesh.
        for(unsigned i=0;i<6;++i){const unsigned axis=i%3,offset=(i<3?12u:24u)+4u*axis;
            at((i<3?0x12038cu:0x1203b0u)+8u*axis);
            if(!n.count||!n.raw||n.known.size()<offset+4)return false;
            std::uint32_t bits=0;for(unsigned k=0;k<4;++k){if(!n.known[offset+k])return false;
                bits|=std::uint32_t(n.raw[offset+k])<<(8*k);}
            (i<3?b.localCenter:b.localHalf)[axis]=bits;++b.reads;
        }
        //The source copies current composed00, never world80 or outer cached60.
        for(unsigned i=0;i<16;++i){at((i&1)?0x1203e4u:0x1203e0u);
            if(shape.matrix00[i].known!=15)return false;
            b.composed[i]=shape.matrix00[i].bits();++b.matrixWords;
        }
        //Bounded finite-nearest binary32 adaptation, not EE/VU bit parity.
        //Keep products/adds separate, and retain actual partial store order.
        float matrix[16],center[4],extent[4];std::memcpy(matrix,b.composed.data(),64);
        std::memcpy(center,b.localCenter.data(),16);std::memcpy(extent,b.localHalf.data(),16);
        const auto transform=[&](const float* v,unsigned axis,bool point,std::uint32_t base,float& out){
            at(base);if(std::fegetround()!=FE_TONEAREST)return false;
            float value=0;
            for(unsigned term=0;term<3;++term){at(base+4u*term);
                if(!std::isfinite(matrix[4u*term+axis])||!std::isfinite(v[term]))return false;
                volatile float product=matrix[4u*term+axis]*v[term];if(!std::isfinite(product))return false;
                if(!term)value=product;else{volatile float sum=value+product;if(!std::isfinite(sum))return false;value=sum;}
            }
            at(base+12u);if(point&&!std::isfinite(matrix[12u+axis]))return false;
            volatile float sum=value+(point?matrix[12u+axis]:0.f);if(!std::isfinite(sum))return false;
            out=sum;return true;
        };
        float projected[4]={0,0,0,1};
        for(unsigned axis=0;axis<3;++axis)if(!transform(center,axis,true,0x120cc4u,projected[axis]))return false;
        at(0x120cd4u);std::memcpy(b.projectedCenter.data(),projected,16);m.center=b.projectedCenter;b.centerStored=true;
        float axes[3][3];
        for(unsigned term=0;term<3;++term){float v[3]={0,0,0};v[term]=extent[term];
            for(unsigned axis=0;axis<3;++axis)if(!transform(v,axis,false,0x120cf8u+0x34u*term,axes[term][axis]))return false;
        }
        constexpr std::uint32_t sums[3]={0x120db4u,0x120dfcu,0x120e40u};
        constexpr std::uint32_t stores[3]={0x120dbcu,0x120e04u,0x120e48u};
        for(unsigned axis=0;axis<3;++axis){at(sums[axis]);
            volatile float xy=std::fabs(axes[0][axis])+std::fabs(axes[1][axis]);if(!std::isfinite(xy))return false;
            at(sums[axis]+4);volatile float sum=std::fabs(axes[2][axis])+xy;if(!std::isfinite(sum))return false;
            const float value=sum;std::uint32_t bits;std::memcpy(&bits,&value,4);
            at(stores[axis]);b.projectedHalf[axis]=m.half[axis]=bits;b.halfStores|=std::uint8_t(1u<<axis);
        }
        b.returned=true;at(0x120e68u);return true;
    }
