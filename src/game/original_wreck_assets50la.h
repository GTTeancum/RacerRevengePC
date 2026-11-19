#pragma once
#include "../assets/dlc_files50dl.h"
#include "../assets/dlc_registry50dl.h"
#include "original_wreck_constructor_vectors50la.h"
// Cold native geometry/query assets for one actual authored piece. Legacy
// prepare selects the unique one-piece chariot; the full inventory friend loads
// each original occurrence. Neither route constructs runtime wreck owners.
#include "type2_resource49x.h"
#include "pod_shape49w.h"
#include "scene_graph49p.h"
#include "model_bounds49o.h"
#include "../assets/import_decode48c.h"
#include "../assets/import_collision48g.h"
#include "../assets/import_scene48d.h"
#include "../assets/original_standard_model_resource50la.h"
#include "../assets/import_world48f.h"
#include "../assets/model_geometry49o.h"
#include "../assets/pod_physics49n.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iterator>
#include <initializer_list>
#include <limits>
#include <map>
#include <memory>
#include <new>
#include <string>
#include <utility>
#include <vector>

namespace rr::game {
struct OriginalWreckTuning50LA {
    float maxCompression=0,spring=0,damping=0,impulseSpring=0,impulseDamping=0;
    float restitution=0,friction=0,frictionTorqueScale=0,drag=0,rateDamping=0;
};
struct OriginalWreckPieceAuthoring50LA {
    std::string stem;
    float weight=0,inertia=0,breakRate=0,breakScale=0;
    bool emitSmoke=false;
    std::size_t ordinal=0; // Original PartN, one-based, never an asset ordinal.
};
struct OriginalWreckCollectionAuthoring50LA {
    std::string section;
    std::size_t ordinal=0;
    std::uint32_t maxNumberOfDamageEmitters=0;
    OriginalWreckTuning50LA tuning;
    std::vector<OriginalWreckPieceAuthoring50LA> pieces;
    std::uint32_t sourceNumberOfParts=0; //Exact retained PHY count, not live array state.
};
struct OriginalWreckAuthoring50LA {
    std::vector<OriginalWreckCollectionAuthoring50LA> collections;
    std::size_t selectedCollection=0; // Zero-based selection; inventory uses count if no unique chariot.
    std::string chariotStem;
    float gravity=0,gravityAcceleration=0;
    std::uint32_t sourceNumberOfParts=0; //Exact [Wreckage]158920 input.
};
// Actual22C148/22C158/22C194 constructor globals and165CF8 zero sentinel.
// The caller supplies their initialized source state; absence is not identity.
struct OriginalWreckAssetBinding50LA {
    std::string archivePath;
    std::string memberPrefix; // Exact archive prefix, e.g. Pods\\Pod02\\.
    std::string phyMember;    // Exact original member in that same archive.
    const OriginalWreckConstructorVectors50LA* constructorVectors=nullptr;
};
struct OriginalWreckEndpointPolicy50LA {
    void* context=nullptr;
    // COL omits endpoint W. The owning caller explicitly supplies those words
    // in actual ray order; no file-derived, zero, or vehicle-body substitute.
    bool (*words)(void*,const OriginalWreckPieceAuthoring50LA&,std::size_t,
                  Type2OpaqueEndpointWords49X&)=nullptr;
};
struct OriginalWreckTarget50LA {
    rr::assets::CollisionMesh48I mesh;
    std::vector<float> radii;
    std::size_t maxDepth=0;
    PodShapeSource49W source{};
};
struct OriginalWreckAssetReceipt50LA {
    std::string archivePath,phyMember,sltRequest,psgMember,queryMember,targetMember;
    std::array<rr::assets::ResourceEntry48C,4> members{}; // PHY, PSG, m.COL, f.COL.
    std::size_t archiveBytes=0;
    std::uint32_t sourceModelNodeCount=0;
    bool endpointWordsCallerSupplied=false;
};
class OriginalWreckAssetInventory50LA;
class OriginalWreckAssets50LA final {
    friend class OriginalWreckAssetInventory50LA;
    friend class OriginalWreckRacerStorage50LA;
public:
    OriginalWreckAssets50LA()=default;
    OriginalWreckAssets50LA(const OriginalWreckAssets50LA&)=delete;
    OriginalWreckAssets50LA& operator=(const OriginalWreckAssets50LA&)=delete;
    OriginalWreckAssets50LA(OriginalWreckAssets50LA&&)=delete;
    OriginalWreckAssets50LA& operator=(OriginalWreckAssets50LA&&)=delete;
    // Fresh setup only, transactional publication. Decoder functions run only
    // when a future authorized native caller invokes prepare, never at static
    // initialization. Allocations and file I/O are confined to this cold path.
    bool prepare(const OriginalWreckAssetBinding50LA& binding,
                 const OriginalWreckEndpointPolicy50LA& endpoint,std::string& error) {
        if(storage_||!endpoint.words||!binding.constructorVectors||
           binding.archivePath.empty()||binding.memberPrefix.empty()||binding.phyMember.empty()||
           std::fegetround()!=FE_TONEAREST)
            return fail(error,"invalid fresh original wreck asset binding");
        try {
            auto next=std::make_unique<Storage>();
            if(!load(binding,endpoint,*next,error))return false;
            storage_=std::move(next);error.clear();return true;
        } catch(const std::bad_alloc&) {return fail(error,"original wreck cold allocation failed");}
    }
    bool prepared()const noexcept{return bool(storage_);}
    // Accessors require prepared(). Views remain stable for this owner's life.
    const OriginalWreckAuthoring50LA& authoring()const noexcept{return storage_->authoring;}
    const OriginalWreckCollectionAuthoring50LA& selected()const noexcept {
        return authoring().collections[authoring().selectedCollection];
    }
    const OriginalWreckPieceAuthoring50LA& selectedPiece()const noexcept {
        return selected().pieces[storage_->selectedPieceIndex];
    }
    const Type2ResourceView49X& queryE0()const noexcept{return storage_->query.view;}
    const OriginalWreckTarget50LA& targetE4()const noexcept{return storage_->target;}
    SceneGraph49P graph()noexcept{return {storage_->nodes.data(),static_cast<std::uint32_t>(storage_->nodes.size())};}
    const SceneNode49P* modelGraphIdentity()const noexcept{return storage_->nodes.data();}
    std::uint32_t modelGraphNodeCount()const noexcept{return static_cast<std::uint32_t>(storage_->nodes.size());}
    SceneNodeIndex49P model14C()const noexcept{return static_cast<SceneNodeIndex49P>(0);}
    const ModelBoundsResult49O& modelBounds()const noexcept{return storage_->bounds;}
    // Actual26F4B0 box-cache provider, separate from166400's28D670 inertia
    // query. Uses the actual flat1B8 source list INCLUDING model row0 itself.
    // No allocation. A failed call may retain completed source graph refresh;
    // cached160/170/198 and root40 publish only after finite numeric completion.
    bool refreshCachedModelBounds()noexcept {
        return storage_&&cacheBounds(*storage_);
    }
    bool cachedModelBoundsReady()const noexcept{return storage_&&storage_->cachedBoundsReady;}
    // Requires cachedModelBoundsReady(): ctor does not author radius198.
    const ModelBoundsResult49O& cachedModelBounds()const noexcept{return storage_->cachedBounds;}
    const std::array<float,4>& authored50()const noexcept{return storage_->position50;}
    const std::array<float,4>& look60()const noexcept{return storage_->look60;}
    const std::array<float,4>& up70()const noexcept{return storage_->up70;}
    const rr::assets::Scene48D& scene()const noexcept{return *storage_->standardModel->scene();}
    // Immutable authentic PSG payload used by the real Standard geometry
    // owner. This payload is not a source240110 reference or live model.
    const std::shared_ptr<const rr::assets::OriginalStandardModelResource50LA>& standardModelPayload()const noexcept {
        return storage_->standardModel;
    }
    rr::assets::OriginalStandardModelFileId50LA modelFileIdentity()const noexcept{return storage_->modelFile;}
    rr::assets::OriginalStandardModelFileId50LA targetFileIdentity()const noexcept{return storage_->targetFile;}
    rr::assets::OriginalStandardModelFileId50LA queryFileIdentity()const noexcept{return storage_->queryFile;}
    const rr::assets::PsgHierarchyView49O& hierarchy()const noexcept{return storage_->hierarchy;}
    const OriginalWreckAssetReceipt50LA& receipt()const noexcept{return storage_->receipt;}
    const std::array<std::vector<std::uint8_t>,4>& originalMembers()const noexcept{return storage_->raw;}
private:
    using Section=std::map<std::string,std::string>;
    using Document=std::map<std::string,Section>;
    struct Query {
        std::vector<Type2Node49X> nodes;
        std::vector<Type2Ray49X> rays;
        Type2ResourceView49X view{};
    };
    struct Storage {
        OriginalWreckAuthoring50LA authoring;
        std::size_t selectedPieceIndex=0;
        OriginalWreckAssetReceipt50LA receipt;
        std::array<std::vector<std::uint8_t>,4> raw;
        Query query;
        OriginalWreckTarget50LA target;
        rr::assets::PsgHierarchyView49O hierarchy{};
        std::shared_ptr<const rr::assets::OriginalStandardModelResource50LA> standardModel;
        rr::assets::OriginalStandardModelFileId50LA modelFile{},targetFile{},queryFile{};
        std::vector<SceneNode49P> nodes;
        std::vector<rr::assets::PsgNode49O> sourceNodes;
        ModelBoundsResult49O bounds{};
        ModelBoundsResult49O cachedBounds{};
        bool cachedBoundsReady=false,canonicalGraphBacking=false;
        std::array<float,4> position50{},look60{},up70{};
    };
    static bool fail(std::string& error,const char* why){error=why;return false;}
    static std::string lower(std::string value) {
        for(char& c:value){if(c>='A'&&c<='Z')c=static_cast<char>(c-'A'+'a');if(c=='/')c='\\';}
        return value;
    }
    static std::string trim(std::string value) {
        const auto first=value.find_first_not_of(" \t\r\n");
        if(first==std::string::npos)return {};
        return value.substr(first,value.find_last_not_of(" \t\r\n")-first+1);
    }
    static bool simpleStem(const std::string& value)noexcept {
        if(value.empty()||value.size()>63)return false;
        for(unsigned char c:value)if(!((c>='A'&&c<='Z')||(c>='a'&&c<='z')||
            (c>='0'&&c<='9')||c=='_'))return false;
        return true;
    }
    static const std::string* value(const Document& doc,const std::string& section,const std::string& key) {
        const auto s=doc.find(lower(section));if(s==doc.end())return nullptr;
        const auto k=s->second.find(lower(key));return k==s->second.end()?nullptr:&k->second;
    }
    static bool number(const Document& doc,const std::string& section,const std::string& key,float& out) {
        const auto* p=value(doc,section,key);
        return p&&rr::assets::parsePodPhysicsNumber49N(p->data(),p->size(),out);
    }
    static bool count(const Document& doc,const std::string& section,const std::string& key,
                      std::uint32_t& out,std::uint32_t maximum) {
        const auto* p=value(doc,section,key);if(!p||p->empty())return false;
        std::uint32_t n=0;for(char c:*p){if(c<'0'||c>'9'||n>maximum/10||
            static_cast<unsigned>(c-'0')>maximum-n*10)return false;n=n*10+static_cast<unsigned>(c-'0');}
        out=n;return true;
    }
    static bool textDocument(const std::vector<std::uint8_t>& bytes,Document& out,std::string& error) {
        if(bytes.empty()||bytes.size()>rr::assets::maxPodPhysicsBytes49N)
            return fail(error,"original wreck PHY text extent");
        for(unsigned char c:bytes)if((c<32&&c!='\t'&&c!='\r'&&c!='\n')||c>=127)
            return fail(error,"original wreck PHY text character");
        std::string section;std::size_t fields=0;
        const std::string text(bytes.begin(),bytes.end());
        for(std::size_t start=0;start<text.size();){
            auto end=text.find('\n',start);if(end==std::string::npos)end=text.size();
            if(end-start>rr::assets::maxPodPhysicsLine49N)return fail(error,"original wreck PHY line extent");
            const auto line=trim(text.substr(start,end-start));start=end+1;
            if(line.empty()||line.front()==';')continue;
            if(line.front()=='['){
                if(line.size()<3||line.back()!=']')return fail(error,"original wreck PHY section syntax");
                section=lower(trim(line.substr(1,line.size()-2)));
                if(section.empty()||out.size()>=rr::assets::maxPodPhysicsSections49N||
                   !out.emplace(section,Section{}).second)return fail(error,"original wreck PHY duplicate/empty section");
            }else{
                const auto eq=line.find('=');if(section.empty()||eq==std::string::npos)
                    return fail(error,"original wreck PHY field syntax");
                const auto key=lower(trim(line.substr(0,eq)));auto field=trim(line.substr(eq+1));
                if(field.size()>=2&&field.front()=='"'&&field.back()=='"')field=field.substr(1,field.size()-2);
                if(key.empty()||++fields>rr::assets::maxPodPhysicsKeys49N||
                   !out[section].emplace(key,field).second)return fail(error,"original wreck PHY duplicate/empty key");
            }
        }
        return true;
    }
    static bool document(const std::vector<std::uint8_t>& bytes,Document& out,std::string& error) {
        constexpr char binary[]="BINARY";
        if(bytes.size()<sizeof binary||std::memcmp(bytes.data(),binary,sizeof binary)!=0)
            return textDocument(bytes,out,error);
        rr::assets::ParameterDocument48F decoded;rr::assets::DecodeError48C why;
        if(!rr::assets::decodeParameters48F(bytes,decoded,why))return fail(error,"original wreck binary PHY rejected");
        if(decoded.blocks.size()>rr::assets::maxPodPhysicsSections49N)return fail(error,"original wreck PHY section limit");
        std::size_t count=0;
        for(const auto& block:decoded.blocks){
            Section fields;
            for(const auto& field:block.fields){
                if(++count>rr::assets::maxPodPhysicsKeys49N||!fields.emplace(lower(field.name),trim(field.value)).second)
                    return fail(error,"original wreck PHY duplicate/too many fields");
            }
            if(!out.emplace(lower(block.name),std::move(fields)).second)
                return fail(error,"original wreck PHY duplicate section");
        }
        return true;
    }
    static bool author(const Document& doc,OriginalWreckAuthoring50LA& out,std::string& error,
                       bool requireOnePieceChariot=true) {
        const auto* chariot=value(doc,"Pod Setup","Chariot");
        if(!chariot||chariot->size()<5||lower(chariot->substr(chariot->size()-4))!=".slt")
            return fail(error,"original PHY does not author a chariot SLT");
        out.chariotStem=chariot->substr(0,chariot->size()-4);
        if(!simpleStem(out.chariotStem))return fail(error,"original chariot reference is not a bounded local stem");
        std::uint32_t collectionCount=0;
        if(!count(doc,"Wreckage","NumberOfParts",collectionCount,512)||!collectionCount||
           !number(doc,"Wreckage","Gravity",out.gravity))return fail(error,"original Wreckage count/gravity missing");
        // Source215B7C..215BB0, not Physical Properties.Gravity.
        const std::uint32_t gravityWord=0x4200cccdu;float scale;std::memcpy(&scale,&gravityWord,4);
        volatile float acceleration=out.gravity*scale;out.gravityAcceleration=acceleration;
        if(!std::isfinite(out.gravityAcceleration))return fail(error,"original wreck gravity overflow");
        out.sourceNumberOfParts=collectionCount;
        out.collections.reserve(collectionCount);std::size_t allPieces=0,matches=0;
        for(std::uint32_t c=1;c<=collectionCount;++c){
            const auto* section=value(doc,"Wreckage","Part"+std::to_string(c));
            if(!section||!simpleStem(*section))return fail(error,"original Wreckage ordered PartN missing");
            OriginalWreckCollectionAuthoring50LA collection;collection.section=*section;collection.ordinal=c;
            std::uint32_t pieceCount=0;
            if(!count(doc,*section,"NumberOfParts",pieceCount,512)||!pieceCount||allPieces+pieceCount>512)
                return fail(error,"original wreck collection piece count unsupported");
            collection.sourceNumberOfParts=pieceCount;
            allPieces+=pieceCount;
            // The original22C05C default is4; it is not inferred from smoke flags.
            collection.maxNumberOfDamageEmitters=4;
            if(value(doc,*section,"MaxNumberOfDamageEmitters")&&
               !count(doc,*section,"MaxNumberOfDamageEmitters",collection.maxNumberOfDamageEmitters,512))
                return fail(error,"original wreck damage emitter count invalid");
            auto& t=collection.tuning;
            if(!number(doc,*section,"MaxCompression",t.maxCompression)||!number(doc,*section,"Spring",t.spring)||
               !number(doc,*section,"Damping",t.damping)||!number(doc,*section,"ImpulseSpring",t.impulseSpring)||
               !number(doc,*section,"ImpulseDamping",t.impulseDamping)||!number(doc,*section,"Restitution",t.restitution)||
               !number(doc,*section,"Friction",t.friction)||!number(doc,*section,"FrictionTorqueScale",t.frictionTorqueScale)||
               !number(doc,*section,"Drag",t.drag)||!number(doc,*section,"RateDamping",t.rateDamping))
                return fail(error,"original wreck collection tuning missing/invalid");
            collection.pieces.reserve(pieceCount);
            for(std::uint32_t p=1;p<=pieceCount;++p){
                const auto suffix=std::to_string(p);const auto* stem=value(doc,*section,"Part"+suffix);
                const auto* smoke=value(doc,*section,"EmitSmoke"+suffix);
                OriginalWreckPieceAuthoring50LA piece;piece.ordinal=p;
                if(!stem||!simpleStem(*stem)||!smoke||(lower(*smoke)!="t"&&lower(*smoke)!="f")||
                   !number(doc,*section,"Weight"+suffix,piece.weight)||
                   !number(doc,*section,"Inertia"+suffix,piece.inertia))
                    return fail(error,"original wreck ordered piece authoring missing/invalid");
                piece.stem=*stem;piece.emitSmoke=lower(*smoke)=="t";
                // Exact22C4A8/22C4C0 fallback values, each independently applied.
                piece.breakRate=piece.breakScale=1;
                if((value(doc,*section,"BreakRate"+suffix)&&!number(doc,*section,"BreakRate"+suffix,piece.breakRate))||
                   (value(doc,*section,"BreakScale"+suffix)&&!number(doc,*section,"BreakScale"+suffix,piece.breakScale)))
                    return fail(error,"original wreck break tuning invalid");
                collection.pieces.push_back(std::move(piece));
            }
            if(pieceCount==1&&lower(collection.pieces[0].stem)==lower(out.chariotStem)){
                out.selectedCollection=out.collections.size();++matches;
            }
            out.collections.push_back(std::move(collection));
        }
        if(matches!=1){
            out.selectedCollection=out.collections.size(); // no unique selected one-piece view
            if(requireOnePieceChariot)return fail(error,"PHY does not uniquely author a one-piece chariot wreck collection");
        }
        return true;
    }
    static std::uint32_t u32(const std::uint8_t* p)noexcept {
        return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|
            (std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);
    }
    struct BoundsMath {
        bool valid=true;
        float keep(float x)noexcept{valid=valid&&std::isfinite(x);return x;}
        float add(float a,float b)noexcept{keep(a);keep(b);volatile float x=a+b;return keep(x);}
        float sub(float a,float b)noexcept{keep(a);keep(b);volatile float x=a-b;return keep(x);}
        float mul(float a,float b)noexcept{keep(a);keep(b);volatile float x=a*b;return keep(x);}
        float root(float a)noexcept {
            keep(a);std::uint32_t bits;std::memcpy(&bits,&a,4);
            if((bits&0x7f800000u)==0){bits&=0x80000000u;float zero;std::memcpy(&zero,&bits,4);return zero;}
            volatile float x=static_cast<float>(std::sqrt(static_cast<double>(a)));return keep(x);
        }
        std::array<float,3> direction(const std::array<float,3>& p,const BodyMatrix48G& m)noexcept {
            std::array<float,3> out{};
            for(unsigned j=0;j<3;++j)out[j]=add(add(mul(m.v[j],p[0]),mul(m.v[4+j],p[1])),mul(m.v[8+j],p[2]));
            return out;
        }
        std::array<float,3> toRoot(std::array<float,3> p,const BodyMatrix48G& source,bool point)noexcept {
            //28DC68..28DC78's VADD +0 canonicalizes signed-zero root lanes
            // before the original transpose operation. This is not an inverse.
            BodyMatrix48G matrix=source;for(float& lane:matrix.v)lane=add(lane,0.f);
            if(point)for(unsigned j=0;j<3;++j)p[j]=sub(p[j],matrix.v[12+j]);
            std::array<float,3> out{};
            for(unsigned j=0;j<3;++j)out[j]=add(add(mul(matrix.v[4*j],p[0]),mul(matrix.v[4*j+1],p[1])),mul(matrix.v[4*j+2],p[2]));
            return out;
        }
    };
    static std::int32_t signedWord(float value)noexcept {
        std::int32_t out;std::memcpy(&out,&value,4);return out;
    }
    static float boundMin(float a,float b)noexcept {
        const auto x=signedWord(a),y=signedWord(b);return x<0&&y<0?(x>y?a:b):(x<y?a:b);
    }
    static float boundMax(float a,float b)noexcept {
        const auto x=signedWord(a),y=signedWord(b);return x<0&&y<0?(x<y?a:b):(x>y?a:b);
    }
    static void mergeBounds(BoundsMath& math,ModelBoundsResult49O& out,
        const std::array<float,3>& center,const std::array<float,3>& half)noexcept {
        for(unsigned j=0;j<3;++j){
            const float lo=math.sub(center[j],half[j]),hi=math.add(center[j],half[j]);
            out.minimum[j]=out.seeded?boundMin(out.minimum[j],lo):lo;
            out.maximum[j]=out.seeded?boundMax(out.maximum[j],hi):hi;
        }
        out.seeded=true;
    }
    static bool cacheBounds(Storage& out)noexcept {
        if(std::fegetround()!=FE_TONEAREST||out.nodes.empty()||out.nodes.size()!=out.sourceNodes.size()||
           (out.nodes[0].sceneFlags190&0x10u))return false; // Sphere route remains pending.
        const SceneGraph49P graph{out.nodes.data(),static_cast<std::uint32_t>(out.nodes.size())};
        if(!validateSceneGraph49P(graph)||!refreshSceneWorld49P(graph,static_cast<SceneNodeIndex49P>(0)))return false;
        const BodyMatrix48G rootWorld=out.nodes[0].world100;
        BoundsMath math;ModelBoundsResult49O result{};
        result.minimum[3]=result.maximum[3]=result.center160[3]=result.halfExtent170[3]=1.f;
        //26F638..26F6C8 seeds directly from model14C's own authored bound.
        if(out.nodes[0].sceneFlags190&4u){
            const auto& b=out.sourceNodes[0].bounds;
            mergeBounds(math,result,{{b[0],b[1],b[2]}},{{b[3],b[4],b[5]}});
        }
        //26F6D0..26F710 then28DAF0(recursive=false): source1B8 contains
        // every loaded source row, including row0 which is model14C itself.
        // Consume actual cached graph worlds, without a synthetic parent/local
        // multiplication for that alias and without dropping its contribution.
        for(std::size_t i=0;i<out.nodes.size();++i){
            if(!(out.nodes[i].sceneFlags190&4u))continue;
            if(!refreshSceneWorld49P(graph,static_cast<SceneNodeIndex49P>(i)))return false;
            const auto& source=out.sourceNodes[i];const auto& world=out.nodes[i].world100;
            std::array<float,3> center{{source.bounds[0],source.bounds[1],source.bounds[2]}};
            center=math.direction(center,world);
            for(unsigned j=0;j<3;++j)center[j]=math.add(center[j],world.v[12+j]);
            center=math.toRoot(center,rootWorld,true);
            std::array<float,3> axes[3]{},half{};
            for(unsigned axis=0;axis<3;++axis){
                std::array<float,3> v{};v[axis]=source.bounds[3+axis];
                auto forward=math.direction(v,world);
                //28E0FC adds vf0 after the direction MAC; retain its +0
                // signed-zero canonicalization before the root transpose.
                for(float& lane:forward)lane=math.add(lane,0.f);
                axes[axis]=math.toRoot(forward,rootWorld,false);
            }
            for(unsigned j=0;j<3;++j)half[j]=math.add(std::fabs(axes[2][j]),math.add(std::fabs(axes[0][j]),std::fabs(axes[1][j])));
            mergeBounds(math,result,center,half);
        }
        for(unsigned j=0;j<3;++j){
            result.center160[j]=math.mul(.5f,math.add(result.minimum[j],result.maximum[j]));
            result.halfExtent170[j]=math.mul(.5f,math.sub(result.maximum[j],result.minimum[j]));
        }
        const float x=math.mul(result.halfExtent170[0],result.halfExtent170[0]);
        const float y=math.mul(result.halfExtent170[1],result.halfExtent170[1]);
        const float xy=math.add(x,y);
        result.radius198=math.root(math.add(xy,math.mul(result.halfExtent170[2],result.halfExtent170[2])));
        if(!math.valid)return false;
        result.rootSceneFlags190=static_cast<std::uint8_t>(out.nodes[0].sceneFlags190|(result.radius198>0.f?0x40u:0u));
        out.cachedBounds=result;out.cachedBoundsReady=true;
        out.nodes[0].sceneFlags190=result.rootSceneFlags190;return true;
    }
    static bool member(const rr::assets::ResArchive48C& archive,const std::string& name,
                       std::vector<std::uint8_t>& out,rr::assets::ResourceEntry48C& identity,
                       std::string& error) {
        const rr::assets::ResourceEntry48C* found=nullptr;
        for(const auto& entry:archive.entries)if(lower(entry.name)==lower(name)){
            if(found)return fail(error,"ambiguous original wreck RES member identity");found=&entry;
        }
        if(!found)return fail(error,"required original wreck RES member unavailable");
        rr::assets::DecodeError48C why;
        if(!rr::assets::extractResMember48C(archive,found->name,out,why))
            return fail(error,"original wreck RES member extraction rejected");
        identity=*found;return true;
    }
    static bool decodeQuery(const std::vector<std::uint8_t>& bytes,
        const OriginalWreckPieceAuthoring50LA& piece,const OriginalWreckEndpointPolicy50LA& policy,
        Query& out,std::string& error) {
        if(bytes.size()<92)return fail(error,"short original wreck type2 resource");
        const auto nr=u32(bytes.data()+80),nn=u32(bytes.data()+84),nl=u32(bytes.data()+88);
        if(!nn||nn>32767||nr!=nn+1||nr!=nl||nr>32768||
           bytes.size()!=92u+std::size_t(32)*nn+std::size_t(24)*nr)
            return fail(error,"invalid original wreck type2 resource extents");
        out.nodes.resize(nn);out.rays.resize(nr);
        std::vector<Type2OpaqueEndpointWords49X> opaque(nr);
        for(std::size_t i=0;i<nr;++i)if(!policy.words(policy.context,piece,i,opaque[i]))
            return fail(error,"caller endpoint-W policy does not cover original wreck rays");
        std::vector<std::uint32_t> topology(std::size_t(nn)+nr);
        if(decodeType2Resource49X(bytes.data(),bytes.size(),opaque.data(),opaque.size(),
            out.nodes.data(),out.nodes.size(),out.rays.data(),out.rays.size(),
            {topology.data(),topology.size()},out.view)!=Type2ResourceStatus49X::Complete)
            return fail(error,"original wreck type2 native decoding rejected");
        return true;
    }
    static bool decodeTarget(const std::vector<std::uint8_t>& bytes,OriginalWreckTarget50LA& out,std::string& error) {
        rr::assets::Collision48G source;rr::assets::DecodeError48C why;std::vector<std::uint8_t> cache;
        if(!rr::assets::decodeCollision48G(bytes,source,why)||
           !rr::assets::encodeCollisionAsset48G(source,cache,error)||
           !rr::assets::loadCollisionMesh48I(cache,out.mesh,error)||out.mesh.nodes.empty())
            return fail(error,"original wreck type0 native decoding rejected");
        out.radii.resize(out.mesh.nodes.size());
        std::vector<std::uint32_t> topology(out.mesh.nodes.size()+out.mesh.faces.size());
        if(prepareStaticTargetRadii49X(out.mesh,out.radii.data(),out.radii.size(),
            {topology.data(),topology.size()},out.maxDepth)!=Type2ResourceStatus49X::Complete)
            return fail(error,"original wreck type0 target tree rejected");
        std::memcpy(out.source.local40.v,out.mesh.local_matrix.data(),sizeof(BodyMatrix48G));
        std::memcpy(out.source.rootCenter.v,out.mesh.nodes[0].center.data(),sizeof(Vector3_48C));
        std::memcpy(out.source.rootHalf.v,out.mesh.nodes[0].half.data(),sizeof(Vector3_48C));
        return true;
    }
    static bool model(const OriginalWreckConstructorVectors50LA& constructor,Storage& out,std::string& error) {
        for(const auto* v:{&constructor.position434830,&constructor.look434860,
                           &constructor.up434850,&constructor.zero433B10})
            for(float lane:*v)if(!std::isfinite(lane))return fail(error,"nonfinite original wreck constructor vector");
        rr::assets::GeometryDiagnostic49O diagnostic;
        rr::assets::OriginalStandardResourceDiagnostic50LA resourceDiagnostic;
        auto resource=std::make_shared<rr::assets::OriginalStandardModelResource50LA>();
        // The exact resolved archive name and retained bytes already passed
        // the occurrence inventory checks. The existing Standard loader owns
        // immutable geometry/hierarchy; no second decoded Scene48D is kept.
        if(!resource->prepare(out.receipt.members[1].name,out.raw[1],resourceDiagnostic)||
           !rr::assets::readPsgHierarchy49O(out.raw[1].data(),out.raw[1].size(),out.hierarchy,diagnostic))
            return fail(error,"original wreck Standard PSG hierarchy/geometry unsupported");
        const auto count=resource->nodeCount();out.standardModel=std::move(resource);
        if(!count||count>maxSceneNodes49P||out.standardModel->scene()->nodes.size()!=count)
            return fail(error,"original wreck model graph extent invalid");
        out.nodes.resize(count);
        out.sourceNodes.resize(count);auto& originals=out.sourceNodes;
        for(std::uint32_t i=0;i<count;++i){
            auto& original=originals[i];
            original=out.standardModel->sourceNodes()[i];
            if(i==0?original.parent!=-1:original.parent<0)
                return fail(error,"original wreck model requires one actual PSG root");
            auto& node=out.nodes[i];node={};
            std::memcpy(node.localC0.v,original.local,sizeof original.local);
            //2893A0 ctor then28CB90..28CBF0, including actual root row0.
            node.sceneFlags190=0xa4;
            node.parent180=static_cast<SceneNodeIndex49P>(original.parent);
            if(i){
                auto& parent=out.nodes[static_cast<std::size_t>(original.parent)];
                const auto index=static_cast<SceneNodeIndex49P>(i);
                if(parent.firstChild184==SceneNodeIndex49P::none)parent.firstChild184=index;
                else{
                    auto sibling=parent.firstChild184;
                    while(out.nodes[static_cast<std::size_t>(sibling)].nextSibling188!=SceneNodeIndex49P::none)
                        sibling=out.nodes[static_cast<std::size_t>(sibling)].nextSibling188;
                    out.nodes[static_cast<std::size_t>(sibling)].nextSibling188=index;
                }
            }
        }
        //165C84..165C98 retain the actual22C148/158/194 constructor inputs.
        out.position50=constructor.position434830;out.look60=constructor.look434860;out.up70=constructor.up434850;
        //165CF8..165D80 compares XYZ only.28B180 copies XYZ only, retaining50W.
        bool sentinel=true;for(unsigned j=0;j<3;++j)
            sentinel=sentinel&&out.position50[j]==constructor.zero433B10[j];
        if(sentinel)for(unsigned j=0;j<3;++j)out.position50[j]=out.nodes[0].localC0.v[12+j];
        //165D84 ->28AB90 resets XYZ only. It does not discard authored basis,
        // replace root by a wrapper, normalize NULLCG, or overwrite localFC.
        for(unsigned j=0;j<3;++j)out.nodes[0].localC0.v[12+j]=constructor.zero433B10[j];
        const SceneGraph49P graph{out.nodes.data(),count};
        if(!validateSceneGraph49P(graph))return fail(error,"original wreck scene graph invalid");
        for(std::uint32_t i=0;i<count;++i)
            if(!refreshSceneWorld49P(graph,static_cast<SceneNodeIndex49P>(i)))
                return fail(error,"original wreck scene world preparation failed");
        //166400 consumes model14C's real model box (28D670), not either COL
        // root box. The kernel receives root row0 once, then each actual child
        // in original flat order. Child parent0 becomes the context root.
        ModelBoundsContext49O context{};context.rootWorld100=out.nodes[0].world100;
        context.rootSceneFlags190=out.nodes[0].sceneFlags190;
        for(unsigned j=0;j<3;++j){context.rootCenter140.v[j]=originals[0].bounds[j];
            context.rootHalfExtent150.v[j]=originals[0].bounds[3+j];}
        std::vector<ModelBoundsNode49O> children(count-1);
        for(std::uint32_t i=1;i<count;++i){auto& child=children[i-1];child={};
            child.local=out.nodes[i].localC0;child.parent=originals[i].parent-1;
            child.sceneFlags190=out.nodes[i].sceneFlags190;
            for(unsigned j=0;j<3;++j){child.center140.v[j]=originals[i].bounds[j];
                child.halfExtent150.v[j]=originals[i].bounds[3+j];}}
        ModelBoundsWorkspace49O workspace{};
        if(!computeModelBounds49O(children.data(),children.size(),context,workspace,out.bounds)||!out.bounds.seeded)
            return fail(error,"original wreck model bounds unavailable");
        // Neither28D670 nor the auxiliary inertia descriptor above writes the
        // model cache. Its radius198 is source-unwritten until26F4B0; do not
        // expose host value-initialization as an original cached bound.
        out.receipt.sourceModelNodeCount=count;return true;
    }
    static bool load(const OriginalWreckAssetBinding50LA& binding,
        const OriginalWreckEndpointPolicy50LA& endpoint,Storage& out,std::string& error) {
        std::ifstream file(rr::assets::dlc50DL::host(binding.archivePath),std::ios::binary|std::ios::ate);
        if(!file)return fail(error,"original wreck RES unavailable");
        const auto extent=static_cast<std::streamoff>(file.tellg());
        if(extent<=0||extent>static_cast<std::streamoff>(rr::assets::DecodeLimits48C{}.resource_file_bytes))
            return fail(error,"original wreck RES extent unsupported");
        std::vector<std::uint8_t> bytes(static_cast<std::size_t>(extent));file.seekg(0);
        if(!file.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size())))
            return fail(error,"original wreck RES read incomplete");
        rr::assets::ResArchive48C archive;rr::assets::DecodeError48C why;
        if(!rr::assets::decodeResource48C(bytes,archive,why))return fail(error,"original wreck RES rejected");
        auto& receipt=out.receipt;receipt.archiveBytes=bytes.size();receipt.archivePath=binding.archivePath;
        receipt.phyMember=binding.phyMember;
        if(!member(archive,binding.phyMember,out.raw[0],receipt.members[0],error))return false;
        Document doc;
        if(!document(out.raw[0],doc,error)||!author(doc,out.authoring,error))return false;
        if(!podIdentity(binding,doc,error))return false;
        return loadSelectedPiece(archive,binding,endpoint,out,error);
    }
    static bool podIdentity(const OriginalWreckAssetBinding50LA& binding,const Document& doc,
                            std::string& error) {
        const auto* prefix=value(doc,"Pod Setup","Prefix");
        if(!prefix)return fail(error,"original wreck PHY pod identity missing");
        const bool digits=prefix->size()==2&&(*prefix)[0]>='0'&&(*prefix)[0]<='9'&&(*prefix)[1]>='0'&&(*prefix)[1]<='9';
        // One stem rule: a two-digit Prefix names "pod"+Prefix (stock); any
        // other Prefix must be an admitted add-on pod's stem (dlc-contract-2:
        // Prefix equals the stem; PORT EXTENSION, NOT ORIGINAL).
        const auto* addon=digits?nullptr:rr::assets::dlc50DL::addonPodByPodId(*prefix);
        if(!digits&&(!addon||lower(*prefix)!=addon->stem))
            return fail(error,"original wreck PHY pod identity missing");
        const auto stem=digits?"pod"+*prefix:addon->stem;
        const auto expectedPrefix="pods\\"+stem+"\\";
        if(lower(binding.memberPrefix)!=expectedPrefix||
           lower(binding.phyMember)!=expectedPrefix+stem+".phy")
            return fail(error,"original wreck PHY/archive pod identity mismatch");
        return true;
    }
    static bool loadSelectedPiece(const rr::assets::ResArchive48C& archive,
        const OriginalWreckAssetBinding50LA& binding,const OriginalWreckEndpointPolicy50LA& endpoint,
        Storage& out,std::string& error) {
        if(out.authoring.selectedCollection>=out.authoring.collections.size()||
           out.selectedPieceIndex>=out.authoring.collections[out.authoring.selectedCollection].pieces.size())
            return fail(error,"original wreck selected piece identity out of range");
        auto& receipt=out.receipt;
        const auto& piece=out.authoring.collections[out.authoring.selectedCollection].pieces[out.selectedPieceIndex];
        receipt.sltRequest=binding.memberPrefix+piece.stem+".slt";
        //165C30 model vslotA8=240740 ->26E820;26E8F4..26E934 replaces
        // the extension with actual3B9790 '.psg'. No guessed SLT binary reader.
        receipt.psgMember=binding.memberPrefix+piece.stem+".psg";
        //165F80..166020 use this same source stem for m/f collision owners.
        receipt.queryMember=binding.memberPrefix+piece.stem+"m.col";
        receipt.targetMember=binding.memberPrefix+piece.stem+"f.col";
        if(!member(archive,receipt.psgMember,out.raw[1],receipt.members[1],error)||
           !member(archive,receipt.queryMember,out.raw[2],receipt.members[2],error)||
           !member(archive,receipt.targetMember,out.raw[3],receipt.members[3],error)||
           !model(*binding.constructorVectors,out,error)||
           !decodeQuery(out.raw[2],piece,endpoint,out.query,error)||
           !decodeTarget(out.raw[3],out.target,error))return false;
        receipt.endpointWordsCallerSupplied=true;return true;
    }
    std::unique_ptr<Storage> storage_;
};
} // namespace rr::game
