// Common rewrite (R3, PLAN 2.1 steps 04-05, 2.2): the ONE track load of a race, for every
// course - 1A6D80 -> 1A9EE0 in the original order, every count read from the course's own
// files. Implements the R3 part of common_track50cr.h (W0 freeze): identityForCourse,
// Archive, Track::~Track/release/logTrackData, Load::run, readScene270900, readTob1B4330,
// buildPhysicsIndex270CDC, buildCollision271230, readInstances274080, finishTail1AA400 and
// Application::loadConsumedRaceTrack1A9EE0.
//
// Original order (RA's native chain, original_race_world_tob_prefix50la.cpp, reduced to its
// data reads): 1A6D80 name 187330 -> archive 12CEA0/132A20/132E40 -> 1A9F68 save model
// directory -> 1AA078 SCB bind / 270900 SceneInfo -> 271C10 SCB withdraw -> 1B4330 TOB ->
// 271370 terrain / 271230 collision -> 273C30 fog -> 1B4DC0 particles -> 274080 instances ->
// 274160 materials -> 274E90 triggers -> 274570/2745D4 water -> 274AE0 sky -> 1B4720/271600
// AUB -> lights -> 101860 animation -> 1AA400..1AA508 tail (restore model directory).
// RA's constants (archive 4604561/1010, ra.scb 540795, ra.tob 95558, ra.aub 22511, the
// 178/180/2/113/713/8/7/41/10/20/1472/1261/1085/176/673 counts) are data reads here and
// receipts only (stock_course_receipts50cr.h, logged by Track::logTrackData).
//
// Runs BEFORE the camera session (1A6DC0), as the original does; it has no Session
// dependency. Nothing here names a course; nothing writes guest memory.
#include "common_track50cr.h"
#include "common_race_world50cr.h"
#include "stock_course_receipts50cr.h"
#include "track_collision_world50cr.h"
#include "single_pass_world50lm.h"
#include "track_static_model_params50cr.h"   // 271A00 rows (I2 applies them to the standard models)
#include "track_atm50cr.h"
#include "track_particle_emitters50cr.h"  // Roles::emitters teardown (quiesce, closeAfterPool)
#include "mc_world_semantics50mc.h"
#include "course_archive_receipt50ci.h"
#include "native_course_archive_release50pd.h"
#include "track_session_assets50ig_fixture.h"
#include "runtime_asset_paths50lb.h"
#include "pod_materials50ed_fixture.h"
#include "original_ui_main_entry50kg.h"
#include "../../src/assets/dlc_files50dl.h"
#include "../../src/assets/dlc_registry50dl.h"
#include "../../src/game/dlc_course_entries50dl.h"
#include "../../src/assets/original_standard_archive_provider50la.h"
#include "../../src/assets/original_standard_texture_resource50la.h"
#include "../../src/assets/original_standard_material_domain50la.h"
#include "../../src/assets/track_height_descriptor50jb.h"
#include "../../src/assets/original_ui_archive50kg.h"
#include "../../src/assets/original_ui_material_application50la.h"
#include "../../src/game/original_application_entities50la.h"
#include "../../src/game/vehicle_animation49t.h"
#include <charconv>
#include <cstdio>
#include <fstream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace common_track50CR {
namespace a=rr::assets;
namespace receipts=stock_course_receipts50CR;
using Provider=a::OriginalStandardArchiveProvider50LA;

namespace {
std::string upper(std::string s){for(auto& c:s)if(c>='a'&&c<='z')c=static_cast<char>(c-'a'+'A');return s;}
bool sameName(std::string_view x,std::string_view y)noexcept{return Provider::sameName(x,y);}
// Host file read (stock disc path or dlc:/ overlay through dlc50DL::host). Size-bounded.
bool readHostFile(const std::string& path,Bytes& out,std::size_t limit,std::string& why)noexcept{
    try{
        const auto host=a::dlc50DL::host(runtime_asset_paths50LB::resolve(path));
        std::ifstream file(host,std::ios::binary|std::ios::ate);
        if(!file){why="file missing: "+host.string();return false;}
        const auto size=file.tellg();
        if(size<=0||static_cast<std::uint64_t>(size)>limit){why="file size outside the archive limit: "+host.string();return false;}
        Bytes next(static_cast<std::size_t>(size));file.seekg(0);
        file.read(reinterpret_cast<char*>(next.data()),size);
        if(!file){why="file read failed: "+host.string();return false;}
        out=std::move(next);return true;
    }catch(const std::exception& e){try{why=e.what();}catch(...){}return false;}
    catch(...){return false;}
}
// Unique block / field of a parameter document (a duplicate is refused, absence is genuine).
bool uniqueBlock(const a::ParameterDocument48F& doc,std::string_view name,const a::ParameterBlock48F*& out)noexcept{
    out=nullptr;for(const auto& b:doc.blocks)if(b.name==name){if(out)return false;out=&b;}
    return true;
}
bool uniqueField(const a::ParameterBlock48F& block,std::string_view name,const a::ParameterField48F*& out)noexcept{
    out=nullptr;for(const auto& f:block.fields)if(f.name==name){if(out)return false;out=&f;}
    return true;
}
bool decimal(std::string_view text,std::uint32_t& out)noexcept{
    while(!text.empty()&&text.front()==' ')text.remove_prefix(1);
    while(!text.empty()&&text.back()==' ')text.remove_suffix(1);
    if(text.empty())return false;
    std::uint32_t v=0;const auto r=std::from_chars(text.data(),text.data()+text.size(),v);
    if(r.ec!=std::errc{}||r.ptr!=text.data()+text.size())return false;
    out=v;return true;
}
// Count field of a block: absent block = 0 (the original reads nothing); a present block
// must carry the field exactly once with a decimal value.
enum class CountRead {absent,present,refused};
CountRead countField(const a::ParameterDocument48F& doc,std::string_view block,std::string_view field,std::uint32_t& out)noexcept{
    out=0;const a::ParameterBlock48F* b=nullptr;const a::ParameterField48F* f=nullptr;
    if(!uniqueBlock(doc,block,b))return CountRead::refused;
    if(!b)return CountRead::absent;
    if(!uniqueField(*b,field,f)||!f)return CountRead::refused;
    return decimal(f->value,out)?CountRead::present:CountRead::refused;
}
bool refuse(Diagnostic& d,log::Step step,std::uint32_t pc,std::string why)noexcept{
    try{d.refuse(step,pc,std::move(why));log::load(step,false,d.value,d.reason,nullptr,pc);}catch(...){}
    return false;
}
bool pass(Diagnostic& d,log::Step step,std::uint64_t value,std::string_view why={})noexcept{
    try{d.step=step;d.pc=log::stepRow(step).pc;d.value=value;log::load(step,true,value,why);}catch(...){}
    return true;
}
const receipts::Receipt* stockReceipt(const Track& t,std::string_view role)noexcept{
    return t.source()==Source::stock?receipts::find(t.code(),role):nullptr;
}
} // namespace

