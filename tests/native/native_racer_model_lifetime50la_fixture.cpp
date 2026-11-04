#include "original_racer_terrain_query_owner50la.h"
// Out-of-line retained model implementation, including source-only counted348 lifetime.
#include "native_racer_model_lifetime50la_fixture.h"
#include "../../src/game/original_wreck_tuning50la.h"
#include "../../src/assets/original_phy_cursor50la.h"
#include "ra_source_camera_ownership50la.h"

namespace native_racer_model50LA {

void Owner::captureCold434630BeforeModel(const g::SceneGraph49P& canonical) {
        if(modelRole_!=Rig::right)return;
        failurePC_=0x210bbcu;
        need(!coldStack0F70_&&canonical.nodes==arena_->nodes&&canonical.count==rig_->nodeCount,
            "original cold434630 capture requires the same construction arena");
        ColdStack0F70 captured;
        std::memcpy(captured.shared434630.data(),arena_->visualShared.zero434630.v,16);
        captured.stack0F70=captured.shared434630; //210BCC full vector copy.
        coldStack0F70_=captured;
    }

void Owner::captureColdNullcgAfterModel(const g::SceneGraph49P& canonical) {
        if(modelRole_!=Rig::right&&modelRole_!=Rig::chariot)return;
        failurePC_=modelRole_==Rig::right?0x210cc4u:0x2119b8u;
        need(canonical.nodes==arena_->nodes&&canonical.count==rig_->nodeCount&&
            g::validateSceneGraph49P(canonical),"original cold nullcg capture graph mismatch");
        // At this boundary the real model subtree is constructed and restored
        // by1556C0. No fresh PSG read, row0 assumption or later Rig pose is used.
        const std::array<const g::OriginalRacerModelSceneOwner50LA*,5> own{{&scene_,nullptr,nullptr,nullptr,nullptr}};
        const g::OriginalRacerSceneObject50LA* cg=nullptr;
        need(findBoundsNullcg28C590(scene_,canonical,own,cg)&&cg,
            "original cold retained nullcg lookup stopped");
        const auto index=static_cast<std::uint32_t>(cg->sceneIndex);
        if(modelRole_==Rig::right){
            need(coldStack0F70_&&!coldStack0F70_->rightNullcg,
                "original right nullcg cold input already captured or missing");
            failurePC_=0x210cd8u;
            //28B1A0(relative=null) obtains world XYZ before28AB90 normalization.
            // Existing graph refresh owns the real cache effects, not scratch.
            need(g::refreshSceneWorld49P(canonical,cg->sceneIndex),
                "original right nullcg cold world refresh stopped");
            std::memcpy(&coldStack0F70_->stack0F70[2],&canonical.nodes[index].world100.v[14],4);
            coldStack0F70_->rightNullcg=cg; //210CFC replaces only stack0F78.
        }else{
            const auto* right=peers_[Rig::right];
            need(!coldChariotLocal_&&right&&right->coldStack0F70_&&
                right->coldStack0F70_->rightNullcg&&right->rig_==rig_&&right->arena_==arena_&&
                right->bank_==bank_&&right->slot_==slot_&&right->generation_==generation_&&
                right->application_==application_,"original chariot cold right-owner provenance missing");
            ColdChariotLocal captured;captured.nullcg=cg;captured.rightOwner=right;
            std::memcpy(captured.xyz.data(),&canonical.nodes[index].localC0.v[12],12);
            coldChariotLocal_=captured; //2119C4 captures local XYZ; never world or W.
        }
        // These are retained input operands only. No213380/effect/190/194/IK
        // completion is asserted here. The owning chariot constructor immediately
        // publishes these operands to its bound canonical Fixture destinations.
    }

void Owner::publishColdTarget2119C4() {
        failurePC_=0x2119c4u;
        need(fixture_ && &fixture_->state==arena_&&modelRole_==Rig::chariot&&coldChariotLocal_,
            "original cold target destination lifetime unavailable");
        auto& length=arena_->fields.length858;
        using Stage=g::OriginalRacerLength85850LA::Stage;
        const auto* right=coldChariotLocal_->rightOwner;
        need(length.stage_==Stage::unwritten&&right&&right->fixture_==fixture_&&right->coldStack0F70_&&
            right->coldStack0F70_->rightNullcg,"original cold stack/destination already consumed or missing");
        need(std::fegetround()==FE_TONEAREST,"original target rounding rejected");
        auto& target=fixture_->secondaryInputs.targetRelative2F0;
        //28B180 writes only XYZ. Preserve the canonical existing2FC bits.
        std::memcpy(target.v,coldChariotLocal_->xyz.data(),12);
        for(unsigned lane=0;lane<3;++lane){
            failurePC_=0x2119d8u+lane*0x10u;
            const float operand=coldFloat(right->coldStack0F70_->stack0F70[lane]);
            volatile float result=target.v[lane]-operand;
            need(std::isfinite(result),"original2119CC target subtraction stopped");
            target.v[lane]=result; //Earlier source XYZ writes survive later failure.
        }
        failurePC_=0x211a0cu;const float result=coldLength(target.v);
        fixture_->parameters.lifecycle.restLength854=result; //211A28
        length.generation_=generation_;length.slot_=slot_;
        length.value_=result;length.stage_=Stage::initial211A2C; //211A2C
    }

g::OriginalRacerModelSceneResult50LA Owner::prepareMaterials213380()noexcept {
        using S=g::OriginalRacerModelSceneStatus50LA;
        if(!racer_||closed_||!attachedFor(*racer_)||!racer_->selectionMaterialTable_||
           materials_.table382B38()!=racer_->selectionMaterialTable_.get())return {S::phase,0x213380u};
        auto& table=*racer_->selectionMaterialTable_;const auto& fields=scene_.fields();
        if(fields.materialCount1C0<0||(fields.materialCount1C0&&!fields.materials1C4))
            return {S::binding,0x2133b4u};
        try {
            for(std::int32_t i=0;i<fields.materialCount1C0;++i){
                a::OriginalStandardMaterialRowView50LA row;
                if(!table.row(fields.materials1C4[i],row)||!row.name00)return {S::binding,0x298d90u};
                a::OriginalStandardMaterialDiagnostic50LA d;std::int32_t original=-1;
                if(!table.find298C80(*row.name00,original,d))return {S::binding,0x2133dcu};
                if(row.name00->find("ghost")!=std::string::npos)continue; //309BA8 substring, case-sensitive
                if(row.name00->size()>250u)return {S::binding,0x21340cu};
                const std::string ghost=*row.name00+"ghost"; //Original format "%sghost"
                (void)ghost; //Suffix is not a replacement-material ownership operation.
                if(racer_->mode3C_!=0)return {S::phase,0x213428u};
                //Real normal mode branches213414->213460: no lookup/retain/override.
            }
            return {S::complete,0x213478u};
        }catch(...){return {S::allocation,0x21340cu};}
    }

g::OriginalRacerModelSceneResult50LA Owner::prepareBounds228704()noexcept {
        using S=g::OriginalRacerModelSceneStatus50LA;
        using R=g::OriginalRacerModelSceneResult50LA;
        if(modelRole_!=Rig::chariot||!bank_||!arena_||!racer_||!finishedFor(*racer_))
            return {S::phase,0x2286f0u};
        try {return bank_->visit(slot_,[&]()->R {
            auto& flags=arena_->bodies.main.controls.player.flagsB9C;
            if(flags&0x80u)return {S::complete,0x228724u}; //Original gate, no repair.
            const auto current=graph();
            if(arena_->sceneNodeCount!=rig_->nodeCount||!g::validateSceneGraph49P(current))
                return {S::graph,0x217440u};
            const std::array<unsigned,5> order{{Rig::chariot,Rig::right,Rig::left,
                Rig::bottomRight,Rig::bottomLeft}};
            const std::array<const g::OriginalRacerModelSceneOwner50LA*,5> models{{
                racer_->chariotModel1B4,racer_->rightModel1B8,racer_->leftModel1BC,
                racer_->bottomRightModel1C0,racer_->bottomLeftModel1C4}};
            const std::array<std::uint32_t,5> lookups{{0x217464u,0x2174e0u,0x21755cu,0x2175d8u,0x217654u}};
            const std::array<std::uint32_t,5> setters{{0x217478u,0x2174f4u,0x217570u,0x2175ecu,0x217668u}};
            for(unsigned n=0;n<order.size();++n){
                if(!models[n])continue; //Actual1B4/1B8/1BC/1C0/1C4 null branch.
                auto* owner=order[n]==modelRole_?this:peers_[order[n]];
                if(!owner||!owner->finishedFor(*racer_)||&owner->scene_!=models[n]||
                   owner->bank_!=bank_||owner->slot_!=slot_||owner->arena_!=arena_||
                   owner->rig_!=rig_||owner->generation_!=generation_||
                   owner->application_!=application_)return {S::binding,lookups[n]};
                const g::OriginalRacerSceneObject50LA* cg=nullptr;
                if(!findBoundsNullcg28C590(*models[n],current,models,cg)||!cg)
                    return {S::sourceRowTable,lookups[n]};
                g::SceneDisplacement49P zero;
                // SAME existing live434630 owner used by49R/49V and banked state.
                std::memcpy(zero.v,arena_->visualShared.zero434630.v,sizeof zero.v);
                if(!g::setLocalTranslation49P(current,cg->sceneIndex,zero))
                    return {S::graph,setters[n]};
                auto& cgNode=current.nodes[static_cast<std::uint32_t>(cg->sceneIndex)];
                cgNode.sceneFlags190=static_cast<std::uint8_t>(cgNode.sceneFlags190&~0x20u);
                // Source loads animation.model04 AFTER translation and flag20.
                // Preserve those mutations if this later ownership boundary fails.
                if(owner->animation_.fields_.model04!=&owner->scene_||!owner->scene_.model())
                    return {S::binding,setters[n]+0x34u};
                const auto root=owner->scene_.model()->sceneIndex;
                const auto index=static_cast<std::uint32_t>(root);
                if(index>=current.count)return {S::graph,setters[n]+0x34u};
                current.nodes[index].sceneFlags190=static_cast<std::uint8_t>(
                    current.nodes[index].sceneFlags190&~0x40u);
                // Already inside the sole Bank visit: never call a visiting wrapper.
                auto result=owner->scene_.publishAggregate28E110(current);
                if(!result.okay())return result;
            }
            flags=static_cast<std::uint8_t>(flags|0x80u); //22870C..228720 only after return.
            return {S::complete,0x228724u};
        });}catch(...){return {S::graph,0x228704u};}
    }

bool Owner::findBoundsNullcg28C590(const g::OriginalRacerModelSceneOwner50LA& start,
        const g::SceneGraph49P& graph,
        const std::array<const g::OriginalRacerModelSceneOwner50LA*,5>& models,
        const g::OriginalRacerSceneObject50LA*& found,const char* wanted)noexcept {
        found=nullptr;if(!start.model()||start.released()||!start.sourceRowCount())return false;
        std::size_t remaining=graph.count;
        auto equal=[wanted](const std::string& name)noexcept {
            const auto size=std::strlen(wanted);
            if(name.size()!=size)return false;
            for(std::size_t i=0;i<size;++i){auto c=name[i];if(c>='A'&&c<='Z')c=char(c+32);
                if(c!=wanted[i])return false;}return true;
        };
        auto walk=[&](auto&& self,g::SceneNodeIndex49P index)->bool {
            if(index==g::SceneNodeIndex49P::none)return true;
            const auto n=static_cast<std::uint32_t>(index);
            if(!remaining||n>=graph.count)return false;--remaining;
            const g::OriginalRacerSceneObject50LA* object=nullptr;
            for(const auto* model:models)if(model){
                if(model->released())return false;
                if(model->model()&&model->model()->sceneIndex==index)object=model->model();
                for(std::uint32_t j=0;!object&&j<model->sourceRowCount();++j){
                    const auto* row=model->sourceRow(j);if(row&&row->sceneIndex==index)object=row;}
                if(object)break;
            }
            if(!object||!object->constructed||object->released)return false;
            if(equal(object->name38)){found=object;return true;}
            // Original28C590: self, next sibling188, then first child184.
            if(!self(self,graph.nodes[n].nextSibling188))return false;
            if(found)return true;
            return self(self,graph.nodes[n].firstChild184);
        };
        return walk(walk,start.model()->sceneIndex);
    }

void Owner::constructColdWrists211A4C(const g::SceneGraph49P& canonical) {
        need(modelRole_==Rig::chariot&&coldScenes_.parented_&&
            canonical.nodes==arena_->nodes&&canonical.count==rig_->nodeCount,
            "original optional wrist construction requires actual190/194 and chariot");
        const std::array<const g::OriginalRacerModelSceneOwner50LA*,5> own{{&scene_,nullptr,nullptr,nullptr,nullptr}};
        auto object=[&](g::SceneNodeIndex49P index)->const g::OriginalRacerSceneObject50LA* {
            if(index==g::SceneNodeIndex49P::none)return nullptr;
            if(scene_.model()->sceneIndex==index)return scene_.model();
            for(std::uint32_t i=0;i<scene_.sourceRowCount();++i){
                const auto* row=scene_.sourceRow(i);if(row&&row->sceneIndex==index)return row;}
            return nullptr;
        };
        for(unsigned i=0;i<2;++i){
            auto& owner=coldWrists_[i];failurePC_=i?0x211ab0u:0x211a4cu;
            need(owner.stage==ColdWrist::Stage::unreached,"original wrist constructor cannot replay");
            need(findBoundsNullcg28C590(scene_,canonical,own,owner.wrist,i?"handrt":"handlt"),
                "original wrist name lookup topology stopped");
            if(!owner.wrist){owner.stage=ColdWrist::Stage::absent;continue;}
            const auto parent=canonical.nodes[static_cast<unsigned>(owner.wrist->sceneIndex)].parent180;
            if(parent==g::SceneNodeIndex49P::none){owner.stage=ColdWrist::Stage::absent;continue;}
            owner.elbow=object(parent);need(owner.elbow,"original wrist parent lacks retained scene identity");
            const auto grandparent=canonical.nodes[static_cast<unsigned>(parent)].parent180;
            if(grandparent==g::SceneNodeIndex49P::none){owner.stage=ColdWrist::Stage::absent;continue;}
            owner.shoulder=object(grandparent);need(owner.shoulder,"original wrist grandparent lacks retained scene identity");
            failurePC_=i?0x211adcu:0x211a78u;
            try{owner.allocation=std::make_unique<ColdWristAllocation>();}
            catch(const std::bad_alloc&){owner.stage=ColdWrist::Stage::allocationNull;continue;}
            owner.stage=ColdWrist::Stage::allocated;
            g::ChariotIkVector49T axis;
            std::memcpy(axis.v,application_->binderTriple434650_.data(),sizeof axis.v);
            if(i){for(unsigned lane=0;lane<3;++lane)axis.v[lane]=-axis.v[lane];axis.v[3]=1.f;}
            const g::ChariotIkChainBinding49T binding{owner.shoulder->sceneIndex,owner.elbow->sceneIndex,
                owner.wrist->sceneIndex,g::SceneNodeIndex49P::none};
            g::ChariotIkWorkspace49T work{};g::ChariotIkDiagnostic49T diagnostic{};
            failurePC_=i?0x211b34u:0x211a9cu;
            need(g::initializeChariotIkRecord50LA(binding,axis,canonical,owner.allocation->record,
                arena_->animationShared.libm,work,diagnostic),"original separate wrist28EB90 numeric boundary stopped");
            owner.stage=ColdWrist::Stage::constructed; //211AA4 or211B3C publication.
        }
    }

void Owner::closeUnpublishedColdWrists()noexcept {
        // Native pre-Session exceptional unwind only. Published allocations
        // have moved to racer208/20C and survive all model/scene child closes.
        for(auto& owner:coldWrists_)owner.allocation.reset();
    }

g::OriginalWreckComponentDomain50LA Owner::outside(
        std::array<g::OriginalWreckComponent50LA*,29>& storage)noexcept {
        std::size_t count=0;storage[count++]=&animationEntity();
        for(auto* peer:peers_)if(peer&&peer->animation_.constructed1536F0())
            storage[count++]=&peer->animationEntity();
        if(racer_){
            storage[count++]=&racer_->racer;storage[count++]=&racer_->overlay240.component;
            storage[count++]=&racer_->particle7FC;storage[count++]=&racer_->detached7F8;
        }
        if(body340_&&body340_->liveEntity())storage[count++]=body340_->liveEntity();
        for(auto* peer:peers_)if(peer&&peer->body340_&&peer->body340_->liveEntity())
            storage[count++]=peer->body340_->liveEntity();
        if(gravity344_&&gravity344_->liveEntity())storage[count++]=gravity344_->liveEntity();
        for(auto* peer:peers_)if(peer&&peer->gravity344_&&peer->gravity344_->liveEntity())
            storage[count++]=peer->gravity344_->liveEntity();
        if(sourceTerrain348_&&sourceTerrain348_->liveEntity())storage[count++]=sourceTerrain348_->liveEntity();
        if(sourceEngine34C_&&sourceEngine34C_->liveEntity())storage[count++]=sourceEngine34C_->liveEntity();
        if(sourceAero350_&&sourceAero350_->liveEntity())storage[count++]=sourceAero350_->liveEntity();
        if(sourceQuery690_&&sourceQuery690_->liveEntity())storage[count++]=sourceQuery690_->liveEntity();
        if(sourceQuery694_&&sourceQuery694_->liveEntity())storage[count++]=sourceQuery694_->liveEntity();
        if(sourceContact354_&&sourceContact354_->liveEntity())storage[count++]=sourceContact354_->liveEntity();
        if(sourceDetector358_&&sourceDetector358_->liveEntity())storage[count++]=sourceDetector358_->liveEntity();
        if(sourceQuery7A4_&&sourceQuery7A4_->liveEntity())storage[count++]=sourceQuery7A4_->liveEntity();
        if(sourceQuery79C_&&sourceQuery79C_->liveEntity())storage[count++]=sourceQuery79C_->liveEntity();
        if(sourceBody35C_&&sourceBody35C_->liveEntity())storage[count++]=sourceBody35C_->liveEntity();
        if(joint360_&&joint360_->liveEntity())storage[count++]=joint360_->liveEntity();
        if(sourceQuery364_&&sourceQuery364_->liveEntity())storage[count++]=sourceQuery364_->liveEntity();
        if(sourceContact368_&&sourceContact368_->liveEntity())storage[count++]=sourceContact368_->liveEntity();
        if(sourceDetector36C_&&sourceDetector36C_->liveEntity())storage[count++]=sourceDetector36C_->liveEntity();
        for(auto* peer:peers_)if(peer&&peer->joint360_&&peer->joint360_->liveEntity())
            storage[count++]=peer->joint360_->liveEntity();
        coldScenes_.addDomain(storage.data(),count);
        for(auto* peer:peers_)if(peer)peer->coldScenes_.addDomain(storage.data(),count);
        // Scene methods add the at-most-five real Model Entity8 addresses privately.
        // No released peer or stale model alias becomes a new live object.
        return {generation_,storage.data(),count};
    }

bool Owner::admitAnimationClose()noexcept {
        // A later partially prepared owner must retire before an earlier
        // animation can consume its surviving model alias, even if the later
        // Session append was never reached. Successful close severs the peer.
        for(const auto* peer:peers_)if(peer&&sourceRank(peer->modelRole_)>sourceRank(modelRole_))
            return false;
        if(!animation_.constructed1536F0())return true;
        const auto& c=animationEntity();
        const auto* next=body340_&&body340_->stage_==g::OriginalRacerMainBodyOwner50LA::Stage::appended?
            body340_->liveEntity():nullptr;
        if(c.reference00!=1u||c.next14!=next||
           (modelAppended_?c.firstChild18!=scene_.modelEntity8():c.firstChild18!=nullptr))return false;
        if(!animationAttached_)return !c.parent1C&&!c.previous10;
        if(!racer_||c.parent1C!=&racer_->racer||c.mask20!=2u)return false;
        auto* previous=&racer_->particle7FC;
        for(const auto role:sourceRoles_){
            if(role==modelRole_)break;
            auto* peer=peers_[role];
            if(!peer){
                if((role==Rig::bottomRight||role==Rig::bottomLeft)&&rig_->modelCount==3u)continue;
                return false;
            }
            if(!peer->finishedFor(*racer_))return false;
            previous=&peer->animationEntity();
        }
        // Source142590/1424E0 closes later animation siblings first. In this
        // bounded projection the reverse is C, BL, BR, L, R (or C, L, R),
        // before each earlier owner consumes its surviving model alias.
        if(modelRole_==Rig::chariot){
            if(coldScenes_.appended_!=3)return false;
            previous=coldScenes_.lastEntity();
        }
        return c.previous10==previous&&previous->next14==&c;
    }

bool Owner::closeOnCurrentGraph()noexcept {
        // Native exceptional unwind must restore the same shared source path.
        restoreModelDirectory1543A8();
        if(closed_)return true;
        if(!application_) {closed_=true;return true;}
        if(application_->attachmentLock423F58())return false;
        try {
            // Admit the complete motion release before ANY scene/model
            // mutation; a failure must leave the animation alias retryable.
            const auto motionAdmission=motions_.preflightRelease153F50();
            if(!motionAdmission.okay()){failurePC_=motionAdmission.sourcePC;return false;}
            if(!admitAnimationClose()){failurePC_=0x142590u;return false;}
            const auto* bodyTail=body340_&&body340_->stage_==g::OriginalRacerMainBodyOwner50LA::Stage::appended?
                body340_->liveEntity():nullptr;
            const auto* gravityTail=gravity344_&&gravity344_->appended()?gravity344_->liveEntity():nullptr;
            const auto* jointTail=joint360_&&joint360_->stage_==g::OriginalPodSphereOwner50LA::Stage::appended?
                joint360_->liveEntity():nullptr;
            const auto* terrainTail=sourceTerrain348_&&sourceTerrain348_->appended()?
                sourceTerrain348_->liveEntity():nullptr;
            const auto* engineTail=sourceEngine34C_&&sourceEngine34C_->appended_&&sourceEngine34C_->liveEntity()?
                sourceEngine34C_->liveEntity():nullptr;
            const auto* aeroTail=sourceAero350_&&sourceAero350_->appended_&&sourceAero350_->liveEntity()?
                sourceAero350_->liveEntity():nullptr;
            const auto* queryTail=sourceQuery690_&&sourceQuery690_->appended()?sourceQuery690_->liveEntity():nullptr;
            const auto* podTail=sourceQuery694_&&sourceQuery694_->appended()?sourceQuery694_->liveEntity():nullptr;
            const auto* contactTail=sourceContact354_&&sourceContact354_->appended()?sourceContact354_->liveEntity():nullptr;
            const auto* detectorTail=sourceDetector358_&&sourceDetector358_->appended()?sourceDetector358_->liveEntity():nullptr;
            const auto* movementTail=sourceQuery7A4_&&sourceQuery7A4_->appended()?sourceQuery7A4_->liveEntity():nullptr;
            const auto* triggerTail=sourceQuery79C_&&sourceQuery79C_->appended()?sourceQuery79C_->liveEntity():nullptr;
            const auto* secondarySceneTail=coldScenes_.secondaryAppended_?coldScenes_.secondaryLiveEntity():nullptr;
            const auto* secondaryBodyTail=sourceBody35C_&&sourceBody35C_->appended()?sourceBody35C_->liveEntity():nullptr;
            const auto* secondaryQueryTail=sourceQuery364_&&sourceQuery364_->appended()?sourceQuery364_->liveEntity():nullptr;
            const auto* secondaryContactTail=sourceContact368_&&sourceContact368_->appended()?sourceContact368_->liveEntity():nullptr;
            const auto* secondaryDetectorTail=sourceDetector36C_&&sourceDetector36C_->appended()?sourceDetector36C_->liveEntity():nullptr;
            if((secondaryQueryTail&&!jointTail)||(secondaryContactTail&&!secondaryQueryTail)||
               (secondaryDetectorTail&&!secondaryContactTail)){failurePC_=0x142590u;return false;}
            if(sourceDetector36C_&&sourceDetector36C_->liveEntity()&&
               !sourceDetector36C_->preflightClose142590(secondaryDetectorTail&&racer_?&racer_->racer:nullptr,
                    secondaryDetectorTail?secondaryContactTail:nullptr,nullptr)){failurePC_=0x142590u;return false;}
            if(sourceContact368_&&sourceContact368_->liveEntity()&&
               !sourceContact368_->preflightClose142590(secondaryContactTail&&racer_?&racer_->racer:nullptr,
                    secondaryContactTail?secondaryQueryTail:nullptr,secondaryContactTail?secondaryDetectorTail:nullptr)){failurePC_=0x142590u;return false;}
            if(sourceQuery364_&&sourceQuery364_->liveEntity()&&
               !sourceQuery364_->preflightClose142590(secondaryQueryTail&&racer_?&racer_->racer:nullptr,
                    secondaryQueryTail?jointTail:nullptr,secondaryQueryTail?secondaryContactTail:nullptr)){failurePC_=0x142590u;return false;}

            if((secondarySceneTail&&!triggerTail)||(secondaryBodyTail&&!secondarySceneTail)){
                failurePC_=0x142590u;return false;
            }
            if(sourceBody35C_&&sourceBody35C_->liveEntity()&&
               !sourceBody35C_->preflightClose142590(secondaryBodyTail&&racer_?&racer_->racer:nullptr,
                    secondaryBodyTail?secondarySceneTail:nullptr,secondaryBodyTail&&joint360_&&joint360_->sourceRoute_?jointTail:nullptr)){failurePC_=0x142590u;return false;}
            if(!coldScenes_.preflightSecondary142590(graph(),secondarySceneTail&&racer_?&racer_->racer:nullptr,
                    secondarySceneTail?triggerTail:nullptr,secondarySceneTail?secondaryBodyTail:nullptr)){
                failurePC_=0x142590u;return false;
            }
            if((engineTail&&!terrainTail)||(aeroTail&&!engineTail)||(queryTail&&!aeroTail)||(podTail&&!queryTail)||
               (contactTail&&!podTail)||(detectorTail&&!contactTail)||(movementTail&&!detectorTail)||
               (triggerTail&&!movementTail)){
                failurePC_=0x142590u;return false;
            }
            if(sourceQuery79C_&&sourceQuery79C_->liveEntity()&&
               !sourceQuery79C_->preflightClose142590(triggerTail&&racer_?&racer_->racer:nullptr,
                    triggerTail?movementTail:nullptr,triggerTail?secondarySceneTail:nullptr)){failurePC_=0x142590u;return false;}
            if(sourceQuery7A4_&&sourceQuery7A4_->liveEntity()&&
               !sourceQuery7A4_->preflightClose142590(movementTail&&racer_?&racer_->racer:nullptr,
                    movementTail?detectorTail:nullptr,movementTail?triggerTail:nullptr)){failurePC_=0x142590u;return false;}
            if(sourceDetector358_&&sourceDetector358_->liveEntity()&&
               !sourceDetector358_->preflightClose142590(detectorTail&&racer_?&racer_->racer:nullptr,
                    detectorTail?contactTail:nullptr,detectorTail?movementTail:nullptr)){failurePC_=0x142590u;return false;}
            if(sourceContact354_&&sourceContact354_->liveEntity()&&
               !sourceContact354_->preflightClose142590(contactTail&&racer_?&racer_->racer:nullptr,
                    contactTail?podTail:nullptr,contactTail?detectorTail:nullptr)){failurePC_=0x142590u;return false;}
            if(sourceQuery694_&&sourceQuery694_->liveEntity()&&
               !sourceQuery694_->preflightClose142590(podTail&&racer_?&racer_->racer:nullptr,
                    podTail?queryTail:nullptr,podTail?contactTail:nullptr)){failurePC_=0x142590u;return false;}
            if(sourceQuery690_&&sourceQuery690_->liveEntity()&&
               !sourceQuery690_->preflightClose142590(queryTail&&racer_?&racer_->racer:nullptr,
                    queryTail?aeroTail:nullptr,queryTail?podTail:nullptr)){failurePC_=0x142590u;return false;}
            if(sourceAero350_&&sourceAero350_->liveEntity()&&
               !sourceAero350_->preflightClose142590(aeroTail&&racer_?&racer_->racer:nullptr,
                    aeroTail?engineTail:nullptr,aeroTail?queryTail:nullptr)){failurePC_=0x142590u;return false;}
            if(sourceEngine34C_&&sourceEngine34C_->liveEntity()&&
               !sourceEngine34C_->preflightClose142590(engineTail&&racer_?&racer_->racer:nullptr,
                    engineTail?terrainTail:nullptr,engineTail?aeroTail:nullptr)){failurePC_=0x142590u;return false;}
            if(sourceTerrain348_&&!sourceTerrain348_->preflightClose(racer_?&racer_->racer:nullptr,gravityTail,engineTail)){
                failurePC_=0x142590u;return false;
            }
            if(terrainTail&&jointTail&&(!joint360_||!joint360_->sourceRoute_||!secondaryBodyTail)){
                failurePC_=0x142590u;return false;
            } //Legacy early joint cannot stand in for the actual later source tail.
            if(body340_&&!body340_->preflightClose(racer_?&racer_->racer:nullptr,&animationEntity(),gravityTail)){
                failurePC_=0x142590u;return false;
            }
            if(gravity344_&&!gravity344_->preflightClose(racer_?&racer_->racer:nullptr,bodyTail,terrainTail?terrainTail:jointTail)){
                failurePC_=0x142590u;return false;
            }
            if(joint360_&&!joint360_->preflightClose(racer_?&racer_->racer:nullptr,
                joint360_->sourceRoute_?secondaryBodyTail:gravityTail,joint360_->sourceRoute_?secondaryQueryTail:nullptr)){
                failurePC_=0x142590u;return false;
            }
            // Revoke native secondary borrows before any retained body/model
            // retirement. Registry weak custody cannot extend arena validity.
            if(joint360_)joint360_->nativeBodies_.revoked=true;
            if(nativeSecondaryContacts_){nativeSecondaryContacts_->revoked_=true;nativeSecondaryContacts_->targetRelative2F0_=nullptr;}
            if(nativeRacerTerrain_)nativeRacerTerrain_->revoked_=true;
            if(nativeAero_)nativeAero_->revoked_=true;
            closeUnpublishedColdWrists();
            sourceEngineQueryWithdrawn_=true;sourceQuery694Withdrawn_=true;sourceContact354Withdrawn_=true;
            sourceDetector358Withdrawn_=true;sourceAuxiliaryWithdrawn_=true;sourceSecondaryWithdrawn_=true;sourceSecondarySetupWithdrawn_=true;
            sourceSecondaryContactsWithdrawn_=true;
            if(sourceDetector36C_&&sourceDetector36C_->liveEntity()){
                const auto released=sourceDetector36C_->close142590(secondaryDetectorTail&&racer_?&racer_->racer:nullptr,
                    secondaryDetectorTail?secondaryContactTail:nullptr);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            if(sourceContact368_&&sourceContact368_->liveEntity()){
                const auto released=sourceContact368_->close142590(secondaryContactTail&&racer_?&racer_->racer:nullptr,
                    secondaryContactTail?secondaryQueryTail:nullptr);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            //Retain dead368 native Entity metadata until364's historical9C
            //registration is retired; source pointer aliases are not cleared.
            if(sourceQuery364_&&sourceQuery364_->liveEntity()){
                const auto released=sourceQuery364_->close142590(secondaryQueryTail&&racer_?&racer_->racer:nullptr,
                    secondaryQueryTail?jointTail:nullptr);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            if(joint360_&&joint360_->sourceRoute_){
                const auto released=joint360_->close142590(racer_?&racer_->racer:nullptr,secondaryBodyTail);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            if(sourceBody35C_&&sourceBody35C_->liveEntity()){
                const auto released=sourceBody35C_->close142590(secondaryBodyTail&&racer_?&racer_->racer:nullptr,
                    secondaryBodyTail?secondarySceneTail:nullptr);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            if(coldScenes_.secondaryLiveEntity()){
                auto* previous=sourceQuery79C_&&sourceQuery79C_->appended()?sourceQuery79C_->liveEntity():nullptr;
                const auto released=coldScenes_.closeSecondary142590(graph(),secondarySceneTail&&racer_?&racer_->racer:nullptr,
                    secondarySceneTail?previous:nullptr);
                if(!released.okay()){failurePC_=released.sourcePC;return false;}
            }
            if(sourceQuery79C_&&sourceQuery79C_->liveEntity()){
                const auto released=sourceQuery79C_->close142590(triggerTail&&racer_?&racer_->racer:nullptr,
                    triggerTail?movementTail:nullptr);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            if(sourceQuery7A4_&&sourceQuery7A4_->liveEntity()){
                const auto released=sourceQuery7A4_->close142590(movementTail&&racer_?&racer_->racer:nullptr,
                    movementTail?detectorTail:nullptr);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            if(sourceDetector358_&&sourceDetector358_->liveEntity()){
                const auto released=sourceDetector358_->close142590(detectorTail&&racer_?&racer_->racer:nullptr,
                    detectorTail?contactTail:nullptr);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            if(sourceContact354_&&sourceContact354_->liveEntity()){
                const auto released=sourceContact354_->close142590(contactTail&&racer_?&racer_->racer:nullptr,
                    contactTail?podTail:nullptr);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            if(sourceQuery694_&&sourceQuery694_->liveEntity()){
                const auto released=sourceQuery694_->close142590(podTail&&racer_?&racer_->racer:nullptr,
                    podTail?queryTail:nullptr);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            if(sourceQuery690_&&sourceQuery690_->liveEntity()){
                const auto released=sourceQuery690_->close142590(queryTail&&racer_?&racer_->racer:nullptr,
                    queryTail?aeroTail:nullptr);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            if(sourceAero350_&&sourceAero350_->liveEntity()){
                const auto released=sourceAero350_->close142590(aeroTail&&racer_?&racer_->racer:nullptr,
                    aeroTail?engineTail:nullptr);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            if(sourceEngine34C_&&sourceEngine34C_->liveEntity()){
                const auto released=sourceEngine34C_->close142590(engineTail&&racer_?&racer_->racer:nullptr,
                    engineTail?terrainTail:nullptr);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){failurePC_=released.sourcePC;return false;}
            }
            if(sourceTerrain348_){
                sourceTerrainWithdrawn_=true;sourceTerrain348_->withdrawNativeConsumers();
                if(!sourceTerrainBridge_||!sourceTerrainBridge_->withdrawSource348Body128(slot_,*sourceTerrain348_)){
                    failurePC_=0x1b1130u;return false;
                }
                const auto released=sourceTerrain348_->close142590(racer_?&racer_->racer:nullptr,gravityTail);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){
                    failurePC_=released.sourcePC;return false;
                }
            }
            if(joint360_&&!joint360_->sourceRoute_){
                const auto released=joint360_->close142590(racer_?&racer_->racer:nullptr,gravityTail);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){
                    failurePC_=released.sourcePC;return false;
                }
            }
            if(gravity344_){
                const auto released=gravity344_->close142590(racer_?&racer_->racer:nullptr,bodyTail);
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){
                    failurePC_=released.sourcePC;return false;
                }
            }
            if(body340_){
                const auto released=body340_->close142590(racer_?&racer_->racer:nullptr,&animationEntity());
                if(released.status!=g::OriginalWreckComponentStatus50LA::complete){
                    failurePC_=released.sourcePC;return false;
                }
            }
            if(scene_.modelEntity8()&&!scene_.released()){
                if(!modelAppended_){
                    const auto r=scene_.closeFreshUnlinked(graph());
                    if(!r.okay()){failurePC_=r.sourcePC;return false;}
                }else{
                    std::array<g::OriginalWreckComponent50LA*,29> storage{};
                    const auto d=outside(storage);
                    const auto peers=peerScenes();
                    const auto admission=scene_.preflightScene2896D0(graph());
                    if(!admission.okay()){failurePC_=admission.sourcePC;return false;}
                    const auto referenceAdmission=scene_.preflightModelReference142590(
                        d,peers[0],peers[1],peers[2],peers[3]);
                    if(!referenceAdmission.okay()){failurePC_=referenceAdmission.sourcePC;return false;}
                    if(!materials_.canRelease26E4C0()||
                       (geometry_&&geometry_->constructed240110()&&
                        !geometry_->canReleaseForModel(&scene_)))return false;
                    if(packets_.constructed240740()&&!packets_.preflight240450().okay())return false;
                    // The bank owns only a borrowed clone pointer. Remove it
                    // before the final26E4C0 release can destroy that material.
                    if(bankedFlame_){g::clearCanonicalEngineFlareBorrows50LA(*bankedFlame_,scene_);
                        bankedFlame_=nullptr;}
                    const auto reference=scene_.releaseModelReference142590(
                        d,peers[0],peers[1],peers[2],peers[3]);
                    if(!reference.okay()){failurePC_=reference.sourcePC;return false;}
                    //142590 reached the model through animation18, but repairs
                    // its CURRENT parent7F8. The old animation alias survives
                    // until the enclosing animation is disposed immediately.
                    if(packets_.constructed240740()&&!packets_.release240450().okay())return false;
                    a::OriginalStandardMaterialDiagnostic50LA md;
                    if(!materials_.release26E4C0(md)){failurePC_=md.sourcePC;return false;}
                    if(geometry_&&geometry_->constructed240110()){a::OriginalStandardResourceDiagnostic50LA rd;
                        if(!geometry_->release26E598(&scene_,materials_,rd)){
                            failurePC_=rd.sourcePC;return false;}}
                    const auto freed=scene_.freeSourceRowTable26E5CC();
                    if(!freed.okay()){failurePC_=freed.sourcePC;return false;}
                    const auto released=scene_.releaseSceneBase2896D0(graph());
                    if(!released.okay()){failurePC_=released.sourcePC;return false;}
                }
            }
            if(animation_.constructed1536F0()){
                auto& c=animationEntity();
                if(c.reference00!=1u||c.next14||
                   (modelReappended_&&c.firstChild18!=scene_.modelEntity8())||
                   (!modelReappended_&&c.firstChild18))return false;
                if(c.previous10)c.previous10->next14=c.next14;
                if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
                --c.reference00; //104970 ->156FF0 ->153F50.
                animation_.fields_.motion18=nullptr;
                if(setup_)setup_->releaseBeforeMotion153FA4();
                const auto motionRelease=motions_.release153F50();
                if(!motionRelease.okay()){failurePC_=motionRelease.sourcePC;return false;}
                animation_.fields_.count208=0;
                if(setup_)setup_->releaseAfterMotion1540C4();
                if(cachePublished_)files_->clearAnimation1327D0(&animation_);
                --application_->objectCount423F60();
                animation_.released_=true;
            }
            const auto coldClosed=coldScenes_.close(graph());
            if(!coldClosed.okay()){failurePC_=coldClosed.sourcePC;return false;}
            // Native partial-construction unwind also releases a still-live
            // temporary config. Completed1538B0 already disposed it1573B0.
            configuration08_.reset();input_.reset();archive_.reset();closed_=true;
            // Native peer bookkeeping only. Source parent/previous/head,
            // racer1A4/1A8/1AC/1B0/1A0 and model1B8/1BC/1C0/1C4/1B4 stay stale.
            for(auto*& peer:peers_)if(peer){peer->peers_[modelRole_]=nullptr;peer=nullptr;}
            return true;
        }catch(...){return false;}
    }

void Owner::prepareEarly(const Rig& rig,std::size_t slot,std::uint64_t generation,
        native_racer50HN::Bank& bank,runtime_startup50fl::Fixture& live,
        std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<g::OriginalMotionLibrary50LA> motionLibrary,
        std::string& modelDirectory36B860,unsigned modelRole) {
        need(!rig_&&generation&&application&&domain&&domain->prepared()&&motionLibrary&&
            (modelRole==Rig::right||modelRole==Rig::left||modelRole==Rig::chariot||
                ((modelRole==Rig::bottomRight||modelRole==Rig::bottomLeft)&&rig.modelCount==5u))&&
            modelRole<rig.modelCount,
            "native original model cold owner binding rejected");
        rig_=&rig;slot_=slot;generation_=generation;bank_=&bank;fixture_=&live;arena_=&live.state;modelRole_=modelRole;
        application_=std::move(application);domain_=std::move(domain);
        modelDirectory36B860_=&modelDirectory36B860;
        files_=domain_->resolver3E1A70();motionLibrary_=std::move(motionLibrary);
        need(bool(files_),"native original model requires shared application file owner");
        a::OriginalStandardTextureDiagnostic50LA fd;
        need(files_->bindModelMember(rig.mcfMembers[modelRole_],a::OriginalStandardModelFileKind50LA::mcf,
                rig.mcf[modelRole_],mcfId_,fd)&&
            files_->bindModelMember(rig.psgMembers[modelRole_],a::OriginalStandardModelFileKind50LA::psg,
                rig.psg[modelRole_],psgId_,fd),"original model authenticated cache binding rejected");
        a::OriginalStandardModelFileView50LA mcf,psg;
        need(files_->modelFile(mcfId_,mcf,fd)&&files_->modelFile(psgId_,psg,fd),
            "original model cache rows disappeared");
        // This useful first packet admits the actual fresh route. A genuine
        // warm result is a boundary, never forcibly cleared or called a miss.
        if(mcf.animation1536F0){failurePC_=0x153c20u;
            throw std::runtime_error("original live MCF warm156300 requires a distinct clone owner: "+
                rig.mcfMembers[modelRole_]+" row="+std::to_string(mcfId_.row));}
        if(psg.geometry240110){failurePC_=0x26ea28u;
            throw std::runtime_error("original live PSG warm26EA28 requires geometry retain/hierarchy clone: "+
                rig.psgMembers[modelRole_]+" row="+std::to_string(psgId_.row));}
        const auto constructed=animation_.constructFresh1536F0({generation_,key(&animation_),
            &application_->objectCount423F60(),&application_->attachmentLock423F58()});
        need(constructed.okay(),"original1536F0 animation constructor failed");
        animation_.fields_.field18C=1;
        need(files_->publishAnimation1326A0(mcfId_,&animation_,fd),"original MCF cache publication failed");
        cachePublished_=true;
        archive_=std::make_unique<setup::Archive>(rig);
        a::OriginalStandardArchiveProvider50LA::Diagnostic providerDiagnostic;
        need(domain_->archiveProvider3E1A70()&&
            archive_->mount(*domain_->archiveProvider3E1A70(),providerDiagnostic),
            "original Rig complete archive mount rejected");
        input_=std::make_unique<setup::Input>(setup::readInput1538B0(rig,modelRole_,*archive_));
        configuration08_=std::make_unique<setup::Ini>(input_->parsedConfiguration);
        animation_.fields_.configuration08=configuration08_.get();
        const auto& c=input_->configuration;
        auto& f=animation_.fields_;
        f.definition48=c.definition48;f.rig138=c.slt138;f.rigOverride188=c.rigOverride188;
        f.directory190=c.directory190;f.model98=c.vut98;f.motionE8=c.mirrorE8;
        f.motionCount14=c.motionCount14;f.field20=static_cast<std::uint32_t>(c.interpolatePosition20);
        f.field24=static_cast<std::uint32_t>(c.interpolateRotation24);
        f.field28=c.rotationType28;f.field2C=c.positionType2C;f.field0C=c.mirrorAxis0C;
    }

void Owner::admitPeer(const Owner& other)const {
        need(&other!=this&&rig_&&rig_==other.rig_&&
            slot_==other.slot_&&bank_==other.bank_&&arena_==other.arena_&&
            generation_==other.generation_&&application_==other.application_&&
            domain_==other.domain_&&files_==other.files_&&motionLibrary_==other.motionLibrary_&&
            modelDirectory36B860_==other.modelDirectory36B860_&&modelRole_!=other.modelRole_&&
            animation_.constructed1536F0()&&other.animation_.constructed1536F0()&&
            !graphStarted_&&!other.graphStarted_&&!racer_&&!other.racer_&&!closed_&&!other.closed_,
            "original model peer binding rejected");
        const auto rows=rig_->hierarchy[modelRole_].nodeCount;
        const auto peerRows=rig_->hierarchy[other.modelRole_].nodeCount;
        need(rows&&peerRows&&rows<=a::maxModelNodes49O&&peerRows<=a::maxModelNodes49O&&
            rig_->roots[modelRole_]!=rig_->roots[other.modelRole_],
            "original model peers require distinct canonical model ranges");
        for(std::uint32_t row=0;row<rows;++row){
            const auto node=rig_->sourceNodes[modelRole_][row];
            need(node!=rig_->roots[other.modelRole_],"original model source row aliases peer root");
            for(std::uint32_t peerRow=0;peerRow<peerRows;++peerRow)
                need(node!=rig_->sourceNodes[other.modelRole_][peerRow],
                    "original model source rows overlap");
        }
        for(std::uint32_t row=0;row<peerRows;++row)
            need(rig_->roots[modelRole_]!=rig_->sourceNodes[other.modelRole_][row],
                "original model root aliases peer source row");
    }

bool Owner::graphOrderAdmitted()const noexcept {
        if(rig_->modelCount==5u)
            for(const auto role:sourceRoles_)if(role!=modelRole_&&!peers_[role])return false;
        for(const auto* peer:peers_)if(peer){
            if(peer->modelDirectorySaved_)return false;
            const bool earlier=sourceRank(peer->modelRole_)<sourceRank(modelRole_);
            if(earlier){if(!peer->setup_||!peer->setup_->restored1556C0())return false;}
            else if(peer->graphStarted_)return false;
        }
        return true;
    }

bool Owner::attachmentOrderAdmitted(const g::OriginalRacerEntityCold50LA& racer)const noexcept {
        for(const auto role:sourceRoles_){
            if(role==modelRole_)continue;
            const auto* peer=peers_[role];
            if(!peer){
                if((role==Rig::bottomRight||role==Rig::bottomLeft)&&rig_->modelCount==3u)continue;
                // Preserve the upper right/left staging before the optional
                // separate-chariot join, but never omit an earlier owner.
                if(rig_->modelCount==3u&&sourceRank(role)>sourceRank(modelRole_))continue;
                return false;
            }
            if(sourceRank(role)<sourceRank(modelRole_)){
                if(!peer->finishedFor(racer))return false;
            }else if(peer->animationAttached_)return false;
        }
        return true;
    }

void Owner::bindSibling(Owner& other) {
        need((modelRole_==Rig::right&&other.modelRole_==Rig::left)||
             (modelRole_==Rig::left&&other.modelRole_==Rig::right),
            "original engine sibling roles rejected");
        for(const auto* peer:peers_)need(!peer,"original engine already has a peer");
        for(const auto* peer:other.peers_)need(!peer,"original engine peer already bound");
        admitPeer(other);
        peers_[other.modelRole_]=&other;other.peers_[modelRole_]=this;
    }

void Owner::bindChariotSiblings(Owner& right,Owner& left,
        Owner* bottomRight,Owner* bottomLeft) {
        need(rig_&&modelRole_==Rig::chariot&&right.modelRole_==Rig::right&&left.modelRole_==Rig::left&&
            right.peers_[Rig::left]==&left&&left.peers_[Rig::right]==&right&&
            bool(bottomRight)==bool(bottomLeft)&&
            (bottomRight?rig_->modelCount==5u:rig_->modelCount==3u),
            "original separate chariot requires its actual engine pair");
        for(unsigned role=0;role<peers_.size();++role){
            need(right.peers_[role]==(role==Rig::left?&left:nullptr)&&
                 left.peers_[role]==(role==Rig::right?&right:nullptr),
                "original engine pair already has additional peers");
        }
        for(const auto* peer:peers_)need(!peer,"original chariot peers already bound");
        if(bottomRight){
            need(bottomRight->modelRole_==Rig::bottomRight&&bottomLeft->modelRole_==Rig::bottomLeft,
                "original lower engine peer roles rejected");
            for(const auto* peer:bottomRight->peers_)need(!peer,"original bottom-right peers already bound");
            for(const auto* peer:bottomLeft->peers_)need(!peer,"original bottom-left peers already bound");
        }
        const std::array<Owner*,5> owners{{&right,&left,bottomRight,bottomLeft,this}};
        // All pairwise identity/domain/range admissions precede mutation.
        // Five owners borrow one another; none owns or clones another model.
        for(std::size_t i=0;i<owners.size();++i)if(owners[i])
            for(std::size_t j=0;j<i;++j)if(owners[j])owners[i]->admitPeer(*owners[j]);
        for(auto* owner:owners)if(owner)
            for(auto* peer:owners)if(peer&&peer!=owner)owner->peers_[peer->modelRole_]=peer;
    }

void Owner::constructGraph(const g::SceneGraph49P& canonical) {
        failurePC_=0x154140u;
        need(rig_&&input_&&!graphStarted_&&canonical.nodes==arena_->nodes&&
            canonical.count==rig_->nodeCount&&graphOrderAdmitted()&&
            (modelRole_!=Rig::chariot||(peers_[Rig::right]&&peers_[Rig::left])),
            "original model graph construction order rejected");
        captureCold434630BeforeModel(canonical);
        const auto& c=input_->configuration;
        if(!c.rigOverride188){
            savedModelDirectory_=*modelDirectory36B860_;
            modelDirectory36B860_->clear();modelDirectorySaved_=true;
        }
        //154140's real prefix state feeds26E820; it remains cleared until
        //152EA0/MIR/VUT finish, then restores before153610 motion setup.
        psgRequest_=*modelDirectory36B860_;
        if(!psgRequest_.empty()&&psgRequest_.back()!='\\')psgRequest_+='\\';
        psgRequest_+=c.rigOverride188?c.slt138:c.directory190+c.slt138;
        psgRequest_=setup::detail::canonical(std::move(psgRequest_));
        const auto dot=psgRequest_.find_last_of('.');
        psgRequest_=psgRequest_.substr(0,dot)+".psg";
        a::OriginalStandardTextureDiagnostic50LA fd;
        a::OriginalStandardModelFileView50LA selected;
        need(files_->lookupModel132820(psgRequest_,a::OriginalStandardModelFileKind50LA::psg,selected,fd)&&
            selected.id.resolver==psgId_.resolver&&selected.id.row==psgId_.row,
            "original source PSG request differs from authenticated Rig member");
        graphStarted_=true;
        const g::OriginalRacerModelSceneBinding50LA binding{generation_,
            &application_->objectCount423F60(),&application_->attachmentLock423F58(),
            &application_->types383408(),rig_->roots[modelRole_],rig_->sourceNodes[modelRole_],
            rig_->hierarchy[modelRole_].nodeCount};
        if(modelRole_==Rig::chariot){
            const auto result=coldScenes_.construct(binding,canonical,rig_->main,rig_->hidden,rig_->container);
            failurePC_=result.sourcePC;
            need(result.okay(),"original190/198/194 SceneObject construction failed");
        }
        need(scene_.construct2403A0(binding,canonical,(animationEntity().flags29&1u)!=0).okay(),
            "original2403A0 model constructor failed");
        animation_.fields_.model04=&scene_;
        need(scene_.constructSeparateRoot26E944(canonical,psgRequest_).okay()&&
            scene_.loadSourceRows28CAC0(canonical,rig_->hierarchy[modelRole_]).okay(),
            "original separate-root PSG model construction failed");
        std::array<g::OriginalWreckComponent50LA*,29> storage{};
        const auto peers=peerScenes();
        need(scene_.appendToAnimation154348(outside(storage),animationEntity(),
            peers[0],peers[1],peers[2],peers[3]).okay(),
            "original154348 model append failed");
        modelAppended_=true;
    }

void Owner::prepareAnimation(const g::SceneGraph49P& canonical) {
        failurePC_=0x154350u;
        need(modelAppended_&&input_&&!setup_,"original animation setup order rejected");
        setup_=std::make_unique<setup::Setup>();setup_->prepareTables154140(std::move(*input_));input_.reset();
        need(scene_.uppercaseSourceNames152EA0().okay(),"original scene name enumeration failed");
        auto& f=animation_.fields_;const auto& c=setup_->configuration();
        f.nodeCount1E8=static_cast<std::int32_t>(setup_->records1EC().size());
        f.allocation1EC=setup_->records1EC().data();
        if(setup_->mirrorOpened153450())f.allocation1F0=setup_->mirrorRows1F0().data();
        f.field1F8=setup_->field1F8();
        restoreModelDirectory1543A8();
        failurePC_=0x155730u;
        setup_->capture155730(canonical);f.allocation1F4=setup_->savedMatrices1F4().data();
        const g::OriginalMotionDefaults50LA defaults{c.directory190,c.interpolatePosition20,
            c.interpolateRotation24,c.rotationType28,c.positionType2C};
        failurePC_=0x153610u;
        const auto loaded=motions_.load153610(*motionLibrary_,setup_->parsedConfiguration(),c.motionCount14,
            f.field1E0,defaults,[&](const std::string& requested,g::OriginalAdmittedMotion50LA& out){
                const auto source=archive_->lookup(requested);
                const auto sourceName=archive_->originalMemberName(requested);
                if(!source||!sourceName)return false;
                for(unsigned slot=0;slot<Rig::MotionCount;++slot)
                    if(g::original_motion_detail50LA::equal308F00(*sourceName,rig_->motMembers[slot])&&
                       *source==rig_->mot[slot]&&rig_->motions[slot].data==rig_->mot[slot].data()){
                        out={&rig_->motions[slot],*sourceName};return true;}
                // No incomplete retained-manifest miss is called source absence.
                return false;
            });
        need(loaded.okay(),"original153610 admitted MOT ownership failed");
        f.motion18=motions_.current18();f.motionArray1E4=motions_.motionArray1E4();
        failurePC_=0x1556c0u;setup_->restore1556C0(canonical);
        archive_.reset();
        // Rig invokes this before its normalization, motion and root placement.
        // Cold snapshots stay inside the same owner across delayed Session join.
        captureColdNullcgAfterModel(canonical);
        if(modelRole_==Rig::chariot){
            publishColdTarget2119C4();
            const auto result=coldScenes_.parent211A34(canonical);failurePC_=result.sourcePC;
            need(result.okay(),"original190/194 scene parenting failed");
            constructColdWrists211A4C(canonical);
        }
    }

void Owner::bindColdWrists(g::ChariotIkBindings49T& bindings,g::ChariotIkPair49T& records,
        const g::ChariotIkPair49T*& borrowed)const {
        need(!closed_,"original wrist adapter cannot read retired scene borrows");
        const g::ChariotIkChainBinding49T absent{g::SceneNodeIndex49P::none,g::SceneNodeIndex49P::none,
            g::SceneNodeIndex49P::none,g::SceneNodeIndex49P::none};
        bindings={absent,absent};borrowed=nullptr;
        for(unsigned i=0;i<2;++i){
            const auto& owner=coldWrists_[i];
            need(owner.stage!=ColdWrist::Stage::unreached&&owner.stage!=ColdWrist::Stage::allocated,
                "original wrist constructor did not reach terminal optional boundary");
            if(owner.stage!=ColdWrist::Stage::constructed)continue;
            const auto* allocation=owner.allocation?owner.allocation.get():
                racer_&&!racer_->released()?racer_->wristAllocations_[i].get():nullptr;
            need(allocation&&owner.wrist&&!owner.wrist->released,
                "original wrist allocation or retained scene lifetime ended");
            const std::array<const g::OriginalRacerModelSceneOwner50LA*,5> own{{&scene_,nullptr,nullptr,nullptr,nullptr}};
            const g::OriginalRacerSceneObject50LA* target=nullptr;
            need(findBoundsNullcg28C590(scene_,graph(),own,target,i?"r_hand_dummy":"l_hand_dummy"),
                "original retained wrist target lookup topology stopped");
            auto& b=i?bindings.right:bindings.left;
            b={owner.shoulder->sceneIndex,owner.elbow->sceneIndex,owner.wrist->sceneIndex,
                target?target->sceneIndex:g::SceneNodeIndex49P::none};
            (i?records.right:records.left)=allocation->record;borrowed=&records;
        }
    }

void Owner::normalizeColdEngineBeforeMotion(const g::SceneGraph49P& canonical) {
        failurePC_=0x28ab90u;
        need(modelRole_!=Rig::chariot&&setup_&&!resourcesComplete_&&
            canonical.nodes==arena_->nodes&&canonical.count==rig_->nodeCount&&
            g::validateSceneGraph49P(canonical),"original cold engine normalization boundary rejected");
        const std::array<const g::OriginalRacerModelSceneOwner50LA*,5> own{{&scene_,nullptr,nullptr,nullptr,nullptr}};
        const g::OriginalRacerSceneObject50LA* cg=nullptr;
        need(findBoundsNullcg28C590(scene_,canonical,own,cg)&&cg,
            "original cold engine normalization retained nullcg missing");
        if(modelRole_==Rig::right)need(coldStack0F70_&&coldStack0F70_->rightNullcg==cg,
            "original right normalization must follow its exact cold world capture");
        g::SceneDisplacement49P zero;
        std::memcpy(zero.v,arena_->visualShared.zero434630.v,sizeof zero.v);
        need(g::setLocalTranslation49P(canonical,cg->sceneIndex,zero),
            "original28AB90 cold engine normalization stopped");
    }

void Owner::constructColdMain211F6C(const g::SceneGraph49P& canonical) {
        need(modelRole_==Rig::chariot&&fixture_ && &fixture_->state==arena_&&
            !bodyAllocationAttempted_&&!body340_&&canonical.nodes==arena_->nodes&&
            canonical.count==rig_->nodeCount&&coldScenes_.initialized_==3&&
            coldScenes_.objects_[0]&&coldScenes_.objects_[0]->sceneIndex==rig_->main&&
            fixture_->bindings.lifecycle.main190==rig_->main,
            "original main340 construction destination rejected");
        bodyAllocationAttempted_=true;failurePC_=0x211f6cu;
        body340_=std::make_unique<g::OriginalRacerMainBodyOwner50LA>();
        const auto constructed=body340_->construct167920(*application_,generation_,
            fixture_->state,fixture_->parameters,fixture_->bindings);
        failurePC_=constructed.sourcePC;
        need(constructed.status==g::OriginalWreckComponentStatus50LA::complete,
            "original main340 constructor stopped");
        const auto bound=body340_->bindMain211FB0(*coldScenes_.objects_[0]);
        failurePC_=bound.sourcePC;
        need(bound.status==g::OriginalWreckComponentStatus50LA::complete,
            "original main340 scene288 binding stopped");
        need(!gravityAllocationAttempted_&&!gravity344_,"original gravity344 allocation cannot replay");
        gravityAllocationAttempted_=true;failurePC_=0x211ff8u;
        gravity344_=std::make_unique<g::OriginalRacerGravityOwner50LA>();
        const auto gravityConstructed=gravity344_->construct282670(*application_,generation_,
            fixture_->state.bodies.main,fixture_->parameters.bodies.main);
        failurePC_=gravityConstructed.sourcePC;
        need(gravityConstructed.status==g::OriginalWreckComponentStatus50LA::complete,
            "original gravity344 constructor stopped");
        const auto gravityTargets=gravity344_->allocateOne282750();
        failurePC_=gravityTargets.sourcePC;
        need(gravityTargets.status==g::OriginalWreckComponentStatus50LA::complete,
            "original gravity344 target-array allocation stopped");
    }

// Private prerequisite, not a completed2146C0 or coldReady receipt.
// Immutable handling reservation applies no geometry/body/joint projection;
// each live store is deferred until its actual source stage.
bool Owner::sourceHandlingBodyCurrent(const runtime_startup50fl::Fixture& actual)const noexcept {
    return !closed_&&bank_&&bank_->sealed_&&!bank_->retired_&&bank_->visitDepth_==1u&&
        bank_->currentSlot_==slot_&&!bank_->retirementVisit_&&
        &actual==fixture_&& &actual.state==arena_&&modelRole_==Rig::chariot&&
        body340_&&body340_->entity_.reference00&&body340_->entity_.sessionGeneration==generation_&&
        body340_->canonicalNumericAddress()==&actual.state.bodies.main&&
        body340_->parameters_==&actual.parameters.bodies.main&&
        (body340_->stage_==g::OriginalRacerMainBodyOwner50LA::Stage::sceneBound||
         body340_->stage_==g::OriginalRacerMainBodyOwner50LA::Stage::appended);
}

void Owner::constructSourceTerrain21204C(runtime_startup50fl::Fixture& actual,
    ::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&
        sourceModelPrefixComplete_&&!sourceTerrainAttempted_&&!sourceTerrainWithdrawn_&&sourceConstructorPC_==0x21204cu&&
        racer_&&racer_->prepared()&&racer_->sourceTerrain348_.has_value()&&!*racer_->sourceTerrain348_&&
        racer_->sourceTerrain330_.has_value()&&racer_->sourceWater334_.has_value()&&
        !*racer_->sourceTerrain330_&&!*racer_->sourceWater334_&&
        racer_->body340==body340_.get()&&racer_->gravity344==gravity344_.get()&&
        gravity344_&&gravity344_->appended(),"actual source348 caller/body/World prefix differs");
    auto* stage=bridge.admitSource348CountedConstructor212060(slot_,*body340_);
    need(stage,"actual counted348 namespace/Bank owner unavailable");
    sourceTerrainAttempted_=true;sourceTerrainBridge_=&bridge;
    failurePC_=sourceConstructorPC_=0x212050u;
    //Native allocation admission: failure retains the attempted prefix and does
    //not claim the original allocator's NULL branch or a source348 NULL store.
    sourceTerrain348_=std::make_unique<g::OriginalRacerTerrainQueryOwner50LA>();
    g::SpringVector4_48N clear,up;
    std::memcpy(clear.v,application_->terrainClear434260_.data(),16);
    std::memcpy(up.v,application_->terrainUp434280_.data(),16);
    failurePC_=sourceConstructorPC_=0x212064u;
    const auto constructed=sourceTerrain348_->construct1B0EE0(*stage,*application_,slot_,generation_,
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(sourceTerrain348_.get())),
        original_follow_query_bridge::QueryBridge::sharedSentinel3E02C0(),clear,up);
    failurePC_=constructed.sourcePC;
    need(constructed.status==g::OriginalWreckComponentStatus50LA::complete,"actual counted1B0EE0 stopped");
    racer_->sourceTerrain348_=sourceTerrain348_.get(); //21206C, BEFORE setup call.
    failurePC_=sourceConstructorPC_=0x21207cu;
    //Same declared native registry, not literal guest4342A0 integer recovery.
    const auto& floor=application_->nativeCollisionCategoryBindings_[4];
    need(std::string_view(floor.name)=="FLOOR","actual native FLOOR binding differs");
    const auto bound=sourceTerrain348_->setup1B11B0(*body340_,*racer_->sourceTerrain330_,
        *racer_->sourceWater334_,floor.bit,application_->collisionGlobalValid3E0730_);
    failurePC_=bound.sourcePC;
    need(bound.status==g::OriginalWreckComponentStatus50LA::complete,"actual1B11B0 prefix stopped");
    need(bridge.publishSource348Body128(slot_,*sourceTerrain348_,*body340_),
        "actual348 body borrow publication refused");
    failurePC_=sourceConstructorPC_=0x212090u;
    need(sourceTerrain348_->bindSourceList212090(racer_->sourceList488_,racer_->sourceCount48C_),
        "actual Racer488/48C address binding refused");
    std::array<g::OriginalWreckComponent50LA*,29> objects{};
    const auto domain=outside(objects);
    failurePC_=sourceConstructorPC_=0x2120acu;
    const auto appended=sourceTerrain348_->append2120AC(domain,racer_->racer);
    failurePC_=appended.sourcePC;
    need(appended.status==g::OriginalWreckComponentStatus50LA::complete,"actual source348 append refused");
    sourceTerrainComplete_=true;failurePC_=sourceConstructorPC_=0x2120b4u;
}

void Owner::constructSourceEngineQuery2120B4(runtime_startup50fl::Fixture& actual,
    ::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&
        sourceTerrainComplete_&&!sourceTerrainWithdrawn_&&!sourceEngineQueryAttempted_&&
        !sourceEngineQueryWithdrawn_&&sourceConstructorPC_==0x2120b4u&&racer_&&racer_->prepared()&&
        sourceTerrain348_&&sourceTerrain348_->appended()&&racer_->sourceTerrain348_&&
        *racer_->sourceTerrain348_==sourceTerrain348_.get()&&
        !racer_->sourceEngine34C_&&racer_->sourceAero350_&&!*racer_->sourceAero350_&&
        racer_->sourceQuery690_&&!*racer_->sourceQuery690_&&!nativeAero_&&
        bridge.bankIdentity_==bank_&&bridge.source348CountedOwners_[slot_]==sourceTerrain348_.get()&&
        bridge.actual_&&&bridge.actual_->frame==&actual.state&&!bridge.terminal_&&!bridge.closed_&&
        slot_<bridge.inputs_.collisionRoster.size(),"actual engine/query caller prefix differs");
    sourceEngineQueryAttempted_=true;
    failurePC_=sourceConstructorPC_=0x2120b4u;
    sourceEngine34C_=std::make_unique<g::OriginalRacerEngine34COwner50LA>();
    failurePC_=sourceConstructorPC_=0x2120c8u;
    auto r=sourceEngine34C_->construct2E7510(*application_,*body340_,actual.state.bodies.main,
        actual.parameters.bodies.main,generation_,
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(sourceEngine34C_.get())),
        application_->engineOrigin446800_);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,
        "actual2E7510 constructor stopped");
    racer_->sourceEngine34C_=sourceEngine34C_.get(); //2120D0 BEFORE body bind.
    failurePC_=sourceConstructorPC_=0x2120d8u;r=sourceEngine34C_->bind2E7670(*body340_);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,"actual2E7670 bind stopped");
    std::array<g::OriginalWreckComponent50LA*,29> objects{};
    failurePC_=sourceConstructorPC_=0x2120e8u;r=sourceEngine34C_->append2120E8(outside(objects),racer_->racer);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,"actual34C append stopped");
    failurePC_=sourceConstructorPC_=0x2120f0u;
    sourceAero350_=std::make_unique<g::OriginalRacerAero350Owner50LA>();
    nativeAero_.reset(new g::NativeRacerAeroOwner50LA(g::NativeRacerAeroOwner50LA::FreshSourceTag{},
        *bank_,actual.state,actual.parameters.bodies.main,*body340_,*application_,this,generation_,slot_));
    failurePC_=sourceConstructorPC_=0x212104u;
    r=sourceAero350_->construct16CCD0(*application_,*nativeAero_,generation_,
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(sourceAero350_.get())),
        application_->aeroFlowOrigin3D6B40_,application_->aeroDampingOrigin433BD0_);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,"actual16CCD0 constructor stopped");
    racer_->sourceAero350_=sourceAero350_.get(); //21210C BEFORE actual virtual74 bind.
    failurePC_=sourceConstructorPC_=0x21211cu;r=sourceAero350_->bind100420(*body340_);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,"actual100420 bind stopped");
    failurePC_=sourceConstructorPC_=0x21212cu;r=sourceAero350_->append21212C(outside(objects),racer_->racer);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,"actual350 append stopped");
    //Only native weak custody; no published_ runtime/row/phase readiness.
    bridge.stagedAero_[slot_]=nativeAero_;
    failurePC_=sourceConstructorPC_=0x212140u;
    need(racer_->sourcePod60_&&*racer_->sourcePod60_>0u&&*racer_->sourcePod60_<100u,
        "actual sourcePod60 formatter domain unavailable");
    const auto pod=*racer_->sourcePod60_;
    std::string prefix="Pods\\Pod";prefix.push_back(static_cast<char>('0'+pod/10u));
    prefix.push_back(static_cast<char>('0'+pod%10u));prefix.push_back('\\');
    sourceStack840_=prefix+"podboxm.col"; //212140 uses3B17A8='%spodboxm.col'.
    const auto* collision=bridge.inputs_.collisionRoster[slot_];
    need(collision&&collision==bridge.bodyIdentities_[slot_].collision&&
        collision->binding().memberPrefix==prefix&&bridge.actual_->main.resource==&collision->main(),
        "actual source formatted collision member differs from retained owner");
    failurePC_=sourceConstructorPC_=0x212148u;
    sourceQuery690_=std::make_unique<g::OriginalRacerQuery690Owner50LA>();
    a::OriginalStandardTextureDiagnostic50LA fileDiagnostic;
    need(sourceQuery690_->reserveRetainedFile(*collision,files_,fileDiagnostic),
        "actual690 immutable source file admission refused");
    failurePC_=sourceConstructorPC_=0x21215cu;
    r=sourceQuery690_->construct21215C(*application_,generation_,
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(sourceQuery690_.get())),
        original_follow_query_bridge::QueryBridge::sharedSentinel3E02C0(),
        bridge.actual_->mainFilter.flags31,bridge.actual_->mainFilter.flagsF8);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,"actual69011B190 constructor stopped");
    racer_->sourceQuery690_=sourceQuery690_.get(); //212164 BEFORE configuration/setup.
    failurePC_=sourceConstructorPC_=0x21216cu;
    const auto& physics=application_->nativeCollisionCategoryBindings_[1];
    need(std::string_view(physics.name)=="PHYSICSBODY"&&
        sourceQuery690_->configure21216C(application_->collisionGlobalValid3E0730_,physics.bit),
        "actual690 configuration source prefix stopped");
    failurePC_=sourceConstructorPC_=0x21219cu;
    const bool setup=racer_->scene190&&sourceQuery690_->setup21219C(*racer_,*racer_->scene190,sourceStack840_,
        application_->observeCollisionDirectory36B0F0(),application_->collisionName3E0730_,fileDiagnostic);
    if(!setup&&sourceQuery690_->boundary_!=0x212190u)failurePC_=sourceQuery690_->boundary_;
    need(setup,"actual690 source COL/shape/result setup stopped");
    failurePC_=sourceConstructorPC_=0x2121b8u;r=sourceQuery690_->append2121B8(outside(objects),racer_->racer);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,"actual690 append stopped");
    failurePC_=sourceConstructorPC_=0x2121c8u;
    need(bridge.publishSource690Bounds2121C8(slot_,*racer_,*sourceQuery690_),
        "actual6901202E0 bounds publication stopped");
    sourceEngineQueryComplete_=true;failurePC_=sourceConstructorPC_=0x2121d0u;
}

