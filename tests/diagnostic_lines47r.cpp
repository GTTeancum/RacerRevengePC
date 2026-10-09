#include "diagnostic_lines47r.h"
#include <string>
struct Sink : std::streambuf {
    std::string text;size_t calls=0;bool shortWrites=false,fail=false;
    std::streamsize xsputn(const char* p,std::streamsize n) override {++calls;if(fail)return 0;if(shortWrites)n=std::min<std::streamsize>(13,n);text.append(p,static_cast<size_t>(n));return n;}
};
int main(){
    Sink sink;rr47r::Lines buffer(&sink);std::ostream out(&buffer);
    for(int i=0;i<1000;++i)out<<"address="<<i<<", result="<<(i*7)<<'\n';
    std::string expected;for(int i=0;i<1000;++i)expected+="address="+std::to_string(i)+", result="+std::to_string(i*7)+"\n";
    if(sink.text!=expected||sink.calls!=1000)return 1;
    out<<"partial";if(sink.text!=expected)return 2;out.flush();expected+="partial";if(sink.text!=expected)return 3;
    sink.shortWrites=true;std::string longLine(9000,'x');out<<longLine<<'\n';expected+=longLine+'\n';if(sink.text!=expected)return 4;
    sink.fail=true;out<<"failure\n";if(!out.bad())return 5;
    std::cout<<"1000 multipart records coalesced to 1000 writes; byte equality, partial flush, long line, short write and output failure checked\n";
}