// ---------------------------------------------------------------- identity (187330 name)
bool identityForCourse(std::string_view code,bool registryRow,Identity& out,std::string& error)noexcept{
    try{
        Identity next;
        if(!Code::parse(code,next.code)){error="course code is not two letters/digits";log::load(log::Step::archive12CEA0,false,0,error);return false;}
        const auto lowerCode=next.code.lower();const std::string upperCode(next.code.view());
        next.loadingPst="LD_"+upperCode+".PST";
        next.tobMember="tracks\\"+lowerCode+".tob";next.aubMember="tracks\\"+lowerCode+".aub";
        if(!registryRow){
            const auto* world=track_session_assets50IG::findWorld(upperCode.c_str());
            const auto* assets=world?track_session_assets50IG::sessionAssets(*world):nullptr;
            if(!assets){error="course code is not a stock course row";log::load(log::Step::archive12CEA0,false,0,error);return false;}
            next.source=Source::stock;
            // PORT STAND-IN: courseNumber (Hall row) is not filled for a stock row; nothing in the
            // load reads it (the Course screen owns the order).
            next.archivePath=assets->archive;
            next.listedSha256=assets->archiveSha256?assets->archiveSha256:"";
            next.listedBytes=assets->archiveBytes;
            next.scbMember=world->worldMember;
            next.mapSpline=assets->mapSpline;next.aiSpline=assets->aiSpline;
            next.tuning=assets->impactTuning?assets->impactTuning:"";
        }else{
            rr::game::dlc50DL::AddonCourseLoad load;
            const auto* row=a::dlc50DL::addonCourseByCode(lowerCode);
            if(!row)row=a::dlc50DL::addonCourseByCode(upperCode);
            if(!row||!rr::game::dlc50DL::addonCourseLoad(row->number,load)){
                error="course code is not an admitted registry course";log::load(log::Step::archive12CEA0,false,0,error);return false;}
            next.source=Source::registry;next.courseNumber=load.number;
            next.archivePath=load.archive;next.listedSha256=row->archiveSha256;next.listedBytes=row->archiveBytes;
            next.scbMember=load.scb;if(!load.tob.empty())next.tobMember=load.tob;if(!load.aub.empty())next.aubMember=load.aub;
            next.mapSpline=load.mapSpline;next.aiSpline=load.aiSpline;next.tuning=load.tuningMember;
            next.registryPackage=load.key;
            if(!load.loadingScreen.empty()&&load.loadingFromPackage)next.loadingPst=load.loadingScreen;
        }
        if(next.archivePath.empty()||next.scbMember.empty()){error="course row names no archive or SCB";log::load(log::Step::archive12CEA0,false,0,error);return false;}
        out=std::move(next);error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();log::load(log::Step::archive12CEA0,false,0,error);}catch(...){}return false;}
    catch(...){return false;}
}

