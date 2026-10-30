    //Private actual295C90/295DD0 occurrence. No runtime/point-generation issuer.
    struct SourceTuning295C90 {
        bool entered=false,nullBody=false,returned=false,frictionEntered=false,frictionReturned=false;
        std::uint32_t pc=0,frictionPC=0,frictionInput=0;
        std::array<std::uint32_t,8> input{};
        std::array<std::uint32_t,4> mass{};
        std::uint8_t massMask=0;
        std::array<std::optional<bool>,4> nonpositive{};
        std::array<std::uint32_t,12> arithmetic{};
        std::uint16_t arithmeticMask=0,storeMask=0;
        std::array<std::uint32_t,9> written{};
        const void* bodyIdentity=nullptr;
    } tuning_;
    static constexpr std::array<unsigned,9> tuningOffsets_{{0x90,0x9c,0xa0,0xa4,0xa8,0xb8,0xbc,0xc0,0xd8}};
    bool sourceTuningWrittenNative(unsigned off)const noexcept {
        for(unsigned i=0;i<tuningOffsets_.size();++i)
            if(tuningOffsets_[i]==off)return bool(tuning_.storeMask&(1u<<i));
        return false;
    }
    float& tuningMirrorNative(unsigned i)noexcept {
        auto& p=storage_->physical;
        switch(i){case 0:return p.response.maxPenetration90;case 1:return p.response.stiffness9C;
        case 2:return p.response.dampingA0;case 3:return p.response.impactStiffnessA4;
        case 4:return p.response.impactDampingA8;case 5:return p.generated.impact.restitutionB8;
        case 6:return p.generated.impact.restitutionBC;case 7:return p.generated.impact.restitutionC0;
        default:return p.generated.frictionByMaterial[0];}
    }
    void tuningStoreNative(unsigned i,std::uint32_t raw,std::uint32_t pc)noexcept {
        tuning_.pc=pc;storage_->word(tuningOffsets_[i],raw);
        std::memcpy(&tuningMirrorNative(i),&raw,4);
        tuning_.written[i]=raw;tuning_.storeMask|=std::uint16_t(1u<<i);
    }
    Result configureSource295C90(const std::array<std::uint32_t,8>& rawF12Through19)noexcept {
        auto& h=tuning_;
        if(h.entered||nativeWithdrawn_||nativeRetired_||detachedNative_||sourcePoints_.nativeWithdrawn||!preflightNative()||
           !storage_||!storage_->raw140||!countedEntityNative())return {Status::owner,0x295c90u};
        //The source NULL test precedes any Body arithmetic, stores or Body proof.
        if(storage_->body30==std::optional<const void*>(nullptr)){
            h.entered=true;h.input=rawF12Through19;h.nullBody=true;h.returned=true;h.pc=0x295dc8u;
            return {Status::reached,h.pc};
        }
        if(!bodyOwner_||!bodyStored_||!bodyOwner_->body_||bodyOwner_->nativeWithdrawn_||
           bodyOwner_->nativeRetired_||bodyOwner_->detachedNative_||!bodyOwner_->preflightNative()||
           bodyOwner_->base_!=base_||bodyOwner_->contactBodyBorrower_!=this||
           storage_->body30!=std::optional<const void*>(&bodyOwner_->body_->entity)||
           !bodyOwner_->sourceInertiaReturned166618Native(this))return {Status::owner,0x295c90u};
        h.entered=true;h.input=rawF12Through19;h.bodyIdentity=&bodyOwner_->body_->entity;
        const auto stop=[&](std::uint32_t pc){h.pc=pc;return Result{Status::bounded,pc};};
        const auto supported=[](std::uint32_t u){const auto e=u&0x7f800000u;
            return !(u&0x7fffffffu)||(e&&e!=0x7f800000u);};
        const auto operation=[&](unsigned i,std::uint32_t pc,bool squareRoot,std::uint32_t a,std::uint32_t b){
            h.pc=pc;if(std::fegetround()!=FE_TONEAREST||!supported(a)||(!squareRoot&&!supported(b)))return false;
            float x,y;std::memcpy(&x,&a,4);std::memcpy(&y,&b,4);
            if(squareRoot&&x<0.f)return false;
            original_wreck_tuning_detail50LA::Math math;
            const float value=squareRoot?math.root(x):math.mul(x,y);
            std::uint32_t raw;std::memcpy(&raw,&value,4);
            if(!math.valid||!supported(raw))return false;
            h.arithmetic[i]=raw;h.arithmeticMask|=std::uint16_t(1u<<i);return true;
        };
        const auto readMass=[&](unsigned i,std::uint32_t pc){h.pc=pc;
            std::memcpy(&h.mass[i],&bodyOwner_->body_->physical.parameters.body.mass,4);
            h.massMask|=std::uint8_t(1u<<i);};
        const auto compare=[&](unsigned i,std::uint32_t raw,std::uint32_t pc){h.pc=pc;
            if(std::fegetround()!=FE_TONEAREST||!supported(raw))return false;
            float f;std::memcpy(&f,&raw,4);h.nonpositive[i]=f<=0.f;return true;};
        tuningStoreNative(0,h.input[0],0x295c9cu); //Raw store before arithmetic admission.
        for(unsigned stage=0;stage<2;++stage){
            const unsigned a=stage*6,m=stage*2,coefficient=stage?3:1,damping=stage?4:2;
            readMass(m,stage?0x295d40u:0x295cb4u);
            if(!operation(a,stage?0x295d44u:0x295cb8u,false,h.input[coefficient],h.mass[m])||
               !operation(a+1,stage?0x295d48u:0x295cbcu,false,0x4200cccdu,h.arithmetic[a]))return stop(h.pc);
            const auto stiffness=h.arithmetic[a+1]&0x7fffffffu; //Actual ABS.S, including signed zero.
            tuningStoreNative(stage?3:1,stiffness,stage?0x295d5cu:0x295cd0u);
            readMass(m+1,stage?0x295d64u:0x295cd8u);
            if(!compare(m,h.mass[m+1],stage?0x295d68u:0x295cdcu))return stop(h.pc);
            std::uint32_t value=0;
            if(!*h.nonpositive[m]){
                if(!compare(m+1,stiffness,stage?0x295d78u:0x295cecu))return stop(h.pc);
                if(!*h.nonpositive[m+1]){
                    if(!operation(a+2,stage?0x295d88u:0x295cfcu,false,h.mass[m+1],stiffness)||
                       !operation(a+3,stage?0x295d90u:0x295d04u,true,h.arithmetic[a+2],0)||
                       !operation(a+4,stage?0x295da4u:0x295d18u,false,0x40000000u,h.input[damping])||
                       !operation(a+5,stage?0x295dacu:0x295d20u,false,h.arithmetic[a+4],h.arithmetic[a+3]))return stop(h.pc);
                    value=h.arithmetic[a+5];
                }
            }
            tuningStoreNative(stage?4:2,value,stage?0x295db4u:0x295d28u);
        }
        tuningStoreNative(5,h.input[5],0x295db8u);tuningStoreNative(6,h.input[6],0x295dbcu);
        tuningStoreNative(7,h.input[7],0x295dc0u);h.returned=true;h.pc=0x295dc8u;
        return {Status::reached,h.pc};
    }
    Result frictionSource295DD0(std::uint32_t rawF12)noexcept {
        auto& h=tuning_;
        if(h.frictionEntered||!h.returned||nativeWithdrawn_||nativeRetired_||detachedNative_||sourcePoints_.nativeWithdrawn||
           !preflightNative()||!storage_||!storage_->raw140||
           (!h.nullBody&&(!bodyOwner_||!bodyOwner_->sourceInertiaReturned166618Native(this))))return {Status::owner,0x295dd0u};
        h.frictionEntered=true;h.frictionInput=rawF12;h.frictionPC=0x295decu;
        //This caller supplies signed index0; no ten-entry fill or mode mutation.
        const auto priorPC=h.pc;tuningStoreNative(8,rawF12,0x295decu);h.pc=priorPC;
        h.frictionReturned=true;h.frictionPC=0x295df4u;return {Status::reached,h.frictionPC};
    }
    bool sourceTuningHistoryNative()const noexcept {
        const auto& h=tuning_;
        if(!h.entered)return !h.nonpositive[0]&&!h.nonpositive[1]&&!h.nonpositive[2]&&!h.nonpositive[3]&&!h.nullBody&&!h.returned&&!h.frictionEntered&&!h.frictionReturned&&
            !h.pc&&!h.frictionPC&&!h.massMask&&!h.arithmeticMask&&!h.storeMask&&!h.bodyIdentity;
        if(!storage_||h.storeMask>511u||h.massMask>15u||h.arithmeticMask>4095u)return false;
        if(h.nullBody){if(h.nonpositive[0]||h.nonpositive[1]||h.nonpositive[2]||h.nonpositive[3]||h.bodyIdentity||h.massMask||h.arithmeticMask||(h.storeMask&255u)||
            !h.returned||h.pc!=0x295dc8u||storage_->body30!=std::optional<const void*>(nullptr))return false;}
        else {
            if(!bodyOwner_||!bodyOwner_->body_||h.bodyIdentity!=&bodyOwner_->body_->entity||
               !bodyOwner_->inertia_.returned||!bodyOwner_->inertiaHistoryNative()||!(h.storeMask&1u))return false;
            std::uint32_t actualMass;std::memcpy(&actualMass,&bodyOwner_->body_->physical.parameters.body.mass,4);
            for(unsigned i=0;i<4;++i)if(h.massMask&(1u<<i)){
                if(h.mass[i]!=actualMass||(i&&!(h.massMask&(1u<<(i-1)))))return false;
            }
            for(unsigned i=1;i<8;++i)if((h.storeMask&(1u<<i))&&!(h.storeMask&(1u<<(i-1))))return false;
            for(unsigned stage=0;stage<2;++stage){const unsigned a=stage*6,m=stage*2,s=stage?3:1;
                const auto mask=std::uint16_t(0x3fu<<a);
                for(unsigned i=1;i<6;++i)if((h.arithmeticMask&(1u<<(a+i)))&&!(h.arithmeticMask&(1u<<(a+i-1))))return false;
                if((h.arithmeticMask&mask)&&!(h.massMask&(1u<<m)))return false;
                if(stage&&(h.massMask&(1u<<m))&&!(h.storeMask&(1u<<2)))return false;
                if(h.storeMask&(1u<<s)){
                    if(!(h.arithmeticMask&(1u<<(a+1)))||h.written[s]!=(h.arithmetic[a+1]&0x7fffffffu))return false;
                }
                if(h.nonpositive[m]&&(!(h.massMask&(1u<<(m+1)))||!(h.storeMask&(1u<<s))))return false;
                if(h.nonpositive[m+1]&&(!h.nonpositive[m]||*h.nonpositive[m]))return false;
                const auto isNonpositive=[](std::uint32_t bits){return !(bits&0x7fffffffu)||bool(bits&0x80000000u);};
                if(h.nonpositive[m]&&*h.nonpositive[m]!=isNonpositive(h.mass[m+1]))return false;
                if(h.nonpositive[m+1]&&*h.nonpositive[m+1]!=isNonpositive(h.written[s]))return false;
                if((h.arithmeticMask&(1u<<(a+2)))&&(!h.nonpositive[m]||*h.nonpositive[m]||
                    !h.nonpositive[m+1]||*h.nonpositive[m+1]))return false;
                if(h.storeMask&(1u<<(s+1))){
                    if(!h.nonpositive[m])return false;
                    const bool zero=*h.nonpositive[m]||(h.nonpositive[m+1]&&*h.nonpositive[m+1]);
                    if(!zero&&(!h.nonpositive[m+1]||!(h.arithmeticMask&(1u<<(a+5)))))return false;
                    if(h.written[s+1]!=(zero?0u:h.arithmetic[a+5]))return false;
                }
            }
            if(h.written[0]!=h.input[0])return false;
            for(unsigned i=5;i<8;++i)if((h.storeMask&(1u<<i))&&h.written[i]!=h.input[i])return false;
            if(h.returned&&((h.storeMask&255u)!=255u||h.pc!=0x295dc8u))return false;
        }
        if(h.frictionEntered!=h.frictionReturned||h.frictionReturned!=bool(h.storeMask&256u)||
           (h.frictionEntered&&(!h.returned||h.frictionPC!=0x295df4u||h.written[8]!=h.frictionInput))||
           (!h.frictionEntered&&h.frictionPC))return false;
        for(unsigned i=0;i<9;++i)if(h.storeMask&(1u<<i)){
            if(storage_->known[tuningOffsets_[i]/4]!=0xffffffffu||storage_->words[tuningOffsets_[i]/4]!=h.written[i])return false;
            const auto& p=storage_->physical;const float* mirror=nullptr;
            switch(i){case 0:mirror=&p.response.maxPenetration90;break;case 1:mirror=&p.response.stiffness9C;break;
            case 2:mirror=&p.response.dampingA0;break;case 3:mirror=&p.response.impactStiffnessA4;break;
            case 4:mirror=&p.response.impactDampingA8;break;case 5:mirror=&p.generated.impact.restitutionB8;break;
            case 6:mirror=&p.generated.impact.restitutionBC;break;case 7:mirror=&p.generated.impact.restitutionC0;break;
            default:mirror=&p.generated.frictionByMaterial[0];break;}
            std::uint32_t raw;std::memcpy(&raw,mirror,4);if(raw!=h.written[i])return false;
        }
        return true;
    }

    bool sourceConfigurationTailAdmissionNative(const Base* actualBase)const noexcept {
        if(!actualBase||base_!=actualBase||nativeWithdrawn_||nativeRetired_||detachedNative_||
           sourcePoints_.nativeWithdrawn||!storage_||!storage_->raw140||!counted_||
           !tuning_.returned||!tuning_.frictionReturned||tuning_.pc!=0x295dc8u||
           tuning_.frictionPC!=0x295df4u||!actualBase->object_||actualBase->contactBorrower_!=this||
           actualBase->object_->contactDC!=std::optional<const void*>(&storage_->entity)||
           !preflightNative())return false;
        return tuning_.nullBody?storage_->body30==std::optional<const void*>(nullptr):
            (bodyOwner_&&bodyOwner_->sourceInertiaReturned166618Native(this));
    }
