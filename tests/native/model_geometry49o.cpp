#include "../../src/assets/model_geometry49o.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
using namespace rr::assets;
using Bytes = std::vector<std::uint8_t>;
static unsigned checks=0, rejects=0, originalNodes=0, originalKeys=0;
static void need(bool ok,const char* why){++checks;if(!ok)throw std::runtime_error(why);}
static std::uint32_t word(const Bytes& b,std::size_t at){return std::uint32_t(b.at(at))|(std::uint32_t(b.at(at+1))<<8)|(std::uint32_t(b.at(at+2))<<16)|(std::uint32_t(b.at(at+3))<<24);}
static std::uint32_t bits(float f){std::uint32_t b;std::memcpy(&b,&f,4);return b;}
static void put(Bytes& b,std::size_t at,std::uint32_t w){for(unsigned j=0;j<4;++j)b.at(at+j)=std::uint8_t(w>>(8*j));}
static void putf(Bytes& b,std::size_t at,float f){put(b,at,bits(f));}
static Bytes load(const std::string& path){std::ifstream f(path,std::ios::binary);need(bool(f),"fixture missing");return Bytes((std::istreambuf_iterator<char>(f)),{});}
static void badPsg(const Bytes& b,GeometryError49O e){
    PsgHierarchyView49O v{b.data(),123,17};GeometryDiagnostic49O d{};
    need(!readPsgHierarchy49O(b.data(),b.size(),v,d),"bad PSG accepted");need(d.code==e,"wrong PSG diagnostic");
    need(v.bytes==123&&v.nodeCount==17,"failed PSG changed result values");++rejects;
}
static void badMot(const Bytes& b,GeometryError49O e){
    MotionView49O v{};v.frameCount=29;v.duration=7;v.totalKeys=31;GeometryDiagnostic49O d{};
    need(!readMot49O(b.data(),b.size(),v,d),"bad MOT accepted");need(d.code==e,"wrong MOT diagnostic");
    need(v.frameCount==29&&v.duration==7&&v.totalKeys==31,"failed MOT changed result values");++rejects;
}
static Bytes psg(unsigned n){
    Bytes b(12+n*224);put(b,0,0x00677370);put(b,4,3);put(b,8,n);
    for(unsigned i=0;i<n;++i){const auto p=12+i*224;b[p]='n';for(unsigned j:{0u,5u,10u,15u})putf(b,p+128+j*4,1);put(b,p+220,i?i-1:0xffffffff);}
    return b;
}
static Bytes mot(unsigned positions,unsigned rotations,unsigned keys){
    Bytes b(28+(positions+rotations)*8+positions*keys*40+rotations*keys*8);
    put(b,0,0xfffffffe);put(b,4,256);putf(b,8,256);put(b,16,1);put(b,20,positions);put(b,24,rotations);
    std::size_t at=28;
    for(unsigned c=0;c<2;++c)for(unsigned i=0;i<(c?rotations:positions);++i){
        put(b,at,keys);put(b,at+4,i%64);at+=8;
        for(unsigned j=0;j<keys;++j){putf(b,at,float(j));at+=c?8:40;}
    }
    return b;
}
static void authentic(const std::string& dir){
    for(const auto* name:{"43-engrt1.psg","48-englt1.psg","52-chariot.psg"}){
        const auto b=load(dir+"/"+name);PsgHierarchyView49O v{};GeometryDiagnostic49O d{};
        need(readPsgHierarchy49O(b.data(),b.size(),v,d),"authentic PSG rejected");need(d.code==GeometryError49O::none,"PSG success diagnostic");
        need(v.nodeCount==word(b,8)&&v.bytes==b.size(),"PSG header differs");
        for(unsigned i=0;i<v.nodeCount;++i){PsgNode49O n{};need(readPsgNode49O(v,i,n),"authentic node rejected");
            const auto at=12+i*224;need(std::memcmp(n.name,b.data()+at,128)==0,"node name differs");
            for(unsigned j=0;j<16;++j)need(bits(n.local[j])==word(b,at+128+j*4),"node local bits differ");
            for(unsigned j=0;j<7;++j)need(bits(n.bounds[j])==word(b,at+192+j*4),"node bounds bits differ");
            need(std::uint32_t(n.parent)==word(b,at+220),"parent differs");++originalNodes;
        }
        const auto prefix=12+v.nodeCount*224;
        for(std::size_t size=1;size<prefix;++size){Bytes cut(b.begin(),b.begin()+size);badPsg(cut,GeometryError49O::truncated);}
        Bytes prefixOnly(b.begin(),b.begin()+prefix);
        need(readPsgHierarchy49O(prefixOnly.data(),prefixOnly.size(),v,d),"complete hierarchy prefix rejected");
        prefixOnly.insert(prefixOnly.end(),19,0xff);
        need(readPsgHierarchy49O(prefixOnly.data(),prefixOnly.size(),v,d),"uninspected tail wrongly rejected");
    }
    for(const auto* name:{"45-engrt1.mot","50-englt1.mot","54-01_brklt.mot","55-01_brkrt.mot","56-01_forwd.mot","57-01_back.mot"}){
        const auto b=load(dir+"/"+name);MotionView49O v{};GeometryDiagnostic49O d{};
        need(readMot49O(b.data(),b.size(),v,d),"authentic MOT rejected");need(d.code==GeometryError49O::none,"MOT success diagnostic");
        need(v.frameCount==21&&bits(v.duration)==0x3faaaaab,"authentic MOT header");
        need(v.positionCount==word(b,20)&&v.rotationCount==word(b,24),"authentic track counts");std::size_t at=28;unsigned total=0;
        for(unsigned c=0;c<2;++c)for(unsigned i=0;i<(c?v.rotationCount:v.positionCount);++i){
            const auto& t=c?v.rotation[i]:v.position[i];need(t.keyCount==word(b,at)&&t.rowIndex==word(b,at+4)&&t.keyDataOffset==at+8,"track order/extent differs");at+=8;
            for(unsigned j=0;j<t.keyCount;++j){
                if(c){ByteAxisKey49O k{};need(readRotationKey49O(v,i,j,k),"rotation key rejected");need(bits(k.time)==word(b,at)&&k.angle==b[at+4],"rotation prefix differs");
                    for(unsigned z=0;z<3;++z){need(std::uint8_t(k.axis[z])==b[at+5+z],"signed packed axis differs");}at+=8;
                }else{BezierKey49O k{};need(readPositionKey49O(v,i,j,k),"position key rejected");need(bits(k.time)==word(b,at),"position time differs");
                    for(unsigned z=0;z<3;++z){need(bits(k.position[z])==word(b,at+4+z*4),"position differs");need(bits(k.incoming[z])==word(b,at+16+z*4),"incoming differs");need(bits(k.outgoing[z])==word(b,at+28+z*4),"outgoing differs");}at+=40;
                }++total;++originalKeys;
            }
        }
        need(at==b.size()&&total==v.totalKeys,"MOT exact consumption differs");
        for(std::size_t size=1;size<b.size();++size){Bytes cut(b.begin(),b.begin()+size);badMot(cut,GeometryError49O::truncated);}
    }
}
static void scalarControls(){
    GeometryDiagnostic49O d{};PsgHierarchyView49O pv{};MotionView49O mv{};
    need(!readPsgHierarchy49O(nullptr,0,pv,d)&&d.code==GeometryError49O::invalid_input,"null PSG");
    need(!readMot49O(nullptr,0,mv,d)&&d.code==GeometryError49O::invalid_input,"null MOT");
    auto p=psg(1);auto m=mot(1,1,2);Bytes b;
    b=p;b.resize(maxPsgBytes49O+1);badPsg(b,GeometryError49O::file_limit);
    b=m;b.resize(maxMotBytes49O+1);badMot(b,GeometryError49O::file_limit);
    for(unsigned at:{0u,4u}){b=p;put(b,at,5);badPsg(b,GeometryError49O::version);}
    for(unsigned n:{0u,65u,0xffffffffu}){b=p;put(b,8,n);badPsg(b,GeometryError49O::node_count);}
    b=psg(64);need(readPsgHierarchy49O(b.data(),b.size(),pv,d)&&pv.nodeCount==64,"64 node boundary");
    b=p;b[12]=0;badPsg(b,GeometryError49O::node_name);
    for(unsigned c:{1u,31u,127u,128u,255u}){b=p;b[12]=std::uint8_t(c);badPsg(b,GeometryError49O::node_name);}
    b=p;b[14]='x';badPsg(b,GeometryError49O::node_name);
    b=p;for(unsigned j=0;j<128;++j)b[12+j]='x';badPsg(b,GeometryError49O::node_name);
    for(unsigned w:{0x7f800000u,0xff800000u,0x7fc00001u,0x7f800001u}){b=p;put(b,140,w);badPsg(b,GeometryError49O::matrix_nonfinite);b=p;put(b,204,w);badPsg(b,GeometryError49O::bounds);}
    for(unsigned j:{3u,7u,11u,15u}){b=p;putf(b,140+4*j,2);badPsg(b,GeometryError49O::matrix_nonaffine);}
    for(unsigned j:{3u,4u,5u,6u}){b=p;putf(b,204+4*j,-1);badPsg(b,GeometryError49O::bounds);}
    for(unsigned w:{0u,1u,0xfffffffeu}){b=p;put(b,232,w);badPsg(b,GeometryError49O::parent_order);}
    b=m;put(b,0,1);badMot(b,GeometryError49O::version);
    for(unsigned w:{0u,257u,0xffffffffu}){b=m;put(b,4,w);badMot(b,GeometryError49O::frame_count);}
    for(unsigned w:{0u,0x80000000u,0xbf800000u,0x7f800000u,0x7fc00000u}){b=m;put(b,8,w);badMot(b,GeometryError49O::duration);}
    for(unsigned at:{12u,16u}){b=m;put(b,at,2);badMot(b,GeometryError49O::track_type);}
    for(unsigned at:{20u,24u})for(unsigned n:{65u,0xffffffffu}){b=m;put(b,at,n);badMot(b,GeometryError49O::track_count);}
    for(unsigned n:{0u,257u,0xffffffffu}){b=m;put(b,28,n);badMot(b,GeometryError49O::key_count);}
    b=m;put(b,32,64);badMot(b,GeometryError49O::row_index);
    b=m;put(b,32,0xffffffffu);badMot(b,GeometryError49O::row_index);
    for(unsigned at:{36u,40u,48u,60u,72u,124u})for(unsigned w:{0x7f800000u,0xff800000u,0x7fc00001u}){b=m;put(b,at,w);badMot(b,GeometryError49O::key_nonfinite);}
    for(unsigned w:{0xbf800000u,0x43808000u}){b=m;put(b,36,w);badMot(b,GeometryError49O::key_time);}
    b=m;putf(b,76,0);badMot(b,GeometryError49O::key_time);
    b=m;putf(b,36,2);putf(b,76,1);badMot(b,GeometryError49O::key_time);
    b=m;putf(b,132,0);badMot(b,GeometryError49O::key_time);
    b=m;b.push_back(0);badMot(b,GeometryError49O::trailing_bytes);
    b=mot(64,64,1);need(readMot49O(b.data(),b.size(),mv,d)&&mv.totalKeys==128,"64+64 track boundary");
    b=mot(16,0,256);need(readMot49O(b.data(),b.size(),mv,d)&&mv.totalKeys==4096,"4096 key boundary");
    b=mot(17,0,256);badMot(b,GeometryError49O::total_keys);
    b=mot(0,0,1);need(readMot49O(b.data(),b.size(),mv,d)&&mv.totalKeys==0,"empty channels");
    b=mot(0,3,1);put(b,32,7);put(b,48,3);put(b,64,7);need(readMot49O(b.data(),b.size(),mv,d),"repeated unsorted rows rejected");
    need(mv.rotation[0].rowIndex==7&&mv.rotation[1].rowIndex==3&&mv.rotation[2].rowIndex==7,"rows reordered");
    // Numeric bit preservation is separate from any runtime arithmetic domain.
    for(unsigned w:{0u,0x80000000u,1u,0x80000001u,0x007fffffu,0x807fffffu,0x00800000u,0x80800000u,0x3f800001u,0xbf800001u,0x7f7fffffu,0xff7fffffu}){
        b=p;put(b,144,w);put(b,204,w);PsgNode49O n{};
        need(readPsgHierarchy49O(b.data(),b.size(),pv,d)&&readPsgNode49O(pv,0,n),"finite PSG scalar rejected");need(bits(n.local[1])==w&&bits(n.bounds[0])==w,"PSG canonicalized bits");
        b=m;for(unsigned j=1;j<10;++j)put(b,36+j*4,w);BezierKey49O k{};
        need(readMot49O(b.data(),b.size(),mv,d)&&readPositionKey49O(mv,0,0,k),"finite position scalar rejected");
        for(unsigned j=0;j<3;++j)need(bits(k.position[j])==w&&bits(k.incoming[j])==w&&bits(k.outgoing[j])==w,"Bezier canonicalized bits");
    }
    b=m;put(b,36,0x80000000);put(b,124,0x80000000);b[128]=255;b[129]=128;b[130]=127;b[131]=255;
    need(readMot49O(b.data(),b.size(),mv,d),"negative zero time rejected");BezierKey49O bk{};ByteAxisKey49O rk{};
    need(readPositionKey49O(mv,0,0,bk)&&readRotationKey49O(mv,0,0,rk),"signed controls read");
    need(bits(bk.time)==0x80000000&&bits(rk.time)==0x80000000&&rk.angle==255&&rk.axis[0]==-128&&rk.axis[1]==127&&rk.axis[2]==-1,"packed/signed-zero controls changed");
    need(!readPositionKey49O(mv,1,0,bk)&&!readPositionKey49O(mv,0,2,bk),"out-of-range position index");
    need(!readRotationKey49O(mv,1,0,rk)&&!readRotationKey49O(mv,0,2,rk),"out-of-range rotation index");
    b=p;put(b,152,0x80000000);need(readPsgHierarchy49O(b.data(),b.size(),pv,d),"signed affine zero rejected");PsgNode49O pn{};
    need(readPsgNode49O(pv,0,pn)&&bits(pn.local[3])==0x80000000,"signed affine zero changed");need(!readPsgNode49O(pv,1,pn),"out-of-range node index");
}
int main(int argc,char** argv){try{
    const std::string dir=argc>1?argv[1]:"evidence/authoredGeometry49O/asset-reader/fixtures";
    authentic(dir);scalarControls();
    std::cout<<"PASS ModelGeometry49O: "<<originalNodes<<" authenticated PSG nodes, "<<originalKeys<<" authored MOT keys, "<<rejects<<" scalar/extent rejections, "<<checks<<" checks\n";
}catch(const std::exception& e){std::cerr<<"FAIL ModelGeometry49O: "<<e.what()<<'\n';return 1;}}