// ---------------------------------------------------------------- archive (step 04)
struct Archive::State {
    a::ResArchive48C decoded;
    std::string sha256,container;
    std::uint64_t bytes=0;
    std::shared_ptr<a::ResArchive48C> particles;      // registry inherit.particles closure (PORT EXTENSION)
    bool opened=false,mounted=false;
};
Archive::Archive()noexcept=default;
Archive::~Archive()=default;
bool Archive::opened()const noexcept{return state_&&state_->opened;}
const a::ResArchive48C* Archive::decoded()const noexcept{return opened()?&state_->decoded:nullptr;}
std::uint64_t Archive::bytes()const noexcept{return opened()?state_->bytes:0u;}
const std::string& Archive::sha256()const noexcept{static const std::string none;return opened()?state_->sha256:none;}
std::size_t Archive::memberCount()const noexcept{return opened()?state_->decoded.entries.size():0u;}
bool Archive::exists(std::string_view name)const noexcept{
    if(!opened()||name.empty())return false;
    for(const auto& e:state_->decoded.entries)if(sameName(e.name,name))return true;
    return false;
}
bool Archive::member(std::string_view name,Bytes& out,std::size_t limit,Diagnostic& d)const noexcept{
    try{
        if(!opened()){d.reason="archive not opened";return false;}
        const a::ResourceEntry48C* match=nullptr;
        for(const auto& e:state_->decoded.entries)if(sameName(e.name,name)){
            if(match){d.reason="ambiguous archive member: "+std::string(name);return false;}match=&e;}
        if(!match){d.reason="archive member absent: "+std::string(name);return false;}
        if(match->bytes>limit){d.reason="archive member larger than its limit: "+std::string(name);return false;}
        Bytes next;
        if(!a::extractResMember48C(state_->decoded,match->name,next,d.decode,limit)||next.size()!=match->bytes){
            d.reason="archive member extraction failed: "+std::string(name)+" "+d.decode.code;return false;}
        out=std::move(next);return true;
    }catch(...){d.reason="archive member allocation failed";return false;}
}
bool Archive::open(const Identity& identity,const std::shared_ptr<Provider>& provider,
    const std::shared_ptr<a::OriginalStandardTextureResolver50LA>& resolver,Diagnostic& d)noexcept{
    constexpr auto step=log::Step::archive12CEA0;
    try{
        if(state_&&state_->opened)return refuse(d,step,0x12CEA0u,"archive already opened for this race");
        if(!provider||!resolver)return refuse(d,step,0x12CEA0u,"Application archive provider/resolver 3E1A70 unavailable");
        auto next=std::make_unique<State>();
        Bytes raw;std::string why;
        if(!readHostFile(identity.archivePath,raw,course_archive_receipt50CI::maximumArchiveBytes(),why))return refuse(d,step,0x12CEA0u,why);
        next->bytes=raw.size();next->sha256=pod_materials50ed_fixture::detail::sha256(raw);
        next->container="TRACKS/"+std::string(identity.code.view())+".RES";
        // Archive identity: a stock row's size/SHA and a registry row's listed size/SHA are receipts.
        if(identity.source==Source::stock){
            const auto* size=receipts::find(identity.code.view(),"archive_size");
            const auto* hash=receipts::findHash(identity.code.view(),"archive_sha256");
            log::trackData(log::Role::archive,identity.code.view(),identity.archivePath,next->bytes,size!=nullptr,size?size->value:0);
            log::trackDataNamed("archive_sha256",identity.code.view(),next->sha256,1u,hash!=nullptr,hash&&hash->sha256==next->sha256?1u:0u);
        }else{
            log::trackData(log::Role::archive,identity.code.view(),identity.archivePath,next->bytes,false,0);
            log::trackDataNamed("archive_listed",identity.code.view(),identity.listedSha256,
                identity.listedBytes==next->bytes&&identity.listedSha256==next->sha256?1u:0u,true,1u);
        }
        if(!a::decodeResource48C(raw,next->decoded,d.decode,nullptr,course_archive_receipt50CI::limits())||next->decoded.entries.empty())
            return refuse(d,step,0x132A20u,"course archive decode: "+d.decode.code);
        raw.clear();raw.shrink_to_fit();
        for(std::size_t i=0;i<next->decoded.entries.size();++i)for(std::size_t j=0;j<i;++j)
            if(sameName(next->decoded.entries[i].name,next->decoded.entries[j].name))
                return refuse(d,step,0x132A20u,"ambiguous archive member identity: "+next->decoded.entries[i].name);
        // One race mount lease for every mount of this race (released at teardown, 1316A0).
        // Null when an earlier race's lease is still open: the mounts then behave as before.
        auto* lease=native_course_archive_release50PD::holder().begin(provider,resolver);
        // Registry inherit.particles (PORT EXTENSION, NOT ORIGINAL): the named stock archive's
        // particle closure joins this course's native namespace, in its own order.
        if(identity.source==Source::registry){
            rr::game::dlc50DL::AddonCourseLoad row;
            const std::string code(identity.code.view());
            if(rr::game::dlc50DL::addonCourseLoadByCode(code.c_str(),row)&&row.particlesInherited){
                Bytes stock;
                if(!readHostFile("work/native/project/disc/"+row.particleArchive,stock,course_archive_receipt50CI::maximumArchiveBytes(),why))
                    return refuse(d,step,0x132A20u,"inherited particle archive: "+why);
                a::ResArchive48C source;
                if(!a::decodeResource48C(stock,source,d.decode,nullptr,course_archive_receipt50CI::limits()))
                    return refuse(d,step,0x132A20u,"inherited particle archive is not a readable RES");
                auto subset=std::make_shared<a::ResArchive48C>();
                for(const char* wanted:{"particle\\demoprt.prt","particle\\demoprt.psm","particle\\demoprt.pst"}){
                    const a::ResourceEntry48C* entry=nullptr;
                    for(const auto& e:source.entries)if(sameName(e.name,wanted)){entry=&e;break;}
                    Bytes member;
                    if(!entry||!a::extractResMember48C(source,entry->name,member,d.decode)||member.empty()||
                       member.size()>std::numeric_limits<std::uint32_t>::max()-subset->decoded.size())
                        return refuse(d,step,0x132A20u,"inherited particle member missing from the named stock archive");
                    subset->entries.push_back({entry->name,static_cast<std::uint32_t>(subset->decoded.size()),static_cast<std::uint32_t>(member.size())});
                    subset->decoded.insert(subset->decoded.end(),member.begin(),member.end());
                }
                subset->blocks.push_back({0u,static_cast<std::uint32_t>(subset->decoded.size())});
                Provider::Diagnostic pd;
                if(!provider->mountAuthenticatedRes(row.particleArchive,pod_materials50ed_fixture::detail::sha256(stock),stock.size(),*subset,pd,lease))
                    return refuse(d,step,0x132E40u,"inherited particle closure mount rejected");
                next->particles=std::move(subset);
            }
        }
        // 12CEA0/132E40: mount under the race's mount lease. The mount label is the archive's
        // actual SHA-256/size (PORT STAND-IN: the provider's authentication label; the original
        // mounts by name only).
        Provider::Diagnostic pd;
        if(!provider->mountAuthenticatedRes(next->container,next->sha256,static_cast<std::size_t>(next->bytes),next->decoded,pd,lease))
            return refuse(d,step,0x132E40u,"course archive mount rejected (status "+std::to_string(unsigned(pd.status))+")");
        next->mounted=true;
        a::OriginalStandardTextureDiagnostic50LA td;
        if(!provider->sealNativeMounts(pd)||!resolver->bindProvider3E1A70(provider,td))
            return refuse(d,step,0x132E40u,"course archive namespace seal/bind rejected");
        next->opened=true;state_=std::move(next);
        d.value=state_->bytes;
        return pass(d,step,state_->bytes);
    }catch(const std::exception& e){return refuse(d,step,0x12CEA0u,e.what());}
    catch(...){return refuse(d,step,0x12CEA0u,"course archive allocation failed");}
}
bool Archive::release()noexcept{
    const bool released=native_course_archive_release50PD::holder().release();
    if(released&&state_){state_->mounted=false;state_->opened=false;state_.reset();}
    log::teardownRole("archive",released);
    return released;
}

