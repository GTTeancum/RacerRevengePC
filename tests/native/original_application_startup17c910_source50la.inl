// Included inside original_main_entry50KG after Application/Video definitions.
inline rr::game::OriginalApplicationStartup17C910Source50LA::Result
Application::enterSourceStartup17C910()const noexcept {
    using R=rr::game::OriginalApplicationStartup17C910Source50LA::Result;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!enabled)
        return R::authority;
    if(sourceStartup17C910_.nativeCaller_&&
       (sourceStartup17C910_.nativeCaller_!=&nativeStartupCall23A0DC_||
        !nativeStartupCall23A0DC_.current(*this)||
        nativeStartupCall23A0DC_.phase_!=rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Phase::issuing||
        nativeStartupCall23A0DC_.body_!=&sourceStartup17C910_))return R::phase;
    //Do not claim this source call owns already-created input or source Video,
    //or reset directories after the canonical APPSTART consumers have run.
    if(sourceInput04_||sourcePadHostLease_||sourceVideoProduction_.attempted_||
       !standardMaterialDomainOwner_||standardMaterialDomainOwner_->serviceAppStart_||
       standardMaterialDomainOwner_->earlyAppStartMaterials_||standardMaterialDomainOwner_->programs_||
       standardMaterialDomainOwner_->application_||!standardMaterialDomainOwner_->resolver_||
       !standardMaterialDomainOwner_->resolver_->nativeBeforeAppStart17CAB8()||
       (materialApplication&&materialApplication->appStart()))
        return R::phase;
    return sourceStartup17C910_.begin(*this);
}

inline rr::game::OriginalApplicationStartup17C910Source50LA::Result
Application::continueNativeNormalStartup17C910()const noexcept {
    using S=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=S::Result;using P=S::Phase;auto& s=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||
       !enabled||!s.current(*this))return R::authority;
    if(s.phase_==P::collisionDirectoryPending)return R::collisionDirectoryPending;
    //Recheck real downstream history on every resume. A failed registration
    //attempt is also later work; absence of a completed archive is not coldness.
    if(sourceVideoProduction_.attempted_||!standardMaterialDomainOwner_||
       standardMaterialDomainOwner_->serviceAppStart_||standardMaterialDomainOwner_->earlyAppStartMaterials_||
       standardMaterialDomainOwner_->programs_||standardMaterialDomainOwner_->application_||
       !standardMaterialDomainOwner_->resolver_||
       !standardMaterialDomainOwner_->resolver_->nativeBeforeAppStart17CAB8()||
       (materialApplication&&materialApplication->appStart()))return R::phase;
    if(s.phase_==P::argumentsPending){
        //Explicit bounded native normal-UI policy. Do NOT write435378/435380,
        //338/344/298/2E0 or claim17C924's load/comparisons/branch executed.
        //Special original argument modes remain unsupported, not defaulted.
        if(sourceInput04_||sourcePadHostLease_||sourceVideoProduction_.attempted_||
           !sourceUseProfile2E0_||*sourceUseProfile2E0_!=1||sourceBuildUiResFiles344_!=0)
            return R::phase;
        s.nativeNormalEntry_=true;s.phase_=P::inputPending;s.pc_=0x17c9f0;
    }
    if(s.phase_==P::inputPending){
        if(!s.input_){
            if(sourceInput04_)return R::authority;
            //The existing private producer owns13FD40 ->237430 exactly once.
            //Its native bool false means pending/blocked, NOT source return0.
            (void)constructSourceInput04_13FD40();
            if(!sourceInput04_)return R::nativeAllocationPending;
            s.input_=sourceInput04_;
        }
        if(sourceInput04_!=s.input_||s.input_->application_!=this||
           s.input_->thread_!=std::this_thread::get_id()||s.input_->nativeClosed_)
            return R::authority;
        s.lastInputPC_=s.input_->pc_;
        if(!s.input_->startupReturnedWithNativeTransport_){
            if(s.input_->pc_!=0x2374bc)return R::inputPrefixBlocked;
            //Use only the existing live same-App Window transport. This does
            //not create a Window, enable controllers, poll/open devices or RPC.
            (void)continueSourceInput04NativeActivation();
            s.lastInputPC_=s.input_->pc_;
            if(!s.input_->startupReturnedWithNativeTransport_)
                return s.input_->pc_==0x2374bc?R::inputNativePending:R::inputPrefixBlocked;
        }
        if(s.input_->pc_!=0x13fd98||!sourceInput04NativeBorrowCurrent(s.input_.get()))
            return R::authority;
        //The exact descendant's private return is consumed, not a supplied
        //acknowledgment or copied ready bit.17C910 does not branch on its v0.
        s.inputReturnObserved_=true;s.phase_=P::directories;s.pc_=0x17c9f8;
    }
    if(s.phase_!=P::directories||!s.inputReturnObserved_||sourceInput04_!=s.input_||
       !sourceInput04NativeBorrowCurrent(s.input_.get()))return R::authority;
    //Native string preparation may allocate. Each completed source copy is
    //retained; retry starts at the first uncommitted copy, not at13FD40.
    try {
        if(s.directoryCopies_==0){
            s.pc_=0x17ca1c;std::string next="anim\\";
            animationDirectory36AED0_.emplace(std::move(next));
            s.directoryCopies_=1;s.pc_=0x17ca24; //17CA1C word +17CA20 half.
        }
        if(s.directoryCopies_==1){
            s.pc_=0x17ca4c;std::string next="geometry\\";
            modelDirectory36B860_.swap(next);
            s.directoryCopies_=2;s.pc_=0x17ca54; //17CA4C doubleword +17CA50 half.
        }
        if(s.directoryCopies_==2){
            s.pc_=0x17ca6c;std::string next="textures\\";
            materialDirectory36B970_.swap(next);
            s.directoryCopies_=3;s.pc_=0x17ca74; //17CA6C doubleword +17CA70 half.
        }
    }catch(...){return R::nativeAllocationPending;}
    //Next17CA88/90 writes36B0F0, already owned by entitiesOwner_ and used by
    //native collision borrowers. Do not invent a second directory or silently
    //replace that domain. Track-directory store17CAAC is also not reached.
    s.pc_=0x17ca88;s.phase_=P::collisionDirectoryPending;
    //12CEA0/17CAB8, material/shader calls,127AD0 mappings,17C910 return,
    //23A100/Video startup and every platform/IRQ/mode operation remain pending.
    return R::collisionDirectoryPending;
}

inline rr::game::OriginalApplicationStartup17C910Source50LA::Result
Application::continueSourceStartupDirectories17CA88()const noexcept {
    using S=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=S::Result;using P=S::Phase;auto& s=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||
       !enabled||!s.current(*this)||!s.nativeNormalEntry_||!s.inputReturnObserved_||
       sourceInput04_!=s.input_||!sourceInput04NativeBorrowCurrent(s.input_.get()))return R::authority;
    if(s.phase_!=P::collisionDirectoryPending&&s.phase_!=P::trackDirectoryPending)return R::phase;
    if(sourceVideoProduction_.attempted_||!standardMaterialDomainOwner_||
       standardMaterialDomainOwner_->serviceAppStart_||standardMaterialDomainOwner_->earlyAppStartMaterials_||
       standardMaterialDomainOwner_->programs_||standardMaterialDomainOwner_->application_||
       !standardMaterialDomainOwner_->resolver_||
       !standardMaterialDomainOwner_->resolver_->nativeBeforeAppStart17CAB8()||
       (materialApplication&&materialApplication->appStart()))return R::phase;
    if(s.phase_==P::collisionDirectoryPending){
        if(s.pc_!=0x17ca88||s.directoryCopies_!=3)return R::phase;
        using C=rr::game::OriginalApplicationEntities50LA::CollisionDirectoryStartupResult;
        const auto copied=entitiesOwner_->writeCollisionDirectory17CA88();
        if(copied==C::authority)return R::authority;
        if(copied==C::used)return R::collisionDirectoryUsed;
        if(copied==C::allocation)return R::nativeAllocationPending;
        //Same process owner and exact literal, never a shadow directory or
        //a copy of mutable modelDirectory36B860_. This source copy is retained.
        s.directoryCopies_=4;s.phase_=P::trackDirectoryPending;s.pc_=0x17caac;
    }
    if(s.pc_!=0x17caac||s.directoryCopies_!=4)return R::phase;
    try {std::string next="tracks\\";trackDirectory36B640_.swap(next);}
    catch(...){return R::nativeAllocationPending;}
    s.directoryCopies_=5;s.phase_=P::appStartInputPending;s.pc_=0x17cab8;
    //17CAB0's outgoing zero stack argument and fixed literal call arguments
    //belong to the next bounded call. No file registration has run yet.
    return R::appStartInputPending;
}

