#include "vehicle_window_gl50ds.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_model_resource50kg.h"
#include "original_ui_garage_lowering_proof50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include <algorithm>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <cstring>
#include <functional>
#include <mutex>
#include <vector>
#include <xmmintrin.h>
#include <limits>
#include <thread>
#include <utility>

namespace rr::platform {
bool loweringSerialOnly50KG()noexcept;
namespace {
// Host worker pool for the pure per-triangle numeric pass (never source state).
// Workers run the SAME projection functions on contiguous triangle ranges with
// the caller's SSE control word (rounding, FTZ/DAZ); the caller then appends
// every triangle's packet in source order and applies the unchanged per-
// triangle capacity check and first-failure report exactly as the serial loop.
class LoweringPool50KG {
public:
    static LoweringPool50KG* instance()noexcept{
        static LoweringPool50KG* pool=[]()->LoweringPool50KG*{
            const char* v=std::getenv("RR_GARAGE_LEGACY_PARALLEL");
            if(v&&v[0]=='1'&&!v[1])return nullptr;
            // Up to 11 workers (12 threads with the caller), leaving cores for
            // the driver, audio and window threads on the target 16-thread host.
            unsigned n=std::thread::hardware_concurrency();n=n>1?std::min(n-1,11u):0u;
            if(!n)return nullptr;
            try{return new LoweringPool50KG(n);}catch(...){return nullptr;}
        }();
        return pool;
    }
    unsigned workers()const noexcept{return static_cast<unsigned>(threads_.size());}
    // Runs job(chunk) for chunk in [0,count), on the workers and this thread.
    void run(unsigned count,const std::function<void(unsigned)>& job){
        {std::lock_guard<std::mutex> lock(mutex_);job_=&job;count_=count;next_=0;finished_=0;++generation_;}
        wake_.notify_all();
        work();
        std::unique_lock<std::mutex> lock(mutex_);
        done_.wait(lock,[&]{return finished_==count_;});
        job_=nullptr;
    }
private:
    explicit LoweringPool50KG(unsigned n){
        for(unsigned i=0;i<n;++i)threads_.emplace_back([this]{loop();});
    }
    void loop(){
        unsigned seen=0;
        for(;;){
            {std::unique_lock<std::mutex> lock(mutex_);wake_.wait(lock,[&]{return generation_!=seen;});seen=generation_;}
            work();
        }
    }
    void work(){
        for(;;){
            unsigned chunk;const std::function<void(unsigned)>* job;
            {std::lock_guard<std::mutex> lock(mutex_);if(!job_||next_>=count_)return;chunk=next_++;job=job_;}
            (*job)(chunk);
            {std::lock_guard<std::mutex> lock(mutex_);if(++finished_==count_)done_.notify_all();}
        }
    }
    std::vector<std::thread> threads_;
    std::mutex mutex_;std::condition_variable wake_,done_;
    const std::function<void(unsigned)>* job_=nullptr;
    unsigned count_=0,next_=0,finished_=0,generation_=0;
};
bool loweringParallelShadow50KG()noexcept{
    static const bool on=[]{const char* v=std::getenv("RR_GARAGE_PROOF_SHADOW");return v&&v[0]=='1'&&!v[1];}();
    return on;
}
[[noreturn]] void loweringParallelMismatch50KG()noexcept{
    std::fprintf(stderr,"garage_parallel_lowering_shadow MISMATCH\n");std::fflush(stderr);std::terminate();
}
void loweringParallelCompared50KG(std::uint32_t triangles)noexcept{
    static unsigned long long total=0,surfaces=0;total+=triangles;
    if(!(++surfaces%500))std::fprintf(stderr,"garage_parallel_lowering_shadow surfaces=%llu triangles=%llu mismatches=0\n",surfaces,total);
}
template<class Diag> struct LoweringChunk50KG {
    std::uint32_t first=0,last=0,failedAt=0;bool failed=false,allocationFailed=false;
    Diag failure{};
    std::vector<std::uint32_t> quads;
    std::vector<assets::Vertex48B> vertices;std::vector<std::array<float,3>> stq;std::vector<float> rasterW,fog;
};
}
bool OriginalUiGarageDrawContribution50KG::lowerQueuedModel24B26C(
    std::size_t ordinal,const OriginalUiGarageModelLowering50KG::Context& context,
    OriginalUiGarageOwnedModelGeometry50KG& out,assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    using S=assets::OriginalStandardMaterialStatus50LA;
    using Lower=OriginalUiGarageModelLowering50KG;
    using Program=assets::OriginalUiGarageProgram50KG;
    d={};const auto fail=[&](S status,std::uint32_t pc,const char* field){
        d.status=status;d.sourcePC=pc;d.field=field;return false;
    };
    if(!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||sourcePC_!=0x24b26cu||
       !publication_||!renderer_||!renderer_->currentFor(issuer_)||
       !renderer_->ownsRenderPrelude(*this,publication_)||!cameraFrame_.current())
        return fail(S::phase,0x24b26c,"actual queued-model drain invocation");
    const auto fifo=issuer_->nativeCollectorFifoOwner_;
    if(!fifo||fifo->application_!=issuer_||fifo->closed_||!fifo->draining_||!fifo->uiConstructor1B73BC_||
       fifo->ownerThread_!=std::this_thread::get_id()||!fifo->table_||
       !fifo->table_->coldCollectorCurrent(fifo->registry_)||ordinal>=fifo->records_.size())
        return fail(S::collector,0x24b26c,"same live FIFO and actual record ordinal");
    auto it=fifo->records_.cbegin();for(std::size_t i=0;i<ordinal;++i)++it;
    const auto& record=*it;
    if(record.garageDraw.get()!=this||record.garagePublication!=publication_||record.epoch!=fifo->epoch04_||
       !record.garageModel||record.garageText||record.primitive04||record.camera||record.allocation||
       record.material||record.display||record.bank||record.cables)
        return fail(S::collector,0x24b26c,"actual model record and publication");
    const auto& model=*record.garageModel;
    const auto revision=model.nativeGaragePacketRevision();
    if(!model.current()||!model.nativeGarageFifoPins_||!revision||record.matrixRevision!=revision||
       model.nativeSharedSelf_.lock().get()!=&model||model.application_!=issuer_->entitiesOwner_||
       model.domain_!=issuer_->standardMaterialDomainOwner_||!model.domain_||
       model.domain_->materials382B38()!=fifo->table_||!model.geometry_||
       !model.geometry_->holdsModelReference(&model.scene_)||
       record.lod!=static_cast<std::size_t>(model.scene_.fields().field1B0)||
       record.lod>std::numeric_limits<std::uint32_t>::max()||
       record.surface>std::numeric_limits<std::uint32_t>::max()||
       record.packetIndex>std::numeric_limits<std::uint32_t>::max())
        return fail(S::phase,0x24b4a0,"same retained model, packet revision and LOD");
    const auto* surface=model.geometry_->surface246300(record.lod,record.surface);
    const auto* packet=model.packets_.packet(record.packetIndex);
    if(!surface||surface!=record.surface04||!surface->material00||*surface->material00<0||!packet||
       record.attributeArray1F0||record.attributes08!=packet||
       (model.geometry_->hasPalette20()?record.packetIndex!=surface->palette0E:record.packetIndex!=0u))
        return fail(S::source,0x24b4bc,"actual runtime Surface04 and direct attributes08");
    // The actual fast24178C record names FIRST Surface once and the sole
    // non-palette packet. There is no invented surface-count expansion here.
    if(model.scene_.fields().field1C8&&(model.geometry_->hasPalette20()||record.surface!=0u||
       model.packets_.count()!=1u||model.nativeCollector1C8Owner_.lock()!=fifo||
       model.nativeCollector1C8Registration_!=record.originRegistration||
       !fifo->table_->coldCollectorCurrent(model.nativeCollector1C8Receipt_)))
        return fail(S::collector,0x24178c,"real non-palette first-Surface cache record");
    assets::OriginalStandardMaterialCollectorEntryView50LA origin,final;
    if(record.originRegistration<0||record.finalRegistration<0||
       !fifo->table_->coldCollectorEntry(fifo->registry_,static_cast<std::size_t>(record.originRegistration),origin)||
       !fifo->table_->coldCollectorEntry(fifo->registry_,static_cast<std::size_t>(record.finalRegistration),final)||
       origin.registration!=record.originRegistration||origin.finalLeaf!=record.finalRegistration||
       final.registration!=record.finalRegistration||final.finalLeaf!=record.finalRegistration||final.redirect18!=-1||
       !origin.material||!final.material||origin.pass<0||final.pass<0)
        return fail(S::collector,0x24b26c,"actual canonical origin and final leaf");
    d.row=origin.materialRow;d.pass=static_cast<std::size_t>(origin.pass);
    const auto* handle=model.materials_.handle1C4(static_cast<std::size_t>(*surface->material00));
    if(!handle||(*handle==-1?*surface->material00:*handle)!=origin.materialRow)
        return fail(S::reference,0x2417d8,"actual Surface material override");
    assets::OriginalStandardMaterialPassView50LA originPass,finalPass;
    if(!origin.material->pass4C(static_cast<std::size_t>(origin.pass),originPass)||
       !final.material->pass4C(static_cast<std::size_t>(final.pass),finalPass)||
       record.originParameters!=originPass.parameters34||record.textureBase!=originPass.textureAnimation10||
       record.vertexBase||originPass.animation10||!originPass.parameterCount28||!originPass.parameterMask24)
        return fail(S::source,0x24b4a0,"retained originating pass operands");
    // Nonzero origin VU parameter overlays and vertex animation require their
    // own numeric source consumer. They cannot be silently replaced by the
    // final leaf's parameters or by the unmodified common packet rows.
    if(*originPass.parameterCount28!=0||*originPass.parameterMask24!=0||
       (record.originParameters&&!record.originParameters->empty()))
        return fail(S::unsupportedPass,0x24b4e4,"originating VU parameter overlay");
    const auto& programs=model.domain_->programs382B70();
    assets::OriginalStandardProgramRowView50LA program;
    if(!programs||!programs->prepared()||!finalPass.program18||
       !programs->row(*finalPass.program18,program)||!program.canonicalName)
        return fail(S::program,0x24b1c0,"actual final-leaf program18");
    Program family;
    if(std::strcmp(program.canonicalName,"PrelitCompressed")==0)family=Program::prelitCompressed;
    else if(std::strcmp(program.canonicalName,"Diffuse")==0)family=Program::diffuse;
    else if(std::strcmp(program.canonicalName,"DiffuseCompressed")==0)family=Program::diffuseCompressed;
    else if(std::strcmp(program.canonicalName,"SpecularMapCompressed")==0)family=Program::specularMapCompressed;
    else return fail(S::unsupportedPass,0x24b1c0,"unimplemented final-leaf model program");
    try {
        auto raw=std::shared_ptr<OriginalUiGarageRawSurface50KG>(new OriginalUiGarageRawSurface50KG());
        OriginalUiGarageRawDiagnostic50KG rawDiagnostic;
        if(!raw->prepare(record.garageModel,static_cast<std::uint32_t>(record.lod),
           static_cast<std::uint32_t>(record.surface),rawDiagnostic)) {
            d.slot=rawDiagnostic.sourceOffset;
            return fail(rawDiagnostic.status==OriginalUiGarageRawStatus50KG::allocation?S::allocation:S::source,
                rawDiagnostic.sourcePC,"actual raw Surface stream (slot is source byte offset)");
        }
        OriginalUiGarageRawSurface50KG::Metadata metadata;
        if(!raw->describe(metadata,rawDiagnostic)||metadata.runtimeSurface!=surface||
           metadata.materialSlot!=static_cast<std::uint32_t>(*surface->material00))
            return fail(S::source,rawDiagnostic.sourcePC,"same retained raw/runtime Surface identity");
        Lower lower;Lower::Diagnostic numeric;
        const auto lowerFailure=[&]() {
            const char* field="model numeric owner/packet";S status=S::source;
            if(numeric.status==Lower::Status::program){field="distinct model program unavailable";status=S::unsupportedPass;}
            else if(numeric.status==Lower::Status::numeric){
                switch(numeric.numeric){
                case game::garage_prelit50KG::Status::input:field="model triangle numeric input";status=S::input;break;
                case game::garage_prelit50KG::Status::arithmetic:field="model triangle arithmetic";status=S::arithmetic;break;
                case game::garage_prelit50KG::Status::unsupportedPacketRange:field="model triangle packet range";status=S::range;break;
                case game::garage_prelit50KG::Status::capacity:field="model triangle clip capacity";status=S::overflow;break;
                case game::garage_prelit50KG::Status::complete:break;
                }
            }else if(numeric.status==Lower::Status::raw){field="model triangle raw source";status=S::source;}
            return fail(status,numeric.sourcePC,field);
        };
        if(!lower.prepare(raw,static_cast<std::uint32_t>(record.packetIndex),*packet,family,context,numeric))
            return lowerFailure();
        OriginalUiGarageOwnedModelGeometry50KG next;
        if(!lower.readTriangleCount(next.sourceTriangleCount)||next.sourceTriangleCount!=metadata.triangleCount||
           !lower.current()||!raw->current())
            return fail(S::phase,0x24b4a0,"actual complete triangle extent");
        // Pure numeric pass bracketed by the full checks above and below.
        const OriginalUiGarageLoweringProof50KG proof(&lower,raw.get());
        {   // Capacity only: one unclipped triangle is one four-vertex unit.
            const std::size_t expected=std::min<std::size_t>(std::size_t(next.sourceTriangleCount)*4u,
                std::size_t(originalUiMaximumSourceQuads50KM)*4u);
            next.vertices.reserve(expected);next.stq.reserve(expected);
            next.rasterW.reserve(expected);next.fog.reserve(expected);
        }
        auto* pool=next.sourceTriangleCount>=256u&&!loweringSerialOnly50KG()?LoweringPool50KG::instance():nullptr;
        if(pool){
            using Chunk=LoweringChunk50KG<Lower::Diagnostic>;
            // Up to three chunks per thread (at least 64 triangles each), claimed
            // dynamically, so clipped/cheap ranges do not leave threads idle.
            // Chunking never changes output: the merge below is in source order.
            const unsigned count=std::min<unsigned>(3u*(pool->workers()+1u),(next.sourceTriangleCount+63u)/64u);
            std::vector<Chunk> chunks(count);
            for(unsigned c=0;c<count;++c){
                chunks[c].first=static_cast<std::uint32_t>(std::uint64_t(next.sourceTriangleCount)*c/count);
                chunks[c].last=static_cast<std::uint32_t>(std::uint64_t(next.sourceTriangleCount)*(c+1)/count);
                const std::size_t n=chunks[c].last-chunks[c].first;
                chunks[c].quads.reserve(n);chunks[c].vertices.reserve(n*4);chunks[c].stq.reserve(n*4);
                chunks[c].rasterW.reserve(n*4);chunks[c].fog.reserve(n*4);
            }
            const unsigned csr=_mm_getcsr();
            const std::function<void(unsigned)> job=[&,csr](unsigned c){
                const unsigned saved=_mm_getcsr();_mm_setcsr(csr);
                // The same bracket proof as the caller's: it is held across this run.
                const OriginalUiGarageLoweringProof50KG workerProof(&lower,raw.get());
                auto& k=chunks[c];
                try{
                for(std::uint32_t triangle=k.first;triangle<k.last;++triangle){
                    OriginalUiGaragePrelitPacket50KG polygon;Lower::Diagnostic diagnostic;
                    if(!lower.projectTriangle(triangle,polygon,diagnostic)){k.failed=true;k.failedAt=triangle;k.failure=diagnostic;break;}
                    const auto quads=std::min<std::uint32_t>(polygon.quadCount,7u);
                    const auto count=quads*4u;
                    k.quads.push_back(polygon.quadCount);
                    k.vertices.insert(k.vertices.end(),polygon.vertices.begin(),polygon.vertices.begin()+count);
                    k.stq.insert(k.stq.end(),polygon.stq.begin(),polygon.stq.begin()+count);
                    k.rasterW.insert(k.rasterW.end(),polygon.rasterW.begin(),polygon.rasterW.begin()+count);
                    k.fog.insert(k.fog.end(),polygon.fog.begin(),polygon.fog.begin()+count);
                }
                }catch(...){k.allocationFailed=true;}
                _mm_setcsr(saved);
            };
            pool->run(count,job);
            // Source-order merge with the serial loop's exact checks and reports.
            for(const auto& k:chunks)if(k.allocationFailed)
                return fail(S::allocation,0x24b26c,"queued model geometry allocation");
            for(const auto& k:chunks){
                std::size_t at=0;
                for(std::uint32_t triangle=k.first;triangle<k.last;++triangle){
                    d.slot=triangle;
                    if(k.failed&&triangle==k.failedAt){numeric=k.failure;return lowerFailure();}
                    const auto quads=k.quads[triangle-k.first];
                    if(quads>originalUiMaximumSourceQuads50KM-next.quadCount)
                        return fail(S::overflow,0x24b50c,"unchanged whole-frame model geometry capacity");
                    if(!quads)++next.clippedTriangleCount;
                    const auto count=std::size_t(quads)*4u;
                    next.vertices.insert(next.vertices.end(),k.vertices.begin()+at,k.vertices.begin()+at+count);
                    next.stq.insert(next.stq.end(),k.stq.begin()+at,k.stq.begin()+at+count);
                    next.rasterW.insert(next.rasterW.end(),k.rasterW.begin()+at,k.rasterW.begin()+at+count);
                    next.fog.insert(next.fog.end(),k.fog.begin()+at,k.fog.begin()+at+count);
                    next.quadCount+=quads;at+=count;
                }
            }
            if(loweringParallelShadow50KG()){
                // Off by default (RR_GARAGE_PROOF_SHADOW=1): the serial pass on this
                // thread must reproduce every merged stream byte for byte.
                std::vector<assets::Vertex48B> v;std::vector<std::array<float,3>> q;std::vector<float> w,f;
                std::uint32_t quads=0,clipped=0;
                for(std::uint32_t triangle=0;triangle<next.sourceTriangleCount;++triangle){
                    OriginalUiGaragePrelitPacket50KG polygon;Lower::Diagnostic diagnostic;
                    if(!lower.projectTriangle(triangle,polygon,diagnostic))loweringParallelMismatch50KG();
                    if(!polygon.quadCount)++clipped;
                    const auto count=polygon.quadCount*4u;
                    v.insert(v.end(),polygon.vertices.begin(),polygon.vertices.begin()+count);
                    q.insert(q.end(),polygon.stq.begin(),polygon.stq.begin()+count);
                    w.insert(w.end(),polygon.rasterW.begin(),polygon.rasterW.begin()+count);
                    f.insert(f.end(),polygon.fog.begin(),polygon.fog.begin()+count);
                    quads+=polygon.quadCount;
                }
                if(quads!=next.quadCount||clipped!=next.clippedTriangleCount||v.size()!=next.vertices.size()||
                   (!v.empty()&&(std::memcmp(v.data(),next.vertices.data(),v.size()*sizeof(v[0]))||
                    std::memcmp(q.data(),next.stq.data(),q.size()*sizeof(q[0]))||
                    std::memcmp(w.data(),next.rasterW.data(),w.size()*sizeof(float))||
                    std::memcmp(f.data(),next.fog.data(),f.size()*sizeof(float)))))loweringParallelMismatch50KG();
                loweringParallelCompared50KG(next.sourceTriangleCount);
            }
        }else
        for(std::uint32_t triangle=0;triangle<next.sourceTriangleCount;++triangle){
            d.slot=triangle;OriginalUiGaragePrelitPacket50KG polygon;
            if(!lower.projectTriangle(triangle,polygon,numeric))return lowerFailure();
            if(polygon.quadCount>originalUiMaximumSourceQuads50KM-next.quadCount)
                return fail(S::overflow,0x24b50c,"unchanged whole-frame model geometry capacity");
            if(!polygon.quadCount)++next.clippedTriangleCount;
            const auto count=polygon.quadCount*4u;
            next.vertices.insert(next.vertices.end(),polygon.vertices.begin(),polygon.vertices.begin()+count);
            next.stq.insert(next.stq.end(),polygon.stq.begin(),polygon.stq.begin()+count);
            next.rasterW.insert(next.rasterW.end(),polygon.rasterW.begin(),polygon.rasterW.begin()+count);
            next.fog.insert(next.fog.end(),polygon.fog.begin(),polygon.fog.begin()+count);
            next.quadCount+=polygon.quadCount;
        }
        if(!lower.current()||!raw->current()||model.nativeGaragePacketRevision()!=record.matrixRevision||
           !model.nativeGarageFifoPins_||record.epoch!=fifo->epoch04_||
           next.vertices.size()!=std::size_t(next.quadCount)*4u||next.stq.size()!=next.vertices.size()||
           next.rasterW.size()!=next.vertices.size()||next.fog.size()!=next.vertices.size())
            return fail(S::phase,0x24b50c,"retained model and parallel output streams");
        next.recordOrdinal=ordinal;next.recordIdentity=&record;next.publication=publication_;
        next.epoch=record.epoch;next.matrixRevision=record.matrixRevision;
        next.lod=record.lod;next.surface=record.surface;next.packetIndex=record.packetIndex;
        next.materialSlot=metadata.materialSlot;next.originRegistration=record.originRegistration;
        next.finalRegistration=record.finalRegistration;next.program=family;
        // Includes the genuine all-clipped empty case only after EVERY source
        // triangle succeeded. Failure above leaves the caller's output intact.
        out=std::move(next);d.status=S::complete;d.sourcePC=0x24b50c;d.slot=ordinal;d.field=nullptr;return true;
    }catch(...){return fail(S::allocation,0x24b26c,"queued model geometry allocation");}
}
}
