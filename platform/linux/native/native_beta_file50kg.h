#pragma once
// Native user-data IO only. This is not the original memory-card format.
#include <cstdint>
#include <atomic>
#include <cstdlib>
#include <cerrno>
#include <filesystem>
#include <vector>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
namespace rr::platform::native_beta50kg {
enum class Result {ok,missing,invalid,ioError,unavailable,durabilityUnknown};
using Bytes=std::vector<std::uint8_t>;
inline std::filesystem::path defaultDirectory() {
    if(const auto* p=std::getenv("RR_NATIVE_SAVE_DIR"))return *p?std::filesystem::path(p):std::filesystem::path{};
    if(const auto* p=std::getenv("XDG_DATA_HOME"))if(*p&&std::filesystem::path(p).is_absolute())return std::filesystem::path(p)/"racer-revenge";
    if(const auto* p=std::getenv("HOME"))if(*p)return std::filesystem::path(p)/".local/share/racer-revenge";
    return {};
}
struct Fd {int value=-1;explicit Fd(int v):value(v){} ~Fd(){if(value>=0)::close(value);} Fd(const Fd&)=delete;Fd& operator=(const Fd&)=delete;};
inline Result readFile(const std::filesystem::path& path,Bytes& out) noexcept {
 try {
    Fd fd(::open(path.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK));
    if(fd.value<0)return errno==ENOENT?Result::missing:Result::ioError;
    struct stat st{};if(::fstat(fd.value,&st)||!S_ISREG(st.st_mode)||st.st_size<0||st.st_size>1024*1024)return Result::invalid;
    Bytes next(static_cast<std::size_t>(st.st_size));std::size_t at=0;
    while(at<next.size()){auto n=::read(fd.value,next.data()+at,next.size()-at);if(n<0&&errno==EINTR)continue;if(n<=0)return Result::ioError;at+=static_cast<std::size_t>(n);}
    std::uint8_t extra;ssize_t n;do{n=::read(fd.value,&extra,1);}while(n<0&&errno==EINTR);
    if(n!=0)return Result::invalid;
    out.swap(next);return Result::ok;
 }catch(...){return Result::ioError;}
}
inline Result writeFile(const std::filesystem::path& path,const Bytes& bytes) noexcept {
 std::string temporary;bool renamed=false;
 try {
    if(bytes.size()>1024*1024||path.filename().empty())return Result::invalid;
    const auto directory=path.parent_path();if(directory.empty())return Result::unavailable;
    std::error_code ec;std::filesystem::create_directories(directory,ec);if(ec)return Result::ioError;
    Fd dir(::open(directory.c_str(),O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW));if(dir.value<0)return Result::ioError;
    const auto name=path.filename().string();struct stat current{};
    if(::fstatat(dir.value,name.c_str(),&current,AT_SYMLINK_NOFOLLOW)==0){if(!S_ISREG(current.st_mode))return Result::invalid;}
    else if(errno!=ENOENT)return Result::ioError;
    // O_EXCL prevents following a stale/malicious temporary-file symlink.
    static std::atomic<std::uint64_t> serial{0};int raw=-1;
    for(unsigned i=0;i<64&&raw<0;++i){temporary=name+".tmp."+std::to_string(::getpid())+"."+std::to_string(++serial);raw=::openat(dir.value,temporary.c_str(),O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC|O_NOFOLLOW,0600);if(raw<0&&errno!=EEXIST)return Result::ioError;}
    if(raw<0)return Result::ioError;Fd fd(raw);
    const auto cleanup=[&](){::unlinkat(dir.value,temporary.c_str(),0);};
    std::size_t at=0;while(at<bytes.size()){auto n=::write(fd.value,bytes.data()+at,bytes.size()-at);if(n<0&&errno==EINTR)continue;if(n<=0){cleanup();return Result::ioError;}at+=static_cast<std::size_t>(n);}
    if(::fsync(fd.value)){cleanup();return Result::ioError;}
    if(::close(fd.value)){fd.value=-1;cleanup();return Result::ioError;}fd.value=-1;
    if(::renameat(dir.value,temporary.c_str(),dir.value,name.c_str())){cleanup();return Result::ioError;}
    renamed=true;
    return ::fsync(dir.value)==0?Result::ok:Result::durabilityUnknown;
 }catch(...){return renamed?Result::durabilityUnknown:Result::ioError;}
}
// Stable little-endian field encoding, no object padding or pointers.
inline void put32(Bytes& b,std::uint32_t n){for(unsigned i=0;i<4;++i)b.push_back(static_cast<std::uint8_t>(n>>(8*i)));}
inline std::uint32_t checksum(const Bytes& b,std::size_t begin=0)noexcept{
 std::uint32_t crc=0xffffffffu;for(std::size_t i=begin;i<b.size();++i){crc^=b[i];for(unsigned j=0;j<8;++j)crc=(crc>>1)^(0xedb88320u&(~(crc&1u)+1u));}return ~crc;
}
}
