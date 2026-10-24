#include "original_save_native_reader50kg.h"
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>

namespace rr::platform {
namespace {
using Reader=OriginalSaveNativeReader50KG;
constexpr char originalName[]="BASLUS-20268"; // original string003A8A48
constexpr std::size_t payloadBytes=0xF508;
struct Fd {
    int value=-1;
    explicit Fd(int v=-1):value(v){}
    ~Fd(){if(value>=0)::close(value);}
    Fd(const Fd&)=delete;Fd& operator=(const Fd&)=delete;
    int release(){const int v=value;value=-1;return v;}
    void reset(int v){if(value>=0)::close(value);value=v;}
};
Reader::Identity identity(const struct stat& s)noexcept {
    return {static_cast<std::uint64_t>(s.st_dev),static_cast<std::uint64_t>(s.st_ino)};
}
bool sameFile(const struct stat& a,const struct stat& b)noexcept {
    return identity(a)==identity(b)&&a.st_mode==b.st_mode&&a.st_size==b.st_size&&
        a.st_mtim.tv_sec==b.st_mtim.tv_sec&&a.st_mtim.tv_nsec==b.st_mtim.tv_nsec&&
        a.st_ctim.tv_sec==b.st_ctim.tv_sec&&a.st_ctim.tv_nsec==b.st_ctim.tv_nsec;
}
Reader::Status fail(Reader::Diagnostic& d,Reader::Status s,int e=0)noexcept {
    d={s,e};return s;
}
bool rootSyntax(const std::string& p)noexcept {
    if(p.size()<2||p.front()!='/'||p.back()=='/'||p.find('\0')!=std::string::npos)return false;
    std::size_t start=1;
    while(start<p.size()){
        const auto end=p.find('/',start);const auto n=(end==std::string::npos?p.size():end)-start;
        if(n==0||(n==1&&p[start]=='.')||(n==2&&p[start]=='.'&&p[start+1]=='.'))return false;
        if(end==std::string::npos)break;
        start=end+1;
    }
    return true;
}
}
OriginalSaveNativeReader50KG::OriginalSaveNativeReader50KG(int fd,Identity id,std::string path):
    rootFd_(fd),root_(id),rootPath_(std::move(path)){}
OriginalSaveNativeReader50KG::~OriginalSaveNativeReader50KG(){if(rootFd_>=0)::close(rootFd_);}
OriginalSaveNativeReader50KG::Receipt::Receipt(
    std::shared_ptr<const OriginalSaveNativeReader50KG> owner,Kind k,Identity dir,
    Identity file,std::vector<std::uint8_t>&& bytes):owner_(std::move(owner)),kind_(k),
    directory_(dir),file_(file),bytes_(std::move(bytes)){}
OriginalSaveNativeReader50KG::Identity OriginalSaveNativeReader50KG::Receipt::rootIdentity()const noexcept {
    return owner_->rootIdentity();
}
OriginalSaveNativeReader50KG::Status OriginalSaveNativeReader50KG::openExplicitRoot(
    const std::string& path,std::shared_ptr<OriginalSaveNativeReader50KG>& out,Diagnostic& d)noexcept {
    d={};if(!rootSyntax(path))return fail(d,Status::invalidRoot);
    try {
        Fd fd(::open("/",O_RDONLY|O_DIRECTORY|O_CLOEXEC));
        if(fd.value<0)return fail(d,Status::rootUnavailable,errno);
        std::size_t start=1;
        while(start<path.size()){
            const auto end=path.find('/',start);
            const auto name=path.substr(start,(end==std::string::npos?path.size():end)-start);
            const int child=::openat(fd.value,name.c_str(),O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);
            if(child<0)return fail(d,Status::rootUnavailable,errno);
            fd.reset(child);
            if(end==std::string::npos)break;
            start=end+1;
        }
        struct stat st{};
        if(::fstat(fd.value,&st)<0)return fail(d,Status::rootUnavailable,errno);
        // Allocate path/object while RAII still owns fd. Then transfer it before
        // shared_ptr allocation, whose failure deletes the object and closes fd.
        auto* raw=new OriginalSaveNativeReader50KG(fd.value,identity(st),path);
        fd.release();
        std::shared_ptr<OriginalSaveNativeReader50KG> next(raw);
        out=std::move(next);return Status::ok;
    }catch(...){return fail(d,Status::allocationFailure);}
}
OriginalSaveNativeReader50KG::Status OriginalSaveNativeReader50KG::openConfiguredRoot(
    std::shared_ptr<OriginalSaveNativeReader50KG>& out,Diagnostic& d)noexcept {
    d={};
    // Caller must serialize process configuration; getenv storage must not be
    // invalidated concurrently. Copy once, before any path/descriptor work.
    const char* setting=std::getenv("RR_NATIVE_SAVE_ROOT");
    if(!setting)return fail(d,Status::notConfigured);
    try {
        const std::string root(setting);
        return openExplicitRoot(root,out,d);
    }catch(...){return fail(d,Status::allocationFailure);}
}
OriginalSaveNativeReader50KG::Status OriginalSaveNativeReader50KG::read(
    Kind kind,std::unique_ptr<const Receipt>& out,Diagnostic& d)const noexcept {
    d={};
    if(kind!=Kind::header186E50&&kind!=Kind::payload186C70)return fail(d,Status::wrongKind);
    try {
        Fd directory(::openat(rootFd_,originalName,O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW));
        if(directory.value<0)return fail(d,Status::directoryUnavailable,errno);
        struct stat dir{};
        if(::fstat(directory.value,&dir)<0)return fail(d,Status::directoryUnavailable,errno);
        // NONBLOCK prevents a substituted FIFO from blocking before fstat rejects it.
        Fd file(::openat(directory.value,originalName,O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK));
        if(file.value<0)return fail(d,Status::fileUnavailable,errno);
        struct stat before{};
        if(::fstat(file.value,&before)<0)return fail(d,Status::fileUnavailable,errno);
        if(!S_ISREG(before.st_mode))return fail(d,Status::wrongKind);
        const std::size_t count=kind==Kind::header186E50?8:payloadBytes;
        if(before.st_size<static_cast<off_t>(count)||
            (kind==Kind::payload186C70&&before.st_size!=static_cast<off_t>(count)))
            return fail(d,Status::wrongSize);
        std::vector<std::uint8_t> bytes(count);
        std::size_t offset=0;
        while(offset<count){
            const auto n=::pread(file.value,bytes.data()+offset,count-offset,static_cast<off_t>(offset));
            if(n<0){if(errno==EINTR)continue;return fail(d,Status::readFailure,errno);}
            if(n==0)return fail(d,Status::wrongSize);
            offset+=static_cast<std::size_t>(n);
        }
        struct stat after{},namedFile{},namedDirectory{};
        if(::fstat(file.value,&after)<0)return fail(d,Status::readFailure,errno);
        if(::fstatat(directory.value,originalName,&namedFile,AT_SYMLINK_NOFOLLOW)<0||
            ::fstatat(rootFd_,originalName,&namedDirectory,AT_SYMLINK_NOFOLLOW)<0)
            return fail(d,Status::changedDuringRead,errno);
        if(!sameFile(before,after)||!sameFile(after,namedFile)||
            !(identity(dir)==identity(namedDirectory))||!S_ISDIR(namedDirectory.st_mode))
            return fail(d,Status::changedDuringRead);
        std::unique_ptr<const Receipt> next(new Receipt(shared_from_this(),kind,
            identity(dir),identity(after),std::move(bytes)));
        out=std::move(next);return Status::ok;
    }catch(...){return fail(d,Status::allocationFailure);}
}
}
