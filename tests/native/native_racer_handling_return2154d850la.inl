// Native same-owner handling continuation. The counted source34C/350/354
// allocation route remains distinct. These are the live legacy frame/contact
// owners reached from the actual retained model's Bank visit.
bool Owner::continueNativeAnimationContact2154D8(g::OriginalRacerEntityCold50LA& actualRacer,
        const std::vector<std::uint8_t>& retainedPhy,original_camera_session50KX::Session& session,
        ra_source_camera50LA::OwnershipBridge& bridge,std::uint32_t& pc)noexcept {
    if(!fixture_||!sourceHandlingBodyCurrent(*fixture_)||handlingRoute_!=HandlingRoute::legacy||
       !nativeAeroEngineComplete_||!nativeHandlingStack_||nativeAnimationContactAttempted_||
       nativeAnimationContactComplete_||!racer_||racer_!=&actualRacer||!attachedFor(actualRacer)||
       !racer_->prepared()||racer_->nativeWreckWithdrawn_||pc!=0x2154d8u||racer_->nativePostEmberPC_!=pc||
       !rig_||&retainedPhy!=&rig_->phy||!bridge.nativeHandlingInputOwnerCurrent(session,*this))return false;
    if(!nativePhyLoad_||racer_->nativePhyLoad_!=nativePhyLoad_)return false;
    auto phy=nativePhyLoad_->borrow(nativePhyContext_,retainedPhy);if(!phy)return false;
    auto& reader=phy.cursor();
    nativeAnimationContactAttempted_=true;auto& s=*nativeHandlingStack_;
    auto& v=fixture_->parameters.visual;auto& l=fixture_->parameters.lifecycle;
    const auto at=[&](std::uint32_t address){pc=failurePC_=racer_->nativePostEmberPC_=address;};
    const auto read=[&](std::uint32_t address,const char* key,std::optional<float>& destination){
        at(address);return reader.float158A70(key,destination);};
    const auto direct=[&](std::uint32_t address,const char* key,float& destination){
        std::optional<float> value;if(!read(address,key,value))return false;
        if(value)destination=*value;return true;};
    const auto select=[&](std::uint32_t address,const char* name){
        at(address);bool present=false;return reader.select157D00(name,present);};
    g::original_wreck_tuning_detail50LA::Math math;
    const auto mul=[&](std::uint32_t address,float x,float y,float& result){
        at(address);if(std::fegetround()!=FE_TONEAREST)return false;result=math.mul(x,y);return math.valid;};
    try {
        if(!select(0x2154e0u,"Pod Animation"))return false;
        struct D{const char* key;std::uint32_t pc;float* field;};
        const D first[]={
            {"ImpactForce",0x2154f4u,&v.impact9F0},{"GroundImpactForce",0x215508u,&v.groundImpact9F4},
            {"SeparationForce",0x21551cu,&v.separation9F8},{"ThrustForce",0x215530u,&v.thrust9FC},
            {"LiftForce",0x215544u,&v.liftA00},{"DragForce",0x215558u,&v.dragA04},
            {"EngineSpring",0x21556cu,&v.springA08},{"EngineDamping",0x215580u,&v.dampingA0C},
            {"BinderSpring",0x215594u,&v.binderSpringA10},{"YawTorque",0x2155a8u,&v.yawA14},
            {"PowerslideYawTorque",0x2155bcu,&v.slideYawA18},{"PowerslidePitchTorque",0x2155d0u,&v.slidePitchA1C},
            {"PitchTorque",0x2155e4u,&v.pitchA20},{"RollTorque",0x2155f8u,&v.rollA24},
            {"EngineTorque",0x21560cu,&v.angleReturnA28},{"EngineTorqueDamping",0x215620u,&v.angularDampingA2C},
            {"BinderTorque",0x215634u,&v.binderTorqueA30},{"MaxInputScaleSpeed",0x215648u,&v.effectiveInputSpeedA68},
            {"CableForce",0x21565cu,&l.cableForceA6C},{"CableRadiusForce",0x215670u,&l.radiusForceA70},
            {"ChariotImpactForce",0x215684u,&l.collisionXYAA0},{"ChariotHeadOnImpactForce",0x215698u,&l.collisionZAA4},
            {"ChariotRepulsorImpactForce",0x2156acu,&l.groundGainAA8},{"ChariotLandingImpulseScale",0x2156c0u,&l.groundTransitionGainAAC},
            {"ChariotMaxImpulse",0x2156d4u,&l.collisionBoundA98},{"ChariotMaxRepulsorImpulse",0x2156e8u,&l.groundBoundA9C},
            {"ChariotVelocityRateForce",0x2156fcu,&l.returnGainA7C},{"ChariotVelocityForce",0x215710u,&l.restoringA74},
            {"ChariotPowerslideVelocityForce",0x215724u,&l.slideRestoringA78},{"ChariotVelocityDamping",0x215738u,&l.linearDampingA80},
            {"ChariotAngularRateDamping",0x21574cu,&l.angularDampingA84},{"ChariotMaxYawRate",0x215760u,&l.yawInputRangeA88},
            {"ChariotMaxYawAngle",0x215774u,&l.yawAngleA8C}};
        for(const auto& d:first)if(!direct(d.pc,d.key,*d.field))return false;
        if(!read(0x215788u,"ChariotMaxPowerslideYawAngle",racer_->powerslideYawA90_))return false;
        const D last[]={
            {"MinTurbulenceScale",0x21579cu,&v.minAmplitudeA44},{"MaxTurbulenceScale",0x2157b0u,&v.maxAmplitudeA48},
            {"MinTurbulenceTimeScale",0x2157c4u,&v.minTimeA34},{"MaxTurbulenceTimeScale",0x2157d8u,&v.maxTimeA38}};
        for(const auto& d:last)if(!direct(d.pc,d.key,*d.field))return false;
        std::optional<float> speed;
        if(!read(0x2157ecu,"MaxTurbulenceSpeed",speed))return false;
        // One sourceA54 store, two existing consumer projections. Missing
        // retains the actual current visual lane and its coherent projection.
        if(speed)v.effectiveSpeedSquaredA54=l.effectiveSpeedSquaredA54=*speed;
        const D end[]={
            {"OrientationScale",0x215800u,&v.orientationScaleA64},{"DamageTurbulenceScale",0x215814u,&v.damageTranslationA58},
            {"DamageOrientationTurbulenceScale",0x215828u,&v.damageOrientationA5C},
            {"MinChariotTurbulenceScale",0x21583cu,&l.minAmplitudeA4C},{"MaxChariotTurbulenceScale",0x215850u,&l.maxAmplitudeA50},
            {"MinChariotTurbulenceTimeScale",0x215864u,&l.minTimeA3C},{"MaxChariotTurbulenceTimeScale",0x215878u,&l.maxTimeA40},
            {"ChariotOrientationScale",0x21588cu,&l.orientationScaleA60}};
        for(const auto& d:end)if(!direct(d.pc,d.key,*d.field))return false;
        const float degrees=coldFloat(0x3c8efa34u),units=coldFloat(0x3fbbbb84u);float f0;
        if(!mul(0x2158acu,l.yawInputRangeA88,degrees,f0))return false;at(0x2158b4u);l.yawInputRangeA88=f0;
        if(!mul(0x2158c8u,l.yawAngleA8C,degrees,f0))return false;at(0x2158ccu);l.yawAngleA8C=f0;
        at(0x2158d0u);if(!racer_->powerslideYawA90_)return false;
        if(!mul(0x2158d4u,*racer_->powerslideYawA90_,degrees,f0))return false;at(0x2158d8u);racer_->powerslideYawA90_=f0;
        if(!mul(0x2158e0u,v.effectiveInputSpeedA68,units,f0))return false;at(0x2158e4u);v.effectiveInputSpeedA68=f0;
        const float authoredSpeed=v.effectiveSpeedSquaredA54;
        if(!mul(0x2158ecu,units,authoredSpeed,f0)||!mul(0x2158f0u,f0,authoredSpeed,f0)||
           !mul(0x2158f4u,units,f0,f0))return false;
        at(0x2158fcu);v.effectiveSpeedSquaredA54=l.effectiveSpeedSquaredA54=f0; //JAL delay store precedes select.
        if(!select(0x2158f8u,"Collisions"))return false;
        constexpr const char* keys[]={"MaxCompression","Spring","Damping","ImpulseSpring","ImpulseDamping","Friction","WallRestitution","PodRestitution"};
        for(unsigned i=0;i<8;++i)if(!read(0x21590cu+20u*i,keys[i],s.contact45C[i]))return false;
        g::SpringGeneratedParameters48K* generated=nullptr;g::SpringResponseParameters48L* response=nullptr;
        at(0x2159a0u);if(!bridge.readNativeHandlingContact215900(session,*this,generated,response)||!generated||!response)return false;
        if(!direct(0x2159b0u,"MomentScale",generated->impact.forceTorqueScaleB4)||
           !direct(0x2159c8u,"AngularImpulseScale",generated->impact.angularImpulseScaleB0)||
           !direct(0x2159e0u,"FrictionTorqueScale",generated->frictionTorqueScaleC4)||
           !read(0x2159f4u,"ExplosiveWreckSpeed",racer_->explosiveWreckSpeed50_)||
           !read(0x215a08u,"ResetSpeed",racer_->resetSpeed54_))return false;
        at(0x215a10u);if(!racer_->explosiveWreckSpeed50_)return false;
        if(!mul(0x215a24u,*racer_->explosiveWreckSpeed50_,units,f0))return false;
        at(0x215a28u);racer_->explosiveWreckSpeed50_=f0;
        at(0x215a2cu);if(!racer_->resetSpeed54_)return false;
        if(!mul(0x215a30u,*racer_->resetSpeed54_,units,f0))return false;
        at(0x215a34u);racer_->resetSpeed54_=f0;
        // Actual stack-load order. Missing Friction is not consumed until the
        // later295DD0 call, so all295C90 stores can survive that failure.
        constexpr unsigned slots[]={6,0,1,2,3,4,7};
        constexpr std::uint32_t loads[]={0x215a38u,0x215a40u,0x215a44u,0x215a48u,0x215a4cu,0x215a50u,0x215a54u};
        for(unsigned i=0;i<7;++i){at(loads[i]);if(!s.contact45C[slots[i]])return false;}
        at(0x295c9cu);response->maxPenetration90=*s.contact45C[0];
        const float mass=fixture_->parameters.bodies.main.body.mass,gravity=coldFloat(0x4200cccdu);
        if(!mul(0x295cb8u,*s.contact45C[1],mass,f0)||!mul(0x295cbcu,gravity,f0,f0))return false;
        at(0x295cc0u);f0=std::fabs(f0);at(0x295cd0u);response->stiffness9C=f0;
        if(mass>0.f&&response->stiffness9C>0.f){
            if(!mul(0x295cfcu,mass,response->stiffness9C,f0))return false;
            at(0x295d04u);const float root=math.root(f0);if(!math.valid)return false;
            if(!mul(0x295d18u,2.f,*s.contact45C[2],f0)||!mul(0x295d20u,f0,root,f0))return false;
        }else {at(0x295d24u);f0=0.f;}
        at(0x295d28u);response->dampingA0=f0;
        if(!mul(0x295d44u,*s.contact45C[3],mass,f0)||!mul(0x295d48u,gravity,f0,f0))return false;
        at(0x295d4cu);f0=std::fabs(f0);at(0x295d5cu);response->impactStiffnessA4=f0;
        if(mass>0.f&&response->impactStiffnessA4>0.f){
            if(!mul(0x295d88u,mass,response->impactStiffnessA4,f0))return false;
            at(0x295d90u);const float root=math.root(f0);if(!math.valid)return false;
            if(!mul(0x295da4u,2.f,*s.contact45C[4],f0)||!mul(0x295dacu,f0,root,f0))return false;
        }else {at(0x295db0u);f0=0.f;}
        at(0x295db4u);response->impactDampingA8=f0;
        at(0x295db8u);generated->impact.restitutionB8=*s.contact45C[6];
        at(0x295dbcu);generated->impact.restitutionBC=*s.contact45C[6];
        at(0x295dc0u);generated->impact.restitutionC0=*s.contact45C[7];
        at(0x215a64u);if(!s.contact45C[5])return false;
        at(0x295decu);generated->frictionByMaterial[0]=*s.contact45C[5]; //Material0 only.
        at(0x215a70u);if(!bridge.publishNativeHandlingContact215A70(session,*this))return false;
        if(!select(0x215a78u,"Joystick"))return false;
        auto& control=fixture_->parameters.control;auto& rates=fixture_->parameters.rates;
        const D joystick[]={
            {"SteerSpeed1",0x215a8cu,&control.speed1B84},{"SteerScale1",0x215aa0u,&control.scale1B88},
            {"SteerSpeed2",0x215ab4u,&control.speed2B8C},{"SteerScale2",0x215ac8u,&control.scale2B90},
            {"XAxisRateLimit",0x215adcu,&rates.x},{"YAxisRateLimit",0x215af0u,&rates.y}};
        for(const auto& d:joystick)if(!direct(d.pc,d.key,*d.field))return false;
        nativeAnimationContactComplete_=true;at(0x215af8u);return true;
    }catch(...){return false;} //Source prefix survives; this invocation cannot replay.
}

