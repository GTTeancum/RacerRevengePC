//Five caller-ordered settings on the explicitly native-adapted Wake branch.
//The reused value receipts here are NATIVE results, never original returns.
bool OriginalWaterSourceConfig50LA::readWakeSettingNative(std::size_t ordinal,
    std::optional<std::uint32_t>& destination,D& d)noexcept {
    d={};auto& g=nativeWakeGetters_;
    if(!g.entered||g.active||ordinal>=5u||g.completed!=ordinal||!nativeWakeLoad_.nativeLoaded||
       pc_!=0x30492cu||nativeClosed_||!borrowed_||thread_!=std::this_thread::get_id()||
       !preflightNative(waterReceiver_))return false;
    if(ordinal==1u?g.alpha.attempted:g.floats[ordinal?ordinal-1u:0u].attempted)return false;
    struct Scope {
        bool& active;explicit Scope(bool& value)noexcept:active(value){active=true;}
        ~Scope(){active=false;}
    } scope(g.active); //Every return, including refusal, restores the source sink.
    if(ordinal==1u){
        auto& f=g.alpha;f.attempted=true;g.pc=0x158b48;
        if(!findWaterField158240(f,"AlphaReference"))return false;
        if(*f.found){
            if(!convertWaterInteger158920(f))return false;
            g.pc=*f.storePC;destination=f.converted;f.result=f.converted;f.innerReturned=true;
            g.pc=0x158a60;g.pc=0x158b5c;f.returned=true;
        }else{
            g.pc=0x158a60;f.innerReturned=false;
            g.pc=0x158b60;destination=50u;f.result=50u;f.returned=false;
        }
        g.pc=0x158b78;
    }else{
        const auto index=ordinal?ordinal-1u:0u;auto& f=g.floats[index];
        static constexpr std::array<std::string_view,4> names{{"MinForce","TexCoordScale","FrothScale","FrothStartSize"}};
        static constexpr std::array<std::uint32_t,4> defaults{{0x3c23d70au,0x3dcccccdu,0x399d4952u,0x40000000u}};
        f.attempted=true;g.pc=0x158b9c;
        if(!findWaterField158240(f,names[index]))return false;
        if(*f.found){
            g.pc=0x158bac;if(!convertWaterDecimal302E10(f))return false;
            g.pc=0x158bc4;destination=f.converted;f.result=f.converted;f.returned=true;
        }else{
            g.pc=0x158bdc;destination=defaults[index];f.result=defaults[index];f.returned=false;
        }
        g.pc=0x158bf4;
    }
    static constexpr std::array<std::uint32_t,5> next{{0x2b97e0u,0x2b97fcu,0x2b9820u,0x2b9844u,0x2b9864u}};
    ++g.completed;g.pc=next[ordinal];return true;
}
bool OriginalWaterSourceConfig50LA::preflightWakeGettersNative()const noexcept {
    const auto& g=nativeWakeGetters_;
    if(g.active||g.completed>5u)return false; //No public/invariant boundary inside the private scope.
    static constexpr std::array<std::uint32_t,4> defaults{{0x3c23d70au,0x3dcccccdu,0x399d4952u,0x40000000u}};
    for(std::size_t i=0;i<4u;++i)
        if(!preflightFloatValueNative(g.floats[i],nativeWakeLoad_.counted,defaults[i]))return false;
    if(!preflightIntegerValueNative(g.alpha,nativeWakeLoad_.counted,50u))return false;
    if(!g.entered&&g.pc!=0x2b97bcu)return false;
    if(g.entered&&(!nativeWakeLoad_.nativeLoaded||pc_!=0x30492cu||wakeSelection_.returned))return false;
    for(std::size_t ordinal=0;ordinal<5u;++ordinal){
        const bool attempted=ordinal==1u?g.alpha.attempted:g.floats[ordinal?ordinal-1u:0u].attempted;
        const auto returned=ordinal==1u?g.alpha.returned:g.floats[ordinal?ordinal-1u:0u].returned;
        if(!g.entered){if(attempted||returned||g.completed)return false;continue;}
        if(ordinal<g.completed){if(!attempted||!returned)return false;}
        else if(ordinal==g.completed){
            if(returned)return false;
            if(!attempted){
                static constexpr std::array<std::uint32_t,5> starts{{0x2b97bcu,0x2b97e0u,0x2b97fcu,0x2b9820u,0x2b9844u}};
                static constexpr std::array<std::uint32_t,5> delays{{0x2b97dcu,0x2b97f8u,0x2b981cu,0x2b9840u,0x2b9860u}};
                if(g.pc!=starts[ordinal]&&g.pc!=delays[ordinal])return false;
            }
        }else if(attempted||returned)return false;
    }
    return g.completed!=5u||g.pc==0x2b9864u;
}