inline rr::game::OriginalApplicationStartup17C910Source50LA::Result
Application::continueSourceStartupAppStart17CAB8()const noexcept {
    using S=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=S::Result;using P=S::Phase;auto& s=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||
       !enabled||!s.current(*this)||!s.nativeNormalEntry_||!s.inputReturnObserved_||
       s.directoryCopies_!=5||sourceInput04_!=s.input_||
       !sourceInput04NativeBorrowCurrent(s.input_.get()))return R::authority;
    if(s.phase_==P::appStartRegistrationBlocked)return R::appStartRegistrationBlocked;
    if(s.phase_==P::appStartRegistered){
        return s.appStartRegistrationReturned_&&s.appStartDomain_==standardMaterialDomainOwner_&&
            s.appStartResolver_==standardMaterialDomainOwner_->resolver_&&
            s.appStartResolver_->registeredAppStart17CAB8()==s.appStart_?
                R::appStartRegistered:R::authority;
    }
    if(s.phase_!=P::appStartInputPending||s.pc_!=0x17cab8||s.appStartRegistrationAttempted_)
        return R::phase;
    if(sourceVideoProduction_.attempted_||!standardMaterialDomainOwner_||
       standardMaterialDomainOwner_->serviceAppStart_||standardMaterialDomainOwner_->earlyAppStartMaterials_||
       standardMaterialDomainOwner_->programs_||standardMaterialDomainOwner_->application_||
       !standardMaterialDomainOwner_->resolver_||
       !standardMaterialDomainOwner_->resolver_->nativeBeforeAppStart17CAB8()||
       (materialApplication&&materialApplication->appStart()))return R::phase;
    if(!s.appStartDomain_){
        s.appStartDomain_=standardMaterialDomainOwner_;
        s.appStartResolver_=standardMaterialDomainOwner_->resolver_;
    }
    if(s.appStartDomain_!=standardMaterialDomainOwner_||
       s.appStartResolver_!=standardMaterialDomainOwner_->resolver_)return R::authority;
    if(!s.appStart_){
        //Existing authenticated Application loader; no caller-supplied archive,
        //guest open result, mount-derived receipt, or unverified resource name.
        if(assetRoot.empty())return R::appStartInputUnavailable;
        try {s.appStart_=readArchive(assetRoot,rr::assets::OriginalUiArchiveId50KG::appStart);}
        catch(...){return R::appStartInputUnavailable;}
        if(!s.appStart_)return R::appStartInputUnavailable;
    }
    //The native receiver is the SAME already-owned canonical file service.
    //This does not fabricate raw3E1A70 pointer bits or its13FB6C publication.
    if(!s.appStartResolver_->nativeBeforeAppStart17CAB8())return R::phase;
    rr::assets::OriginalStandardTextureDiagnostic50LA d;
    s.appStartRegistrationAttempted_=true;s.phase_=P::appStartRegistrationBlocked;
    if(!s.appStartResolver_->registerAppStart17CAB8(s.appStart_,d)){
        if(d.sourcePC)s.pc_=d.sourcePC;
        //Terminal for THIS call, preserving actual reservation/cleanup/partial
        //history in the canonical resolver. Never retry a failed12CEA0 here.
        return R::appStartRegistrationBlocked;
    }
    s.appStartRegistrationReturned_=true;s.phase_=P::appStartRegistered;s.pc_=0x17cac0;
    //Actual activate=0 registration only. No native mount,17CAD4 material read,
    //shader/template calls,127AD0 mapping,17C910 return, Video or IRQ work.
    return R::appStartRegistered;
}

inline rr::game::OriginalApplicationStartup17C910Source50LA::Result
Application::continueSourceStartupMaterialRead17CAD4()const noexcept {
    using S=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=S::Result;using P=S::Phase;auto& s=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||
       !enabled||!s.current(*this)||!s.nativeNormalEntry_||!s.inputReturnObserved_||
       s.directoryCopies_!=5||sourceInput04_!=s.input_||
       !sourceInput04NativeBorrowCurrent(s.input_.get()))return R::authority;
    //Immutable Application thread authority precedes mutable domain/history.
    if(!s.appStartRegistrationReturned_||!s.appStart_||!s.appStartDomain_||
       s.appStartDomain_!=standardMaterialDomainOwner_||!s.appStartResolver_||
       s.appStartResolver_!=s.appStartDomain_->resolver_||
       s.appStartResolver_->registeredAppStart17CAB8()!=s.appStart_)return R::authority;
    auto& domain=*s.appStartDomain_;
    if(s.phase_==P::appStartNativeMountBlocked)return R::appStartNativeMountBlocked;
    if(s.phase_==P::appStartNamesReadBlocked)return R::appStartNamesReadBlocked;
    //Neither a later lazy appStart() nor another resource consumer may be
    //adopted as this source call's work. Refuse every later mutable owner.
    if(sourceVideoProduction_.attempted_||domain.programs_||domain.application_||domain.prepared()||
       (materialApplication&&materialApplication->appStart()))return R::phase;
    if(s.phase_==P::appStartRegistered){
        if(s.pc_!=0x17cac0||s.appStartNativeMountAttempted_||domain.serviceAppStart_||
           domain.earlyAppStartMaterials_||!domain.archiveProvider_||
           domain.archiveProvider_->mountCount()!=0||domain.archiveProvider_->revision()!=0||
           domain.archiveProvider_->sealed())return R::phase;
        s.appStartProvider_=domain.archiveProvider_;
        //Native prerequisite only.17CAB8 already returned with activate=0.
        //The authenticated mount/join is not source activation or a file read.
        //A failed join can leave real mounted/partially joined rows: retain
        //that exact provider and block, never remount or invent rollback.
        s.appStartNativeMountAttempted_=true;s.phase_=P::appStartNativeMountBlocked;
        rr::assets::OriginalStandardMaterialDiagnostic50LA d;
        if(!domain.retainNativeAppStart(s.appStart_,d)){
            s.lastMaterialPC_=d.sourcePC?d.sourcePC:d.texture.sourcePC;
            return R::appStartNativeMountBlocked;
        }
        if(domain.serviceAppStart_!=s.appStart_||domain.archiveProvider_!=s.appStartProvider_||
           s.appStartProvider_->mountCount()!=1||s.appStartProvider_->sealed())return R::authority;
        s.appStartProviderRevision_=s.appStartProvider_->revision();
        s.appStartNativeMountReturned_=true;s.phase_=P::appStartNamesReadPending;s.pc_=0x17cad4;
    }
    if(s.phase_!=P::appStartNamesReadPending&&s.phase_!=P::appStartNamesParsePending)return R::phase;
    if(!s.appStartNativeMountReturned_||!s.appStartProvider_||
       domain.archiveProvider_!=s.appStartProvider_||domain.serviceAppStart_!=s.appStart_||
       s.appStartProvider_->mountCount()!=1||s.appStartProvider_->sealed()||
       s.appStartProvider_->revision()!=s.appStartProviderRevision_)return R::authority;
    if(s.phase_==P::appStartNamesParsePending){
        //Read-only repeat of the same owned boundary, not a second read or
        //permission to parse/promote a table or complete the source call.
        return s.pc_==0x29871c&&s.appStartNamesReadReturned_&&
            domain.sourceAppStartNamesReadCurrent(s.appStartMaterials_,s.appStart_)?
                R::appStartNamesParsePending:R::authority;
    }
    if(s.pc_!=0x17cad4||s.appStartNamesCallIssued_||domain.earlyAppStartMaterials_)return R::phase;
    //Actual global382B38 receiver is bound to the SAME native canonical table.
    //No guest pointer/vtable bytes or preceding static constructor are faked.
    s.appStartNamesCallIssued_=true;s.phase_=P::appStartNamesReadBlocked;
    rr::assets::OriginalStandardMaterialDiagnostic50LA d;
    const bool read=domain.readSourceAppStartNames17CAD4(d);
    s.appStartMaterials_=domain.earlyAppStartMaterials_; // Also pin partial failure.
    s.lastMaterialPC_=d.sourcePC?d.sourcePC:d.texture.sourcePC;
    if(!read){
        if(s.lastMaterialPC_)s.pc_=s.lastMaterialPC_;
        //Terminal for THIS call. The actual table keeps partial output and
        //the canonical resolver keeps reached device/cache/close effects.
        //Native failure is never guest return0, EOF, or17CAD4 completion.
        return R::appStartNamesReadBlocked;
    }
    if(!domain.sourceAppStartNamesReadCurrent(s.appStartMaterials_,s.appStart_))return R::authority;
    s.appStartNamesReadReturned_=true;s.phase_=P::appStartNamesParsePending;
    s.pc_=0x29871c;s.lastMaterialPC_=0x29871c;
    //298714/18 FClose returned. Next29871C calls3094E0. Source tokenization,
    //row allocation2987DC, association allocation14D31C,17CAD4 return,
    //DefaultMT17CAE8,ScreenMT17CAFC,127AD0 mappings and17C910 return remain.
    return R::appStartNamesParsePending;
}

