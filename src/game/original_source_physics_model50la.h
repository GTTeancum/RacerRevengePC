#pragma once
#include "original_source_physics_object50la.h"
#include "original_racer_model_scene50la.h"
#include "original_racer_model_packets50la.h"
#include "original_source_collision_observer50la.h"
#include "original_source_portal_overlap231150.h"
#include <array>
#include <memory>
#include <new>
#include <string_view>
#include <cstdio>
#include "../assets/original_source_psg_missing50la.h"
#include "../assets/original_standard_material_table50la.h"
namespace rr::game {
//Private conditional165C30 prefix. BASE PhysicsObject and Model+8 are
//different counted identities; neither a native resource nor loader return
//is inferred from a returned2403A0 constructor.
class OriginalSourcePhysicsModel50LA final {
    friend class OriginalSourcePhysicsBody50LA;
    friend class ::ra_static_model_cold50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    using Base=OriginalSourcePhysicsObject50LA;
    using Scene=OriginalRacerModelSceneOwner50LA;
    using C=OriginalWreckComponent50LA;
    using V=std::array<std::uint32_t,4>;
    enum class Status : std::uint8_t { reached,bounded,owner };
    struct Result {Status status=Status::owner;std::uint32_t sourcePC=0;};
    const void* bodyModelBorrower_=nullptr; //Exact raw288 borrower, no source retain.
    Base* base_=nullptr; //Exact borrowed owner; token forbids earlier withdrawal.
    std::unique_ptr<Scene> model_;
    std::array<SceneNode49P,rr::assets::maxModelNodes49O> nodes_{};
    std::uint32_t graphCount_=1;
    bool rowsEntered_=false,rowsReturned_=false;
    std::uint32_t rowsImporterPC_=0;
    std::optional<std::uint32_t> rowsReturnV0_;
    std::shared_ptr<rr::assets::OriginalSourcePsgFile50LA> rowsFile_;
    std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> rowsResolver_;
    rr::assets::OriginalStandardModelFileView50LA rowsNativeFile_{};
    std::array<bool,rr::assets::maxModelNodes49O> childLedgerWithdrawn_{};
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> materialTable_;
    const std::string* materialPrefix_=nullptr; //Exact live36B970 for source reads; history for cancellation.
    bool materialEntered_=false,materialReturned_=false;
    bool quantizationEntered_=false,quantizationReturned_=false;
    std::uint32_t materialPC_=0,quantizationPC_=0;
    bool geometryEntered_=false,geometryAllocated_=false;
    bool geometryLoadEntered_=false,geometryLodsReturned_=false;
    bool geometryPaletteEntered_=false,geometryReturned_=false;
    std::uint32_t geometryAllocationPC_=0,geometryLoadPC_=0,geometryPalettePC_=0;
    bool prototypeEntered_=false,prototypeReturned_=false;
    std::uint32_t prototypePC_=0;
    bool rowTableEntered_=false,rowTableReturned_=false;
    std::uint32_t rowTablePC_=0;
    const C* renderableReturnedEntity_=nullptr; //Typed26E820 V0, never final240740 return.
    OriginalRacerModelPackets50LA sourcePackets_;
    bool packetsEntered_=false,packetsReturned_=false;
    std::uint32_t packetsPC_=0;
    const C* loaderReturnedEntity_=nullptr; //Actual final240740 typed Entity+8, not literal1.
    bool attachEntered_=false,attachReturned_=false,modelAttached_=false,detachedNative_=false;
    std::optional<std::uint32_t> attachLock_;
    std::uint32_t attachPC_=0;
    bool placementEntered_=false,placementReturned_=false,positionCopied_=false,resetReturned_=false;
    std::uint32_t placementPC_=0;
    std::array<std::optional<bool>,3> positionEqualsZero_{};
    std::array<std::uint32_t,3> positionCompared_{},zeroCompared_{};
    std::uint8_t comparisonReadMask_=0;
    V currentPosition50_{},resetInput_{};
    bool resetEntered_=false;
    std::array<std::uint32_t,16> localBeforeReset_{},worldBeforeReset_{};
    std::array<std::uint8_t,rr::assets::maxModelNodes49O> flagsAfterReset_{};
    V position_{},look_{},up_{};
    const char* requestedName_=nullptr; //Historical argument only; not read by this stage.
    std::uint8_t vectorStoreMask_=0;
    bool entered_=false,waterStored_=false,allocationAttempted_=false;
    bool allocationReturned_=false,constructorReturned_=false,published_=false;
    bool ledgerWithdrawn_=false,modelRetired_=false,nativeWithdrawn_=false;
    std::uint32_t pc_=0;
    std::shared_ptr<rr::assets::OriginalSourcePsgMissingLookup50LA> missing_;
    bool missingEntered_=false,missingLogAttempted_=false,sourceReferenceDropped_=false;
    bool sourceSceneReleased_=false,sourceOuterDeleted_=false,sourceReleaseReturned_=false;
    std::optional<int> missingLogResult_;
    std::optional<std::uint32_t> sourceReleaseLock_,sourceLoaderReturnV0_;
    std::uint32_t destructorStoreMask_=0,destructorVtable1A8_=0,destructorVtable10_=0;
    std::uint8_t destructorNullSkips_=0;
    bool nameEntered_=false,noOpReturned_=false,directoryRead_=false,directoryCopied_=false;
    bool separatorAppended_=false,nameCopied_=false,lowercaseReturned_=false;
    bool extensionStored_=false,modelNameStored_=false,nameReturned_=false;
    const std::string* directoryIdentity_=nullptr; //Historical argument; never dereferenced by cleanup.
    std::string requestedPsg_;
    std::size_t lowercaseStores_=0,lowercaseInputSize_=0;
    std::optional<std::int32_t> sourceStackCount680_,sourceHandle678_;
    std::optional<std::uint32_t> sourceWarmFlag684_;
    Scene::SourceWorld28B550 sourceWorld_;
    Scene::SourceWorld28B550 directCollisionWorld_;
    const void* directWorldOccurrence_=nullptr;
    const void* directWorldBody_=nullptr;
    bool directWorldAttempted_=false,directWorldReturned_=false;
    std::uint32_t directWorldPC_=0;
    SceneGraph49P graph()const noexcept {return {const_cast<SceneNode49P*>(nodes_.data()),graphCount_};}
    Result at(Status s,std::uint32_t pc)noexcept {pc_=pc;return {s,pc};}
    Result beginSource165C30(Base& base,const V& position,const V& look,const V& up,
        const char* actualModelName)noexcept {
        if(entered_||nativeWithdrawn_||!base.preflightNative()||!base.wrapperPublished_||
           base.nativeWithdrawn_||base.nestedModelBorrower_||!actualModelName||
           (base.object_&&!base.constructorReturned_))return {Status::owner,0x165c30};
        //The source root authenticates descriptor/filename/same-App and
        //reserves one additional ledger slot before entering this method.
        base_=&base;base.nestedModelBorrower_=this;entered_=true;
        position_=position;look_=look;up_=up;requestedName_=actualModelName;
        //165C4C..80 copies arguments to stack before first object dereference.
        if(!base.object_)return at(Status::bounded,0x165c88);
        auto& o=*base.object_;
        pc_=0x165c88;o.fields.vector(0x50,position_);base.expectedFields_.vector(0x50,position_);vectorStoreMask_|=1u;
        pc_=0x165c90;o.fields.vector(0x60,look_);base.expectedFields_.vector(0x60,look_);vectorStoreMask_|=2u;
        pc_=0x165c98;o.fields.vector(0x70,up_);base.expectedFields_.vector(0x70,up_);vectorStoreMask_|=4u;
        //Actual inspected type2 caller passes t1=0. No other water authority.
        pc_=0x165c9c;o.waterB0=nullptr;o.waterKnown=true;waterStored_=true;
        pc_=0x165ca0;
        if(base.application_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())
            return at(Status::bounded,pc_); //Native count admission, before represented allocation.
        allocationAttempted_=true;model_.reset(new(std::nothrow) Scene);allocationReturned_=true;
        if(model_){
            OriginalRacerModelSceneBinding50LA binding;
            binding.sessionGeneration=base.generation_;
            binding.actualObjectCount423F60=&base.application_->objectCount423F60();
            binding.actualAttachmentLock423F58=&base.application_->attachmentLock423F58();
            binding.actualTypes383408=&base.application_->types383408();
            binding.modelIndex=static_cast<SceneNodeIndex49P>(0);
            pc_=0x165cb4;const auto result=model_->constructSourceDirectRoot2403A0(binding,graph(),true);
            if(!result.okay())return at(Status::bounded,result.sourcePC);
            constructorReturned_=true;
        }
        //Actual nullable publication follows constructor return, before the
        //virtualA8 read at165CCC. Constructor refusal retains priorNULL14C.
        pc_=0x165cbc;o.model14C=model_?static_cast<const void*>(&model_->model_):nullptr;
        o.modelKnown=true;published_=true;
        return at(Status::reached,0x165cc0);
    }
    Result prepareSourceModelName26E848(const std::string& actualDirectory36B860,
        std::string_view actualName)noexcept {
        if(nameEntered_||!preflightNative()||!published_||ledgerWithdrawn_||modelRetired_||
           actualName.data()!=requestedName_||actualName.size()>=256||
           actualName.find('\0')!=std::string_view::npos||actualName.data()[actualName.size()]!='\0')
            return {Status::owner,0x165cc0}; //Admission refusal cannot overwrite earlier source PC/history.
        nameEntered_=true;
        //Actual nullable14C first virtual-table read. No false loaderzero.
        if(!model_)return at(Status::bounded,0x165ccc);
        if(!constructorReturned_||model_->sourceConstructor_.vtable1A8!=0x3d1f40)
            return at(Status::owner,0x165cd0);
        //Actual165CD4 passes a2=0/a3=0;240740 reaches26E848 on same Model.
        pc_=0x26e848;const auto noOp=model_->sourceNoOp141C20();
        if(!noOp.okay())return at(Status::bounded,noOp.sourcePC);
        noOpReturned_=true;directoryIdentity_=&actualDirectory36B860;
        pc_=0x26e850;directoryRead_=true;
        sourceStackCount680_=0; //26E858 before any filename-copy branch.
        if(actualDirectory36B860.find('\0')!=std::string::npos)
            return at(Status::bounded,0x26e864); //Native string projection limit, not source return.
        try {
            if(!actualDirectory36B860.empty()){
                pc_=0x26e86c;requestedPsg_=actualDirectory36B860;directoryCopied_=true;
                if(actualDirectory36B860.back()!='\\'){
                    pc_=0x26e8a4;requestedPsg_+='\\';separatorAppended_=true;
                }
                pc_=0x26e8b0;requestedPsg_.append(actualName.data(),actualName.size());
            }else {pc_=0x26e8c4;requestedPsg_.assign(actualName.data(),actualName.size());}
            nameCopied_=true;
        }catch(...){return at(Status::bounded,pc_);} //Retain prior completed copies/stores.
        lowercaseInputSize_=requestedPsg_.size();
        for(std::size_t i=0;i<requestedPsg_.size();++i){
            pc_=0x26e8d8;const auto c=static_cast<unsigned char>(requestedPsg_[i]);
            if(c>=128)return at(Status::bounded,pc_); //Accepted ASCII-only native30B158 adapter.
            pc_=0x26e8e0;if(c>='A'&&c<='Z')requestedPsg_[i]=static_cast<char>(c+0x20u);
            ++lowercaseStores_;
        }
        lowercaseReturned_=true;pc_=0x26e8f8;
        const auto dot=requestedPsg_.find_last_of('.'); //Actual1169D0 searches entire C string, no slash normalization.
        try {
            if(dot==std::string::npos){pc_=0x26e928;requestedPsg_+=".psg";}
            else {pc_=0x26e910;requestedPsg_.replace(dot,std::string::npos,".psg");}
        }catch(...){return at(Status::bounded,pc_);}
        extensionStored_=true;pc_=0x26e934;
        const auto named=model_->prepareDirectRoot26E934(graph(),requestedPsg_);
        if(!named.okay())return at(Status::bounded,named.sourcePC);
        modelNameStored_=true;
        sourceWarmFlag684_=0;sourceHandle678_=-1; //26E940 and26E948 branch delay.
        //a2=0 skips separate root/191bit1/28BEF0. No lookup/return is invented.
        nameReturned_=true;return at(Status::reached,0x26e9ac);
    }
    Result importSourceRows28CAC0(std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> resolver,
        const rr::assets::OriginalStandardModelFileView50LA& file,
        std::shared_ptr<rr::assets::OriginalSourcePsgFile50LA> sourceFile)noexcept {
        if(rowsEntered_||missingEntered_||!nameReturned_||pc_!=0x26e9ac||!preflightNative()||
           !model_||!constructorReturned_||!published_||ledgerWithdrawn_||modelRetired_||graphCount_!=1||
           !resolver||!sourceFile||!sourceFile->current()||!sourceFile->returnedCold()||
           sourceFile->resolver()!=resolver.get()||sourceFile->nativeFile().resolver!=file.id.resolver||
           sourceFile->nativeFile().row!=file.id.row||file.id.resolver!=resolver.get())
            return {Status::owner,0x26ebcc};
        //Root authenticates same lookup/caller and reserves63 possible ledger
        //slots BEFORE this call. No rows, allocations or counts are invented.
        rowsFile_=std::move(sourceFile);rowsResolver_=std::move(resolver);rowsNativeFile_=file;
        rowsEntered_=true;auto liveGraph=graph();
        const auto imported=model_->importSourceDirectRows28CAC0(liveGraph,
            rr::assets::maxModelNodes49O,rowsResolver_,rowsNativeFile_,rowsFile_);
        graphCount_=static_cast<std::uint32_t>(liveGraph.count);rowsImporterPC_=imported.sourcePC;
        //Caller must enroll countedChildEntityNative(row) on EVERY exit,
        //including registry refusal before original rowpointer publication.
        if(!imported.okay())return at(Status::bounded,imported.sourcePC);
        const auto& rows=model_->sourceDirectRows_;
        if(!rows.returned||!rows.returnCount||imported.sourcePC!=0x28cd5cu||
           *rows.returnCount!=graphCount_)return at(Status::owner,imported.sourcePC);
        rowsReturned_=true;rowsReturnV0_=*rows.returnCount; //Actual importerV0, not caller stackstore.
        return at(Status::reached,0x28cd5c); //Root owns26ED4C/50 branch; no materials/loader return.
    }
    C* countedChildEntityNative(std::uint32_t row)const noexcept {
        if(!row||!model_||!rowsEntered_||modelRetired_||sourceSceneReleased_||
           row>=model_->binding_.sourceRowCount)return nullptr;
        auto* child=model_->sourceRowObject(row);
        return child&&child->constructed&&!child->released?model_->wreckRowStorage(row):nullptr;
    }
    bool rowsHistoryNative()const noexcept {
        if(graphCount_==0||graphCount_>rr::assets::maxModelNodes49O)return false;
        if(!rowsEntered_){
            if(graphCount_!=1||rowsReturned_||rowsReturnV0_||rowsFile_||rowsResolver_||rowsImporterPC_)return false;
            for(bool retired:childLedgerWithdrawn_)if(retired)return false;
            return !model_||!model_->sourceDirectRows_.attempted;
        }
        if(missingEntered_||!model_||!nameReturned_||!rowsFile_||!rowsResolver_||
           !rowsFile_->returnedCold()||!rowsFile_->preflightNative()||
           rowsFile_->resolver()!=rowsResolver_.get()||rowsNativeFile_.id.resolver!=rowsResolver_.get()||
           rowsFile_->nativeFile().resolver!=rowsNativeFile_.id.resolver||
           rowsFile_->nativeFile().row!=rowsNativeFile_.id.row||childLedgerWithdrawn_[0])return false;
        const auto& rows=model_->sourceDirectRows_;
        if(rows.attempted){
            if(rows.nativeScratchWithdrawn){if(!modelRetired_)return false;}
            else if(rows.sourceFile!=rowsFile_||rows.resolver!=rowsResolver_||
                rows.bytes!=rowsNativeFile_.bytes||rows.file.resolver!=rowsNativeFile_.id.resolver||rows.file.row!=rowsNativeFile_.id.row)return false;
            if(rows.pc!=rowsImporterPC_||rows.countStored!=rows.rowZeroPublished||
               rows.rowZeroPublished!=rows.published[0]||
               (rows.rowZeroPublished&&rows.publishedRows[0]!=&model_->model_))return false;
            if(model_->sourceConstructor_.rowsBound){
                if(!rows.countStored||model_->binding_.sourceRowCount!=graphCount_||
                   model_->model_.nodeCount1A0!=graphCount_)return false;
            }else if(graphCount_!=1||model_->binding_.sourceRowCount)return false;
            for(std::uint32_t row=0;row<rr::assets::maxModelNodes49O;++row){
                if(rows.readMask[row]&~std::uint8_t(63))return false;
                if(row&&rows.published[row]&&!rows.allocationReturned[row])return false;
                if(rows.linked[row]&&rows.readMask[row]!=63u)return false;
                if(childLedgerWithdrawn_[row]&&(!row||!model_->sourceRowObject(row)||
                   !model_->sourceRowObject(row)->constructed))return false;
            }
        }else if(graphCount_!=1||model_->binding_.sourceRowCount||rowsReturned_)return false;
        if(rowsReturned_!=rows.returned)return false;
        if(rowsReturned_){
            if(!rowsReturnV0_||!rows.returnCount||*rowsReturnV0_!=*rows.returnCount||
               *rowsReturnV0_!=graphCount_||rowsImporterPC_!=0x28cd5cu||
               (!materialEntered_&&pc_!=0x28cd5cu))return false;
        }else if(rowsReturnV0_||pc_!=rowsImporterPC_)return false;
        return true;
    }
    Result loadSourceMaterials26EE18(
        std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> table,
        const std::string* actualPrefix36B970,rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic)noexcept {
        diagnostic={};
        if(bodyModelBorrower_||materialEntered_||!rowsReturned_||pc_!=0x28cd5cu||!preflightNative()||
           !model_||modelRetired_||ledgerWithdrawn_||!rowsFile_||!rowsFile_->current()||
           (model_->fields_.flags1B3&4u)||!table||!table->prepared()||!actualPrefix36B970)
            return {Status::owner,0x26ee18};
        //Future root authenticates table against SAME App382B38 domain and
        //sourceNames/screenApplication, plus exactlive App36B970 identity.
        materialTable_=std::move(table);materialPrefix_=actualPrefix36B970;materialEntered_=true;
        const rr::assets::OriginalStandardMaterialPath50LA path{materialPrefix_};
        const auto result=model_->loadSourceMaterials26E640(materialTable_,path,diagnostic);
        materialPC_=result.sourcePC;
        if(!result.okay())return at(Status::bounded,result.sourcePC);
        if(!model_->sourceMaterials_.returned||result.sourcePC!=0x26e810u)
            return at(Status::owner,result.sourcePC);
        materialReturned_=true;return at(Status::reached,0x26e810);
    }
    Result loadSourceQuantization26EE30()noexcept {
        if(bodyModelBorrower_||quantizationEntered_||!materialReturned_||pc_!=0x26e810u||
           !preflightNative()||!model_||modelRetired_||ledgerWithdrawn_||
           !rowsFile_||!rowsFile_->current()||model_->sourceMaterials_.nativeWithdrawn||
           model_->sourceConstructor_.vtable1A8!=0x3d1f40u)
            return {Status::owner,0x26ee30};
        quantizationEntered_=true;
        const auto result=model_->loadSourceQuantization240570();quantizationPC_=result.sourcePC;
        if(!result.okay())return at(Status::bounded,result.sourcePC);
        if(!model_->sourceQuantization_.returned||result.sourcePC!=0x2406dcu)
            return at(Status::owner,result.sourcePC);
        //Actual original caller ignoresV0. This is reached-return evidence,
        //not an invented successboolean. Root owns next26EE38 allocation.
        quantizationReturned_=true;return at(Status::reached,0x2406dc);
    }
    bool materialHistoryNative()const noexcept {
        if(!materialEntered_){
            return !materialTable_&&!materialPrefix_&&!materialReturned_&&!materialPC_&&
                !quantizationEntered_&&!quantizationReturned_&&!quantizationPC_&&
                (!model_||(!model_->sourceMaterials_.attempted&&!model_->sourceQuantization_.attempted));
        }
        if(!model_||!rowsReturned_||missingEntered_||!materialTable_||!materialPrefix_||
           (model_->fields_.flags1B3&4u)||!model_->preflightSourceMaterialsNative())return false;
        const auto& m=model_->sourceMaterials_;
        if(m.attempted){if(m.prefixAddress!=materialPrefix_||m.pc!=materialPC_)return false;}
        else if(materialReturned_)return false;
        if(materialReturned_!=m.returned||(materialReturned_&&materialPC_!=0x26e810u))return false;
        if(!quantizationEntered_)return !quantizationReturned_&&!quantizationPC_&&
            !model_->sourceQuantization_.attempted&&pc_==materialPC_;
        if(!materialReturned_)return false;
        const auto& q=model_->sourceQuantization_;
        //Independent source readmask: only fully returned scalar reads update
        //the Model words; partial copied bytes remain in typedFile receipt.
        const unsigned mask=q.readMask;
        if(mask&(mask+1u))return false;
        for(unsigned lane=0;lane<8;++lane)if(mask&(1u<<lane)){
            std::uint32_t actual=0;
            if(lane<4)std::memcpy(&actual,&model_->fields_.scale1E0[lane],4);
            else if(lane<7)std::memcpy(&actual,&model_->fields_.offset1D0[lane-4],4);
            else actual=model_->fields_.offsetEnabled1DC;
            if(actual!=q.words[lane])return false;
        }
        if(q.xEqualsOne&&mask!=255u)return false;
        if(q.yEqualsOne&&(!q.xEqualsOne||!*q.xEqualsOne))return false;
        if(q.flagStored){
            if(!q.xEqualsOne||(*q.xEqualsOne&&!q.yEqualsOne))return false;
            const bool scaled=!*q.xEqualsOne||!*q.yEqualsOne;
            if(bool(model_->fields_.flags1B2&0x40u)!=scaled)return false;
        }
        if(quantizationReturned_!=q.returned||q.returned!=q.flagStored||
           model_->quantizationLoaded_!=q.returned||(!geometryEntered_&&pc_!=quantizationPC_))return false;
        if(q.attempted&&q.returned&&quantizationPC_!=0x2406dcu)return false;
        return true;
    }
    Result allocateSourceGeometry26EE38()noexcept {
        if(bodyModelBorrower_||geometryEntered_||!quantizationReturned_||pc_!=0x2406dcu||
           !preflightNative()||!model_||modelRetired_||ledgerWithdrawn_||
           !rowsFile_||!rowsFile_->current()||model_->sourceMaterials_.nativeWithdrawn)
            return {Status::owner,0x26ee38};
        geometryEntered_=true;
        const auto result=model_->allocateSourceGeometry26EE38();geometryAllocationPC_=result.sourcePC;
        const auto& g=model_->sourceGeometry_;
        //This existing staged allocator deliberately returns phase even after
        //a real constructor and nullable publication. Only actual receipts
        //prove nonNULL240210 admission; native status is not sourceV0.
        if(g.attempted&&g.constructorReturned&&g.published&&g.owner&&
           g.published1B4==g.owner.get()&&result.sourcePC==0x26ee64u){
            geometryAllocated_=true;return at(Status::reached,0x26ee64);
        }
        return at(Status::bounded,result.sourcePC); //NULL1B4 stops26EE5C, never loaderzero.
    }
    Result loadSourceGeometry26E170()noexcept {
        if(bodyModelBorrower_||geometryLoadEntered_||!geometryAllocated_||pc_!=0x26ee64u||
           !preflightNative()||modelRetired_||ledgerWithdrawn_||!rowsFile_->current()||
           model_->sourceMaterials_.nativeWithdrawn||model_->sourceGeometry_.nativeWithdrawn)
            return {Status::owner,0x26ee64};
        geometryLoadEntered_=true;
        const auto result=model_->loadSourceGeometry26E170();geometryLoadPC_=result.sourcePC;
        if(model_->sourceGeometry_.loadAttempted&&model_->sourceGeometry_.lodsReturned&&
           result.sourcePC==0x24024cu){
            geometryLodsReturned_=true;return at(Status::reached,0x24024c);
        }
        return at(Status::bounded,result.sourcePC);
    }
    Result finishSourceGeometry240210()noexcept {
        if(bodyModelBorrower_||geometryPaletteEntered_||!geometryLodsReturned_||pc_!=0x24024cu||
           !preflightNative()||modelRetired_||ledgerWithdrawn_||!rowsFile_->current()||
           model_->sourceMaterials_.nativeWithdrawn||model_->sourceGeometry_.nativeWithdrawn)
            return {Status::owner,0x24024c};
        geometryPaletteEntered_=true;
        const auto result=model_->finishSourceGeometry240210();geometryPalettePC_=result.sourcePC;
        if(!result.okay())return at(Status::bounded,result.sourcePC);
        if(!model_->sourceGeometry_.geometryCallReturned||result.sourcePC!=0x240394u)
            return at(Status::owner,result.sourcePC);
        geometryReturned_=true;return at(Status::reached,0x240394);
        //Root owns26EE6C continuation. No prototype/cache/fileclose/loaderreturn.
    }
    Result publishSourceGeometryPrototype26EE74()noexcept {
        if(bodyModelBorrower_||prototypeEntered_||!geometryReturned_||pc_!=0x240394u||
           !preflightNative()||!model_||modelRetired_||ledgerWithdrawn_||
           !rowsFile_||!rowsFile_->current()||model_->sourceMaterials_.nativeWithdrawn||
           model_->sourceGeometry_.nativeWithdrawn)
            return {Status::owner,0x26ee74};
        prototypeEntered_=true;
        const auto result=model_->publishSourceGeometryPrototype26EE74();prototypePC_=result.sourcePC;
        if(!result.okay())return at(Status::bounded,result.sourcePC);
        if(!model_->sourceGeometry_.prototypeStored||result.sourcePC!=0x26ee78u)
            return at(Status::owner,result.sourcePC);
        prototypeReturned_=true;return at(Status::reached,0x26ee78);
        //Raw same-Model prototype borrow, not a reference retain. Root alone
        //issues actual1326A0 association and131040; neither is implied here.
    }
    bool geometryHistoryNative()const noexcept {
        if(!prototypeEntered_&&(prototypeReturned_||prototypePC_))return false;
        if(prototypeEntered_&&(!geometryEntered_||!geometryReturned_))return false;
        if(!geometryEntered_)return !geometryAllocated_&&!geometryLoadEntered_&&!geometryLodsReturned_&&
            !geometryPaletteEntered_&&!geometryReturned_&&!geometryAllocationPC_&&!geometryLoadPC_&&
            !geometryPalettePC_&&(!model_||(!model_->sourceGeometry_.attempted&&!model_->fields_.geometry1B4));
        if(!model_||!quantizationReturned_||!materialReturned_||!rowsReturned_||
           !model_->preflightSourceGeometryNative())return false;
        const auto& g=model_->sourceGeometry_;
        if(!prototypeEntered_&&(g.prototypeAttempted||g.prototypeStored))return false;
        if(prototypeEntered_){
            if(prototypeReturned_!=g.prototypeStored||(!rowTableEntered_&&pc_!=prototypePC_)||
               (g.prototypeAttempted&&g.pc!=prototypePC_)||
               (prototypeReturned_&&prototypePC_!=0x26ee78u))return false;
            //Scene preflight authenticates exact sourcePrototypeFile and raw
            //same-Model identity. It remains valid after source/file closure
            //without invoking reader.current or recursively authorizing IO.
        }
        if(geometryAllocated_!=(g.constructorReturned&&g.published&&g.allocationNonNull))return false;
        if(geometryAllocated_&&geometryAllocationPC_!=0x26ee64u)return false;
        if(!geometryLoadEntered_)return !geometryLodsReturned_&&!geometryPaletteEntered_&&!geometryReturned_&&
            !geometryLoadPC_&&!geometryPalettePC_&&!g.loadAttempted&&pc_==geometryAllocationPC_&&
            (!g.attempted||g.pc==geometryAllocationPC_);
        if(!geometryAllocated_||geometryLodsReturned_!=g.lodsReturned)return false;
        if(geometryLodsReturned_&&geometryLoadPC_!=0x24024cu)return false;
        if(!geometryPaletteEntered_)return !geometryReturned_&&!geometryPalettePC_&&!g.paletteAttempted&&
            pc_==geometryLoadPC_&&(!g.loadAttempted||g.pc==geometryLoadPC_);
        if(!geometryLodsReturned_||geometryReturned_!=g.geometryCallReturned||
           (!prototypeEntered_&&(pc_!=geometryPalettePC_||
               (g.paletteAttempted&&g.pc!=geometryPalettePC_))))return false;
        return !geometryReturned_||geometryPalettePC_==0x240394u;
    }
    Result allocateClosedSourceRowTable26EE9C()noexcept {
        if(bodyModelBorrower_||rowTableEntered_||!prototypeReturned_||pc_!=0x26ee78u||
           !preflightNative()||!model_||modelRetired_||ledgerWithdrawn_||
           !rowsFile_||!rowsFile_->sourceCloseReturned()||!rowsFile_->preflightNative()||
           model_->sourceMaterials_.nativeWithdrawn||model_->sourceGeometry_.nativeWithdrawn||
           !model_->sourceGeometry_.owner||
           !rowsFile_->associatedGeometryCurrent(model_->sourceGeometry_.owner.get(),model_.get()))
            return {Status::owner,0x26ee98};
        //Root has executed actual same-file1326A0 and131040, whose original
        //return is ignored. Existing Scene proves those exact receipts before
        //nullable row-array allocation/publication and logical pointer copy.
        rowTableEntered_=true;
        const auto result=model_->allocateClosedSourceRowTable26EE9C();rowTablePC_=result.sourcePC;
        if(!result.okay())return at(Status::bounded,result.sourcePC);
        const auto& h=model_->sourceRowTable_;
        if(!h.returned||h.returnedEntityV0!=model_->wreckModelStorage()||result.sourcePC!=0x26eee8u)
            return at(Status::owner,result.sourcePC);
        renderableReturnedEntity_=h.returnedEntityV0;rowTableReturned_=true;
        return at(Status::reached,0x26eee8); //Only26E820;240740 packets remain unissued.
    }
    bool rowTableHistoryNative()const noexcept {
        if(!rowTableEntered_)return !rowTableReturned_&&!rowTablePC_&&!renderableReturnedEntity_&&
            (!model_||(!model_->sourceRowTable_.attempted&&!model_->fields_.sourceRows1B8));
        if(!model_||!prototypeReturned_||!rowsFile_||!rowsFile_->sourceCloseReturned()||
           !rowsFile_->preflightNative()||!model_->preflightSourceRowTableNative()||(!packetsEntered_&&pc_!=rowTablePC_))
            return false;
        const auto& h=model_->sourceRowTable_;
        if(h.attempted&&(h.closedFile!=rowsFile_.get()||h.pc!=rowTablePC_))return false;
        if(rowTableReturned_!=h.returned)return false;
        if(rowTableReturned_)return rowTablePC_==0x26eee8u&&renderableReturnedEntity_==model_->wreckModelStorage()&&
            h.returnedEntityV0==renderableReturnedEntity_;
        return !renderableReturnedEntity_&&!h.returnedEntityV0;
    }
    Result constructSourcePackets240740()noexcept {
        if(bodyModelBorrower_||packetsEntered_||!rowTableReturned_||pc_!=0x26eee8u||
           !preflightNative()||!model_||modelRetired_||ledgerWithdrawn_||
           !model_->preflightSourceRenderableReturn26E820Native()||
           model_->sourceRenderableEntity26E820Native()!=renderableReturnedEntity_||
           !model_->sourceGeometry_.owner)return {Status::owner,0x240768};
        packetsEntered_=true;
        //Original165CD8 passes raw a3=0. Constructor a1=1 and Standard's
        //descriptor dynamic flag are different source arguments.
        const auto result=sourcePackets_.constructSourcePackets240740(
            *model_,*model_->sourceGeometry_.owner,0u);packetsPC_=result.sourcePC;
        if(!result.okay())return at(Status::bounded,result.sourcePC);
        const auto& h=sourcePackets_.sourcePackets_;
        if(!h.returned||h.returnedEntityV0!=renderableReturnedEntity_||result.sourcePC!=0x2407d8u)
            return at(Status::owner,result.sourcePC);
        loaderReturnedEntity_=h.returnedEntityV0;packetsReturned_=true;
        return at(Status::reached,0x2407d8); //Root owns subsequent165CDC; no placement/attachment here.
    }
    bool packetsHistoryNative()const noexcept {
        const auto& h=sourcePackets_.sourcePackets_;
        if(!packetsEntered_)return !packetsReturned_&&!packetsPC_&&!loaderReturnedEntity_&&!h.entered&&
            (!model_||(!model_->fields_.packets1F0&&!model_->sourcePacketBorrower_));
        if(!model_||!rowTableReturned_||(!attachEntered_&&pc_!=packetsPC_)||!sourcePackets_.preflightSourcePacketsNative())return false;
        if(h.entered){
            if(h.model!=model_.get()||h.rawA3||h.pc!=packetsPC_)return false;
            if(!h.nativeWithdrawn&&h.geometry!=model_->sourceGeometry_.owner.get())return false;
            if(h.nativeWithdrawn&&(model_->fields_.packets1F0||model_->sourcePacketBorrower_))return false;
        }else if(model_->fields_.packets1F0||model_->sourcePacketBorrower_)return false;
        if(packetsReturned_!=h.returned)return false;
        if(packetsReturned_)return packetsPC_==0x2407d8u&&loaderReturnedEntity_==renderableReturnedEntity_&&
            h.returnedEntityV0==loaderReturnedEntity_;
        return !loaderReturnedEntity_&&!h.returnedEntityV0;
    }
    template<class ActualAttach>
    Result attachSourceModel141960(ActualAttach&& attach)noexcept {
        if(bodyModelBorrower_||attachEntered_||!packetsReturned_||pc_!=0x2407d8u||
           !preflightNative(true)||!model_||modelRetired_||ledgerWithdrawn_||
           sourcePackets_.sourcePackets_.nativeWithdrawn||!base_||!base_->object_||
           loaderReturnedEntity_!=model_->wreckModelStorage()||
           base_->object_->model14C!=&model_->model_)
            return {Status::owner,0x165cdc};
        auto& child=*model_->wreckModelStorage();auto& parent=base_->object_->entity;
        if(base_->nestedChild141960_||parent.firstChild18||parent.descendants24||
           child.parent1C||child.previous10||child.next14||child.firstChild18||child.mask20||child.descendants24)
            return {Status::owner,0x165cf0};
        attachEntered_=true;attachPC_=0x141970;pc_=attachPC_;
        attachLock_=base_->application_->attachmentLock423F58();
        if(!*attachLock_){
            //Friended root supplies the real SAME Application authority; both
            //counted identities are already enrolled. No reference/count change.
            const auto result=attach(parent,child,0u,*attachLock_);
            if(result.status!=OriginalWreckComponentStatus50LA::complete){
                attachPC_=result.sourcePC;return at(Status::bounded,attachPC_);
            }
            base_->nestedChild141960_=&child;modelAttached_=true;
        } //Actual lockedNULL return ignored by165C30; no fabricated link.
        attachReturned_=true;attachPC_=0x165cf8;return at(Status::reached,attachPC_);
    }
    bool attachmentHistoryNative()const noexcept {
        if(!attachEntered_)return !attachReturned_&&!modelAttached_&&!detachedNative_&&!attachLock_&&!attachPC_&&
            (!base_||!base_->nestedChild141960_)&&(!model_||!model_->wreckModelStorage()->parent1C);
        if(!packetsReturned_||!model_||!base_||!base_->object_||!attachLock_||(!placementEntered_&&pc_!=attachPC_)||
           (detachedNative_&&!modelAttached_))return false;
        if(attachReturned_&&((*attachLock_?modelAttached_:!modelAttached_)||attachPC_!=0x165cf8u))return false;
        if(*attachLock_&&modelAttached_)return false;
        const auto& child=*model_->wreckModelStorage();const auto& parent=base_->object_->entity;
        const bool live=modelAttached_&&!detachedNative_;
        if(base_->nestedChild141960_!=(live?&child:nullptr)||!base_->sourceChildChainCurrentNative()||
           child.parent1C!=(live?&parent:nullptr)||child.previous10||
           (!live&&child.next14)||child.firstChild18||child.mask20||child.descendants24)return false;
        return true; //SceneObject.parent180 remains independently detached, never aliased to Entity.parent1C.
    }
    Result placeSourceModel165CF8()noexcept {
        if(bodyModelBorrower_||placementEntered_||!attachReturned_||pc_!=0x165cf8u||
           !preflightNative()||!model_||modelRetired_||ledgerWithdrawn_||
           sourcePackets_.sourcePackets_.nativeWithdrawn||!base_||!base_->object_||
           !base_->zero433B10_)return {Status::owner,0x165cf8};
        placementEntered_=true;currentPosition50_=position_;
        const auto supported=[](std::uint32_t bits){const auto exponent=bits&0x7f800000u;
            return !(bits&0x7fffffffu)||(exponent&&exponent!=0x7f800000u);};
        constexpr std::uint32_t comparisons[3]={0x165d08,0x165d30,0x165d58};
        bool sentinel=true;
        for(unsigned lane=0;lane<3&&sentinel;++lane){
            placementPC_=comparisons[lane];
            positionCompared_[lane]=base_->object_->fields.words[0x50/4+lane];
            std::memcpy(&zeroCompared_[lane],&(*base_->zero433B10_)[lane],4);
            comparisonReadMask_=static_cast<std::uint8_t>(comparisonReadMask_|(1u<<lane));
            if(!supported(positionCompared_[lane])||!supported(zeroCompared_[lane]))
                return at(Status::bounded,placementPC_); //Native C.EQ.S support boundary, no invented branch.
            sentinel=positionCompared_[lane]==zeroCompared_[lane]||
                (!(positionCompared_[lane]&0x7fffffffu)&&!(zeroCompared_[lane]&0x7fffffffu));
            positionEqualsZero_[lane]=sentinel;
        }
        auto& node=nodes_[static_cast<std::size_t>(model_->model_.sceneIndex)];
        if(sentinel){
            placementPC_=0x28b180;
            //Original getter transfers only localF0/F4/F8 raw words;50W
            //retains descriptor history even for unusual authored local bits.
            for(unsigned lane=0;lane<3;++lane)
                std::memcpy(&currentPosition50_[lane],&node.localC0.v[12+lane],4);
            base_->object_->fields.vector(0x50,currentPosition50_);
            base_->expectedFields_.vector(0x50,currentPosition50_);positionCopied_=true;
        }
        placementPC_=0x165d8c;resetEntered_=true;
        std::memcpy(resetInput_.data(),base_->zero433B10_->data(),16);
        std::memcpy(localBeforeReset_.data(),node.localC0.v,64);
        std::memcpy(worldBeforeReset_.data(),node.world100.v,64);
        SceneDisplacement49P zero;std::memcpy(zero.v,resetInput_.data(),16);
        if(!setLocalTranslation49P(graph(),model_->model_.sceneIndex,zero))
            return at(Status::bounded,placementPC_); //Prior Base50 copy remains committed.
        resetReturned_=true;
        for(std::uint32_t i=0;i<graphCount_;++i)flagsAfterReset_[i]=nodes_[i].sceneFlags190;
        placementReturned_=true;placementPC_=0x165d94;return at(Status::reached,placementPC_);
    }
    Result copySourceWorld28B550(std::array<std::uint32_t,16>& output,
        std::uint16_t& known)noexcept {
        // The exact completed cold64-byte28CB88 transfer established every
        // local word; placement only replacesXYZ with the same known Appzero.
        // Its dirty root forces this occurrence to refresh from those words.
        if(sourceWorld_.entered||!preflightNative()||!model_||modelRetired_||ledgerWithdrawn_||
           !placementReturned_||!resetReturned_||!rowsReturned_||!packetsReturned_||
           sourcePackets_.sourcePackets_.nativeWithdrawn||!loaderReturnedEntity_||
           model_->sourceRenderableEntity26E820Native()!=loaderReturnedEntity_||
           !(model_->sourceDirectRows_.readMask[0]&2u))return {Status::owner,0x28b550u};
        const auto index=static_cast<std::size_t>(model_->model_.sceneIndex);
        if(index>=graphCount_||(flagsAfterReset_[index]&1u)||
           (nodes_[index].sceneFlags190&1u))return {Status::bounded,0x28b550u};
        known=0;
        const auto result=model_->copySourceWorld28B550(graph(),output,sourceWorld_);
        // Keep165D94 history distinct from this later getter call.
        if(!result.okay())return {Status::bounded,result.sourcePC};
        known=0xffffu;return {Status::reached,result.sourcePC};
    }
    //A distinct E4 occurrence. The first E0 dirty refresh is immutable proof
    //of all16 world words; this call can only observe that unchanged clean root.
    Result copyDirectCollisionWorld28B550(const void* actualBody,const void* actualOccurrence,
        std::array<std::uint32_t,16>& output,std::uint16_t& known)noexcept {
        if(directWorldAttempted_||!actualBody||bodyModelBorrower_!=actualBody||!actualOccurrence||
           !preflightNative()||!model_||modelRetired_||ledgerWithdrawn_||nativeWithdrawn_||
           !placementReturned_||!resetReturned_||!rowsReturned_||!packetsReturned_||
           sourcePackets_.sourcePackets_.nativeWithdrawn||!loaderReturnedEntity_||
           model_->sourceRenderableEntity26E820Native()!=loaderReturnedEntity_||
           !sourceWorld_.returned||!sourceWorld_.worldReady||!sourceWorld_.outputStored||
           sourceWorld_.outputWordMask!=0xffffu||!sourceWorld_.refreshAttempted||
           sourceWorld_.refreshWordMask!=0xffffu||!model_->sourceWorldHistoryCurrentNative(sourceWorld_)||
           !(model_->sourceDirectRows_.readMask[0]&2u))return {Status::owner,0x28b550u};
        const auto root=static_cast<std::size_t>(model_->model_.sceneIndex);
        if(root>=graphCount_||nodes_[root].parent180!=SceneNodeIndex49P::none||!(nodes_[root].sceneFlags190&1u)||
           nodes_[root].sceneFlags190!=sourceWorld_.flags190After||
           std::memcmp(nodes_[root].world100.v,sourceWorld_.world.data(),64)||
           std::memcmp(nodes_[root].localC0.v,sourceWorld_.localBefore.data(),64))return {Status::bounded,0x28b550u};
        directWorldAttempted_=true;directWorldOccurrence_=actualOccurrence;directWorldBody_=actualBody;
        known=0;
        const auto result=model_->copySourceWorld28B550(graph(),output,directCollisionWorld_);
        directWorldPC_=result.sourcePC;
        if(!result.okay())return {Status::bounded,result.sourcePC};
        directWorldReturned_=true;known=0xffffu;return {Status::reached,result.sourcePC};
    }
    bool directCollisionWorldHistoryNative()const noexcept {
        const auto& h=directCollisionWorld_;
        if(!directWorldAttempted_)return !directWorldOccurrence_&&!directWorldBody_&&!directWorldReturned_&&
            !directWorldPC_&&!h.entered;
        if(!directWorldOccurrence_||!directWorldBody_||!sourceWorld_.returned||
           sourceWorld_.outputWordMask!=0xffffu||!sourceWorld_.refreshAttempted||
           sourceWorld_.refreshWordMask!=0xffffu||!model_||
           !model_->sourceWorldHistoryCurrentNative(h))return false;
        if(!h.entered)return !directWorldReturned_&&directWorldPC_==0x28b550u;
        if(!directWorldReturned_||!h.returned||directWorldPC_!=0x28b67cu||
           h.owner!=sourceWorld_.owner||h.canonicalNodes!=sourceWorld_.canonicalNodes||
           h.graphCount!=sourceWorld_.graphCount||h.sceneIndex!=sourceWorld_.sceneIndex||
           h.localBefore!=sourceWorld_.localBefore||h.worldBefore!=sourceWorld_.world||h.world!=sourceWorld_.world||
           h.flags190Before!=sourceWorld_.flags190After||h.flags190After!=sourceWorld_.flags190After||
           !(h.flags190Before&1u)||h.refreshAttempted||h.refreshWordMask||h.outputWordMask!=0xffffu)return false;
        return true; //No new graph mutation; placementHistory still pins every actual local/world/flag lane.
    }
    Scene::SourceTranslation28ADD0 translation_;
    const void* translationUpdateOccurrence_=nullptr;
    bool translationCallEntered_=false;
    std::uint32_t translationCallPC_=0;
    Result translateSourceNullRelative28ADD0(const void* actualUpdateOccurrence)noexcept {
        if(translationCallEntered_||!actualUpdateOccurrence||!nullObserver_.returned||
           nullObserver_.updateOccurrence!=actualUpdateOccurrence||nativeWithdrawn_||modelRetired_||ledgerWithdrawn_||
           detachedNative_||sourceSceneReleased_||sourceOuterDeleted_||!base_||!base_->object_||!model_||
           !bodyModelBorrower_||!packetsReturned_||sourcePackets_.sourcePackets_.nativeWithdrawn||
           !localBounds_.returned||!preflightNative()||
           base_->object_->model14C!=static_cast<const void*>(&model_->model_)||
           !model_->preflightSourceRenderableReturn26E820Native())return {Status::owner,0x28add0u};
        translationCallEntered_=true;translationUpdateOccurrence_=actualUpdateOccurrence;
        //Exact actual Base50 and knownness; source reads occur AFTER the guard.
        const auto r=model_->translateSourceNullRelative28ADD0(graph(),
            base_->object_->fields.words.data()+0x50/4,base_->object_->fields.known.data()+0x50/4,translation_);
        translationCallPC_=r.sourcePC;
        return {r.okay()?Status::reached:Status::bounded,r.sourcePC};
    }
    bool translationHistoryNative()const noexcept {
        if(!translationCallEntered_)return !translationUpdateOccurrence_&&!translationCallPC_&&!translation_.entered;
        if(!translationUpdateOccurrence_||!nullObserver_.returned||
           translationUpdateOccurrence_!=nullObserver_.updateOccurrence||!localBounds_.returned||!model_)return false;
        if(!translation_.entered)return translationCallPC_==0x28add0u;
        return translationCallPC_==translation_.pc&&model_->sourceTranslationHistoryNative(graph(),translation_,
            orientation_.entered?&orientation_:nullptr,postPoseWorld_.entered?&postPoseWorld_:nullptr,modelObserverBounds_.entered?&modelObserverBounds_:nullptr);
    }
    //Separate166710 occurrence: successful1666F0 translation remains immutable
    //history. No post-pose11FE50 world/shape getter is represented here.
    Scene::SourceOrientation28A090 orientation_;
    const void* orientationUpdateOccurrence_=nullptr;
    const Base* orientationBaseIdentity_=nullptr;
    const void* orientationModelIdentity_=nullptr;
    bool orientationCallEntered_=false;
    std::uint32_t orientationCallPC_=0;
    Result orientSourceNullRelative28A090(const void* actualUpdateOccurrence)noexcept {
        if(orientationCallEntered_||!actualUpdateOccurrence||!translationCallEntered_||
           !translation_.returned||translationCallPC_!=0x28af90u||
           translationUpdateOccurrence_!=actualUpdateOccurrence||!nullObserver_.returned||
           nullObserver_.updateOccurrence!=actualUpdateOccurrence||nativeWithdrawn_||modelRetired_||ledgerWithdrawn_||
           detachedNative_||sourceSceneReleased_||sourceOuterDeleted_||!base_||!base_->object_||!model_||
           !bodyModelBorrower_||!packetsReturned_||sourcePackets_.sourcePackets_.nativeWithdrawn||
           !localBounds_.returned||!preflightNative()||
           base_->object_->model14C!=static_cast<const void*>(&model_->model_)||
           !model_->preflightSourceRenderableReturn26E820Native())return {Status::owner,0x28a090u};
        orientationCallEntered_=true;orientationUpdateOccurrence_=actualUpdateOccurrence;
        orientationBaseIdentity_=base_;orientationModelIdentity_=&model_->model_;
        //1666FC/166700 supply actual Base60 look and Base70 up, not prepared
        //Wreck/reset vectors. The Scene service owns guard-before-read effects
        //and the exact t0=0,t1=1,t2=0 independent normalize/up-cross-look path.
        const auto& fields=base_->object_->fields;
        const auto r=model_->orientSourceNullRelative28A090(graph(),
            fields.words.data()+0x60/4,fields.known.data()+0x60/4,
            fields.words.data()+0x70/4,fields.known.data()+0x70/4,orientation_);
        orientationCallPC_=r.sourcePC;
        return {r.okay()?Status::reached:Status::bounded,r.sourcePC};
    }
    bool orientationHistoryNative()const noexcept {
        if(!orientationCallEntered_)return !orientationUpdateOccurrence_&&!orientationBaseIdentity_&&
            !orientationModelIdentity_&&!orientationCallPC_&&!orientation_.entered;
        if(!orientationUpdateOccurrence_||!base_||!model_||orientationBaseIdentity_!=base_||
           orientationModelIdentity_!=&model_->model_||!translationCallEntered_||!translation_.returned||
           translationCallPC_!=0x28af90u||orientationUpdateOccurrence_!=translationUpdateOccurrence_||
           !nullObserver_.returned||orientationUpdateOccurrence_!=nullObserver_.updateOccurrence||
           !localBounds_.returned)return false;
        if(!orientation_.entered)return orientationCallPC_==0x28a090u;
        if(!base_->object_)return false;
        const auto& fields=base_->object_->fields;
        for(unsigned lane=0;lane<4;++lane){
            if((orientation_.lookKnown&(1u<<lane))&&
               (fields.known[0x60/4+lane]!=0xffffffffu||orientation_.look[lane]!=fields.words[0x60/4+lane]))return false;
            if((orientation_.upKnown&(1u<<lane))&&
               (fields.known[0x70/4+lane]!=0xffffffffu||orientation_.up[lane]!=fields.words[0x70/4+lane]))return false;
        }
        return orientationCallPC_==orientation_.pc&&model_->sourceOrientationHistoryNative(graph(),orientation_,
            postPoseWorld_.entered?&postPoseWorld_:nullptr,modelObserverBounds_.entered?&modelObserverBounds_:nullptr);
    }
    Scene::SourcePostPoseWorld28B550 postPoseWorld_;
    const void* postPoseWorldUpdateOccurrence_=nullptr;
    const void* postPoseWorldCollisionOccurrence_=nullptr;
    const void* postPoseWorldBodyIdentity_=nullptr; //Opaque retained Body occurrence; never dereferenced.
    bool postPoseWorldCallEntered_=false;
    std::uint32_t postPoseWorldCallPC_=0;
    Result copySourcePostPoseWorld28B550(const void* actualUpdateOccurrence,
        const void* actualCollisionOccurrence,std::array<std::uint32_t,16>& output,
        std::uint16_t& known)noexcept {
        if(postPoseWorldCallEntered_||!actualUpdateOccurrence||!actualCollisionOccurrence||
           !orientationCallEntered_||!orientation_.returned||orientationCallPC_!=0x28a528u||
           orientationUpdateOccurrence_!=actualUpdateOccurrence||!directWorldReturned_||
           directWorldOccurrence_!=actualCollisionOccurrence||directWorldBody_!=bodyModelBorrower_||
           !bodyModelBorrower_||nativeWithdrawn_||modelRetired_||ledgerWithdrawn_||detachedNative_||
           sourceSceneReleased_||sourceOuterDeleted_||!base_||!base_->object_||!model_||
           !packetsReturned_||sourcePackets_.sourcePackets_.nativeWithdrawn||!preflightNative()||
           base_->sourceInverseExtent118_.occurrence!=bodyModelBorrower_||
           !base_->sourceInverseExtent118_.value||base_->sourceInverseExtent118_.nativeWithdrawn||
           base_->bodyBorrower_!=bodyModelBorrower_||model_->sourcePhysicsBodyBorrower_!=bodyModelBorrower_||
           base_->object_->bodyD0NativeWithdrawn||
           base_->object_->model14C!=static_cast<const void*>(&model_->model_)||
           !model_->preflightSourceRenderableReturn26E820Native())return {Status::owner,0x28b550u};
        //Collision owner installs its exact update lease before this callback.
        //This is the E4 occurrence only: its historical init borrower identity
        //is not interchangeable with E0, or with a caller-supplied ready matrix.
        postPoseWorldCallEntered_=true;postPoseWorldUpdateOccurrence_=actualUpdateOccurrence;
        postPoseWorldCollisionOccurrence_=actualCollisionOccurrence;postPoseWorldBodyIdentity_=bodyModelBorrower_;known=0;
        const auto r=model_->copySourcePostPoseWorld28B550(graph(),output,postPoseWorld_);
        postPoseWorldCallPC_=r.sourcePC;
        if(r.okay())known=postPoseWorld_.getter.outputWordMask;
        return {r.okay()?Status::reached:Status::bounded,r.sourcePC};
    }
    bool postPoseWorldHistoryNative()const noexcept {
        if(!postPoseWorldCallEntered_)return !postPoseWorldUpdateOccurrence_&&!postPoseWorldCollisionOccurrence_&&
            !postPoseWorldBodyIdentity_&&!postPoseWorldCallPC_&&!postPoseWorld_.entered;
        if(!base_||!base_->object_||!model_||!postPoseWorldBodyIdentity_||
           !postPoseWorldUpdateOccurrence_||!postPoseWorldCollisionOccurrence_||
           !orientationCallEntered_||!orientation_.returned||orientationCallPC_!=0x28a528u||
           postPoseWorldUpdateOccurrence_!=orientationUpdateOccurrence_||!directWorldReturned_||
           postPoseWorldCollisionOccurrence_!=directWorldOccurrence_||directWorldBody_!=postPoseWorldBodyIdentity_)return false;
        //Body withdrawal clears all live borrowers, but retains the exact
        //Base16652C occurrence tombstone. It cannot erase this getter's
        //historical Body identity or make a different/null lease sufficient.
        const auto& extent=base_->sourceInverseExtent118_;
        if(extent.occurrence!=postPoseWorldBodyIdentity_||!extent.value)return false;
        if(extent.nativeWithdrawn){
            if(bodyModelBorrower_||base_->bodyBorrower_||model_->sourcePhysicsBodyBorrower_||
               !base_->object_->bodyD0NativeWithdrawn||!base_->object_->bodyD0||*base_->object_->bodyD0)return false;
        }else if(bodyModelBorrower_!=postPoseWorldBodyIdentity_||base_->bodyBorrower_!=postPoseWorldBodyIdentity_||
                 model_->sourcePhysicsBodyBorrower_!=postPoseWorldBodyIdentity_||base_->object_->bodyD0NativeWithdrawn)return false;
        if(!postPoseWorld_.entered)return postPoseWorldCallPC_==0x28b550u;
        return postPoseWorldCallPC_==postPoseWorld_.pc&&
            model_->sourcePostPoseWorldHistoryNative(graph(),postPoseWorld_,
                modelObserverBounds_.entered?&modelObserverBounds_:nullptr);
    }
    //166720 E0 requests its own28B550 after the E4 getter and shape pass.
    //The caller/COL owns the intervening shape-return proof; this getter
    //requires the exact still-current E4 graph and copies a clean root only.
    Scene::SourceWorld28B550 postE4World_;
    const void* postE4WorldUpdateOccurrence_=nullptr;
    const void* postE4WorldCollisionOccurrence_=nullptr;
    const void* postE4WorldCollisionOuterIdentity_=nullptr; //Historical outer, distinct from occurrence owner.
    bool postE4WorldCallEntered_=false;
    std::uint32_t postE4WorldCallPC_=0;
    Result copySourcePostE4World28B550(const void* actualUpdateOccurrence,
        const void* actualCollisionOccurrence,std::array<std::uint32_t,16>& output,
        std::uint16_t& known)noexcept {
        if(postE4WorldCallEntered_||!actualUpdateOccurrence||!actualCollisionOccurrence||
           !postPoseWorldCallEntered_||!postPoseWorld_.returned||postPoseWorldCallPC_!=0x28b67cu||
           postPoseWorldUpdateOccurrence_!=actualUpdateOccurrence||
           postPoseWorldCollisionOccurrence_==actualCollisionOccurrence||
           nativeWithdrawn_||modelRetired_||ledgerWithdrawn_||detachedNative_||sourceSceneReleased_||
           sourceOuterDeleted_||!base_||!base_->object_||!model_||!bodyModelBorrower_||
           bodyModelBorrower_!=postPoseWorldBodyIdentity_||
           base_->sourceInverseExtent118_.nativeWithdrawn||base_->object_->bodyD0NativeWithdrawn||
           base_->collisionBorrower_!=actualCollisionOccurrence||!base_->object_->collisionE0||
           !*base_->object_->collisionE0||base_->object_->collisionE0NativeWithdrawn||
           !packetsReturned_||sourcePackets_.sourcePackets_.nativeWithdrawn||!preflightNative()||
           base_->object_->model14C!=static_cast<const void*>(&model_->model_)||
           !model_->preflightSourceRenderableReturn26E820Native())return {Status::owner,0x28b550u};
        const auto root=static_cast<std::uint32_t>(model_->model_.sceneIndex);
        if(root>=graphCount_||nodes_[root].parent180!=SceneNodeIndex49P::none||
           !(nodes_[root].sceneFlags190&1u))return {Status::owner,0x28b550u};
        //Full E4 currentness above pins local/world/flags and every child.
        //The new occurrence must issue the raw getter, not copy old output
        //scratch or reset bit0 to manufacture a second world refresh.
        postE4WorldCallEntered_=true;postE4WorldUpdateOccurrence_=actualUpdateOccurrence;
        postE4WorldCollisionOccurrence_=actualCollisionOccurrence;
        postE4WorldCollisionOuterIdentity_=*base_->object_->collisionE0;known=0;
        const auto r=model_->copySourceWorld28B550(graph(),output,postE4World_);
        postE4WorldCallPC_=r.sourcePC;
        if(r.okay())known=postE4World_.outputWordMask;
        return {r.okay()?Status::reached:Status::bounded,r.sourcePC};
    }
    bool postE4WorldHistoryNative()const noexcept {
        const auto& h=postE4World_;
        if(!postE4WorldCallEntered_)return !postE4WorldUpdateOccurrence_&&!postE4WorldCollisionOccurrence_&&
            !postE4WorldCollisionOuterIdentity_&&!postE4WorldCallPC_&&!h.entered;
        if(!base_||!base_->object_||!model_||!postE4WorldUpdateOccurrence_||!postE4WorldCollisionOccurrence_||
           !postE4WorldCollisionOuterIdentity_||
           !postPoseWorldCallEntered_||!postPoseWorld_.returned||postPoseWorldCallPC_!=0x28b67cu||
           postE4WorldUpdateOccurrence_!=postPoseWorldUpdateOccurrence_||
           postE4WorldCollisionOccurrence_==postPoseWorldCollisionOccurrence_||!postPoseWorldHistoryNative())return false;
        //Keep the opaque E0 identity after native cancellation. Only the
        //actual BaseE0 withdrawal tombstone permits the cleared live lease.
        if(base_->object_->collisionE0NativeWithdrawn){
            if(base_->collisionBorrower_||!base_->object_->collisionE0||*base_->object_->collisionE0)return false;
        }else if(base_->collisionBorrower_!=postE4WorldCollisionOccurrence_||
                 !base_->object_->collisionE0||*base_->object_->collisionE0!=postE4WorldCollisionOuterIdentity_)return false;
        if(!h.entered)return postE4WorldCallPC_==0x28b550u;
        const auto& prior=postPoseWorld_.getter;
        return model_->sourceWorldHistoryCurrentNative(h)&&h.returned&&postE4WorldCallPC_==0x28b67cu&&
            h.owner==prior.owner&&h.canonicalNodes==prior.canonicalNodes&&h.graphCount==prior.graphCount&&
            h.sceneIndex==prior.sceneIndex&&h.localBefore==prior.localBefore&&h.worldBefore==prior.world&&
            h.world==prior.world&&h.flags190Before==prior.flags190After&&h.flags190After==prior.flags190After&&
            (h.flags190Before&1u)&&!h.refreshAttempted&&!h.refreshWordMask&&h.outputWordMask==0xffffu;
    }
    Scene::SourceLocalBounds28D670 localBounds_;
    Result produceSourceLocalBounds28D670(V& actualCenter,V& actualHalf)noexcept {
        if(localBounds_.entered||&actualCenter==&actualHalf||nativeWithdrawn_||modelRetired_||ledgerWithdrawn_||
           !model_||!base_||!base_->object_||!bodyModelBorrower_||!placementReturned_||!resetReturned_||
           !packetsReturned_||sourcePackets_.sourcePackets_.nativeWithdrawn||!preflightNative()||
           !sourceWorld_.returned||!sourceWorld_.worldReady||sourceWorld_.outputWordMask!=0xffffu||
           !model_->sourceWorldHistoryCurrentNative(sourceWorld_)||
           base_->object_->model14C!=static_cast<const void*>(&model_->model_)||
           !model_->preflightSourceRenderableReturn26E820Native())return {Status::owner,0x28d670u};
        const auto result=model_->produceSourceLocalBounds28D670(graph(),actualCenter,actualHalf,localBounds_);
        return {result.okay()?Status::reached:Status::bounded,result.sourcePC};
        //Conditional direct callee only. Actual166400/+74 predecessor belongs to root.
    }
    bool localBoundsHistoryNative()const noexcept {
        if(!localBounds_.entered)return !localBounds_.owner&&!localBounds_.returned&&!localBounds_.pc;
        if(!model_||!placementReturned_||!resetReturned_||!sourceWorld_.returned||
           !model_->sourceLocalBoundsHistoryNative(graph(),localBounds_,translation_.entered?&translation_:nullptr,
               orientation_.entered?&orientation_:nullptr,postPoseWorld_.entered?&postPoseWorld_:nullptr,modelObserverBounds_.entered?&modelObserverBounds_:nullptr))return false;
        const auto& h=localBounds_;const auto root=static_cast<std::uint32_t>(model_->model_.sceneIndex);
        if(h.root!=root||h.worldWritten[root]||h.before[root].sceneFlags190!=sourceWorld_.flags190After||
           std::memcmp(h.before[root].world100.v,sourceWorld_.world.data(),64))return false;
        for(std::uint32_t i=0;i<graphCount_;++i)
            if(h.before[i].sceneFlags190!=(i==root?sourceWorld_.flags190After:flagsAfterReset_[i]))return false;
        return true;
    }
    bool placementHistoryNative()const noexcept {
        if(!postE4WorldHistoryNative()||!postPoseWorldHistoryNative()||!orientationHistoryNative()||!translationHistoryNative()||!directCollisionWorldHistoryNative()||!localBoundsHistoryNative())return false;
        if(!placementEntered_)return !sourceWorld_.entered&&!placementReturned_&&!positionCopied_&&!resetEntered_&&!resetReturned_&&
            !placementPC_&&!comparisonReadMask_;
        if(!attachReturned_||!model_||!base_||!base_->object_||pc_!=placementPC_||
           (comparisonReadMask_!=1u&&comparisonReadMask_!=3u&&comparisonReadMask_!=7u))return false;
        bool all=true;unsigned reached=0;
        for(unsigned lane=0;lane<3;++lane){
            const bool read=(comparisonReadMask_&(1u<<lane))!=0;
            if(read){if(!all)return false;++reached;
                if(positionCompared_[lane]!=position_[lane])return false;
                if(!positionEqualsZero_[lane]){all=false;if(positionCopied_||resetEntered_||placementReturned_)return false;}
                else {
                    const auto magnitude=[](std::uint32_t bits){return bits&0x7fffffffu;};
                    const auto supported=[](std::uint32_t bits){const auto e=bits&0x7f800000u;
                        return !(bits&0x7fffffffu)||(e&&e!=0x7f800000u);};
                    if(!supported(positionCompared_[lane])||!supported(zeroCompared_[lane]))return false;
                    const bool equal=positionCompared_[lane]==zeroCompared_[lane]||
                        (!magnitude(positionCompared_[lane])&&!magnitude(zeroCompared_[lane]));
                    if(*positionEqualsZero_[lane]!=equal)return false;all=equal;
                }
            }else if(positionEqualsZero_[lane])return false;
        }
        if(positionCopied_!=(reached==3&&all))return false;
        if(positionCopied_&&currentPosition50_[3]!=position_[3])return false;
        if(resetReturned_&&!resetEntered_)return false;
        if(!resetEntered_&&positionEqualsZero_[reached-1])return false;
        if(placementReturned_!=resetReturned_||(placementReturned_&&placementPC_!=0x165d94u))return false;
        if(resetEntered_){
            if(!positionEqualsZero_[reached-1]||(!all&&*positionEqualsZero_[reached-1]))return false;
            const auto rootIndex=static_cast<std::size_t>(model_->model_.sceneIndex);
            //Earlier placement/getters remain immutable history; the exact
            //new translation proof alone authenticates current changed lanes.
            const auto& node=translation_.entered?translation_.before[rootIndex]:nodes_[rootIndex];
            std::array<std::uint32_t,16> local{},world{};
            std::memcpy(local.data(),node.localC0.v,64);std::memcpy(world.data(),node.world100.v,64);
            if(sourceWorld_.entered){
                const auto& h=sourceWorld_;
                const auto root=static_cast<std::size_t>(model_->model_.sceneIndex);
                if(!placementReturned_||!resetReturned_||!(model_->sourceDirectRows_.readMask[0]&2u)||
                   !model_->sourceWorldHistoryCurrentNative(h)||h.canonicalNodes!=nodes_.data()||
                   h.graphCount!=graphCount_||h.sceneIndex!=root||h.worldBefore!=worldBeforeReset_||
                   h.localBefore!=local||h.flags190Before!=flagsAfterReset_[root]||
                   (h.flags190Before&1u)||world!=(h.worldReady?h.world:worldBeforeReset_))return false;
            }else if(world!=worldBeforeReset_)return false;
            for(unsigned lane=0;lane<16;++lane){
                const auto expected=resetReturned_&&lane>=12&&lane<15?resetInput_[lane-12]:localBeforeReset_[lane];
                if(local[lane]!=expected)return false;
            }
            if(positionCopied_)for(unsigned lane=0;lane<3;++lane)
                if(currentPosition50_[lane]!=localBeforeReset_[12+lane])return false;
            if(resetReturned_)for(std::uint32_t i=0;i<graphCount_;++i)
                if((translation_.entered?translation_.before[i].sceneFlags190:nodes_[i].sceneFlags190)!=(localBounds_.entered?localBounds_.after[i].sceneFlags190:
                    sourceWorld_.entered&&i==static_cast<std::uint32_t>(model_->model_.sceneIndex)?
                    sourceWorld_.flags190After:flagsAfterReset_[i]))return false;
        }
        return true;
    }
    template<class ActualDetach>
    bool detachSourceModelNative(ActualDetach&& detach)noexcept {
        if(!entered_||nativeWithdrawn_||!modelAttached_||detachedNative_)return true;
        if((modelObserver_.entered&&!modelObserver_.nativeWithdrawn)||bodyModelBorrower_||!base_||!base_->application_||
           base_->application_->attachmentLock423F58()||!preflightNative())return false;
        const auto result=detach(*model_->wreckModelStorage(),0u);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        base_->nestedChild141960_=nullptr;detachedNative_=true;return true;
    }
    bool withdrawSourcePacketsNative()noexcept {
        if(!entered_||nativeWithdrawn_||!sourcePackets_.sourcePackets_.entered||
           sourcePackets_.sourcePackets_.nativeWithdrawn)return true;
        if((modelObserver_.entered&&!modelObserver_.nativeWithdrawn)||bodyModelBorrower_||(modelAttached_&&!detachedNative_)||
           (attachEntered_&&base_&&base_->application_->attachmentLock423F58())||!preflightNative())return false;
        //Root MUST detach the exact Entity link, then call before member association/file native withdrawal.
        //Live packet preflight borrows actual returned Model/resource; the
        //withdrawn history remains valid after later resource retirement.
        return sourcePackets_.withdrawSourcePacketsNative();
    }
    template<class RetireActualModelLedger>
    Result releaseSourceMissing26E9D4(
        std::shared_ptr<rr::assets::OriginalSourcePsgMissingLookup50LA> missing,
        const rr::assets::OriginalStandardTextureResolver50LA* actualResolver,
        const rr::assets::OriginalRaTrackSourceFile50LA* actualTrack,
        RetireActualModelLedger&& retire)noexcept {
        if(bodyModelBorrower_||missingEntered_||rowsEntered_||!nameReturned_||pc_!=0x26e9ac||!preflightNative(true)||
           !model_||!constructorReturned_||!published_||ledgerWithdrawn_||modelRetired_||
           !missing||!missing->liveMissingNative()||missing->requestedName()!=requestedPsg_||
           missing->resolver()!=actualResolver||missing->root()!=actualTrack||
           !actualResolver||!actualTrack)return {Status::owner,0x26e9b4};
        auto& scene=*model_;auto* entity=scene.wreckModelStorage();
        if(entity->reference00!=1||entity->parent1C||entity->firstChild18||entity->previous10||entity->next14||
           scene.binding_.sourceRowCount||scene.fields_.sourceRows1B8||scene.fields_.materials1C4||
           scene.fields_.geometry1B4||scene.fields_.packets1F0||scene.model_.observer19C||
           scene.model_.configuration1A4)return {Status::owner,0x26e9cc};
        //Root authenticates SAME Application/track/resolver and its actual
        //ledger callback. A provider miss or injected bool cannot enter here.
        missing_=std::move(missing);missingEntered_=true;
        pc_=0x26e9c4;missingLogAttempted_=true;
        missingLogResult_=std::fprintf(stdout,"RenderableObject '%s' not found\n",requestedPsg_.c_str());
        //Original ignores diagnostic result; no native output failure is
        //converted into a different source lookup/release branch.
        pc_=0x1425a8;sourceReleaseLock_=base_->application_->attachmentLock423F58();
        if(!*sourceReleaseLock_){
            if(!scene.preflightScene2896D0(graph()).okay())return at(Status::bounded,0x1425b8);
            //Native membership leaves just before the real last-reference drop.
            //A refused callback retains source ref/count/14C and gives no V0zero.
            pc_=0x142654;if(!retire(*entity))return at(Status::bounded,pc_);
            ledgerWithdrawn_=true;
            const auto dropped=scene.releasePreflightedModelReference142590();
            if(!dropped.okay())return at(Status::bounded,dropped.sourcePC);
            sourceReferenceDropped_=true;
            pc_=0x240480;destructorVtable1A8_=0x3d1f40;destructorStoreMask_|=1u;
            pc_=0x24048c;destructorVtable10_=0x3d1f48;destructorStoreMask_|=2u;
            pc_=0x240494;destructorNullSkips_|=1u; //No geometry/palette walk.
            pc_=0x24050c;destructorNullSkips_|=2u; //No packetfree(NULL).
            pc_=0x26e48c;destructorVtable1A8_=0x3d2390;destructorStoreMask_|=4u;
            pc_=0x26e498;destructorVtable10_=0x3d2398;destructorStoreMask_|=8u;
            pc_=0x26e4a0;destructorNullSkips_|=4u; //No material/resource/cache disposal.
            pc_=0x26e5d0;destructorNullSkips_|=8u; //No rowarray free.
            pc_=0x26e5e4;destructorNullSkips_|=16u; //No materialarray free.
            pc_=0x2896f4;destructorVtable1A8_=0x3d3190;destructorStoreMask_|=16u;
            pc_=0x289700;destructorVtable10_=0x3d3198;destructorStoreMask_|=32u;
            pc_=0x289708;destructorNullSkips_|=32u; //No source scene parent.
            pc_=0x28971c;destructorNullSkips_|=64u; //No source scene child.
            pc_=0x289764;destructorNullSkips_|=128u; //No source observer.
            pc_=0x141404;destructorVtable10_=0x3cd240;destructorStoreMask_|=64u;
            pc_=0x141420;const auto released=scene.releaseSceneBase2896D0(graph());
            if(!released.okay())return at(Status::bounded,released.sourcePC);
            //14141C calls104910(a1=0): branch delay still rewrites the
            //embeddedEntity dispatch even though this base does not free itself.
            pc_=0x10493c;destructorVtable10_=0x3cc980;destructorStoreMask_|=128u;
            sourceSceneReleased_=true;
            pc_=0x240538;sourceOuterDeleted_=true; //Logical source death; native tombstone remains.
        }
        //Both genuine142590 lock-return0 and104970 last-drop return0 reach
        //26E9E0,26EEE8,240768/2407D8.165C30 ignores that actual calleeV0.
        sourceReleaseReturned_=true;sourceLoaderReturnV0_=0;
        return at(Status::reached,0x165cdc);
    }
    C* countedModelEntityNative()const noexcept {
        return model_&&model_->sourceConstructor_.entityCounted&&!modelRetired_&&!sourceSceneReleased_?
            model_->wreckModelStorage():nullptr;
    }
    #include "original_source_physics_model_observer50la.inl"
    struct SourceNullObserver28EA80 {
        const void* updateOccurrence=nullptr;
        const void* ownerIdentity=nullptr;
        bool entered=false,observedNull=false,returned=false;
        std::uint32_t pc=0;
    } nullObserver_;
    Result removeSourceNullObserver28EA80(const void* actualUpdateOccurrence)noexcept {
        if(!actualUpdateOccurrence||nullObserver_.entered||nativeWithdrawn_||modelRetired_||ledgerWithdrawn_||
           sourceSceneReleased_||sourceOuterDeleted_||detachedNative_||!model_||!base_||!base_->object_||
           !bodyModelBorrower_||!placementReturned_||!localBounds_.returned||!packetsReturned_||
           sourcePackets_.sourcePackets_.nativeWithdrawn||!preflightNative()||
           base_->object_->model14C!=static_cast<const void*>(&model_->model_)||
           !model_->preflightSourceRenderableReturn26E820Native())return {Status::owner,0x28ea80u};
        auto& h=nullObserver_;h.updateOccurrence=actualUpdateOccurrence;
        h.ownerIdentity=&model_->model_;h.entered=true;h.pc=0x28ea90u;
        //Actual ModelBASE19C load; Entity+8 and COL membership are distinct identities.
        if(model_->model_.observer19C)return {Status::bounded,h.pc=0x28ea9cu};
        h.observedNull=true;h.returned=true;h.pc=0x28eac4u;
        return {Status::reached,h.pc}; //JR28EABC/delay28EAC0, no literal V0.
    }
    bool nullObserverHistoryNative()const noexcept {
        const auto& h=nullObserver_;
        if(!h.entered)return !h.updateOccurrence&&!h.ownerIdentity&&!h.observedNull&&!h.returned&&!h.pc;
        if(!h.updateOccurrence||!h.ownerIdentity)return false;
        if(h.returned?(!h.observedNull||h.pc!=0x28eac4u):(h.observedNull||h.pc!=0x28ea9cu))return false;
        if(nativeWithdrawn_)return true; //No access to an erased scene or caller occurrence.
        return model_&&h.ownerIdentity==&model_->model_&&
            (h.observedNull?model_->model_.observer19C==sourceModelObserverCurrentNative():!!model_->model_.observer19C);
    }
    bool preflightNative(bool sourceReleaseAdmission=false)const noexcept {
        if(!sourceModelObserverHistoryNative()||!nullObserverHistoryNative())return false;
        if(!entered_)return !base_&&!model_&&!nativeWithdrawn_;
        if(nativeWithdrawn_)return !base_&&!model_&&!missing_&&!rowsFile_&&!rowsResolver_&&!bodyModelBorrower_&&!materialTable_;
        if(!base_||base_->nativeWithdrawn_||base_->nestedModelBorrower_!=this||
           !base_->preflightNative()||!requestedName_)return false;
        if(!base_->object_)return !model_&&!vectorStoreMask_&&!waterStored_&&
            !allocationAttempted_&&!published_&&pc_==0x165c88;
        const auto& o=*base_->object_;
        if(vectorStoreMask_!=7u||!waterStored_||!o.waterKnown||o.waterB0)return false;
        const auto same=[&](unsigned offset,const V& value){
            for(unsigned i=0;i<4;++i)if(o.fields.known[offset/4+i]!=0xffffffffu||
                o.fields.words[offset/4+i]!=value[i])return false;return true;
        };
        if(!same(0x50,positionCopied_?currentPosition50_:position_)||!same(0x60,look_)||!same(0x70,up_))return false;
        if(allocationAttempted_!=allocationReturned_||(!allocationAttempted_&&model_))return false;
        if(model_){
            const auto& s=model_->sourceConstructor_;
            if(noOpReturned_!=s.noOp141C20Returned||modelNameStored_!=model_->directRootPrepared_)return false;
            //Actual row0 name read can replace source26E934 name. Historical
            //requestedPsg_ still authenticates file identity, not current row0.
            const bool rowNameRead=model_->sourceDirectRows_.attempted&&
                (model_->sourceDirectRows_.readMask[0]&1u);
            if(!rowNameRead&&(modelNameStored_?model_->model_.name38!=requestedPsg_:!model_->model_.name38.empty()))return false;
            if(s.attempted&&(s.canonicalNodes!=nodes_.data()||
               model_->binding_.sessionGeneration!=base_->generation_||
               model_->binding_.actualObjectCount423F60!=&base_->application_->objectCount423F60()||
               model_->binding_.actualAttachmentLock423F58!=&base_->application_->attachmentLock423F58()||
               model_->binding_.actualTypes383408!=&base_->application_->types383408()))return false;
            if(s.entityCounted){
                if(modelRetired_||sourceSceneReleased_){
                    if(!model_->released_||!model_->model_.released||!model_->referenceReleased_||
                       model_->wreckModelStorage()->reference00)return false;
                }else {
                    const auto current=model_->preflightNativePartialScene2896D0(graph(),sourceModelObserverCurrentNative());
                    if(!current.okay()&&!(current.status==OriginalRacerModelSceneStatus50LA::attachmentLocked&&
                        (sourceReleaseAdmission||(sourceReleaseLock_&&*sourceReleaseLock_)||
                         (attachReturned_&&attachLock_&&*attachLock_&&
                          base_->application_->attachmentLock423F58()==*attachLock_))))return false;
                }
            }else if(model_->model_.constructed||modelRetired_)return false;
            if(constructorReturned_!=s.modelReturned)return false;
            if(model_->sourceWarmGeometry_.attempted)
                return false; //Warm continuation remains unissued; packet facet proved separately.
        }else if(constructorReturned_||modelRetired_||ledgerWithdrawn_||sourceSceneReleased_)return false;
        if(published_){if(!allocationReturned_||(model_&&!constructorReturned_)||!o.modelKnown||
            o.model14C!=(model_?static_cast<const void*>(&model_->model_):nullptr))return false;}
        else if(o.model14C)return false;
        if(nameEntered_){
            if(!published_)return false;
            if(!model_)return pc_==0x165ccc&&!noOpReturned_&&!directoryIdentity_&&!directoryRead_;
            if(noOpReturned_&&(!directoryIdentity_||!directoryRead_||sourceStackCount680_!=0))return false;
            if(lowercaseStores_>lowercaseInputSize_||
               (lowercaseReturned_&&!nameCopied_)||(extensionStored_&&!lowercaseReturned_)||
               (modelNameStored_&&!extensionStored_))return false;
            if(nameReturned_&&(!modelNameStored_||sourceHandle678_!=-1||sourceWarmFlag684_!=0||(!missingEntered_&&!rowsEntered_&&pc_!=0x26e9ac)))return false;
        }else if(noOpReturned_||directoryIdentity_||directoryRead_||modelNameStored_||nameReturned_)return false;
        if(!rowsHistoryNative()||!materialHistoryNative()||!geometryHistoryNative()||!rowTableHistoryNative()||!packetsHistoryNative()||!attachmentHistoryNative()||!placementHistoryNative())return false;
        if(missingEntered_){
            if(!nameReturned_||!missing_||!missing_->preflightNative()||
               !missing_->returnedMissing132820()||missing_->requestedName()!=requestedPsg_||
               !missingLogAttempted_||!missingLogResult_||!sourceReleaseLock_)return false;
            if(*sourceReleaseLock_&&(sourceReferenceDropped_||sourceSceneReleased_||sourceOuterDeleted_||
                destructorStoreMask_||destructorNullSkips_))return false;
            if(sourceReferenceDropped_&&(!ledgerWithdrawn_||!model_->referenceReleased_))return false;
            if(sourceSceneReleased_&&(!sourceReferenceDropped_||destructorStoreMask_!=255u||
               destructorNullSkips_!=255u||destructorVtable1A8_!=0x3d3190||destructorVtable10_!=0x3cc980))return false;
            if(sourceOuterDeleted_&&!sourceSceneReleased_)return false;
            if(sourceReleaseReturned_&&(!sourceLoaderReturnV0_||*sourceLoaderReturnV0_||
               (!*sourceReleaseLock_&&!sourceOuterDeleted_)||pc_!=0x165cdc))return false;
            if(sourceLoaderReturnV0_&&!sourceReleaseReturned_)return false;
        }else if(missing_||missingLogAttempted_||sourceReferenceDropped_||sourceSceneReleased_||
            sourceOuterDeleted_||sourceReleaseReturned_||sourceReleaseLock_||sourceLoaderReturnV0_)return false;
        return true;
    }
    template<class RetireActualModelLedger>
    bool withdrawNative(RetireActualModelLedger&& retire)noexcept {
        if(!entered_||nativeWithdrawn_)return true;
        if(bodyModelBorrower_||(modelAttached_&&!detachedNative_)||!preflightNative()||!withdrawSourcePacketsNative())return false;
        if(model_&&materialEntered_){
            //Preflight entire source forest AND partial resource custody before
            //any release. Reader may now be native-closed; no new IO is issued.
            if(!model_->preflightNativePartialScene2896D0(graph()).okay()||
               !model_->preflightSourceMaterialsNative()||!model_->preflightSourceGeometryNative()||
               !model_->preflightSourceRowTableNative())return false;
            if(!model_->releaseSourceMaterialsNative())return false;
            if(!model_->releaseSourceGeometryNative())return false;
            if(!model_->withdrawSourceRowTableNative())return false;
        }
        if(auto* entity=countedModelEntityNative()){
            const auto before=sourceReferenceDropped_?model_->preflightScene2896D0(graph()):
                model_->preflightFreshUnlinkedNative(graph());
            if(!before.okay())return false;
            //Native membership must leave for every counted Scene child,
            //including allocated-but-not-published registry-refusal prefixes.
            //Retry skips successful callbacks but does not drop source ref/count
            //until the whole preflighted Scene forest is ready for retirement.
            for(std::uint32_t row=1;row<rr::assets::maxModelNodes49O;++row){
                auto* child=countedChildEntityNative(row);if(!child)continue;
                if(!childLedgerWithdrawn_[row]){
                    if(!retire(*child))return false;childLedgerWithdrawn_[row]=true;
                }
            }
            if(!ledgerWithdrawn_){if(!retire(*entity))return false;ledgerWithdrawn_=true;}
            const auto closed=sourceReferenceDropped_?model_->releaseSceneBase2896D0(graph()):
                model_->closeFreshUnlinked(graph());
            if(!closed.okay())return false;modelRetired_=true;
        }
        if(missing_){rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
            if(!missing_->closeNative(diagnostic))return false;}
        //Native alias withdrawal before freeing the actual nested Model.
        //No original loaderzero/165BD0 or source14C=NULL is claimed.
        if(base_->object_&&published_)base_->object_->model14C=nullptr;
        model_.reset();missing_.reset();rowsFile_.reset();rowsResolver_.reset();materialTable_.reset();base_->nestedModelBorrower_=nullptr;base_=nullptr;
        nativeWithdrawn_=true;return true;
    }
public:
    OriginalSourcePhysicsModel50LA()=default;
    OriginalSourcePhysicsModel50LA(const OriginalSourcePhysicsModel50LA&)=delete;
    OriginalSourcePhysicsModel50LA& operator=(const OriginalSourcePhysicsModel50LA&)=delete;
    ~OriginalSourcePhysicsModel50LA(){if(entered_&&!nativeWithdrawn_)std::terminate();}
};
}