void Owner::constructSourceQuery6942121D0(runtime_startup50fl::Fixture& actual,
    ::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&
        sourceEngineQueryComplete_&&!sourceEngineQueryWithdrawn_&&!sourceQuery694Attempted_&&
        !sourceQuery694Withdrawn_&&sourceConstructorPC_==0x2121d0u&&racer_&&racer_->prepared()&&
        sourceQuery690_&&sourceQuery690_->appended()&&sourceQuery690_->liveEntity()&&
        racer_->sourceQuery690_&&*racer_->sourceQuery690_==sourceQuery690_.get()&&
        racer_->sourceQuery694_&&!*racer_->sourceQuery694_&&
        bridge.bankIdentity_==bank_&&bridge.actual_&&&bridge.actual_->frame==&actual.state&&
        !bridge.terminal_&&!bridge.closed_&&slot_<bridge.inputs_.collisionRoster.size(),
        "actual694 caller prefix differs");
    const auto& reserved=bridge.stagedPods694_[slot_];
    const auto* collision=bridge.inputs_.collisionRoster[slot_];
    need(collision&&collision==bridge.bodyIdentities_[slot_].collision&&
        reserved.collision==collision&&reserved.shape==&collision->mainTarget().source&&
        reserved.bodyA0==body340_.get()&&
        reserved.generation==generation_&&reserved.slot==slot_&&
        reserved.responseE8==::ra_source_camera50LA::OwnershipBridge::NativePodResponseE8::unpublished,
        "actual694 immutable native reservation differs");
    sourceQuery694Attempted_=true;failurePC_=sourceConstructorPC_=0x2121dcu;
    need(racer_->sourcePod60_&&*racer_->sourcePod60_>0u&&*racer_->sourcePod60_<100u,
        "actual sourcePod60 formatter domain unavailable");
    const auto pod=*racer_->sourcePod60_;
    std::string prefix="Pods\\Pod";prefix.push_back(static_cast<char>('0'+pod/10u));
    prefix.push_back(static_cast<char>('0'+pod%10u));prefix.push_back('\\');
    sourceStack840_=prefix+"podboxf.col"; //2121DC uses3B17B8='%spodboxf.col'.
    need(collision->binding().memberPrefix==prefix,"actual694 source formatted member differs");
    failurePC_=sourceConstructorPC_=0x2121e4u;
    sourceQuery694_=std::make_unique<g::OriginalRacerQuery694Owner50LA>();
    a::OriginalStandardTextureDiagnostic50LA fileDiagnostic;
    need(sourceQuery694_->reserveRetainedFile(*collision,files_,fileDiagnostic),
        "actual694 immutable source file admission refused");
    failurePC_=sourceConstructorPC_=0x2121f8u;
    auto r=sourceQuery694_->construct2121F8(*application_,generation_,
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(sourceQuery694_.get())),
        original_follow_query_bridge::QueryBridge::sharedSentinel3E02C0(),
        bridge.actual_->mainFilter.flags31,bridge.actual_->mainFilter.flagsF8);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,
        "actual69411B190 constructor stopped");
    racer_->sourceQuery694_=sourceQuery694_.get(); //212200 BEFORE mode/category/setup.
    failurePC_=sourceConstructorPC_=0x212208u;
    const auto& physics=application_->nativeCollisionCategoryBindings_[1];
    const auto& category=application_->nativeCollisionCategoryBindings_[7];
    need(std::string_view(physics.name)=="PHYSICSBODY"&&std::string_view(category.name)=="POD"&&
        sourceQuery694_->configure212208(application_->collisionGlobalValid3E0730_,physics.bit,category.bit),
        "actual694 same-App named category setup stopped");
    failurePC_=sourceConstructorPC_=0x212240u;
    const bool setup=racer_->scene190&&sourceQuery694_->setup212240(*racer_,*racer_->scene190,sourceStack840_,
        application_->observeCollisionDirectory36B0F0(),application_->collisionName3E0730_,fileDiagnostic);
    if(!setup&&sourceQuery694_->boundary_!=0x212238u)failurePC_=sourceQuery694_->boundary_;
    need(setup,"actual694 source COL/type0 setup stopped");
    failurePC_=sourceConstructorPC_=0x21224cu;
    need(sourceQuery694_->bindOwners21224C(*racer_,*body340_),"actual6949C/A0 bind stopped");
    std::array<g::OriginalWreckComponent50LA*,29> objects{};
    failurePC_=sourceConstructorPC_=0x212270u;
    r=sourceQuery694_->append212270(outside(objects),racer_->racer);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,
        "actual694 append stopped");
    //The native row remains unpublished/coldBound=false. Counted source694
    //does not grant current collection membership or gameplay query readiness.
    sourceQuery694Complete_=true;failurePC_=sourceConstructorPC_=0x212278u;
}

