#include "track_start50ad.h"
#include <cstring>
namespace rr::assets {
bool parseTrackNumber50AD(const char* text, std::size_t bytes, float& output,
                             std::uint64_t* binary64Bits) noexcept {
    if (!text || !bytes || bytes > 13) return false;
    std::size_t at=0, digits=0, fractional=0;
    const bool negative=text[0]=='-';
    if (negative || text[0]=='+') ++at;
    const std::size_t first=at;
    std::uint64_t n=0, d=1;
    while (at<bytes && text[at]>='0' && text[at]<='9') {
        if (++digits>11) return false;
        n=n*10+static_cast<unsigned>(text[at++]-'0');
    }
    if (at==first) return false;
    if (at<bytes && text[at]=='.') {
        ++at; const std::size_t start=at;
        while (at<bytes && text[at]>='0' && text[at]<='9') {
            if (++digits>11 || ++fractional>6) return false;
            n=n*10+static_cast<unsigned>(text[at++]-'0'); d*=10;
        }
        if (at==start) return false;
    }
    if (at!=bytes) return false;
    std::uint64_t binary64=0;
    std::uint32_t binary32=0;
    if (n) {
        // n and d are exact nonnegative integers <=99999999999 and1000000.
        // Normalize a/b into[1,2), then generate53 bits by long division.
        // Remainder compares implement nearest-even without host arithmetic.
        int exponent=0;
        while (n<d) { n*=2; --exponent; }
        while (n>=d*2) { d*=2; ++exponent; }
        std::uint64_t remainder=n-d, significand=1;
        for (unsigned i=0;i<52;++i) {
            remainder*=2; significand*=2;
            if (remainder>=d) { ++significand; remainder-=d; }
        }
        if (remainder*2>d || (remainder*2==d && (significand&1))) ++significand;
        if (significand==(std::uint64_t(1)<<53)) { significand>>=1; ++exponent; }
        binary64=(std::uint64_t(negative)<<63) |
            (std::uint64_t(exponent+1023)<<52) |
            (significand&((std::uint64_t(1)<<52)-1));
        // All admitted nonzero values remain normal binary32, so only the
        // original310498 normal tie-even branch is reachable after310440.
        std::uint32_t m=static_cast<std::uint32_t>(significand>>22);
        if (significand&((std::uint64_t(1)<<22)-1)) m|=1;
        if ((m&127u)==64u) { if (m&128u) m+=64u; } else m+=63u;
        if (m&0x80000000u) { m>>=1; ++exponent; }
        binary32=(std::uint32_t(negative)<<31) |
            (std::uint32_t(exponent+127)<<23) | ((m>>7)&0x7fffffu);
    }
    std::memcpy(&output,&binary32,4);
    if (binary64Bits) *binary64Bits=binary64;
    return true;
}
namespace {
struct Slice {const char*p;std::size_t n,at;};
bool space(char c){return c==' '||c=='\t'||c=='\r';}
Slice trim(Slice s){while(s.n&&space(*s.p)){++s.p;--s.n;++s.at;}while(s.n&&space(s.p[s.n-1]))--s.n;return s;}
char lower(char c){return c>='A'&&c<='Z'?char(c+32):c;}
bool equal(Slice s,const char*t){const auto n=std::strlen(t);if(n!=s.n)return false;for(std::size_t i=0;i<n;++i)if(lower(s.p[i])!=lower(t[i]))return false;return true;}
bool integer(Slice s,std::uint32_t&out){if(!s.n||s.n>3)return false;unsigned n=0;for(std::size_t i=0;i<s.n;++i){if(s.p[i]<'0'||s.p[i]>'9')return false;n=n*10+unsigned(s.p[i]-'0');}out=n;return true;}
bool vector(Slice s,std::uint32_t*out){std::size_t begin=0;for(unsigned i=0;i<3;++i){std::size_t end=begin;while(end<s.n&&s.p[end]!=',')++end;if((i<2&&end==s.n)||(i==2&&end!=s.n))return false;auto part=trim({s.p+begin,end-begin,s.at+begin});float f;if(!parseTrackNumber50AD(part.p,part.n,f))return false;std::memcpy(out+i,&f,4);begin=end+1;}out[3]=0x3f800000u;return true;}
bool overlap(const void*a,std::size_t an,const void*b,std::size_t bn){auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);return x<=y?y-x<an:x-y<bn;}
}
bool readTrackStart50AD(const char*text,std::size_t bytes,std::uint32_t rank,TrackStartScratch50AD&scratch,TrackStart50AD&out,TrackStartDiagnostic50AD&diag) noexcept {
 if(overlap(&scratch,sizeof scratch,&out,sizeof out)||overlap(&diag,sizeof diag,&out,sizeof out)||overlap(&diag,sizeof diag,&scratch,sizeof scratch)||overlap(text,bytes,&scratch,sizeof scratch)||overlap(text,bytes,&out,sizeof out)||overlap(text,bytes,&diag,sizeof diag))return false;
 auto fail=[&](TrackStartError50AD e,std::size_t at){diag={e,at};return false;};
 if(!text||!bytes)return fail(TrackStartError50AD::Input,0);if(bytes>maxTrackBytes50AD)return fail(TrackStartError50AD::Limit,0);if(rank!=1&&rank!=4)return fail(TrackStartError50AD::UnsupportedStart,0);
 for(std::size_t i=0;i<bytes;++i){auto c=static_cast<unsigned char>(text[i]);if((c<32&&c!=9&&c!=10&&c!=13)||c>126)return fail(TrackStartError50AD::Syntax,i);}
 scratch={};int section=-2;bool trackSeen=false,countSeen=false,startSeen=false;std::uint32_t count=0,start=0;std::size_t at=0;
 while(at<bytes){const auto begin=at;while(at<bytes&&text[at]!='\n')++at;auto line=trim({text+begin,at-begin,begin});if(at<bytes)++at;if(line.n>255)return fail(TrackStartError50AD::Limit,line.at);if(!line.n||line.p[0]==';')continue;
  if(line.p[0]=='['){if(line.n<3||line.p[line.n-1]!=']')return fail(TrackStartError50AD::Syntax,line.at);auto name=trim({line.p+1,line.n-2,line.at+1});if(equal(name,"Track")){if(trackSeen)return fail(TrackStartError50AD::Duplicate,line.at);trackSeen=true;section=-1;}else{if(name.n<5||lower(name.p[0])!='n'||lower(name.p[1])!='o'||lower(name.p[2])!='d'||lower(name.p[3])!='e')return fail(TrackStartError50AD::Syntax,line.at);std::uint32_t index;if(!integer({name.p+4,name.n-4,name.at+4},index)||index>=maxTrackNodes50AD)return fail(TrackStartError50AD::Limit,line.at);if(scratch.nodes[index].seen)return fail(TrackStartError50AD::Duplicate,line.at);scratch.nodes[index].seen=true;section=static_cast<int>(index);}continue;}
  if(section==-2)return fail(TrackStartError50AD::Syntax,line.at);std::size_t split=0;while(split<line.n&&line.p[split]!='=')++split;if(!split||split==line.n)return fail(TrackStartError50AD::Syntax,line.at);auto key=trim({line.p,split,line.at}),value=trim({line.p+split+1,line.n-split-1,line.at+split+1});if(!key.n||!value.n)return fail(TrackStartError50AD::Syntax,line.at);
  if(section==-1){if(equal(key,"NumNodes")){if(countSeen)return fail(TrackStartError50AD::Duplicate,line.at);countSeen=true;if(!integer(value,count)||count<2||count>maxTrackNodes50AD)return fail(TrackStartError50AD::Number,value.at);}else if(equal(key,"StartNode")){if(startSeen)return fail(TrackStartError50AD::Duplicate,line.at);startSeen=true;if(!integer(value,start))return fail(TrackStartError50AD::Number,value.at);if(start>1)return fail(TrackStartError50AD::UnsupportedStart,value.at);}}
  else{auto&n=scratch.nodes[static_cast<unsigned>(section)];if(equal(key,"Position")){if(n.fields&1)return fail(TrackStartError50AD::Duplicate,line.at);if(!vector(value,n.position))return fail(TrackStartError50AD::Number,value.at);n.fields|=1;}else if(equal(key,"NumNextNodes")){if(n.fields&4)return fail(TrackStartError50AD::Duplicate,line.at);std::uint32_t countNext;if(!integer(value,countNext)||!countNext||countNext>8)return fail(TrackStartError50AD::Number,value.at);n.fields|=4;}else if(equal(key,"Next0")){if(n.fields&2)return fail(TrackStartError50AD::Duplicate,line.at);if(!integer(value,n.next0))return fail(TrackStartError50AD::Number,value.at);n.fields|=2;}}
 }
 if(!trackSeen||!countSeen)return fail(TrackStartError50AD::Missing,bytes);
 for(unsigned i=0;i<maxTrackNodes50AD;++i){const auto&n=scratch.nodes[i];if(i<count){if(!n.seen||n.fields!=7||n.next0>=count)return fail(TrackStartError50AD::Missing,bytes);}else if(n.seen)return fail(TrackStartError50AD::Limit,bytes);}
 unsigned predecessor=0;while(predecessor<count&&scratch.nodes[predecessor].next0!=0)++predecessor;if(predecessor==count)return fail(TrackStartError50AD::NoPredecessor,bytes);
 TrackStart50AD result{};std::memcpy(result.grid.current,scratch.nodes[0].position,16);std::memcpy(result.grid.previous,scratch.nodes[predecessor].position,16);result.grid.rank=rank;result.grid.nextNode=scratch.nodes[0].next0;result.nodeCount=count;result.previousNode=predecessor;out=result;diag={TrackStartError50AD::None,0};return true;
}
}
