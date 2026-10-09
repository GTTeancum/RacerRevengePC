// COLD ONLY. Reuse unchanged validated exporter initialization and serializers.
// This translation unit and every original-word interpreter are excluded from
// the native benchmark link and allocation probe.
#include "../../evidence/frameBridge49W/frame/frame_fixture.h"
#define main(...) rr49w_cold_exporter_main(__VA_ARGS__)
#include "../visual/export_vehicle_frame49w.cpp"
#undef main
#include "native_frame49w.h"
#include <fstream>
namespace b=budget49w;
using J=nlohmann::json;
static std::uint64_t leaves(const J&j){if(j.is_primitive())return 1;std::uint64_t n=0;for(const auto&v:j)n+=leaves(v);return n;}
static J load(const std::string&path){std::ifstream in(path);b::need(bool(in),"approved trace open");J j;in>>j;return j;}
template<class T>static void write(std::ofstream&out,const T&x){const auto y=b::normalized(x);out.write(reinterpret_cast<const char*>(&y),sizeof y);b::need(bool(out),"cold fixture write");}
int main(int argc,char**argv){try{
    b::need(argc==2,"prepare_frame49w output-directory");b::need(std::fesetround(FE_TONEAREST)==0,"rounding");
    const std::filesystem::path output=argv[1];std::filesystem::create_directories(output);
    auto fixture=std::make_unique<frame49w_test::Fixture>(120);auto&x=*fixture;
    const auto authored=advancing49w::initialize(x);
    b::Immutable data{};data.parameters=x.parameters;data.bindings=x.bindings;data.secondary=x.secondaryInputs;data.selectors=x.selectors;data.codes=x.actionCodes;data.configurationEntryNonzero=x.configurationEntryNonzero;
    for(unsigned i=0;i<3;++i)data.rows[i]=x.assets.rows[i];data.ik=*x.bindings.animation.handRecords;
    b::clearPointers(data.bindings);b::bindPointers(data,x.assets.motions.data());
    std::ofstream out(output/"actual-history144.bin",std::ios::binary);b::need(bool(out),"fixture output");
    out.write("RR49WB01",8);write(out,b::U(0x01020304));write(out,b::U(sizeof(b::Immutable)));write(out,b::U(sizeof(b::VehicleFrameState49W)));write(out,b::U(sizeof(b::Expected)));write(out,b::frames);
    auto saved=data;b::clearPointers(saved.bindings);write(out,saved);write(out,x.state);
    for(const auto&mot:x.assets.mot){write(out,b::U(mot.size()));out.write(reinterpret_cast<const char*>(mot.data()),mot.size());}
    J report={{"passed",true},{"frames_per_scenario",b::frames},{"abi",{{"immutable_bytes",sizeof(b::Immutable)},{"state_bytes",sizeof(b::VehicleFrameState49W)},{"expected_bytes",sizeof(b::Expected)}}},{"initial_state_hash",b::hash(x.state)},{"source_authored_compared_words",authored.comparedWords},{"scenarios",J::array()}};
    auto run=std::make_unique<b::Running>();
    for(unsigned scenario=0;scenario<2;++scenario){
        const bool support=scenario==0;const std::string base="evidence/frameBridge49W/connected/"+std::string(support?"":"no-support/");
        const J trace=load(base+"trace.json");b::need(trace["summary"]["passed"]&&trace["frames"].size()==b::frames,"approved trace shape");
        b::restart(*run,x.state);const auto initial=advancing49w::state(run->state);
        for(auto it=initial.begin();it!=initial.end();++it)b::need(trace["initial"].at(it.key())==it.value(),"exact cold initial field");
        std::uint64_t words=leaves(initial),substeps=0;J hashes=J::array();
        for(b::U frame=0;frame<b::frames;++frame){
            const auto&row=trace["frames"][frame];b::need(advancing49w::state(run->state)==row["before"],"canonical before word parity");
            b::need(b::graphics(*run,data,frame),"native graphics admitted");
            const auto graphics=advancing49w::state(run->state);b::need(graphics==row["after_graphics_before_physics"],"all graphics words match approved trace");
            b::Expected expected{};expected.graphics=run->state;
            do{const auto step=expected.substeps;b::need(step<4,"bounded expected substeps");b::need(b::substep(*run,data,frame,support,&expected.support[step]),"native substep admitted");
                const auto&supplied=row["supplied354_support_forces"][step];const auto&actual=expected.support[step];
                b::need(actual.mainBits==supplied["main_support_y_bits"]&&actual.secondaryBits==supplied["secondary_support_y_bits"]&&actual.physical==supplied["physical838"],"exact supplied support words");++expected.substeps;
            }while(run->state.phase!=b::VehicleFramePhase49W::Ready);
            const auto after=advancing49w::state(run->state);b::need(after==row["after"],"all after words match approved trace");b::need(expected.substeps==row["supplied354_support_forces"].size(),"all substeps");
            expected.after=run->state;write(out,expected);words+=leaves(row["before"])+leaves(graphics)+leaves(after);substeps+=expected.substeps;
            hashes.push_back({{"frame",frame},{"graphics",b::hash(expected.graphics)},{"after",b::hash(expected.after)},{"graph_fnv64",connected49r::graphHash(advancing49w::graphCopy(run->state))},{"noise_fnv64",connected49r::noiseHash(run->state.noise)}});
        }
        b::need(substeps==trace["summary"]["substeps"],"trace total substeps");
        report["scenarios"].push_back({{"name",support?"synthetic-support":"no-support"},{"compared_logical_leaves",words},{"substeps",substeps},{"hashes",hashes},{"final_state_hash",b::hash(run->state)},{"limits",trace["initial"]["scope"]}});
    }
    out.close();b::need(bool(out),"fixture close");std::ofstream meta(output/"fixture-validation.json");meta<<report.dump(2)<<'\n';b::need(bool(meta),"metadata output");std::cout<<"PASS cold approved trace word parity both144 histories\n";
}catch(const std::exception&e){std::cerr<<"FAIL cold budget49W "<<e.what()<<'\n';return 2;}}