void Owner::constructSourceContact354212278(runtime_startup50fl::Fixture& actual,
    ::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&
        sourceQuery694Complete_&&!sourceQuery694Withdrawn_&&!sourceContact354Attempted_&&
        !sourceContact354Withdrawn_&&sourceConstructorPC_==0x212278u&&racer_&&racer_->prepared()&&
        !racer_->sourceContact354_&&sourceQuery694_&&sourceQuery694_->appended()&&
        sourceQuery694_->liveEntity()&&racer_->sourceQuery694_&&
        *racer_->sourceQuery694_==sourceQuery694_.get()&&sourceQuery690_&&sourceQuery690_->liveEntity(),
        "actual354 caller prefix differs");
    g::SpringGeneratedParameters48K* generated=nullptr;g::SpringResponseParameters48L* response=nullptr;
    need(bridge.sourceContact354Lanes212278(slot_,*body340_,generated,response)&&generated&&response,
        "actual354 registered native lanes unavailable");
    sourceContact354Attempted_=true;failurePC_=sourceConstructorPC_=0x212278u;
    sourceContact354_=std::make_unique<g::OriginalRacerContact354Owner50LA>();
    //Only immutable traversal storage is admitted here; no source POINT or
    //constructor store precedes actual295A40. Same appended690 is retained.
    need(sourceContact354_->reservePointTraversal(*sourceQuery690_),"actual354 native traversal admission refused");
    failurePC_=sourceConstructorPC_=0x21228cu;
    auto r=sourceContact354_->construct295A40(*application_,*body340_,actual.state.bodies.main,
        *generated,*response,generation_,
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(sourceContact354_.get())),
        application_->springZero4435A0_,bridge.actual_->mainFilter.flags31);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,
        "actual354295A40 constructor stopped");
    racer_->sourceContact354_=sourceContact354_.get(); //212294 BEFORE arrays/body/POINT setup.
    failurePC_=sourceConstructorPC_=0x21229cu;
    const bool allocated=sourceContact354_->allocate295C20();
    if(!allocated)failurePC_=sourceContact354_->boundary_;
    need(allocated,"actual354295C20 forty-record allocation stopped");
    failurePC_=sourceConstructorPC_=0x2122a8u;
    need(sourceContact354_->bind295C80(*body340_),"actual354295C80 body bind stopped");
    failurePC_=sourceConstructorPC_=0x2122bcu;
    const bool points=racer_->scene190&&sourceContact354_->producePoints296190(*sourceQuery690_,*racer_,
        *racer_->scene190,application_->springZero4435A0_);
    if(!points)failurePC_=sourceContact354_->boundary_;
    need(points,"actual354296190 POINT producer stopped");
    failurePC_=sourceConstructorPC_=0x2122c4u;
    need(sourceContact354_->applyCaller2122C4(),"actual354 caller overrides stopped");
    std::array<g::OriginalWreckComponent50LA*,29> objects{};
    failurePC_=sourceConstructorPC_=0x212340u;
    r=sourceContact354_->append212340(outside(objects),racer_->racer);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,
        "actual354 append stopped");
    sourceContact354Complete_=true;failurePC_=sourceConstructorPC_=0x212348u;
}

