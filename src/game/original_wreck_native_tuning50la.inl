// Included only inside OriginalWreckRacerStorage50LA's private scope.
// The native numerical admission is finite FE_TONEAREST, as in the retained
// tuning helpers. Live source stores are deliberately not transactional.
    static std::string_view nativeTuningKey(std::string_view stem,std::size_t n,char (&out)[32])noexcept {
        std::memcpy(out,stem.data(),stem.size());
        const auto r=std::to_chars(out+stem.size(),out+32,n);
        return r.ec==std::errc{}?std::string_view(out,static_cast<std::size_t>(r.ptr-out)):std::string_view{};
    }
    bool nativeTuningPieceCurrent(const OriginalWreckCollectionStorage50LA& c,std::size_t i)const noexcept {
        if(i>=c.sourceCount30_||i>=c.count_||!c.sourceArray38_||c.sourceArray38_!=c.ordered38_.get()||
           c.sourceArray38_[i]!=&c.pieces_[i])return false;
        const auto& p=c.pieces_[i];const auto body=p.sceneCount_+1u;
        if(p.collectionOrdinal_!=c.ordinal_||p.pieceOrdinal_!=i+1u||!p.assets_||!p.assets_->prepared()||
           !p.progress_.appended22C1B8||p.progress_.pieceRetired||p.progress_.retirementAttempted||
           body+3u>=p.componentCount_||!p.sourceBodyD0_||*p.sourceBodyD0_!=p.componentSlots_[body]||
           !p.sourceGravityD4_||*p.sourceGravityD4_!=p.componentSlots_[body+1u]||
           !p.sourceDC_||*p.sourceDC_!=p.componentSlots_[body+3u]||
           !p.sourceModel14C_||*p.sourceModel14C_!=&p.modelOwner_.model_||p.modelOwner_.released()||
           !p.modelPackets_.constructed240740()||!p.modelMaterials_.complete()||!p.modelGeometry_||
           p.modelOwner_.fields().packets1F0!=&p.modelPackets_||p.modelOwner_.fields().geometry1B4!=p.modelGeometry_.get()||
           !p.physical_.bodyConstructorFieldsApplied||!p.physical_.contactConstructorFieldsApplied||
           p.physical_.contactBody30!=&p.physical_.body||p.physical_.linkedCount268||
           p.physical_.model288!=p.assets_->model14C()||
           p.physical_.modelGraphNodes!=p.assets_->modelGraphIdentity()||
           p.physical_.modelGraphCount!=p.assets_->modelGraphNodeCount()||
           p.gravity_.count30!=1u||p.gravity_.capacity34!=1u||p.gravity_.target50!=&p.physical_)return false;
        return true;
    }
    bool nativeTuningCollectionCurrent(const OriginalWreckCollectionStorage50LA& c)const noexcept {
        const auto& g=c.group_;
        if(!c.count_||c.sourceCount30_!=c.count_||c.sourceSlots38_!=c.sourceCount30_||
           c.completedPieceConstructors_!=c.sourceCount30_||c.source3C_!=&c.groupComponent_||
           !c.nativeGroupBound_||c.nativeGroupRetired_||!g.bound_||!g.cold_||g.freshSelected_||g.nativeRetired_||
           g.component_!=&c.groupComponent_||g.capacity34_!=c.count_-1u||g.allocated_!=g.capacity34_||
           g.primary_.piece!=&c.pieces_[0].piece_||g.primary_.physical!=&c.pieces_[0].physical_||
           g.primary_.component!=c.pieces_[0].componentSlots_[0])return false;
        for(std::size_t i=0;i<c.count_;++i)if(!nativeTuningPieceCurrent(c,i))return false;
        for(std::size_t i=0;i<g.allocated_;++i)if(g.attachedA0_[i]||g.breakLatchB0_[i])return false;
        return true;
    }
    // The private source-history check is the ONLY admission for disposing
    // registered but never attached native members. Payload not yet written
    // (48/A4/A8) is never consumed, and registered30 is never forged/reset.
    bool nativeTuningRowsRetirable(const OriginalWreckCollectionStorage50LA& c)const noexcept {
        const auto& g=c.group_;
        if(!nativeTuningAttempted_||g.freshSelected_||!g.bound_||g.nativeRetired_||
           g.component_!=&c.groupComponent_||g.capacity34_!=g.allocated_||
           g.primary_.piece!=&c.pieces_[0].piece_||g.primary_.physical!=&c.pieces_[0].physical_||
           g.primary_.component!=c.pieces_[0].componentSlots_[0])return false;
        std::size_t completed=0;
        for(std::size_t slot=0;slot<g.allocated_;++slot){
            if(g.attachedA0_[slot]||g.breakLatchB0_[slot])return false;
            const auto& row=g.pieces44_[slot];
            if(!row.piece){if(row.physical||row.component)return false;continue;}
            const OriginalWreckPieceStorage50LA* owner=nullptr;
            for(std::size_t pi=1;pi<c.count_;++pi)if(row.piece==&c.pieces_[pi].piece_){owner=&c.pieces_[pi];break;}
            if(!owner||row.physical!=&owner->physical_||row.component!=owner->componentSlots_[0]||
               !owner->nativeTuning_.entered||!owner->nativeTuning_.publishedSlot||
               *owner->nativeTuning_.publishedSlot!=slot||owner->nativeTuning_.registrationStores<1u||
               owner->nativeTuning_.registrationStores>5u)return false;
            if(owner->nativeTuning_.registrationStores==5u)++completed;
        }
        return completed==g.registered30_;
    }
    bool nativeTune166400(OriginalWreckPieceStorage50LA& owner,const std::array<std::optional<float>,10>& t,
        float weight,float inertiaScale,std::uint32_t& pc)noexcept {
        using namespace original_wreck_tuning_detail50LA;
        auto& p=owner.physical_;auto& piece=owner.piece_;Math f;
        auto at=[&](std::uint32_t v){pc=owner.nativeTuning_.pc=v;};
        //Actual166498 call on sourceModel14C, with retained source stack
        //outputs and partial graph/cache history. Never use the asset preview.
        auto& h=owner.nativeTuning_;at(0x166454u);h.center40={0,0,0,0x3f800000u};
        at(0x16645cu);h.half50={0,0,0,0x3f800000u};
        at(0x166498u);h.bounds.reset(new(std::nothrow) OriginalRacerModelSceneOwner50LA::SourceLocalBounds28D670);
        if(!h.bounds)return false; //Native history backing, not a guest allocation.
        const auto bounds=owner.modelOwner_.produceNativeWreckLocalBounds28D670(owner.assets_->graph(),h.center40,h.half50,*h.bounds);
        if(!bounds.okay()){at(bounds.sourcePC);return false;}
        float half[4];std::memcpy(half,h.half50.data(),16);
        at(0x1664b8u);const float x=f.mul(2.f,half[0]);if(!f.valid)return false;
        at(0x1664c0u);const float y=f.mul(2.f,half[1]);if(!f.valid)return false;
        at(0x1664c8u);const float z=f.mul(2.f,half[2]);if(!f.valid)return false;
        at(0x1664ecu);const float xx=f.mul(x,x);if(!f.valid)return false;
        at(0x1664f0u);const float yy=f.mul(y,y);if(!f.valid)return false;
        at(0x1664f4u);const float xy=f.add(xx,yy);if(!f.valid)return false;
        at(0x1664f8u);const float xyz=f.add(xy,f.mul(z,z));if(!f.valid)return false;
        at(0x1664fcu);const float extent=f.root(xyz);if(!f.valid)return false;
        if(extent>0.f){at(0x166528u);const float inv=f.div(1.f,extent);if(!f.valid)return false;
            at(0x16652cu);piece.inverseExtent118=inv;}
        const float unit=original_wreck_physical_detail50LA::word(0x4200cccdu);
        at(0x166540u);const float mass=f.div(weight,unit);if(!f.valid)return false;
        if(mass>0.f){at(0x165318u);p.parameters.body.mass=mass;
            at(0x16532cu);const float inv=f.div(1.f,mass);if(!f.valid)return false;
            at(0x165330u);p.parameters.body.inverseMass=inv;}
        auto matrix=original_wreck_physical_detail50LA::identity();
        // Source MULA/MADDA/MADD are represented by the existing finite native
        // binary32 arithmetic model; this is not an EE accumulator equivalence claim.
        at(0x1665a4u);const float y2=f.mul(y,y);if(!f.valid)return false;
        at(0x1665a8u);const float yz=f.add(y2,f.mul(z,z));if(!f.valid)return false;
        at(0x1665acu);const float x2=f.mul(x,x);if(!f.valid)return false;
        at(0x1665b0u);const float xz=f.add(x2,f.mul(z,z));if(!f.valid)return false;
        at(0x1665b4u);const float x22=f.mul(x,x);if(!f.valid)return false;
        at(0x1665b8u);const float xy2=f.add(x22,f.mul(y,y));if(!f.valid)return false;
        const unsigned lane[]={0,5,10};const float sum[]={yz,xz,xy2};
        const std::uint32_t division[]={0x1665bcu,0x1665d8u,0x1665f0u};
        const std::uint32_t scale[]={0x1665c4u,0x1665dcu,0x1665f4u};
        const std::uint32_t product[]={0x1665c8u,0x1665e0u,0x1665f8u};
        for(unsigned j=0;j<3;++j){at(division[j]);const float d=f.div(p.parameters.body.mass,12.f);if(!f.valid)return false;
            at(scale[j]);const float s=f.mul(inertiaScale,d);if(!f.valid)return false;
            at(product[j]);matrix.v[lane[j]]=f.mul(s,sum[j]);if(!f.valid)return false;}
        at(0x165364u);p.inertia100=matrix; //all input copies precede14E910
        at(0x14e910u);const auto inverse=f.inverse(matrix);if(!f.valid)return false;
        inverseToParameters(inverse,p.parameters);
        at(0x295c9cu);p.response.maxPenetration90=*t[0];
        at(0x295cb8u);float v=f.mul(*t[1],p.parameters.body.mass);if(!f.valid)return false;
        at(0x295cbcu);v=f.mul(unit,v);if(!f.valid)return false;
        at(0x295cd0u);p.response.stiffness9C=std::fabs(v);
        float damping=0.f;
        if(p.parameters.body.mass>0.f&&p.response.stiffness9C>0.f){
            at(0x295cfcu);const float m=f.mul(p.parameters.body.mass,p.response.stiffness9C);if(!f.valid)return false;
            at(0x295d04u);const float r=f.root(m);if(!f.valid)return false;
            at(0x295d18u);const float d=f.mul(2.f,*t[2]);if(!f.valid)return false;
            at(0x295d20u);damping=f.mul(d,r);if(!f.valid)return false;}
        at(0x295d28u);p.response.dampingA0=damping;
        at(0x295d44u);v=f.mul(*t[3],p.parameters.body.mass);if(!f.valid)return false;
        at(0x295d48u);v=f.mul(unit,v);if(!f.valid)return false;
        at(0x295d5cu);p.response.impactStiffnessA4=std::fabs(v);
        damping=0.f;
        if(p.parameters.body.mass>0.f&&p.response.impactStiffnessA4>0.f){
            at(0x295d88u);const float m=f.mul(p.parameters.body.mass,p.response.impactStiffnessA4);if(!f.valid)return false;
            at(0x295d90u);const float r=f.root(m);if(!f.valid)return false;
            at(0x295da4u);const float d=f.mul(2.f,*t[4]);if(!f.valid)return false;
            at(0x295dacu);damping=f.mul(d,r);if(!f.valid)return false;}
        at(0x295db4u);p.response.impactDampingA8=damping;
        at(0x295db8u);p.generated.impact.restitutionB8=0.f;
        at(0x295dbcu);p.generated.impact.restitutionBC=0.f;
        at(0x295dc0u);p.generated.impact.restitutionC0=0.f;
        at(0x295decu);p.generated.frictionByMaterial[0]=*t[6];
        at(0x166654u);piece.dragEC=*t[8];at(0x16665cu);piece.rateDampingF0=*t[9];
        at(0x166664u);piece.waterDragF4=1.f;at(0x16666cu);piece.waterRateF8=200.f;
        at(0x166674u);piece.buoyancyFC=.5f;at(0x16667cu);piece.baseDrag100=piece.dragEC;
        at(0x166684u);piece.baseRateDamping104=piece.rateDampingF0;
        at(0x22c53cu);return true;
    }
    bool nativeRegister2CA190(OriginalWreckCollectionStorage50LA& c,OriginalWreckPieceStorage50LA& p,
        float rate,float scale,std::uint32_t& pc)noexcept {
        auto& g=c.group_;auto& h=p.nativeTuning_;
        auto at=[&](std::uint32_t value){pc=h.pc=value;};at(0x2ca190u);
        const OriginalWreckGroupPiece50LA member{&p.piece_,&p.physical_,p.componentSlots_[0]};
        if(member.piece==g.primary_.piece){at(0x22c594u);return true;}
        std::size_t free=g.allocated_;
        for(std::size_t slot=0;slot<g.allocated_;++slot){
            const auto& old=g.pieces44_[slot];
            if(old.piece==member.piece){
                if(old.physical!=member.physical||old.component!=member.component)return false;
                at(0x2ca1ccu);g.breakRateA4_[slot]=rate;at(0x2ca1dcu);g.breakScaleA8_[slot]=scale;
                at(0x22c594u);return true;
            }
            if(!old.piece&&free==g.allocated_)free=slot;
        }
        if(free==g.allocated_){at(0x22c594u);return true;}
        at(0x2ca230u);g.pieces44_[free]=member;h.publishedSlot=free;h.registrationStores=1;
        at(0x2ca240u);g.breakRateA4_[free]=rate;h.registrationStores=2;
        at(0x2ca24cu);g.breakScaleA8_[free]=scale;h.registrationStores=3;
        SpringVector4_48N offset{{0.f,0.f,0.f,1.f}};
        for(unsigned j=0;j<3;++j){at(0x2ca268u+16u*j);
            const float a=member.piece->authored50.v[j],b=g.primary_.piece->authored50.v[j];
            if(!std::isfinite(a)||!std::isfinite(b)||(a!=0.f&&!std::isnormal(a))||(b!=0.f&&!std::isnormal(b)))return false;
            volatile float value=a-b;
            if(!std::isfinite(value)||(value!=0.f&&!std::isnormal(value)))return false;
            offset.v[j]=value;
        }
        at(0x2ca2a0u);g.offsets48_[free]=offset;h.registrationStores=4;
        at(0x2ca2b0u);++g.registered30_;h.registrationStores=5;
        at(0x22c594u);return true;
    }
    bool nativeTuneCollection22C310(OriginalWreckCollectionStorage50LA& c,rr::assets::OriginalPhyCursor50LA& cursor,
        std::string_view name,std::uint32_t& pc)noexcept {
        pc=0x22c334u;bool present=false;if(!cursor.select157D00(name,present))return false;
        // These source locals are UNKNOWN afresh on each call.22C610's spills
        // invalidate cross-collection assumptions. Weight/Inertia persist only
        // between pieces of THIS invocation, matching the reused source slots.
        std::array<std::optional<float>,10> t{};std::optional<float> weight,inertia;
        std::optional<std::string_view> unusedPart;
        constexpr std::string_view keys[]={"MaxCompression","Spring","Damping","ImpulseSpring","ImpulseDamping",
            "Restitution","Friction","FrictionTorqueScale","Drag","RateDamping"};
        for(unsigned j=0;j<10;++j){pc=0x22c348u+20u*j;if(!cursor.float158A70(keys[j],t[j]))return false;}
        for(std::size_t i=0;i<c.sourceCount30_;++i){
            if(!nativeTuningPieceCurrent(c,i))return false;auto& p=c.pieces_[i];
            if(p.nativeTuning_.entered)return false;p.nativeTuning_.entered=true;
            auto at=[&](std::uint32_t v){pc=p.nativeTuning_.pc=v;};char key[32];
            at(0x22c480u);if(!cursor.string158240(nativeTuningKey("Part",i+1u,key),unusedPart))return false;
            at(0x22c490u);if(!cursor.float158A70(nativeTuningKey("Weight",i+1u,key),weight))return false;
            at(0x22c4a0u);if(!cursor.float158A70(nativeTuningKey("Inertia",i+1u,key),inertia))return false;
            float breakRate,breakScale;
            at(0x22c4b8u);if(!cursor.defaultFloat158B80(nativeTuningKey("BreakRate",i+1u,key),1.f,breakRate))return false;
            at(0x22c4d0u);if(!cursor.defaultFloat158B80(nativeTuningKey("BreakScale",i+1u,key),1.f,breakScale))return false;
            at(0x22c4dcu);if(!t[8])return false;at(0x22c4f0u);if(!t[9])return false;
            at(0x22c508u);if(!inertia)return false;at(0x22c50cu);if(!t[0])return false;
            at(0x22c510u);if(!t[1])return false;at(0x22c514u);if(!t[2])return false;
            at(0x22c51cu);if(!t[3])return false;at(0x22c524u);if(!t[4])return false;
            at(0x22c528u);if(!t[6])return false;at(0x22c538u);if(!weight)return false;
            if(!nativeTune166400(p,t,*weight,*inertia,pc))return false;
            at(0x22c540u);if(!t[5])return false;at(0x22c54cu);p.piece_.restitution110=*t[5];
            at(0x22c554u);if(!t[7])return false;at(0x22c560u);p.piece_.frictionTorqueScale114=*t[7];
            at(0x22c574u);p.physical_.generated.impact.forceTorqueScaleB4=0.f;
            if(!nativeRegister2CA190(c,p,breakRate,breakScale,pc))return false;p.nativeTuning_.complete=true;
        }
        pc=0x22c5c0u;c.group_.mass4C_=c.pieces_[0].physical_.parameters.body.mass;
        // Eight source doubleword copies, not a center/recompute operation.
        for(unsigned j=0;j<8;++j){pc=0x22c5e0u;std::memcpy(c.group_.inertia60_.v+2u*j,c.pieces_[0].physical_.inertia100.v+2u*j,8);}
        pc=0x215b78u;return true;
    }
    bool continueNativeTuning215AF8(rr::assets::OriginalPhyCursor50LA& cursor,
        std::optional<float>& gravity47C,std::uint32_t& pc)noexcept {
        pc=nativeTuningPC_;
        if(nativeTuningAttempted_||!nativeCollectionsReached212B50()||std::fegetround()!=FE_TONEAREST)return false;
        for(std::size_t i=0;i<storage_->count;++i){const auto& c=storage_->collections[i];
            if(!nativeTuningCollectionCurrent(c)||c.group_.registered30_)return false;
            for(std::size_t j=0;j<c.group_.allocated_;++j)if(c.group_.pieces44_[j].piece)return false;
        }
        nativeTuningAttempted_=true;
        // Persist the terminal PC on every exit without creating resumability.
        struct Finish {std::uint32_t& into;std::uint32_t& from;~Finish(){into=from;}} finish{nativeTuningPC_,pc};
        bool present=false; //Same live selection/cache, lent by the actual PHY owner.
        pc=0x215b00u;if(!cursor.select157D00("Wreckage",present))return false;
        pc=0x215b14u;if(!cursor.float158A70("Gravity",gravity47C))return false;
        std::optional<std::string_view> collectionName;char key[32];
        for(std::size_t i=0;i<storage_->count;++i){
            pc=0x215b2cu;if(!cursor.select157D00("Wreckage",present))return false;
            pc=0x215b54u;if(!cursor.string158240(nativeTuningKey("Part",i+1u,key),collectionName))return false;
            auto& c=storage_->collections[i];pc=0x22c334u;if(!collectionName)return false;
            if(!nativeTuneCollection22C310(c,cursor,*collectionName,pc))return false;
            pc=0x215b84u;if(!gravity47C)return false;
            pc=0x215b9cu;original_wreck_tuning_detail50LA::Math math;
            const float acceleration=math.mul(original_wreck_physical_detail50LA::word(0x4200cccdu),*gravity47C);
            if(!math.valid)return false;
            for(std::size_t j=0;j<c.sourceCount30_;++j){pc=0x22c644u;if(!nativeTuningPieceCurrent(c,j))return false;
                pc=0x1143e4u;c.pieces_[j].gravity_.acceleration38=acceleration;c.pieces_[j].nativeTuning_.gravityStored=true;}
            pc=0x215ba0u;
        }
        pc=0x215bb8u;nativeTuningComplete_=true;return true;
    }
