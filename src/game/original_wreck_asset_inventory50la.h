#pragma once
#include "original_wreck_assets50la.h"
#include "../assets/original_standard_material_domain50la.h"

namespace rr::game {
// The opaque endpoint words belong to the actual source owner, not the COL.
// Include BOTH authored ordinals: repeated stems/PartN ordinals across distinct
// collections are legal source occurrences and must never be deduplicated.
struct OriginalWreckInventoryEndpointPolicy50LA {
    void* context=nullptr;
    bool (*words)(void*,const OriginalWreckCollectionAuthoring50LA&,
        const OriginalWreckPieceAuthoring50LA&,std::size_t,Type2OpaqueEndpointWords49X&)=nullptr;
};
struct OriginalWreckPieceInventory50LA {
    std::size_t collectionOrdinal=0,pieceOrdinal=0; // one-based source PartN
    std::string sltRequest,psgMember,queryMember,targetMember;
    std::array<rr::assets::ResourceEntry48C,3> members{}; // PSG,m.COL,f.COL
    std::array<bool,3> uniqueMemberFound{};
};
struct OriginalWreckCollectionInventory50LA {
    std::size_t ordinal=0;
    std::vector<OriginalWreckPieceInventory50LA> pieces;
};
enum class OriginalWreckInventoryStatus50LA : std::uint8_t {
    inventoried,pieceLoaded,complete,binding,archive,authoring,allocation,piece,modelSourcesBound
};
struct OriginalWreckInventoryResult50LA {
    OriginalWreckInventoryStatus50LA status=OriginalWreckInventoryStatus50LA::binding;
    std::size_t collectionOrdinal=0,pieceOrdinal=0,loadedPieces=0;
};
// Cold immutable source inventory plus stable, separately mutable asset graphs.
// Keeps actual collection/PartN order from2129C0..212AAC and22BFD0. It does not
// construct Entities, bodies, groups, effects, registration, or racer374 views.
// Native loading is strictly sequential. Unsupported original bytes stop at
// the exact occurrence; earlier genuine assets and full inventory stay retained.
class OriginalWreckAssetInventory50LA final {
    using Assets=OriginalWreckAssets50LA;
    struct Storage {
        OriginalWreckAssetBinding50LA binding;
        rr::assets::ResArchive48C archive;
        std::vector<std::uint8_t> phy;
        rr::assets::ResourceEntry48C phyEntry;
        std::size_t archiveBytes=0,loaded=0,nextCollection=0,nextPiece=0,retainedPieceBytes=0;
        OriginalWreckAuthoring50LA authoring;
        std::vector<OriginalWreckCollectionInventory50LA> inventory;
        // Outlives every per-occurrence file identity and future geometry borrow.
        std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> modelResolver;
        std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> modelMaterials;
        std::vector<std::vector<std::unique_ptr<Assets>>> assets;
        bool modelBindingAttempted=false,modelSourcesBound=false;
    };
    std::unique_ptr<Storage> storage_;
    struct Endpoints {
        const OriginalWreckInventoryEndpointPolicy50LA* policy=nullptr;
        const OriginalWreckCollectionAuthoring50LA* collection=nullptr;
        static bool read(void* context,const OriginalWreckPieceAuthoring50LA& piece,
                         std::size_t ray,Type2OpaqueEndpointWords49X& out) {
            auto& self=*static_cast<Endpoints*>(context);
            return self.policy->words(self.policy->context,*self.collection,piece,ray,out);
        }
    };
    static bool identity(const rr::assets::ResArchive48C& archive,const std::string& name,
                         rr::assets::ResourceEntry48C& out) {
        const rr::assets::ResourceEntry48C* found=nullptr;
        for(const auto& e:archive.entries)if(Assets::lower(e.name)==Assets::lower(name)){
            if(found)return false;found=&e;
        }
        if(!found)return false;out=*found;return true;
    }
    OriginalWreckInventoryResult50LA result(OriginalWreckInventoryStatus50LA status)const noexcept {
        if(!storage_)return {status,0,0,0};
        const auto& s=*storage_;
        return {status,s.nextCollection<s.inventory.size()?s.nextCollection+1u:0u,
            s.nextCollection<s.inventory.size()?s.nextPiece+1u:0u,s.loaded};
    }
public:
    OriginalWreckAssetInventory50LA()=default;
    OriginalWreckAssetInventory50LA(const OriginalWreckAssetInventory50LA&)=delete;
    OriginalWreckAssetInventory50LA& operator=(const OriginalWreckAssetInventory50LA&)=delete;
    OriginalWreckAssetInventory50LA(OriginalWreckAssetInventory50LA&&)=delete;
    OriginalWreckAssetInventory50LA& operator=(OriginalWreckAssetInventory50LA&&)=delete;

