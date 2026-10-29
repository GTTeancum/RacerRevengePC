// Private SAME attached_ backing. Explicit native fixed-storage adoption of
//13DCA0, not a second payload or literal guest allocation/failure behavior.
struct NativeAttached6C {
    bool attempted=false,constructed=false,retired=false,completed=false;
    bool cuts74Published=false,cuts74Withdrawn=false;
    unsigned cuts74Rows=0;
    std::uint32_t pc=0x174f50,vtable00=0;
    const void* mainBody20=nullptr;
    const void* model24=nullptr;
    //13DCA0 clears source pointer slots; numeric direction30 backing is NOT
    //allocated/published until the later13DDB0 call, which is not reached here.
    std::array<OriginalAttachedFilter*,3> filters30{};
} nativeAttached6C_;
bool adoptNativeAttached6CAnd70(std::uint32_t& pc)noexcept {
    auto& n=nativeAttached6C_;auto& c=coldController_;pc=n.pc;
    if(!onOwnerThread()||n.attempted||!nativeRaOwnedPreparationAttempted_||
       !nativeDeathFiltersCurrent()||!c.nativeLoadPrefixComplete||
       c.nativeLoadOperandsRevoked||!c.nativeScene3COwner||!c.race88||
       c.children64[2]||attached_.prepared||attached_.active)return false;
    n.attempted=true;
    pc=n.pc=0x13dca0;n.vtable00=0x3cca90u;n.vtable00=0x3cd000u;
    attached_.offset10={0,0,0,1};attached_.fov04=80;
    n.mainBody20=nullptr;n.model24=nullptr;
    attached_.shakeAmplitude28=0;attached_.shakeScale2C=0;
    n.filters30[0]=nullptr;n.filters30[1]=nullptr;n.filters30[2]=nullptr;
    attached_.snap3C=false;n.constructed=true;
    //174F70 is in the next70 allocation delay slot. Retain6C publication
    //even if the following prewarmed70 identity/adoption admission rejects.
    pc=n.pc=0x174f70;c.children64[2]=&attached_;
    pc=n.pc=0x174f6c;
    if(!nativeCinematographer70Attempted_||!nativeCinematographer70_||
       nativeCinematographer70Retired_||nativeCinematographer70Published_||
       c.children64[3])return false;
    //Existing real118A10 backing was constructed once during native staging.
    //Adopt, do not replay its constructor or claim guest allocation timing.
    pc=n.pc=0x174f84;c.children64[3]=nativeCinematographer70_.get();
    nativeCinematographer70Published_=true;
    pc=n.pc=0x174f88;
    if((c.halfA8Known&0x1f0u)!=0x1f0u)return false;
    const auto count=(c.halfA8Value>>4)&31u;
    //The source signed five-bit extract yields -16..15, not an authored
    //unsigned count. Next positive branch allocates74; nonpositive skips it.
    pc=n.pc=(count&&count<16u)?0x174fa4u:0x175044u;
    if(count&&count<16u){
        //Use actual A8, never authored count as the source branch authority.
        //The typed fixed-capacity owner is the native allocation policy; its
        //prewarmed children are adopted without allocating/replaying282860.
        if(!taCuts_||taCuts_->loaded()||taCuts_->cutCount()!=count||count>7u||
           c.children64[4]||!actual_.applicationEntities||
           actual_.applicationEntities->cutOrigin443200_!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}})
            return false;
        pc=n.pc=0x174fb4;c.children64[4]=taCuts_.get();n.cuts74Published=true;
        for(unsigned i=0;i<count;++i){
            pc=n.pc=0x174fc4;
            const auto* child=taCuts_->child(i+1);
            if(!child||child->ordinal()!=i+1||child->failed()||child->activated()||
               child->look20()||child->up24()||child->position28()||child->scalar50())return false;
            //The same collection's actual pointer row, not a second array.
            //Its retained prefix bounds source publication, while later
            //prewarmed rows are still native backing only.
            pc=n.pc=0x174ff0;++n.cuts74Rows;
            //175000 invokes282990 with a1/a2/a3=0. Its exact null branch
            //returns1 at2829B0 without touching any child field.
            pc=n.pc=0x175000;
        }
        pc=n.pc=0x175038; //Mandatory177750 remains the next required call.
    }
    n.completed=true;return true;
}
bool retireNativeAttached6C()noexcept {
    auto& n=nativeAttached6C_;
    if(!n.attempted)return true;
    if(!onOwnerThread())return false;
    if(n.retired)return !coldController_.children64[2];
    if(!n.constructed)return true;
    if(coldController_.children64[2]!=&attached_||attached_.prepared||attached_.active||
       attachedChild_.attached6C||n.mainBody20||n.model24)return false;
    for(const auto* f:n.filters30)if(f)return false;
    //13DD10: three actual constructor-null branches, then115B00. Native
    //borrow withdrawal leaves fixed backing inert; it does not claim a free.
    n.vtable00=0x3cd000u;n.vtable00=0x3cca90u;
    coldController_.children64[2]=nullptr;n.retired=true;return true;
}

bool withdrawNativeCamera74()noexcept {
    auto& n=nativeAttached6C_;
    if(!n.attempted)return true; //Never-adopted standalone route is unchanged.
    if(!onOwnerThread())return false;
    if(n.cuts74Withdrawn)return !coldController_.children64[4];
    if(!n.cuts74Published)return !coldController_.children64[4];
    if(!taCuts_||coldController_.children64[4]!=taCuts_.get()||
       n.cuts74Rows>taCuts_->cutCount())return false;
    //No source consumer may borrow unpublished rows. Registry admission stays
    //behind the original phase5 fence. Revoke owner alias BEFORE freeing the
    //same collection; native cleanup also owns its unadopted staging suffix.
    coldController_.children64[4]=nullptr;n.cuts74Withdrawn=true;return true;
}

bool nativeCamera74ConsumersCurrent()const noexcept {
    if(!nativeAttached6C_.attempted)return true;
    const auto& n=nativeAttached6C_;const auto& c=coldController_;
    if(!n.completed||!n.cuts74Published||n.cuts74Withdrawn||!taCuts_||
       c.children64[4]!=taCuts_.get()||(c.halfA8Known&0x1f0u)!=0x1f0u)return false;
    const auto count=(c.halfA8Value>>4)&31u;
    return count>0&&count<16u&&n.cuts74Rows==count&&taCuts_->cutCount()==count;
}