void Owner::constructSourceDetector358212348(runtime_startup50fl::Fixture& actual,
    ::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&
        sourceContact354Complete_&&!sourceContact354Withdrawn_&&!sourceDetector358Attempted_&&
        !sourceDetector358Withdrawn_&&sourceConstructorPC_==0x212348u&&racer_&&racer_->prepared()&&
        !racer_->sourceDetector358_&&sourceContact354_&&sourceContact354_->appended()&&
        sourceContact354_->liveEntity()&&racer_->sourceContact354_&&
        *racer_->sourceContact354_==sourceContact354_.get()&&sourceQuery690_&&sourceQuery690_->liveEntity()&&
        bridge.bankIdentity_==bank_&&bridge.actual_&&&bridge.actual_->frame==&actual.state&&
        !bridge.terminal_&&!bridge.closed_,"actual358 caller prefix differs");
    sourceDetector358Attempted_=true;failurePC_=sourceConstructorPC_=0x212348u;
    sourceDetector358_=std::make_unique<g::OriginalRacerDetector358Owner50LA>();
    failurePC_=sourceConstructorPC_=0x21235cu;
    auto r=sourceDetector358_->construct295290(*application_,generation_,
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(sourceDetector358_.get())),
        application_->detectorOrigin443550_,bridge.actual_->mainFilter.flags31);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,
        "actual358295290 constructor stopped");
    racer_->sourceDetector358_=sourceDetector358_.get(); //212364 BEFORE actual330/setup load.
    failurePC_=sourceConstructorPC_=0x212370u;
    need(racer_->sourceTerrain330_.has_value(),"actual358 source330 operand unavailable");
    failurePC_=sourceConstructorPC_=0x212384u;
    need(sourceDetector358_->setup2953A0(*sourceContact354_,*racer_,racer_->sourceTerrain330_,
        application_->detectorFrame434630_),"actual3582953A0 null-pair setup stopped");
    failurePC_=sourceConstructorPC_=0x21239cu;
    need(sourceDetector358_->bindAddresses21239C(racer_->sourceCount384_,racer_->sourceCandidate388_),
        "actual358384/388 address bind stopped");
    failurePC_=sourceConstructorPC_=0x2123b0u;
    need(sourceDetector358_->bindQuery2955F0(*sourceQuery690_),
        "actual3582955F0 receiver/callback registration stopped");
    std::array<g::OriginalWreckComponent50LA*,29> objects{};
    failurePC_=sourceConstructorPC_=0x2123c0u;
    r=sourceDetector358_->append2123C0(outside(objects),racer_->racer);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,
        "actual358 append stopped");
    //Typed294BB0 registration is not dispatch. Candidate388 contents and
    //extent stay unknown; no122660 query or later contact update is admitted.
    sourceDetector358Complete_=true;failurePC_=sourceConstructorPC_=0x2123c8u;
}

void Owner::constructSourceAuxiliary2123C8(runtime_startup50fl::Fixture& actual,
    ::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&
        sourceDetector358Complete_&&!sourceDetector358Withdrawn_&&!sourceAuxiliaryAttempted_&&
        !sourceAuxiliaryWithdrawn_&&sourceConstructorPC_==0x2123c8u&&racer_&&racer_->prepared()&&
        !racer_->sourceQuery7A4_&&!racer_->sourceQuery79C_&&sourceDetector358_&&
        sourceDetector358_->appended()&&sourceDetector358_->liveEntity()&&racer_->sourceDetector358_&&
        *racer_->sourceDetector358_==sourceDetector358_.get(),"actual auxiliary query caller prefix differs");
    failurePC_=sourceConstructorPC_=0x2123c8u;
    need(bridge.adoptSourceAuxiliaryQueries2123C8(slot_,*body340_),
        "actual source arrays cannot adopt active native fixed recovery backing");
    sourceAuxiliaryAttempted_=true; //After native custody, BEFORE any source allocation.
    sourceQuery7A4_=std::make_unique<g::OriginalRacerQuery7A4Owner50LA>();
    failurePC_=sourceConstructorPC_=0x2123dcu;
    auto r=sourceQuery7A4_->construct2123DC(*application_,generation_,
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(sourceQuery7A4_.get())),
        original_follow_query_bridge::QueryBridge::sharedSentinel3E02C0(),
        bridge.actual_->movement->filter.flags31,bridge.actual_->movement->filter.flagsF8);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,
        "actual7A411B190 constructor stopped");
    racer_->sourceQuery7A4_=sourceQuery7A4_.get(); //2123E4 BEFORE mode and append.
    failurePC_=sourceConstructorPC_=0x2123ecu;
    need(sourceQuery7A4_->mode3_2123EC(application_->collisionGlobalValid3E0730_),"actual7A4 mode3 stopped");
    std::array<g::OriginalWreckComponent50LA*,29> objects{};
    failurePC_=sourceConstructorPC_=0x212408u;
    r=sourceQuery7A4_->append212408(outside(objects),*racer_);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,
        "actual7A4 append BEFORE shape stopped");
    failurePC_=sourceConstructorPC_=0x212424u;
    const auto& physics=application_->nativeCollisionCategoryBindings_[1];
    need(std::string_view(physics.name)=="PHYSICSBODY"&&sourceQuery7A4_->bindReceiver212424(*racer_,physics.bit),
        "actual7A4 typed20EE10 receiver/category registration stopped");
    failurePC_=sourceConstructorPC_=0x212444u;
    const bool shape=sourceQuery7A4_->shape11C710_212444(original_follow_query_bridge::QueryBridge::sharedSentinel3E02C0());
    if(!shape)failurePC_=sourceQuery7A4_->boundary_;
    need(shape,"actual7A4 source type3 allocation stopped");
    failurePC_=sourceConstructorPC_=0x212450u;
    const auto& statics=application_->nativeCollisionCategoryBindings_[2];
    need(std::string_view(statics.name)=="STATICOBJECT","actual7A4 same-App STATICOBJECT unavailable");
    const bool category=sourceQuery7A4_->mode0StaticCategory212450(statics.bit);
    if(!category)failurePC_=sourceQuery7A4_->boundary_;
    need(category,"actual7A4 mode0 category allocation stopped");
    failurePC_=sourceConstructorPC_=0x212468u;
    sourceQuery79C_=std::make_unique<g::OriginalRacerQuery79COwner50LA>();
    failurePC_=sourceConstructorPC_=0x21247cu;
    r=sourceQuery79C_->construct21247C(*application_,generation_,
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(sourceQuery79C_.get())),
        original_follow_query_bridge::QueryBridge::sharedSentinel3E02C0(),
        bridge.actual_->mainFilter.flags31,bridge.actual_->mainFilter.flagsF8);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,
        "actual79C11B190 constructor stopped");
    racer_->sourceQuery79C_=sourceQuery79C_.get(); //212484 BEFORE mode/category/shape.
    failurePC_=sourceConstructorPC_=0x21248cu;
    const auto& trigger=application_->nativeCollisionCategoryBindings_[3];
    need(std::string_view(trigger.name)=="TRIGGER"&&
        sourceQuery79C_->configure21248C(application_->collisionGlobalValid3E0730_,trigger.bit),
        "actual79C mode2/trigger category stopped");
    failurePC_=sourceConstructorPC_=0x2124b8u;
    const bool sphere=sourceQuery79C_->shape11C0B0_2124B8(application_->detectorFrame434630_,
        std::bit_cast<float>(std::uint32_t{0x447a0000u}));
    if(!sphere)failurePC_=sourceQuery79C_->boundary_;
    need(sphere,"actual79C source shape5/result allocation stopped");
    failurePC_=sourceConstructorPC_=0x2124d4u;
    r=sourceQuery79C_->append2124D4(outside(objects),*racer_);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,"actual79C append stopped");
    //Real dynamic arrays are NOT copied into inactive Fresh fixed arrays.
    //No source span/runtime adapter or current collection membership is granted.
    sourceAuxiliaryComplete_=true;failurePC_=sourceConstructorPC_=0x2124dcu;
}

void Owner::constructSourceSecondary2124DC(runtime_startup50fl::Fixture& actual) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&
        sourceGeometryReserved_&&!sourceGeometryComplete_&&!jointAllocationAttempted_&&!joint360_&&
        sourceAuxiliaryComplete_&&!sourceAuxiliaryWithdrawn_&&!sourceSecondaryAttempted_&&
        !sourceSecondaryWithdrawn_&&sourceConstructorPC_==0x2124dcu&&racer_&&racer_->prepared()&&
        racer_->chariotModel1B4==&scene_&&scene_.model_.constructed&&!scene_.model_.released&&
        nativeSecondaryContacts_&&!nativeSecondaryContacts_->revoked_&&
        nativeSecondaryContacts_->bank_==bank_&&nativeSecondaryContacts_->frame_==arena_&&
        nativeSecondaryContacts_->parameters_==&actual.parameters.bodies.secondary&&
        nativeSecondaryContacts_->slot_==slot_&&nativeSecondaryContacts_->generation_==generation_&&
        nativeSecondaryContacts_->physical19C_==actual.bindings.lifecycle.physical19C&&
        actual.bindings.lifecycle.physical19C==rig_->physical,
        "actual secondary positive1B4 owner/current slot or fresh chronology rejected");
    // Positive actual1B4 branch only. No authored-count or absent-owner NULL
    // branch substitutes for the already published chariot Model.
    sourceSecondaryAttempted_=true;failurePC_=0x2124e8u;
    auto sceneResult=coldScenes_.constructSecondary2124E8(graph(),rig_->physical);
    failurePC_=sceneResult.sourcePC;need(sceneResult.okay(),"actual19C constructor stopped");
    racer_->sourceScene19C_=coldScenes_.secondary19C_.get(); //212504
    sceneResult=coldScenes_.initializeSecondary212514();failurePC_=sceneResult.sourcePC;
    need(sceneResult.okay(),"actual19C identity initializer stopped");
    std::array<g::OriginalWreckComponent50LA*,29> objects{};
    sceneResult=coldScenes_.appendSecondary212530(outside(objects),racer_->racer);
    failurePC_=sceneResult.sourcePC;need(sceneResult.okay(),"actual19C append stopped");
    failurePC_=0x212538u;
    sourceBody35C_=std::make_unique<g::OriginalRacerSecondaryBodyOwner50LA>();
    auto bodyResult=sourceBody35C_->construct167920(*application_,generation_,actual.state,
        actual.parameters,actual.bindings,application_->bodyOrigin433AD0_,
        application_->bodyReset433B80_,application_->bodyQuaternion382B60_);
    failurePC_=bodyResult.sourcePC;need(bodyResult.status==g::OriginalWreckComponentStatus50LA::complete,
        "actual35C constructor stopped");
    racer_->sourceBody35C_=sourceBody35C_.get(); //212554
    failurePC_=0x212564u;need(sourceBody35C_->initialize212564(),"actual35C identity initializer stopped");
    bodyResult=sourceBody35C_->append212574(outside(objects),*racer_);failurePC_=bodyResult.sourcePC;
    need(bodyResult.status==g::OriginalWreckComponentStatus50LA::complete,"actual35C append stopped");
    bodyResult=sourceBody35C_->bindScene21257C(*coldScenes_.secondary19C_);failurePC_=bodyResult.sourcePC;
    need(bodyResult.status==g::OriginalWreckComponentStatus50LA::complete,"actual35C limits and scene bind stopped");
    // No1B4 slot90 refresh, authored mass/inertia, target2F0 or joint setup
    // is adopted from immutable Rig geometry or native backing defaults.
    sourceSecondaryComplete_=true;failurePC_=sourceConstructorPC_=0x2125c4u;
}

void Owner::constructSourceSecondaryContacts212720(runtime_startup50fl::Fixture& actual,
    ::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&
        sourceSecondarySetupComplete_&&!sourceSecondarySetupWithdrawn_&&sourceGeometryComplete_&&
        !sourceSecondaryContactsAttempted_&&!sourceSecondaryContactsWithdrawn_&&sourceConstructorPC_==0x212720u&&
        racer_&&racer_->prepared()&&sourceBody35C_&&sourceBody35C_->appended()&&
        racer_->sourceScene19C_&&*racer_->sourceScene19C_&&racer_->sourceBody35C_&&
        *racer_->sourceBody35C_==sourceBody35C_.get()&&joint360_&&joint360_->sourceBComplete_&&
        !racer_->sourceQuery364_&&!racer_->sourceContact368_&&!racer_->sourceDetector36C_&&
        sourceQuery690_&&sourceQuery690_->appended()&&sourceQuery694_&&sourceQuery694_->appended()&&
        sourceQuery7A4_&&sourceQuery7A4_->appended()&&
        bridge.bankIdentity_==bank_&&bridge.actual_&&&bridge.actual_->frame==&actual.state&&
        !bridge.terminal_&&!bridge.closed_&&slot_<bridge.inputs_.collisionRoster.size(),
        "actual secondary query caller prefix differs");
    sourceSecondaryContactsAttempted_=true;failurePC_=sourceConstructorPC_=0x212720u;
    need(racer_->sourcePod60_&&*racer_->sourcePod60_>0u&&*racer_->sourcePod60_<100u,
        "actual secondary sourcePod60 formatter domain unavailable");
    const auto pod=*racer_->sourcePod60_;
    std::string prefix="Pods\\Pod";prefix.push_back(static_cast<char>('0'+pod/10u));
    prefix.push_back(static_cast<char>('0'+pod%10u));prefix.push_back('\\');
    sourceStack840_=prefix+"chariotm.col"; //21272C formats actual3B17C8 request.
    const auto* collision=bridge.inputs_.collisionRoster[slot_];
    need(collision&&collision==bridge.bodyIdentities_[slot_].collision&&
        collision->binding().memberPrefix==prefix&&bridge.actual_->secondary.resource==&collision->secondary(),
        "actual secondary formatted member differs from retained owner");
    auto& scene=**racer_->sourceScene19C_;
    failurePC_=sourceConstructorPC_=0x212734u;
    sourceQuery364_=std::make_unique<g::OriginalRacerQuery364Owner50LA>();
    a::OriginalStandardTextureDiagnostic50LA fileDiagnostic;
    need(sourceQuery364_->reserveRetainedFile(*collision,files_,fileDiagnostic),
        "actual364 immutable source file admission refused");
    failurePC_=sourceConstructorPC_=0x212748u;
    auto result=sourceQuery364_->construct212748(*application_,generation_,
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(sourceQuery364_.get())),
        original_follow_query_bridge::QueryBridge::sharedSentinel3E02C0(),
        bridge.actual_->secondaryFilter.flags31,bridge.actual_->secondaryFilter.flagsF8);
    failurePC_=result.sourcePC;need(result.status==g::OriginalWreckComponentStatus50LA::complete,"actual364 constructor stopped");
    racer_->sourceQuery364_=sourceQuery364_.get(); //212750 BEFORE mode/setup/category.
    failurePC_=sourceConstructorPC_=0x212758u;
    const auto& physics=application_->nativeCollisionCategoryBindings_[1];
    const auto& podCategory=application_->nativeCollisionCategoryBindings_[7];
    need(std::string_view(physics.name)=="PHYSICSBODY"&&
        sourceQuery364_->configure212758(application_->collisionGlobalValid3E0730_,physics.bit),
        "actual364 same-App physics category stopped");
    failurePC_=sourceConstructorPC_=0x212788u;
    const bool setup=sourceQuery364_->setup212788(*racer_,scene,*sourceBody35C_,sourceStack840_,
        application_->observeCollisionDirectory36B0F0(),application_->collisionName3E0730_,fileDiagnostic);
    if(!setup)failurePC_=sourceQuery364_->boundary_;
    need(setup,"actual364 source COL/type2 setup stopped");
    failurePC_=sourceConstructorPC_=0x212794u;
    need(std::string_view(podCategory.name)=="POD"&&sourceQuery364_->mode1Pod212794(podCategory.bit),
        "actual364 mode1 POD list stopped");
    std::array<g::OriginalWreckComponent50LA*,29> objects{};
    failurePC_=sourceConstructorPC_=0x2127c0u;
    result=sourceQuery364_->append2127C0(outside(objects),racer_->racer);
    failurePC_=result.sourcePC;need(result.status==g::OriginalWreckComponentStatus50LA::complete,"actual364 append stopped");
    failurePC_=sourceConstructorPC_=0x2127c8u;
    g::SpringGeneratedParameters48K* generated=nullptr;g::SpringResponseParameters48L* response=nullptr;
    need(bridge.sourceContact368Lanes2127C8(slot_,*sourceBody35C_,*racer_,generated,response)&&generated&&response,
        "actual368 registered secondary lanes unavailable");
    sourceContact368_=std::make_unique<g::OriginalRacerContact368Owner50LA>();
    need(sourceContact368_->reservePointTraversal(*sourceQuery364_),"actual368 native traversal admission refused");
    failurePC_=sourceConstructorPC_=0x2127dcu;
    result=sourceContact368_->construct295A40(*application_,*sourceBody35C_,actual.state.bodies.secondary,
        *generated,*response,generation_,
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(sourceContact368_.get())),
        application_->springZero4435A0_,bridge.actual_->secondaryFilter.flags31);
    failurePC_=result.sourcePC;need(result.status==g::OriginalWreckComponentStatus50LA::complete,"actual368 constructor stopped");
    racer_->sourceContact368_=sourceContact368_.get(); //2127E4 BEFORE both60-record allocations.
    failurePC_=sourceConstructorPC_=0x2127ecu;
    const bool allocated=sourceContact368_->allocate295C20();if(!allocated)failurePC_=sourceContact368_->boundary_;
    need(allocated,"actual368 sixty-record allocation stopped");
    failurePC_=sourceConstructorPC_=0x2127f8u;
    need(sourceContact368_->bind295C80(*sourceBody35C_),"actual368 body bind stopped");
    failurePC_=sourceConstructorPC_=0x21280cu;
    const bool points=sourceContact368_->producePoints296190(*sourceQuery364_,*racer_,scene,application_->springZero4435A0_);
    if(!points)failurePC_=sourceContact368_->boundary_;
    need(points,"actual368 POINT producer stopped");
    failurePC_=sourceConstructorPC_=0x212814u;
    need(sourceContact368_->applyCaller212814(),"actual368 caller overrides stopped");
    failurePC_=sourceConstructorPC_=0x21288cu;
    result=sourceContact368_->append21288C(outside(objects),racer_->racer);
    failurePC_=result.sourcePC;need(result.status==g::OriginalWreckComponentStatus50LA::complete,"actual368 append stopped");
    failurePC_=sourceConstructorPC_=0x212894u;
    sourceDetector36C_=std::make_unique<g::OriginalRacerDetector36COwner50LA>();
    failurePC_=sourceConstructorPC_=0x2128a8u;
    result=sourceDetector36C_->construct295290(*application_,generation_,
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(sourceDetector36C_.get())),
        application_->detectorOrigin443550_,bridge.actual_->secondaryFilter.flags31);
    failurePC_=result.sourcePC;need(result.status==g::OriginalWreckComponentStatus50LA::complete,"actual36C constructor stopped");
    racer_->sourceDetector36C_=sourceDetector36C_.get(); //2128B0 BEFORE setup.
    failurePC_=sourceConstructorPC_=0x2128bcu;
    need(racer_->sourceTerrain330_.has_value(),"actual36C source330 operand unavailable");
    failurePC_=sourceConstructorPC_=0x2128d0u;
    need(sourceDetector36C_->setup2953A0(*sourceContact368_,*racer_,racer_->sourceTerrain330_,
        application_->detectorFrame434630_),"actual36C null-pair setup stopped");
    failurePC_=sourceConstructorPC_=0x2128e8u;
    need(sourceDetector36C_->bindAddresses2128E8(racer_->sourceCount384_,racer_->sourceCandidate388_),
        "actual36C384/388 address bind stopped");
    failurePC_=sourceConstructorPC_=0x2128fcu;
    need(sourceDetector36C_->bindQuery2955F0(*sourceQuery364_),"actual36C receiver registration stopped");
    failurePC_=sourceConstructorPC_=0x21290cu;
    result=sourceDetector36C_->append21290C(outside(objects),racer_->racer);
    failurePC_=result.sourcePC;need(result.status==g::OriginalWreckComponentStatus50LA::complete,"actual36C append stopped");
    //Four actual123EB0 calls. No rollback or replay after any prior insertion.
    failurePC_=sourceConstructorPC_=0x212918u;
    need(sourceQuery690_->appendIdentity123EB0(sourceQuery364_->base_,sourceQuery364_->entity()),"actual690 excludes364 stopped");
    failurePC_=sourceConstructorPC_=0x212924u;
    need(sourceQuery364_->appendIdentity123EB0(sourceQuery690_->base_,sourceQuery690_->entity()),"actual364 excludes690 stopped");
    failurePC_=sourceConstructorPC_=0x212930u;
    need(sourceQuery364_->appendIdentity123EB0(sourceQuery694_->base_,sourceQuery694_->entity()),"actual364 excludes694 stopped");
    failurePC_=sourceConstructorPC_=0x21293cu;
    need(sourceQuery364_->appendIdentity123EB0(sourceQuery7A4_->base_,sourceQuery7A4_->entity()),"actual364 excludes7A4 stopped");
    //Shared positive/negative1B4 suffix. This represented route is positive;
    //it does not execute the source21294C negative-arm35C/838/83C stores.
    failurePC_=sourceConstructorPC_=0x212960u;
    need(sourceQuery690_->appendIdentity123EB0(sourceQuery694_->base_,sourceQuery694_->entity()),"actual690 excludes694 stopped");
    failurePC_=sourceConstructorPC_=0x21296cu;
    need(sourceQuery690_->appendIdentity123EB0(sourceQuery7A4_->base_,sourceQuery7A4_->entity()),"actual690 excludes7A4 stopped");
    failurePC_=sourceConstructorPC_=0x212978u;
    need(sourceTerrain348_&&sourceTerrain348_->appended()&&
        sourceQuery690_->appendIdentity123EB0(sourceTerrain348_->base_,sourceTerrain348_->entity()),"actual690 excludes348 stopped");
    failurePC_=sourceConstructorPC_=0x212984u;
    need(sourceQuery694_->appendIdentity123EB0(sourceQuery690_->base_,sourceQuery690_->entity()),"actual694 excludes690 stopped");
    failurePC_=sourceConstructorPC_=0x212990u;
    need(sourceQuery694_->appendIdentity123EB0(sourceQuery7A4_->base_,sourceQuery7A4_->entity()),"actual694 excludes7A4 stopped");
    failurePC_=sourceConstructorPC_=0x21299cu;
    need(sourceQuery694_->appendIdentity123EB0(sourceTerrain348_->base_,sourceTerrain348_->entity()),"actual694 excludes348 stopped");
    sourceSecondaryContactsComplete_=true;failurePC_=sourceConstructorPC_=0x2129a4u;
}