// ---------------------------------------------------------------- 05.01 SCB / SceneInfo
bool readScene270900(Track& track,const LoadInputs&,Diagnostic& d)noexcept{
    constexpr auto step=log::Step::scene270900;
    try{
        Bytes scb;
        if(!track.archive.member(track.identity.scbMember,scb,course_archive_receipt50CI::maximumArchiveBytes(),d))
            return refuse(d,step,0x1AA078u,"SCB: "+d.reason);
        a::World48F world;a::DecodeError48C error;
        a::WorldLimits48F limits;limits.input_bytes=std::max(limits.input_bytes,scb.size());
        bool strict=a::decodeWorld48F(scb,world,error,nullptr,limits)&&a::validateWorldSemantics48F(world,error);
        if(!strict){
            // The original 270900 reader takes a label's first value; an SCB that retains
            // duplicate unused labels is read by the tolerant reader (never unchecked).
            // PORT STAND-IN: the strict decoder is the port's; the tolerant reader still lives
            // in an MC-named file (mc_world_semantics50mc.h) and applies to every course.
            world={};a::DecodeError48C tolerant;
            if(!mc_world_semantics50MC::decodeWorld(scb,world,tolerant,nullptr,limits))
                return refuse(d,step,0x270900u,"SCB SceneInfo decode: "+error.code+" / "+tolerant.code);
        }
        if(world.zones.empty()||world.zones.size()>track_runtime_storage50IG::hardZones||
           world.portals.size()>track_runtime_storage50IG::hardPortals)
            return refuse(d,step,0x270900u,"SCB zones/portals outside the zone-world store");
        for(std::size_t i=0;i<world.zones.size();++i)if(world.zones[i].zone!=i+1)
            return refuse(d,step,0x270900u,"SCB zone order");
        std::uint32_t collisions=0;
        for(const auto& instance:world.instances)if(track.archive.exists(instance.collision_member))++collisions;
        // A9: water present unless the Environment's WaterFile is "none" (2745B0 compare);
        // the same read the collision ground's height descriptor makes.
        track_height_cold50JB::Owner height;std::string why;
        if(!track_height_cold50JB::prepareFreshGameTrack(world,*track.archive.decoded(),track.identity.scbMember,height,why))
            return refuse(d,step,0x270A58u,"SCB Environment height/water: "+why);
        track.counts.zones=static_cast<std::uint32_t>(world.zones.size());
        track.counts.portals=static_cast<std::uint32_t>(world.portals.size());
        track.counts.portalLinks=2u*track.counts.portals;
        track.counts.instances=static_cast<std::uint32_t>(world.instances.size());
        track.counts.collisions=collisions;
        track.counts.water=height.sourceDomain.water334==rr::game::UndertrackHeightOwner50EW::Present;
        track.documents.waterName=track.counts.water?std::string(height.descriptor.waterFile110.value.data()):std::string();
        track.documents.waterLogicalOffset=track.counts.water?height.waterLogicalOffset:0u;
        track.documents.scb=std::move(scb);track.documents.world=std::move(world);
        return pass(d,step,track.counts.zones,strict?"strict":"tolerant_duplicate_labels");
    }catch(const std::exception& e){return refuse(d,step,0x270900u,e.what());}
    catch(...){return refuse(d,step,0x270900u,"SCB allocation failed");}
}

// ---------------------------------------------------------------- 05.02 TOB
bool readTob1B4330(Track& track,const LoadInputs&,Diagnostic& d)noexcept{
    constexpr auto step=log::Step::tob1B4330;
    try{
        Bytes tob;
        if(!track.archive.member(track.identity.tobMember,tob,course_archive_receipt50CI::maximumArchiveBytes(),d))
            return refuse(d,step,0x1B4330u,"TOB: "+d.reason);
        // Duplicate labels are retained (the original reader takes the first); the readers that
        // consume a label refuse a duplicate of it themselves (R5 triggers/animations).
        a::ParameterDocument48F doc;a::DecodeError48C error;a::WorldLimits48F limits;limits.input_bytes=std::max(limits.input_bytes,tob.size());
        limits.retainDuplicateLabels=true;
        if(!a::decodeParameters48F(tob,doc,error,nullptr,limits))return refuse(d,step,0x1B4330u,"TOB decode: "+error.code);
        struct Row {std::string_view block,field;std::uint32_t* out;std::uint32_t pc;};
        const Row rows[]={
            {"cscameras","numberofcameras",&track.counts.cuts,0x2723A0u},                // World+3E4 (174E58)
            {"triggers","numberoftriggers",&track.counts.triggers,0x2DC474u},            // 274E90 / 1B5000
            {"physicsobjects","numberofphysicsobjects",&track.counts.physicsObjects,0x2718C0u},
            {"staticmodelparams","numberofstaticmodelparams",&track.counts.staticModels,0x271A4Cu},
            {"animations","numberofanimations",&track.counts.animations,0x2713E0u},           // 101860 rows (count only; R5 reads them)
        };
        for(const auto& r:rows)if(countField(doc,r.block,r.field,*r.out)==CountRead::refused)
            return refuse(d,step,r.pc,"TOB ["+std::string(r.block)+"] "+std::string(r.field)+" duplicate or not a count");
        std::uint32_t actions=0;
        for(std::uint32_t i=1;i<=track.counts.triggers;++i){
            std::uint32_t n=0;const auto block="trigger"+std::to_string(i);
            if(countField(doc,block,"numberofactions",n)==CountRead::refused)
                return refuse(d,step,0x2DC474u,"TOB ["+block+"] numberofactions duplicate or not a count");
            if(n>std::numeric_limits<std::uint32_t>::max()-actions)return refuse(d,step,0x2DC474u,"TOB trigger action total overflow");
            actions+=n;
        }
        track.counts.triggerActions=actions;
        track.documents.tob=std::move(tob);track.documents.tobDocument=std::move(doc);
        // AUB (1B4720 binds it after the TOB): bytes and document for 05.12 (R7). An AUB that
        // does not decode is left empty here; 05.12 refuses it with its own reason.
        if(track.archive.exists(track.identity.aubMember)){
            Bytes aub;Diagnostic member;
            if(track.archive.member(track.identity.aubMember,aub,course_archive_receipt50CI::maximumArchiveBytes(),member)){
                a::ParameterDocument48F aubDoc;a::DecodeError48C aubError;a::WorldLimits48F aubLimits;aubLimits.input_bytes=std::max(aubLimits.input_bytes,aub.size());
                if(a::decodeParameters48F(aub,aubDoc,aubError,nullptr,aubLimits))track.documents.aubDocument=std::move(aubDoc);
                track.documents.aub=std::move(aub);
            }
        }
        return pass(d,step,track.documents.tob.size());
    }catch(const std::exception& e){return refuse(d,step,0x1B4330u,e.what());}
    catch(...){return refuse(d,step,0x1B4330u,"TOB allocation failed");}
}

