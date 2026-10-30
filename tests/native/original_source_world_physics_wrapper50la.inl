#pragma once
namespace ra_cold_aggregate50LA {
//Conditional type2 leaf. Current Standard-only cohort does NOT issue it.
//Future caller must first genuinely resolve type2 and retain the current parser.
bool Owner::constructSourcePhysicsWrapper27BA3C(std::size_t zone,std::size_t slot,std::uint32_t& pc)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||!sourceStaticOwner_||
       !sourceStaticOwner_->sourceMode_||sourceStaticOwner_->closed_||!sourceStaticOwner_->sourceLedgerPrepared_||
       !worldApplication_||sourceStaticOwner_->application_!=worldApplication_||
       sourceStaticPhase_!=SourceStaticPhase::continuing||!sourceStaticFactoryVtable_||*sourceStaticFactoryVtable_!=0x3d2630||
       !sourceStaticResolvedType_||*sourceStaticResolvedType_!=2||
       sourceStaticZoneIndex_!=zone||sourceStaticModelIndex_!=slot||
       zone>=sourceStaticOwner_->parents_.size()||!sourceStaticOwner_->parents_[zone])return false;
    auto& owner=*sourceStaticOwner_;auto& parent=*owner.parents_[zone];
    if(slot>=owner.sourceWrapperLedger_[zone].size()||!parent.sourceEntries34||
       slot!=parent.sourceEntriesKnown||slot>=parent.count30)return false;
    auto& owned=owner.sourceWrapperLedger_[zone][slot];if(owned)return false;
    using W=ra_static_model_cold50LA::Owner::StandardWrapper;
    pc=0x27ba3c; //Actual allocator argument16 at branch delay27BA40.
    try{owned.reset(new W);}catch(...){owned.reset();}
    if(owned){auto& w=*owned;auto& h=w.sourcePhysics;w.sourceMode=true;
        pc=0x27ba54;w.sourceVtable04=0x3d2710;
        pc=0x27ba60;w.type00=0;
        pc=0x27ba64;w.sourceVtable04=0x3d26c0;w.dispatch=W::Dispatch::physics3D26C0;
        pc=0x27ba68;h.body8.emplace(nullptr);
        pc=0x27ba6c;h.descriptor0C.reset();h.descriptorLaneKnown=true;
    }
    pc=0x27ba74;parent.sourceEntries34[slot]=owned.get();++parent.sourceEntriesKnown; //NULL is published too.
    pc=0x27bac4;if(!owned)return false; //Original NULL type00 store, not graceful factory0.
    owned->type00=*sourceStaticResolvedType_;owned->sourceTypeKnown=true;
    pc=0x27bad8;owned->sourcePhysics.factoryReturned=true;return true; //Actual factory return, no descriptor/count/body admission.
}
bool Owner::loadSourcePhysicsDescriptor27B370(std::size_t zone,std::size_t slot,std::uint32_t& pc)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||!sourceStaticOwner_||!sourceWorldConfig_||
       !sourceStaticOwner_->sourceMode_||sourceStaticOwner_->closed_||sourceStaticPhase_!=SourceStaticPhase::continuing||
       !worldApplication_||sourceStaticOwner_->application_!=worldApplication_||
       sourceStaticZoneIndex_!=zone||sourceStaticModelIndex_!=slot||zone>=sourceStaticOwner_->parents_.size()||
       !sourceStaticOwner_->parents_[zone]||slot>=sourceStaticOwner_->sourceWrapperLedger_[zone].size())return false;
    auto& owner=*sourceStaticOwner_;auto& parent=*owner.parents_[zone];auto& owned=owner.sourceWrapperLedger_[zone][slot];
    if(!owned||!parent.sourceEntries34||slot>=parent.sourceEntriesKnown||parent.sourceEntries34[slot]!=owned.get()||
       !owner.sourcePhysicsWrapperCurrentNative(*owned))return false;
    auto& w=*owned;auto& h=w.sourcePhysics;if(h.descriptorAttempted||h.descriptor0C)return false;
    std::array<std::uint32_t,4> origin,forward,up;
    if(!copySourceWorldStartDefaults(origin,forward)||!copySourceWorldUpDefault(up))return false;
    h.descriptorAttempted=true;pc=0x27b388;
    try{h.descriptor0C.reset(new ra_static_model_cold50LA::Owner::SourcePhysicsDescriptor27A650);}catch(...){h.descriptor0C.reset();}
    h.descriptorAllocationReturned=true;
    if(h.descriptor0C){auto& d=*h.descriptor0C;auto& f=d.common;
        const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
        pc=0x27a11c;f.vtable170=0x3d2760;
        pc=0x27a124;f.position110=vf0;f.vectorKnown|=1;
        pc=0x27a12c;f.look120=vf0;f.vectorKnown|=2;
        pc=0x27a134;f.up130=vf0;f.vectorKnown|=4;
        pc=0x27a13c;f.smokeOffset150=vf0;f.vectorKnown|=8;
        pc=0x27a144;f.smokeVelocity160=vf0;f.vectorKnown|=16;
        pc=0x27a1f0;std::memcpy(f.name00.data(),"none",4);
        pc=0x27a1fc;f.name00[4]=0;f.name00Known=5;
        pc=0x27a210;f.position110=origin;pc=0x27a21c;f.look120=forward;pc=0x27a228;f.up130=up;
        pc=0x27a22c;f.useLighting140=1;pc=0x27a230;f.dynamic144=0;pc=0x27a234;f.shadow148=1;pc=0x27a238;f.smoker14C=0;
        pc=0x27a244;f.smokeOffset150=origin;pc=0x27a250;f.smokeVelocity160=origin;
        pc=0x27a66c;f.vtable170=0x3d2740;
        pc=0x27a674;d.vector2B0=vf0;d.vectorKnown=true;
        pc=0x27a680;f.name180[0]=0;f.name180Known=1; //Actual empty literal3B9AE0, NOT common name00.
        pc=0x27a6a4;d.vector2B0=vf0;
        static constexpr std::uint8_t order[11]={0,1,2,3,4,5,6,8,7,9,10};
        for(unsigned i=0;i<11;++i){pc=0x27a6a8+4*i;d.scalar284[order[i]]=0;d.scalarKnown|=std::uint16_t(1u<<order[i]);}
        f.constructed=true;
    }
    pc=0x27b3a0;h.descriptorPublished=true; //Actual nullable0C publication, no counted Entity.
    pc=0x27b3a8;if(!h.descriptor0C)return false; //NULL descriptor170 read.
    auto& f=h.descriptor0C->common;auto& config=*sourceWorldConfig_;
    a::OriginalStandardTextureDiagnostic50LA diagnostic;bool returned=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept {
        if(!config.readSourceStaticModelField158240(key,value,found,copy,diagnostic)){
            pc=diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_;return false;}return true;
    };
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};std::array<char,260> slt;
    h.descriptorReadEntered=true;
    //3D2740+8 is27A270. Specialised27A6F0 is+0C and is NOT called here.
    pc=0x27a2a4;if(!read.text("slt","none",slt.data(),slt.size(),returned))return false;
    pc=0x27a2b4;std::size_t count=0;while(slt[count]&&slt[count]!='.')++count;if(count>50)count=50;
    pc=0x27a2e0;std::memcpy(f.name00.data(),slt.data(),count);
    pc=0x27a2ec;f.name00[count]=0;f.name00Known=count+1;
    pc=0x27a304;if(!read.vector("position",origin,&f.position110,returned))return false;
    pc=0x27a320;if(!read.vector("lookvector",forward,&f.look120,returned))return false;
    pc=0x27a33c;if(!read.vector("upvector",up,&f.up130,returned))return false;
    std::int32_t value;
    pc=0x27a354;if(!read.integer("uselighting",1,&value,returned))return false;f.useLighting140=value;
    pc=0x27a36c;if(!read.integer("usedynamiclighting",0,&value,returned))return false;f.dynamic144=value;
    pc=0x27a384;if(!read.integer("issmoker",0,&value,returned))return false;f.smoker14C=value;
    pc=0x27a3a0;if(!read.vector("smokeoffset",origin,&f.smokeOffset150,returned))return false;
    pc=0x27a3bc;if(!read.vector("smokevelocity",origin,&f.smokeVelocity160,returned))return false;
    f.loaded=true;pc=0x27a3d4;h.descriptorReturnV0=1;h.descriptorReadReturned=true;
    pc=0x27b3c8;return true; //Only descriptor wrapper return, never27B3D0 or27B600.
}
}