// The intervening same-owner WreckBacking call must finish each collection's
//22C310 and22C610 before this method may begin. No reconstructed Wreck roster,
//second whole-roster pass, new Player dispatch or invented nonnull audio call.
bool Owner::continueNativeHandling215BB8(g::OriginalRacerEntityCold50LA& actualRacer,
        const std::vector<std::uint8_t>& retainedPhy,original_camera_session50KX::Session& session,
        const g::OriginalRaceConstructor50LA& race,ra_source_camera50LA::OwnershipBridge& bridge,
        std::uint32_t& pc)noexcept {
    if(!fixture_||!sourceHandlingBodyCurrent(*fixture_)||handlingRoute_!=HandlingRoute::legacy||
       !nativeAeroEngineComplete_||!nativeAnimationContactComplete_||!nativeWreckTuningComplete_||
       !nativeHandlingStack_||nativeHandlingTailAttempted_||nativeHandlingReturned_||
       !racer_||racer_!=&actualRacer||!attachedFor(actualRacer)||!racer_->prepared()||racer_->nativeWreckWithdrawn_||
       pc!=0x215bb8u||racer_->nativePostEmberPC_!=pc||!rig_||&retainedPhy!=&rig_->phy||
       !bridge.nativeHandlingInputOwnerCurrent(session,*this))return false;
    if(!nativePhyLoad_||racer_->nativePhyLoad_!=nativePhyLoad_)return false;
    auto phy=nativePhyLoad_->borrow(nativePhyContext_,retainedPhy);if(!phy)return false;
    auto& reader=phy.cursor();
    nativeHandlingTailAttempted_=true;auto& s=*nativeHandlingStack_;
    const auto at=[&](std::uint32_t address){pc=failurePC_=racer_->nativePostEmberPC_=address;};
    const auto read=[&](std::uint32_t address,const char* key,std::optional<float>& destination){
        at(address);return reader.float158A70(key,destination);};
    const auto direct=[&](std::uint32_t address,const char* key,float& destination){
        std::optional<float> value;if(!read(address,key,value))return false;
        if(value)destination=*value;return true;};
    const auto select=[&](std::uint32_t address,const char* name){
        at(address);bool present=false;return reader.select157D00(name,present);};
    g::original_wreck_tuning_detail50LA::Math math;
    const auto op=[&](std::uint32_t address,char operation,float x,float y,float& result){
        at(address);if(std::fegetround()!=FE_TONEAREST)return false;
        result=operation=='+'?math.add(x,y):operation=='-'?math.sub(x,y):
            operation=='*'?math.mul(x,y):math.div(x,y);return math.valid;};
    const auto clamp=[](float value,float minimum,float maximum){
        if(maximum<=minimum){if(value<maximum)return maximum;if(!(value<=minimum))return minimum;}
        else {if(!(value<=maximum))return maximum;if(value<minimum)return minimum;}
        return value;
    };
    try {
        if(!select(0x215bc0u,"Binder"))return false;
        float value;
        at(0x215bdcu);if(!reader.defaultFloat158B80("BinderMeshHalfHeight",coldFloat(0x40100000u),value))return false;
        racer_->binderMeshHalfHeight25C_=value;
        at(0x215bf8u);if(!reader.defaultFloat158B80("BinderMeshHalfHeight",coldFloat(0x40100000u),value))return false;
        racer_->binderMeshHalfHeight25C_=value;
        at(0x215c08u);racer_->color260_=0x00ffffffu;
        at(0x215c1cu);if(!reader.string158240("VertexColor",s.binderColor2B0))return false;
        if(s.binderColor2B0){
            // Deliberately bounded byte-only decimal subset of29FF50/29FFA0
            //and302E30. Source packing itself has NO per-channel masking.
            auto text=*s.binderColor2B0;
            const auto trim=[](std::string_view t){
                while(!t.empty()&&(t.front()==' '||t.front()=='\t'||t.front()=='\r'))t.remove_prefix(1);
                while(!t.empty()&&(t.back()==' '||t.back()=='\t'||t.back()=='\r'))t.remove_suffix(1);
                return t;
            };
            constexpr std::uint32_t conversions[]={0x215c48u,0x215c64u,0x215c80u};
            for(unsigned i=0;i<3;++i){
                at(conversions[i]);const auto comma=text.find(',');
                if((i<2&&comma==std::string_view::npos)||(i==2&&comma!=std::string_view::npos))return false;
                auto token=trim(i<2?text.substr(0,comma):text);if(token.empty())return false;
                unsigned channel=0;
                for(char c:token){if(c<'0'||c>'9')return false;channel=channel*10u+unsigned(c-'0');if(channel>255u)return false;}
                s.binderChannels[i]=static_cast<std::int32_t>(channel);
                if(i<2)text.remove_prefix(comma+1u);
            }
            at(0x215c98u);racer_->color260_=(std::uint32_t(*s.binderChannels[0])<<16u)|
                (std::uint32_t(*s.binderChannels[1])<<8u)|std::uint32_t(*s.binderChannels[2]);
        }
        // Actual25C/color260 are retained for later213A40, which is beyond
        //the caller fence. The independent already-loaded Binder is not a receipt.
        if(!select(0x215ca4u,"CameraInfo"))return false;
        at(0x215cc0u);if(!racer_->cameraOffsetCE0_)return false;
        std::array<float,4> origin;std::memcpy(origin.data(),arena_->visualShared.zero434630.v,16);
        if(!reader.vector158C00("FirstPersonCamOffset",origin,*racer_->cameraOffsetCE0_))return false;
        // Keep actual Entity CE0 for the genuine later1750F8+ camera owner.
        // The independent standalone camera parser is not this native path.
        at(0x215cc8u);
        if(!select(0x215cd0u,"DamageRepair"))return false;
        at(0x215cd8u);if(!s.useValues)return false;
        g::BoostParameters50FK* boost=nullptr;float* cooling=nullptr;
        if(!bridge.readNativeHandlingCooling215CC8(session,*this,boost,cooling)||!boost||!cooling)return false;
        float& defense=fixture_->state.fields.defenseCF0;float& repair=fixture_->state.fields.repairCF4;
        if(*s.useValues){
            at(0x215ce4u);if(!s.selectedClass||*s.selectedClass<1u||*s.selectedClass>3u)return false;
            constexpr const char* defenseKeys[]={"DefenseA","DefenseB","DefenseC"};
            constexpr const char* repairKeys[]={"BaseRepairAmountA","BaseRepairAmountB","BaseRepairAmountC"};
            constexpr const char* coolingKeys[]={"BaseCoolingTimeA","BaseCoolingTimeB","BaseCoolingTimeC"};
            constexpr std::uint32_t first[]={0x215d0cu,0x215d50u,0x215d94u};
            const unsigned i=*s.selectedClass-1u;
            if(!direct(first[i],defenseKeys[i],defense)||!direct(first[i]+20u,repairKeys[i],repair)||
               !direct(first[i]+40u,coolingKeys[i],*cooling))return false;
        }else {
            auto& d=s.damage480;
            for(unsigned i=0;i<12;++i){at(i==11u?0x215e0cu:0x215ddcu+i*4u);d[i]=0.f;}
            constexpr const char* keys[]={"MinDefense","MaxDefense","MinBaseRepairAmount","MaxBaseRepairAmount","MinBaseCoolingTime","MaxBaseCoolingTime"};
            constexpr unsigned index[]={0,1,4,5,8,9};
            for(unsigned i=0;i<6;++i)if(!read(0x215e08u+20u*i,keys[i],d[index[i]]))return false;
            if(!select(0x215e7cu,"BaseAbilities")||!read(0x215e90u,"Defense",d[2])||
               !read(0x215ea4u,"BaseRepairAmount",d[6])||!read(0x215eb8u,"BaseCoolingTime",d[10])||
               !select(0x215ec8u,"MaxAbilities")||!read(0x215edcu,"Defense",d[3])||
               !read(0x215ef0u,"BaseRepairAmount",d[7])||!read(0x215f04u,"BaseCoolingTime",d[11]))return false;
            float* destinations[]={&defense,&repair,cooling};
            for(unsigned i=0;i<3;++i){
                const unsigned n=i*4u;const std::uint32_t start=0x215f20u+i*0x30u;float f3,f2,f0;
                at(start-12u);if(!s.upgrade3F0)return false;
                if(!op(start,'-',*d[n+3],*d[n+2],f3)||!op(start+4u,'*',*s.upgrade3F0,f3,f2)||
                   !op(start+8u,'+',*d[n+2],f2,f2)||!op(start+12u,'-',*d[n+1],*d[n],f0)||
                   !op(start+16u,'*',f2,f0,f0)||!op(start+20u,'+',*d[n],f0,f0))return false;
                at(start+24u);*destinations[i]=f0;
            }
        }
        at(0x215fb0u);boost->baseCooling80=*cooling;
        if(!op(0x215fb8u,'*',coldFloat(0x37a7c5acu),*cooling,value))return false;
        at(0x215fbcu);boost->speedCooling84=value;
        at(0x215fc0u);
        // This exact Entity constructor wroteD40=NULL at20F62C. Its unchanged
        //base dispatch+A0=212EC0 performed no allocation. A native human58 or
        //event5C borrow does not establish Player+A0=1A1DF0 or permit skipping
        //a nonnull audio owner. Such a changed owner still stops here.
        if(!racer_->nativeDispatch20F390Known_||!racer_->sourceAudioD40_||*racer_->sourceAudioD40_)return false;
        bool profileRequired=false;std::array<float,6> abilities;
        at(0x215fe4u);if(!bridge.readNativeHandlingProfile216040(session,*this,race,profileRequired,abilities))return false;
        auto& engine=fixture_->parameters.bodies.main.engine;auto& aero=fixture_->parameters.bodies.main.aero;
        if(profileRequired){
            auto& p=s.profile4C;
            for(unsigned i=0;i<4;++i){at(i==3u?0x216084u:0x216074u+i*4u);p[i]=0.f;}
            if(!select(0x216080u,"Engine")||!read(0x216094u,"MinMaxThrust",p[0])||
               !read(0x2160a8u,"MaxMaxThrust",p[1])||!read(0x2160bcu,"MinTheoreticalTopSpeed",p[2])||
               !read(0x2160d0u,"MaxTheoreticalTopSpeed",p[3]))return false;
            float f0,f1;
            if(!op(0x216108u,'-',*p[1],*p[0],f0)||!op(0x216120u,'-',abilities[0],0.f,f1)||
               !op(0x216128u,'*',f1,f0,f0)||!op(0x216134u,'/',f0,1.f,f0)||
               !op(0x216144u,'+',*p[0],f0,f1))return false;
            at(0x2161bcu);engine.maxThrust=clamp(f1,*p[0],*p[1]);
            if(!op(0x2161e8u,'-',*p[3],*p[2],f1)||!op(0x216204u,'*',abilities[1],f1,f1)||
               !op(0x216208u,'+',*p[2],f1,f1)||!op(0x21620cu,'*',coldFloat(0x3fbbbb84u),f1,f0))return false;
            at(0x216210u);engine.theoreticalTopSpeed=f0; //UNCLAMPED profile speed.
            at(0x216214u);p[4]=0.f;at(0x216218u);p[5]=0.f;at(0x216220u);p[6]=0.f;
            if(!select(0x21621cu,"Engine Aerodynamics")||!read(0x216230u,"MinSpoilerYawMoment",p[4])||
               !read(0x216244u,"MaxSpoilerYawMoment",p[5])||!read(0x216258u,"SpoilerYawMomentOffset",p[6]))return false;
            float minimum,maximum;
            if(!op(0x21627cu,'+',*p[4],*p[6],minimum)||!op(0x216284u,'+',*p[5],*p[6],maximum))return false;
            at(0x216288u);p[4]=minimum;at(0x216290u);p[5]=maximum; //Offset regardless of58.
            if(!op(0x2162a0u,'-',maximum,minimum,f1)||!op(0x2162c4u,'-',abilities[2],0.f,f0)||
               !op(0x2162c8u,'*',f0,f1,f1)||!op(0x2162ccu,'/',f1,1.f,f0)||
               !op(0x2162dcu,'+',minimum,f0,f0))return false;
            at(0x21634cu);aero.steering100=clamp(f0,minimum,maximum);
            // Do not regenerate engine torque80/90 after this profile override.
            for(unsigned i=0;i<6;++i){at(i==5u?0x216368u:0x216350u+i*4u);p[i+7u]=0.f;}
            if(!select(0x216364u,"DamageRepair"))return false;
            constexpr const char* damageKeys[]={"MinDefense","MaxDefense","MinBaseRepairAmount","MaxBaseRepairAmount","MinBaseCoolingTime","MaxBaseCoolingTime"};
            for(unsigned i=0;i<6;++i)if(!read(0x216378u+20u*i,damageKeys[i],p[i+7u]))return false;
            at(0x216428u);defense=abilities[3];at(0x216458u);repair=abilities[4];at(0x216480u);*cooling=abilities[5];
            // Raw profile fields are all published BEFORE any mapped value.
            //A later arithmetic rejection must preserve those earlier writes.
            float* destinations[]={&defense,&repair,cooling};
            constexpr std::uint32_t first[]={0x216490u,0x216530u,0x2165d0u};
            constexpr std::uint32_t add[]={0x2164b0u,0x216550u,0x2165f0u};
            constexpr std::uint32_t store[]={0x216510u,0x2165b0u,0x216650u};
            for(unsigned i=0;i<3;++i){
                const float lo=*p[7u+2u*i],hi=*p[8u+2u*i];float f2;
                if(!op(first[i],'-',*destinations[i],0.f,f2)||!op(first[i]+4u,'-',hi,lo,f1)||
                   !op(first[i]+8u,'*',f2,f1,f1)||!op(first[i]+12u,'/',f1,1.f,f0)||
                   !op(add[i],'+',lo,f0,f0))return false;
                at(store[i]);*destinations[i]=clamp(f0,lo,hi);
            }
            at(0x216654u);boost->baseCooling80=*cooling;
            if(!op(0x21666cu,'*',coldFloat(0x37a7c5acu),*cooling,f0))return false;
            at(0x216670u);boost->speedCooling84=f0;
            at(0x216684u);at(0x216688u); //Actual profile return and delay.
        }
        at(0x216000u);if(!s.engine414[7])return false;
        if(!op(0x216008u,'-',engine.theoreticalTopSpeed,*s.engine414[7],value))return false;
        at(0x21600cu);if(!bridge.publishNativeHandlingDifference21600C(session,*this,value))return false;
        at(0x216030u);at(0x216034u);nativeHandlingReturned_=true;return true;
    }catch(...){return false;} //No rollback, retry, source Player/load/Restart readiness.
}
