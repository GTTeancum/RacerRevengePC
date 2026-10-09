#include <string>
#include "../../src/game/body_scene48g.h"
#include "../../tools/gameplay/body_scene_oracle48g.h"
#include <random>

static unsigned checks=0;
static std::array<bool,4> qCoverage{};
static std::array<bool,12> angleCoverage{};
static void require(bool ok,const char* what) {++checks;if(!ok)throw std::runtime_error(what);}
static void equal(float a,float b,const char* what) {
    ++checks;if(bits(a)!=bits(b))throw std::runtime_error(std::string(what)+" actual="+std::to_string(bits(a))+" expected="+std::to_string(bits(b)));
}
static void call(OriginalScene48G& m,std::uint32_t pc) {
    m.steps=0;m.g[4][0]=0x600;m.g[5][0]=0x400;m.g[29][0]=0x1800;m.g[31][0]=0xf00df00d;m.v[0][3]=1;
    while(pc!=0xf00df00d) {
        std::uint32_t target=0,unused=0;
        if(m.execute(pc,target)) {if(m.execute(pc+4,unused))throw std::runtime_error("branch in delay slot");pc=target;}
        else pc+=4;
    }
    require(m.g[29][0]==0x1800,"original stack restored");
    for(unsigned i=0;i<4;++i)qCoverage[i]=qCoverage[i]||m.quaternionBranches[i];
    for(unsigned i=0;i<12;++i)angleCoverage[i]=angleCoverage[i]||m.angleBranches[i];
}
static BodyMatrix48G checkMatrix(const Quaternion48C& q) {
    OriginalScene48G m;m.store(0x400,q);call(m,0x14f4f0);
    BodyMatrix48G result{};require(quaternionToMatrix48G(q,result),"quaternion to matrix accepted");
    for(unsigned i=0;i<16;++i)equal(result.v[i],m.readFloat(0x600+4*i),"matrix bit comparison");
    return result;
}
static Quaternion48C checkQuaternion(const BodyMatrix48G& matrix) {
    OriginalScene48G m;for(unsigned i=0;i<16;++i)m.store(0x400+4*i,matrix.v[i]);call(m,0x14f950);
    Quaternion48C result{};require(matrixToQuaternion48G(matrix,result),"matrix to quaternion accepted");
    for(unsigned i=0;i<4;++i)equal(result.v[i],m.readFloat(0x600+4*i),"quaternion bit comparison");
    return result;
}
static void checkAngles(const Quaternion48C& q) {
    OriginalScene48G m;m.store(0x400,q);call(m,0x14fc00);
    Vector3_48C result{};require(quaternionToAngles48G(q,result),"quaternion to angles accepted");
    for(unsigned i=0;i<3;++i)equal(result.v[i],m.readFloat(0x600+4*i),"angles bit comparison");
    equal(m.readFloat(0x60c),1,"original homogeneous angle lane");
}
static void checkAngle(float y,float x) {
    OriginalScene48G m;m.f[12]=y;m.f[13]=x;call(m,0x13c1f0);
    float result=19;require(originalAngle48G(y,x,result),"angle accepted");equal(result,m.f[0],"angle bit comparison");
}
int main() {try {
    require(std::fegetround()==FE_TONEAREST,"nearest rounding mode");
    Quaternion48C saved{};BodyMatrix48G savedMatrix{};
    for(unsigned i=0;i<4;++i)saved.v[i]=fp(rr_saved_quaternion_48g[i]);
    for(unsigned i=0;i<16;++i)savedMatrix.v[i]=fp(rr_saved_matrix_48g[i]);
    checkMatrix(saved);checkQuaternion(savedMatrix);checkAngles(saved);
    const float boundaries[]={0,-0.0f,1,-1,2,-2,.5f,-.5f,fp(0x3f7fffffu),fp(0x3f800001u),1e-15f,-1e-15f};
    for(float y:boundaries)for(float x:boundaries)checkAngle(y,x);
    const Quaternion48C cases[]={{{0,0,0,0}},{{1,0,0,0}},{{0,1,0,0}},{{0,0,1,0}},{{0,0,0,1}},
        {{.5f,.5f,.5f,.5f}},{{-.5f,-.5f,.5f,.5f}},{{1,1,0,0}},{{0,1,1,0}},{{0,0,1,1}},{{1,0,0,1}},
        {{1,-0.0f,0,-0.0f}},{{1e-15f,0,0,0}}};
    for(const auto& q:cases){checkQuaternion(checkMatrix(q));checkAngles(q);}
    std::mt19937 random(0x48b0d1u);
    auto value=[&](){return static_cast<float>(static_cast<int>(random()%200001)-100000)/16384.0f;};
    for(unsigned n=0;n<10000;++n) {
        Quaternion48C q{{value(),value(),value(),value()}};
        checkQuaternion(checkMatrix(q));checkAngles(q);checkAngle(value(),value());
        BodyMatrix48G arbitrary{};for(float& x:arbitrary.v)x=value();checkQuaternion(arbitrary);
    }
    // Repeated original scene round trips, rather than retaining a normalized
    // quaternion indefinitely. Both branches retain their own previous result.
    Quaternion48C native=saved,original=saved;
    for(unsigned n=0;n<1000;++n) {
        native.v[1]=add(native.v[1],.00003f);original.v[1]=add(original.v[1],.00003f);
        BodyMatrix48G matrix{};require(quaternionToMatrix48G(native,matrix),"chained matrix");
        require(matrixToQuaternion48G(matrix,native),"chained quaternion");
        OriginalScene48G m;m.store(0x400,original);call(m,0x14f4f0);
        for(unsigned i=0;i<16;++i)m.write(0x400+4*i,m.read(0x600+4*i));call(m,0x14f950);original=m.quat(0x600);
        for(unsigned i=0;i<4;++i)equal(native.v[i],original.v[i],"chained bit comparison");
    }
    for(bool hit:qCoverage)require(hit,"all largest-component branches covered");
    for(bool hit:angleCoverage)require(hit,"all nonzero atan octant and axis branches covered");
    const float invalid[]={std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()};
    for(float x:invalid) {
        float a=23;require(!originalAngle48G(x,1,a)&&a==23,"invalid angle transaction");
        for(unsigned i=0;i<4;++i) {
            auto q=saved;q.v[i]=x;BodyMatrix48G m=savedMatrix;Vector3_48C v{{1,2,3}};
            require(!quaternionToMatrix48G(q,m)&&!std::memcmp(&m,&savedMatrix,sizeof(m)),"invalid quaternion matrix transaction");
            require(!quaternionToAngles48G(q,v)&&v.v[0]==1&&v.v[1]==2&&v.v[2]==3,"invalid angles transaction");
        }
        for(unsigned r=0;r<3;++r)for(unsigned c=0;c<3;++c) {
            auto m=savedMatrix;m.v[r*4+c]=x;auto q=saved;
            require(!matrixToQuaternion48G(m,q)&&!std::memcmp(&q,&saved,sizeof(q)),"invalid matrix transaction");
        }
    }
    float a=5;require(!originalAngle48G(1e30f,1e-30f,a)&&a==5,"overflow angle transaction");
    auto matrix=savedMatrix;Quaternion48C huge{{1e30f,0,0,0}};
    require(!quaternionToMatrix48G(huge,matrix)&&!std::memcmp(&matrix,&savedMatrix,sizeof(matrix)),"overflow matrix transaction");
    std::cout<<checks<<" body scene checks passed; all 4 quaternion selections and 12 angle branches covered\n";return 0;
}catch(const std::exception& e){std::cerr<<"FAIL after "<<checks<<" checks: "<<e.what()<<'\n';return 1;}}
