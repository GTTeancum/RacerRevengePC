// Included in native_racer_model50LA after Owner's native Repulsor method.
// A native legacy continuation only. It does not construct/adopt source34C or
//350, return2146, establish Player dispatch, or grant a later gameplay gate.
bool Owner::continueNativeAeroEngine214954(g::OriginalRacerEntityCold50LA& actualRacer,
        const std::vector<std::uint8_t>& retainedPhy,original_camera_session50KX::Session& session,
        const g::OriginalRaceConstructor50LA& race,ra_source_camera50LA::OwnershipBridge& bridge,std::uint32_t& pc)noexcept {
    if(!fixture_||!sourceHandlingBodyCurrent(*fixture_)||handlingRoute_!=HandlingRoute::legacy||
       !nativeRepulsorComplete_||nativeAeroEngineAttempted_||nativeAeroEngineComplete_||nativeHandlingStack_||
       !racer_||racer_!=&actualRacer||!attachedFor(actualRacer)||!racer_->prepared()||racer_->nativeWreckWithdrawn_||
       pc!=0x214954u||racer_->nativePostEmberPC_!=pc||racer_->racer.sessionGeneration!=generation_||
       racer_->body340!=body340_.get()||!rig_||&retainedPhy!=&rig_->phy||retainedPhy.empty()||
       !nativeAero_||!nativeAero_->published_||nativeAero_->revoked_||nativeAero_->sourceFresh_||
       nativeAero_->bank_!=bank_||nativeAero_->frame_!=arena_||nativeAero_->parameters_!=&fixture_->parameters.bodies.main||
       nativeAero_->model_!=this||nativeAero_->body_!=body340_.get()||nativeAero_->application_!=application_.get()||
       nativeAero_->slot_!=slot_||nativeAero_->generation_!=generation_||nativeAero_->racer_!=racer_||
       nativeAero_->racerKey_!=racer_->racer.objectKey)return false;
    if(!nativePhyLoad_||racer_->nativePhyLoad_!=nativePhyLoad_)return false;
    auto phy=nativePhyLoad_->borrow(nativePhyContext_,retainedPhy);if(!phy)return false;
    auto& reader=phy.cursor();
    nativeAeroEngineAttempted_=true;nativeHandlingStack_.emplace();auto& s=*nativeHandlingStack_;
    //Keep the actual selection reached by21489C, including a missing-select
    //preservation. No late "Repulsor" seed or independent Config cursor.
    auto& aero=fixture_->parameters.bodies.main.aero;auto& engine=fixture_->parameters.bodies.main.engine;
    const auto at=[&](std::uint32_t address){pc=failurePC_=racer_->nativePostEmberPC_=address;};
    const auto read=[&](std::uint32_t address,const char* key,std::optional<float>& value){at(address);return reader.float158A70(key,value);};
    const auto direct=[&](std::uint32_t address,const char* key,float& destination){
        std::optional<float> value;if(!read(address,key,value))return false;
        if(value)destination=*value;return true;
    };
    const auto select=[&](std::uint32_t address,const char* section){at(address);bool present=false;return reader.select157D00(section,present);};
    const auto track=[&](std::uint32_t address,std::array<char,3>& text){at(address);
        return bridge.readNativeHandlingTrack187330(session,*this,race,text)&&text[2]=='\0';};
    // Host arithmetic rejection is before the rejected instruction's store,
    // never rollback. Each operation narrows separately; no fused expression.
    g::original_wreck_tuning_detail50LA::Math math;
    const auto arithmetic=[&](std::uint32_t address,char operation,float x,float y,float& value){
        at(address);if(std::fegetround()!=FE_TONEAREST)return false;
        value=operation=='+'?math.add(x,y):operation=='-'?math.sub(x,y):math.mul(x,y);
        return math.valid;
    };
    try {
        std::array<char,3> name;
        if(!track(0x21495cu,name))return false;
        bool foundA=false;for(char c:name){if(!c)break;if(c=='a'){foundA=true;break;}}
        if(foundA)s.selectedClass=1u;
        else {if(!track(0x2149bcu,name))return false;
            bool foundB=false;for(char c:name){if(!c)break;if(c=='b'){foundB=true;break;}}
            s.selectedClass=foundB?2u:3u;}
        if(!track(0x214a20u,name))return false;
        if(name[0]=='s'&&name[1]=='b')s.selectedClass=3u;
        else {if(!track(0x214a50u,name))return false;
            if(name[0]=='r'&&name[1]=='a')s.selectedClass=2u;
            else {if(!track(0x214a80u,name))return false;
                if(name[0]=='s'&&name[1]=='c')s.selectedClass=2u;}}
        at(0x214aacu);s.useValues=true;at(0x214ab8u);s.upgrade3F0=0.f;
        at(0x214ab4u);bool tuning=false;if(!reader.select157D00("Tuning",tuning))return false;
        if(tuning){
            at(0x214ad8u);s.tuning3F4=0;at(0x214ad4u);bool written=false;
            if(!reader.integer158920("UseValues",s.tuning3F4,written))return false;
            if(written&&*s.tuning3F4==0)s.useValues=false;
            if(*s.useValues){
                constexpr const char* keys[]={"UseAValues","UseBValues","UseCValues"};
                constexpr std::uint32_t pcs[]={0x214b08u,0x214b38u,0x214b68u};
                for(unsigned i=0;i<3;++i){at(pcs[i]);
                    if(!reader.integer158920(keys[i],s.tuning3F4,written))return false;
                    if(written&&*s.tuning3F4==1)s.selectedClass=i+1u;}
            }else if(!read(0x214b9cu,"UpgradePercent",s.upgrade3F0))return false;
        }
        if(!select(0x214bacu,"Engine Aerodynamics"))return false;
        at(0x214bb8u);for(unsigned i=0;i<28;++i){at(i==27u?0x1004a0u:0x100430u+i*4u);s.aero100[i]=0.f;}
        constexpr const char* bulkKeys[]={"Chord","WingSpan","WingArea","DefaultAngleOfAttack","Lift",
            "PitchRateLift","ElevatorLift","BasicDrag","LiftDrag","ElevatorDrag","PitchMoment",
            "PitchRatePitchMoment","ElevatorPitchMoment","SideSlipSideForce",nullptr,"RudderSideForce",
            "RollRateSideForce","YawRateSideForce","SideSlipRollMoment","AileronRollMoment","RudderRollMoment",
            "RollRateRollMoment","YawRateRollMoment","SideSlipYawMoment","AileronYawMoment","RudderYawMoment",
            "RollRateYawMoment","YawRateYawMoment"};
        unsigned lookup=0;
        for(unsigned i=0;i<28;++i)if(bulkKeys[i]){
            if(!read(0x214bccu+20u*lookup,bulkKeys[i],s.aero100[i]))return false;++lookup;}
        at(0x214de0u);unsigned absent=0;
        for(unsigned i=0;i<28;++i){at(0x1004bcu+i*8u);const float value=*s.aero100[i];
            switch(i){
            case 3:aero.offset68=value;break;case 4:aero.lift6c=value;break;case 7:aero.drag78=value;break;
            case 10:aero.pitch84=value;break;case 11:aero.pitchDamping88=value;break;case 13:aero.lateral90=value;break;
            case 21:aero.rollDampingB0=value;break;case 23:aero.timerYawB8=value;break;case 27:aero.yawDampingC8=value;break;
            default:nativeAero_->nativeBaseCoefficients_[absent++]=value;break;
            }
        }
        struct Direct{const char* key;std::uint32_t pc;float* destination;};
        const Direct directAero[]={
            {"SpoilerDrag",0x214df8u,&aero.driveDragF8},{"SpoilerBrakeDrag",0x214e10u,&aero.brakeDragFc},
            {"SpoilerSideForce",0x214e28u,&aero.timerSteering108},{"SpoilerYawMomentScale",0x214e40u,&aero.steeringRamp104},
            {"PowerslideSpoilerYawMoment",0x214e58u,&aero.timerYaw110},{"PowerslideSideForce",0x214e70u,&aero.timerLateral10c},
            {"PowerslideThrust",0x214e88u,&aero.timerDrag114},{"InvertedSideSlipSideForce",0x214ea0u,&aero.tiltedLateral118},
            {"InvertedSpoilerSideForce",0x214eb8u,&aero.tiltedSteering11c},{"InvertedSideSlipYawMoment",0x214ed0u,&aero.tiltedYaw120}};
        for(const auto& item:directAero)if(!direct(item.pc,item.key,*item.destination))return false;
        if(!read(0x214ee4u,"RampDownElevatorPitch",s.rampDownElevator3F8)||
           !read(0x214ef8u,"MaxElevatorPitch",s.maxElevator3FC))return false;
        if(*s.useValues){
            constexpr const char* keys[]={"SpoilerYawMomentA","SpoilerYawMomentB","SpoilerYawMomentC"};
            constexpr std::uint32_t pcs[]={0x214f38u,0x214f58u,0x214f78u};
            if(!direct(pcs[*s.selectedClass-1u],keys[*s.selectedClass-1u],aero.steering100))return false;
        }else {
            auto& q=s.steering400;for(unsigned i=0;i<5;++i){at(i==4u?0x214facu:0x214f98u+i*4u);q[i]=0.f;}
            if(!read(0x214fa8u,"MinSpoilerYawMoment",q[0])||!read(0x214fbcu,"MaxSpoilerYawMoment",q[1]))return false;
            // Actual native current-state58, shared with runtime consumers. It
            // does not stand in for an unjoined derived Player source write.
            at(0x214fc4u);if(!fixture_->state.fields.vehicle58&&
                !read(0x214fdcu,"SpoilerYawMomentOffset",q[4]))return false;
            float minimum,maximum;
            if(!arithmetic(0x214ffcu,'+',*q[0],*q[4],minimum)||
               !arithmetic(0x215000u,'+',*q[1],*q[4],maximum))return false;
            at(0x215004u);q[0]=minimum;at(0x21500cu);q[1]=maximum;
            if(!select(0x215008u,"BaseAbilities")||!read(0x21501cu,"SpoilerYawMoment",q[2])||
               !select(0x21502cu,"MaxAbilities")||!read(0x215040u,"SpoilerYawMoment",q[3]))return false;
            float f3,f2,f0;
            if(!arithmetic(0x215060u,'-',*q[3],*q[2],f3)||
               !arithmetic(0x215064u,'*',*s.upgrade3F0,f3,f2)||
               !arithmetic(0x215068u,'+',*q[2],f2,f2)||
               !arithmetic(0x21506cu,'-',*q[1],*q[0],f0)||
               !arithmetic(0x215070u,'*',f2,f0,f0)||
               !arithmetic(0x215074u,'+',*q[0],f0,f0))return false;
            at(0x215078u);aero.steering100=f0;
        }
        auto& e=s.engine414;for(unsigned i=0;i<10;++i){at(0x215080u+i*4u);e[i]=0.f;}
        at(0x2150a8u);s.engineVectors170[0]=std::array<float,4>{{0,0,0,1}};
        at(0x2150b0u);s.engineVectors170[1]=std::array<float,4>{{0,0,0,1}};
        if(!select(0x2150bcu,"Engine"))return false;
        if(*s.useValues){
            constexpr const char* thrust[]={"MaxThrustA","MaxThrustB","MaxThrustC"};
            constexpr const char* speed[]={"TheoreticalTopSpeedA","TheoreticalTopSpeedB","TheoreticalTopSpeedC"};
            constexpr std::uint32_t first[]={0x2150f8u,0x215128u,0x215158u};
            const auto i=*s.selectedClass-1u;
            if(!read(first[i],thrust[i],e[0])||!read(first[i]+20u,speed[i],e[6]))return false;
        }else {
            auto& q=s.abilities43C;for(unsigned i=0;i<8;++i){at(i==7u?0x2151acu:0x21518cu+i*4u);q[i]=0.f;}
            if(!read(0x2151a8u,"MinMaxThrust",q[0])||!read(0x2151bcu,"MaxMaxThrust",q[1])||
               !read(0x2151d0u,"MinTheoreticalTopSpeed",q[2])||!read(0x2151e4u,"MaxTheoreticalTopSpeed",q[3])||
               !select(0x2151f4u,"BaseAbilities")||!read(0x215208u,"MaxThrust",q[4])||
               !read(0x21521cu,"TheoreticalTopSpeed",q[6])||!select(0x21522cu,"MaxAbilities")||
               !read(0x215240u,"MaxThrust",q[5])||!read(0x215254u,"TheoreticalTopSpeed",q[7]))return false;
            float f6,f2,f4,f0;
            if(!arithmetic(0x215278u,'-',*q[5],*q[4],f6)||
               !arithmetic(0x215288u,'*',*s.upgrade3F0,f6,f6)||
               !arithmetic(0x215294u,'-',*q[7],*q[6],f2)||
               !arithmetic(0x215298u,'+',*q[4],f6,f6)||
               !arithmetic(0x21529cu,'-',*q[1],*q[0],f4)||
               !arithmetic(0x2152a0u,'*',f6,f4,f4)||
               !arithmetic(0x2152a4u,'*',*s.upgrade3F0,f2,f2)||
               !arithmetic(0x2152a8u,'+',*q[0],f4,f4)||
               !arithmetic(0x2152acu,'+',*q[6],f2,f6))return false;
            at(0x2152b0u);e[0]=f4;
            if(!arithmetic(0x2152b4u,'-',*q[3],*q[2],f0)||
               !arithmetic(0x2152b8u,'*',f6,f0,f0)||
               !arithmetic(0x2152bcu,'+',*q[2],f0,f0))return false;
            at(0x2152c4u);e[6]=f0;if(!select(0x2152c0u,"Engine"))return false;
        }
        if(!read(0x2152d4u,"BoostThrust",e[1])||!read(0x2152e8u,"RepairThrust",e[2])||
           !read(0x2152fcu,"RampDownSpeed",e[5])||!read(0x215310u,"ActualTopSpeed",e[7])||
           !read(0x215324u,"RepairSpeed",e[9]))return false;
        at(0x215350u);if(!reader.vector158C00("LeftEnginePosition",*s.engineVectors170[0]))return false;
        at(0x21537cu);if(!reader.vector158C00("RightEnginePosition",*s.engineVectors170[1]))return false;
        if(!read(0x215390u,"RampUpFilter",e[3])||!read(0x2153a4u,"RampDownFilter",e[4])||
           !read(0x2153b8u,"BoostSpeed",e[8]))return false;
        const float units=coldFloat(0x3fbbbb84u);float f4,f3,f2,f1,f0;
        if(!arithmetic(0x2153e0u,'*',*e[5],units,f4)||!arithmetic(0x2153e4u,'*',*e[6],units,f3))return false;
        at(0x2153e8u);e[5]=f4;
        if(!arithmetic(0x2153ecu,'*',*e[7],units,f2))return false;
        at(0x2153f0u);e[6]=f3;
        if(!arithmetic(0x2153f4u,'*',*e[8],units,f1))return false;
        at(0x2153f8u);e[7]=f2; //Retain actual converted stack430 for21600C, not engine5C.
        if(!arithmetic(0x2153fcu,'*',*e[9],units,f0))return false;
        at(0x215400u);e[8]=f1;at(0x215404u);e[9]=f0;
        at(0x215408u);
        if(!fixture_->state.fields.vehicle58){
            at(0x215434u);std::uint32_t configuration;
            if(!bridge.readNativeHandlingConfiguration215434(session,*this,configuration))return false;
            if(configuration){
                if(!arithmetic(0x215454u,'*',aero.steering100,0.5f,f0))return false;
                at(0x215458u);aero.steering100=f0;
                //neg.s toggles sign, including positive/negative zero.
                std::uint32_t bits;std::memcpy(&bits,&f0,4);const float negative=coldFloat(bits^0x80000000u);
                at(0x21547cu);s.engineVectors170[0]=std::array<float,4>{{0,negative,0,1}};
                at(0x21549cu);s.engineVectors170[1]=std::array<float,4>{{0,aero.steering100,0,1}};
            }
        }
        at(0x2154d0u); //Actual legacy destination uses the already registered48D lanes.
        at(0x2e769cu);engine.maxThrust=*e[0];at(0x2e76a0u);engine.boostThrust=*e[1];
        at(0x2e76a4u);engine.repairThrust=*e[2];at(0x2e76a8u);engine.rampUpFilter=*e[3];
        at(0x2e76acu);engine.rampDownFilter=*e[4];at(0x2e76b0u);engine.rampDownSpeed=*e[5];
        at(0x2e76b4u);engine.theoreticalTopSpeed=*e[6];at(0x2e76b8u);engine.boostSpeed=*e[8];
        at(0x2e76c0u);engine.repairSpeed=*e[9]; //No store to5C in2E7680.
        for(unsigned i=0;i<2;++i){at(i?0x2e76d8u:0x2e76c8u);const auto& vector=*s.engineVectors170[i];
            std::memcpy(engine.torqueBasis[i].v,vector.data(),3*sizeof(float));
            std::uint32_t w;std::memcpy(&w,&vector[3],4);nativeEngineBasisW_[i]=w;
        }
        nativeAeroEngineComplete_=true;at(0x2154d8u);return true;
    }catch(...){return false;} //Retain prefix, no rollback/retry or global teardown latch.
}