// ---------------------------------------------------------------- 05.03 physics index + storage plan
bool buildPhysicsIndex270CDC(Track& track,const LoadInputs&,Diagnostic& d)noexcept{
    constexpr auto step=log::Step::physicsIndex270CDC;
    try{
        namespace rt=track_runtime_storage50IG;
        // 270CDC/27C110: one per-zone physics index over the SCB's own zones; the terrain
        // owners are the placed COL owners (271370).
        // PORT STAND-IN: the zone-world Storage (track_runtime_storage50IG) is allocated from
        // these counts later, at collision prepareStorage, because its shared layout also sizes
        // every racer's query rows from the roster (step 10). The original registers racers
        // into an already-built world; the port pre-sizes their rows instead.
        if(track.counts.zones>rt::hardZones||track.counts.portals>rt::hardPortals)
            return refuse(d,step,0x27C110u,"zones/portals exceed the zone-world store");
        if(!track.counts.collisions||track.counts.collisions>rt::hardTerrainOwners||
           track.counts.triggers>rt::hardTerrainOwners-track.counts.collisions)
            return refuse(d,step,0x270CDCu,"placed COL + trigger owners exceed the terrain owner store");
        track.counts.terrain=track.counts.collisions;
        log::storage("terrain_owners",track.counts.terrain+track.counts.triggers,0,rt::hardTerrainOwners);
        log::storage("zones",track.counts.zones,0,rt::hardZones);
        track.roles.collision=std::make_shared<track_collision_world50CR::Owner>();
        return pass(d,step,track.counts.terrain);
    }catch(const std::exception& e){return refuse(d,step,0x270CDCu,e.what());}
    catch(...){return refuse(d,step,0x270CDCu,"physics index allocation failed");}
}

// ---------------------------------------------------------------- 05.04 collision zones
bool buildCollision271230(Track& track,const LoadInputs&,Diagnostic& d)noexcept{
    constexpr auto step=log::Step::collision271230;
    if(!track.roles.collision)return refuse(d,step,0x271230u,"collision world not created at 05.03");
    std::string error;
    if(!track.roles.collision->prepareGround(track,error))return refuse(d,step,0x27BB40u,"collision ground: "+error);
    if(track.roles.collision->sourceCount()!=track.counts.collisions)return refuse(d,step,0x27BB40u,"placed COL count differs from the SCB read");
    return pass(d,step,track.roles.collision->sourceCount());
}

// ---------------------------------------------------------------- 05.07 instances / standard models
bool readInstances274080(Track& track,const LoadInputs&,Diagnostic& d)noexcept{
    constexpr auto step=log::Step::instances274080;
    try{
        single_pass_world50LM::Identity id;
        const std::string code(track.code());
        id.code=code;id.container="TRACKS/"+code+".RES";id.scb=track.identity.scbMember;
        id.sha=track.archive.sha256();id.bytes=static_cast<std::size_t>(track.archive.bytes());
        // Texture identity namespace: the two code bytes (the rule every course already used).
        id.asset=(std::uint64_t(std::uint8_t(code[0]))<<8)|std::uint8_t(code[1]);
        id.stockRow=false;id.mountedByTrack=true;
        auto world=std::make_shared<single_pass_world50LM::Owner>(std::move(id));
        single_pass_world50LM::Diagnostic sd;
        if(!world->loadDecoded(*track.archive.decoded(),track.documents.world,sd))
            return refuse(d,step,0x274080u,"static world ("+std::string(sd.stage)+" "+sd.member+"): "+sd.message);
        if(!world->mesh())return refuse(d,step,0x274080u,"static world published no mesh");
        track.roles.staticWorld=std::move(world);
        return pass(d,step,track.counts.instances);
    }catch(const std::exception& e){return refuse(d,step,0x274080u,e.what());}
    catch(...){return refuse(d,step,0x274080u,"static world allocation failed");}
}

