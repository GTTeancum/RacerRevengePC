// Private inside the SAME Session. Fixed prewarmed backing is the Windows/Linux
// allocation policy here, NOT literal guest allocator/failure equivalence.
// No duplicate wrapper/payload, Entity count, query, registration or activation.
struct NativeCamera6468 {
    bool attempted=false,followConstructed=false,deathConstructed=false,deathLoaded=false;
    bool followRetired=false,deathRetired=false;
    std::uint32_t pc=0x174ec8,followVtable=0,deathVtable=0;
    //Source constructor-null resource slots, not numeric filter backing.
    //Derived[0..10]=12C..154,[11]=190; base[0..10]=28..50,
    //[11]=108,[12]=90,[13]=24. No loader has populated these roles.
    std::array<const void*,12> followDerived{};
    std::array<const void*,14> followBase{};
    const void* deathQuery120=nullptr;
    //Actual source128..144 publications alias the five-lane SAME numeric
    //objects; no copied scalar-filter payload or independent readiness owner.
    std::array<OriginalFollowFilter*,8> deathFilters{};
    std::size_t deathFiltersPublished=0;
} nativeCamera6468_;
OriginalFollowFilter* nativeDeathFilter(std::size_t i)noexcept {
    if(i==0)return &death_.speed128;if(i==1)return &death_.fov12C;
    if(i<5)return &death_.direction130[i-2];
    return i<8?&death_.debris13C[i-5]:nullptr;
}
bool nativeDeathFiltersCurrent()noexcept {
    auto& n=nativeCamera6468_;
    if(!n.deathConstructed||n.deathRetired||!n.deathLoaded||n.deathFiltersPublished!=8u||
       coldController_.children64[1]!=&death_)return false;
    for(std::size_t i=0;i<8u;++i)if(n.deathFilters[i]!=nativeDeathFilter(i))return false;
    return true;
}
bool adoptNativeCamera6468(std::uint32_t& pc)noexcept {
    auto& n=nativeCamera6468_;auto& c=coldController_;pc=n.pc;
    if(!onOwnerThread()||!actual_.followQueries.get()||n.attempted||!nativeRaOwnedPreparationAttempted_||
       !c.nativeLoadPrefixComplete||c.nativeLoadOperandsRevoked||!c.nativeScene3COwner||
       !c.race88||c.children64[0]||follow_.prepared||follow_.active)return false;
    n.attempted=true;
    //174EC8 adopts existing uncounted backing;179320→283A20→115AE0.
    //Only mapped source-written fields are touched; unrepresented/unwritten
    //native priors are not certified as guest values. No bulk C{} reset.
    pc=n.pc=0x179320;n.followVtable=0x3cca90u;n.followVtable=0x3d2f00u;
    follow_.historyC0={0,0,0,1};follow_.historyD0={0,0,0,1};
    follow_.fov10=80.f;n.followBase[13]=nullptr; //283A80:24
    for(std::size_t i=0;i<11;++i)n.followBase[i]=nullptr; //283A84..AAC:28..50
    follow_.distance04=0;follow_.distanceOffset58=0;follow_.yawOffset5C=0;
    follow_.pitchOffset60=0;follow_.fovOffset64=0;follow_.savedOffsets68.fill(0);
    follow_.reverseAngle78=0;follow_.reverseDistance7C=0;follow_.reverseLookAhead80=0;
    n.followBase[12]=nullptr; //283AE0:90
    follow_.bodySpeedScale88=0;follow_.bodyScale8C=3.f;follow_.angleY08=0;follow_.angleX0C=0;
    follow_.cameraSwing14=1;follow_.lookSwing18=1;follow_.current100=0;
    n.followBase[11]=nullptr; //283B0C:108
    follow_.snap10C=false;follow_.terrain110=0;
    n.followVtable=0x3ce440u;
    follow_.scratch180={0,0,0,1};follow_.collision.localPoint1A0={0,0,0,1};
    follow_.terrainProvider120Present=false;
    for(std::size_t i=0;i<11;++i)n.followDerived[i]=nullptr; //179358..380:12C..154
    follow_.heightOffset124=0;follow_.dynamicFov15C=0;follow_.dynamicDistance160=0;
    follow_.racerModeTimer164=0;follow_.debugCountdown168=0;follow_.debugIndex16C=0;
    follow_.shakeAmplitude170=0;follow_.shakeScale174=0;follow_.queryRadius178=3.5f;
    const auto& zero=actual_.followQueries.get()->sharedZero433DB0();
    std::memcpy(&follow_.scratch180,&zero,16);
    n.followDerived[11]=nullptr; //1793BC:190 between final vector stores
    std::memcpy(&follow_.collision.localPoint1A0,&zero,16);
    n.followConstructed=true;
    //174EE8 is the SECOND allocation's delay-slot publication:64 precedes68.
    pc=n.pc=0x174ee8;c.children64[0]=&follow_;
    pc=n.pc=0x174ee4;
    if(c.children64[1]||death_.prepared||death_.active)return false;
    //22CF20→286C30→115AE0, again SAME native backing, no new Entity.
    pc=n.pc=0x22cf20;n.deathVtable=0x3cca90u;n.deathVtable=0x3d2f40u;
    death_.fov0C=80;death_.bodyScale20=0;death_.targetHeight24=3;death_.started50=false;
    n.deathVtable=0x3d18c0u;
    death_.initial80={0,0,0,1};death_.velocity90={0,0,0,1};death_.lookA0={0,0,0,1};
    death_.positionB0={0,0,0,1};death_.previousC0={0,0,0,1};death_.fieldD0={0,0,0,1};
    death_.targetE0={0,0,0,1};death_.fieldF0={0,0,0,1};death_.field100={0,0,0,1};death_.debris110={0,0,0,1};
    death_.terrain60Present=false;n.deathQuery120=nullptr;death_.started50=false;
    death_.fieldD0={0,0,0,1};death_.targetE0={0,0,0,1};
    death_.fieldF0=deathGlobals_.zero434880;death_.field100=deathGlobals_.zero434880;
    death_.velocity90=deathGlobals_.zero434880;death_.lookA0=deathGlobals_.zero434880;
    death_.scale74=1;n.deathFilters[0]=nullptr;death_.field78=75;death_.radius7C=100;
    death_.debris110=deathGlobals_.zero434880;
    for(std::size_t i=1;i<5;++i)n.deathFilters[i]=nullptr;
    death_.flags14CLow2=static_cast<std::uint8_t>(death_.flags14CLow2&~1u);
    death_.flags14CLow2=static_cast<std::uint8_t>(death_.flags14CLow2&~2u);
    for(std::size_t i=5;i<8;++i)n.deathFilters[i]=nullptr;
    death_.mode148=2;n.deathConstructed=true;
    pc=n.pc=0x174efc;c.children64[1]=&death_;
    //Mandatory22D1C0 before later6C:286D00 scalar order then6C,64,68.
    pc=n.pc=0x286d00;death_.fov10=80;death_.minFov14=40;death_.distance18=20;death_.distance1C=100;
    pc=n.pc=0x22d1f8;death_.clearance6C=2;death_.fov64=60;death_.distance68=60;
    //Explicit fixed five-lane backing replaces eight guest20-byte allocations.
    //No failure equivalence is claimed; publication and store order is exact.
    constexpr std::uint32_t publication[8]={0x22d24c,0x22d29c,0x22d2e8,0x22d334,0x22d380,0x22d3d0,0x22d420,0x22d470};
    for(std::size_t i=0;i<8;++i){
        auto* f=nativeDeathFilter(i);if(!f||n.deathFilters[i])return false;
        f->value=i==0?1.f:i==1?death_.fov10:0.f;
        f->tau=i==0?2.5f:i==1?3.5f:i<5?.5f:std::bit_cast<float>(std::uint32_t{0x3e99999a});
        f->upper0C=std::bit_cast<float>(std::uint32_t{0x7f7fffff});
        f->lower10=std::bit_cast<float>(std::uint32_t{0xff7fffff});f->blend=1;
        pc=n.pc=publication[i];n.deathFilters[i]=f;++n.deathFiltersPublished;
    }
    pc=n.pc=0x22d484;death_.velocity90=deathGlobals_.zero434880;death_.scale74=1;
    n.deathLoaded=true;
    if(!nativeDeathFiltersCurrent())return false;
    pc=n.pc=0x174f50;return true; //Next6C adoption unjoined; NOT174DF0 return.
}
bool retireNativeCamera6468()noexcept {
    auto& n=nativeCamera6468_;auto& c=coldController_;
    if(!n.attempted)return true;
    if(!onOwnerThread()||(n.followConstructed&&(follow_.prepared||follow_.active))||
       (n.deathConstructed&&(death_.prepared||death_.active))||
       followChild_.follow64||deathChild_.death68)return false;
    if(n.followConstructed&&!n.followRetired){
        if(c.children64[0]!=&follow_)return false;
        //1793E0 derived resources then283B60 base resources. They remain
        //actual constructor-null: no179580/query/preset loader was reached.
        for(const auto* role:n.followDerived)if(role)return false;
        for(const auto* role:n.followBase)if(role)return false;
        n.followVtable=0x3ce440u;n.followVtable=0x3d2f00u;n.followVtable=0x3cca90u;
        c.children64[0]=nullptr;n.followRetired=true; //Native borrow withdrawal, not guest64 store.
    }
    if(n.deathConstructed&&!n.deathRetired){
        if(c.children64[1]!=&death_||n.deathQuery120||n.deathFiltersPublished>8)return false;
        //Preflight the whole published prefix before withdrawing any alias;
        //a rejected cleanup can retry without mistaking earlier withdrawals.
        for(std::size_t i=0;i<n.deathFiltersPublished;++i)
            if(n.deathFilters[i]!=nativeDeathFilter(i))return false;
        n.deathVtable=0x3d18c0u;
        for(std::size_t i=0;i<n.deathFiltersPublished;++i)
            n.deathFilters[i]=nullptr; //Fixed backing retained; no heap-free claim.
        n.deathVtable=0x3d2f40u;n.deathVtable=0x3cca90u;
        c.children64[1]=nullptr;n.deathRetired=true;
    }
    return true;
}
