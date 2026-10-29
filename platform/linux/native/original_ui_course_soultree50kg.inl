// Distinct actual Course thirteen-row Soultree_Character cohort, NOT CPU projections.
    struct CourseAnimationRecord {
        std::array<char,50> name{}; // Safe backing; nameKnown is the actual producer ledger.
        game::OriginalRacerSceneObject50LA* scene34=nullptr;
        bool translation38=false,listed3C=false,nodeFieldsKnown=false,nameKnown=false;
    };
    struct CourseSoultree {
        C entity;
        const C* manager30=nullptr;
        std::uint32_t type34=0,field38=0,field3C=0,field40=0,field44=0,field48=0,field4C=0;
        std::int32_t field50=0,field54=0;std::uint32_t field58=0;
        std::array<float,4> position70{{1,1,1,1}},look80{{0,0,0,1}},up90{{0,0,0,1}};
        std::array<std::optional<std::string>,3> motionNames; //1A0,2A0,A0; ctor does NOT write these.
        std::int32_t randomCount3A0=0;const void* randomNames3A4=nullptr;
        std::unique_ptr<OriginalUiCourseManagerConfig50KG> widgetMcf,animationMcf;
        bool widgetMcfAllocated=false,animationMcfAllocated=false;
        std::string path;
        std::unique_ptr<game::OriginalRacerAnimationCold50LA> animation60;
        const game::OriginalRacerAnimationCold50LA* sourceAnimation60=nullptr;
        std::shared_ptr<assets::OriginalStandardTextureResolver50LA> files;
        assets::OriginalStandardModelFileId50LA mcfId;
        bool cachePublished=false,animationEnrolled=false,animationAttached=false;
        CourseSoultreeModel model;
        std::unique_ptr<game::OriginalMotionConfiguration50LA> motionConfiguration;
        assets::MotionNodeBindings49O bindings{};assets::MotionView49O motionView{};
        std::shared_ptr<game::OriginalMotionLibrary50LA> motionLibrary;
        std::unique_ptr<const game::OriginalMotionResource50LA*[]> motionSlots1E4;
        const game::OriginalMotionResource50LA* retainedMotion=nullptr;
        bool motionAttempted=false,motionRetained=false,motionSlotWritten=false,motionReleased=false;
        bool animationPayloadReleased=false;
        std::unique_ptr<CourseAnimationRecord[]> records1EC;
        std::size_t recordsProduced=0;
        std::unique_ptr<game::BodyMatrix48G[]> saved1F4;
        std::size_t savedProduced=0;
        bool constructed=false,loaderReturned=false,animationRetiring=false,animationReleased=false;
    };
    std::array<std::unique_ptr<CourseSoultree>,84> courseSoultreeRows_;
    std::array<bool,84> courseSoultreeAttempted_{};
    std::array<std::string,84> courseSoultreeFactoryPathStack50_;
    std::array<bool,84> courseSoultreeFactoryPathKnown_{};
    struct CourseSoultreeSourceRow {
        std::size_t ordinal;
        const char* stem;
        const char* mcf;
        const char* slt;
        const char* vut;
        const char* psg;
        const char* mot;
    };
    // Exact authored cohort: sparse ordinals deliberately exclude Empty rows.
    // These are admission identities, never replacement source Config values.
    inline static constexpr std::array<CourseSoultreeSourceRow,13> courseSoultreeSourceRows_{{
        {8,"holo_ta","holo_ta.mcf","holo_ta.slt","holo_ta.vut","ui\\anim\\holo_ta.psg","UI\\Anim\\holo_ta.MOT"},
        {9,"holo_ga","holo_ga.mcf","holo_ga.slt","holo_ga.vut","ui\\anim\\holo_ga.psg","UI\\Anim\\holo_ga.MOT"},
        {11,"holo_ra","holo_ra.mcf","holo_ra.slt","holo_ra.vut","ui\\anim\\holo_ra.psg","UI\\Anim\\holo_ra.MOT"},
        {12,"holo_sa","holo_sa.mcf","holo_sa.slt","holo_sa.vut","ui\\anim\\holo_sa.psg","UI\\Anim\\holo_sa.MOT"},
        {13,"holo_ma","holo_ma.mcf","holo_ma.slt","holo_ma.vut","ui\\anim\\holo_ma.psg","UI\\Anim\\holo_ma.MOT"},
        {14,"holo_tb","holo_tb.mcf","holo_tb.slt","holo_tb.vut","ui\\anim\\holo_tb.psg","UI\\Anim\\holo_tb.MOT"},
        {15,"holo_gb","holo_gb.mcf","holo_gb.slt","holo_gb.vut","ui\\anim\\holo_gb.psg","UI\\Anim\\holo_gb.MOT"},
        {19,"holo_mb","holo_mb.mcf","holo_mb.slt","holo_mb.vut","ui\\anim\\holo_mb.psg","UI\\Anim\\holo_mb.MOT"},
        {20,"holo_tc","holo_tc.mcf","holo_tc.slt","holo_tc.vut","ui\\anim\\holo_tc.psg","UI\\Anim\\holo_tc.MOT"},
        {21,"holo_gc","holo_gc.mcf","holo_gc.slt","holo_gc.vut","ui\\anim\\holo_gc.psg","UI\\Anim\\holo_gc.MOT"},
        {22,"holo_rc","holo_rc.mcf","holo_rc.slt","holo_rc.vut","ui\\anim\\holo_rc.psg","UI\\Anim\\holo_rc.MOT"},
        {24,"holo_sc","holo_sc.mcf","holo_sc.slt","holo_sc.vut","ui\\anim\\holo_sc.psg","UI\\Anim\\holo_sc.MOT"},
        {25,"holo_mc","holo_mc.mcf","holo_mc.slt","holo_mc.vut","ui\\anim\\holo_mc.psg","UI\\Anim\\holo_mc.MOT"},
    }};
    static const CourseSoultreeSourceRow* courseSoultreeSourceRow(std::size_t i)noexcept {
        for(const auto& row:courseSoultreeSourceRows_)if(row.ordinal==i)return &row;
        return nullptr;
    }
    static bool courseSoultreeOrdinalAdmitted(std::size_t i)noexcept{return courseSoultreeSourceRow(i)!=nullptr;}
    static std::size_t previousCourseSoultreeOrdinal(std::size_t i)noexcept {
        std::size_t previous=84;
        for(const auto& row:courseSoultreeSourceRows_){if(row.ordinal==i)return previous;previous=row.ordinal;}
        return 84;
    }
    bool courseSoultreeMemberCurrent(const assets::OriginalUiMemberView50KG& member)const noexcept {
        if(!member.identity||!member.data||!issuer_||!issuer_->standardMaterialDomainOwner_)return false;
        const auto provider=issuer_->standardMaterialDomainOwner_->archiveProvider3E1A70();
        assets::OriginalStandardArchiveProvider50LA::MemberView winner;
        return provider&&provider->sealed()&&provider->lookup132820(member.identity->name,winner)==
            assets::OriginalStandardArchiveProvider50LA::Lookup::present&&winner.data&&winner.size==member.size&&
            !std::memcmp(winner.data,member.data,member.size);
    }
    static std::string courseSoultreeLower(std::string text){for(auto& c:text)if(c>='A'&&c<='Z')c=char(c+32);return text;}
    bool loadCourseSoultreeCold153CA0(std::size_t i,Diagnostic& d)noexcept {
        if(!courseSoultreeOrdinalAdmitted(i)||!courseSoultreeRows_[i]||!courseSoultreeRows_[i]->animationEnrolled||!construction_||construction_->courseSoultreeRows_[i].animation!=courseSoultreeRows_[i]->animation60.get())return false;
        auto& s=*courseSoultreeRows_[i];auto& f=s.animation60->fields_;
        const auto fail=[&](std::uint32_t pc)noexcept{d.sourcePC=pc;return false;};
        try {
            // Actual153CB8 reselects General info, replacing the selected cache
            // even though the prior153B54 selected the same named section.
            if(!s.animationMcf->selectWidgetsSection157D00("General info",d.config))return fail(d.config.sourcePC);
            const auto* general=s.animationMcf->config_->records_.section("General info");
            if(!s.animationMcf->selectedCurrent("General info")||!general)return fail(0x153CB8);
            const auto* directory=general->find("ContentDirectory");f.directory190=directory?directory->text:"";
            if(f.directory190.empty()){
                if(!issuer_->animationDirectory36AED0_)return fail(0x153CF8);
                f.directory190=*issuer_->animationDirectory36AED0_;
            }
            const auto* vut=general->find("VUTFilename");f.model98=vut?vut->text:"\\";
            const auto* mirror=general->find("MIRFilename");f.motionE8=mirror?mirror->text:"\\";
            if(!assets::originalUiInteger50KG(*general,"NumberOfMotions",0,f.motionCount14,d.config,0x153D50))return fail(0x153D50);
            std::int32_t value=1;
            if(!assets::originalUiInteger50KG(*general,"InterpolatePosition",1,value,d.config,0x153D68))return fail(0x153D68);f.field20=std::uint32_t(value);
            if(!assets::originalUiInteger50KG(*general,"InterpolateRotation",1,value,d.config,0x153D80))return fail(0x153D80);f.field24=std::uint32_t(value);
            const auto* rotation=general->find("RotationType");
            if(!rotation||rotation->text=="BYTEAXIS")f.field28=0;
            else if(rotation->text=="SHORTAXIS")f.field28=1;
            else return fail(0x153DF4); // No substitute accepted for unknown source assertion/print branch.
            const auto* position=general->find("PositionType");
            if(!position||position->text=="BEZIER")f.field2C=1;
            else if(position->text=="LINEAR")f.field2C=0;
            else return fail(0x153E74);
            if(general->find("MirrorAxis"))return fail(0x153E94); // Present branch compares stale stack20, not the newstack60; unjoined.
            f.field0C=0;
            if(f.directory190!="UI\\Anim\\"||f.rig138!=courseSoultreeSourceRow(i)->slt||f.model98!=courseSoultreeSourceRow(i)->vut||
               f.motionE8!="\\"||f.motionCount14!=1||f.field28!=1)return fail(0x153F14);
            s.motionConfiguration=std::make_unique<game::OriginalMotionConfiguration50LA>();
            for(const auto& section:s.animationMcf->config_->records_.sections())for(const auto& item:section.values)
                if(item.sourceValuePresent)(*s.motionConfiguration)[courseSoultreeLower(section.name)].emplace(courseSoultreeLower(item.key),item.text);
            if(!loadCourseSoultreeModel154140(i,s.model,d))return false;
            auto& m=s.model;const auto graph=m.graph();
            // Actual recursive28C270(model) then minus1 at15435C. Validate the
            // retained graph instead of replacing source traversal with catalog row count.
            if(!game::validateSceneGraph49P(graph))return fail(0x154350);
            std::array<bool,assets::maxModelNodes49O+1u> seen{};std::uint32_t visited=0;
            const auto visit=[&](auto&& self,game::SceneNodeIndex49P index)->bool {
                const auto n=static_cast<std::int32_t>(index);if(n<0||std::uint32_t(n)>=graph.count||seen[std::size_t(n)])return false;
                seen[std::size_t(n)]=true;++visited;
                for(auto child=graph.nodes[n].firstChild184;child!=game::SceneNodeIndex49P::none;child=graph.nodes[static_cast<std::size_t>(child)].nextSibling188)
                    if(!self(self,child))return false;
                return true;
            };
            if(!visit(visit,m.scene->model()->sceneIndex)||visited!=3)return fail(0x154350);
            f.nodeCount1E8=std::int32_t(visited-1);
            s.records1EC.reset(new(std::nothrow) CourseAnimationRecord[std::size_t(*f.nodeCount1E8)]); //154364 malloc1E8<<6 semantic records.
            f.allocation1EC=s.records1EC.get(); //15436C actual pointer publication, includingNULL.
            if(!s.records1EC)return fail(0x152EA0); // Stop before first record store.
            assets::MotionBindingDiagnostic49O bindingDiagnostic;
            if(!assets::readMotionNodeEnumeration49O(m.hierarchy,s.bindings,bindingDiagnostic)||s.bindings.count!=*f.nodeCount1E8)
                return fail(0x152EA0);
            //152EA0 walks actual scene references, publishes34 and clears38/3C
            // BEFORE uppercasing that SAME scene.name38 in place and copying it.
            for(std::uint32_t row=0;row<s.bindings.count;++row){
                const auto original=s.bindings.rows[row].sceneNode;
                if(original>=m.scene->sourceRowCount())return fail(0x152EC8);
                auto* actual=m.scene->sourceRowObject(original);if(!actual)return fail(0x152EC8);
                auto& record=s.records1EC[row];record.scene34=actual;record.translation38=false;record.listed3C=false;record.nodeFieldsKnown=true;
                for(auto& ch:actual->name38)if(ch>='a'&&ch<='z')ch=char(ch-32); //152F00 actual inplace store.
                const auto length=std::min<std::size_t>(49,actual->name38.size());
                std::memcpy(record.name.data(),actual->name38.data(),length);record.name[length]=0;record.nameKnown=true;
                s.bindings.rows[row].sceneNode=static_cast<std::uint8_t>(actual->sceneIndex);
                ++s.recordsProduced; // Each reached record survives later failure.
            }
            assets::OriginalStandardArchiveProvider50LA::MemberView mirrorMember;
            if(issuer_->standardMaterialDomainOwner_->archiveProvider3E1A70()->lookup132820(f.directory190+f.motionE8,mirrorMember)!=
               assets::OriginalStandardArchiveProvider50LA::Lookup::absent)return fail(0x153534);
            const auto vutMember=loaderConfig_->archive_->find(f.directory190+f.model98);
            assets::MotionNodeBindings49O parsedBindings{};
            if(!courseSoultreeMemberCurrent(vutMember)||!assets::readMotionNodeBindings49O(m.hierarchy,
                reinterpret_cast<const char*>(vutMember.data),vutMember.size,parsedBindings,bindingDiagnostic)||parsedBindings.count!=s.bindings.count)
                return fail(0x153218);
            // Source1530E0 modifies existing same record names/flags, not the
            // node pointers established152EA0. Native parser is admitted first.
            for(std::uint32_t row=0;row<s.bindings.count;++row){
                const auto node=s.bindings.rows[row].sceneNode;s.bindings.rows[row]=parsedBindings.rows[row];s.bindings.rows[row].sceneNode=node;
                const auto length=std::strlen(s.bindings.rows[row].name);
                if(length>=50)return fail(0x153218);
                std::memcpy(s.records1EC[row].name.data(),s.bindings.rows[row].name,length+1);
                s.records1EC[row].translation38=s.bindings.rows[row].translationFlag38;
                s.records1EC[row].listed3C=s.bindings.rows[row].listedFlag3c;
            }
            f.field1F8=1; //153424 after successful actual VUT parser/close branch only.
            if(!finishCourseSoultreeModelPath1543A8(i))return fail(0x1543A8);
            s.saved1F4.reset(new(std::nothrow) game::BodyMatrix48G[std::size_t(*f.nodeCount1E8)]); //155754 malloc, not vector completion.
            f.allocation1F4=s.saved1F4.get(); //15575C, includesNULL before first28B520 copy.
            if(!s.saved1F4)return fail(0x15577C);
            for(std::uint32_t row=0;row<s.bindings.count;++row){
                const auto* actual=s.records1EC[row].scene34;if(!actual)return fail(0x155778);
                const auto index=static_cast<std::uint32_t>(actual->sceneIndex);if(index>=graph.count)return fail(0x155778);
                // Actual28B520 is eight two-word copies of this SAME localC0.
                s.saved1F4[row]=graph.nodes[index].localC0;++s.savedProduced;
            }
            game::OriginalMotionDefaults50LA defaults;defaults.directory190=f.directory190;
            defaults.interpolatePosition20=static_cast<std::int32_t>(f.field20);defaults.interpolateRotation24=static_cast<std::int32_t>(f.field24);
            defaults.rotationType28=f.field28;defaults.positionType2C=f.field2C;
            const auto resolve=[&](const std::string& name,game::OriginalAdmittedMotion50LA& out){
                // Invoked by genuine151270 only on a real MotionLibrary miss,
                // AFTER153650 array publication. No speculative future-MOT failure.
                if(!game::original_motion_detail50LA::equal308F00(name,courseSoultreeSourceRow(i)->mot))return false;
                const auto mot=loaderConfig_->archive_->find(name);assets::GeometryDiagnostic49O motionDiagnostic;
                if(!courseSoultreeMemberCurrent(mot)||!assets::readMot49O(mot.data,mot.size,s.motionView,motionDiagnostic))return false;
                out.motion=&s.motionView;out.originalMemberName=mot.identity->name;return true;
            };
            s.motionLibrary=issuer_->motionLibrary4333C0();if(!s.motionLibrary||s.motionAttempted||f.motionCount14!=1||f.field1E0!=1)return fail(0x153610);
            s.motionAttempted=true;f.motion18=nullptr; //153624 before the positive-count branch.
            s.motionSlots1E4.reset(new(std::nothrow) const game::OriginalMotionResource50LA*[1]); //153648 actual malloc.
            f.motionArray1E4=s.motionSlots1E4.get(); //153650 NULL is published but DOES NOT skip151270/retain.
            //15366C formats MOTION 1;151298 selects it BEFORE NAME lookup and
            // the MotionLibrary cache lookup1512C8, including warm library hits.
            // This follows actual153650 nullable array publication above.
            if(!s.animationMcf->selectWidgetsSection157D00("MOTION 1",d.config))return fail(d.config.sourcePC);
            const auto selected=s.motionConfiguration->find("motion 1");
            if(!s.animationMcf->selectedCurrent("MOTION 1")||selected==s.motionConfiguration->end())return fail(0x1512B8);
            const game::OriginalMotionResource50LA* resource=nullptr;
            const auto loaded=s.motionLibrary->load151270(selected->second,defaults,resolve,resource);
            if(!loaded.okay())return fail(loaded.sourcePC);if(!resource)return fail(0x153688); // No fake unwritten successful slot.
            if(resource->fields_.referenceD8<0||std::uint32_t(resource->fields_.referenceD8)<resource->publishedSlotReferences_||
               s.motionLibrary->slotOwners_==std::numeric_limits<std::size_t>::max())return fail(0x153690);
            const auto retained=s.motionLibrary->retain153610(resource);if(!retained.okay())return fail(retained.sourcePC);
            s.retainedMotion=resource;s.motionRetained=true;
            // Native lifetime guards cover this real D8 contribution even if
            // sourceNULL1E4 prevents publication below. No extra source retain.
            auto* actual=s.motionLibrary->mutableResource(resource);if(!actual)std::terminate();
            ++actual->publishedSlotReferences_;++s.motionLibrary->slotOwners_;
            if(!s.motionSlots1E4)return fail(0x1536B0);
            s.motionSlots1E4[0]=resource;s.motionSlotWritten=true;
            for(std::uint32_t row=0;row<s.bindings.count;++row)
                if(!game::setLocalMatrix49P(graph,static_cast<game::SceneNodeIndex49P>(s.bindings.rows[row].sceneNode),s.saved1F4[row]))return fail(0x1556C0);
            //153C6C destroys actual Config, retaining source08 historical address.
            if(!s.animationMcf->closeHeap1573B0(1,d.config))return fail(d.config.sourcePC?d.config.sourcePC:0x153C6C);
            s.motionConfiguration.reset();
            // Actual141C20 at153C74 is jrra/movev0,a0, no hidden callback.
            return true;
        }catch(...){return fail(0x153CA0);}
    }
    bool openCourseSoultreeAnimation1538B0(std::size_t i,Diagnostic& d)noexcept {
        if(!courseSoultreeOrdinalAdmitted(i)||!courseSoultreeRows_[i]||!courseSoultreeRows_[i]->animationEnrolled||!construction_||construction_->courseSoultreeRows_[i].animation!=courseSoultreeRows_[i]->animation60.get())return false;
        auto& s=*courseSoultreeRows_[i];auto& a=*s.animation60;auto& f=a.fields_;
        const auto fail=[&](std::uint32_t pc)noexcept{d.sourcePC=pc;return false;};
        f.field18C=1; //1538C8 a3=1, before resolver lookup.
        const auto member=loaderConfig_->archive_->find(s.path);
        if(!courseSoultreeMemberCurrent(member))return fail(0x153998);
        s.files=issuer_->standardMaterialDomainOwner_->resolver3E1A70();
        assets::OriginalStandardTextureDiagnostic50LA cache;
        try {
            if(!s.files||!s.files->bindModelMember(member.identity->name,assets::OriginalStandardModelFileKind50LA::mcf,
                std::vector<std::uint8_t>(member.data,member.data+member.size),s.mcfId,cache))return fail(cache.sourcePC?cache.sourcePC:0x153998);
            assets::OriginalStandardModelFileView50LA found;
            if(!s.files->modelFile(s.mcfId,found,cache))return fail(cache.sourcePC);
            // No optional alias a2 on the actual2A8B24 call. Cold publishes the
            // same actual animation BEFORE Config allocation; warm does not overwrite it.
            if(!found.animation1536F0){if(!s.files->publishAnimation1326A0(s.mcfId,&a,cache))return fail(cache.sourcePC);s.cachePublished=true;}
            s.animationMcfAllocated=s.animationMcf->allocateAndConstruct157320(); //153A6C/7C.
            f.configuration08=s.animationMcf->config_.get(); //153A84 actual NULL or actual Config.
            f.definition48=s.path.substr(0,79); //153B2C and explicit terminator, before Configopen.
            if(!s.animationMcfAllocated)return fail(0x153B44);
            if(s.animationMcf->open1577D0(s.path,d.config)!=OriginalUiCourseManagerConfig50KG::OpenResult::opened)return fail(0x153B44);
            if(!s.animationMcf->selectWidgetsSection157D00("General info",d.config))return fail(d.config.sourcePC);
            const auto* general=s.animationMcf->config_->records_.section("General info");
            if(!s.animationMcf->selectedCurrent("General info")||!general)return fail(0x153B54);
            const auto* slt=general->find("SLTFile");if(!slt)return fail(0x153B8C);
            f.rig138=slt->text;f.rigOverride188=0;
            // Cached model/name custody is NOT inferred from a nonnull cache
            // pointer. Stop before its actual model04 read, retaining all reached
            // Config/definition/rig writes. Never silently choose the cold path.
            if(found.animation1536F0)return fail(0x153BA4);
            return loadCourseSoultreeCold153CA0(i,d);
        }catch(...){return fail(0x1538B0);}
    }
    bool loadCourseSoultree1CD160(std::size_t i,const assets::OriginalUiDatSection50KG& section,Diagnostic& d)noexcept {
        d.sourcePC=0x1CD274;
        if(thread_!=std::this_thread::get_id()||terminal_||!courseSoultreeOrdinalAdmitted(i)||courseSoultreeAttempted_[i]||
           !issuer_||!scope_||!construction_||!modeEffect_||!courseWidgetsConfig_||!courseWidgetsConfig_->parsedCurrent()||
           !loaderConfig_||!loaderConfig_->parsedCurrent()||courseWidgetOrdinal_!=i||loaderStopPC_!=0x1CD8A0||
           !courseWidgetOnTopKnown_||courseWidgetOnTopStack64C_||
           courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_)!=&section||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        const auto fail=[&](std::uint32_t pc)noexcept{d.sourcePC=pc;return false;};
        const auto* type=section.find("Type");if(!type||type->text!="Soultree_Character")return false;
        std::int32_t dynamic=0;
        if(!assets::originalUiInteger50KG(section,"DynamicLoad",0,dynamic,d.config,0x1CD298)||dynamic)return fail(0x1CD2B8);
        const auto* mcf=section.find("MCF");if(!mcf||mcf->text!=courseSoultreeSourceRow(i)->mcf)return fail(0x1CD2B0);
        if(!issuer_->animationDirectory36AED0_||*issuer_->animationDirectory36AED0_!="UI\\Anim\\")return fail(0x1CD318);
        try {courseSoultreeFactoryPathStack50_[i]=*issuer_->animationDirectory36AED0_+mcf->text;}
        catch(...){return fail(0x1CD318);}
        courseSoultreeFactoryPathKnown_[i]=true; // Actual1CD318 precedes1CD320, even when malloc returnsNULL.
        courseSoultreeAttempted_[i]=true;
        courseSoultreeRows_[i].reset(new(std::nothrow) CourseSoultree); // Actual1CD320 outer allocation3B0.
        if(!courseSoultreeRows_[i])return fail(0x1CD340); // NULL before virtualvtable8 read.
        auto& s=*courseSoultreeRows_[i];
        try {
            s.path=courseSoultreeFactoryPathStack50_[i]; // Native retained transport of already reached stack50.
            // Transport backing only; no Config construction/mount/lookup receipt.
            s.widgetMcf.reset(new OriginalUiCourseManagerConfig50KG);
            s.animationMcf.reset(new OriginalUiCourseManagerConfig50KG);
            if(!s.widgetMcf->admitBorrowedHeapTransportNative(*loaderConfig_)||
               !s.animationMcf->admitBorrowedHeapTransportNative(*loaderConfig_))return fail(0x2A85F0);
            d.entity=construction_->constructCourseSoultree2A85F0(i,s.entity);if(!d.entity.okay())return fail(d.entity.sourcePC);
            s.constructed=true;s.manager30=&scope_->manager_;
            s.field38=0;s.type34=0;s.field3C=0;s.field40=0;s.field44=0;s.field48=0;
            s.field4C=0;s.field50=0;s.field54=0;s.field58=0;
            s.sourceAnimation60=nullptr;s.position70={{1,1,1,1}};s.look80={{0,0,0,1}};s.up90={{0,0,0,1}};
            s.randomCount3A0=0;s.randomNames3A4=nullptr;
            const char* keys[]{"EnableOnSelect","DisableOnSelect","EnableOnDeSelect","DisableOnDeSelect"};
            std::uint32_t* fields[]{&s.field3C,&s.field40,&s.field44,&s.field48};
            for(std::size_t n=0;n<4;++n){bool value=false;const auto pc=std::uint32_t(0x2A87B4+24*n);
                // Exact authoredT/absent integer158B30 branch, produces1/0.
                if(!assets::originalUiBoolean50KG(section,keys[n],false,value,d.config,pc))return fail(pc);*fields[n]=value?1u:0u;}
            s.widgetMcfAllocated=s.widgetMcf->allocateAndConstruct157320(); //2A8804; NULL still reaches vectors.
            const char* vectors[]{"Position","LookVector","UpVector"};
            std::array<float,4>* destinations[]{&s.position70,&s.look80,&s.up90};
            for(std::size_t n=0;n<3;++n){std::array<float,3> v{};const auto pc=std::uint32_t(0x2A8848+44*n);
                if(!assets::originalUiTriple50KG(section,vectors[n],n?std::array<float,3>{{0,0,1}}:std::array<float,3>{{1,1,1}},v,d.config,pc))return fail(pc);
                for(std::size_t lane=0;lane<3;++lane)(*destinations[n])[lane]=v[lane];(*destinations[n])[3]=1;}
            if(!s.widgetMcfAllocated)return fail(0x2A88B0);
            if(s.widgetMcf->open1577D0(s.path,d.config)!=OriginalUiCourseManagerConfig50KG::OpenResult::opened)return fail(0x2A88B0);
            const char* motionKeys[]{"SelectionMotion","DeSelectionMotion","InitialMotion"};
            constexpr std::uint32_t getPC[]{0x2A88D8,0x2A8948,0x2A89B8};
            constexpr std::uint32_t namePC[]{0x2A8920,0x2A8990,0x2A8A00};
            for(std::size_t n=0;n<3;++n){s.motionNames[n]="NONE";std::int32_t ordinal=0;
                if(!assets::originalUiInteger50KG(section,motionKeys[n],0,ordinal,d.config,getPC[n]))return fail(getPC[n]);
                if(ordinal>0){
                    // Actual2A88FC/896C/89DC formatting, then157D00 at
                    //2A8908/8978/89E8 before the NAME getter.
                    std::string sectionName;
                    try {sectionName="Motion "+std::to_string(ordinal);}
                    catch(...){return fail(0x2A88FCu+std::uint32_t(n*0x70));} // Native formatting admission, prior writes retained.
                    if(!s.widgetMcf->selectWidgetsSection157D00(sectionName,d.config))return fail(d.config.sourcePC);
                    const auto* motion=s.widgetMcf->config_->records_.section(sectionName);
                    const auto* name=motion?motion->find("NAME"):nullptr;
                    if(!s.widgetMcf->selectedCurrent(sectionName)||!name)return fail(namePC[n]);s.motionNames[n]=name->text;}}
            if(!assets::originalUiInteger50KG(section,"NumberOfRandomMotions",0,s.randomCount3A0,d.config,0x2A8A18))return fail(0x2A8A18);
            if(s.randomCount3A0)return fail(0x2A8A2C); // Actual authoredzero; no fabricated empty positive list.
            if(!s.widgetMcf->closeHeap1573B0(1,d.config))return fail(d.config.sourcePC?d.config.sourcePC:0x2A8AE4);
            s.animation60.reset(new(std::nothrow) game::OriginalRacerAnimationCold50LA); //2A8AF0.
            s.type34=3; //2A8AF4 delay slot, after Config destructor, before NULL/ctor branch.
            if(!s.animation60){s.sourceAnimation60=nullptr;return fail(0x1538C8);}
            game::OriginalRacerAnimationColdBinding50LA binding;
            d.entity=construction_->prepareCourseAnimationBinding1536F0(i,*s.animation60,binding);if(!d.entity.okay())return fail(d.entity.sourcePC);
            const auto made=s.animation60->constructFresh1536F0(binding);if(!made.okay())return fail(made.sourcePC);
            s.sourceAnimation60=s.animation60.get(); //2A8B10 only after constructor returns.
            d.entity=construction_->enrollCourseAnimation1536F0(i,*s.animation60);if(!d.entity.okay())return fail(d.entity.sourcePC);
            s.animationEnrolled=true;
            if(!openCourseSoultreeAnimation1538B0(i,d))return false;
            d.entity=construction_->attachCourseAnimation2A8B44(i,s.entity,*s.animation60);if(!d.entity.okay())return fail(d.entity.sourcePC);
            s.animationAttached=true;
            auto graph=s.model.graph();const auto root=static_cast<std::uint32_t>(s.model.scene->model()->sceneIndex);
            graph.nodes[root].sceneFlags190=std::uint8_t(graph.nodes[root].sceneFlags190|8u); //2A8B84.
            s.loaderReturned=true;d.sourcePC=0x1CD35C;return true; // Before real Search/Observer28E8A0, NOT factoryreturn.
        }catch(...){return fail(0x2A8770);}
    }
    bool preflightCourseSoultreeMotionNative(std::size_t i)const noexcept {
        if(!courseSoultreeOrdinalAdmitted(i))return false;
        if(!courseSoultreeRows_[i])return true;const auto& s=*courseSoultreeRows_[i];
        if(s.motionReleased||!s.motionRetained)return true;
        if(!s.motionLibrary||s.motionLibrary!=issuer_->motionLibrary4333C0()||!s.retainedMotion||
           !s.motionLibrary->owns(s.retainedMotion)||!s.motionLibrary->slotOwners_)return false;
        const auto& r=*s.retainedMotion;
        if(!r.publishedSlotReferences_||r.fields_.referenceD8<0||
           std::uint32_t(r.fields_.referenceD8)<r.publishedSlotReferences_)return false;
        return !s.motionSlotWritten||(s.motionSlots1E4&&s.motionSlots1E4[0]==s.retainedMotion);
    }
    bool abortNativeCourseSoultreeMotion(std::size_t i)noexcept {
        if(!courseSoultreeOrdinalAdmitted(i)||!courseSoultreeRows_[i])return false;
        if(!preflightCourseSoultreeMotionNative(i))return false;auto& s=*courseSoultreeRows_[i];
        if(s.motionReleased)return true;
        if(s.motionRetained){auto* r=s.motionLibrary->mutableResource(s.retainedMotion);if(!r)return false;
            --r->publishedSlotReferences_;
            const auto released=s.motionLibrary->release151180(r);
            if(!released.okay()){++r->publishedSlotReferences_;return false;}
            --s.motionLibrary->slotOwners_;s.motionRetained=false;
        }
        s.motionSlots1E4.reset();s.motionReleased=true;return true; // Source1E4 historical pointer is deliberately retained.
    }
    bool abortNativeCourseSoultreeRow(std::size_t i)noexcept {
        if(!courseSoultreeOrdinalAdmitted(i))return false;
        for(std::size_t later=i+1;later<courseSoultreeRows_.size();++later)if(courseSoultreeRows_[later])return false;
        if(thread_!=std::this_thread::get_id())return false;
        if(!courseSoultreeRows_[i])return true;auto& s=*courseSoultreeRows_[i];
        if(!preflightNativeCourseSoultreeRow(i))return false;
        Diagnostic d;
        if(!abortNativeCourseSoultreeModel(i,s.model,d))return false; // Child graph before animation payload destructor.
        if(s.animation60&&s.animation60->constructed_&&!s.animation60->released_){
            auto& f=s.animation60->fields_;
            if(!s.animationPayloadReleased){
                // Actual153FA0 current18 clear; storage frees preserve source
                // historical1F4/1E4/1EC pointers, never interpreted as live again.
                f.motion18=nullptr;s.saved1F4.reset();
                if(!abortNativeCourseSoultreeMotion(i))return false;
                s.records1EC.reset();f.count208=0;
                if(s.cachePublished){s.files->clearAnimation1327D0(s.animation60.get());s.cachePublished=false;}
                if(s.animationMcf&&!s.animationMcf->closeNative())return false;
                s.motionConfiguration.reset();s.animationPayloadReleased=true;
            }
            if(s.animationEnrolled){
                const auto r=construction_->abortNativeCourseAnimation153F50(i,*s.animation60);if(!r.okay())return false;s.animationEnrolled=false;
            }else {
                // The only unenrolled constructed state is failure immediately
                // after constructor, before1538B0/cache/payload producers.
                if(s.cachePublished||s.animationMcfAllocated||s.motionAttempted||s.model.attempted)return false;
                const auto r=s.animation60->releaseFresh153F50();if(!r.okay())return false;
            }
            s.animationReleased=true;s.animationAttached=false;
        }
        if(s.widgetMcf&&!s.widgetMcf->closeNative())return false;
        if(s.animationMcf&&!s.animationMcf->closeNative())return false;
        // Actual authoredrandomCount0 owns no3A4 allocations; positive branch
        // never passed its boundary. Names here retain only source strings.
        if(s.randomNames3A4)return false;
        if(s.constructed){const auto r=construction_->abortNativeCourseSoultree2A86A0(i,s.entity);if(!r.okay())return false;s.constructed=false;}
        courseSoultreeRows_[i].reset();return true; // Native-only unwind, never source2A86A0/153F50 success receipt.
    }
    bool preflightNativeCourseSoultreeRow(std::size_t i)const noexcept {
        if(!courseSoultreeOrdinalAdmitted(i))return false;
        if(!courseSoultreeRows_[i])return true;const auto& s=*courseSoultreeRows_[i];
        if(s.constructed&&(!construction_||!construction_->preflightCourseSoultreeRetirement(i,s.entity)||
           construction_->courseSoultreeRows_[i].outer!=&s.entity||s.manager30!=&scope_->manager_))return false;
        if(s.animationEnrolled&&(!s.animation60||construction_->courseSoultreeRows_[i].animation!=s.animation60.get()||
           s.sourceAnimation60!=s.animation60.get()))return false;
        // Entire actual Entity/resource/Motion/cache preflight before first free.
        if(!preflightCourseSoultreeModelNative(i,s.model)||!preflightCourseSoultreeMotionNative(i))return false;
        if(s.cachePublished){assets::OriginalStandardModelFileView50LA found;assets::OriginalStandardTextureDiagnostic50LA cache;
            if(!s.files||!s.animation60||!s.files->modelFile(s.mcfId,found,cache)||found.animation1536F0!=s.animation60.get())return false;}
        return true;
    }
    bool abortNativeCourseSoultree()noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(!preflightAllCourseModelObserversNative())return false;
        // Whole acquired cohort proof before the first payload free. Tail-only
        // retirement admissions run separately as each later row is removed.
        for(std::size_t i=0;i<courseSoultreeRows_.size();++i)
            if(courseSoultreeRows_[i]&&!preflightNativeCourseSoultreeRow(i))return false;
        for(std::size_t end=courseSoultreeRows_.size();end;--end){const auto i=end-1;
            if(courseSoultreeRows_[i]&&!abortNativeCourseSoultreeRow(i))return false;}
        return true;
    }