// ---------------------------------------------------------------- 05.15 tail
bool finishTail1AA400(Track& track,const LoadInputs&,Diagnostic& d)noexcept{
    constexpr auto step=log::Step::tail1AA400;
    try{
        // Member bytes only; the 2A1160 spline decode is step 08 (R4).
        const auto limit=course_archive_receipt50CI::maximumArchiveBytes();
        Diagnostic member;
        if(!track.identity.mapSpline.empty()&&track.archive.exists(track.identity.mapSpline)&&
           !track.archive.member(track.identity.mapSpline,track.documents.mapSpline,limit,member))
            return refuse(d,step,0x1AA400u,"map spline: "+member.reason);
        if(!track.identity.aiSpline.empty()&&track.archive.exists(track.identity.aiSpline)&&
           !track.archive.member(track.identity.aiSpline,track.documents.aiSpline,limit,member))
            return refuse(d,step,0x1AA400u,"AI spline: "+member.reason);
        // Tuning: in the course archive, the registry row's inherited stock archive, or the
        // row's tuning file. Absent tuning is reported, not invented.
        std::string tuningSource="none";
        if(!track.identity.tuning.empty()){
            if(track.archive.exists(track.identity.tuning)){
                if(!track.archive.member(track.identity.tuning,track.documents.tuning,limit,member))
                    return refuse(d,step,0x1AA400u,"tuning: "+member.reason);
                tuningSource="archive";
            }else if(track.source()==Source::registry){
                rr::game::dlc50DL::AddonCourseLoad row;const std::string code(track.code());
                if(rr::game::dlc50DL::addonCourseLoadByCode(code.c_str(),row)&&row.tuningInherited&&!row.tuningArchive.empty()){
                    Bytes stock;std::string why;a::ResArchive48C source;a::DecodeError48C error;
                    if(!readHostFile("work/native/project/disc/"+row.tuningArchive,stock,limit,why)||
                       !a::decodeResource48C(stock,source,error,nullptr,course_archive_receipt50CI::limits()))
                        return refuse(d,step,0x1AA400u,"inherited tuning archive: "+why+error.code);
                    for(const auto& e:source.entries)if(sameName(e.name,track.identity.tuning)){
                        if(!a::extractResMember48C(source,e.name,track.documents.tuning,error))
                            return refuse(d,step,0x1AA400u,"inherited tuning member: "+error.code);
                        tuningSource="inherited";break;}
                }
            }else{
                std::string why;
                if(readHostFile(track.identity.tuning,track.documents.tuning,limit,why))tuningSource="file";
            }
        }
        return pass(d,step,track.documents.tuning.size(),"tuning="+tuningSource);
    }catch(const std::exception& e){return refuse(d,step,0x1AA400u,e.what());}
    catch(...){return refuse(d,step,0x1AA400u,"tail allocation failed");}
}

// ---------------------------------------------------------------- the load (steps 04-05)
bool Load::run(Track& track,const Identity& identity,const std::shared_ptr<Provider>& provider,
    const std::shared_ptr<a::OriginalStandardTextureResolver50LA>& resolver,const LoadInputs& inputs,Diagnostic& d)noexcept{
    reached_=log::Step::archive12CEA0;pc_=0x12CEA0u;d={};
    if(track.loaded_||track.archive.opened())return refuse(d,log::Step::archive12CEA0,0x12CEA0u,"track already loaded");
    // A race passes all three (I2: &application, race, race->world74()); only the host data
    // check (runHostDataCheck, never a race) passes none.
    if(!inputs.hostDataCheck){
        if(!inputs.application||!inputs.race||!inputs.world)
            return refuse(d,log::Step::archive12CEA0,0x1A6D80u,"load inputs name no Application, race or World");
        if(inputs.world->track.get()!=&track)return refuse(d,log::Step::archive12CEA0,0x1A6D80u,"the race World does not hold this Track");
    }
    try{track.identity=identity;}catch(...){return refuse(d,log::Step::archive12CEA0,0x187330u,"identity copy failed");}
    if(!track.archive.open(identity,provider,resolver,d)){pc_=d.pc;return false;}
    // 1A9F68..1A9FA8 saves the model directory before the load; 1AA400.. restores it. The
    // host data check has no Application, so it has no model directory to save or restore
    // (Application-publishing part; no reader reads it).
    std::string savedModelDirectory;
    if(!inputs.hostDataCheck)
        try{savedModelDirectory=inputs.application->modelDirectory36B860_;}catch(...){return refuse(d,log::Step::trackLoad1A9EE0,0x1A9F68u,"model directory save failed");}
    using Reader=bool(*)(Track&,const LoadInputs&,Diagnostic&)noexcept;
    struct Sub {log::Step step;Reader read;};
    static constexpr Sub chain[]={
        {log::Step::scene270900,&readScene270900},
        {log::Step::tob1B4330,&readTob1B4330},
        {log::Step::physicsIndex270CDC,&buildPhysicsIndex270CDC},
        {log::Step::collision271230,&buildCollision271230},
        {log::Step::fog273C30,&readFog273C30},
        {log::Step::particles1B4DC0,&prepareParticles1B4DC0},
        {log::Step::instances274080,&readInstances274080},
        {log::Step::materials274160,&readMaterials274160},
        {log::Step::triggers274E90,&readTriggers274E90},
        {log::Step::water274570,&prepareWater274570},
        {log::Step::sky274AE0,&readSky274AE0},
        {log::Step::aub271600,&readAub271600},
        {log::Step::lights,&prepareLights},
        {log::Step::animation101860,&prepareAnimation101860},
        {log::Step::tail1AA400,&finishTail1AA400},
    };
    for(const auto& sub:chain){
        reached_=sub.step;pc_=log::stepRow(sub.step).pc;
        Diagnostic step;step.step=sub.step;step.pc=pc_;
        const bool ok=sub.read(track,inputs,step);
        if(!ok){
            if(!inputs.hostDataCheck)try{inputs.application->modelDirectory36B860_=savedModelDirectory;}catch(...){}
            d=std::move(step);if(!d.refused)d.refuse(sub.step,pc_,d.reason.empty()?"refused":d.reason);
            pc_=d.pc;
            try{log::load(log::Step::trackLoad1A9EE0,false,0,std::string(log::stepRow(sub.step).number)+" "+d.reason,nullptr,d.pc);}catch(...){}
            return false;
        }
    }
    if(!inputs.hostDataCheck)
        try{inputs.application->modelDirectory36B860_=savedModelDirectory;}catch(...){return refuse(d,log::Step::tail1AA400,0x1AA400u,"model directory restore failed");}
    track.loaded_=true;reached_=log::Step::tail1AA400;pc_=0x1AA508u;
    d={};d.step=log::Step::trackLoad1A9EE0;d.pc=0x1AA508u;d.value=track.archive.memberCount();
    return pass(d,log::Step::trackLoad1A9EE0,d.value);
}

