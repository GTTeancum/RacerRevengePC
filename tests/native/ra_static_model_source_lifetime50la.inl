// Included inside SAME ra_static_model_cold50LA::Owner. No alternate model graph.
    bool admitSourceNative(std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        std::uint64_t generation)noexcept {
        if(sourceMode_||attempted_||closed_||application_||!parents_.empty()||!models_.empty()||
           !application||!application->application10()||!generation||application->attachmentLock423F58()||
           application->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return false;
        sourceMode_=true;sourceThread_=std::this_thread::get_id();
        application_=std::move(application);generation_=generation;
        //Native bookkeeping only, before271124 and any counted constructor.
        try{parents_.reserve(10);}catch(...){return false;}
        return true;
    }
    bool canCloseSourceNative()const noexcept {
        if(!sourceMode_||sourceThread_!=std::this_thread::get_id())return false;
        if(closed_)return parents_.empty()&&!sourceWorld3C4Borrower_;
        if(!application_||!generation_||application_->attachmentLock423F58()||constructed_||
           !models_.empty()||materials_||resolver_||visibility_||resourcesAttempted_||observersAttempted_||
           nativeCollisionForest_||parents_.size()>10)return false;
        if(sourceLedgerPrepared_&&(!sourceLedgerAttempted_||!sourceFirstTransferred_))return false;
        const auto wrapperCurrent=[](const StandardWrapper& w)noexcept {
            if(!w.sourceMode||w.model8||w.smoker0C||w.sourceVtable04!=0x3d26f0||
               w.dispatch!=StandardWrapper::Dispatch::standard3D26F0||
               (w.sourceTypeKnown&&w.type00!=1)||
               (w.sourceDescriptorPublished&&!w.sourceDescriptorAllocationReturned))return false;
            return !w.descriptor10||(w.descriptor10->sourceFields&&w.sourceTypeKnown&&w.sourceDescriptorPublished);
        };
        std::size_t counted=0;
        for(std::size_t z=0;z<parents_.size();++z)if(const auto& p=parents_[z]){
            const auto count=sourceLedgerPrepared_?sourceZoneCounts_[z]:467u;
            if(!p->sourceMode||!p->entries34.empty()||p->released||p->zone!=z+1||
               p->sourceEntriesKnown>p->count30||
               (p->sourceEntriesKnown&&!p->sourceEntries34)||
               (p->sourceEntries34&&!p->sourceArrayReturned)||
               (p->sourceArrayReturned&&p->count30!=count))return false;
            if(p->constructed){const auto& e=p->entity;
                if(e.reference00!=1||e.kind!=K::staticModels27B610||e.sessionGeneration!=generation_||
                   e.objectKey!=key(&e)||e.nativeFlags29||e.parent1C||e.previous10||e.next14||e.firstChild18||
                   e.mask20||e.descendants24||e.category28!=0xff||e.flags29!=3)return false;
                ++counted;
            }else if(p->entity.reference00)return false;
            if(p->sourceFirstWrapper){
                if(z||sourceFirstTransferred_||!wrapperCurrent(*p->sourceFirstWrapper))return false;
            }
            if(p->sourcePendingDescriptor&&!p->sourcePendingDescriptor->sourceFields)return false;
            if(sourceLedgerPrepared_){
                if(sourceWrapperLedger_[z].size()!=count||p->sourceFirstWrapper)return false;
                for(std::size_t i=0;i<count;++i)if(const auto& w=sourceWrapperLedger_[z][i]){
                    if(i>p->sourceEntriesKnown||!wrapperCurrent(*w))return false;
                }
            }
            for(std::size_t i=0;i<p->sourceEntriesKnown;++i){
                const auto* expected=sourceLedgerPrepared_?sourceWrapperLedger_[z][i].get():
                    (z==0&&i==0?p->sourceFirstWrapper.get():nullptr);
                if((!sourceLedgerPrepared_&&(z||i))||p->sourceEntries34[i]!=expected)return false;
            }
        }
        if(!sourceLedgerPrepared_)for(const auto& row:sourceWrapperLedger_)for(const auto& w:row)if(w)return false;
        return counted<=application_->objectCount423F60();
    }
    bool closeSourceNative()noexcept {
        if(!canCloseSourceNative()||sourceWorld3C4Borrower_)return false;
        if(closed_)return true;
        //Native ledger-only cleanup. No walk of unproduced raw slots and no
        //guest27B650 destructor receipt. All owned wrapper/descriptor payloads
        //precede the real Parent reference/count retirement.
        for(auto& row:sourceWrapperLedger_)row.clear();
        for(auto& p:parents_)if(p){
            p->sourcePendingDescriptor.reset();p->sourceFirstWrapper.reset();p->sourceEntries34.reset();
            if(p->constructed&&!p->released){
                --p->entity.reference00;--application_->objectCount423F60();p->released=true;
            }
        }
        parents_.clear();application_.reset();closed_=true;return true;
    }