inline rr::game::OriginalApplicationStartup17C910Source50LA::Result
Application::continueSourceStartupMaterialRows29871C()const noexcept {
    using S=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=S::Result;using P=S::Phase;auto& s=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||
       !enabled||!s.current(*this)||!s.nativeNormalEntry_||!s.inputReturnObserved_||
       s.directoryCopies_!=5||sourceInput04_!=s.input_||
       !sourceInput04NativeBorrowCurrent(s.input_.get()))return R::authority;
    if(!s.appStartRegistrationReturned_||!s.appStartNativeMountReturned_||!s.appStartNamesReadReturned_||
       !s.appStart_||!s.appStartDomain_||s.appStartDomain_!=standardMaterialDomainOwner_||
       !s.appStartResolver_||s.appStartResolver_!=s.appStartDomain_->resolver_||
       s.appStartResolver_->registeredAppStart17CAB8()!=s.appStart_)return R::authority;
    auto& domain=*s.appStartDomain_;
    if(s.phase_==P::appStartNameRowsBlocked)return R::appStartNameRowsBlocked;
    if(sourceVideoProduction_.attempted_||domain.programs_||domain.application_||domain.prepared()||
       (materialApplication&&materialApplication->appStart()))return R::phase;
    if(!s.appStartProvider_||domain.archiveProvider_!=s.appStartProvider_||
       domain.serviceAppStart_!=s.appStart_||s.appStartProvider_->mountCount()!=1||
       s.appStartProvider_->sealed()||s.appStartProvider_->revision()!=s.appStartProviderRevision_)
        return R::authority;
    if(s.phase_==P::appStartNameRowsPending){
        return s.pc_==0x2987e8&&s.appStartNameRowsPublished_&&
            domain.sourceAppStartNameRowsCurrent(s.appStartMaterials_,s.appStart_)?
                R::appStartNameRowsPending:R::authority;
    }
    if(s.phase_!=P::appStartNamesParsePending||s.pc_!=0x29871c||s.appStartNameRowsCallIssued_||
       !domain.sourceAppStartNamesReadCurrent(s.appStartMaterials_,s.appStart_))return R::phase;
    //Only this current owner may mutate its actual retained reader output.
    //No new archive read, remount, semantic-parser substitute or copied rows.
    s.appStartNameRowsCallIssued_=true;s.phase_=P::appStartNameRowsBlocked;
    rr::assets::OriginalStandardMaterialDiagnostic50LA d;
    if(!domain.allocateSourceAppStartNameRows29871C(s.appStartMaterials_,s.appStart_,d)){
        s.lastMaterialPC_=d.sourcePC;
        if(s.lastMaterialPC_)s.pc_=s.lastMaterialPC_;
        //Lowercase/tokenization/count may already be committed. Native heap
        //failure does NOT establish the source new-handler outcome orNULL+1C.
        //Keep this occurrence terminal rather than restarting its source call.
        return R::appStartNameRowsBlocked;
    }
    if(!domain.sourceAppStartNameRowsCurrent(s.appStartMaterials_,s.appStart_))return R::authority;
    s.appStartNameRowsPublished_=true;s.phase_=P::appStartNameRowsPending;
    s.pc_=0x2987e8;s.lastMaterialPC_=0x2987e8;
    //The raw allocation is owned, but every row byte is still unwritten.
    //Source row stores29888C/2988A0/2988B0,capacity+14,base-reader return,
    //association+30 allocation,templates,17C910 return andVideo remain pending.
    return R::appStartNameRowsPending;
}

inline rr::game::OriginalApplicationStartup17C910Source50LA::Result
Application::continueSourceStartupMaterialPopulation2987E8()const noexcept {
    using S=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=S::Result;using P=S::Phase;auto& s=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||
       !enabled||!s.current(*this)||!s.nativeNormalEntry_||!s.inputReturnObserved_||
       s.directoryCopies_!=5||sourceInput04_!=s.input_||
       !sourceInput04NativeBorrowCurrent(s.input_.get()))return R::authority;
    if(!s.appStartRegistrationReturned_||!s.appStartNativeMountReturned_||!s.appStartNamesReadReturned_||
       !s.appStartNameRowsPublished_||!s.appStart_||!s.appStartDomain_||
       s.appStartDomain_!=standardMaterialDomainOwner_||!s.appStartResolver_||
       s.appStartResolver_!=s.appStartDomain_->resolver_||
       s.appStartResolver_->registeredAppStart17CAB8()!=s.appStart_)return R::authority;
    auto& domain=*s.appStartDomain_;
    if(s.phase_==P::appStartMaterialPopulationBlocked)return R::appStartMaterialPopulationBlocked;
    if(sourceVideoProduction_.attempted_||domain.programs_||domain.application_||domain.prepared()||
       (materialApplication&&materialApplication->appStart()))return R::phase;
    if(!s.appStartProvider_||domain.archiveProvider_!=s.appStartProvider_||
       domain.serviceAppStart_!=s.appStart_||s.appStartProvider_->mountCount()!=1||
       s.appStartProvider_->sealed()||s.appStartProvider_->revision()!=s.appStartProviderRevision_)
        return R::authority;
    if(s.phase_==P::appStartMaterialNamesReturned){
        return s.pc_==0x17cadc&&s.appStartMaterialNamesReturned_&&
            domain.sourceAppStartMaterialNamesCurrent(s.appStartMaterials_,s.appStart_)?
                R::appStartMaterialNamesReturned:R::authority;
    }
    if(s.phase_!=P::appStartNameRowsPending||s.pc_!=0x2987e8||s.appStartMaterialPopulationCallIssued_||
       !domain.sourceAppStartNameRowsCurrent(s.appStartMaterials_,s.appStart_))return R::phase;
    s.appStartMaterialPopulationCallIssued_=true;s.phase_=P::appStartMaterialPopulationBlocked;
    rr::assets::OriginalStandardMaterialDiagnostic50LA d;
    if(!domain.populateSourceAppStartNameRows2987E8(s.appStartMaterials_,s.appStart_,d)){
        s.lastMaterialPC_=d.sourcePC;
        if(s.lastMaterialPC_)s.pc_=s.lastMaterialPC_;
        //The same table retains row masks/name aliases and any capacity/base
        //return reached before an association failure. Never replay or rollback.
        return R::appStartMaterialPopulationBlocked;
    }
    if(!domain.sourceAppStartMaterialNamesCurrent(s.appStartMaterials_,s.appStart_))return R::authority;
    s.appStartMaterialNamesReturned_=true;s.phase_=P::appStartMaterialNamesReturned;
    s.pc_=0x17cadc;s.lastMaterialPC_=0x17cadc;
    //The actual bounded14D2F0 call returned after both owned arrays and stores.
    //This is not a native template/material-domain preparation receipt.17CAE8,
    //17CAFC,127AD0,17C910 return,source Video and all platform work remain.
    return R::appStartMaterialNamesReturned;
}

inline rr::game::OriginalApplicationStartup17C910Source50LA::Result
Application::continueSourceStartupDefaultTemplate17CAE8()const noexcept {
    using S=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=S::Result;using P=S::Phase;auto& s=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||
       !enabled||!s.current(*this)||!s.nativeNormalEntry_||!s.inputReturnObserved_||
       s.directoryCopies_!=5||sourceInput04_!=s.input_||
       !sourceInput04NativeBorrowCurrent(s.input_.get()))return R::authority;
    if(!s.appStartRegistrationReturned_||!s.appStartNativeMountReturned_||!s.appStartMaterialNamesReturned_||
       !s.appStart_||!s.appStartDomain_||s.appStartDomain_!=standardMaterialDomainOwner_||
       !s.appStartResolver_||s.appStartResolver_!=s.appStartDomain_->resolver_||
       s.appStartResolver_->registeredAppStart17CAB8()!=s.appStart_)return R::authority;
    auto& domain=*s.appStartDomain_;
    if(s.phase_==P::appStartDefaultTemplateBlocked)return R::appStartDefaultTemplateBlocked;
    if(sourceVideoProduction_.attempted_||domain.application_||domain.prepared())return R::phase;
    if(!s.appStartProvider_||domain.archiveProvider_!=s.appStartProvider_||
       domain.serviceAppStart_!=s.appStart_||s.appStartProvider_->mountCount()!=1||
       s.appStartProvider_->sealed()||s.appStartProvider_->revision()!=s.appStartProviderRevision_||
       !domain.sourceAppStartMaterialNamesCurrent(s.appStartMaterials_,s.appStart_))return R::authority;
    if(s.phase_==P::appStartDefaultTemplateReturned){
        return s.pc_==0x17caf0&&s.appStartDefaultTemplateReturned_&&s.appStartNativeProgramsReturned_&&
            s.appStartPrograms_&&domain.programs_==s.appStartPrograms_&&s.appStartTemplateApplication_&&
            s.appStartTemplateApplication_==materialApplication&&
            s.appStartTemplateApplication_->sourceDefault17CAE8Current(s.appStart_,s.appStartResolver_,s.appStartPrograms_)&&
            s.appStartResolver_->sourceDefaultTemplate17CAE8Closed(s.appStart_)?
                R::appStartDefaultTemplateReturned:R::authority;
    }
    if(s.phase_!=P::appStartMaterialNamesReturned||s.pc_!=0x17cadc||s.appStartNativeProgramsAttempted_||
       s.appStartDefaultTemplateCallIssued_||domain.programs_||!materialApplication||
       materialApplication->appStart()||materialApplication->prepared())return R::phase;
    s.appStartTemplateApplication_=materialApplication;
    s.appStartNativeProgramsAttempted_=true;s.phase_=P::appStartDefaultTemplateBlocked;
    rr::assets::OriginalStandardMaterialDiagnostic50LA programDiagnostic;
    const bool names=domain.prepareNativeSourceStartupPrograms17CAE8(
        s.appStartMaterials_,s.appStart_,programDiagnostic);
    s.appStartPrograms_=domain.programs_; // Retain even a partial native prerequisite.
    if(!names){
        s.lastMaterialPC_=programDiagnostic.sourcePC?programDiagnostic.sourcePC:programDiagnostic.program.sourcePC;
        //Native prerequisite failure stays at17CADC. No source static initializer
        //or17CAE8 call has been claimed from a native diagnostic location.
        return R::appStartDefaultTemplateBlocked;
    }
    s.appStartNativeProgramsReturned_=true;
    s.pc_=0x17cae8;s.appStartDefaultTemplateCallIssued_=true;
    rr::assets::OriginalUiMaterialDiagnostic50KG d;
    if(!s.appStartTemplateApplication_->prepareSourceDefault17CAE8(
        s.appStart_,s.appStartResolver_,s.appStartPrograms_,d)){
        s.lastMaterialPC_=d.sourcePC?d.sourcePC:d.document.sourcePC;
        if(s.lastMaterialPC_)s.pc_=s.lastMaterialPC_;
        //Same persistent template owner and real file-service history remain.
        //No retry, ScreenMT fallback, cache publication or guest return0.
        return R::appStartDefaultTemplateBlocked;
    }
    if(s.appStartTemplateApplication_!=materialApplication||domain.programs_!=s.appStartPrograms_||
       !s.appStartTemplateApplication_->sourceDefault17CAE8Current(s.appStart_,s.appStartResolver_,s.appStartPrograms_)||
       !s.appStartResolver_->sourceDefaultTemplate17CAE8Closed(s.appStart_))return R::authority;
    s.appStartDefaultTemplateReturned_=true;s.phase_=P::appStartDefaultTemplateReturned;
    s.pc_=0x17caf0;s.lastMaterialPC_=0x17caf0;
    //Actual DefaultMT24A9A0 completed; only then may the later ScreenMT17CAFC
    //call occur. Public templates/materials, mappings,17C910 andVideo stay closed.
    return R::appStartDefaultTemplateReturned;
}

