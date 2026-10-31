//Source-owned11ED60/11F680 passes over the SAME raw shape/node allocations.
//Existing finite-nearest math policy is reused, never semantic Resource.mesh().
    //Private E4 lease is native custody only: no result90, shape54 or ref60 store.
    struct SourceDirectTransformLease {
        const void* occurrence=nullptr;
        const void* shape=nullptr;
        const rr::assets::OriginalSourceCollisionFile50LA* file=nullptr;
        const Resource* resource=nullptr;
        const OriginalRacerSceneObject50LA* model=nullptr;
        bool attempted=false,live=false,nativeWithdrawn=false;
    } sourceDirectTransform_;
    bool preflightSourceDirectTransformNative()const noexcept {
        const auto& l=sourceDirectTransform_;const auto& s=sourcePrefix_;const auto& r=sourceResource_;
        if(!l.attempted)return !l.occurrence&&!l.shape&&!l.file&&!l.resource&&!l.model&&!l.live&&!l.nativeWithdrawn;
        if(!l.occurrence||!l.shape||!l.file||!l.resource||!l.model||l.live==l.nativeWithdrawn||
           sourceResult_.attempted||!r.pending||r.pending.get()!=l.resource||
           r.pending->sourceDirectTransformBorrower_!=(l.live?l.occurrence:nullptr))return false;
        if(l.nativeWithdrawn)return true; //Opaque identities only after exact native unpin.
        if(s.nativeWithdrawn||r.nativeWithdrawn||!r.storesReturned||s.file.get()!=l.file||
           s.model!=l.model||shape8C_.get()!=l.shape||!shape8C_||shape8C_->racerModel110!=l.model||
           shape8C_->resource124.get()!=l.resource||r.pending->sourceAssociationBorrower_!=l.file||
           !s.actual90||*s.actual90!=std::optional<const void*>(nullptr))return false;
        for(unsigned i=0;i<16;++i)if(s.local40[i].bytes!=r.matrix40[i].bytes||
            s.local40[i].known!=r.matrix40[i].known)return false;
        return true;
    }
    bool pinSourceDirectTransform11F5B4(const void* occurrence,
        const OriginalRacerSceneObject50LA& model)noexcept {
        auto& l=sourceDirectTransform_;auto& r=sourceResource_;const auto& s=sourcePrefix_;
        if(!occurrence||l.attempted||sourceResult_.attempted||s.nativeWithdrawn||s.model!=&model||
           !r.storesReturned||r.nativeWithdrawn||!r.pending||
           r.pending->sourceAssociationBorrower_!=s.file.get()||r.pending->sourceDirectTransformBorrower_||
           !preflightSourceType0PrefixNative())return false;
        l.attempted=true;l.live=true;l.occurrence=occurrence;l.shape=shape8C_.get();
        l.file=s.file.get();l.resource=r.pending.get();l.model=&model;
        r.pending->sourceDirectTransformBorrower_=occurrence;
        return true;
    }
    bool unpinSourceDirectTransformNative(const void* occurrence)noexcept {
        auto& l=sourceDirectTransform_;
        if(!l.attempted)return true;
        if(l.occurrence!=occurrence||sourceUpdate_.live||!preflightSourceDirectTransformNative())return false;
        if(l.nativeWithdrawn)return true;
        if(!preflightSourceType0PrefixNative()||!withdrawSourceType0TransformNative())return false;
        sourceResource_.pending->sourceDirectTransformBorrower_=nullptr;
        l.live=false;l.nativeWithdrawn=true;return true;
    }
    using Prepare=OriginalWreckSourceType0Prepare50LA;
    struct SourceTransformEvent {
        Prepare::Store kind=Prepare::Store::previousWord;
        unsigned index=0;
        std::uint32_t pc=0;
        std::array<std::uint32_t,4> bits{};
    };
    struct SourceTransformRootRead {
        std::uint32_t pc=0,offset=0;
        SourcePrefixWord word;
        bool addressReadable=false;
    };
    struct SourceTransformPass {
        bool entered=false,returned=false;
        std::uint32_t pc=0;
        std::array<SourceTransformEvent,42> events{};
        std::size_t eventCount=0;
        std::array<SourceTransformRootRead,6> rootReads{};
        std::size_t rootReadCount=0;
        std::array<std::uint32_t,16> previous{},world{},composed{};
        std::array<std::uint32_t,4> center{};
        std::optional<std::uint32_t> radius;
        std::uint16_t previousKnown=0,worldKnown=0,composedKnown=0;
    };
    struct SourceTransform {
        bool attempted=false,returned=false,nativeWithdrawn=false;
        const OriginalRacerSceneObject50LA* model=nullptr;
        SpringVector4_48N* center=nullptr;
        std::optional<float>* radius=nullptr;
        std::array<std::uint32_t,16> incoming{};
        std::uint16_t incomingKnown=0;
        BodyMatrix48G composed00{},world80{},previousC0{};
        std::uint16_t composedKnown=0,worldKnown=0,previousKnown=0;
        SourceTransformPass first,second;
        std::uint32_t pc=0;
    } sourceTransform_;
    //Distinct post-placement11FE50/11F680 occurrence; initialization is immutable.
    struct SourceUpdate {
        const void* occurrence=nullptr;
        const void* shape=nullptr;
        const rr::assets::OriginalSourceCollisionFile50LA* file=nullptr;
        const Resource* resource=nullptr;
        const OriginalRacerSceneObject50LA* model=nullptr;
        bool leaseAttempted=false,live=false,nativeWithdrawn=false;
        bool resultBacked=false;
        const void* initializationOccurrence=nullptr;
        const void* result90History=nullptr;
        const void* contactHistory=nullptr;
        bool attempted=false,returned=false;
        SpringVector4_48N* center=nullptr;
        std::optional<float>* radius=nullptr;
        std::array<std::uint32_t,16> incoming{};
        std::uint16_t incomingKnown=0;
        BodyMatrix48G composed00{},world80{},previousC0{};
        std::uint16_t composedKnown=0,worldKnown=0,previousKnown=0;
        SourceTransformPass pass;
        std::uint32_t pc=0;
    } sourceUpdate_;
    static std::uint32_t sourceFloatBits(const float& value)noexcept {
        std::uint32_t bits;std::memcpy(&bits,&value,4);return bits;
    }
    bool runSourceType0Pass(bool second)noexcept {
        auto& t=sourceTransform_;auto& p=second?t.second:t.first;
        p.entered=true;p.pc=second?0x11f6c8u:0x11eda8u;
        BodyMatrix48G incoming{},current40{};
        std::memcpy(incoming.v,t.incoming.data(),64);
        for(unsigned i=0;i<16;++i){const auto bits=sourcePrefix_.local40[i].bits();std::memcpy(&current40.v[i],&bits,4);}
        //Unknown scratch is backing only. Providers fill each root field only
        //at its genuine reached load, after all preceding output stores.
        rr::assets::CollisionMeshNode48I root{};
        float radiusScratch=t.radius->value_or(0.f); //Unknown optional stays unknown until actual store event.
        const auto matrixRead=[&](unsigned row,unsigned lane,unsigned term,std::uint32_t)noexcept {
            return sourcePrefix_.local40[4*row+term].known==15&&
                (t.incomingKnown&(std::uint16_t(1u)<<(4*term+lane)));
        };
        const auto rootRead=[&](bool center,unsigned axis,std::uint32_t pc)noexcept {
            if(p.rootReadCount>=p.rootReads.size())return false;
            auto& r=p.rootReads[p.rootReadCount++];r.pc=pc;r.offset=(center?0x0cu:0x18u)+4*axis;
            const auto& nodes=sourceNodes_.nodes;
            r.addressReadable=nodes.raw&&nodes.bytes>=std::size_t(r.offset)+4;
            if(!r.addressReadable)return false;
            for(unsigned i=0;i<4;++i){r.word.bytes[i]=nodes.raw[r.offset+i];
                if(nodes.known[r.offset+i])r.word.known|=std::uint8_t(1u<<i);}
            if(r.word.known!=15)return false;
            const auto bits=r.word.bits();std::memcpy(center?&root.center[axis]:&root.half[axis],&bits,4);return true;
        };
        const auto store=[&](Prepare::Store kind,unsigned index,std::uint32_t pc)noexcept {
            auto& e=p.events[p.eventCount++];e.kind=kind;e.index=index;e.pc=pc;
            switch(kind){
            case Prepare::Store::previousWord:
                e.bits[0]=sourceFloatBits(t.previousC0.v[index]);t.previousKnown|=std::uint16_t(1u<<index)&(second?t.worldKnown:t.incomingKnown);break;
            case Prepare::Store::worldWord:
                e.bits[0]=sourceFloatBits(t.world80.v[index]);t.worldKnown|=std::uint16_t(1u<<index)&t.incomingKnown;break;
            case Prepare::Store::composedQuad:
                std::memcpy(e.bits.data(),&t.composed00.v[4*index],16);t.composedKnown|=std::uint16_t(15u<<(4*index));break;
            case Prepare::Store::centerLane:e.bits[0]=sourceFloatBits(t.center->v[index]);break;
            case Prepare::Store::centerQuad:std::memcpy(e.bits.data(),t.center->v,16);break;
            case Prepare::Store::radius:*t.radius=radiusScratch;e.bits[0]=sourceFloatBits(radiusScratch);break;
            }
        };
        p.returned=Prepare::passSource(second,incoming,current40,root,t.composed00,t.world80,t.previousC0,
            *t.center,radiusScratch,p.pc,matrixRead,rootRead,store);
        std::memcpy(p.previous.data(),t.previousC0.v,64);std::memcpy(p.world.data(),t.world80.v,64);
        std::memcpy(p.composed.data(),t.composed00.v,64);std::memcpy(p.center.data(),t.center->v,16);
        if(*t.radius)p.radius=sourceFloatBits(**t.radius);
        p.previousKnown=t.previousKnown;p.worldKnown=t.worldKnown;p.composedKnown=t.composedKnown;t.pc=p.pc;
        return p.returned;
    }
    bool runSourceType0UpdatePass11F680()noexcept {
        auto& t=sourceUpdate_;auto& p=t.pass;
        constexpr bool second=true;
        p.entered=true;p.pc=second?0x11f6c8u:0x11eda8u;
        BodyMatrix48G incoming{},current40{};
        std::memcpy(incoming.v,t.incoming.data(),64);
        for(unsigned i=0;i<16;++i){const auto bits=sourcePrefix_.local40[i].bits();std::memcpy(&current40.v[i],&bits,4);}
        //Unknown scratch is backing only. Providers fill each root field only
        //at its genuine reached load, after all preceding output stores.
        rr::assets::CollisionMeshNode48I root{};
        float radiusScratch=t.radius->value_or(0.f); //Unknown optional stays unknown until actual store event.
        const auto matrixRead=[&](unsigned row,unsigned lane,unsigned term,std::uint32_t)noexcept {
            return sourcePrefix_.local40[4*row+term].known==15&&
                (t.incomingKnown&(std::uint16_t(1u)<<(4*term+lane)));
        };
        const auto rootRead=[&](bool center,unsigned axis,std::uint32_t pc)noexcept {
            if(p.rootReadCount>=p.rootReads.size())return false;
            auto& r=p.rootReads[p.rootReadCount++];r.pc=pc;r.offset=(center?0x0cu:0x18u)+4*axis;
            const auto& nodes=sourceNodes_.nodes;
            r.addressReadable=nodes.raw&&nodes.bytes>=std::size_t(r.offset)+4;
            if(!r.addressReadable)return false;
            for(unsigned i=0;i<4;++i){r.word.bytes[i]=nodes.raw[r.offset+i];
                if(nodes.known[r.offset+i])r.word.known|=std::uint8_t(1u<<i);}
            if(r.word.known!=15)return false;
            const auto bits=r.word.bits();std::memcpy(center?&root.center[axis]:&root.half[axis],&bits,4);return true;
        };
        const auto store=[&](Prepare::Store kind,unsigned index,std::uint32_t pc)noexcept {
            auto& e=p.events[p.eventCount++];e.kind=kind;e.index=index;e.pc=pc;
            switch(kind){
            case Prepare::Store::previousWord:
                e.bits[0]=sourceFloatBits(t.previousC0.v[index]);t.previousKnown=std::uint16_t((t.previousKnown&~(1u<<index))|(t.worldKnown&(1u<<index)));break;
            case Prepare::Store::worldWord:
                e.bits[0]=sourceFloatBits(t.world80.v[index]);t.worldKnown=std::uint16_t((t.worldKnown&~(1u<<index))|(t.incomingKnown&(1u<<index)));break;
            case Prepare::Store::composedQuad:
                std::memcpy(e.bits.data(),&t.composed00.v[4*index],16);t.composedKnown|=std::uint16_t(15u<<(4*index));break;
            case Prepare::Store::centerLane:e.bits[0]=sourceFloatBits(t.center->v[index]);break;
            case Prepare::Store::centerQuad:std::memcpy(e.bits.data(),t.center->v,16);break;
            case Prepare::Store::radius:*t.radius=radiusScratch;e.bits[0]=sourceFloatBits(radiusScratch);break;
            }
        };
        p.returned=Prepare::passSource(second,incoming,current40,root,t.composed00,t.world80,t.previousC0,
            *t.center,radiusScratch,p.pc,matrixRead,rootRead,store);
        std::memcpy(p.previous.data(),t.previousC0.v,64);std::memcpy(p.world.data(),t.world80.v,64);
        std::memcpy(p.composed.data(),t.composed00.v,64);std::memcpy(p.center.data(),t.center->v,16);
        if(*t.radius)p.radius=sourceFloatBits(**t.radius);
        p.previousKnown=t.previousKnown;p.worldKnown=t.worldKnown;p.composedKnown=t.composedKnown;t.pc=p.pc;
        return p.returned;
    }
    SourcePrefixResult continueSourceType0Transform11ED60(const OriginalRacerSceneObject50LA& actualModel,
        const std::array<std::uint32_t,16>& incoming,std::uint16_t incomingKnown,
        SpringVector4_48N& actualCenter,std::optional<float>& actualRadius)noexcept {
        auto& t=sourceTransform_;
        if(t.attempted||!sourceResult_.live||!sourceResult_.published90||sourcePrefix_.nativeWithdrawn||
           sourcePrefix_.model!=&actualModel||!preflightSourceType0PrefixNative()||actualRadius||
           sourceFloatBits(actualCenter.v[0])||sourceFloatBits(actualCenter.v[1])||sourceFloatBits(actualCenter.v[2])||
           sourceFloatBits(actualCenter.v[3])!=0x3f800000u)return {SourcePrefixStatus::owner,0x11ed60};
        return runAdmittedSourceType0Transform11ED60(actualModel,incoming,incomingKnown,actualCenter,actualRadius);
    }
    SourcePrefixResult continueSourceDirectType0Transform11ED60(const void* occurrence,
        const OriginalRacerSceneObject50LA& actualModel,const std::array<std::uint32_t,16>& incoming,
        std::uint16_t incomingKnown,SpringVector4_48N& actualCenter,std::optional<float>& actualRadius)noexcept {
        if(sourceTransform_.attempted||!sourceDirectTransform_.live||sourceDirectTransform_.occurrence!=occurrence||
           sourcePrefix_.nativeWithdrawn||sourcePrefix_.model!=&actualModel||!preflightSourceType0PrefixNative()||
           actualRadius||sourceFloatBits(actualCenter.v[0])||sourceFloatBits(actualCenter.v[1])||
           sourceFloatBits(actualCenter.v[2])||sourceFloatBits(actualCenter.v[3])!=0x3f800000u)
            return {SourcePrefixStatus::owner,0x11ed60};
        return runAdmittedSourceType0Transform11ED60(actualModel,incoming,incomingKnown,actualCenter,actualRadius);
    }
    SourcePrefixResult runAdmittedSourceType0Transform11ED60(const OriginalRacerSceneObject50LA& actualModel,
        const std::array<std::uint32_t,16>& incoming,std::uint16_t incomingKnown,
        SpringVector4_48N& actualCenter,std::optional<float>& actualRadius)noexcept {
        auto& t=sourceTransform_;
        t.attempted=true;t.model=&actualModel;t.center=&actualCenter;t.radius=&actualRadius;
        t.incoming=incoming;t.incomingKnown=incomingKnown;t.pc=0x11eda8;
        if(!runSourceType0Pass(false))return {SourcePrefixStatus::bounded,t.pc};
        //Actual11EF24 calls11F680; first-pass effects are retained separately.
        if(!runSourceType0Pass(true))return {SourcePrefixStatus::bounded,t.pc};
        t.returned=true;t.pc=0x11fe28;
        return {SourcePrefixStatus::reached,t.pc}; //No following COL attachment/detector.
    }
    bool preflightSourceType0PassNative(const SourceTransformPass& p,bool second,bool update=false,bool liveNodes=true)const noexcept {
        if(!p.entered)return !p.returned&&!p.eventCount&&!p.rootReadCount;
        const auto& t=sourceTransform_;const auto& prior=update?t.second:t.first;
        const auto& incoming=update?sourceUpdate_.incoming:t.incoming;
        const auto incomingKnown=update?sourceUpdate_.incomingKnown:t.incomingKnown;
        const unsigned total=second?42u:39u;
        if(p.eventCount<32||p.eventCount>total||p.rootReadCount>(second?6u:3u))return false;
        std::array<std::uint32_t,16> previous=incoming,world=incoming;
        std::array<std::uint32_t,16> composed=second?prior.composed:std::array<std::uint32_t,16>{};
        if(second)previous=prior.world;
        std::array<std::uint32_t,4> center=second?prior.center:std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}};
        std::optional<std::uint32_t> radius=second?prior.radius:std::optional<std::uint32_t>{};
        std::uint16_t composedKnown=second?prior.composedKnown:0;
        for(std::size_t i=0;i<p.eventCount;++i){
            Prepare::Store kind;unsigned index;std::uint32_t pc;
            if(i<16){kind=Prepare::Store::previousWord;index=unsigned(i);pc=(second?0x11f6e4u:0x11edc4u)+4u*(index&1u);}
            else if(i<32){kind=Prepare::Store::worldWord;index=unsigned(i-16);pc=(second?0x11f714u:0x11edf4u)+4u*(index&1u);}
            else if(i<36){kind=Prepare::Store::composedQuad;index=unsigned(i-32);pc=(second?0x11f790u:0x11ee70u)+4u*index;}
            else if(second&&i<40){kind=Prepare::Store::centerLane;index=unsigned(i-36);pc=index==3?0x11f7c8u:0x11f7b4u+8u*index;}
            else if(!second&&i==36){kind=Prepare::Store::centerQuad;index=0;pc=0x11ee9c;}
            else if(i==(second?40u:37u)){kind=Prepare::Store::radius;index=0;pc=second?0x11f81c:0x11eef0;}
            else{kind=Prepare::Store::centerQuad;index=1;pc=second?0x11f84c:0x11ef1c;}
            const auto& e=p.events[i];if(e.kind!=kind||e.index!=index||e.pc!=pc)return false;
            switch(kind){
            case Prepare::Store::previousWord:if(e.bits[0]!=previous[index])return false;break;
            case Prepare::Store::worldWord:if(e.bits[0]!=world[index])return false;break;
            case Prepare::Store::composedQuad:for(unsigned j=0;j<4;++j)composed[4*index+j]=e.bits[j];composedKnown|=std::uint16_t(15u<<(4*index));break;
            case Prepare::Store::centerLane:
                if(index==3){if(e.bits[0]!=0x3f800000u)return false;}
                else if(p.rootReadCount<=index||p.rootReads[index].word.known!=15||
                        e.bits[0]!=p.rootReads[index].word.bits())return false;
                center[index]=e.bits[0];break;
            case Prepare::Store::centerQuad:
                if(index==0&&(e.bits[0]!=incoming[12]||e.bits[1]!=incoming[13]||
                   e.bits[2]!=incoming[14]||e.bits[3]!=0x3f800000u))return false;
                if(index==1&&e.bits[3]!=0x3f800000u)return false;
                center=e.bits;break;
            case Prepare::Store::radius:radius=e.bits[0];break;
            }
        }
        if(p.previous!=previous||p.world!=world||p.composed!=composed||p.center!=center||p.radius!=radius||
           p.previousKnown!=(second?prior.worldKnown:incomingKnown)||p.worldKnown!=incomingKnown||
           p.composedKnown!=composedKnown)return false;
        for(std::size_t i=0;i<p.rootReadCount;++i){
            const bool centerRead=second&&i<3;const unsigned axis=unsigned(centerRead?i:second?i-3:i);
            const auto pc=(centerRead?0x11f7b0u:second?0x11f7d4u:0x11eea8u)+8u*axis;
            const auto offset=(centerRead?0x0cu:0x18u)+4u*axis;
            const auto& r=p.rootReads[i];const auto& nodes=sourceNodes_.nodes;
            if(r.pc!=pc||r.offset!=offset||r.word.known>15u||
               (liveNodes&&r.addressReadable!=bool(nodes.raw&&nodes.bytes>=std::size_t(offset)+4))||
               p.eventCount<(centerRead?36u+axis:second?40u:37u))return false;
            if(!r.addressReadable&&(r.word.known||r.word.bits()))return false;
            if(liveNodes&&r.addressReadable){SourcePrefixWord word;
                for(unsigned j=0;j<4;++j){word.bytes[j]=nodes.raw[offset+j];if(nodes.known[offset+j])word.known|=std::uint8_t(1u<<j);}
                if(word.bytes!=r.word.bytes||word.known!=r.word.known)return false;
            }
            if((!r.addressReadable||r.word.known!=15)&&(i+1!=p.rootReadCount||p.returned||p.pc!=r.pc))return false;
        }
        if(p.returned)return p.eventCount==total&&p.rootReadCount==(second?6u:3u)&&incomingKnown==0xffff&&
            p.pc==(second?0x11fe28u:0x11ef24u);
        return p.eventCount<total;
    }
    bool preflightSourceType0TransformNative()const noexcept {
        const auto& t=sourceTransform_;
        if(!preflightSourceDirectTransformNative()||!preflightSourceType0UpdateNative())return false;
        if(!t.attempted)return !t.returned&&!t.nativeWithdrawn&&!t.model&&!t.center&&!t.radius&&!t.first.entered&&!t.second.entered;
        if(t.nativeWithdrawn)return !t.center&&!t.radius;
        if(sourcePrefix_.nativeWithdrawn||t.model!=sourcePrefix_.model||!t.center||!t.radius||
           !(sourceDirectTransform_.attempted?sourceDirectTransform_.model==t.model:
             sourceResult_.attempted&&sourceResult_.published90)||!sourceResource_.storesReturned||
           !t.first.entered||t.second.entered!=t.first.returned||t.returned!=t.second.returned||
           !preflightSourceType0PassNative(t.first,false)||!preflightSourceType0PassNative(t.second,true))return false;
        const auto& last=t.second.entered?t.second:t.first;
        if(t.pc!=last.pc||std::memcmp(t.previousC0.v,last.previous.data(),64)||
           std::memcmp(t.world80.v,last.world.data(),64)||std::memcmp(t.composed00.v,last.composed.data(),64)||
           std::memcmp(t.center->v,(sourceUpdate_.attempted?sourceUpdate_.pass:last).center.data(),16)||t.previousKnown!=last.previousKnown||
           t.worldKnown!=last.worldKnown||t.composedKnown!=last.composedKnown)return false;
        const auto& latest=sourceUpdate_.attempted?sourceUpdate_.pass:last;
        return bool(*t.radius)==bool(latest.radius)&&(!latest.radius||sourceFloatBits(**t.radius)==*latest.radius);
    }
    bool withdrawSourceType0TransformNative()noexcept {
        auto& t=sourceTransform_;if(!t.attempted||t.nativeWithdrawn)return true;
        if(sourceUpdate_.live||!preflightSourceType0TransformNative()||sourceResult_.live)return false;
        t.center=nullptr;t.radius=nullptr;t.nativeWithdrawn=true;return true; //History only, no source rollback.
    }
    bool sourceType0TransformOutputsCurrentNative(const SpringVector4_48N& center,
        const std::optional<float>& radius)const noexcept {
        const auto& t=sourceTransform_;if(!t.attempted)return false;
        const auto& last=sourceUpdate_.attempted?sourceUpdate_.pass:(t.second.entered?t.second:t.first);
        return !std::memcmp(center.v,last.center.data(),16)&&bool(radius)==bool(last.radius)&&
            (!radius||sourceFloatBits(*radius)==*last.radius);
    }

    bool pinSourceDirectUpdate11FE50(const void* occurrence,const OriginalRacerSceneObject50LA& model)noexcept {
        auto& u=sourceUpdate_;const auto& t=sourceTransform_;
        if(!occurrence||u.leaseAttempted||!t.returned||t.nativeWithdrawn||t.model!=&model||
           !sourceDirectTransform_.live||sourceResult_.attempted||sourceContactBorrower_||
           !sourceResource_.pending||sourceResource_.pending->sourceUpdateBorrower_||
           !preflightSourceType0PrefixNative())return false;
        u.leaseAttempted=true;u.live=true;u.occurrence=occurrence;u.shape=shape8C_.get();
        u.initializationOccurrence=sourceDirectTransform_.occurrence;
        u.file=sourcePrefix_.file.get();u.resource=sourceResource_.pending.get();u.model=&model;
        sourceResource_.pending->sourceUpdateBorrower_=occurrence;
        return true;
    }
    //Distinct result90-backed E0 lease. A preexisting Contact point lease
    //continues unchanged: this pass mutates no vertex/resource/Contact bytes.
    bool pinSourceResultUpdate11FE50(const void* occurrence,const OriginalRacerSceneObject50LA& model)noexcept {
        auto& u=sourceUpdate_;const auto& t=sourceTransform_;const auto& lease=sourceResult_;
        if(!occurrence||u.leaseAttempted||!t.returned||t.nativeWithdrawn||t.model!=&model||
           sourceDirectTransform_.attempted||!lease.live||lease.nativeWithdrawn||!lease.published90||
           !lease.result90History||lease.modelSnapshot!=&model||
           !preflightSourceContactLeaseNative()||sourceContactNativeWithdrawn_||
           !sourceResource_.pending||sourceResource_.pending->sourceUpdateBorrower_||
           !preflightSourceType0PrefixNative())return false;
        u.leaseAttempted=true;u.live=true;u.resultBacked=true;u.occurrence=occurrence;
        u.shape=lease.shapeIdentity;u.file=lease.fileSnapshot;u.resource=lease.resourceSnapshot;u.model=&model;
        u.initializationOccurrence=lease.occurrence;u.result90History=lease.result90History;
        u.contactHistory=sourceContactHistory_;
        sourceResource_.pending->sourceUpdateBorrower_=occurrence;
        return true;
    }
    SourcePrefixResult continueSourceDirectType0Update11F680(const void* occurrence,
        const OriginalRacerSceneObject50LA& model,const std::array<std::uint32_t,16>& incoming,
        std::uint16_t incomingKnown,SpringVector4_48N& center,std::optional<float>& radius)noexcept {
        if(sourceUpdate_.resultBacked)return {SourcePrefixStatus::owner,0x11f680u};
        return continueSourceType0Update11F680(occurrence,model,incoming,incomingKnown,center,radius);
    }
    SourcePrefixResult continueSourceResultType0Update11F680(const void* occurrence,
        const OriginalRacerSceneObject50LA& model,const std::array<std::uint32_t,16>& incoming,
        std::uint16_t incomingKnown,SpringVector4_48N& center,std::optional<float>& radius)noexcept {
        if(!sourceUpdate_.resultBacked)return {SourcePrefixStatus::owner,0x11f680u};
        return continueSourceType0Update11F680(occurrence,model,incoming,incomingKnown,center,radius);
    }
    SourcePrefixResult continueSourceType0Update11F680(const void* occurrence,
        const OriginalRacerSceneObject50LA& model,const std::array<std::uint32_t,16>& incoming,
        std::uint16_t incomingKnown,SpringVector4_48N& center,std::optional<float>& radius)noexcept {
        auto& u=sourceUpdate_;const auto& t=sourceTransform_;
        if(!u.live||u.occurrence!=occurrence||u.model!=&model||u.attempted||
           t.center!=&center||t.radius!=&radius||!preflightSourceType0PrefixNative())
            return {SourcePrefixStatus::owner,0x11f680u};
        u.attempted=true;u.center=&center;u.radius=&radius;u.incoming=incoming;u.incomingKnown=incomingKnown;
        //11F6C8 starts with current shape80 from the prior completed init.
        //No identity/reset, extra first pass, or initialization receipt write.
        u.composed00=t.composed00;u.world80=t.world80;u.previousC0=t.previousC0;
        u.composedKnown=t.composedKnown;u.worldKnown=t.worldKnown;u.previousKnown=t.previousKnown;
        if(!runSourceType0UpdatePass11F680())return {SourcePrefixStatus::bounded,u.pc};
        //Actual common epilogue JR11FE3C / delay11FE40, independent of boolean V0.
        u.returned=true;u.pc=0x11fe44u;return {SourcePrefixStatus::reached,u.pc};
    }
    bool preflightSourceType0UpdateNative()const noexcept {
        const auto& u=sourceUpdate_;const auto& t=sourceTransform_;const auto& lease=sourceDirectTransform_;
        if(!u.leaseAttempted)return !u.occurrence&&!u.shape&&!u.file&&!u.resource&&!u.model&&
            !u.live&&!u.nativeWithdrawn&&!u.resultBacked&&!u.initializationOccurrence&&!u.result90History&&!u.contactHistory&&
            !u.attempted&&!u.returned&&!u.center&&!u.radius&&!u.pass.entered;
        if(!u.occurrence||!u.shape||!u.file||!u.resource||!u.model||u.live==u.nativeWithdrawn||
           !u.initializationOccurrence||!t.returned||!t.second.returned||t.pc!=0x11fe28u||t.model!=u.model||
           std::memcmp(t.previousC0.v,t.second.previous.data(),64)||
           std::memcmp(t.world80.v,t.second.world.data(),64)||std::memcmp(t.composed00.v,t.second.composed.data(),64)||
           t.previousKnown!=t.second.previousKnown||t.worldKnown!=t.second.worldKnown||
           t.composedKnown!=t.second.composedKnown||!t.first.entered||!t.first.returned||!t.second.entered||
           !preflightSourceType0PassNative(t.first,false,false,!u.nativeWithdrawn)||
           !preflightSourceType0PassNative(t.second,true,false,!u.nativeWithdrawn))return false;
        if(u.resultBacked){
            const auto& result=sourceResult_;
            if(lease.attempted||!result.attempted||!result.published90||!u.result90History||
               u.initializationOccurrence!=result.occurrence||u.result90History!=result.result90History||
               u.shape!=result.shapeIdentity||u.file!=result.fileSnapshot||u.resource!=result.resourceSnapshot||
               u.model!=result.modelSnapshot||u.contactHistory!=sourceContactHistory_||
               result.live==result.nativeWithdrawn||sourceContactPinAttempted_!=bool(u.contactHistory)||
               sourceContactBorrower_!=(sourceContactNativeWithdrawn_?nullptr:u.contactHistory)||
               (!sourceContactPinAttempted_&&sourceContactNativeWithdrawn_))return false;
            if(!u.nativeWithdrawn&&(!result.live||result.nativeWithdrawn||
               !preflightSourceResultShapeNative()||!preflightSourceContactLeaseNative()||
               sourceContactNativeWithdrawn_||sourceContactBorrower_!=u.contactHistory))return false;
        }else if(!lease.attempted||u.initializationOccurrence!=lease.occurrence||
                 u.shape!=lease.shape||u.file!=lease.file||u.resource!=lease.resource||u.model!=lease.model||
                 u.result90History||u.contactHistory||sourceResult_.attempted||(!u.nativeWithdrawn&&!lease.live))return false;
        if(u.nativeWithdrawn){if(u.center||u.radius)return false;}
        else if(t.nativeWithdrawn||
           sourcePrefix_.nativeWithdrawn||sourceResource_.nativeWithdrawn||!sourceResource_.pending||
           sourceResource_.pending.get()!=u.resource||sourceResource_.pending->sourceUpdateBorrower_!=u.occurrence||
           sourceResource_.pending->sourceAssociationBorrower_!=u.file||sourcePrefix_.file.get()!=u.file||
           sourcePrefix_.model!=u.model||shape8C_.get()!=u.shape||!shape8C_||
           shape8C_->racerModel110!=u.model||shape8C_->resource124.get()!=u.resource)return false;
        if(!u.attempted)return !u.returned&&!u.center&&!u.radius&&!u.pass.entered&&!u.pc;
        //Retired histories still validate their exact immutable predecessor,
        //store/read sequence, knownness, partial outputs and control endpoint.
        //Only the live raw-node reread is suppressed after native withdrawal.
        if(!u.pass.entered||u.returned!=u.pass.returned||u.pc!=(u.returned?0x11fe44u:u.pass.pc)||
           !preflightSourceType0PassNative(u.pass,true,true,!u.nativeWithdrawn))return false;
        const auto& p=u.pass;
        if(std::memcmp(u.previousC0.v,p.previous.data(),64)||std::memcmp(u.world80.v,p.world.data(),64)||
           std::memcmp(u.composed00.v,p.composed.data(),64)||u.previousKnown!=p.previousKnown||
           u.worldKnown!=p.worldKnown||u.composedKnown!=p.composedKnown)return false;
        if(u.nativeWithdrawn)return true; //No live outputs remain; all receipt checks above still apply.
        return u.center&&u.radius&&u.center==t.center&&u.radius==t.radius&&
            !std::memcmp(u.center->v,p.center.data(),16)&&bool(*u.radius)==bool(p.radius)&&
            (!p.radius||sourceFloatBits(**u.radius)==*p.radius);
    }
    bool unpinSourceDirectUpdateNative(const void* occurrence)noexcept {
        if(sourceUpdate_.leaseAttempted&&sourceUpdate_.resultBacked)return false;
        return unpinSourceUpdateNative(occurrence);
    }
    bool unpinSourceResultUpdateNative(const void* occurrence)noexcept {
        if(sourceUpdate_.leaseAttempted&&!sourceUpdate_.resultBacked)return false;
        return unpinSourceUpdateNative(occurrence);
    }
    bool unpinSourceUpdateNative(const void* occurrence)noexcept {
        auto& u=sourceUpdate_;if(!u.leaseAttempted)return true;
        if(u.occurrence!=occurrence||!preflightSourceType0UpdateNative())return false;
        if(u.nativeWithdrawn)return true;
        if(!preflightSourceType0PrefixNative())return false;
        sourceResource_.pending->sourceUpdateBorrower_=nullptr;
        u.center=nullptr;u.radius=nullptr;u.live=false;u.nativeWithdrawn=true;
        return true; //Newest output borrow withdrawn before immutable init lease.
    }
