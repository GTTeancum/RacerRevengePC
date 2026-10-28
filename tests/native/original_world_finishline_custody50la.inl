// Included by the existing World implementation TU; no new translation unit.
#include "../../src/game/original_finishline_numeric50la.h"
namespace ra_pod_scene50LA {
bool Owner::canRetireNativeFinishLine540()const noexcept {
    if(!nativeFinishLine540_)return true;
    if(!sceneApplication_||sceneApplication_->attachmentLock423F58())return false;
    const auto& f=*nativeFinishLine540_;
    if(f.nativeStartupSentinel4342D0!=std::array<float,4>{{0.f,0.f,0.f,1.f}}||!f.table||!f.table->prepared())return false;
    if(!f.strip)return !f.counted&&!f.setupStarted&&!f.setupComplete&&!f.complete;
    const auto& s=*f.strip;const auto& e=s.entity_;
    if(!f.counted||e.kind!=g::OriginalWreckComponentKind50LA::strip23BCA0||e.objectKey!=key(&e)||
       e.sessionGeneration!=sceneGeneration_||e.reference00!=1u||e.category28!=0xffu||(e.flags29&15u)!=3u||
       e.nativeFlags29||e.parent1C||e.firstChild18||e.previous10||e.next14||e.mask20||e.descendants24||
       !sceneApplication_->entityAuthority_.canRetire(e)||!sceneApplication_->objectCount423F60())return false;
    if(s.primitive30_&&!s.primitive30_->reference.canRelease299AC0())return false;
    if(f.setupComplete&&(!f.setupStarted||!s.primitive30_||!s.primitive30_->reference.retained()||
       s.format3C_!=7u||s.count34_!=1u||s.field40_!=0u||!s.groups38_||s.groups38_[0]!=4u||
       s.color44_!=0x00ffffffu||s.color70_!=std::array<float,4>{{127.f,127.f,127.f,64.f}}||
       s.primitive30_->byte0C!=7u||s.primitive30_->attributes10!=&s.attributes_||
       s.primitive30_->vertices20!=4u||s.primitive30_->groups22!=1u))return false;
    return !f.complete||(f.setupComplete&&f.anchorsWritten&&f.material544&&*f.material544>=0&&!f.terminal);
}
bool Owner::retireNativeFinishLine540()noexcept {
    if(!nativeFinishLine540_)return true;
    if(!canRetireNativeFinishLine540())return false;
    auto& f=*nativeFinishLine540_;
    if(f.strip){auto& s=*f.strip;
        if(!sceneApplication_->entityAuthority_.retire(s.entity_))std::terminate();
        --s.entity_.reference00;
        //Actual standalone142590→23BCF0 resources, then1503B0 Entity base.
        //No later1B4D0C parent/mask8 attachment has been enabled here.
        s.attributes_.release23BCF0();s.groups38_.reset();s.primitive30_.reset();
        --sceneApplication_->objectCount423F60();f.strip.reset();
    }
    //544 was only an index. No spurious material release for its lookup.
    nativeFinishLine540_.reset();return true;
}
bool Owner::setupNativeFinishLine540(std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> table,
    const std::string& directory,std::uint32_t& pc)noexcept {
    pc=0x1b3ba4;
    if(!sceneApplication_||!table||!table->prepared()||sceneApplication_->attachmentLock423F58())return false;
    try {
        if(!nativeFinishLine540_){nativeFinishLine540_=std::make_unique<NativeFinishLine540>();nativeFinishLine540_->table=table;}
        auto& f=*nativeFinishLine540_;
        if(f.table!=table||f.terminal||!canRetireNativeFinishLine540())return false;
        if(f.setupComplete)return true;
        if(f.setupStarted)return false;
        f.terminal=true; //Entered effects never replay after partial failure.
        if(!f.strip){
            using Strip=g::OriginalPodCableStripCold50LA;
            auto strip=std::make_unique<Strip>();auto& e=strip->entity_;
            auto& authority=sceneApplication_->entityAuthority_;auto& count=sceneApplication_->objectCount423F60();
            pc=0x1b3bc0;
            if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
               !authority.canEnroll(e,key(&e)))return false;
            if(g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::strip23BCA0,
                sceneGeneration_,key(&e),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
               !authority.enroll(e))std::terminate();
            strip->flags48_=1;f.strip=std::move(strip);f.counted=true; //Real540 publication1B3BC8.
        }
        auto& strip=*f.strip;f.setupStarted=true;
        //Actual+74=23BEB0→150450, not a cable eighteen-vertex substitution.
        pc=0x150498;strip.format3C_=7;strip.count34_=1;strip.groups38_.reset(new std::uint32_t[1]);
        strip.field40_=0;strip.color44_=0x00ffffffu;strip.flags48_|=1u;
        strip.color70_=std::array<float,4>{{127.f,127.f,127.f,64.f}}; //23D750 ignores packed alpha for W.
        strip.groups38_[0]=4; //Actual382C38 integerword4, not float4.
        pc=0x1504f0;strip.primitive30_=std::make_unique<g::OriginalPodCableStripCold50LA::Primitive>();
        rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
        if(!strip.primitive30_->reference.acquireNamed299BE0(table,directory,"finishline",diagnostic)){
            pc=diagnostic.sourcePC;f.failurePC=pc;return false;}
        strip.primitive30_->byte0C=7;
        strip.attributes_.constructQuad23BEB0(7,pc);
        strip.primitive30_->attributes10=&strip.attributes_;strip.primitive30_->flags0F&=static_cast<std::uint8_t>(~1u);
        strip.primitive30_->vertices20=4;strip.primitive30_->groups22=1;
        f.setupComplete=true;f.terminal=false;pc=0x1b3bfc;return true;
    }catch(...){if(nativeFinishLine540_){nativeFinishLine540_->terminal=true;nativeFinishLine540_->failurePC=pc;}return false;}
}
bool Owner::commitNativeFinishLineGeometry1B3DDC(std::uint32_t& pc)noexcept {
    pc=0x1b3ddc;
    if(!nativeFinishLine540_||!canRetireNativeFinishLine540())return false;
    auto& f=*nativeFinishLine540_;
    if(!f.setupComplete||!f.anchorsWritten||!f.material544||f.complete||f.terminal)return false;
    f.terminal=true;
    //Explicit bounded native atomic numeric admission before source setter
    //publication. No partial retail numeric-failure stores are claimed.
    g::finishline_numeric50LA::Geometry geometry;
    if(!g::finishline_numeric50LA::prepare1B3DDC(f.anchorXYZ[0],f.anchorXYZ[1],geometry,pc)){
        f.failurePC=pc;return false;}
    auto& attributes=f.strip->attributes_;
    constexpr std::uint32_t positionPC[]{0x1b3df0,0x1b3e20,0x1b3e3c,0x1b3e6c};
    constexpr std::uint32_t normalPC[]{0x1b3fa8,0x1b3fc4,0x1b3fe0,0x1b3ffc};
    constexpr std::uint32_t uvPC[]{0x1b401c,0x1b4040,0x1b4064,0x1b4088};
    //23C090/23C260 only XYZ. Do not manufacture source vector W or stripControl.
    for(std::size_t v=0;v<4u;++v){pc=positionPC[v];for(std::size_t k=0;k<3u;++k)
        attributes.vertices_->positions[v].xyz[k]={geometry.positions[v][k],0xffu};}
    for(std::size_t v=0;v<4u;++v){pc=normalPC[v];for(std::size_t k=0;k<3u;++k)
        attributes.vertices_->triples[v].components[k]={geometry.normals[v][k],0xffu};}
    for(std::size_t v=0;v<4u;++v){pc=uvPC[v];attributes.setTexcoord23C800(static_cast<std::uint32_t>(v),geometry.uv[v][0],geometry.uv[v][1]);}
    f.complete=true;f.terminal=false;pc=0x1b4090;return true;
}
}
namespace ra_cold_aggregate50LA {
bool Owner::continueNativeFinishLine1B3B80(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa208;
    //Only the actual+11C continuation calls this, AFTER its same coldPhase1
    //Camera/Display/context/idle and both retained-publication checks.
    if(!nativeStandardServiceComplete_||nativeStandardServiceTerminal_||!nativeBuoyCount538_||
       *nativeBuoyCount538_!=0||!world_||!world_->modelPrefixes_||!world_->domain_||
       !source_||!nativeSceneInfo270900Current(app,pc))return false;
    //1B3AF8 null53C returns; nonnull53C reaches538==0 loop test before any
    //element access. Both source branches have no effect.53C remains UNKNOWN.
    nativeBuoyAttachNoEffect1B3AE0_=true;pc=0x1aa210;
    if(nativeFinishLine540_&&nativeFinishLine540_->complete){
        if(!canRetireNativeFinishLine540())return false;
        return continueNativeCollisionObjects2743B0(app,pc); //Same explicit native progress policy, not observed guest330/+30.
    }
    if(nativeFinishLine540_&&nativeFinishLine540_->terminal){pc=nativeFinishLine540_->failurePC;return false;}
    try {
        const auto table=world_->domain_->materials382B38();
        if(!table||app.standardMaterialDomain()!=world_->domain_||!world_->context_->live())return false;
        if(!setupNativeFinishLine540(table,app.materialDirectory36B970(),pc)){
            if(nativeFinishLine540_)nativeFinishLine540_->failurePC=pc;return false;}

        auto& f=*nativeFinishLine540_;f.terminal=true;
        rr::assets::OriginalStandardMaterialDiagnostic50LA material;
        pc=0x1b3c0c;
        std::int32_t row=-1;
        if(!table->find298C80("textures\\finishline",row,material)||row<0){
            f.failurePC=material.sourcePC?material.sourcePC:pc;return false;}
        f.material544=row; //1B3C14 numeric index ONLY; no second reference.
        auto& models=*world_->modelPrefixes_;
        if(!models.canClose()||models.parents_.size()!=10u||!models.parents_[0]||
           models.parents_[0]->count30!=467u||models.parents_[0]->entries34.size()!=467u){f.failurePC=pc;return false;}
        const auto* data=source_->sourcesDecoded();if(!data){f.failurePC=pc;return false;}
        const auto equal=[](const std::string& left,const char* right)noexcept {
            std::size_t i=0;for(;i<left.size()&&right[i];++i){auto a=left[i],b=right[i];
                if(a>='A'&&a<='Z')a=static_cast<char>(a+'a'-'A');if(b>='A'&&b<='Z')b=static_cast<char>(b+'a'-'A');
                if(a!=b)return false;}return i==left.size()&&!right[i];
        };
        std::array<unsigned,2> matches{};
        std::array<std::array<float,3>,2> actual{{
            {{f.nativeStartupSentinel4342D0[0],f.nativeStartupSentinel4342D0[1],f.nativeStartupSentinel4342D0[2]}},
            {{f.nativeStartupSentinel4342D0[0],f.nativeStartupSentinel4342D0[1],f.nativeStartupSentinel4342D0[2]}}}};
        //Actual first3C4 list only, preserving source scan order. Unknown
        //retail wrappers are not synthesized: prior+11C now owns every slot.
        for(std::size_t ordinal=0;ordinal<467u;++ordinal){
            auto& slot=models.parents_[0]->entries34[ordinal];pc=0x1b3c30;
            if(!models.nativeStandardSlotCurrent27ABC0(0,ordinal)){f.failurePC=pc;return false;}
            auto* wrapper=models.nativeStandardWrapper(slot);
            if(!wrapper||!wrapper->model8)continue; //Genuine admitted negative-load null.
            if(!slot.model||wrapper->model8!=&slot.model->scene){f.failurePC=pc;return false;}
            auto& model=*slot.model;const auto* scene=model.scene.model();
            if(!scene||model.scene.released()){f.failurePC=pc;return false;}
            const int which=equal(scene->name38,"finish_droidleft.slt")?0:
                equal(scene->name38,"finish_droidright.slt")?1:-1;
            if(which<0)continue;
            if(slot.worldInstance!=465u+static_cast<unsigned>(which)||
               model.source.worldInstance!=slot.worldInstance||slot.worldInstance>=data->world.instances.size()||
               model.node.parent180!=g::SceneNodeIndex49P::none||!model.placed||!model.parentAttached){f.failurePC=pc;return false;}
            pc=which?0x1b3cb0:0x1b3c7c;
            //Actual28B1A0(a1=null): same real ready-cache/parentless graph.
            //If dirty, copy localC0→world100 and set ready bit; NEVER reparse
            //SCB or replace it with a separately calculated world transform.
            if(!g::refreshSceneWorld49P(model.graph(),static_cast<g::SceneNodeIndex49P>(0))){f.failurePC=pc;return false;}
            for(std::size_t k=0;k<3u;++k)actual[which][k]=model.node.world100.v[12u+k];
            ++matches[which]; //Actual corpus requires exactly one of each, not arbitrary last wins.
        }
        pc=0x1b3ce8;
        const std::array<float,3> sentinel{{f.nativeStartupSentinel4342D0[0],f.nativeStartupSentinel4342D0[1],f.nativeStartupSentinel4342D0[2]}};
        if(matches!=std::array<unsigned,2>{{1u,1u}}||actual[0]==sentinel||actual[1]==sentinel){f.failurePC=pc;return false;}
        //Both genuine authenticated anchors reach positive branch; no claim
        //that guest4342D0 retained immutable frontend state. Private native
        //startup-value domain is an explicit adaptation of3C9CCC's producer.
        f.anchorXYZ=actual;f.anchorsWritten=true;f.terminal=false;
        if(!commitNativeFinishLineGeometry1B3DDC(pc))return false;
        //1B40A4 represented native-adapted return.1AA218 optional progress
        //uses the SAME bounded native progress policy as earlier330 joins;
        //do not claim actual330/+30 read, null/default, or1CC900 execution.
        return continueNativeCollisionObjects2743B0(app,pc); //No update/draw/fullWorld.
    }catch(...){if(nativeFinishLine540_){nativeFinishLine540_->terminal=true;nativeFinishLine540_->failurePC=pc;}return false;}
}
}