// ---------------------------------------------------------------- track teardown and receipts
namespace {
template<class T> bool releaseRole(std::shared_ptr<T>& role,const char* name,bool& all)noexcept{
    if(!role)return true;
    const bool released=role.use_count()==1;
    if(released)role.reset();
    log::teardownRole(name,released);
    if(!released)all=false;
    return released;
}
}
bool Track::release()noexcept{
    bool all=true;
    // Reverse role order, teardown step 18.
    releaseRole(roles.navigation,"navigation",all);
    releaseRole(roles.sounds,"sounds",all);
    releaseRole(roles.audio,"aub",all);
    releaseRole(roles.water,"water",all);
    releaseRole(roles.waterResources,"water_resources",all);
    // The ground-effects owner emits into the particle manager: it stops emitting and
    // releases its placements before the manager (its pool) goes (W0 round 2, r7 #3).
    if(roles.emitters){
        bool released=false;
        if(roles.emitters.use_count()==1){
            const bool quiet=roles.emitters->quiesce();
            released=quiet&&roles.emitters->closeAfterPool();
            if(released)roles.emitters.reset();
        }
        log::teardownRole("particles.emitters",released);
        if(!released)all=false;
    }
    releaseRole(roles.particles,"particles",all);
    releaseRole(roles.sky,"sky",all);
    releaseRole(roles.lights,"lights",all);
    releaseRole(roles.fog,"fog",all);
    releaseRole(roles.staticWorld,"static_world",all);
    // The trigger action owner is borrowed by the collision world (StorageRequest.actions),
    // which closes it at its retirement: the role reference goes first (W0 round 2, r3 #3 /
    // r5 #2), released=1 meaning the Track no longer holds it.
    if(roles.triggerActions){roles.triggerActions.reset();log::teardownRole("trigger_actions",true);}
    // The storage role is an alias of the collision world (same control block): dropping
    // the alias releases nothing by itself; the collision world's close() below decides.
    if(roles.storage){roles.storage.reset();log::teardownRole("storage",true);}
    if(roles.collision){
        auto& world=*roles.collision;bool closed=false;
        if(roles.collision.use_count()==1){
            using P=track_collision_world50CR::Phase;
            if(world.phase()!=P::Closed&&world.phase()!=P::Retiring&&world.phase()!=P::Empty&&world.phase()!=P::Ground)world.beginRetirement();
            if(world.phase()==P::Retiring)world.retireContacts();
            closed=world.close();
        }
        if(closed)roles.collision.reset();
        log::teardownRole("collisions",closed);
        if(!closed)all=false;
    }
    // R5 order after the action owner closed: the group, its rows, then ATM (its GPU side was
    // closed by I3 first), UV animation, animation (the collision world's animation lane
    // borrowed it until its close above).
    releaseRole(roles.triggers,"triggers",all);
    releaseRole(roles.triggerSource,"trigger_source",all);
    releaseRole(roles.atm,"atm",all);
    releaseRole(roles.uvAnimation,"uv_animation",all);
    releaseRole(roles.animation,"animation",all);
    if(!all){log::teardownRole("archive",false);return false;}
    if(archive.opened()&&!archive.release())return false;
    loaded_=false;
    return true;
}
Track::~Track(){
    if(loaded_||archive.opened())release();
}
void Track::logTrackData()const{
    const auto code=this->code();
    const auto line=[&](log::Role role,std::string_view file,std::uint64_t value){
        const auto* r=stockReceipt(*this,log::roleName(role));
        log::trackData(role,code,file,value,r!=nullptr,r?r->value:0);
    };
    const auto named=[&](std::string_view role,std::string_view file,std::uint64_t value){
        const auto* r=stockReceipt(*this,role);
        log::trackDataNamed(role,code,file,value,r!=nullptr,r?r->value:0);
    };
    line(log::Role::fog,identity.scbMember,roles.fog?1u:0u);
    line(log::Role::lights,identity.scbMember,counts.lights);
    // role=cuts is printed by the camera factory (R2) with its receipt; not here.
    line(log::Role::triggers,identity.tobMember,counts.triggers);
    line(log::Role::nav_nodes,identity.mapSpline,counts.navMapNodes);
    line(log::Role::ai_nodes,identity.aiSpline,counts.navAiNodes);
    line(log::Role::lap_markers,identity.mapSpline,counts.lapMarkers);
    line(log::Role::zones,identity.scbMember,counts.zones);
    named("zones.portals",identity.scbMember,counts.portals);
    line(log::Role::instances,identity.scbMember,counts.instances);
    line(log::Role::collisions,identity.scbMember,counts.collisions);
    line(log::Role::storage,identity.scbMember,counts.storageBytes);
    line(log::Role::water,documents.waterName,counts.water?1u:0u);
    line(log::Role::particles,identity.scbMember,counts.particleEmitters);
    line(log::Role::sky,identity.scbMember,counts.skyCubes);
    line(log::Role::aub,identity.aubMember,counts.aubRows);
    if(roles.atm)roles.atm->logTrackData(code,source());else line(log::Role::atm,identity.tobMember,0u);
    line(log::Role::materials,identity.scbMember,counts.materials);
    line(log::Role::archive,identity.archivePath,archive.bytes());
    const auto* hash=source()==Source::stock?receipts::findHash(code,"archive_sha256"):nullptr;
    log::trackDataNamed("archive_sha256",code,archive.sha256(),1u,hash!=nullptr,hash&&hash->sha256==archive.sha256()?1u:0u);
}