void Owner::finishSourceWreckConfig2129A4(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceSecondaryContactsComplete_&&!sourceSecondaryContactsWithdrawn_&&
        !sourceWreckConfigAttempted_&&sourceConstructorPC_==0x2129a4u&&racer_&&rig_&&
        sourceHandlingPhy_==rig_->phy,"actual WreckConfig source caller differs");
    sourceWreckConfigAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual WreckConfig Bank/body differs");
        std::uint32_t pc=0x2129a4u;
        const bool done=bridge.loadFreshSourceWreckConfig2129A4(slot_,*racer_,sourceHandlingPhy_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x212a50u,"actual WreckConfig/first collection source prefix stopped");
        sourceWreckConfigComplete_=true;
    });
}

void Owner::finishSourceWreckPiece212A50(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckConfigComplete_&&!sourceWreckPieceAttempted_&&
        sourceConstructorPC_==0x212a50u&&racer_&&rig_&&sourceHandlingPhy_==rig_->phy,
        "actual first Wreck piece caller differs");
    sourceWreckPieceAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck piece Bank/body differs");
        std::uint32_t pc=0x212a50u;
        const bool done=bridge.loadFreshSourceWreckPiece212A50(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x165e90u,"actual first Wreck piece/model source prefix stopped");
        sourceWreckPieceComplete_=true;
    });
}

void Owner::finishSourceWreckBody165E90(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckPieceComplete_&&!sourceWreckBodyAttempted_&&
        sourceConstructorPC_==0x165e90u&&racer_,"actual first Wreck body caller differs");
    sourceWreckBodyAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck body Bank/body differs");
        std::uint32_t pc=0x165e90u;
        const bool done=bridge.loadFreshSourceWreckBody165E90(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x166018u,"actual first Wreck D0/D4 prefix stopped");
        sourceWreckBodyComplete_=true;
    });
}

void Owner::finishSourceWreckE0166018(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckBodyComplete_&&!sourceWreckE0Attempted_&&
        sourceConstructorPC_==0x166018u&&racer_,"actual first Wreck E0 caller differs");
    sourceWreckE0Attempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck E0 Bank/body differs");
        std::uint32_t pc=0x166018u;
        const bool done=bridge.loadFreshSourceWreckE0166018(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x166074u,"actual first Wreck E0 constructor prefix stopped");
        sourceWreckE0Complete_=true;
    });
}

void Owner::finishSourceWreckSecondE0Shape166074(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondE0Complete_&&!sourceWreckSecondE0LoadAttempted_&&
        sourceConstructorPC_==0x166074u&&racer_,"actual second Wreck E0 caller differs");
    sourceWreckSecondE0LoadAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck E0 Bank/body differs");
        std::uint32_t pc=0x166074u;
        const bool done=bridge.loadFreshSourceWreckSecondE0Shape166074(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x16607cu,"actual second Wreck E0 native transport/shape prefix stopped");
        sourceWreckSecondE0LoadComplete_=true;
    });
}

void Owner::finishSourceWreckE0Shape166074(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckE0Complete_&&!sourceWreckE0LoadAttempted_&&
        sourceConstructorPC_==0x166074u&&racer_,"actual first Wreck E0 caller differs");
    sourceWreckE0LoadAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck E0 Bank/body differs");
        std::uint32_t pc=0x166074u;
        const bool done=bridge.loadFreshSourceWreckE0Shape166074(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x16607cu,"actual first Wreck E0 native transport/shape prefix stopped");
        sourceWreckE0LoadComplete_=true;
    });
}

void Owner::finishSourceWreckSecondE0Prepare16607C(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondE0LoadComplete_&&!sourceWreckSecondE0PrepareAttempted_&&
        sourceConstructorPC_==0x16607cu&&racer_,"actual second Wreck E0 caller differs");
    sourceWreckSecondE0PrepareAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck E0 Bank/body differs");
        std::uint32_t pc=0x16607cu;
        const bool done=bridge.prepareAppendFreshSourceWreckSecondE016607C(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x1660a0u,"actual second Wreck E0 two-pass preparation/append prefix stopped");
        sourceWreckSecondE0PrepareComplete_=true;
    });
}

void Owner::finishSourceWreckE0Prepare16607C(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckE0LoadComplete_&&!sourceWreckE0PrepareAttempted_&&
        sourceConstructorPC_==0x16607cu&&racer_,"actual first Wreck E0 caller differs");
    sourceWreckE0PrepareAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck E0 Bank/body differs");
        std::uint32_t pc=0x16607cu;
        const bool done=bridge.prepareAppendFreshSourceWreckE016607C(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x1660a0u,"actual first Wreck E0 two-pass preparation/append prefix stopped");
        sourceWreckE0PrepareComplete_=true;
    });
}

void Owner::finishSourceWreckSecondDC1660A0(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondE0PrepareComplete_&&!sourceWreckSecondDCAttempted_&&
        sourceConstructorPC_==0x1660a0u&&racer_,"actual second Wreck DC caller differs");
    sourceWreckSecondDCAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck DC Bank/body differs");
        std::uint32_t pc=0x1660a0u;
        const bool done=bridge.constructFreshSourceWreckSecondDC1660A0(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x16616cu,"actual second Wreck DC constructor/POINT/append prefix stopped");
        sourceWreckSecondDCComplete_=true;
    });
}

void Owner::finishSourceWreckDC1660A0(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckE0PrepareComplete_&&!sourceWreckDCAttempted_&&
        sourceConstructorPC_==0x1660a0u&&racer_,"actual first Wreck DC caller differs");
    sourceWreckDCAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck DC Bank/body differs");
        std::uint32_t pc=0x1660a0u;
        const bool done=bridge.constructFreshSourceWreckDC1660A0(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x16616cu,"actual first Wreck DC constructor/POINT/append prefix stopped");
        sourceWreckDCComplete_=true;
    });
}

void Owner::finishSourceWreckSecondD816616C(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondDCComplete_&&!sourceWreckSecondD8Attempted_&&
        sourceConstructorPC_==0x16616cu&&racer_,"actual second Wreck D8 caller differs");
    sourceWreckSecondD8Attempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck D8 Bank/body differs");
        std::uint32_t pc=0x16616cu;
        const bool done=bridge.constructFreshSourceWreckSecondD816616C(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x1661d8u,"actual second Wreck D8 constructor/setup/append prefix stopped");
        sourceWreckSecondD8Complete_=true;
    });
}

void Owner::finishSourceWreckD816616C(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckDCComplete_&&!sourceWreckD8Attempted_&&
        sourceConstructorPC_==0x16616cu&&racer_,"actual first Wreck D8 caller differs");
    sourceWreckD8Attempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck D8 Bank/body differs");
        std::uint32_t pc=0x16616cu;
        const bool done=bridge.constructFreshSourceWreckD816616C(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x1661d8u,"actual first Wreck D8 constructor/setup/append prefix stopped");
        sourceWreckD8Complete_=true;
    });
}

void Owner::finishSourceWreckSecondE41661D8(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondD8Complete_&&!sourceWreckSecondE4Attempted_&&
        sourceConstructorPC_==0x1661d8u&&racer_,"actual second Wreck E4 caller differs");
    sourceWreckSecondE4Attempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck E4 Bank/body differs");
        std::uint32_t pc=0x1661d8u;
        const bool done=bridge.constructLoadFreshSourceWreckSecondE41661D8(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x16629cu,"actual second Wreck E4 constructor/Type0 native transport prefix stopped");
        sourceWreckSecondE4Complete_=true;
    });
}

void Owner::finishSourceWreckE41661D8(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckD8Complete_&&!sourceWreckE4Attempted_&&
        sourceConstructorPC_==0x1661d8u&&racer_,"actual first Wreck E4 caller differs");
    sourceWreckE4Attempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck E4 Bank/body differs");
        std::uint32_t pc=0x1661d8u;
        const bool done=bridge.constructLoadFreshSourceWreckE41661D8(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x16629cu,"actual first Wreck E4 constructor/Type0 native transport prefix stopped");
        sourceWreckE4Complete_=true;
    });
}

void Owner::finishSourceWreckSecondE4Prepare16629C(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondE4Complete_&&!sourceWreckSecondE4PrepareAttempted_&&
        sourceConstructorPC_==0x16629cu&&racer_,"actual second Wreck E4 caller differs");
    sourceWreckSecondE4PrepareAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck E4 Bank/body differs");
        std::uint32_t pc=0x16629cu;
        const bool done=bridge.prepareAppendFreshSourceWreckSecondE416629C(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x1662e4u,"actual second Wreck E4 callbacks/Type0 preparation/append prefix stopped");
        sourceWreckSecondE4PrepareComplete_=true;
    });
}

void Owner::finishSourceWreckE4Prepare16629C(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckE4Complete_&&!sourceWreckE4PrepareAttempted_&&
        sourceConstructorPC_==0x16629cu&&racer_,"actual first Wreck E4 caller differs");
    sourceWreckE4PrepareAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck E4 Bank/body differs");
        std::uint32_t pc=0x16629cu;
        const bool done=bridge.prepareAppendFreshSourceWreckE416629C(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x1662e4u,"actual first Wreck E4 callbacks/Type0 preparation/append prefix stopped");
        sourceWreckE4PrepareComplete_=true;
    });
}

void Owner::bindSourceWreckSecondEffectInputs2CB618(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondMutualComplete_&&!sourceWreckSecondEffectInputsAttempted_&&
        sourceConstructorPC_==0x2cb618u&&racer_,"actual second Wreck shared context/parent caller differs");
    sourceWreckSecondEffectInputsAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck shared context/parent Bank/body differs");
        std::uint32_t pc=0x2cb618u;
        const bool done=bridge.bindFreshSourceWreckSecondEffectInputs2CB618(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cb628u,"actual second Wreck shared context/parent binding prefix stopped");
        sourceWreckSecondEffectInputsComplete_=true;
    });
}

void Owner::finishSourceWreckSecondMutual166390(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondE8Complete_&&!sourceWreckSecondMutualAttempted_&&
        sourceConstructorPC_==0x166390u&&racer_,"actual second Wreck mutual/derived caller differs");
    sourceWreckSecondMutualAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck mutual/derived Bank/body differs");
        std::uint32_t pc=0x166390u;
        const bool done=bridge.constructFreshSourceWreckSecondMutual166390(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cb618u,"actual second Wreck mutual/derived selectedDamage prefix stopped");
        sourceWreckSecondMutualComplete_=true;
    });
}

void Owner::finishSourceWreckSecondE81662E4(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondE4PrepareComplete_&&!sourceWreckSecondE8Attempted_&&
        sourceConstructorPC_==0x1662e4u&&racer_,"actual second Wreck E8 caller differs");
    sourceWreckSecondE8Attempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck E8 Bank/body differs");
        std::uint32_t pc=0x1662e4u;
        const bool done=bridge.constructFreshSourceWreckSecondE81662E4(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x166390u,"actual second Wreck E8 constructor/append/Type3/category prefix stopped");
        sourceWreckSecondE8Complete_=true;
    });
}

void Owner::finishSourceWreckE81662E4(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckE4PrepareComplete_&&!sourceWreckE8Attempted_&&
        sourceConstructorPC_==0x1662e4u&&racer_,"actual first Wreck E8 caller differs");
    sourceWreckE8Attempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck E8 Bank/body differs");
        std::uint32_t pc=0x1662e4u;
        const bool done=bridge.constructFreshSourceWreckE81662E4(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x166390u,"actual first Wreck E8 constructor/append/Type3/category prefix stopped");
        sourceWreckE8Complete_=true;
    });
}

void Owner::finishSourceWreckSecondRemainingPairs2CB6CC(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondFirstPairComplete_&&!sourceWreckSecondRemainingPairsAttempted_&&
        sourceConstructorPC_==0x2cb6ccu&&racer_,"actual second Wreck remaining pairs caller differs");
    sourceWreckSecondRemainingPairsAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck remaining pairs Bank/body differs");
        std::uint32_t pc=0x2cb6ccu;
        const bool done=bridge.continueFreshSourceWreckSecondPairs2CB6CC(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cb7e0u,"actual second Wreck remaining pairs indexed source prefix stopped");
        sourceWreckSecondRemainingPairsComplete_=true;
    });
}

void Owner::finishSourceWreckRemainingPairs2CB6CC(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckFirstPairComplete_&&!sourceWreckRemainingPairsAttempted_&&
        sourceConstructorPC_==0x2cb6ccu&&racer_,"actual first Wreck remaining pairs caller differs");
    sourceWreckRemainingPairsAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck remaining pairs Bank/body differs");
        std::uint32_t pc=0x2cb6ccu;
        const bool done=bridge.continueFreshSourceWreckPairs2CB6CC(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cb7e0u,"actual first Wreck remaining pairs indexed source prefix stopped");
        sourceWreckRemainingPairsComplete_=true;
    });
}

void Owner::finishSourceWreckFire2CB720(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSmokeReadComplete_&&!sourceWreckFireAttempted_&&
        sourceConstructorPC_==0x2cb720u&&racer_,"actual first Wreck first fire caller differs");
    sourceWreckFireAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck first fire Bank/body differs");
        std::uint32_t pc=0x2cb720u;
        const bool done=bridge.constructFreshSourceWreckFire2CB720(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x15e024u,"actual first Wreck first fire constructor/setup prefix stopped");
        sourceWreckFireComplete_=true;
    });
}

void Owner::finishSourceWreckSecondFirstPair2CB778(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondSuccessorCallerComplete_&&!sourceWreckSecondFirstPairAttempted_&&
        sourceConstructorPC_==0x2cb778u&&racer_,"actual second Wreck first pair caller differs");
    sourceWreckSecondFirstPairAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck first pair Bank/body differs");
        std::uint32_t pc=0x2cb778u;
        const bool done=bridge.stopFreshSourceWreckSecondFirstPair2CB778(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&(pc==0x2cb6ccu||pc==0x2cb7e0u),"actual second Wreck first pair Stop/loop prefix stopped");
        sourceWreckSecondFirstPairComplete_=true;
    });
}

void Owner::finishSourceWreckFirstPair2CB778(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckFireReadComplete_&&!sourceWreckFirstPairAttempted_&&
        sourceConstructorPC_==0x2cb778u&&racer_,"actual first Wreck first pair caller differs");
    sourceWreckFirstPairAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck first pair Bank/body differs");
        std::uint32_t pc=0x2cb778u;
        const bool done=bridge.stopFreshSourceWreckFirstPair2CB778(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&(pc==0x2cb6ccu||pc==0x2cb7e0u),"actual first Wreck first pair Stop/loop prefix stopped");
        sourceWreckFirstPairComplete_=true;
    });
}

void Owner::finishSourceWreckSecondE0166018(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondBodyComplete_&&!sourceWreckSecondE0Attempted_&&
        sourceConstructorPC_==0x166018u&&racer_,"actual second Wreck E0 caller differs");
    sourceWreckSecondE0Attempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck E0 Bank/source differs");
        std::uint32_t pc=0x166018u;
        const bool done=bridge.loadFreshSourceWreckSecondE0166018(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x166074u,"actual second Wreck E0 constructor prefix stopped");
        sourceWreckSecondE0Complete_=true;
    });
}

void Owner::finishSourceWreckSecondBody165E90(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondPieceComplete_&&!sourceWreckSecondBodyAttempted_&&
        sourceConstructorPC_==0x165e90u&&racer_,"actual second Wreck body caller differs");
    sourceWreckSecondBodyAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck body Bank/source differs");
        std::uint32_t pc=0x165e90u;
        const bool done=bridge.loadFreshSourceWreckSecondBody165E90(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x166018u,"actual second Wreck D0/D4 prefix stopped");
        sourceWreckSecondBodyComplete_=true;
    });
}

void Owner::finishSourceWreckSecondPiece22C0A0(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckPieceCursorComplete_&&!sourceWreckSecondPieceAttempted_&&
        sourceConstructorPC_==0x22c0a0u&&racer_,"actual second Wreck piece caller differs");
    sourceWreckSecondPieceAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck piece Bank/body differs");
        std::uint32_t pc=0x22c0a0u;
        const bool done=bridge.loadFreshSourceWreckSecondPiece22C0A0(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x165e90u,"actual second Wreck Config/model prefix stopped");
        sourceWreckSecondPieceComplete_=true;
    });
}

void Owner::finishSourceWreckSecondPieceCursor22C25C(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondWaterComplete_&&!sourceWreckSecondPieceCursorAttempted_&&
        sourceConstructorPC_==0x22c25cu&&racer_,"actual second Wreck piece cursor caller differs");
    sourceWreckSecondPieceCursorAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck piece cursor Bank/body differs");
        std::uint32_t pc=0x22c25cu;
        const bool done=bridge.advanceFreshSourceWreckSecondPieceCursor22C25C(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&(pc==0x22c0a0u||pc==0x22c278u),"actual second Wreck piece cursor source index/offset/comparison stopped");
        sourceWreckSecondPieceCursorComplete_=true;
    });
}

void Owner::beginSourceWreckGroupSparksConfig15F2F0(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckGroupSparksSetupComplete_&&!sourceWreckGroupSparksConfigAttempted_&&
        sourceConstructorPC_==0x15f2f0u&&racer_,"actual Wreck group sparks caller differs");
    sourceWreckGroupSparksConfigAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual Wreck group sparks Bank/body differs");
        std::uint32_t pc=0x15f2f0u;
        const bool done=bridge.beginFreshSourceWreckGroupSparksConfig15F2F0(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x15ae2cu,"actual Wreck group sparks stack Config constructor stopped");
        sourceWreckGroupSparksConfigComplete_=true;
    });
}

void Owner::setupSourceWreckGroupSparks2CC0D4(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckGroupSparksComplete_&&!sourceWreckGroupSparksSetupAttempted_&&
        sourceConstructorPC_==0x2cc0d4u&&racer_,"actual Wreck group sparks caller differs");
    sourceWreckGroupSparksSetupAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual Wreck group sparks Bank/body differs");
        std::uint32_t pc=0x2cc0d4u;
        const bool done=bridge.setupFreshSourceWreckGroupSparks2CC0D4(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&(pc==0x15f2f0u||pc==0x2cc124u),"actual Wreck group sparks model/filename setup prefix stopped");
        sourceWreckGroupSparksSetupComplete_=true;
    });
}

void Owner::constructSourceWreckGroupSparks2CC0B4(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckGroupContextComplete_&&!sourceWreckGroupSparksAttempted_&&
        sourceConstructorPC_==0x2cc0b4u&&racer_,"actual Wreck group sparks caller differs");
    sourceWreckGroupSparksAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual Wreck group sparks Bank/body differs");
        std::uint32_t pc=0x2cc0b4u;
        const bool done=bridge.constructFreshSourceWreckGroupSparks2CC0B4(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cc0d4u,"actual Wreck group sparks allocation/constructor prefix stopped");
        sourceWreckGroupSparksComplete_=true;
    });
}