inline rr::game::OriginalApplicationStartup17C910Source50LA::Result
Application::continueSourceStartupScreenTemplate17CAFC()const noexcept {
    using S=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=S::Result;using P=S::Phase;auto& s=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||
       !enabled||!s.current(*this)||!s.nativeNormalEntry_||!s.inputReturnObserved_||
       s.directoryCopies_!=5||sourceInput04_!=s.input_||
       !sourceInput04NativeBorrowCurrent(s.input_.get()))return R::authority;
    if(!s.appStartRegistrationReturned_||!s.appStartNativeMountReturned_||!s.appStartMaterialNamesReturned_||
       !s.appStartNativeProgramsReturned_||!s.appStartDefaultTemplateReturned_||
       !s.appStart_||!s.appStartDomain_||s.appStartDomain_!=standardMaterialDomainOwner_||
       !s.appStartResolver_||s.appStartResolver_!=s.appStartDomain_->resolver_||
       s.appStartResolver_->registeredAppStart17CAB8()!=s.appStart_)return R::authority;
    auto& domain=*s.appStartDomain_;
    if(s.phase_==P::appStartScreenTemplateBlocked)return R::appStartScreenTemplateBlocked;
    if(sourceVideoProduction_.attempted_||domain.application_||domain.prepared())return R::phase;
    if(!s.appStartProvider_||domain.archiveProvider_!=s.appStartProvider_||
       domain.serviceAppStart_!=s.appStart_||s.appStartProvider_->mountCount()!=1||
       s.appStartProvider_->sealed()||s.appStartProvider_->revision()!=s.appStartProviderRevision_||
       !domain.sourceAppStartMaterialNamesCurrent(s.appStartMaterials_,s.appStart_)||
       !s.appStartPrograms_||domain.programs_!=s.appStartPrograms_||!s.appStartTemplateApplication_||
       s.appStartTemplateApplication_!=materialApplication)return R::authority;
    if(s.phase_==P::appStartScreenTemplateReturned){
        //The DefaultMT-only observation no longer applies after ScreenMT.
        //Repeat calls only validate the same actual second-call boundary.
        return s.pc_==0x17cb04&&s.appStartScreenTemplateCallIssued_&&s.appStartScreenTemplateReturned_&&
            s.appStartTemplateApplication_->sourceScreen17CAFCCurrent(s.appStart_,s.appStartResolver_,s.appStartPrograms_)&&
            s.appStartResolver_->sourceScreenTemplate17CAFCClosed(s.appStart_)?
                R::appStartScreenTemplateReturned:R::authority;
    }
    if(s.phase_!=P::appStartDefaultTemplateReturned||s.pc_!=0x17caf0||s.appStartScreenTemplateCallIssued_||
       !s.appStartTemplateApplication_->sourceDefault17CAE8Current(s.appStart_,s.appStartResolver_,s.appStartPrograms_)||
       !s.appStartResolver_->sourceDefaultTemplate17CAE8Closed(s.appStart_))return R::phase;
    //The native projection of3833F0/443720 is the existing pair's ScreenMT.
    //No replacement pair, new program table, DefaultMT replay or remount.
    s.appStartScreenTemplateCallIssued_=true;s.phase_=P::appStartScreenTemplateBlocked;s.pc_=0x17cafc;
    rr::assets::OriginalUiMaterialDiagnostic50KG d;
    if(!s.appStartTemplateApplication_->prepareSourceScreen17CAFC(
        s.appStart_,s.appStartResolver_,s.appStartPrograms_,d)){
        s.lastMaterialPC_=d.sourcePC?d.sourcePC:d.document.sourcePC;
        if(s.lastMaterialPC_)s.pc_=s.lastMaterialPC_;
        //Terminal native failure keeps reached ScreenMT fields and service
        //history beside intact DefaultMT. No rollback or guest return0.
        return R::appStartScreenTemplateBlocked;
    }
    if(s.appStartTemplateApplication_!=materialApplication||domain.programs_!=s.appStartPrograms_||
       !s.appStartTemplateApplication_->sourceScreen17CAFCCurrent(s.appStart_,s.appStartResolver_,s.appStartPrograms_)||
       !s.appStartResolver_->sourceScreenTemplate17CAFCClosed(s.appStart_))return R::authority;
    s.appStartScreenTemplateReturned_=true;s.phase_=P::appStartScreenTemplateReturned;
    s.pc_=0x17cb04;s.lastMaterialPC_=0x17cb04;
    //Actual ScreenMT24A9A0 returned after close. Public templates/raw material
    //admission,127AD0 mappings,17C910 return and source Video remain pending.
    return R::appStartScreenTemplateReturned;
}

inline rr::game::OriginalApplicationStartup17C910Source50LA::Result
Application::continueSourceStartupInputMappings17CB04()const noexcept {
    using S=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=S::Result;using P=S::Phase;auto& s=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||
       !enabled||!s.current(*this)||!s.nativeNormalEntry_||!s.inputReturnObserved_||
       s.directoryCopies_!=5||sourceInput04_!=s.input_||
       !sourceInput04NativeBorrowCurrent(s.input_.get()))return R::authority;
    if(!s.appStartRegistrationReturned_||!s.appStartNativeMountReturned_||!s.appStartMaterialNamesReturned_||
       !s.appStartNativeProgramsReturned_||!s.appStartDefaultTemplateReturned_||!s.appStartScreenTemplateReturned_||
       !s.appStart_||!s.appStartDomain_||s.appStartDomain_!=standardMaterialDomainOwner_||
       !s.appStartResolver_||s.appStartResolver_!=s.appStartDomain_->resolver_||
       s.appStartResolver_->registeredAppStart17CAB8()!=s.appStart_)return R::authority;
    auto& domain=*s.appStartDomain_;
    if(sourceVideoProduction_.attempted_||domain.application_||domain.prepared())return R::phase;
    if(!s.appStartProvider_||domain.archiveProvider_!=s.appStartProvider_||
       domain.serviceAppStart_!=s.appStart_||s.appStartProvider_->mountCount()!=1||
       s.appStartProvider_->sealed()||s.appStartProvider_->revision()!=s.appStartProviderRevision_||
       !domain.sourceAppStartMaterialNamesCurrent(s.appStartMaterials_,s.appStart_)||
       !s.appStartPrograms_||domain.programs_!=s.appStartPrograms_||!s.appStartTemplateApplication_||
       s.appStartTemplateApplication_!=materialApplication||
       !s.appStartTemplateApplication_->sourceScreen17CAFCCurrent(s.appStart_,s.appStartResolver_,s.appStartPrograms_)||
       !s.appStartResolver_->sourceScreenTemplate17CAFCClosed(s.appStart_))return R::authority;
    if(s.phase_==P::inputMappingsBlocked)return R::inputMappingsBlocked;
    if(s.phase_==P::inputMappingsReturned){
        //Read-only repeat; current borrows alone do not prove cold history.
        return s.pc_==0x17cb9c&&s.inputMappingsAttempted_&&s.inputMappingsReturned_&&
            s.inputMappingCallsIssued_==2&&s.inputMappingCallsReturned_==2&&s.inputMappingWordsWritten_==32&&
            s.input_->startupMappings17CB9CCurrent(*this)?R::inputMappingsReturned:R::authority;
    }
    if(s.phase_!=P::appStartScreenTemplateReturned||s.pc_!=0x17cb04||s.inputMappingsAttempted_||
       !s.input_->beforeStartupMappings17CB04(*this))return R::phase;
    s.inputMappingsAttempted_=true;s.phase_=P::inputMappingsBlocked;
    const bool reached=s.input_->applyStartupMappings17CB84(*this);
    //Copy actual child effects for observation, not caller-supplied receipts.
    s.inputMappingCallsIssued_=s.input_->startupMappingCallsIssued_;
    s.inputMappingCallsReturned_=s.input_->startupMappingCallsReturned_;
    s.inputMappingWordsWritten_=s.input_->startupMappingWordsWritten_;
    s.lastInputPC_=s.input_->startupMappingsPC_;
    if(s.lastInputPC_)s.pc_=s.lastInputPC_;
    if(!reached)return R::inputMappingsBlocked;
    if(!s.input_->startupMappings17CB9CCurrent(*this))return R::authority;
    s.inputMappingsReturned_=true;s.phase_=P::inputMappingsReturned;s.pc_=0x17cb9c;
    //Only two127AD0 calls and their loop tail completed. No17C910 return1,
    //template/material admission, new Player grant, input sample or Video work.
    return R::inputMappingsReturned;
}

