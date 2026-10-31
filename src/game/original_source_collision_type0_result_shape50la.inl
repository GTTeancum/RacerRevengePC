//Exact same-shape lease for the unconditional11BBC0 result caller. Shape48/54
//are raw matrix lanes on type0; neither is a semantic Type2 count/mode field.
    struct SourceResultShape {
        const void* occurrence=nullptr;
        const void* shapeIdentity=nullptr;
        const void* result90History=nullptr;
        const rr::assets::OriginalSourceCollisionFile50LA* fileSnapshot=nullptr;
        const Resource* resourceSnapshot=nullptr;
        const OriginalRacerSceneObject50LA* modelSnapshot=nullptr;
        bool attempted=false,live=false,nativeWithdrawn=false;
        bool store54Returned=false,read48Attempted=false,read48Known=false,published90=false;
        std::uint32_t argument54=0,value48=0;
        SourcePrefixWord before54,read48;
    } sourceResult_;
    bool preflightSourceResultShapeNative()const noexcept {
        const auto& l=sourceResult_;const auto& s=sourcePrefix_;const auto& r=sourceResource_;
        if(!l.attempted)return !l.occurrence&&!l.shapeIdentity&&!l.result90History&&!l.fileSnapshot&&!l.resourceSnapshot&&!l.modelSnapshot&&!l.live&&
            !l.nativeWithdrawn&&!l.store54Returned&&!l.read48Attempted&&!l.read48Known&&!l.published90;
        if(!l.occurrence||!l.shapeIdentity||!l.fileSnapshot||!l.resourceSnapshot||!l.modelSnapshot||!r.storesReturned||!r.pending||
           l.resourceSnapshot!=r.pending.get()||
           l.live==l.nativeWithdrawn||l.argument54||l.read48Known!=(l.read48Attempted&&l.read48.known==15)||
           (l.published90&&(!l.read48Known||!l.result90History))||
           (!l.published90&&l.result90History)||(l.read48Attempted&&!l.store54Returned))return false;
        if(l.before54.bytes!=r.matrix40[5].bytes||l.before54.known!=r.matrix40[5].known)return false;
        if(l.read48Attempted&&(l.read48.bytes!=r.matrix40[2].bytes||l.read48.known!=r.matrix40[2].known||
           (l.read48Known&&l.value48!=l.read48.bits())))return false;
        if(r.pending->sourceResultBorrower_!=(l.live?l.occurrence:nullptr))return false;
        if(s.nativeWithdrawn)return l.nativeWithdrawn&&!l.live;
        if(l.shapeIdentity!=shape8C_.get()||l.fileSnapshot!=s.file.get()||l.modelSnapshot!=s.model||!s.actual90||!*s.actual90)return false;
        const void* expected=l.live&&l.published90?l.result90History:nullptr;
        return **s.actual90==expected;
    }
    bool pinSourceResult11BBE0(const void* occurrence)noexcept {
        auto& l=sourceResult_;const auto& s=sourcePrefix_;auto& r=sourceResource_;
        if(!occurrence||l.attempted||sourceDirectTransform_.attempted||s.nativeWithdrawn||r.nativeWithdrawn||!r.storesReturned||
           !r.pending||r.pending->sourceAssociationBorrower_!=s.file.get()||
           r.pending->sourceResultBorrower_||!preflightSourceType0PrefixNative())return false;
        l.attempted=true;l.live=true;l.occurrence=occurrence;l.shapeIdentity=shape8C_.get();
        l.fileSnapshot=s.file.get();l.resourceSnapshot=r.pending.get();l.modelSnapshot=s.model;
        l.before54=s.local40[5];r.pending->sourceResultBorrower_=occurrence;
        return true; //Native lease only; no original shape/result store yet.
    }
    bool storeSourceShape54_11BBEC(const void* occurrence,const void* exactShape,std::uint32_t rawA3)noexcept {
        auto& l=sourceResult_;
        if(!l.live||l.nativeWithdrawn||l.occurrence!=occurrence||l.shapeIdentity!=exactShape||
           l.store54Returned||rawA3!=0||!preflightSourceType0PrefixNative())return false;
        //Actual166078 supplied rawzero. This delay-slot store precedes malloc;
        //it changes only current shape54, never the earlier Resource matrix.
        l.argument54=rawA3;sourcePrefix_.local40[5].store(rawA3);l.store54Returned=true;
        return true;
    }
    bool readSourceShape48_11BC3C(const void* occurrence,const void* exactShape,std::uint32_t& outRaw)noexcept {
        auto& l=sourceResult_;
        if(!l.live||l.nativeWithdrawn||l.occurrence!=occurrence||l.shapeIdentity!=exactShape||
           !l.store54Returned||l.read48Attempted||!preflightSourceType0PrefixNative())return false;
        l.read48Attempted=true;l.read48=sourcePrefix_.local40[2];l.read48Known=l.read48.known==15;
        if(!l.read48Known)return false; //Actual unknown load; no guessed semantic count.
        l.value48=l.read48.bits();outRaw=l.value48;return true;
    }
    bool publishSourceResult90_11BD18(const void* occurrence,const void* actualResult)noexcept {
        auto& l=sourceResult_;
        if(!l.live||l.nativeWithdrawn||l.occurrence!=occurrence||!l.read48Known||
           l.published90||!actualResult||!preflightSourceType0PrefixNative())return false;
        *sourcePrefix_.actual90=actualResult;l.result90History=actualResult;l.published90=true;
        return true; //Exact privileged COL caller proves all preceding publications.
    }
    bool unpinSourceResultNative(const void* occurrence)noexcept {
        auto& l=sourceResult_;
        if(!l.attempted)return true;
        if(l.occurrence!=occurrence||sourceUpdate_.live)return false;
        if(l.nativeWithdrawn)return preflightSourceResultShapeNative();
        if(!preflightSourceType0PrefixNative())return false;
        //COL has retired its native result arrays/backing, including partial
        //unpublished/NULL cases. Clear exact native publication, not source history.
        *sourcePrefix_.actual90=nullptr;sourceResource_.pending->sourceResultBorrower_=nullptr;
        l.live=false;l.nativeWithdrawn=true;return true;
    }