// ---------------------------------------------------------------- host data check (PLAN 4.3)
// W0 round 2 (r11 #1): the entry R11's track_check.cpp calls. No Application, race or World:
// a check-local standard material domain builds the archive provider / texture resolver pair
// through prepareFreshNativeService (the 12CBA0 fresh file capacity included, so the race
// mount lease and its 1316A0 release behave as in a race), then the SAME Load::run with
// hostDataCheck=true, step 08 readNavigation2A1260 on the check's one libm state, and
// logTrackData. bindNavigation1ACF70 needs the collision world's live storage (after the
// roster, step 10) and is not part of the check.
bool runHostDataCheck(Track& track,const Identity& identity,Diagnostic& d)noexcept{
    d={};
    constexpr auto step=log::Step::archive12CEA0;
    try{
        // PORT STAND-IN: host data check only. The domain is prepared through its public
        // standalone route (prepareFresh: prepareFreshNativeService, APPSTART custody, tables)
        // because prepareFreshNativeService itself is reachable only from the Application; the
        // race's Application runs the 17CAB8/17CAD4/17CAE8 order instead. Never used in a race.
        const auto* appStartIdentity=a::originalUiArchiveIdentity50KG(a::OriginalUiArchiveId50KG::appStart);
        if(!appStartIdentity||!appStartIdentity->discPath)return refuse(d,step,0x12CBA0u,"host check: APPSTART catalog row missing");
        Bytes appStartBytes;std::string why;
        if(!readHostFile(std::string("work/native/project/disc/")+appStartIdentity->discPath,appStartBytes,appStartIdentity->bytes,why))
            return refuse(d,step,0x12CBA0u,"host check: APPSTART read: "+why);
        auto appStart=std::make_shared<a::OriginalUiArchive50KG>();a::OriginalUiArchiveDiagnostic50KG ad;
        if(!appStart->loadOwned(a::OriginalUiArchiveId50KG::appStart,std::move(appStartBytes),ad))
            return refuse(d,step,0x12CBA0u,"host check: APPSTART authentication rejected status="+std::to_string(unsigned(ad.status)));
        auto materialApplication=std::make_shared<a::OriginalUiMaterialApplication50LA>();a::OriginalUiMaterialDiagnostic50KG ud;
        if(!materialApplication->prepareFreshApplication(appStart,ud))
            return refuse(d,step,0x12CBA0u,"host check: APPSTART material application rejected");
        auto domain=std::make_shared<a::OriginalStandardMaterialDomain50LA>();a::OriginalStandardMaterialDiagnostic50LA md;
        if(!domain->prepareFresh(materialApplication,md))
            return refuse(d,step,0x12CBA0u,"host check: standard material domain rejected status="+std::to_string(unsigned(md.status)));
        const auto provider=domain->archiveProvider3E1A70();const auto resolver=domain->resolver3E1A70();
        if(!provider||!resolver)return refuse(d,step,0x12CBA0u,"host check: provider/resolver pair unavailable");
        // The check's ONE shared animation state; its libm is the one serial libm owner of
        // both 05.14 and step 08, as s.animationShared is in a race.
        auto shared=std::make_unique<rr::game::VehicleAnimationShared49T>();
        LoadInputs inputs;
        inputs.hostDataCheck=true;
        inputs.entities=std::make_shared<rr::game::OriginalApplicationEntities50LA>();
        inputs.materials382B38=domain->materials382B38();
        // PORT STAND-IN: host data check only. No race field exists; the full field (1AD520
        // admits 1..8) so every per-racer size takes its race maximum. Generation 1 = one session.
        inputs.racers=8u;
        inputs.generation=1u;
        inputs.audioRoot=runtime_asset_paths50LB::resolve("work/native/project/disc/AUDIO").string();
        inputs.animationShared=shared.get();
        Load load;
        if(!load.run(track,identity,provider,resolver,inputs,d))return false;
        Diagnostic navigation;
        if(!readNavigation2A1260(track,shared->libm,navigation)){
            d=std::move(navigation);
            if(!d.refused)d.refuse(log::Step::navigation2A1260,log::stepRow(log::Step::navigation2A1260).pc,d.reason.empty()?"refused":d.reason);
            return false;
        }
        track.logTrackData();
        return true;
    }catch(const std::exception& e){return refuse(d,step,0x12CBA0u,std::string("host check: ")+e.what());}
    catch(...){return refuse(d,step,0x12CBA0u,"host check allocation failed");}
}

} // namespace common_track50CR

// ---------------------------------------------------------------- collision world lifetime
namespace track_collision_world50CR {
Owner::~Owner(){if(!thread()||!retire())std::terminate();}
}

// ---------------------------------------------------------------- the Application entry
namespace original_main_entry50KG {
bool Application::loadConsumedRaceTrack1A9EE0(rr::game::OriginalRaceConstructor50LA& race,common_track50CR::Track& track,
    const common_track50CR::Identity& identity,const common_track50CR::LoadInputs& inputs,common_track50CR::Diagnostic& d)const noexcept{
    namespace ct=common_track50CR;
    d={};
    if(inputs.hostDataCheck||inputs.race!=&race||inputs.application!=this){
        d.refuse(ct::log::Step::trackLoad1A9EE0,0x1A6D80u,"load inputs name another race or Application");
        ct::log::load(ct::log::Step::trackLoad1A9EE0,false,0,d.reason,nullptr,0x1A6D80u);return false;}
    std::shared_ptr<rr::assets::OriginalStandardArchiveProvider50LA> provider;
    std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> resolver;
    try{
        const auto domain=standardMaterialDomainOwner_;
        if(domain){provider=domain->archiveProvider3E1A70();resolver=domain->resolver3E1A70();}
        // 1A9FB0..1A9FE0: both original branches set the track directory literal.
        trackDirectory36B640_="tracks\\";
    }catch(...){}
    if(!provider||!resolver){
        d.refuse(ct::log::Step::archive12CEA0,0x12CEA0u,"standard material domain 3E1A70 provider/resolver unavailable");
        ct::log::load(ct::log::Step::archive12CEA0,false,0,d.reason);return false;}
    ct::Load load;
    return load.run(track,identity,provider,resolver,inputs,d);
}
} // namespace original_main_entry50KG