inline rr::game::OriginalApplicationStartup17C910Source50LA::Result
Application::captureNativeStartupBodyResult17CB9C()const noexcept {
    using S=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=S::Result;using P=S::Phase;auto& s=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||
       !enabled||!s.current(*this))return R::authority;
    //Check the terminal body phase BEFORE invoking the existing continuation.
    //Its inputMappingsReturned branch only observes; it cannot perform stores
    //or advance earlier work from this exact phase on the same owning thread.
    if(s.phase_!=P::inputMappingsReturned||s.pc_!=0x17cb9c||!s.inputMappingsReturned_)return R::phase;
    const auto proof=continueSourceStartupInputMappings17CB04();
    if(proof!=R::inputMappingsReturned)return proof;
    if(s.nativeBodyResultKnown_){
        if(s.nativeBodyResult_!=1)return R::authority;
        return s.nativeAbiReturnConsumed_?R::nativeBodyResultConsumed:R::nativeBodyResultCallerPending;
    }
    if(s.nativeAbiReturnConsumed_)return R::authority;
    //17CBA8 supplies the normal body result1. This is an explicit native
    //capture after every owned body effect, NOT execution of17CB9C..17CBB0.
    //No guest ra/s1/s0/SP is restored here. An optional bound typed native
    //caller must consume this value separately; capture never returns to it.
    s.nativeBodyResult_=1;s.nativeBodyResultKnown_=true;
    //Keep phase and sourcePC17CB9C unchanged. In particular do not invent
    //23A0E4, consume its result branch, call237640, or publish435360/readiness.
    return R::nativeBodyResultCallerPending;
}

inline rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result
Application::enterNativeStartupCall23A0DC()const noexcept {
    using C=rr::game::OriginalApplicationStartupCall23A0DCNative50LA;
    using B=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=C::Result;auto& call=nativeStartupCall23A0DC_;auto& body=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!enabled||
       call.thread_!=std::this_thread::get_id()||body.thread_!=std::this_thread::get_id()||
       call.nativeClosed_||body.nativeClosed_)return R::authority;
    //Issue only a fresh call. A bare entry, partial body, captured result or
    //closed/cancelled call cannot be retroactively assigned this caller.
    if(call.phase_!=C::Phase::absent||call.application_||call.body_||call.bodyEntryReached_||
       call.nativeReturnKnown_||call.nativeContinuationPC_||body.phase_!=B::Phase::absent||
       body.application_||body.pc_||body.input_||body.appStart_||body.nativeCaller_||
       body.nativeBodyResultKnown_||body.nativeAbiReturnConsumed_)return R::phase;
    call.application_=this;call.body_=&body;call.phase_=C::Phase::issuing;
    body.nativeCaller_=&call;
    //Actual private invocation, not a supplied receipt/callback/guest address.
    //The entry itself enforces every existing downstream cold-history check.
    const auto entered=enterSourceStartup17C910();
    if(entered!=B::Result::argumentsUnowned||body.application_!=this||
       body.phase_!=B::Phase::argumentsPending||body.pc_!=0x17c924){
        call.phase_=C::Phase::entryBlocked;return R::entryBlocked;
    }
    call.bodyEntryReached_=true;call.phase_=C::Phase::bodyPending;return R::bodyPending;
    //The existing explicit normal-native policy and ordered continuations
    //still perform the body. No guest argc or earlier outer/MMIO work is done.
}

inline rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result
Application::returnNativeStartupCall23A0E4()const noexcept {
    using C=rr::game::OriginalApplicationStartupCall23A0DCNative50LA;
    using B=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=C::Result;auto& call=nativeStartupCall23A0DC_;auto& body=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!enabled||
       !call.current(*this)||call.body_!=&body||body.nativeCaller_!=&call)
        return R::authority;
    if(call.phase_==C::Phase::entryBlocked)return R::entryBlocked;
    if(!body.current(*this))return R::authority;
    if(!call.bodyEntryReached_||(call.phase_!=C::Phase::bodyPending&&call.phase_!=C::Phase::returned))return R::phase;
    //A return check cannot execute unfinished input/material/template work.
    if(body.phase_!=B::Phase::inputMappingsReturned||body.pc_!=0x17cb9c||!body.inputMappingsReturned_)
        return call.phase_==C::Phase::bodyPending?R::bodyPending:R::authority;
    const auto captured=captureNativeStartupBodyResult17CB9C();
    if(call.phase_==C::Phase::returned){
        //Capture revalidates the same entire body before accepting history.
        return captured==B::Result::nativeBodyResultConsumed&&body.nativeAbiReturnConsumed_&&
            call.nativeReturnKnown_&&call.nativeReturnValue_==1&&call.nativeContinuationPC_==0x23a0e4?
                R::nativeReturned:R::authority;
    }
    if(captured!=B::Result::nativeBodyResultCallerPending||!body.nativeBodyResultKnown_||
       body.nativeBodyResult_!=1||body.nativeAbiReturnConsumed_)return R::authority;
    //Explicit native ABI: this issued call consumes this exact body value.
    //No guest stack/register restores or global/vtable reads are synthesized.
    call.nativeReturnValue_=body.nativeBodyResult_;call.nativeReturnKnown_=true;
    body.nativeAbiReturnConsumed_=true;call.nativeContinuationPC_=0x23a0e4;call.phase_=C::Phase::returned;
    //Body sourcePC stays17CB9C. Stop BEFORE23A0E4's result branch,304398,
    //the caller's later error-buffer preparation,237640 or435360 publication.
    return R::nativeReturned;
}

inline rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result
Application::continueNativeStartupInitialize23A0E4()const noexcept {
    using C=rr::game::OriginalApplicationStartupCall23A0DCNative50LA;
    using B=rr::game::OriginalApplicationStartup17C910Source50LA;
    using R=C::Result;auto& call=nativeStartupCall23A0DC_;auto& body=sourceStartup17C910_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!enabled||
       !call.current(*this)||call.body_!=&body||body.nativeCaller_!=&call||!body.current(*this))return R::authority;
    if(call.phase_==C::Phase::initializationBlocked)return R::initializationBlocked;
    if(call.phase_!=C::Phase::returned&&call.phase_!=C::Phase::videoAllocationPending)return R::phase;
    //Full same-owner body proof, not a copied return value. At its terminal
    //phase capture only observes; it cannot perform unfinished startup work.
    if(captureNativeStartupBodyResult17CB9C()!=B::Result::nativeBodyResultConsumed||
       !call.bodyEntryReached_||!call.nativeReturnKnown_||call.nativeReturnValue_!=1||
       !body.nativeAbiReturnConsumed_||sourceVideoProduction_.attempted_)return R::authority;
    if(call.phase_==C::Phase::videoAllocationPending)
        return call.initializationAllocationPendingCurrent(*this)?R::videoAllocationPending:R::authority;
    if(call.nativeContinuationPC_!=0x23a0e4||call.nonzeroBranchConsumed_||call.initialization_.issued||
       call.initialization_.application||call.initialization_.caller||call.initialization_.error)return R::phase;
    for(const auto known:call.errorBuffer_.known)if(known)return R::phase;
    //23A0E4's actual consumed1 takes the nonzero branch; no304398(0) call.
    call.nonzeroBranchConsumed_=true;call.nativeContinuationPC_=0x23a0f4;
    auto& init=call.initialization_;
    init.application=this;init.caller=&call;init.error=&call.errorBuffer_;init.issued=true;
    call.phase_=C::Phase::initializationIssuing;call.nativeContinuationPC_=0x23a100;
    //23A0FC supplies this retained native buffer.23A104 clears ONLY byte0
    //before entering the callee; every other byte remains source-unknown.
    call.errorBuffer_.data[0]=0;call.errorBuffer_.known[0]=255;
    const auto entered=enterNativeInitialize237640();
    if(entered!=R::videoAllocationPending){call.phase_=C::Phase::initializationBlocked;return R::initializationBlocked;}
    call.phase_=C::Phase::videoAllocationPending;
    return call.initializationAllocationPendingCurrent(*this)?R::videoAllocationPending:R::authority;
}

