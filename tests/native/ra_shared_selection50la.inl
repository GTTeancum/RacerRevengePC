//Private typed source-list projection. No widened legacy index namespace,
//synthetic identity, reordering, deduplication or callback/collector invocation.
    enum class SharedSelectedKind {staticCandidate,pod694,otherRoster};
    struct SharedSelectedOwner {
        SharedSelectedKind kind=SharedSelectedKind::staticCandidate;
        std::uint32_t identity=0;
        std::size_t index=0; //Actual static ordinal OR actual row ordinal, tagged above.
    };
    struct SharedSelectedList {
        std::array<SharedSelectedOwner,storage::sourceSelectionCapacity> entries{};
        std::size_t count=0;std::uint64_t projection=0;
    };
    //Derived current-call maps only. Their serial authorizes reads solely
    //inside the actual matching producer invocation; no saved frame history.
    SharedSelectedList sharedMergedSelection_,sharedProbeSelection_;
    bool resolveSharedSelected(const SharedOwnerLease& lease,const std::uint32_t* source,
        std::int32_t count,SharedSelectedList& destination)const noexcept {
        if(!sharedLeaseCurrent(lease)||count<0)return false;
        const auto& s=*storage_.get();const auto bindings=staticBindings();
        if(bindings.empty()||bindings.size()!=s.count.terrain||s.candidates.size()!=s.count.terrain||
           (source!=s.mergedIds.data()&&source!=s.secondIds.data()&&source!=s.firstIds.data())||
           static_cast<std::size_t>(count)>storage::sourceSelectionCapacity||
           static_cast<std::size_t>(count)>(source==s.mergedIds.data()?s.mergedIds.size():source==s.secondIds.data()?s.secondIds.size():s.firstIds.size()))return false;
        const auto rows=s.sharedRoster_->count*storage::Storage::rosterRoles;
        if(s.collectionInput.objectCount!=s.count.terrain+rows||s.collectionInput.objectCount>s.count.objects)return false;
        SharedSelectedList result;result.count=static_cast<std::size_t>(count);result.projection=lease.projection_;
        //Validate actual published owner inventory before interpreting any ID.
        //A reserved row/ref without genuine descriptor/membership is not a hole
        //we can fill on behalf of the source collector.
        for(std::size_t i=0;i<s.count.terrain;++i){
            if(!bindings[i].candidate||s.candidates[i]!=bindings[i].candidate||
               !s.candidates[i]->candidate.collision.filter.identity||
               s.objects[i].identity!=s.candidates[i]->candidate.collision.filter.identity)return false;
            for(std::size_t j=0;j<i;++j)if(s.objects[j].identity==s.objects[i].identity)return false;
        }
        for(std::size_t i=0;i<rows;++i){const auto& row=s.sharedRoster_->objects[i];
            if(!row||!row->coldBound||!row->identity||row->slot!=i/storage::Storage::rosterRoles||
               static_cast<std::size_t>(row->role)!=i%storage::Storage::rosterRoles||
               row->reservedRef!=s.count.terrain+i+1u||!row->membership.state.attached||
               row->membership.state.object!=row->reservedRef||
               s.objects[row->reservedRef-1u].identity!=row->identity)return false;
            for(std::size_t j=0;j<s.count.terrain+i;++j)if(s.objects[j].identity==row->identity)return false;
        }
        for(std::size_t n=0;n<result.count;++n){const auto id=source[n];bool found=false;
            for(std::size_t i=0;i<s.count.terrain;++i)if(s.objects[i].identity==id){
                result.entries[n]={SharedSelectedKind::staticCandidate,id,i};found=true;break;
            }
            if(!found)for(std::size_t i=0;i<rows;++i){const auto& row=*s.sharedRoster_->objects[i];
                if(row.identity!=id)continue;
                result.entries[n]={row.role==storage::Storage::RosterRole::pod694?
                    SharedSelectedKind::pod694:SharedSelectedKind::otherRoster,id,i};found=true;break;
            }
            if(!found)return false;
        }
        destination=result;return true; //Only mapping, not a source list/merge return.
    }