void Owner::consumeSourceWreckGroupContext2CC088(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckGroupPrimaryComplete_&&!sourceWreckGroupContextAttempted_&&
        sourceConstructorPC_==0x2cc088u&&racer_,"actual Wreck group context caller differs");
    sourceWreckGroupContextAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual Wreck group context Bank/body differs");
        std::uint32_t pc=0x2cc088u;
        const bool done=bridge.consumeFreshSourceWreckGroupContext2CC088(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cc0b4u,"actual Wreck group context source context/parent prefix stopped");
        sourceWreckGroupContextComplete_=true;
    });
}

void Owner::bindSourceWreckGroupPrimary22C298(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckGroupConstructorComplete_&&!sourceWreckGroupPrimaryAttempted_&&
        sourceConstructorPC_==0x22c298u&&racer_,"actual Wreck group primary caller differs");
    sourceWreckGroupPrimaryAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual Wreck group primary Bank/body differs");
        std::uint32_t pc=0x22c298u;
        const bool done=bridge.bindFreshSourceWreckGroupPrimary22C298(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cc088u,"actual Wreck group primary source capacity/primary/array prefix stopped");
        sourceWreckGroupPrimaryComplete_=true;
    });
}

void Owner::constructSourceWreckGroup22C278(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckPieceCursorComplete_&&!sourceWreckGroupConstructorAttempted_&&
        sourceConstructorPC_==0x22c278u&&racer_,"actual Wreck group constructor caller differs");
    sourceWreckGroupConstructorAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual Wreck group constructor Bank/body differs");
        std::uint32_t pc=0x22c278u;
        const bool done=bridge.constructFreshSourceWreckGroup22C278(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x22c298u,"actual Wreck group constructor source constructor/publication stopped");
        sourceWreckGroupConstructorComplete_=true;
    });
}

void Owner::finishSourceWreckPieceCursor22C25C(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckWaterComplete_&&!sourceWreckPieceCursorAttempted_&&
        sourceConstructorPC_==0x22c25cu&&racer_,"actual first Wreck piece cursor caller differs");
    sourceWreckPieceCursorAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck piece cursor Bank/body differs");
        std::uint32_t pc=0x22c25cu;
        const bool done=bridge.advanceFreshSourceWreckPieceCursor22C25C(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&(pc==0x22c0a0u||pc==0x22c278u),"actual first Wreck piece cursor source index/offset/comparison stopped");
        sourceWreckPieceCursorComplete_=true;
    });
}

void Owner::finishSourceWreckSecondWater22C21C(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondPieceAppendComplete_&&!sourceWreckSecondWaterAttempted_&&
        sourceConstructorPC_==0x22c21cu&&racer_,"actual second Wreck WATER caller differs");
    sourceWreckSecondWaterAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck WATER Bank/body differs");
        std::uint32_t pc=0x22c21cu;
        const bool done=bridge.applyFreshSourceWreckSecondWater22C21C(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x22c25cu,"actual second Wreck WATER category mode/append prefix stopped");
        sourceWreckSecondWaterComplete_=true;
    });
}

void Owner::finishSourceWreckWater22C21C(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckPieceAppendComplete_&&!sourceWreckWaterAttempted_&&
        sourceConstructorPC_==0x22c21cu&&racer_,"actual first Wreck WATER caller differs");
    sourceWreckWaterAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck WATER Bank/body differs");
        std::uint32_t pc=0x22c21cu;
        const bool done=bridge.applyFreshSourceWreckWater22C21C(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x22c25cu,"actual first Wreck WATER category mode/append prefix stopped");
        sourceWreckWaterComplete_=true;
    });
}

void Owner::finishSourceWreckSecondPieceAppend22C1B8(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckPieceCursorComplete_&&(sourceWreckSecondPositiveSplashReaderComplete_||sourceWreckSecondSuccessorCallerComplete_)&&!sourceWreckSecondPieceAppendAttempted_&&
        sourceConstructorPC_==0x2cb8c4u&&racer_,"actual second Wreck piece caller differs");
    sourceWreckSecondPieceAppendAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck piece Bank/body differs");
        std::uint32_t pc=0x2cb8c4u;
        const bool done=bridge.appendFreshSourceWreckSecondPiece22C1B8(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x22c21cu,"actual second Wreck piece return/collection append/scalars prefix stopped");
        sourceWreckSecondPieceAppendComplete_=true;
    });
}

void Owner::finishSourceWreckPieceAppend22C1B8(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSplashReadComplete_&&!sourceWreckPieceAppendAttempted_&&
        sourceConstructorPC_==0x2cb8c4u&&racer_,"actual first Wreck piece caller differs");
    sourceWreckPieceAppendAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck piece Bank/body differs");
        std::uint32_t pc=0x2cb8c4u;
        const bool done=bridge.appendFreshSourceWreckPiece22C1B8(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x22c21cu,"actual first Wreck piece return/collection append/scalars prefix stopped");
        sourceWreckPieceAppendComplete_=true;
    });
}

void Owner::finishSourceWreckSplashRead19DDC8(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSplashComplete_&&!sourceWreckSplashReadAttempted_&&
        sourceConstructorPC_==0x19ddc8u&&racer_,"actual first Wreck splash caller differs");
    sourceWreckSplashReadAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck splash Bank/body differs");
        std::uint32_t pc=0x19ddc8u;
        const bool done=bridge.readStopAppendFreshSourceWreckSplash19DDC8(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cb8c4u,"actual first Wreck splash reader/Stop/append prefix stopped");
        sourceWreckSplashReadComplete_=true;
    });
}

void Owner::finishSourceWreckDustRead19A7D0(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckDustComplete_&&!sourceWreckDustReadAttempted_&&
        sourceConstructorPC_==0x19a7d0u&&racer_,"actual first Wreck dust caller differs");
    sourceWreckDustReadAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck dust Bank/body differs");
        std::uint32_t pc=0x19a7d0u;
        const bool done=bridge.readStopAppendFreshSourceWreckDust19A7D0(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cb854u,"actual first Wreck dust reader/Stop/stores/append prefix stopped");
        sourceWreckDustReadComplete_=true;
    });
}

void Owner::finishSourceWreckFireRead15E024(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckFireComplete_&&!sourceWreckFireReadAttempted_&&
        sourceConstructorPC_==0x15e024u&&racer_,"actual first Wreck first fire caller differs");
    sourceWreckFireReadAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck first fire Bank/body differs");
        std::uint32_t pc=0x15e024u;
        const bool done=bridge.readAppendFreshSourceWreckFire15E024(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cb778u,"actual first Wreck first fire reader/append prefix stopped");
        sourceWreckFireReadComplete_=true;
    });
}

void Owner::completeSourceWreckSecondInitialChild19A7F8(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondInitialReaderComplete_&&!sourceWreckSecondInitialCallerAttempted_&&
        sourceConstructorPC_==0x19a7f8u&&racer_,"actual second Wreck selected child caller differs");
    sourceWreckSecondInitialCallerAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck selected child Bank/body differs");
        std::uint32_t pc=0x19a7f8u;
        const bool done=bridge.completeFreshSourceWreckSecondInitialChild19A7F8(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&(pc==0x2cb720u||pc==0x2cb854u),"actual second Wreck selected child caller prefix stopped");
        sourceWreckSecondInitialCallerComplete_=true;
    });
}

void Owner::completeSourceWreckSecondSuccessorChild(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondSuccessorReaderComplete_&&!sourceWreckSecondSuccessorCallerAttempted_&&
        (sourceConstructorPC_==0x15e0b0u||sourceConstructorPC_==0x19de7cu)&&racer_,"actual second Wreck successor child caller differs");
    sourceWreckSecondSuccessorCallerAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck successor child Bank/body differs");
        std::uint32_t pc=sourceConstructorPC_;
        const bool done=bridge.completeFreshSourceWreckSecondSuccessorChild(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&(pc==0x2cb778u||pc==0x2cb8c4u),"actual second Wreck successor child caller prefix stopped");
        sourceWreckSecondSuccessorCallerComplete_=true;
    });
}

void Owner::readSourceWreckSecondSuccessorChild(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondSuccessorComplete_&&!sourceWreckSecondSuccessorReaderAttempted_&&
        (sourceConstructorPC_==0x15e024u||sourceConstructorPC_==0x19ddc8u)&&racer_,"actual second Wreck successor child caller differs");
    sourceWreckSecondSuccessorReaderAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck successor child Bank/body differs");
        std::uint32_t pc=sourceConstructorPC_;
        const bool done=bridge.readFreshSourceWreckSecondSuccessorChild(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&(pc==0x15e0b0u||pc==0x19de7cu),"actual second Wreck successor child reader-only prefix stopped");
        sourceWreckSecondSuccessorReaderComplete_=true;
    });
}

void Owner::finishSourceWreckSecondPositiveDustRead19A7D0(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondPositiveDustComplete_&&!sourceWreckSecondPositiveDustReaderAttempted_&&
        sourceConstructorPC_==0x19a7d0u&&racer_,"actual second Wreck positive Dust caller differs");
    sourceWreckSecondPositiveDustReaderAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck positive Dust Bank/body differs");
        std::uint32_t pc=0x19a7d0u;
        const bool done=bridge.readStopAppendFreshSourceWreckSecondPositiveDust19A7D0(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cb854u,"actual second Wreck positive Dust reader/Stop/append prefix stopped");
        sourceWreckSecondPositiveDustReaderComplete_=true;
    });
}

void Owner::finishSourceWreckSecondPositiveSplashRead19DDC8(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondPositiveSplashComplete_&&!sourceWreckSecondPositiveSplashReaderAttempted_&&
        sourceConstructorPC_==0x19ddc8u&&racer_,"actual second Wreck positive Splash caller differs");
    sourceWreckSecondPositiveSplashReaderAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck positive Splash Bank/body differs");
        std::uint32_t pc=0x19ddc8u;
        const bool done=bridge.readStopAppendFreshSourceWreckSecondPositiveSplash19DDC8(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cb8c4u,"actual second Wreck positive Splash reader/Stop/append prefix stopped");
        sourceWreckSecondPositiveSplashReaderComplete_=true;
    });
}

void Owner::readSourceWreckSecondInitialChild19A7D0(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondInitialChildComplete_&&!sourceWreckSecondInitialReaderAttempted_&&
        sourceConstructorPC_==0x19a7d0u&&racer_,"actual second Wreck selected child caller differs");
    sourceWreckSecondInitialReaderAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck selected child Bank/body differs");
        std::uint32_t pc=0x19a7d0u;
        const bool done=bridge.readFreshSourceWreckSecondInitialChild19A7D0(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x19a7f8u,"actual second Wreck selected child reader-only prefix stopped");
        sourceWreckSecondInitialReaderComplete_=true;
    });
}

void Owner::finishSourceWreckSmokeRead19A7D0(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSmokeComplete_&&!sourceWreckSmokeReadAttempted_&&
        sourceConstructorPC_==0x19a7d0u&&racer_,"actual first Wreck first smoke caller differs");
    sourceWreckSmokeReadAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck first smoke Bank/body differs");
        std::uint32_t pc=0x19a7d0u;
        const bool done=bridge.readAppendFreshSourceWreckSmoke19A7D0(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cb720u,"actual first Wreck first smoke reader/append prefix stopped");
        sourceWreckSmokeReadComplete_=true;
    });
}

void Owner::finishSourceWreckSplash2CB854(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckDustReadComplete_&&!sourceWreckSplashAttempted_&&
        sourceConstructorPC_==0x2cb854u&&racer_,"actual first Wreck splash caller differs");
    sourceWreckSplashAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck splash Bank/body differs");
        std::uint32_t pc=sourceConstructorPC_;
        const bool done=bridge.constructFreshSourceWreckSplash2CB854(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x19ddc8u,"actual first Wreck splash constructor/manager/filename prefix stopped");
        sourceWreckSplashComplete_=true;
    });
}

void Owner::constructSourceWreckSecondPositiveSplash(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondPositiveDustReaderComplete_&&!sourceWreckSecondPositiveSplashAttempted_&&
        sourceConstructorPC_==0x2cb854u&&racer_,"actual second Wreck positive Splash caller differs");
    sourceWreckSecondPositiveSplashAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck positive Splash Bank/body differs");
        std::uint32_t pc=sourceConstructorPC_;
        const bool done=bridge.constructFreshSourceWreckSecondPositiveSplash(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x19ddc8u,"actual second Wreck positive Splash constructor/manager/filename prefix stopped");
        sourceWreckSecondPositiveSplashComplete_=true;
    });
}

void Owner::constructSourceWreckSecondSuccessorChild(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondInitialCallerComplete_&&!sourceWreckSecondSuccessorAttempted_&&
        (sourceConstructorPC_==0x2cb720u||sourceConstructorPC_==0x2cb854u)&&racer_,"actual second Wreck successor child caller differs");
    sourceWreckSecondSuccessorAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck successor child Bank/body differs");
        std::uint32_t pc=sourceConstructorPC_;
        const bool done=bridge.constructFreshSourceWreckSecondSuccessorChild(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&(pc==0x15e024u||pc==0x19ddc8u),"actual second Wreck successor child constructor/manager/filename prefix stopped");
        sourceWreckSecondSuccessorComplete_=true;
    });
}

void Owner::constructSourceWreckSecondPositiveDust(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondFirstPairComplete_&&!sourceWreckSecondPositiveDustAttempted_&&
        sourceConstructorPC_==0x2cb7e0u&&racer_,"actual second Wreck positive Dust caller differs");
    sourceWreckSecondPositiveDustAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck positive Dust Bank/body differs");
        std::uint32_t pc=sourceConstructorPC_;
        const bool done=bridge.constructFreshSourceWreckSecondPositiveDust(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x19a7d0u,"actual second Wreck positive Dust constructor/manager/filename prefix stopped");
        sourceWreckSecondPositiveDustComplete_=true;
    });
}

void Owner::constructSourceWreckSecondInitialChild(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondArraysComplete_&&!sourceWreckSecondInitialChildAttempted_&&
        (sourceConstructorPC_==0x2cb6ccu||sourceConstructorPC_==0x2cb7e4u)&&racer_,"actual second Wreck selected child caller differs");
    sourceWreckSecondInitialChildAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck selected child Bank/body differs");
        std::uint32_t pc=sourceConstructorPC_;
        const bool done=bridge.constructFreshSourceWreckSecondInitialChild(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x19a7d0u,"actual second Wreck selected child constructor/manager/filename prefix stopped");
        sourceWreckSecondInitialChildComplete_=true;
    });
}

void Owner::finishSourceWreckDust2CB7E0(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckArraysComplete_&&!sourceWreckDustAttempted_&&
        (sourceConstructorPC_==0x2cb7e0u||sourceConstructorPC_==0x2cb7e4u)&&racer_,"actual first Wreck dust caller differs");
    sourceWreckDustAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck dust Bank/body differs");
        std::uint32_t pc=sourceConstructorPC_;
        const bool done=bridge.constructFreshSourceWreckDust2CB7E0(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x19a7d0u,"actual first Wreck dust constructor/manager/filename prefix stopped");
        sourceWreckDustComplete_=true;
    });
}

void Owner::finishSourceWreckSmoke2CB6CC(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckArraysComplete_&&!sourceWreckSmokeAttempted_&&
        sourceConstructorPC_==0x2cb6ccu&&racer_,"actual first Wreck first smoke caller differs");
    sourceWreckSmokeAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck first smoke Bank/body differs");
        std::uint32_t pc=0x2cb6ccu;
        const bool done=bridge.constructFreshSourceWreckSmoke2CB6CC(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x19a7d0u,"actual first Wreck first smoke constructor/manager/filename prefix stopped");
        sourceWreckSmokeComplete_=true;
    });
}

void Owner::finishSourceWreckSecondArrays2CB628(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckSecondEffectInputsComplete_&&!sourceWreckSecondArraysAttempted_&&
        sourceConstructorPC_==0x2cb628u&&racer_,"actual second Wreck effect arrays caller differs");
    sourceWreckSecondArraysAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual second Wreck effect arrays Bank/body differs");
        std::uint32_t pc=0x2cb628u;
        const bool done=bridge.constructFreshSourceWreckSecondArrays2CB628(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&(pc==0x2cb6ccu||pc==0x2cb7e4u),"actual second Wreck effect arrays signed count/arrays/anchor prefix stopped");
        sourceWreckSecondArraysComplete_=true;
    });
}

void Owner::finishSourceWreckArrays2CB628(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckMutualComplete_&&!sourceWreckArraysAttempted_&&
        sourceConstructorPC_==0x2cb628u&&racer_,"actual first Wreck effect arrays caller differs");
    sourceWreckArraysAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck effect arrays Bank/body differs");
        std::uint32_t pc=0x2cb628u;
        const bool done=bridge.constructFreshSourceWreckArrays2CB628(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&(pc==0x2cb6ccu||pc==0x2cb7e4u),"actual first Wreck effect arrays signed count/arrays/anchor prefix stopped");
        sourceWreckArraysComplete_=true;
    });
}

void Owner::finishSourceWreckMutual166390(::ra_source_camera50LA::OwnershipBridge& bridge) {
    need(bank_&&fixture_&&sourceWreckE8Complete_&&!sourceWreckMutualAttempted_&&
        sourceConstructorPC_==0x166390u&&racer_,"actual first Wreck mutual/effect inputs caller differs");
    sourceWreckMutualAttempted_=true;
    bank_->visit(slot_,[&](){
        need(sourceHandlingBodyCurrent(*fixture_),"actual first Wreck mutual/effect inputs Bank/body differs");
        std::uint32_t pc=0x166390u;
        const bool done=bridge.constructFreshSourceWreckMutual166390(slot_,*racer_,pc);
        failurePC_=sourceConstructorPC_=pc;
        need(done&&pc==0x2cb628u,"actual first Wreck mutual/effect inputs derived/count/context prefix stopped");
        sourceWreckMutualComplete_=true;
    });
}

void Owner::constructSourceSecondarySetup2125C4(runtime_startup50fl::Fixture& actual) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&sourceGeometryReserved_&&
        !sourceGeometryComplete_&&sourceSecondaryComplete_&&!sourceSecondaryWithdrawn_&&
        !sourceSecondarySetupAttempted_&&!sourceSecondarySetupWithdrawn_&&sourceConstructorPC_==0x2125c4u&&
        racer_&&racer_->prepared()&&racer_->chariotModel1B4==&scene_&&sourceBody35C_&&
        racer_->sourceBody35C_&&*racer_->sourceBody35C_==sourceBody35C_.get()&&
        coldScenes_.secondary19C_&&sourceBody35C_->sceneBindingCurrent(*coldScenes_.secondary19C_)&&
        sourceBody35C_->canonicalNumericAddress()==&actual.state.bodies.secondary&&
        !jointAllocationAttempted_&&!joint360_&&!racer_->joint360,
        "actual secondary mass/joint source owner or chronology rejected");
    sourceSecondarySetupAttempted_=true;failurePC_=0x2125d0u;
    const auto bounds=scene_.publishDirectBox26F4B0NativeProfileAdapter(graph());
    failurePC_=bounds.sourcePC;need(bounds.okay(),"actual direct26F4B0 native profiling adapter stopped");
    //Actual radius comes from that SAME live Model198 after its direct call,
    //never immutable Rig geometry or a copied main-body radius.
    failurePC_=0x2125ecu;need(scene_.model_.radius198.has_value(),"actual model198 remains unknown");
    need(std::fegetround()==FE_TONEAREST&&std::isfinite(*scene_.model_.radius198),"secondary radius arithmetic domain");
    volatile float radiusSquared=(*scene_.model_.radius198)*(*scene_.model_.radius198);
    need(std::isfinite(radiusSquared),"secondary2125F4 squared radius stopped");
    sourceSecondaryF20_=radiusSquared;
    //14D7F0 constructs stack1110 ZERO, copied to0F90 before the three diagonal
    //stores and final W. The scratch is retained across a later refusal.
    sourceSecondaryInertia0F90_.emplace();std::memset(sourceSecondaryInertia0F90_->v,0,sizeof sourceSecondaryInertia0F90_->v);
    const float mass=coldFloat(0x40c6c1fdu);
    failurePC_=0x21261cu;volatile float halfMass=.5f*mass;
    need(std::isfinite(halfMass),"secondary half-mass stopped");sourceSecondaryHalfMass_=halfMass;
    failurePC_=0x212630u;volatile float inertia=halfMass*(*sourceSecondaryF20_);
    need(std::isfinite(inertia),"secondary inertia scale stopped");
    sourceSecondaryInertia0F90_->v[0]=inertia; //212634
    sourceSecondaryInertia0F90_->v[5]=inertia; //212638
    sourceSecondaryInertia0F90_->v[10]=inertia; //21263C
    sourceSecondaryInertia0F90_->v[15]=1.f; //212640
    failurePC_=0x212650u;
    const bool massOkay=sourceBody35C_->applyMass212650(mass);failurePC_=sourceBody35C_->boundary_;
    need(massOkay,"actual35C mass165300 stopped");
    const bool inertiaOkay=sourceBody35C_->applyInertia212664(*sourceSecondaryInertia0F90_);failurePC_=sourceBody35C_->boundary_;
    need(inertiaOkay,"actual35C inertia165340 stopped");
    failurePC_=0x21266cu;jointAllocationAttempted_=true;
    joint360_=std::make_unique<g::OriginalPodSphereOwner50LA>();
    auto r=joint360_->constructSource1B6200(*application_,generation_,actual.state.bodies.joint,
        application_->jointOrigin434390_);
    failurePC_=r.sourcePC;need(r.status==g::OriginalWreckComponentStatus50LA::complete,
        "actual360 ordered constructor stopped");
    racer_->joint360=joint360_.get(); //212688, BEFORE identity initializer.
    failurePC_=0x212698u;need(joint360_->initializeSource212698(),"actual360 identity initializer stopped");
    std::array<g::OriginalWreckComponent50LA*,29> objects{};
    r=joint360_->append2126A8(outside(objects),racer_->racer);failurePC_=r.sourcePC;
    need(r.status==g::OriginalWreckComponentStatus50LA::complete,"actual360 append stopped");
    failurePC_=0x2126b0u;
    need(racer_->sourceAnchor300_.has_value()&&racer_->binding_.actualAnchorOrigin434630==&application_->detectorFrame434630_,
        "actual source300 constructor/global owner missing");
    auto& anchorWords=*racer_->sourceAnchor300_;
    anchorWords[0]=0u;anchorWords[1]=0u;anchorWords[2]=0x40400000u; //2126B0/B4/BC; W unchanged.
    g::PodSphereVector49L anchor;std::memcpy(anchor.v,anchorWords.data(),16);
    failurePC_=0x2126d0u;const float anchorLength=coldLength(anchor.v);
    using LengthStage=g::OriginalRacerLength85850LA::Stage;
    auto& length=actual.state.fields.length858;
    failurePC_=0x2126ecu;
    need(length.stage_==LengthStage::initial211A2C&&length.generation_==generation_&&length.slot_==slot_,
        "actual854/858 producer ownership missing");
    volatile float adjusted=actual.parameters.lifecycle.restLength854-anchorLength;
    need(std::isfinite(adjusted),"actual2126F0 subtraction stopped");
    length.value_=adjusted;length.stage_=LengthStage::adjusted2126F4;
    //Transient argument projection reads only SAME actual35C270/140. All16
    //words, including supplemental lanes, retain original column order.
    g::PodSphereMatrix49L inverseInertia;constexpr unsigned extra[]={3,7,11,12,13,14,15};
    for(unsigned i=0;i<16;++i){
        if(i/4u<3u&&i%4u<3u)inverseInertia.v[i]=actual.parameters.bodies.secondary.body.inverseInertiaColumns[i/4u].v[i%4u];
        else for(unsigned j=0;j<7;++j)if(extra[j]==i)inverseInertia.v[i]=actual.parameters.bodies.secondary.inverseInertiaExtra[j];
    }
    failurePC_=0x212700u;
    const bool a=joint360_->setupSourceA212700(*sourceBody35C_,actual.parameters.bodies.secondary.body.inverseMass,
        inverseInertia,anchor);failurePC_=joint360_->sourcePC_;need(a,"actual360 A setup stopped");
    g::PodSphereVector49L origin;std::memcpy(origin.v,application_->detectorFrame434630_.data(),16);
    const bool b=joint360_->setupSourceB212718(*body340_,origin,length.value_);
    failurePC_=joint360_->sourcePC_;need(b,"actual360 B setup stopped");
    length.stage_=LengthStage::consumed212718;
    //Typed borrows and numerical setup are complete, but no native contact,
    //physics, camera, or frame producer is activated by this source prefix.
    sourceGeometryComplete_=true;sourceSecondarySetupComplete_=true;
    failurePC_=sourceConstructorPC_=0x212720u;
}