inline rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result
Application::enterNativeInitialize237640()const noexcept {
    using C=rr::game::OriginalApplicationStartupCall23A0DCNative50LA;
    using R=C::Result;auto& call=nativeStartupCall23A0DC_;auto& init=call.initialization_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!enabled||
       !call.current(*this)||call.body_!=&sourceStartup17C910_||
       sourceStartup17C910_.nativeCaller_!=&call||!sourceStartup17C910_.current(*this)||
       !sourceStartup17C910_.nativeAbiReturnConsumed_||sourceVideoProduction_.attempted_)return R::authority;
    if(call.phase_!=C::Phase::initializationIssuing||!call.nonzeroBranchConsumed_||
       call.nativeContinuationPC_!=0x23a100||!init.issued||init.entered||
       init.application!=this||init.caller!=&call||init.error!=&call.errorBuffer_||
       call.errorBuffer_.known[0]!=255||call.errorBuffer_.data[0]!=0)return R::phase;
    //Explicit native ABI owns237640's receiver and error-buffer lifetime.
    //No guest s0/s1/SP save image or numeric source pointer is manufactured.
    init.entered=true;init.pc=0x237654;
    init.prepareResult=C::prepare237A40();init.prepareReturned=true;init.pc=0x23765c;
    //237654's result is ignored. Native Application binds the verified
    //3CE480+9C target237950, whose actual leaf body returns1 and ignores a1.
    init.pc=0x237668;init.policyResult=C::policy237950(0);init.policyReturned=true;init.pc=0x237670;
    //237674's delay argument is written regardless of the branch outcome.
    init.allocationArgumentBytes=0x510;init.allocationArgumentKnown=true;
    if(!init.policyResult){init.pc=0x237678;return R::initializationBlocked;}
    //237670 branches DIRECTLY to2376A0. Its delay237674 supplies510;
    //successful execution skips23769C. Do not enter allocator2E8D90 yet.
    init.pc=0x2376a0;
    //Neither literal stub is host mode readiness.237640 has NOT returned;
    //Video construction/publication,25A3E0 and13FDA0 remain pending.
    return R::videoAllocationPending;
}

inline bool Application::nativeStartupVideoInitializationOwnerCurrent()const noexcept {
    using C=rr::game::OriginalApplicationStartupCall23A0DCNative50LA;
    using B=rr::game::OriginalApplicationStartup17C910Source50LA;
    const auto& call=nativeStartupCall23A0DC_;const auto& s=sourceStartup17C910_;const auto& video=sourceVideoProduction_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!enabled||
       !call.current(*this)||!s.current(*this)||video.thread_!=std::this_thread::get_id()||video.nativeClosed_)
        return false;
    //This is NOT a relaxed cold-history check. Only this exact already-issued
    //Video occurrence, reciprocally bound before allocation, is admitted.
    if((call.phase_!=C::Phase::videoConstructionBlocked&&call.phase_!=C::Phase::modeTableCallPending&&
        call.phase_!=C::Phase::modeTableBlocked&&call.phase_!=C::Phase::descendantAllocationPending&&
        call.phase_!=C::Phase::descendantsBlocked&&call.phase_!=C::Phase::callbacksPending&&
        call.phase_!=C::Phase::intcRegistrationBlocked&&call.phase_!=C::Phase::intcSyscallPending&&
        call.phase_!=C::Phase::intcCustodyBlocked&&call.phase_!=C::Phase::intcCustodyPending)||
       call.body_!=&s||s.nativeCaller_!=&call||!call.bodyEntryReached_||!call.nativeReturnKnown_||
       call.nativeReturnValue_!=1||call.nativeContinuationPC_!=0x23a100||!call.nonzeroBranchConsumed_||
       !s.nativeBodyResultKnown_||s.nativeBodyResult_!=1||!s.nativeAbiReturnConsumed_||
       call.initialization_.application!=this||call.initialization_.caller!=&call||
       call.initialization_.error!=&call.errorBuffer_||!call.initialization_.issued||!call.initialization_.entered||
       !call.initialization_.prepareReturned||call.initialization_.prepareResult!=1||
       !call.initialization_.policyReturned||call.initialization_.policyResult!=1||
       !call.initialization_.allocationArgumentKnown||call.initialization_.allocationArgumentBytes!=0x510||
       call.initialization_.video!=&video||!call.initialization_.videoCallIssued||
       video.nativeInitializerCaller_!=&call||video.application_!=this||!video.attempted_||
       call.initialization_.pc!=video.pc_||call.errorBuffer_.known[0]!=255||call.errorBuffer_.data[0]!=0)return false;
    for(std::size_t i=1;i<call.errorBuffer_.known.size();++i)if(call.errorBuffer_.known[i])return false;
    if(s.phase_!=B::Phase::inputMappingsReturned||s.pc_!=0x17cb9c||!s.nativeNormalEntry_||
       !s.inputReturnObserved_||s.directoryCopies_!=5||!s.input_||sourceInput04_!=s.input_||
       !sourceInput04NativeBorrowCurrent(s.input_.get())||!s.inputMappingsAttempted_||!s.inputMappingsReturned_||
       s.inputMappingCallsIssued_!=2||s.inputMappingCallsReturned_!=2||s.inputMappingWordsWritten_!=32||
       !s.input_->startupMappings17CB9CCurrent(*this)||!s.appStartRegistrationReturned_||
       !s.appStartNativeMountReturned_||!s.appStartMaterialNamesReturned_||!s.appStartNativeProgramsReturned_||
       !s.appStartDefaultTemplateReturned_||!s.appStartScreenTemplateReturned_||!s.appStart_||
       !s.appStartDomain_||s.appStartDomain_!=standardMaterialDomainOwner_||!s.appStartResolver_||
       s.appStartResolver_!=s.appStartDomain_->resolver_||s.appStartResolver_->registeredAppStart17CAB8()!=s.appStart_)
        return false;
    const auto& domain=*s.appStartDomain_;
    return !domain.application_&&!domain.prepared()&&s.appStartProvider_&&domain.archiveProvider_==s.appStartProvider_&&
        domain.serviceAppStart_==s.appStart_&&s.appStartProvider_->mountCount()==1&&!s.appStartProvider_->sealed()&&
        s.appStartProvider_->revision()==s.appStartProviderRevision_&&
        domain.sourceAppStartMaterialNamesCurrent(s.appStartMaterials_,s.appStart_)&&
        s.appStartPrograms_&&domain.programs_==s.appStartPrograms_&&s.appStartTemplateApplication_&&
        s.appStartTemplateApplication_==materialApplication&&
        s.appStartTemplateApplication_->sourceScreen17CAFCCurrent(s.appStart_,s.appStartResolver_,s.appStartPrograms_)&&
        s.appStartResolver_->sourceScreenTemplate17CAFCClosed(s.appStart_);
}

inline rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result
Application::continueNativeStartupVideo2376A0()const noexcept {
    using C=rr::game::OriginalApplicationStartupCall23A0DCNative50LA;
    using V=rr::game::OriginalApplicationVideoSource50LA;
    using R=C::Result;auto& call=nativeStartupCall23A0DC_;auto& init=call.initialization_;auto& video=sourceVideoProduction_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!enabled||
       !call.current(*this)||video.thread_!=std::this_thread::get_id()||video.nativeClosed_)return R::authority;
    if(call.phase_==C::Phase::videoAllocationPending){
        //Use the unchanged full cold proof BEFORE issuing the first allocation.
        if(continueNativeStartupInitialize23A0E4()!=R::videoAllocationPending||
           !call.initializationAllocationPendingCurrent(*this))return R::authority;
        if(video.attempted_||video.application_||video.nativeInitializerCaller_||video.allocation_||
           video.global443A60_||video.published443A60_||video.pc_||video.allocatorReturnUnowned_||
           init.video||init.videoCallIssued||init.videoPublicationObserved)return R::phase;
        init.video=&video;init.videoCallIssued=true;video.nativeInitializerCaller_=&call;
        call.phase_=C::Phase::videoConstructionIssuing;
        const auto produced=constructSourceVideo2376A0(); // Actual same-owner invocation.
        init.pc=video.pc_;
        if(produced==V::Result::publishedConstructorOnly){
            init.videoPublicationObserved=true;call.phase_=C::Phase::modeTableCallPending;
        }else{
            call.phase_=C::Phase::videoConstructionBlocked;
            //Retain allocation and every reached constructor store. Native
            //failure does not own a source handler, exception, NULL return or retry.
        }
    }
    if(call.phase_!=C::Phase::videoConstructionBlocked&&call.phase_!=C::Phase::modeTableCallPending)return R::phase;
    if(!nativeStartupVideoInitializationOwnerCurrent()||video.modeTableEntered_||video.modeDescendantsEntered_||
       video.registrationPending_||init.modeTableCallIssued||init.modeTablePrefixObserved||
       init.descendantsContinuationIssued||init.descendantsPrefixObserved)return R::authority;
    if(call.phase_==C::Phase::videoConstructionBlocked){
        if(init.videoPublicationObserved||!video.allocatorReturnUnowned_||video.published443A60_||
           video.global443A60_||video.pc_!=video.blockedAllocationCallPC_)return R::authority;
        const bool outer=video.pc_==0x2376a0&&!video.allocation_;
        const bool base=video.pc_==0x2ae308&&video.allocation_&&!video.allocation_->preFlipSlots1C&&
            !video.allocation_->preFlipPointerPublished&&!video.allocation_->baseReturned&&!video.allocation_->constructorReturned;
        return outer||base?R::videoConstructionBlocked:R::authority;
    }
    return init.videoPublicationObserved&&!video.allocatorReturnUnowned_&&video.pc_==0x2376c0&&
        video.published443A60_&&video.allocation_&&video.global443A60_==video.allocation_.get()&&
        video.allocation_->preFlipPointerPublished&&video.allocation_->preFlipSlots1C&&
        video.allocation_->baseReturned&&video.allocation_->constructorReturned?
            R::modeTableCallPending:R::authority;
    //No25A3E0 call,237640 return, platform/IRQ/mode state or public admission.
}

