// Reuse the retained test-only original-word fixture reader and body comparator.
#define main retained_response48l_main
#include "spring_response48l.cpp"
#undef main
#include "static_contact_response50e.h"

int main(int argc,char**argv){try{
    if(argc!=2)throw std::runtime_error("original response fixture required");
    std::ifstream input(argv[1]);Json corpus;input>>corpus;
    unsigned admitted=0,skipped=0;
    for(const auto& j:corpus.at("cases")){
        const auto f=parse(j);bool eligible=!f.parameters.pointContacts48&&f.pointRecords.empty()&&!f.bodies.empty();
        StaticResponseScene50E scene{{73},{}};bool haveScene=false;
        for(const auto& rec:f.records){
            if(rec.otherBodyIdentity44)eligible=false;
            if(rec.hasScene40){
                if(haveScene&&std::memcmp(&scene.world,&rec.scene40,sizeof scene.world))eligible=false;
                std::memcpy(&scene.world,&rec.scene40,sizeof scene.world);haveScene=true;
            }
        }
        if(!eligible){++skipped;continue;}++admitted;label=j.at("name").get<std::string>();
        const auto n=f.records.size(),slots=n+2;
        std::vector<SpringRecord48N> records(slots),out(slots+1);
        std::memset(records.data(),0xa5,slots*sizeof(records[0]));
        std::memset(out.data(),0x5a,out.size()*sizeof(out[0]));
        const std::uint32_t opaque[4]={0x7f800123u,0xff800456u,0x80000000u,0x7fc00123u};
        for(auto& rec:records){std::memcpy(rec.field00.v,opaque,sizeof opaque);
            std::memcpy(&rec.localPoint10.v[3],opaque,4);std::memcpy(&rec.normal20.v[3],opaque+1,4);std::memcpy(&rec.surfacePoint30.v[3],opaque+2,4);}
        for(std::size_t i=0;i<n;++i){const auto&r=f.records[i];auto&d=records[i];
            std::memcpy(d.localPoint10.v,&r.localPoint10,12);std::memcpy(d.normal20.v,&r.normal20,12);std::memcpy(d.surfacePoint30.v,&r.surfacePoint30,12);
            d.scene40.value=r.hasScene40?scene.identity.value:0;d.other44.value=0;d.material4C=r.material4C;d.flags50=r.flags50;
        }
        SpringRecordState48N stateN{f.initial.pending38,f.initial.contact3C,f.initial.upward40,f.initial.other44,
            f.initial.point74,f.initial.generated78,f.initial.field130,17,static_cast<int>(n),static_cast<int>(slots),91};
        std::vector<SpringGeneratedRecord48K> projection(n);
        std::vector<std::uint8_t> scratchFlags(n),preparedFlags(n);
        SpringModelBody48K scratch{},prepared{};
        StaticResponseWorkspace50E work{projection.data(),n,{&scratch,1,scratchFlags.data(),n},{&prepared,1,preparedFlags.data(),n}};
        StaticResponseResult50E result{};
        watchAllocations=true;const auto status=respondStaticGenerated50E(f.parameters,f.response,stateN,f.bodies[0],scene,
            records.data(),slots,work,out.data(),out.size(),result);watchAllocations=false;
        check(status==StaticResponseStatus50E::Complete,"adapter complete");check(observedAllocations==0,"no hot allocation");
        const auto&e=j.at("expected");
        const SpringGeneratedState48K got{result.records.pending38,result.records.contact3C,result.records.upward40,result.records.other44,
            result.records.point74,result.records.generated78,result.records.currentCandidate130};
        check(stateWords(got)==stateWords(state(e.at("state"))),"original state words");
        for(unsigned k=0;k<3;++k){check(bits(result.worldCenter110.v[k])==e.at("center").at(k).get<std::uint32_t>(),"original center");check(bits(result.contactVelocity120.v[k])==e.at("velocity").at(k).get<std::uint32_t>(),"original contact velocity");}
        const auto bodyWords=outputWords(result.primary.body);
        for(unsigned k=0;k<24;++k)check(bodyWords[k]==j.at("bodies").at(0).at("expected").at(k).get<std::uint32_t>(),"original body result");
        checkUnchanged(f.bodies[0],result.primary);
        for(std::size_t i=0;i<slots;++i){SpringRecord48N expected;std::memcpy(&expected,&records[i],sizeof expected);if(i<n)expected.flags50=e.at("flags").at(i).get<std::uint8_t>();check(std::memcmp(&expected,&out[i],sizeof expected)==0,"record payload and stale slots preserved");}
        SpringRecord48N tail;std::memset(&tail,0x5a,sizeof tail);check(std::memcmp(&tail,&out.back(),sizeof tail)==0,"destination tail unchanged");
        check(result.records.pointCount7C==17&&result.records.generatedCount84==static_cast<int>(n)&&result.records.generatedCapacity88==static_cast<int>(slots)&&result.records.lastMaterialD4==91,"metadata preserved");
        const auto saved=out;StaticResponseResult50E savedResult;std::memcpy(&savedResult,&result,sizeof result);
        for(unsigned trial=0;trial<8;++trial){auto w=work;auto p=f.parameters;auto st=stateN;auto sc=scene;auto*dest=out.data();auto cap=out.size();
            auto want=StaticResponseStatus50E::InvalidInput;
            switch(trial){
            case 0:p.pointContacts48=true;want=StaticResponseStatus50E::UnsupportedScope;break;
            case 1:st.generatedCount84=-1;break;
            case 2:w.scratch.bodyCapacity=0;want=StaticResponseStatus50E::InsufficientCapacity;break;
            case 3:cap=slots-1;want=StaticResponseStatus50E::InsufficientCapacity;break;
            case 4:dest=records.data();break;
            case 5:w.prepared.bodies=&scratch;break;
            case 6:w.projected=reinterpret_cast<SpringGeneratedRecord48K*>(out.data());w.projectedCapacity=1; if(n>1)want=StaticResponseStatus50E::InsufficientCapacity;break;
            case 7:w.scratch.bodyCapacity=std::numeric_limits<std::size_t>::max();break;
            }
            watchAllocations=true;const auto rejected=respondStaticGenerated50E(p,f.response,st,f.bodies[0],sc,records.data(),slots,w,dest,cap,result);watchAllocations=false;
            check(rejected==want,"rejection status");check(std::memcmp(out.data(),saved.data(),out.size()*sizeof(out[0]))==0,"rejection records atomic");check(std::memcmp(&result,&savedResult,sizeof result)==0,"rejection result atomic");
        }
    }
    check(admitted>0,"original static cases exercised");
    std::cout<<Json{{"cases",admitted},{"out_of_scope_cases",skipped},{"checks",checks},{"failures",failures},{"hot_allocations",observedAllocations}}.dump()<<'\n';return failures?1:0;
}catch(const std::exception&e){watchAllocations=false;std::cerr<<e.what()<<'\n';return 2;}}
