#include "../../src/game/rigid_vertices49s.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

using namespace rr::game;
using Error = RigidVertexError49S;
static std::uint64_t checks = 0, calls = 0, fixtureVertices = 0, fixtureCases = 0;
static void need(bool value, const char* reason) { ++checks; if (!value) throw std::runtime_error(reason); }
static BodyMatrix48G identity() { BodyMatrix48G m{}; for (unsigned i=0;i<4;++i)m.v[5*i]=1; return m; }
static std::uint32_t bits(float f) { std::uint32_t u;std::memcpy(&u,&f,4);return u; }
static bool invoke(RigidVertexView49S input, RigidPaletteView49S palette,
    RigidPositionView49S output, RigidVertexDiagnostic49S& diagnostic) {
    ++calls; return projectRigidVertices49S(input,palette,output,diagnostic);
}
struct Test {
    std::array<RigidVertex49S,maxRigidVertices49S> vertices{};
    std::array<BodyMatrix48G,maxRigidPalette49S> palette{};
    std::array<RigidPosition49S,maxRigidVertices49S> output{}, saved{};
    RigidVertexDiagnostic49S d{};
    Test() {
        for (auto& p:palette) p=identity();
        for (unsigned i=0;i<vertices.size();++i) vertices[i]={{double(i)+.25,-2.5,3.75},std::uint16_t(i%64)};
        std::memset(output.data(),0x5a,sizeof output); saved=output;
    }
    void rejected(Error e, RigidVertexView49S in, RigidPaletteView49S p, RigidPositionView49S out) {
        d={Error::none,99,99,99}; need(!invoke(in,p,out,d),"rejection accepted");
        need(d.error==e,"wrong diagnostic");need(!std::memcmp(output.data(),saved.data(),sizeof output),"rejection changed output");
    }
    void reject(Error e) { rejected(e,{vertices.data(),2},{palette.data(),64},{output.data(),4096}); }
};
static void ordinary() {
    Test t;
    need(invoke({t.vertices.data(),4096},{t.palette.data(),64},{t.output.data(),4096},t.d),"maximum admission");
    need(t.d.error==Error::none,"success diagnostic");
    for (unsigned i=0;i<4096;++i)for(unsigned j=0;j<3;++j)need(bits(t.output[i].v[j])==bits(float(t.vertices[i].positionLocal[j])),"identity maximum output");
    t.output=t.saved;
    need(invoke({t.vertices.data(),1},{t.palette.data(),1},{t.output.data(),4096},t.d),"minimum admission");
    need(!std::memcmp(t.output.data()+1,t.saved.data()+1,sizeof(RigidPosition49S)*4095),"unused output capacity modified");
    t.output=t.saved;
    t.rejected(Error::invalid_view,{nullptr,1},{t.palette.data(),1},{t.output.data(),4096});
    t.rejected(Error::invalid_view,{t.vertices.data(),1},{nullptr,1},{t.output.data(),4096});
    t.rejected(Error::invalid_view,{t.vertices.data(),1},{t.palette.data(),1},{nullptr,4096});
    for (auto count:{0u,4097u})t.rejected(Error::count_limit,{t.vertices.data(),count},{t.palette.data(),64},{t.output.data(),4096});
    for (auto count:{0u,65u})t.rejected(Error::count_limit,{t.vertices.data(),2},{t.palette.data(),count},{t.output.data(),4096});
    t.rejected(Error::count_limit,{t.vertices.data(),2},{t.palette.data(),64},{t.output.data(),4097});
    t.rejected(Error::output_capacity,{t.vertices.data(),2},{t.palette.data(),64},{t.output.data(),1});
    need(std::fesetround(FE_DOWNWARD)==0,"set directed rounding");t.reject(Error::rounding_mode);need(std::fesetround(FE_TONEAREST)==0,"restore rounding");
    t.palette[63].v[0]=std::numeric_limits<float>::infinity();t.reject(Error::matrix_nonfinite);need(t.d.matrix==63&&t.d.lane==0,"unused palette validation");t.palette[63]=identity();
    t.palette[1].v[12]=std::numeric_limits<float>::quiet_NaN();t.reject(Error::matrix_nonfinite);t.palette[1]=identity();
    for (auto lane:{3u,7u,11u,15u}) {t.palette[1].v[lane]=2;t.reject(Error::matrix_nonaffine);need(t.d.matrix==1&&t.d.lane==lane,"affine error element");t.palette[1]=identity();}
    t.vertices[1].paletteIndex=64;t.reject(Error::palette_index);need(t.d.vertex==1&&t.d.matrix==64,"late palette index");t.vertices[1].paletteIndex=1;
    for (auto value:{std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()}) {t.vertices[1].positionLocal[2]=value;t.reject(Error::position_nonfinite);need(t.d.vertex==1&&t.d.lane==2,"late local validation");}
    t.vertices[1]={{std::numeric_limits<double>::max(),0,0},1};t.palette[1].v[0]=2;t.reject(Error::arithmetic_nonfinite);t.palette[1]=identity();
    t.vertices[1]={{std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),0},1};t.palette[1].v[4]=1;t.reject(Error::arithmetic_nonfinite);t.palette[1]=identity();
    t.vertices[1]={{double(std::numeric_limits<float>::max())*2,0,0},1};t.reject(Error::output_range);need(t.d.vertex==1,"late final range");
    t.vertices[0]={{std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),std::numeric_limits<float>::denorm_min()},0};
    need(invoke({t.vertices.data(),1},{t.palette.data(),1},{t.output.data(),1},t.d),"finite float range edges");
    for(unsigned i=0;i<3;++i)need(bits(t.output[0].v[i])==bits(float(t.vertices[0].positionLocal[i])),"range edge bits");
    // Intermediate doubles may exceed float range if later terms cancel.
    t.vertices[0]={{double(std::numeric_limits<float>::max())*2,-double(std::numeric_limits<float>::max())*2,0},0};
    t.palette[0]={};t.palette[0].v[0]=t.palette[0].v[4]=t.palette[0].v[15]=1;
    need(invoke({t.vertices.data(),1},{t.palette.data(),1},{t.output.data(),1},t.d)&&bits(t.output[0].v[0])==0,"double cancellation admission");
}
struct Reader {
    std::ifstream f;
    explicit Reader(const char* path):f(path,std::ios::binary){need(bool(f),"fixture absent");}
    void bytes(void* p,std::size_t n){need(bool(f.read(static_cast<char*>(p),std::streamsize(n))),"fixture truncated");}
    std::uint64_t integer(unsigned n){std::uint64_t u=0;for(unsigned i=0;i<n;++i){unsigned char c;bytes(&c,1);u|=std::uint64_t(c)<<(i*8);}return u;}
    float f32(){std::uint32_t u=std::uint32_t(integer(4));float x;std::memcpy(&x,&u,4);return x;}
    double f64(){auto u=integer(8);double x;std::memcpy(&x,&u,8);return x;}
};
static void fixture(const char* path) {
    Reader r(path);char magic[8];r.bytes(magic,8);need(!std::memcmp(magic,"RRRIG49S",8),"fixture magic");
    const auto count=r.integer(4);need(count>0&&count<=64,"fixture cases");
    for(std::uint64_t ci=0;ci<count;++ci) {
        const auto nv=std::uint32_t(r.integer(4)),np=std::uint32_t(r.integer(4));need(nv&&nv<=4096&&np&&np<=64,"fixture counts");
        Test t;
        for(unsigned i=0;i<np;++i)for(float& x:t.palette[i].v)x=r.f32();
        std::vector<RigidPosition49S> expected(nv);
        for(unsigned i=0;i<nv;++i){for(double& x:t.vertices[i].positionLocal)x=r.f64();t.vertices[i].paletteIndex=std::uint16_t(r.integer(2));for(float& x:expected[i].v)x=r.f32();}
        const auto verticesBefore=t.vertices;
        const auto paletteBefore=t.palette;
        need(invoke({t.vertices.data(),nv},{t.palette.data(),np},{t.output.data(),4096},t.d),"oracle fixture rejected");
        for(unsigned i=0;i<nv;++i)for(unsigned lane=0;lane<3;++lane)need(bits(t.output[i].v[lane])==bits(expected[i].v[lane]),"independent rational oracle mismatch");
        need(!std::memcmp(t.vertices.data(),verticesBefore.data(),sizeof t.vertices),"input vertices changed");
        need(!std::memcmp(t.palette.data(),paletteBefore.data(),sizeof t.palette),"input palette changed");
        need(!std::memcmp(t.output.data()+nv,t.saved.data()+nv,(4096-nv)*sizeof(RigidPosition49S)),"output tail changed");
        fixtureVertices+=nv;++fixtureCases;
    }
    need(r.f.peek()==std::char_traits<char>::eof(),"fixture trailing bytes");
}
int main(int argc,char**argv) {
    try {
        need(argc==2||(argc==3&&!std::strcmp(argv[2],"--fixture-only")),"usage: rigid_vertices49s fixture.bin [--fixture-only]");need(std::fesetround(FE_TONEAREST)==0,"nearest rounding");
        if(argc==2)ordinary();
        fixture(argv[1]);
        std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"calls\":"<<calls<<",\"fixture_cases\":"<<fixtureCases<<",\"fixture_vertices\":"<<fixtureVertices<<",\"input_record_bytes\":"<<sizeof(RigidVertex49S)<<",\"output_record_bytes\":"<<sizeof(RigidPosition49S)<<"}\n";
        return 0;
    }catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}
}