inline rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result
Application::continueNativeStartupModeTable2376C0()const noexcept {
    using C=rr::game::OriginalApplicationStartupCall23A0DCNative50LA;
    using V=rr::game::OriginalApplicationVideoSource50LA;using R=C::Result;
    auto& call=nativeStartupCall23A0DC_;auto& init=call.initialization_;auto& video=sourceVideoProduction_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!enabled||
       !call.current(*this)||video.thread_!=std::this_thread::get_id()||video.nativeClosed_)return R::authority;
    if(call.phase_==C::Phase::modeTableCallPending){
        if(continueNativeStartupVideo2376A0()!=R::modeTableCallPending)return R::authority;
        if(init.modeTableCallIssued||init.modeTablePrefixObserved||init.descendantsContinuationIssued||
           init.descendantsPrefixObserved)return R::phase;
        //2376C0 passes the exact just-published Video in delay2376C4. Bind
        //this call before issue; never adopt a standalone table already built.
        init.modeTableCallIssued=true;call.phase_=C::Phase::modeTableIssuing;
        const auto reached=continueSourceVideoModeTable25A3E0();
        init.pc=video.pc_;
        if(reached==V::ModeTableResult::descendantAllocationPending){
            init.modeTablePrefixObserved=true;call.phase_=C::Phase::descendantAllocationPending;
        }else call.phase_=C::Phase::modeTableBlocked;
    }
    if(call.phase_!=C::Phase::modeTableBlocked&&call.phase_!=C::Phase::descendantAllocationPending)return R::phase;
    if(!nativeStartupVideoInitializationOwnerCurrent()||!init.videoPublicationObserved||!init.modeTableCallIssued||
       init.descendantsContinuationIssued||init.descendantsPrefixObserved||video.allocatorReturnUnowned_||
       video.blockedAllocationCallPC_||!video.published443A60_||!video.allocation_||
       video.global443A60_!=video.allocation_.get()||!video.modeTableEntered_||video.modeDescendantsEntered_||
       video.registrationPending_)return R::authority;
    const auto& storage=*video.allocation_;
    if(!storage.preFlipSlots1C||!storage.preFlipPointerPublished||!storage.baseReturned||!storage.constructorReturned||
       !storage.modeTablePointerPublished||storage.source.readWord(0x30)!=10||storage.source.readWord(0x34)!=4||
       storage.buffer88AllocationAttempted||storage.buffer88||storage.buffer88Published)return R::authority;
    for(std::size_t i=0;i<4;++i)if(storage.packetAllocationAttempted[i]||storage.packets[i]||
       storage.packetConstructorReturned[i]||storage.packetPublished[i])return R::authority;
    if(call.phase_==C::Phase::modeTableBlocked){
        //Direct305700's nullable native adaptation published+38, but never
        //wrote through missing backing or manufactured50 initialized words.
        return !init.modeTablePrefixObserved&&video.pc_==0x25a428&&!storage.modeTable38&&
            storage.modeTableWordsWritten==0?R::modeTableBlocked:R::authority;
    }
    return init.modeTablePrefixObserved&&video.pc_==0x25a5ec&&storage.modeTable38&&
        storage.modeTableWordsWritten==50?R::descendantAllocationPending:R::authority;
    //25A3E0 itself has not returned. No mode choice, callback or237640 return.
}

inline rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result
Application::continueNativeStartupVideoDescendants25A5EC()const noexcept {
    using C=rr::game::OriginalApplicationStartupCall23A0DCNative50LA;
    using V=rr::game::OriginalApplicationVideoSource50LA;using R=C::Result;
    auto& call=nativeStartupCall23A0DC_;auto& init=call.initialization_;auto& video=sourceVideoProduction_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!enabled||
       !call.current(*this)||video.thread_!=std::this_thread::get_id()||video.nativeClosed_)return R::authority;
    if(call.phase_==C::Phase::descendantAllocationPending){
        if(continueNativeStartupModeTable2376C0()!=R::descendantAllocationPending)return R::authority;
        if(init.descendantsContinuationIssued||init.descendantsPrefixObserved)return R::phase;
        init.descendantsContinuationIssued=true;call.phase_=C::Phase::descendantsIssuing;
        const auto reached=continueSourceVideoDescendants25A5EC();
        init.pc=video.pc_;
        if(reached==V::DescendantResult::callbacksPending){
            init.descendantsPrefixObserved=true;call.phase_=C::Phase::callbacksPending;
        }else call.phase_=C::Phase::descendantsBlocked;
    }
    if(call.phase_!=C::Phase::descendantsBlocked&&call.phase_!=C::Phase::callbacksPending)return R::phase;
    if(!nativeStartupVideoInitializationOwnerCurrent()||!init.videoPublicationObserved||!init.modeTableCallIssued||
       !init.modeTablePrefixObserved||!init.descendantsContinuationIssued||!video.published443A60_||!video.allocation_||
       video.global443A60_!=video.allocation_.get()||!video.modeTableEntered_||!video.modeDescendantsEntered_||
       video.registrationPending_||init.intcRegistrationCallIssued||init.intcSyscallBoundaryReached||
       init.intcCustodyAttempted||init.intcCustodyOutcome!=C::IntcCustodyOutcome::absent||
       init.nativeIntcCustodyGeneration)return R::authority;
    const auto& storage=*video.allocation_;
    if(!storage.preFlipSlots1C||!storage.preFlipPointerPublished||!storage.baseReturned||!storage.constructorReturned||
       !storage.modeTablePointerPublished||!storage.modeTable38||storage.modeTableWordsWritten!=50||
       !storage.buffer88AllocationAttempted||storage.source.readWord(0x10)!=0x00400000)return R::authority;
    if(!storage.buffer88||!storage.buffer88->constructorReturned){
        //Neither failure permits+88 publication or any packet allocation.
        if(call.phase_!=C::Phase::descendantsBlocked||init.descendantsPrefixObserved||storage.buffer88Published)
            return R::authority;
        for(std::size_t i=0;i<4;++i)if(storage.packetAllocationAttempted[i]||storage.packets[i]||
           storage.packetConstructorReturned[i]||storage.packetPublished[i])return R::authority;
        if(!storage.buffer88)return video.allocatorReturnUnowned_&&video.pc_==0x25a5ec&&
            video.blockedAllocationCallPC_==0x25a5ec?R::descendantsBlocked:R::authority;
        const auto& buffer=*storage.buffer88;
        return !video.allocatorReturnUnowned_&&!video.blockedAllocationCallPC_&&video.pc_==0x259348&&
            buffer.backingAllocationAttempted&&!buffer.backing&&buffer.pointer00Published&&buffer.uncachedAlias00&&
            buffer.invalidNullAlias00&&!buffer.backingZeroed&&!buffer.cursor04Aliases00?
                R::descendantsBlocked:R::authority;
    }
    const auto& buffer=*storage.buffer88;
    if(!storage.buffer88Published||!buffer.backingAllocationAttempted||!buffer.backing||!buffer.pointer00Published||
       !buffer.uncachedAlias00||buffer.invalidNullAlias00||!buffer.backingZeroed||!buffer.cursor04Aliases00||
       buffer.source.readWord(8)!=24)return R::authority;
    if(call.phase_==C::Phase::descendantsBlocked){
        if(init.descendantsPrefixObserved||!video.allocatorReturnUnowned_||
           video.pc_!=video.blockedAllocationCallPC_)return R::authority;
        static constexpr std::uint32_t allocationPC[4]={0x25a610,0x25a62c,0x25a648,0x25a664};
        std::size_t failed=0;while(failed<4&&allocationPC[failed]!=video.pc_)++failed;
        if(failed==4)return R::authority;
        for(std::size_t i=0;i<4;++i){
            //The failed call's delay slot published the PREVIOUS result.
            //The failed object and every later object remain unpublished.
            if(i<failed){
                if(!storage.packetAllocationAttempted[i]||!storage.packets[i]||
                   !storage.packetConstructorReturned[i]||!storage.packetPublished[i])return R::authority;
            }else if(storage.packetAllocationAttempted[i]!=(i==failed)||storage.packets[i]||
                     storage.packetConstructorReturned[i]||storage.packetPublished[i])return R::authority;
        }
        return R::descendantsBlocked;
    }
    if(!init.descendantsPrefixObserved||video.allocatorReturnUnowned_||video.blockedAllocationCallPC_||
       video.pc_!=0x25a68c)return R::authority;
    for(std::size_t i=0;i<4;++i)if(!storage.packetAllocationAttempted[i]||!storage.packets[i]||
       !storage.packetConstructorReturned[i]||!storage.packetPublished[i])return R::authority;
    return R::callbacksPending;
    //No2F5880 invocation, IRQ, MMIO,25A3E0/237640 return or readiness grant.
}