void Owner::reserveSourceHandlingBefore2146(runtime_startup50fl::Fixture& actual) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::unselected&&
        !sourceGeometryAttempted_&&!jointAllocationAttempted_&&rig_&&!rig_->phy.empty()&&
        body340_->stage_==g::OriginalRacerMainBodyOwner50LA::Stage::sceneBound,
        "fresh source handling cannot adopt legacy or replay geometry");
    // Exact constructor-zero proof, not clearing an already-authored body.
    const auto zero=[](float value){std::uint32_t bits;std::memcpy(&bits,&value,4);return bits==0u;};
    const auto& b=actual.parameters.bodies.main.body;
    need(zero(b.mass)&&zero(b.inverseMass),"source main mass is not constructor-owned zero");
    for(float value:body340_->inertia100_.v)need(zero(value),"source inertia100 already written");
    for(const auto& column:b.inverseInertiaColumns)for(float value:column.v)
        need(zero(value),"source inverse inertia already written");
    for(float value:actual.parameters.bodies.mainSphere.inverseInertiaExtra)
        need(zero(value),"source inverse supplemental lanes already written");
    handlingRoute_=HandlingRoute::source;sourceGeometryAttempted_=true;
    // Allocation failure keeps source custody; it cannot fall back to legacy.
    sourceHandlingPhy_=rig_->phy;
    // Metadata-only native reservations survive until their actual source
    // constructors. They do not write secondary mass/inertia, limits, joint
    // fields or length858, and are not source19C/35C/360 publications.
    constructNativeSecondaryContacts212538(actual,actual.state);
    // The retained Rig geometry is immutable authoring input, not live state.
    // Actual secondary and joint stores must occur at their later source PCs.
    sourceGeometryReserved_=true;
}

// Explicit native continuation of the real full-publication caller. The
// earlier legacy projection is neither replayed nor adopted as source work.
// This writes the SAME canonical body and source gravity fields, stopping
// at214894, where the distinct same-owner native Repulsor continuation joins.
bool Owner::continueNativePhysical214710(g::OriginalRacerEntityCold50LA& actualRacer,
        const std::vector<std::uint8_t>& retainedPhy,std::uint32_t& pc)noexcept {
    if(!fixture_||!sourceHandlingBodyCurrent(*fixture_)||handlingRoute_!=HandlingRoute::legacy||
       nativePhysicalAttempted_||nativePhysicalComplete_||sourcePhysicalAttempted_||
       !racer_||racer_!=&actualRacer||!attachedFor(actualRacer)||!racer_->prepared()||racer_->nativeWreckWithdrawn_||
       !racer_->nativePostEmberAttempted_||racer_->nativePostEmberPC_!=0x214710u||pc!=0x214710u||
       racer_->racer.sessionGeneration!=generation_||racer_->racer.reference00!=1u||
       racer_->racer.kind!=g::OriginalWreckComponentKind50LA::racer27E130||
       racer_->body340!=body340_.get()||body340_->stage_!=g::OriginalRacerMainBodyOwner50LA::Stage::appended||
       body340_->entity_.reference00!=1u||body340_->entity_.parent1C!=&racer_->racer||
       body340_->entity_.kind!=g::OriginalWreckComponentKind50LA::body167920||
       !racer_->sourceGravity378_||!racer_->sourceAirborneGravity37C_||
       !rig_||&retainedPhy!=&rig_->phy||retainedPhy.empty())return false;
    if(!nativePhyLoad_||racer_->nativePhyLoad_!=nativePhyLoad_)return false;
    auto phy=nativePhyLoad_->borrow(nativePhyContext_,retainedPhy);if(!phy)return false;
    auto& reader=phy.cursor();
    nativePhysicalAttempted_=true;
    auto& actual=*fixture_;
    auto at=[&](std::uint32_t address){pc=failurePC_=racer_->nativePostEmberPC_=address;};
    try {
        // Canonical378/37C were initialized by20FA24/2C. Never derive them
        // from the already-authored legacy selectors. Missing keys keep them.
        const auto publishGravity=[&](bool airborne,float value){
            if(airborne){racer_->sourceAirborneGravity37C_=value;
                actual.selectors.vehicle37C=value;actual.secondaryInputs.gravity37C=value;}
            else{racer_->sourceGravity378_=value;
                actual.selectors.vehicle378=value;actual.secondaryInputs.gravity378=value;}
        };
        at(0x214718u);bool present=false;
        if(!reader.select157D00("Physical Properties",present))return false;
        at(0x214738u);nativePhysicalMatrix_={}; //14D7F0, zero matrix.
        constexpr const char* keys[]={"Weight","ChariotWeight","RollInertiaScale","PitchInertiaScale","YawInertiaScale"};
        constexpr std::uint32_t readers[]={0x214774u,0x214788u,0x21479cu,0x2147b0u,0x2147c4u};
        for(unsigned i=0;i<5;++i){at(readers[i]);
            if(!reader.float158A70(keys[i],nativePhysicalStack_[i]))return false;}
        std::optional<float> value;at(0x2147d8u);
        if(!reader.float158A70("Gravity",value))return false;
        if(value)publishGravity(false,*value);
        value.reset();at(0x2147ecu);
        if(!reader.float158A70("AirborneGravity",value))return false;
        if(value)publishGravity(true,*value);
        at(0x214808u);if(std::fegetround()!=FE_TONEAREST)return false;
        g::original_wreck_tuning_detail50LA::Math math;
        const float unit=coldFloat(0x4200cccdu);
        const float gravity=math.mul(*racer_->sourceGravity378_,unit);
        if(!math.valid)return false;publishGravity(false,gravity); //21480C.
        at(0x214820u);const float airborne=math.mul(*racer_->sourceAirborneGravity37C_,unit);
        if(!math.valid)return false;publishGravity(true,airborne); //214828.
        // Original stack reads, not required-key defaults. ChariotWeight was
        // read but is not consumed by this bounded call.
        at(0x21482cu);if(!nativePhysicalStack_[0])return false;
        at(0x214830u);if(!nativePhysicalStack_[3])return false;
        at(0x214834u);if(!nativePhysicalStack_[4])return false;
        at(0x214838u);if(!nativePhysicalStack_[2])return false;
        at(0x21483cu);const float mass=math.div(*nativePhysicalStack_[0],unit);if(!math.valid)return false;
        at(0x214840u);float scale=math.div(mass,12.f);if(!math.valid)return false;
        at(0x214844u);scale=math.mul(2.f,scale);if(!math.valid)return false;
        at(0x214848u);scale=math.mul(scale,10.f);if(!math.valid)return false;
        at(0x21484cu);scale=math.mul(10.f,scale);if(!math.valid)return false;
        at(0x214850u);const float pitch=math.mul(*nativePhysicalStack_[3],scale);if(!math.valid)return false;
        at(0x214854u);const float yaw=math.mul(*nativePhysicalStack_[4],scale);if(!math.valid)return false;
        nativePhysicalMatrix_.v[0]=pitch; //214858.
        at(0x21485cu);const float roll=math.mul(*nativePhysicalStack_[2],scale);if(!math.valid)return false;
        nativePhysicalMatrix_.v[5]=yaw;nativePhysicalMatrix_.v[10]=roll;nativePhysicalMatrix_.v[15]=1.f;
        at(0x214878u); //Actual340 slot78=165300; nonpositive mass is a no-store return.
        if(mass>0.f){
            at(0x165318u);actual.parameters.bodies.main.body.mass=mass;
            at(0x16532cu);volatile float inverse=1.f/actual.parameters.bodies.main.body.mass;
            if(!std::isfinite(inverse))return false;
            actual.parameters.bodies.main.body.inverseMass=inverse; //165330.
        }
        at(0x21488cu); //Actual340 slot7C=165340. Copies precede fallible inversion.
        for(unsigned i=0;i<16u;i+=2u){std::uint32_t pair[2];
            std::memcpy(pair,nativePhysicalMatrix_.v+i,8);std::memcpy(body340_->inertia100_.v+i,pair,8);}
        at(0x16537cu);const auto inverse=math.inverse(body340_->inertia100_);
        if(!math.valid)return false;
        constexpr unsigned extra[]={3,7,11,12,13,14,15};
        for(unsigned i=0;i<16u;++i){
            if(i%4u<3u&&i/4u<3u)
                actual.parameters.bodies.main.body.inverseInertiaColumns[i/4u].v[i%4u]=inverse.v[i];
            else for(unsigned j=0;j<7u;++j)if(extra[j]==i)
                actual.parameters.bodies.mainSphere.inverseInertiaExtra[j]=inverse.v[i];
        }
        nativePhysicalComplete_=true;at(0x214894u);return true;
    }catch(...){return false;} //Reached source effects and the attempt latch survive.
}

//Native same-model continuation of the active full-publication path. Missing
//reader keys retain unknown stack/direct-field knowledge until consumed; no
//legacy projected value or counted source348 owner substitutes for it.
bool Owner::continueNativeRepulsor214894(g::OriginalRacerEntityCold50LA& actualRacer,
        const std::vector<std::uint8_t>& retainedPhy,ra_source_camera50LA::OwnershipBridge& bridge,
        std::uint32_t& pc)noexcept {
    if(!fixture_||!sourceHandlingBodyCurrent(*fixture_)||handlingRoute_!=HandlingRoute::legacy||
       !nativePhysicalComplete_||nativeRepulsorAttempted_||nativeRepulsorComplete_||sourceRepulsorAttempted_||
       !racer_||racer_!=&actualRacer||!attachedFor(actualRacer)||!racer_->prepared()||racer_->nativeWreckWithdrawn_||
       pc!=0x214894u||racer_->nativePostEmberPC_!=pc||racer_->racer.sessionGeneration!=generation_||
       racer_->body340!=body340_.get()||!racer_->sourceLift7EC_||!racer_->sourceGravity378_||
       !rig_||&retainedPhy!=&rig_->phy||retainedPhy.empty())return false;
    if(!nativePhyLoad_||racer_->nativePhyLoad_!=nativePhyLoad_)return false;
    auto phy=nativePhyLoad_->borrow(nativePhyContext_,retainedPhy);if(!phy)return false;
    auto& reader=phy.cursor();nativeRepulsorAttempted_=true;
    const auto at=[&](std::uint32_t address){pc=failurePC_=racer_->nativePostEmberPC_=address;};
    const auto read=[&](const char* key,std::optional<float>& value){return reader.float158A70(key,value);};
    try {
        at(0x21489cu);bool present=false;if(!reader.select157D00("Repulsor",present))return false;
        std::optional<float> value;at(0x2148b0u);
        if(!read("MaxLiftHeight",value))return false;
        if(value){racer_->sourceLift7EC_=*value;fixture_->parameters.visual.verticalLimit7EC=*value;}
        at(0x2148c4u);if(!read("Spring",nativeRepulsorStack_[0]))return false;
        at(0x2148d8u);if(!read("Damping",nativeRepulsorStack_[1]))return false;
        at(0x2148f0u);if(!read("SideForceCoefficient",nativeRepulsorStack_[2]))return false;
        //Direct200 missing key keeps the native lane unknown/unchanged. It is
        //not a stack operand consumed by this setter and does not become zero.
        if(nativeRepulsorStack_[2]&&!bridge.writeNative348ScalarBefore21494C(
            slot_,*racer_,*this,*nativeRepulsorStack_[2]))return false;
        at(0x214904u);if(!read("OrientationSpring",nativeRepulsorStack_[3]))return false;
        at(0x214918u);if(!bridge.writeNative348ScalarBefore21494C(
            slot_,*racer_,*this,*racer_->sourceGravity378_))return false;
        at(0x21491cu);float native324;
        if(!bridge.readNativeRepulsorBound324(slot_,*racer_,*this,native324))return false;
        at(0x214920u);if(!nativeRepulsorStack_[0])return false;
        at(0x214924u);if(!nativeRepulsorStack_[1])return false;
        at(0x214928u);if(!nativeRepulsorStack_[3])return false;
        at(0x21494cu);
        const bool done=bridge.applyNative348Setup21494C(slot_,*racer_,*this,native324,
            *racer_->sourceLift7EC_,*nativeRepulsorStack_[0],*nativeRepulsorStack_[1],
            *nativeRepulsorStack_[3],pc);
        failurePC_=racer_->nativePostEmberPC_=pc;
        if(!done)return false;
        nativeRepulsorComplete_=true;at(0x214954u);return true;
    }catch(...){return false;} //Actual prefixes retained and never replayed.
}

#include "native_racer_aero_engine21495450la.inl"
#include "native_racer_handling_return2154d850la.inl"
#include "native_racer_spin_inventory212ed050la.inl"

void Owner::applySourcePhysical214710(runtime_startup50fl::Fixture& actual) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&
        sourceGeometryComplete_&&!sourcePhysicalAttempted_&&racer_&&racer_->prepared()&&
        !racer_->nativeWreckWithdrawn_&&racer_->body340==body340_.get()&&
        racer_->racer.sessionGeneration==generation_&&racer_->nativePostEmberPC_==0x214710u&&
        racer_->sourceGravity378_&&racer_->sourceAirborneGravity37C_&&
        rig_&&sourceHandlingPhy_==rig_->phy,"source physical caller/identity rejected");
    sourcePhysicalAttempted_=true;
    auto pc=[&](std::uint32_t address){failurePC_=racer_->nativePostEmberPC_=address;};
    pc(0x214738u);sourcePhysicalMatrix_={}; //Actual14D7F0 ZERO matrix.
    constexpr const char* keys[]={"Weight","ChariotWeight","RollInertiaScale","PitchInertiaScale","YawInertiaScale"};
    constexpr std::uint32_t readers[]={0x214774u,0x214788u,0x21479cu,0x2147b0u,0x2147c4u};
    for(unsigned i=0;i<5;++i){pc(readers[i]);
        need(g::OriginalRacerEntityCold50LA::sourceFloat158A70(sourceHandlingPhy_,
            "Physical Properties",keys[i],sourcePhysicalStack_[i]),"source physical float reader stopped");}
    //These two existing numeric consumers are native aliases of the same
    //source fields, not independent authored values. No legacy ready gate is
    //granted by publishing them; only the source route can reach this method.
    const auto publishGravity=[&](bool airborne,float value){
        if(airborne){racer_->sourceAirborneGravity37C_=value;actual.selectors.vehicle37C=value;
            actual.secondaryInputs.gravity37C=value;}
        else{racer_->sourceGravity378_=value;actual.selectors.vehicle378=value;
            actual.secondaryInputs.gravity378=value;}
    };
    std::optional<float> value;pc(0x2147d8u);
    need(g::OriginalRacerEntityCold50LA::sourceFloat158A70(sourceHandlingPhy_,
        "Physical Properties","Gravity",value),"source gravity reader stopped");
    if(value)publishGravity(false,*value);
    value.reset();pc(0x2147ecu);
    need(g::OriginalRacerEntityCold50LA::sourceFloat158A70(sourceHandlingPhy_,
        "Physical Properties","AirborneGravity",value),"source airborne gravity reader stopped");
    if(value)publishGravity(true,*value);
    need(std::fegetround()==FE_TONEAREST,"source physical rounding domain");
    g::original_wreck_tuning_detail50LA::Math math;
    const float unit=coldFloat(0x4200cccdu);
    pc(0x214808u);const float gravity=math.mul(*racer_->sourceGravity378_,unit);
    need(math.valid,"source gravity multiplication stopped");publishGravity(false,gravity); //21480C.
    pc(0x214820u);const float airborne=math.mul(*racer_->sourceAirborneGravity37C_,unit);
    need(math.valid,"source airborne multiplication stopped");publishGravity(true,airborne); //214828.
    //ChariotWeight was read to3D4 but is not consumed by this mass/inertia cut.
    pc(0x21482cu);need(sourcePhysicalStack_[0].has_value(),"source Weight stack destination unknown");
    pc(0x214830u);need(sourcePhysicalStack_[3].has_value(),"source Pitch stack destination unknown");
    pc(0x214834u);need(sourcePhysicalStack_[4].has_value(),"source Yaw stack destination unknown");
    pc(0x214838u);need(sourcePhysicalStack_[2].has_value(),"source Roll stack destination unknown");
    pc(0x21483cu);const float mass=math.div(*sourcePhysicalStack_[0],unit);
    need(math.valid,"source main mass arithmetic stopped");
    pc(0x214840u);float scale=math.div(mass,12.f);need(math.valid,"source inertia scale divide stopped");
    pc(0x214844u);scale=math.mul(2.f,scale);need(math.valid,"source inertia scale multiply stopped");
    pc(0x214848u);scale=math.mul(scale,10.f);need(math.valid,"source inertia scale multiply stopped");
    pc(0x21484cu);scale=math.mul(10.f,scale);need(math.valid,"source inertia scale multiply stopped");
    pc(0x214850u);const float pitch=math.mul(*sourcePhysicalStack_[3],scale);need(math.valid,"source pitch stopped");
    pc(0x214854u);const float yaw=math.mul(*sourcePhysicalStack_[4],scale);need(math.valid,"source yaw stopped");
    sourcePhysicalMatrix_.v[0]=pitch; //214858.
    pc(0x21485cu);const float roll=math.mul(*sourcePhysicalStack_[2],scale);need(math.valid,"source roll stopped");
    sourcePhysicalMatrix_.v[5]=yaw;sourcePhysicalMatrix_.v[10]=roll;sourcePhysicalMatrix_.v[15]=1.f;
    pc(0x214878u);applySourceMass165300(actual,mass);
    pc(0x21488cu);applySourceInertia165340(actual,sourcePhysicalMatrix_);
    pc(0x214894u); //Repulsor reader/setup remains a separate mandatory owner join.
}

void Owner::applySourceRepulsor214894(runtime_startup50fl::Fixture& actual,
        ra_source_camera50LA::OwnershipBridge& bridge) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&
        sourceMassComplete_&&sourceInertiaAttempted_&&!sourceRepulsorAttempted_&&
        racer_&&racer_->prepared()&&!racer_->nativeWreckWithdrawn_&&
        racer_->body340==body340_.get()&&racer_->racer.sessionGeneration==generation_&&
        racer_->nativePostEmberPC_==0x214894u&&racer_->sourceLift7EC_&&
        racer_->sourceGravity378_&&rig_&&sourceHandlingPhy_==rig_->phy,
        "source Repulsor requires actual physical return and same racer");
    sourceRepulsorAttempted_=true;
    auto pc=[&](std::uint32_t address){failurePC_=racer_->nativePostEmberPC_=address;};
    const auto read=[&](const char* key,std::optional<float>& value){
        need(g::OriginalRacerEntityCold50LA::sourceFloat158A70(sourceHandlingPhy_,
            "Repulsor",key,value),"source Repulsor float reader stopped");};
    std::optional<float> height;pc(0x2148b0u);read("MaxLiftHeight",height);
    if(height){racer_->sourceLift7EC_=*height;actual.parameters.visual.verticalLimit7EC=*height;}
    pc(0x2148c4u);read("Spring",sourceRepulsorStack_[0]);
    pc(0x2148d8u);read("Damping",sourceRepulsorStack_[1]);
    pc(0x2148f0u);read("SideForceCoefficient",sourceRepulsorStack_[2]);
    //The fresh348 component has a complete-input admission; this optional
    //missing-key route remains a stop, never an invented zero/readiness bit.
    need(sourceRepulsorStack_[2].has_value(),"source348 missing-key side-force route unjoined");
    need(bridge.writeSource348ScalarBefore21494C(slot_,*racer_,*sourceRepulsorStack_[2]),
        "actual source348 side force store rejected");
    pc(0x214904u);read("OrientationSpring",sourceRepulsorStack_[3]);
    pc(0x214918u);need(bridge.writeSource348ScalarBefore21494C(slot_,*racer_,*racer_->sourceGravity378_),
        "actual source348 gravity store rejected");
    pc(0x21491cu);need(racer_->sourceCollisionHalf320_.has_value(),
        "actual2121C8 Racer324 producer remains unavailable");
    const float actual324=(*racer_->sourceCollisionHalf320_)[1];
    pc(0x214920u);need(sourceRepulsorStack_[0].has_value(),"source Spring stack unknown");
    pc(0x214924u);need(sourceRepulsorStack_[1].has_value(),"source Damping stack unknown");
    pc(0x214928u);need(sourceRepulsorStack_[3].has_value(),"source OrientationSpring stack unknown");
    //Bridge builds the actual local(0,324,0,1) argument and binds the SAME348
    //row. Its source-order partial failure remains retained, with no replay.
    pc(0x21494cu);std::string error;
    const bool setup=bridge.applySource348Setup21494C(slot_,*racer_,actual.parameters,actual324,
        *racer_->sourceLift7EC_,*sourceRepulsorStack_[0],*sourceRepulsorStack_[1],
        *sourceRepulsorStack_[3],error);
    need(setup,error.empty()?"source348 setup stopped":error.c_str());
    pc(0x214954u); //The next class/tuning source loader remains unjoined.
}

void Owner::applySourceMass165300(runtime_startup50fl::Fixture& actual,float mass) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&
        sourceGeometryComplete_&&!sourceMassAttempted_&&racer_&&racer_->prepared()&&
        !racer_->nativeWreckWithdrawn_&&racer_->body340==body340_.get()&&
        racer_->racer.sessionGeneration==generation_&&
        racer_->nativePostEmberPC_==0x214878u&&sourceHandlingPhy_==rig_->phy,
        "source165300 requires its actual reached caller and untouched route");
    sourceMassAttempted_=true;failurePC_=0x165300u;
    need(std::fegetround()==FE_TONEAREST&&std::isfinite(mass),"source mass arithmetic domain");
    if(mass>0.f){
        actual.parameters.bodies.main.body.mass=mass; //165318, before reciprocal.
        failurePC_=0x16532cu;
        volatile float inverse=1.f/actual.parameters.bodies.main.body.mass;
        need(std::isfinite(inverse),"source reciprocal stopped after mass publication");
        actual.parameters.bodies.main.body.inverseMass=inverse; //165330.
    }
    sourceMassComplete_=true;
}

void Owner::applySourceInertia165340(runtime_startup50fl::Fixture& actual,const g::BodyMatrix48G& input) {
    need(sourceHandlingBodyCurrent(actual)&&handlingRoute_==HandlingRoute::source&&
        sourceMassComplete_&&!sourceInertiaAttempted_&&racer_&&racer_->prepared()&&
        !racer_->nativeWreckWithdrawn_&&racer_->body340==body340_.get()&&
        racer_->racer.sessionGeneration==generation_&&
        racer_->nativePostEmberPC_==0x21488cu&&sourceHandlingPhy_==rig_->phy,
        "source165340 requires actual mass return and same authored owner");
    sourceInertiaAttempted_=true;failurePC_=0x165340u;
    need(std::fegetround()==FE_TONEAREST,"source inverse rounding domain");
    //All eight pair copies precede14E910. Even rejected inverse arithmetic
    //must retain the new100 and the previous140; no transactional rollback.
    for(unsigned i=0;i<16;i+=2){std::uint32_t pair[2];
        std::memcpy(pair,input.v+i,8);std::memcpy(body340_->inertia100_.v+i,pair,8);}
    failurePC_=0x16537cu;
    g::original_wreck_tuning_detail50LA::Math math;
    const auto inverse=math.inverse(body340_->inertia100_);
    need(math.valid,"source14E910 stopped after inertia100 publication");
    //Native split layout maps the actual ordered140 matrix copies. Keep each
    //source lane's order instead of grouping XYZ and supplemental writes.
    constexpr unsigned extra[]={3,7,11,12,13,14,15};
    for(unsigned i=0;i<16;++i){
        if(i%4u<3u&&i/4u<3u)
            actual.parameters.bodies.main.body.inverseInertiaColumns[i/4u].v[i%4u]=inverse.v[i];
        else for(unsigned j=0;j<7;++j)if(extra[j]==i)
            actual.parameters.bodies.mainSphere.inverseInertiaExtra[j]=inverse.v[i];
    }
    failurePC_=0x1653b8u;
}

