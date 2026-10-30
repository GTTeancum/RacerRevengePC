// Included inside OriginalRacerModelPackets50LA. This facet is source-order
// custody only: none of the native draw/constructed_ predicates is enabled.
    friend class OriginalSourcePhysicsModel50LA;
    friend class ::ra_static_model_cold50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    struct SourcePacketAllocation {
        struct Delete { void operator()(std::uint8_t* p)const noexcept {
            if(p)::operator delete(p,std::align_val_t(64));
        }};
        std::unique_ptr<std::uint8_t,Delete> bytes;
        std::uint32_t length=0,paletteSize=0;
        bool allocationAttempted=false,slotPublished=false,zeroed=false;
        bool commonStored=false,scaleXYZStored=false,reciprocalStored=false;
        bool commandsStored=false,headerStored=false;
        // Actual address04 is a typed borrow. The raw guest address bits are
        // unknown after its store; the zero bytes retained there are NOT bits.
        const std::uint8_t* headerAddress04=nullptr;
        bool guestAddress04Known=false;
    };
    struct SourcePacketState {
        OriginalRacerModelSceneOwner50LA* model=nullptr;
        const rr::assets::OriginalStandardModelGeometry50LA* geometry=nullptr;
        const void* descriptor20=nullptr;
        std::uint32_t rawA3=0,count=0,pc=0;
        std::array<std::uint32_t,1> sizes{};
        // Native pointer table projects the source count*4 allocation. It is
        // never serialized as a guest table and unknown slots are never read.
        std::unique_ptr<std::uint8_t*[]> table;
        std::array<SourcePacketAllocation,1> allocations;
        const OriginalWreckComponent50LA* returnedEntityV0=nullptr;
        const void* helperReturnIdentity=nullptr;
        bool entered=false,flagStored=false,paletteRead=false;
        bool outerAttempted=false,helperReturned=false,published1F0=false;
        bool returned=false,nativeWithdrawn=false;
    } sourcePackets_;

    R constructSourcePackets240740(OriginalRacerModelSceneOwner50LA& model,
        const rr::assets::OriginalStandardModelGeometry50LA& geometry,
        std::uint32_t rawA3)noexcept {
        auto& s=sourcePackets_;
        if(s.entered||constructed_||released_||!model.preflightSourceRenderableReturn26E820Native()||
           !model.sourceRenderableEntity26E820Native()||
           model.fields().geometry1B4!=&geometry||model.fields().packets1F0||model.sourcePacketBorrower_)
            return {S::phase,0x240768u};
        s.entered=true;s.model=&model;s.geometry=&geometry;s.rawA3=rawA3;model.sourcePacketBorrower_=this;
        auto stop=[&](S status,std::uint32_t pc)->R{s.pc=pc;return {status,pc};};
        auto& fields=model.fields();
        fields.flags1B3=static_cast<std::uint8_t>((fields.flags1B3&~1u)|(rawA3&1u));
        s.flagStored=true; //240780 precedes the actual geometry20 read.
        OriginalRacerModelSceneOwner50LA::SourcePacketPaletteBorrow240210 paletteBorrow;
        if(!model.borrowSourcePacketPalette240210Native(&geometry,paletteBorrow))
            return stop(S::geometry,0x240788u);
        s.descriptor20=paletteBorrow.descriptor20;s.paletteRead=true;
        const bool palette=s.descriptor20!=nullptr;
        if(palette){
            if(!paletteBorrow.count00)return stop(S::geometry,0x24081cu);
            s.count=*paletteBorrow.count00;
        } //No descriptor means no source count read; native count stays0.
        // Existing source palette loader has capacity one; this is a native
        // admission boundary, not a synthetic original branch or return0.
        if(palette&&s.count>s.allocations.size())return stop(S::palette,0x24081cu);
        if(!palette&&s.count)return stop(S::geometry,0x240788u);
        if(palette){
            s.outerAttempted=true;
            try{s.table.reset(new std::uint8_t*[s.count]);}catch(...){s.table.reset();}
        }
        const std::uint32_t count=palette?s.count:1u;
        for(std::uint32_t i=0;i<count;++i){
            auto& a=s.allocations[i];
            if(palette){
                if(!paletteBorrow.sizes04)return stop(S::palette,0x240850u);
                s.sizes[i]=paletteBorrow.sizes04[i];
            }
            const std::uint32_t bones=palette?s.sizes[i]:0u;
            // Source signed multiplication and command loop are admitted only
            // within existing row capacity, before the corresponding allocation.
            if(bones>rr::assets::maxModelNodes49O)return stop(S::palette,0x240850u);
            a.paletteSize=bones;a.length=0xb0u+0x50u*bones+(rawA3?0xa0u:0u);
            a.allocationAttempted=true;
            try{a.bytes.reset(static_cast<std::uint8_t*>(::operator new(a.length,std::align_val_t(64))));}
            catch(...){a.bytes.reset();}
            if(palette){
                // Inner allocation occurs even after outer allocation returned
                // NULL. Keep that pending allocation at the real SW boundary.
                if(!s.table)return stop(S::allocation,0x24087cu);
                s.table[i]=a.bytes.get();a.slotPublished=true;
            }
            if(!a.bytes)return stop(S::allocation,palette?0x240888u:0x240a58u);
            auto* p=a.bytes.get();std::memset(p,0,a.length);a.zeroed=true;
            auto word=[&](std::uint32_t at,std::uint32_t value){
                for(unsigned b=0;b<4;++b)p[at+b]=static_cast<std::uint8_t>(value>>(8u*b));
            };
            word(0x10,0x11000000u);word(0x14,0x03000000u);
            word(0x18,0x02000000u);word(0x1c,0x6c018029u);
            word(0x2c,bones);word(0x3c,0x6c01802au);a.commonStored=true;
            // Preserve all three raw float stores before native DIV.S admission.
            for(unsigned lane=0;lane<3;++lane){std::uint32_t bits=0;
                std::memcpy(&bits,&fields.scale1E0[lane],4);word(0x40u+4u*lane,bits);}
            a.scaleXYZStored=true;
            if(std::fegetround()!=FE_TONEAREST||!std::isfinite(fields.scale1E0[0])||
               fields.scale1E0[0]==0.f)return stop(S::numeric,palette?0x2408f0u:0x240ac8u);
            volatile float inverse=1.f/fields.scale1E0[0];
            if(!std::isfinite(inverse))return stop(S::numeric,palette?0x2408f0u:0x240ac8u);
            float result=inverse;std::uint32_t inverseBits=0;std::memcpy(&inverseBits,&result,4);
            word(0x4c,inverseBits);a.reciprocalStored=true;
            std::uint32_t at=0x50;
            if(rawA3){word(at+12,0x6c048014u);word(at+0x5c,0x6c048018u);at+=0xa0;}
            word(at+12,0x6c048000u);
            if(!palette&&fields.offsetEnabled1DC){
                //14D840 identity followed by raw XYZ transfers: no numerical
                // conversion or invented finite-input guard is needed here.
                word(at+0x10,0x3f800000u);word(at+0x24,0x3f800000u);
                word(at+0x38,0x3f800000u);word(at+0x4c,0x3f800000u);
                for(unsigned lane=0;lane<3;++lane){std::uint32_t bits=0;
                    std::memcpy(&bits,&fields.offset1D0[lane],4);word(at+0x40u+4u*lane,bits);}
            }
            at+=0x50;
            for(std::uint32_t bone=0;bone<bones;++bone){
                word(at+12,0x6c040000u|(0x8000u|(0xb6u+0x10u*bone)));at+=0x50;
            }
            word(at,0x14000010u);word(at+4,0x10000000u);
            word(at+8,palette?0x030002c5u:0x030000b5u);
            word(at+12,palette?0x02000088u:0x02000190u);a.commandsStored=true;
            const auto qwc=static_cast<std::uint16_t>((a.length-16u)>>4);
            p[0]=static_cast<std::uint8_t>(qwc);p[1]=static_cast<std::uint8_t>(qwc>>8);
            p[2]=0;p[3]=0x30;a.headerAddress04=p+16;a.guestAddress04Known=false;
            word(8,0);word(12,0);a.headerStored=true;
        }
        s.helperReturned=true;
        //1F0 is typed owner custody, NOT the numerical original allocation
        // address. The palette table itself may have a genuine NULL return
        // when count0; retain that separately from native facet publication.
        s.helperReturnIdentity=palette?static_cast<const void*>(s.table.get()):
            static_cast<const void*>(s.allocations[0].bytes.get());
        fields.packets1F0=s.helperReturnIdentity?this:nullptr;s.published1F0=true;
        s.returnedEntityV0=model.sourceRenderableEntity26E820Native();
        s.returned=true;return stop(S::complete,0x2407d8u);
    }

    bool preflightSourcePacketsNative()const noexcept {
        const auto& s=sourcePackets_;
        if(!s.entered)return true;
        if(s.nativeWithdrawn)return !s.table&&!s.allocations[0].bytes;
        if(!s.model||s.model->sourcePacketBorrower_!=this||!s.geometry||!s.flagStored||
           !s.model->preflightSourceRenderableReturn26E820Native()||
           s.model->fields().geometry1B4!=s.geometry||
           s.model->fields().packets1F0!=((s.published1F0&&s.helperReturnIdentity)?this:nullptr))return false;
        if(s.paletteRead){OriginalRacerModelSceneOwner50LA::SourcePacketPaletteBorrow240210 view;
            if(!s.model->borrowSourcePacketPalette240210Native(s.geometry,view)||
               view.descriptor20!=s.descriptor20)return false;
            if(s.descriptor20){if(!view.count00||*view.count00!=s.count)return false;
                if(s.allocations[0].allocationAttempted&&(!view.sizes04||view.sizes04[0]!=s.sizes[0]))return false;
            }else if(view.count00)return false;
        }
        const auto& a=s.allocations[0];
        if((s.model->fields().flags1B3&1u)!=(s.rawA3&1u))return false;
        if(a.allocationAttempted&&a.length!=0xb0u+0x50u*a.paletteSize+(s.rawA3?0xa0u:0u))return false;
        if(a.zeroed&&!a.allocationAttempted)return false;
        if(a.commonStored&&!a.zeroed)return false;
        if(a.scaleXYZStored&&!a.commonStored)return false;
        if(a.reciprocalStored&&!a.scaleXYZStored)return false;
        if(a.commandsStored&&!a.reciprocalStored)return false;
        if(s.helperReturned){
            if(!s.paletteRead||(s.descriptor20&&(!s.outerAttempted||s.count>1)))return false;
            if((!s.descriptor20||s.count)&&!a.headerStored)return false;
            const void* identity=s.descriptor20?static_cast<const void*>(s.table.get()):
                static_cast<const void*>(a.bytes.get());
            if(s.helperReturnIdentity!=identity)return false;
        }
        if(s.published1F0&&!s.helperReturned)return false;
        if(a.slotPublished&&(!s.table||s.table[0]!=a.bytes.get()))return false;
        if(a.zeroed&&!a.bytes)return false;
        if(a.headerStored&&(!a.commandsStored||!a.reciprocalStored||a.headerAddress04!=a.bytes.get()+16||
           a.guestAddress04Known))return false;
        if(s.returned&&(!s.helperReturned||!s.published1F0||
           s.returnedEntityV0!=s.model->sourceRenderableEntity26E820Native()))return false;
        return true;
    }
    bool withdrawSourcePacketsNative()noexcept {
        auto& s=sourcePackets_;
        if(!s.entered||s.nativeWithdrawn)return true;
        if((s.model&&(s.model->sourceObserverBorrower_||s.model->sourceSmokerBorrower_||s.model->sourceWarmPrototypeBorrowedNative()))||!preflightSourcePacketsNative())return false;
        // Native borrower withdrawal, not a claim that original240450 ran.
        // Pending inner allocation is owned even if outer slot was never stored.
        if(s.published1F0)s.model->fields().packets1F0=nullptr;
        for(auto& a:s.allocations)a.bytes.reset();
        s.table.reset();s.model->sourcePacketBorrower_=nullptr;s.nativeWithdrawn=true;return true;
    }
