#pragma once
#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <streambuf>
namespace rr47r {
// Only the diagnostic byte stream is coalesced. Newlines, explicit flushes and
// a bounded 4 KiB buffer drain immediately to the original destination.
class Lines final : public std::streambuf {
    std::streambuf* sink_;std::array<char,4096> bytes_{};size_t used_=0;
    std::mutex mutex_;bool failed_=false;
    bool drain() {
        size_t sent=0;
        while(sent<used_) {
            const auto n=sink_->sputn(bytes_.data()+sent,static_cast<std::streamsize>(used_-sent));
            if(n<=0){failed_=true;break;}sent+=static_cast<size_t>(n);
        }
        if(sent){used_-=sent;std::memmove(bytes_.data(),bytes_.data()+sent,used_);}
        return !failed_;
    }
protected:
    std::streamsize xsputn(const char* p,std::streamsize n) override {
        std::lock_guard<std::mutex> lock(mutex_);if(failed_||n<0)return 0;
        std::streamsize done=0;
        while(done<n){
            size_t count=std::min<size_t>(bytes_.size()-used_,static_cast<size_t>(n-done));
            const char* newline=static_cast<const char*>(std::memchr(p+done,'\n',count));
            if(newline)count=static_cast<size_t>(newline-(p+done))+1;
            std::memcpy(bytes_.data()+used_,p+done,count);used_+=count;done+=static_cast<std::streamsize>(count);
            if((newline||used_==bytes_.size())&&!drain())return 0;
        }
        return done;
    }
    int_type overflow(int_type ch) override {
        if(traits_type::eq_int_type(ch,traits_type::eof()))return sync()==0?traits_type::not_eof(ch):traits_type::eof();
        const char byte=traits_type::to_char_type(ch);return xsputn(&byte,1)==1?ch:traits_type::eof();
    }
    int sync() override {std::lock_guard<std::mutex> lock(mutex_);return drain()&&sink_->pubsync()==0?0:-1;}
public:
    explicit Lines(std::streambuf* sink):sink_(sink){}
};
class Scope {
    std::streambuf* original_=std::cerr.rdbuf();Lines lines_{original_};
    std::ios_base::fmtflags flags_=std::cerr.flags();std::ostream* tied_=std::cerr.tie();bool enabled_=false,closed_=false;
public:
    Scope(){const char* p=std::getenv("RR_DIAGNOSTIC_LINES47R");enabled_=p&&p[0]=='1'&&p[1]=='\0';
        if(enabled_){std::cerr.rdbuf(&lines_);std::cerr.unsetf(std::ios_base::unitbuf);std::cerr.tie(nullptr);}}
    bool finish(){if(closed_||!enabled_)return true;const bool ok=lines_.pubsync()==0&&!std::cerr.bad();
        std::cerr.rdbuf(original_);std::cerr.flags(flags_);std::cerr.tie(tied_);closed_=true;return ok;}
    ~Scope(){finish();}
};
}