void Owner::beginLegacyColdHandling(runtime_startup50fl::Fixture& actual) {
    need(&actual==fixture_&&handlingRoute_!=HandlingRoute::source,
        "legacy handling cannot replay or fall back after source custody");
    handlingRoute_=HandlingRoute::legacy;
}

void Owner::commitMainInertia21488C(runtime_startup50fl::Fixture& actual,
        const g::AuthoredBinding49N& binding) {
        need(handlingRoute_!=HandlingRoute::source&& &actual==fixture_&&modelRole_==Rig::chariot&&body340_&&
            body340_->stage_==g::OriginalRacerMainBodyOwner50LA::Stage::sceneBound&&
            body340_->canonicalNumericAddress()==&actual.state.bodies.main&&
            body340_->parameters_==&actual.parameters.bodies.main,
            "original main340 handling commit destination rejected");
        handlingRoute_=HandlingRoute::legacy;
        std::memcpy(body340_->inertia100_.v,binding.inertia100.v,sizeof binding.inertia100.v);
    }

void Owner::preserveColdTargetForHandling(runtime_startup50fl::Fixture& actual,
        g::AuthoredGeometryBinding49O& projection)const {
        using Stage=g::OriginalRacerLength85850LA::Stage;
        need(&actual==fixture_ && &actual.state==arena_&&modelRole_==Rig::chariot&&
            coldChariotLocal_&&arena_->fields.length858.stage_==Stage::initial211A2C&&
            arena_->fields.length858.generation_==generation_&&arena_->fields.length858.slot_==slot_,
            "original cold handling destination or stage replaced");
        projection.targetRelative2F0=actual.secondaryInputs.targetRelative2F0;
        // This transient projection carries the real target into the existing
        // mass/inertia binder; its derived target/858 are never authoritative.
    }

void Owner::constructNativeSecondaryContacts212538(runtime_startup50fl::Fixture& actual,
        g::VehicleFrameState49W& handlingState) {
        if(!nativeSecondaryContactsRequested_)return;
        need(modelRole_==Rig::chariot&&bank_&& &actual==fixture_&& &actual.state==arena_&&
            body340_&&body340_->canonicalNumericAddress()==&actual.state.bodies.main&&
            generation_&&slot_<8u&&!nativeSecondaryContactsAttempted_&&!nativeSecondaryContacts_&&!nativeRacerTerrain_&&!nativeAero_&&
            actual.state.bodies.secondary.body20C==0&&
            actual.state.bodies.secondary.root.callbackState238==0&&
            handlingState.bodies.secondary.body20C==0&&
            handlingState.bodies.secondary.root.callbackState238==0,
            "native secondary contact cold producer ownership rejected");
        const auto physical=actual.bindings.lifecycle.physical19C;
        const auto index=static_cast<std::size_t>(physical);
        need(index<actual.state.sceneNodeCount&&physical!=actual.bindings.lifecycle.main190&&
            index<handlingState.sceneNodeCount,
            "native secondary contact actual19C numeric node unavailable");
        // This is admission of the genuinely fresh native numerical owner,
        // not replay of167920 initialization into existing handling state.
        // No existing count, root, scene or physics field is changed.
        using TargetStage=g::OriginalRacerLength85850LA::Stage;
        const auto& targetPublication=actual.state.fields.length858;
        need(coldChariotLocal_&&targetPublication.stage_==TargetStage::initial211A2C&&
            targetPublication.generation_==generation_&&targetPublication.slot_==slot_,
            "native secondary target2F0 actual2119C4 publication unavailable");
        nativeSecondaryContactsAttempted_=true;
        nativeSecondaryContacts_.reset(new g::NativeSecondaryContactOwner50LA(
            *bank_,actual.state,actual.parameters.bodies.secondary,actual.secondaryInputs.targetRelative2F0,
            generation_,slot_,physical));
        //Shared-only backing reservation, NOT racer Entity publication. The
        //actual Session racer does not exist at this prepareEarly cut yet.
        nativeRacerTerrain_.reset(new g::NativeRacerTerrainOwner50LA(
            *bank_,actual.state,this,generation_,slot_));
        //Native current-state authority, not a source350 Entity publication.
        //The legacy native ctor writes future100330 flow immediately. A
        //fresh source route must not allocate/adopt that already-written owner
        //before the real350 constructor. Its later explicit producer owns it.
        if(handlingRoute_!=HandlingRoute::source)
            nativeAero_.reset(new g::NativeRacerAeroOwner50LA(*bank_,actual.state,
                actual.parameters.bodies.main,*body340_,*application_,this,generation_,slot_,
                application_->aeroFlowOrigin3D6B40_));
    }

void Owner::setupColdJoint2126(runtime_startup50fl::Fixture& actual,
        const g::AuthoredGeometryBinding49O& geometry,const g::PodSphereBody49L& main,
        g::PodSphereJoint49L& joint) {
        g::AuthoredGeometryBinding49O admitted=geometry;
        preserveColdTargetForHandling(actual,admitted);
        auto& length=arena_->fields.length858;
        using Stage=g::OriginalRacerLength85850LA::Stage;
        // Source2126B0/BC writes anchor300 XYZ=(0,0,3); retained original
        // constructor provenance supplies W through the existing binding.
        need(geometry.secondaryAnchor300.v[0]==0.f&&geometry.secondaryAnchor300.v[1]==0.f&&
            geometry.secondaryAnchor300.v[2]==3.f&&geometry.secondaryAnchor300.v[3]==1.f,
            "original2126B0 anchor binding differs");
        auto& sourceJoint=actual.state.bodies.joint;
        need(modelRole_==Rig::chariot&&body340_&&
            body340_->canonicalNumericAddress()==&actual.state.bodies.main&&
            !jointAllocationAttempted_&&!joint360_,
            "original joint360 allocation cannot replay");
        if(nativeSecondaryContactsRequested_){
            failurePC_=0x21266cu; //Native admission before the existing allocation.
            const auto* a=nativeSecondaryContacts_.get();
            need(bank_&& &actual==fixture_&& &actual.state==arena_&&a&&!a->revoked_&&
                a->bank_==bank_&&a->frame_==arena_&&a->slot_==slot_&&
                a->generation_==generation_&&generation_&&slot_<8u&&
                a->parameters_==&actual.parameters.bodies.secondary&&
                a->targetRelative2F0_==&actual.secondaryInputs.targetRelative2F0&&
                a->physical19C_==actual.bindings.lifecycle.physical19C&&rig_&&
                a->physical19C_==rig_->physical&&
                body340_->parameters_==&actual.parameters.bodies.main&&
                body340_->entity().sessionGeneration==generation_,
                "native joint A/B cold producer identity rejected");
        }
        jointAllocationAttempted_=true;failurePC_=0x21266cu;
        joint360_=std::make_unique<g::OriginalPodSphereOwner50LA>();
        const auto constructed=joint360_->construct1B6200(*application_,generation_,sourceJoint);
        failurePC_=constructed.sourcePC;
        need(constructed.status==g::OriginalWreckComponentStatus50LA::complete,
            "original joint360 Entity/numeric construction stopped");
        failurePC_=0x2126d0u;const float anchorLength=coldLength(geometry.secondaryAnchor300.v);
        volatile float result=actual.parameters.lifecycle.restLength854-anchorLength;
        need(std::isfinite(result),"original2126F0 length subtraction stopped");
        length.value_=result;length.stage_=Stage::adjusted2126F4;
        g::PodSphereBody49L secondary{};secondary.inverseMass=geometry.inverseMass270;
        std::memcpy(secondary.inverseInertia.v,geometry.inverseInertia140.v,sizeof secondary.inverseInertia.v);
        // The retained Entity owns this SAME numeric joint; source A/B
        // operations continue here. Session stages the real alias/append later.
        failurePC_=0x212700u;need(g::setupPodSphereA49L(sourceJoint,secondary,geometry.secondaryAnchor300),
            "original212700 joint A setup stopped");
        g::PodSphereVector49L zero;std::memcpy(zero.v,actual.state.visualShared.zero434630.v,16);
        failurePC_=0x212718u;
        need(g::setupPodSphereB49L(sourceJoint,&main,zero,length.value_),"original212718 joint B setup stopped");
        if(nativeSecondaryContactsRequested_){
            // Both actual cold numeric setup calls returned. Retain owners,
            // never addresses of temporary secondary/main sphere projections.
            auto& bindings=joint360_->nativeBodies_;
            bindings.bank=bank_;bindings.frame=arena_;bindings.model=this;
            bindings.a=nativeSecondaryContacts_.get();bindings.b=body340_.get();
            bindings.generation=generation_;bindings.slot=slot_;
        }
        length.stage_=Stage::consumed212718; //Joint4C is a clamped consumer, never858.
        joint=sourceJoint; //Successful handling working copy keeps the same owner writes.
    }

void Owner::bindRendererMaterialFlame(a::PodFlameOwners50W& actualFlame) {
        need((modelRole_==Rig::right||modelRole_==Rig::left||
            modelRole_==Rig::bottomRight||modelRole_==Rig::bottomLeft)&&
            resourcesComplete1538B0()&&!bankedFlame_,"original renderer flame join order rejected");
        // Rig order is L,R,C,BL,BR; the real flame model ordinals are L,R,BL,BR.
        const unsigned flameModel=modelRole_==Rig::left?0u:modelRole_==Rig::right?1u:
            modelRole_==Rig::bottomLeft?2u:3u;
        g::EngineFlareDiagnostic50GV diagnostic;
        need(g::bindCanonicalEngineFlare50LA(actualFlame,scene_,materials_,flameModel,diagnostic),
            "original engine canonical animation join rejected");
        bankedFlame_=&actualFlame;
    }

void Owner::completeResources1538B0(const std::string& directory) {
        failurePC_=0x26e820u;
        need(!resourcesComplete_&&!finished_&&!closed_&&setup_&&setup_->restored1556C0()&&modelAppended_,
            "original model resource completion order rejected");
        a::OriginalStandardTextureDiagnostic50LA fd;
        a::OriginalStandardModelFileView50LA source;
        need(files_->modelFile(psgId_,source,fd)&&source.member&&source.bytes,
            "original PSG cache changed during cold model staging");
        if(source.geometry240110){failurePC_=0x26ea28u;
            throw std::runtime_error("original live PSG warm26EA28 appeared before resource completion: "+
                *source.member+" row="+std::to_string(psgId_.row));}
        payload_=std::make_shared<a::OriginalStandardModelResource50LA>();
        a::OriginalStandardResourceDiagnostic50LA rd;
        need(payload_->prepare(*source.member,*source.bytes,rd),"original PSG geometry resource rejected");
        a::OriginalStandardMaterialDiagnostic50LA md;
        failurePC_=0x26e640u;
        need(materials_.acquire26E640(*payload_,domain_->materials382B38(),{&directory},md),
            "original racer canonical material references rejected");
        auto& fields=scene_.fields();
        fields.materialCount1C0=static_cast<std::int32_t>(*materials_.count1C0());
        fields.materials1C4=materials_.handle1C4(0);
        failurePC_=0x240570u;
        need(scene_.loadQuantization240570(*payload_->scene()).okay(),"original model quantization rejected");
        geometry_=std::make_shared<a::OriginalStandardModelGeometry50LA>();
        failurePC_=0x240110u;
        need(geometry_->construct240110(payload_,files_,psgId_,&scene_,rd),
            "original240110 geometry constructor failed");
        fields.geometry1B4=geometry_.get();
        need(files_->publishGeometry1326A0(psgId_,geometry_,fd),"original PSG geometry cache publication failed");
        need(scene_.allocateSourceRowTable26EE9C().okay(),"original model source-row allocation failed");
        failurePC_=0x240740u;
        need(packets_.construct240740(scene_,*geometry_,true).okay(),"original model CPU palette storage failed");
        // This config08 identity was real while1538B0 consumed it.1573B0
        // disposes storage without writing a source null back into08.
        configuration08_.reset();
        resourcesComplete_=true;failurePC_=0;
    }

void Owner::finish1538B0(g::OriginalRacerEntityCold50LA& racer,const std::string& directory) {
        need(!sourceModelPrefixComplete_,"source model prefix cannot replay before later constructor joins");
        (void)directory;
        g::OriginalPodSphereOwner50LA* pendingNativeJoint=nullptr;
        const bool right=modelRole_==Rig::right;
        const bool left=modelRole_==Rig::left;
        const bool bottomRight=modelRole_==Rig::bottomRight;
        const bool bottomLeft=modelRole_==Rig::bottomLeft;
        const auto animationPC=right?0x210c7cu:left?0x210f40u:
            bottomRight?0x211244u:bottomLeft?0x211540u:0x211970u;
        const auto modelPC=right?0x210ca4u:left?0x210f68u:
            bottomRight?0x21126cu:bottomLeft?0x211568u:0x211998u;
        failurePC_=right?0x210c48u:left?0x210f0cu:
            bottomRight?0x211210u:bottomLeft?0x21150cu:0x21193cu;
        auto& animationSlot=right?racer.animation1A4:left?racer.animation1A8:
            bottomRight?racer.animation1AC:bottomLeft?racer.animation1B0:racer.animation1A0;
        auto& modelSlot=right?racer.rightModel1B8:left?racer.leftModel1BC:
            bottomRight?racer.bottomRightModel1C0:bottomLeft?racer.bottomLeftModel1C4:racer.chariotModel1B4;
        const auto* rightPeer=peers_[Rig::right];const auto* leftPeer=peers_[Rig::left];
        const auto* bottomRightPeer=peers_[Rig::bottomRight];
        const auto* bottomLeftPeer=peers_[Rig::bottomLeft];
        const auto* chariotPeer=peers_[Rig::chariot];
        need(resourcesComplete1538B0()&&!finished_&&racer.postOverlayConstructed210A20()&&
            racer.racer.sessionGeneration==generation_&&!animationSlot&&!modelSlot&&
            (!racer_||racer_==&racer)&&attachmentOrderAdmitted(racer),
            "original model Session attachment order rejected");
        std::array<g::OriginalWreckComponent50LA*,29> storage{};
        racer_=&racer;
        // The same early-constructed1536F0 identity reaches its real Session
        // slot. This is native staging, not a replayed constructor or cache hit.
        const auto d=outside(storage);
        if(modelRole_==Rig::chariot){
            need(!racer.scene190&&!racer.scene198&&!racer.scene194,
                "original retained scene aliases already published");
            racer.scene190=coldScenes_.objects_[0].get();
            racer.scene198=coldScenes_.objects_[1].get();
            racer.scene194=coldScenes_.objects_[2].get();
            need(!racer.wrist208&&!racer.wrist20C&&!racer.wristAllocations_[0]&&!racer.wristAllocations_[1],
                "original racer wrist allocation ownership already published");
            for(const auto& owner:coldWrists_)
                need(owner.stage==ColdWrist::Stage::constructed||owner.stage==ColdWrist::Stage::absent||
                    owner.stage==ColdWrist::Stage::allocationNull,"original wrist construction not complete");
            // Transfer SAME allocations, never reconstruct/copy the source
            // records. Their scene borrows need not outlive plain2E8B00 frees.
            for(unsigned i=0;i<2;++i)racer.wristAllocations_[i]=std::move(coldWrists_[i].allocation);
            racer.wrist208=racer.wristAllocations_[0]?&racer.wristAllocations_[0]->record:nullptr;
            racer.wrist20C=racer.wristAllocations_[1]?&racer.wristAllocations_[1]->record:nullptr;
            const auto result=coldScenes_.append(d,racer.racer);failurePC_=result.sourcePC;
            need(result.okay(),"original190/198/194 Entity append failed");
        }
        animationSlot=&animation_;
        failurePC_=animationPC;
        const auto attached=g::appendOriginalWreckComponent50LA(d,racer.racer,animationEntity(),2u,
            application_->attachmentLock423F58());
        need(attached.status==g::OriginalWreckComponentStatus50LA::complete,
            "original210C7C/210F40/211244/211540/211970 animation append failed");
        animationAttached_=true;
        modelSlot=&scene_; //210C8C/210F50/211254/211550/211980: SAME model04, not model+8.
        failurePC_=modelPC;
        const auto peers=peerScenes();
        const auto reappended=right?
            scene_.reappendToDetached7F8210CA4(d,animationEntity(),racer.detached7F8,
                peers[0],peers[1],peers[2],peers[3]):
            left?scene_.reappendToDetached7F8210F68(d,animationEntity(),racer.detached7F8,
                peers[0],peers[1],peers[2],peers[3]):
            bottomRight?scene_.reappendToDetached7F821126C(d,animationEntity(),racer.detached7F8,
                &rightPeer->scene_,&leftPeer->scene_,&bottomLeftPeer->scene_,&chariotPeer->scene_):
            bottomLeft?scene_.reappendToDetached7F8211568(d,animationEntity(),racer.detached7F8,
                &rightPeer->scene_,&leftPeer->scene_,&bottomRightPeer->scene_,&chariotPeer->scene_):
            scene_.reappendToDetached7F8211998(d,animationEntity(),racer.detached7F8,
                &rightPeer->scene_,&leftPeer->scene_,bottomRightPeer?&bottomRightPeer->scene_:nullptr,
                bottomLeftPeer?&bottomLeftPeer->scene_:nullptr);
        need(reappended.okay(),"original210CA4/210F68/21126C/211568/211998 model alias append failed");
        modelReappended_=true;
        // Same concrete source caller after210CA4/210F68/21126C/211568/211998.
        // Do not admit the native completed join until its material pass returns.
        // On failure the real aliases/material-prefix state remain for close;
        // no automatic append replay or later-effect completion is supplied.
        failurePC_=right?0x210cb4u:left?0x210f78u:
            bottomRight?0x21127cu:bottomLeft?0x211578u:0x2119a8u;
        const auto materialPass=prepareMaterials213380();
        if(!materialPass.okay()){
            failurePC_=materialPass.sourcePC;
            throw std::runtime_error("original213380 canonical material pass stopped");
        }
        if(modelRole_==Rig::chariot){
            failurePC_=0x211f88u;
            need(body340_&&!racer.body340&&
                body340_->canonicalNumericAddress()==&fixture_->state.bodies.main&&
                body340_->scene288()==racer.scene190,
                "original main340 canonical owner publication rejected");
            racer.body340=body340_.get();
            const auto bodyAppended=body340_->append211FA8(d,racer.racer);
            failurePC_=bodyAppended.sourcePC;
            need(bodyAppended.status==g::OriginalWreckComponentStatus50LA::complete,
                "original main340 Entity append stopped");
            failurePC_=0x212014u;
            need(gravity344_&&!racer.gravity344&&
                gravity344_->canonicalAccelerationAddress()==&fixture_->state.bodies.main.selectedGravity,
                "original gravity344 canonical owner publication rejected");
            racer.gravity344=gravity344_.get();
            const auto gravityAppended=gravity344_->append212034(d,racer.racer);
            failurePC_=gravityAppended.sourcePC;
            need(gravityAppended.status==g::OriginalWreckComponentStatus50LA::complete,
                "original gravity344 Entity append stopped");
            const auto gravityBound=gravity344_->bindTarget212048(*body340_);
            failurePC_=gravityBound.sourcePC;
            need(gravityBound.status==g::OriginalWreckComponentStatus50LA::complete,
                "original gravity344 main target binding stopped");
            if(handlingRoute_==HandlingRoute::source){
                //Fresh source chronology cannot inherit the legacy jump over
                //348/34C/350/690/694 into joint360. Its backing may exist, but
                //no alias/append/native consumer publication occurs here.
                sourceModelPrefixComplete_=true;sourceConstructorPC_=0x21204cu;failurePC_=sourceConstructorPC_;return;
            }
            failurePC_=0x212688u;
            need(joint360_&&!racer.joint360&&
                joint360_->canonicalNumericAddress()==&fixture_->state.bodies.joint,
                "original joint360 canonical owner publication rejected");
            racer.joint360=joint360_.get();
            const auto result=joint360_->append2126A8(d,racer.racer);failurePC_=result.sourcePC;
            need(result.status==g::OriginalWreckComponentStatus50LA::complete,
                "original joint360 Entity append stopped");
            if(nativeSecondaryContactsRequested_){
                auto& bindings=joint360_->nativeBodies_;
                const auto* a=nativeSecondaryContacts_.get();
                need(!bindings.published&&!bindings.revoked&&a&&!a->revoked_&&
                    bindings.bank==bank_&&bindings.frame==arena_&&bindings.model==this&&
                    bindings.generation==generation_&&bindings.slot==slot_&&
                    bindings.a==a&&bindings.b==body340_.get()&&
                    a->bank_==bank_&&a->frame_==arena_&&a->generation_==generation_&&a->slot_==slot_&&
                    a->parameters_==&fixture_->parameters.bodies.secondary&&
                    a->physical19C_==rig_->physical&&
                    racer_==&racer&&racer.prepared()&&racer.racer.reference00&&racer.racer.objectKey&&
                    racer.racer.sessionGeneration==generation_&&
                    racer.racer.kind==g::OriginalWreckComponentKind50LA::racer27E130&&
                    racer.joint360==joint360_.get()&&racer.body340==body340_.get()&&
                    racer.scene190==body340_->scene288(),
                    "native joint A/B publication identity rejected");
                // Retain only local admission until all later fallible
                // terrain/aero producer checks have also returned.
                pendingNativeJoint=joint360_.get();
            }
        }
        if(nativeRacerTerrain_){
            //Later actual successful model publication owns the real racer.
            //Source21224C binds694+9C=racer, separately from212258+A0=340.
            //This explicit native join is delayed until all existing model
            //publication calls above returned. No later fallible effect follows.
            failurePC_=0x21224cu;
            auto& owner=*nativeRacerTerrain_;
            need(modelRole_==Rig::chariot&&nativeSecondaryContactsRequested_&&
                !owner.published_&&!owner.revoked_&&owner.bank_==bank_&&owner.frame_==arena_&&
                owner.model_==this&&owner.slot_==slot_&&owner.generation_==generation_&&
                racer_==&racer&&racer.prepared()&&racer.racer.reference00&&racer.racer.objectKey&&
                racer.racer.sessionGeneration==generation_&&
                racer.racer.kind==g::OriginalWreckComponentKind50LA::racer27E130&&
                body340_&&racer.body340==body340_.get()&&racer.scene190==body340_->scene288()&&
                nativeAero_&&!nativeAero_->published_&&!nativeAero_->revoked_&&
                nativeAero_->bank_==bank_&&nativeAero_->frame_==arena_&&nativeAero_->model_==this&&
                nativeAero_->body_==body340_.get()&&nativeAero_->application_==application_.get()&&
                nativeAero_->slot_==slot_&&nativeAero_->generation_==generation_,
                "native racer terrain9C publication ownership rejected");
            owner.racer_=&racer;owner.racerKey_=racer.racer.objectKey;owner.published_=true;
            //Both identity preflights completed before either publication.
            nativeAero_->racer_=&racer;nativeAero_->racerKey_=racer.racer.objectKey;
            nativeAero_->published_=true;
        }
        if(pendingNativeJoint){
            // Final successful producer boundary: no fallible effect follows.
            auto& bindings=pendingNativeJoint->nativeBodies_;
            bindings.racer=&racer;bindings.racerKey=racer.racer.objectKey;
            bindings.published=true;
        }
        finished_=true;failurePC_=0;
    }

} // namespace native_racer_model50LA
