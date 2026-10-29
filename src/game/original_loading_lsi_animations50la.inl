// Complete-class private fragment of OriginalLoadingScreenLayout50LA.
// Requires config_ (the same actual mounted LSI Config), <array>, <optional>,
// <memory>, <cstring>, <string>, <new>. No public parser or activation grant.
struct AnimationRow50LA final {
    // 2E8990 receives a1=NULL and a2=2CCD40 (destructor). No constructor
    // initializes these rows. Every unproduced scalar and pointer stays unknown.
    std::unique_ptr<char[]> nameBacking;
    const char* sourceName00=nullptr;
    bool name00Known=false,nameCopyReached=false;
    std::optional<std::int16_t> first04,last06;
    std::uint8_t flags20=0,known20=0,flags21=0,known21=0;
    std::optional<std::uint16_t> completed30;
    // EE rate bits may use exponent255 as a finite extended-range value;
    // never reinterpret these as a host IEEE float for collector arithmetic.
    std::optional<std::uint32_t> rate34Bits;
    bool rate34Stored=false; // Actual SWC1 at2CDED8, not merely an evaluated result.
    std::optional<float> current4C;
    std::optional<std::int16_t> firstX08,firstY0A,lastX0C,lastY0E;
    std::optional<float> currentX50,currentY54;
    void bit20(unsigned bit,bool value)noexcept {
        const auto mask=std::uint8_t(1u<<bit);
        flags20=std::uint8_t((flags20&~mask)|(value?mask:0));known20|=mask;
    }
    void bit21(unsigned bit,bool value)noexcept {
        const auto mask=std::uint8_t(1u<<bit);
        flags21=std::uint8_t((flags21&~mask)|(value?mask:0));known21|=mask;
    }
};
std::int32_t count00_=0,count04_=0;
std::uint8_t refresh08_=1;
std::unique_ptr<AnimationRow50LA[]> animationBacking_;
AnimationRow50LA* sourceAnimations0C_=nullptr; // Actual2CCC60 ctorNULL.
bool animationsArrayPublished_=false,animationsAttempted_=false,animationsNativeClosed_=false;
std::optional<std::int32_t> animationsReturned_;
std::size_t animationIndex_=0;
std::optional<std::int32_t> animationStartStack830_,animationEndStack834_;
std::optional<float> animationTimeStack840_,animationProductF1_,animationFramesF2_,animationDeltaF0_;
std::array<std::optional<float>,4> animationPositionTemps858_{};
std::optional<std::uint32_t> animationMoveProductF1Bits_,animationMoveDivisorF0Bits_;
std::optional<std::uint32_t> animationRateQuotientF0Bits_;
// Historical delta receipt for this one instruction, NOT a claim that later
// getters/conversions preserve live FCR31. Untouched prior bits remain unknown.
std::uint32_t animationDivideFcr31WriteMask_=0,animationDivideFcr31WriteValue_=0;
// Sony EE scalar DIV.S +0/+0 only. No host division, NaN, infinity or
// IEEE finite-max substitution; exponent255 belongs to the EE bit pattern.
// Records only proven I/D/SI; prior sticky state remains unasserted,
// with no claim about other cause-state bits.
bool divideAnimationZero2CDEC8(std::uint32_t& pc)noexcept {
    pc=0x2CDEC8;
    if(animationIndex_!=0||!sourceAnimations0C_||!animationBacking_||
       sourceAnimations0C_!=animationBacking_.get()||!animationTimeStack840_||
       !animationProductF1_||!animationFramesF2_||!animationDeltaF0_||
       sourceAnimations0C_[0].rate34Stored||animationRateQuotientF0Bits_)return false;
    static_assert(sizeof(float)==sizeof(std::uint32_t));
    const auto positiveZero=[](float value)noexcept {
        std::uint32_t bits;std::memcpy(&bits,&value,sizeof(bits));return bits==0;
    };
    if(!positiveZero(*animationTimeStack840_)||!positiveZero(*animationProductF1_)||
       !positiveZero(*animationFramesF2_)||!positiveZero(*animationDeltaF0_))return false;
    animationRateQuotientF0Bits_=0x7FFFFFFFu;
    constexpr std::uint32_t mask=0x00030040u,value=0x00020040u; // I=1,D=0,SI=1.
    animationDivideFcr31WriteValue_=(animationDivideFcr31WriteValue_&~mask)|value;
    animationDivideFcr31WriteMask_|=mask;
    return true;
}
// Actual first-row continuation requires the reached SWC1 receipt, not merely
// an evaluated quotient. No collector/event/GS readiness is issued here.
bool continueAnimation1AfterRateStore2CDED8(std::uint32_t& pc)noexcept try {
    using Diagnostic=rr::assets::OriginalUiPageDiagnostic50KG;
    pc=0x2CDED4;
    if(!config_||!config_->config_||!animationsAttempted_||animationsNativeClosed_||
       animationIndex_!=0||!animationBacking_||sourceAnimations0C_!=animationBacking_.get()||
       !sourceAnimations0C_[0].rate34Bits||!sourceAnimations0C_[0].rate34Stored||!config_->selectedCurrent("Animation1"))return false;
    const auto* values=config_->config_->records_.section("Animation1");
    if(!values)return false;
    auto& row=sourceAnimations0C_[0];Diagnostic d;
    const auto integer=[&](const char* key,std::int32_t& out,std::uint32_t call) {
        pc=call;if(!rr::assets::originalUiInteger50KG(*values,key,0,out,d,pc)){
            pc=d.sourcePC;return false;}return true;
    };
    std::int32_t start=0,end=0;
    if(!integer("StartPosX",start,0x2CDED4))return false;
    animationStartStack830_=start;
    if(!integer("EndPosX",end,0x2CDEEC))return false;
    animationEndStack834_=end;
    // Native exact-cohort bound before source CVT.S/MIN.S/CVT.W. These small
    // integral values have exact binary32 representations and exact low16.
    pc=0x2CDF00;if(start!=-133||end!=810)return false;
    animationPositionTemps858_[0]=float(start<end?start:end); //MIN.S/store858.
    row.firstX08=std::int16_t(*animationPositionTemps858_[0]); //2CDF34.
    animationPositionTemps858_[1]=float(start>end?start:end); //MAX.S/store85C.
    row.lastX0C=std::int16_t(*animationPositionTemps858_[1]); //2CDF6C.
    row.bit21(1,end<start); //2CDFA8/C8.
    row.currentX50=float(*row.firstX08); //2CE004 call-delay store BEFORE getter.
    if(!integer("StartPosY",start,0x2CE000))return false;
    animationStartStack830_=start;
    if(!integer("EndPosY",end,0x2CE018))return false;
    animationEndStack834_=end;
    pc=0x2CE02C;if(start!=103||end!=103)return false;
    animationPositionTemps858_[2]=float(start<end?start:end);
    row.firstY0A=std::int16_t(*animationPositionTemps858_[2]); //2CE060.
    animationPositionTemps858_[3]=float(start>end?start:end);
    row.lastY0E=std::int16_t(*animationPositionTemps858_[3]); //2CE098.
    row.bit21(2,end<start); //2CE0D4/F4.
    row.currentY54=float(*row.firstY0A); //2CE12C call-delay store BEFORE getter.
    float duration=0;pc=0x2CE128;
    if(!rr::assets::originalUiNumber50KG(*values,"MoveTime",0,duration,d,pc)){pc=d.sourcePC;return false;}
    animationTimeStack840_=duration;
    bool loop=false,reverse=false;pc=0x2CE140;
    if(!rr::assets::originalUiBoolean50KG(*values,"LoopMovement",false,loop,d,pc)){pc=d.sourcePC;return false;}
    row.bit20(1,loop); //2CE188 next call-delay store BEFORE reverse getter.
    pc=0x2CE184;
    if(!rr::assets::originalUiBoolean50KG(*values,"ReversableMovement",false,reverse,d,pc)){pc=d.sourcePC;return false;}
    row.bit20(5,reverse); //2CE1B8.
    pc=0x2CE1C0;if(duration!=20.0f)return false; // Authenticated first row; no broader scalar policy.
    animationMoveProductF1Bits_=0x469C4000u; //2CE1DC: exact1000*20 =20000.
    animationMoveDivisorF0Bits_=0x41855557u; //2CE1EC MTC1, before DIV.S.
    pc=0x2CE1F4;return false; // Actual nonzero DIV.S next; no host rounding assumption.
}catch(...){return false;} // Current call PC preserves all earlier source stores.
bool preflightAnimationsNative()const noexcept {
    // No API exports rows, names or source-array borrows. Validate only the
    // live owned backing; a completed native close leaves historical pointers.
    if(animationsNativeClosed_)return !animationBacking_;
    return !animationsArrayPublished_||sourceAnimations0C_==animationBacking_.get();
}
bool closeAnimationsNative()noexcept {
    if(!preflightAnimationsNative())return false;
    if(animationsNativeClosed_)return true;
    animationBacking_.reset();animationsNativeClosed_=true;
    // Native custody withdrawal, NOT2CCD40 on unknown row00. Keep source0C
    // and all reached source fields as history; no invented sourceNULL store.
    return true;
}
bool loadAnimations2CDAD0(bool sourceLayoutPresent,std::uint32_t& pc)noexcept try {
    using Diagnostic=rr::assets::OriginalUiPageDiagnostic50KG;
    using Config=rr::platform::OriginalLoadingLsiConfig50LA;
    pc=0x2CDAD0;
    if(!config_||animationsAttempted_||animationsNativeClosed_)return false;
    // All host automatic-wrapper admission precedes the source constructor.
    if(!config_->admitAutomaticStorageNative())return false;
    animationsAttempted_=true;
    Diagnostic d;
    const auto fail=[&](std::uint32_t at)noexcept {pc=at;return false;};
    const auto returnedZero=[&](std::uint32_t closePC)noexcept {
        pc=closePC;
        if(!config_->closeWidgetsStack1573B0(-1,d))return fail(d.sourcePC);
        animationsReturned_=0;pc=0x2CE9A4;return true;
    };
    pc=0x2CDAE8;if(!config_->constructAutomatic157320())return false;
    pc=0x2CDB0C;const auto opened=config_->open1577D0("ld_icon.ini",d);
    if(opened==Config::OpenResult::absent)return returnedZero(0x2CDB24);
    if(opened!=Config::OpenResult::opened)return fail(d.sourcePC?d.sourcePC:pc);
    pc=0x2CDB3C;
    if(!config_->selectWidgetsSection157D00("General",d)){
        if(d.sourcePC==0x157DD8)return returnedZero(0x2CDB54);
        return fail(d.sourcePC);
    }
    const auto* general=config_->config_->records_.section("General");
    if(!general||!config_->selectedCurrent("General"))return false;
    pc=0x2CDB74;
    if(!sourceLayoutPresent)return false; // Before getter with genuineNULL output; prior Config effects retained.
    if(!rr::assets::originalUiInteger50KG(*general,"NumberOfFrames",0,count00_,d,pc))return fail(d.sourcePC);
    pc=0x2CDB8C;
    if(!rr::assets::originalUiInteger50KG(*general,"NumberOfAnimations",0,count04_,d,pc))return fail(d.sourcePC);
    // Authenticated LD_ICON.LSI is exactly4 frames/6 rows. This native bound
    // does not rewrite reached source counts or grant other archive authority.
    if(count00_!=4||count04_!=6)return fail(0x2CDB94);
    bool refresh=true;pc=0x2CDBA4;
    if(!rr::assets::originalUiBoolean50KG(*general,"RefreshDisplay",true,refresh,d,pc))return fail(d.sourcePC);
    refresh08_=std::uint8_t(refresh); //2CDBB0 low-byte store of actual T=1.
    pc=0x2CDBBC;
    animationBacking_.reset(new(std::nothrow) AnimationRow50LA[std::size_t(count04_)]);
    // Actual2E8990(NULL) returnsNULL; a1=0 means NO per-row constructor.
    sourceAnimations0C_=animationBacking_.get();animationsArrayPublished_=true; //2CDBE0.
    for(animationIndex_=0;animationIndex_<std::size_t(count04_);++animationIndex_){
        pc=0x2CDBF8;const std::string section="Animation"+std::to_string(animationIndex_+1);
        pc=0x2CDC04;
        if(!config_->selectWidgetsSection157D00(section,d)){
            if(d.sourcePC==0x157DD8)return returnedZero(0x2CDC1C);
            return fail(d.sourcePC);
        }
        const auto* values=config_->config_->records_.section(section);
        if(!values||!config_->selectedCurrent(section))return false;
        if(!sourceAnimations0C_)return fail(0x2CDC44); // First actualNULL row store.
        auto& row=sourceAnimations0C_[animationIndex_];row.completed30=0; //2CDC44.
        pc=0x2CDC54;
        const auto* name=values->find("ReferenceName");
        // These exact first three authenticated sections omit ReferenceName.
        // Later named rows use actual malloc104/publication before strncpy.
        if(!name||!name->sourceValuePresent||name->text=="none"){row.sourceName00=nullptr;row.name00Known=true;}
        else {
            pc=0x2CDC8C;row.nameBacking.reset(new(std::nothrow) char[260]);
            row.sourceName00=row.nameBacking.get();row.name00Known=true; //2CDCA4.
            if(!row.nameBacking)return fail(0x2CDCB4);
            // Retained Config parser has removed quotes; strncpy pads to260.
            std::strncpy(row.nameBacking.get(),name->text.c_str(),260);row.nameCopyReached=true;
        }
        std::int32_t start=1,end=1;pc=0x2CDCD0;
        if(!rr::assets::originalUiInteger50KG(*values,"StartFrame",1,start,d,pc))return fail(d.sourcePC);
        animationStartStack830_=start;pc=0x2CDCE8;
        if(!rr::assets::originalUiInteger50KG(*values,"EndFrame",1,end,d,pc))return fail(d.sourcePC);
        animationEndStack834_=end;
        // Exact source subtract-one/signed comparisons, followed by low16 stores.
        // Authenticated cohort ranges1..4 avoid native signed overflow.
        if(start<1||start>4||end<1||end>4)return fail(0x2CDCF0);
        row.first04=std::int16_t(start<end?start-1:end-1); //2CDD24.
        row.last06=std::int16_t(start>end?start-1:end-1); //2CDD5C.
        row.bit21(0,end<start); //2CDD94/B4; unrelated high bits remainUNKNOWN.
        row.current4C=float(*row.first04); //2CDDEC actual call delay store.
        float duration=0;pc=0x2CDDE8;
        if(!rr::assets::originalUiNumber50KG(*values,"AnimateTime",0,duration,d,pc))return fail(d.sourcePC);
        animationTimeStack840_=duration;
        bool loop=false,reverse=false;pc=0x2CDE00;
        if(!rr::assets::originalUiBoolean50KG(*values,"LoopAnimation",false,loop,d,pc))return fail(d.sourcePC);
        row.bit20(0,loop); //2CDE44 before next getter.
        pc=0x2CDE40;
        if(!rr::assets::originalUiBoolean50KG(*values,"ReversableAnimation",false,reverse,d,pc))return fail(d.sourcePC);
        row.bit20(4,reverse); //2CDE84.
        animationProductF1_=1000.0f*duration; //2CDE9C; input0 produces exact0.
        //2CDEAC divides f1 by f0=bits41855557 (about16.66667), NOT
        // the reverse. The authenticated zero numerator produces exact+0.
        if(duration!=0.0f)return fail(0x2CDEAC); // Broader float lowering unjoined.
        animationFramesF2_=0.0f;
        animationDeltaF0_=float(std::int32_t(*row.last06)-std::int32_t(*row.first04));
        // ScalarDIV.S (not VU Q), admitted only for exact first-row +0/+0.
        if(!divideAnimationZero2CDEC8(pc))return false;
        pc=0x2CDED8;row.rate34Bits=*animationRateQuotientF0Bits_;row.rate34Stored=true;
        return continueAnimation1AfterRateStore2CDED8(pc);
    }
    return fail(0x2CE990); // No complete loader receipt until all row arithmetic joins.
}catch(...){return false;} // pc already names current fallible native/source interval.
