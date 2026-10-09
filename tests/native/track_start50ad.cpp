#include "track_start50ad.h"
#include "import_decode48c.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstring>
#include <cstdlib>
#include <new>
#include <cfenv>
using namespace rr::assets;using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=4)return 2;std::fesetround(FE_TONEAREST);nlohmann::json j;std::ifstream nf(argv[1]);nf>>j;unsigned cases=0;
for(const auto&row:j["cases"]){auto token=row["token"].get<std::string>();float f;std::uint64_t d;auto n=allocations;if(!parseTrackNumber50AD(token.data(),token.size(),f,&d)||allocations!=n){std::cerr<<"parse "<<token<<'\n';return 1;}std::uint32_t bits;std::memcpy(&bits,&f,4);if(bits!=row["binary32"]||d!=row["binary64"]){std::cerr<<"conversion "<<token<<" actual "<<std::hex<<bits<<" double "<<d<<" expected "<<row<<'\n';return 1;}++cases;}
std::ifstream file(argv[2],std::ios::binary);std::vector<std::uint8_t> raw((std::istreambuf_iterator<char>(file)),{}),member;ResArchive48C res;DecodeError48C error;if(!decodeResource48C(raw,res,error)||!extractResMember48C(res,"tracks\\mta.spl",member,error))return 1;std::string text(member.begin(),member.end());TrackStartScratch50AD scratch{};TrackStart50AD start{};TrackStartDiagnostic50AD diag{};auto n=allocations;if(!readTrackStart50AD(text.data(),text.size(),4,scratch,start,diag)||allocations!=n){std::cerr<<"read "<<unsigned(diag.error)<<" at "<<diag.offset<<'\n';return 1;}
if(start.nodeCount!=141||start.startNode||start.previousNode!=98||start.grid.nextNode!=1)return 1;
nlohmann::json poses;std::ifstream pf(argv[3]);pf>>poses;const auto&expected=poses["cases"][1]["matrix"];BodyMatrix48G matrix{};n=allocations;if(!prepareCenterPose50AC(start.grid,5.f,{{0,1,0}},matrix)||allocations!=n)return 1;std::uint32_t words[16];std::memcpy(words,matrix.v,64);for(unsigned i=0;i<16;++i)if(words[i]!=expected[i])return 1;
unsigned rejected=0;for(unsigned bad=0;bad<8;++bad){auto altered=text;auto replace=[&](const char*from,const char*to,std::size_t begin=0){auto at=altered.find(from,begin);if(at==std::string::npos)std::abort();altered.replace(at,std::strlen(from),to);};
if(bad==0)replace("NumNodes=141","NumNodes=257");if(bad==1)replace("[Track]","[Track]\nStartNode=2");if(bad==2)altered+="\n[Node0]\n";if(bad==3)replace("Position=","Pos=");if(bad==4)replace("Next0=1","Next0=999");if(bad==6)replace("NumNextNodes=1","NumNextNodes=0");if(bad==7)replace("NumNextNodes=1","NumNextNodes=1\nNumNextNodes=1");if(bad==5)replace("Next0=0","Next0=1",altered.find("[Node98]"));auto saved=start;n=allocations;if(readTrackStart50AD(altered.data(),altered.size(),4,scratch,start,diag)||allocations!=n||std::memcmp(&start,&saved,sizeof start))return 1;++rejected;}
std::cout<<nlohmann::json{{"numeric_original_cases",cases},{"nodes",start.nodeCount},{"previous",start.previousNode},{"rank",start.grid.rank},{"pose_word_checks",16},{"rejections",rejected},{"reader_allocations",0},{"scratch_bytes",sizeof scratch},{"output_bytes",sizeof start}}.dump()<<'\n';}