inline bool Application::nativeStartupVideoIntcBoundaryCurrent()const noexcept {
    const auto& call=nativeStartupCall23A0DC_;const auto& init=call.initialization_;const auto& video=sourceVideoProduction_;
    if(!nativeStartupVideoInitializationOwnerCurrent()||!init.videoPublicationObserved||!init.modeTableCallIssued||
       !init.modeTablePrefixObserved||!init.descendantsContinuationIssued||!init.descendantsPrefixObserved||
       !init.intcRegistrationCallIssued||!init.intcSyscallBoundaryReached||video.pc_!=0x2f5884||
       video.allocatorReturnUnowned_||video.blockedAllocationCallPC_||!video.modeTableEntered_||
       !video.modeDescendantsEntered_||!video.published443A60_||!video.allocation_||
       video.global443A60_!=video.allocation_.get()||!video.registrationPending_||video.nativeIntcCustodyReleased_)
        return false;
    const auto& request=*video.registrationPending_;
    if(request.callPC!=0x25a68c||request.syscallPC!=0x2f5884||request.returnPC!=0x25a694||
       request.syscallNumberV1!=0x10||request.sourceA0!=0||request.sourceA1!=0x259b80||request.sourceA2!=-1)
        return false;
    const auto& storage=*video.allocation_;
    if(!storage.preFlipSlots1C||!storage.preFlipPointerPublished||!storage.baseReturned||!storage.constructorReturned||
       !storage.modeTablePointerPublished||!storage.modeTable38||storage.modeTableWordsWritten!=50||
       !storage.buffer88AllocationAttempted||!storage.buffer88||!storage.buffer88Published||
       storage.source.readWord(0x10)!=0x00400000)return false;
    const auto& buffer=*storage.buffer88;
    if(!buffer.constructorReturned||!buffer.backingAllocationAttempted||!buffer.backing||!buffer.pointer00Published||
       !buffer.uncachedAlias00||buffer.invalidNullAlias00||!buffer.backingZeroed||!buffer.cursor04Aliases00||
       buffer.source.readWord(8)!=24)return false;
    for(std::size_t i=0;i<4;++i)if(!storage.packetAllocationAttempted[i]||!storage.packets[i]||
       !storage.packetConstructorReturned[i]||!storage.packetPublished[i])return false;
    return true;
    //Readonly actual call/CPU-owner proof. No original syscall outcome or IRQ.
}

inline rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result
Application::continueNativeStartupVideoIntc25A68C()const noexcept {
    using C=rr::game::OriginalApplicationStartupCall23A0DCNative50LA;
    using V=rr::game::OriginalApplicationVideoSource50LA;using R=C::Result;
    auto& call=nativeStartupCall23A0DC_;auto& init=call.initialization_;auto& video=sourceVideoProduction_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!enabled||
       !call.current(*this)||video.thread_!=std::this_thread::get_id()||video.nativeClosed_)return R::authority;
    if(call.phase_==C::Phase::callbacksPending){
        //Only this already-issued initializer's exact completed CPU prefix
        //may enter. A standalone pending request/custody cannot be adopted.
        if(continueNativeStartupVideoDescendants25A5EC()!=R::callbacksPending)return R::authority;
        if(init.intcRegistrationCallIssued||init.intcSyscallBoundaryReached||init.intcCustodyAttempted||
           init.intcCustodyOutcome!=C::IntcCustodyOutcome::absent||init.nativeIntcCustodyGeneration||
           video.registrationPending_||video.nativeIntcCustodyClaimed_||video.nativeIntcCustodyReleased_||
           video.nativeIntcCustodyGeneration_)return R::phase;
        init.intcRegistrationCallIssued=true;call.phase_=C::Phase::intcRegistrationIssuing;
        const auto reached=enterSourceVideoIntcRegistration25A68C();
        init.pc=video.pc_;
        if(reached==V::RegistrationResult::syscallPending){
            init.intcSyscallBoundaryReached=true;call.phase_=C::Phase::intcSyscallPending;
        }else call.phase_=C::Phase::intcRegistrationBlocked;
    }
    if(call.phase_!=C::Phase::intcRegistrationBlocked&&call.phase_!=C::Phase::intcSyscallPending)return R::phase;
    if(!nativeStartupVideoInitializationOwnerCurrent()||!init.intcRegistrationCallIssued||init.intcCustodyAttempted||
       init.intcCustodyOutcome!=C::IntcCustodyOutcome::absent||init.nativeIntcCustodyGeneration||
       video.nativeIntcCustodyClaimed_||video.nativeIntcCustodyReleased_||video.nativeIntcCustodyGeneration_)
        return R::authority;
    if(call.phase_==C::Phase::intcRegistrationBlocked)return !init.intcSyscallBoundaryReached&&
        video.pc_==0x25a68c&&!video.registrationPending_?R::intcRegistrationBlocked:R::authority;
    return nativeStartupVideoIntcBoundaryCurrent()?R::intcSyscallPending:R::authority;
    //The stub's selector prelude is retained; SYSCALL2F5884 is UNEXECUTED.
    //No return/handler ID, enable, GS write, callback dispatch or25A3E0 return.
}

inline rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result
Application::holdNativeStartupVideoIntcCustody()const noexcept {
    using C=rr::game::OriginalApplicationStartupCall23A0DCNative50LA;
    using V=rr::game::OriginalApplicationVideoSource50LA;using R=C::Result;using O=C::IntcCustodyOutcome;
    auto& call=nativeStartupCall23A0DC_;auto& init=call.initialization_;auto& video=sourceVideoProduction_;
    if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this)||!enabled||
       !call.current(*this)||video.thread_!=std::this_thread::get_id()||video.nativeClosed_)return R::authority;
    if(call.phase_==C::Phase::intcSyscallPending){
        if(continueNativeStartupVideoIntc25A68C()!=R::intcSyscallPending)return R::authority;
        if(init.intcCustodyAttempted||init.intcCustodyOutcome!=O::absent||init.nativeIntcCustodyGeneration)
            return R::phase;
        init.intcCustodyAttempted=true;call.phase_=C::Phase::intcCustodyIssuing;
        const auto held=holdSourceVideoPendingIntcNativeCustody();
        //Capture the actual NATIVE custody outcome, not a kernel return/ID.
        switch(held){
        case V::NativeIntcCustodyResult::authority:init.intcCustodyOutcome=O::authority;break;
        case V::NativeIntcCustodyResult::phase:init.intcCustodyOutcome=O::phase;break;
        case V::NativeIntcCustodyResult::otherOwner:init.intcCustodyOutcome=O::otherOwner;break;
        case V::NativeIntcCustodyResult::exhausted:init.intcCustodyOutcome=O::exhausted;break;
        case V::NativeIntcCustodyResult::heldPendingSyscall:init.intcCustodyOutcome=O::held;break;
        }
        if(held==V::NativeIntcCustodyResult::heldPendingSyscall){
            init.nativeIntcCustodyGeneration=video.nativeIntcCustodyGeneration_;
            call.phase_=C::Phase::intcCustodyPending;
        }else call.phase_=C::Phase::intcCustodyBlocked;
    }
    if(call.phase_!=C::Phase::intcCustodyBlocked&&call.phase_!=C::Phase::intcCustodyPending)return R::phase;
    if(!nativeStartupVideoIntcBoundaryCurrent()||!init.intcCustodyAttempted)return R::authority;
    if(call.phase_==C::Phase::intcCustodyBlocked){
        //Read-only history of this failed native claim. A later owner release
        //does not replay it, clear another owner, or complete the source call.
        return init.intcCustodyOutcome!=O::absent&&init.intcCustodyOutcome!=O::held&&
            !init.nativeIntcCustodyGeneration&&!video.nativeIntcCustodyClaimed_&&
            !video.nativeIntcCustodyGeneration_?R::intcCustodyBlocked:R::authority;
    }
    return init.intcCustodyOutcome==O::held&&init.nativeIntcCustodyGeneration&&
        init.nativeIntcCustodyGeneration==video.nativeIntcCustodyGeneration_&&video.nativeIntcCustodyCurrent()?
            R::intcCustodyPending:R::authority;
    //No source progress: init.pc/video.pc remain2F5884, outer23A100 pending.
    //Native close releases only this exact slot; no Disable/Remove/unwind.
}