    // retainedRes/retainedPhy must be the actual same authenticated racer
    // resources. PHY is compared byte-for-byte with its member in this RES.
    // No path is reopened, alternate cache selected, or format guessed here.
    // The shared constructor globals remain borrowed for later source loading.
    OriginalWreckInventoryResult50LA inventoryRetained(
        const OriginalWreckAssetBinding50LA& binding,const std::vector<std::uint8_t>& retainedRes,
        const std::vector<std::uint8_t>& retainedPhy,std::string& error) {
        using S=OriginalWreckInventoryStatus50LA;
        if(storage_||!binding.constructorVectors||binding.archivePath.empty()||
           binding.memberPrefix.empty()||binding.phyMember.empty()||retainedRes.empty()||
           retainedPhy.empty()||std::fegetround()!=FE_TONEAREST){
            error="invalid fresh retained original wreck inventory binding";return result(S::binding);
        }
        try {
            auto next=std::make_unique<Storage>();next->binding=binding;
            rr::assets::DecodeError48C why;
            if(!rr::assets::decodeResource48C(retainedRes,next->archive,why)){
                error="retained original wreck RES rejected: "+why.code;return result(S::archive);
            }
            next->archiveBytes=retainedRes.size();
            if(!Assets::member(next->archive,binding.phyMember,next->phy,next->phyEntry,error))return result(S::archive);
            if(next->phy!=retainedPhy){error="retained original wreck PHY differs from same RES";return result(S::binding);}
            Assets::Document doc;
            if(!Assets::document(next->phy,doc,error)||!Assets::author(doc,next->authoring,error,false)||
               !Assets::podIdentity(binding,doc,error))return result(S::authoring);
            // Original full authored graph need not have a unique one-piece
            // chariot selection. The legacy one-piece prepare remains strict.
            next->inventory.resize(next->authoring.collections.size());
            next->assets.resize(next->authoring.collections.size());
            for(std::size_t c=0;c<next->inventory.size();++c){
                const auto& authored=next->authoring.collections[c];auto& collection=next->inventory[c];
                collection.ordinal=authored.ordinal;collection.pieces.reserve(authored.pieces.size());
                next->assets[c].resize(authored.pieces.size());
                for(const auto& piece:authored.pieces){
                    OriginalWreckPieceInventory50LA p;p.collectionOrdinal=authored.ordinal;p.pieceOrdinal=piece.ordinal;
                    p.sltRequest=binding.memberPrefix+piece.stem+".slt";
                    p.psgMember=binding.memberPrefix+piece.stem+".psg";
                    p.queryMember=binding.memberPrefix+piece.stem+"m.col";
                    p.targetMember=binding.memberPrefix+piece.stem+"f.col";
                    p.uniqueMemberFound[0]=identity(next->archive,p.psgMember,p.members[0]);
                    p.uniqueMemberFound[1]=identity(next->archive,p.queryMember,p.members[1]);
                    p.uniqueMemberFound[2]=identity(next->archive,p.targetMember,p.members[2]);
                    collection.pieces.push_back(std::move(p));
                }
            }
            storage_=std::move(next);error.clear();return result(S::inventoried);
        }catch(const std::bad_alloc&){error="original wreck inventory cold allocation failed";return result(S::allocation);}
    }
    // Same application-owned constructor vectors used while loading every
    // retained PSG occurrence. This is a borrow, not a synthesized default.
    const OriginalWreckConstructorVectors50LA* constructorVectors()const noexcept {
        return storage_?storage_->binding.constructorVectors:nullptr;
    }
    bool inventoried()const noexcept{return bool(storage_);}
    bool complete()const noexcept{return storage_&&storage_->nextCollection==storage_->inventory.size();}
    const OriginalWreckAuthoring50LA& authoring()const noexcept{return storage_->authoring;}
    const std::vector<OriginalWreckCollectionInventory50LA>& collections()const noexcept{return storage_->inventory;}
    // Zero-based native indexes, retaining the one-based authored ordinals in
    // each record. A null return is an unloaded occurrence, never an empty mesh.
    Assets* pieceAssets(std::size_t collection,std::size_t piece)noexcept {
        return storage_&&collection<storage_->assets.size()&&piece<storage_->assets[collection].size()?
            storage_->assets[collection][piece].get():nullptr;
    }
    const Assets* pieceAssets(std::size_t collection,std::size_t piece)const noexcept {
        return storage_&&collection<storage_->assets.size()&&piece<storage_->assets[collection].size()?
            storage_->assets[collection][piece].get():nullptr;
    }
    // Publish exact authenticated PSG and target f.COL file rows into the same application
    //3E1A70 resolver used by materials/textures. COL additionally requires the
    //same sealed mounted provider to resolve those exact bytes. No geometry association,
    //104960 retain, material acquisition or240740 packet is manufactured.
    // A failed serial prefix remains retained and cannot be rebound/replayed.
    OriginalWreckInventoryResult50LA bindModelSources(
        rr::assets::OriginalStandardMaterialDomain50LA& domain,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        using S=OriginalWreckInventoryStatus50LA;diagnostic={};
        if(!complete()||storage_->modelBindingAttempted||!domain.prepared()||!domain.resolver3E1A70())
            return {S::binding,0,0,0};
        storage_->modelBindingAttempted=true;storage_->modelResolver=domain.resolver3E1A70();
        storage_->modelMaterials=domain.materials382B38();
        std::size_t bound=0;
        for(std::size_t c=0;c<storage_->assets.size();++c)
            for(std::size_t p=0;p<storage_->assets[c].size();++p){
                auto& asset=*storage_->assets[c][p];auto& payload=asset.storage_->standardModel;
                if(!payload||!payload->prepared()||!payload->member()||!payload->originalBytes()||
                   !storage_->modelResolver->bindModelMember(*payload->member(),
                        rr::assets::OriginalStandardModelFileKind50LA::psg,*payload->originalBytes(),
                        asset.storage_->modelFile,diagnostic))return {S::piece,c+1u,p+1u,bound};
                if(!storage_->modelResolver->bindModelMember(asset.storage_->receipt.targetMember,
                    rr::assets::OriginalStandardModelFileKind50LA::colType0,asset.storage_->raw[3],
                    asset.storage_->targetFile,diagnostic))return {S::piece,c+1u,p+1u,bound};
                ++bound;
            }
        storage_->modelSourcesBound=true;return {S::modelSourcesBound,0,0,bound};
    }
    bool modelSourcesBound()const noexcept{return storage_&&storage_->modelSourcesBound;}
    const rr::assets::OriginalStandardTextureResolver50LA* modelResolverIdentity()const noexcept {
        return storage_?storage_->modelResolver.get():nullptr;
    }
    const rr::assets::OriginalStandardMaterialTable50LA* modelMaterialsIdentity()const noexcept {
        return storage_?storage_->modelMaterials.get():nullptr;
    }
    OriginalWreckInventoryResult50LA loadNextCold(const OriginalWreckInventoryEndpointPolicy50LA& endpoint,
                                                std::string& error) {
        using S=OriginalWreckInventoryStatus50LA;
        if(!storage_||!endpoint.words||std::fegetround()!=FE_TONEAREST){
            error="original wreck ordered load binding absent";return result(S::binding);
        }
        if(complete()){error.clear();return result(S::complete);}
        auto& s=*storage_;const auto c=s.nextCollection,p=s.nextPiece;
        // Explicit native cold-storage admission, counting each occurrence's
        // actual source bytes (including PHY). Never deduplicate mutable pieces.
        const auto ceiling=rr::assets::DecodeLimits48C{}.resource_decoded_bytes;
        std::size_t bytes=s.phy.size();
        for(unsigned i=0;i<3;++i){const auto& entry=s.inventory[c].pieces[p];
            if(entry.uniqueMemberFound[i]){
                if(entry.members[i].bytes>ceiling||bytes>ceiling-entry.members[i].bytes){
                    error="original wreck occurrence source-byte capacity exceeded";return result(S::allocation);
                }
                bytes+=entry.members[i].bytes;
            }
        }
        if(bytes>ceiling||s.retainedPieceBytes>ceiling-bytes){
            error="original wreck aggregate source-byte capacity exceeded";return result(S::allocation);
        }
        try {
            auto owner=std::make_unique<Assets>();auto data=std::make_unique<Assets::Storage>();
            data->authoring=s.authoring;data->authoring.selectedCollection=c;data->selectedPieceIndex=p;
            data->raw[0]=s.phy;data->receipt.archiveBytes=s.archiveBytes;
            data->receipt.archivePath=s.binding.archivePath;data->receipt.phyMember=s.binding.phyMember;
            data->receipt.members[0]=s.phyEntry;
            Endpoints bridge{&endpoint,&data->authoring.collections[c]};
            OriginalWreckEndpointPolicy50LA policy{&bridge,&Endpoints::read};
            if(!Assets::loadSelectedPiece(s.archive,s.binding,policy,*data,error)){
                error="collection "+std::to_string(c+1u)+" piece "+std::to_string(p+1u)+": "+error;
                return result(S::piece);
            }
            owner->storage_=std::move(data);s.assets[c][p]=std::move(owner);++s.loaded;s.retainedPieceBytes+=bytes;
            if(++s.nextPiece==s.inventory[c].pieces.size()){s.nextPiece=0;++s.nextCollection;}
            error.clear();return {complete()?S::complete:S::pieceLoaded,c+1u,p+1u,s.loaded};
        }catch(const std::bad_alloc&){error="original wreck piece cold allocation failed";return result(S::allocation);}
    }
};
} // namespace rr::game
