#if defined(_WIN32)
#include "original_save_native_reader50kg.h"
#include "native_beta_file50kg.h"
#include <utility>

namespace rr::platform {
namespace {
using Reader=OriginalSaveNativeReader50KG;
using native_beta50kg::Handle;
constexpr wchar_t originalName[]=L"BASLUS-20268";
constexpr std::size_t payloadBytes=0xF508;
Reader::Status fail(Reader::Diagnostic& d,Reader::Status s,DWORD e=0)noexcept {
    d={s,static_cast<int>(e)};return s;
}
Reader::Identity identity(const BY_HANDLE_FILE_INFORMATION& s)noexcept {
    return {s.dwVolumeSerialNumber,(std::uint64_t(s.nFileIndexHigh)<<32)|s.nFileIndexLow};
}
bool sameFile(const BY_HANDLE_FILE_INFORMATION& a,const BY_HANDLE_FILE_INFORMATION& b)noexcept {
    return identity(a)==identity(b)&&a.dwFileAttributes==b.dwFileAttributes&&
        a.nFileSizeHigh==b.nFileSizeHigh&&a.nFileSizeLow==b.nFileSizeLow&&
        a.ftLastWriteTime.dwHighDateTime==b.ftLastWriteTime.dwHighDateTime&&
        a.ftLastWriteTime.dwLowDateTime==b.ftLastWriteTime.dwLowDateTime&&
        a.ftCreationTime.dwHighDateTime==b.ftCreationTime.dwHighDateTime&&
        a.ftCreationTime.dwLowDateTime==b.ftCreationTime.dwLowDateTime;
}
bool wideRoot(const std::string& input,std::wstring& out) {
    if(input.empty()||input.size()>32760||input.find('\0')!=std::string::npos)return false;
    const int count=::MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,input.data(),static_cast<int>(input.size()),nullptr,0);
    if(count<=0)return false;
    std::wstring path(count,L'\0');
    if(::MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,input.data(),static_cast<int>(input.size()),path.data(),count)!=count)return false;
    for(auto& c:path)if(c==L'/')c=L'\\';
    // Deliberately bounded local-drive syntax. Reject UNC/device namespaces,
    // drive-relative paths, ADS, normalization aliases and traversal components.
    if(path.size()<4||!((path[0]>=L'A'&&path[0]<=L'Z')||(path[0]>=L'a'&&path[0]<=L'z'))||
        path[1]!=L':'||path[2]!=L'\\'||path.back()==L'\\')return false;
    for(std::size_t start=3;start<path.size();) {
        const auto end=path.find(L'\\',start);const auto stop=end==std::wstring::npos?path.size():end;
        if(stop==start||path[stop-1]==L'.'||path[stop-1]==L' ')return false;
        for(auto i=start;i<stop;++i)if(path[i]<32||path[i]==L':'||path[i]==L'*'||path[i]==L'?'||path[i]==L'"'||path[i]==L'<'||path[i]==L'>'||path[i]==L'|')return false;
        start=stop+1;
    }
    out=L"\\\\?\\"+path;return true;
}
HANDLE openDirectory(const std::wstring& path)noexcept {
    return ::CreateFileW(path.c_str(),FILE_READ_ATTRIBUTES,FILE_SHARE_READ,
        nullptr,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OPEN_REPARSE_POINT,nullptr);
}
bool directoryInfo(HANDLE h,BY_HANDLE_FILE_INFORMATION& info)noexcept {
    return ::GetFileType(h)==FILE_TYPE_DISK&&::GetFileInformationByHandle(h,&info)&&
        (info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)&&!(info.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT);
}
}
// All ancestors stay pinned without write/delete sharing. Unlike an unchecked
// absolute-path reopen, a parent cannot be renamed/replaced between components.
struct OriginalSaveNativeReader50KG::State {
    std::vector<HANDLE> ancestors;
    std::wstring path;
    ~State(){for(auto it=ancestors.rbegin();it!=ancestors.rend();++it)::CloseHandle(*it);}
};
OriginalSaveNativeReader50KG::OriginalSaveNativeReader50KG(std::unique_ptr<State> state,Identity id,std::string path):
    windows_(std::move(state)),root_(id),rootPath_(std::move(path)){}
OriginalSaveNativeReader50KG::~OriginalSaveNativeReader50KG()=default;
OriginalSaveNativeReader50KG::Receipt::Receipt(
    std::shared_ptr<const OriginalSaveNativeReader50KG> owner,Kind k,Identity dir,
    Identity file,std::vector<std::uint8_t>&& bytes):owner_(std::move(owner)),kind_(k),
    directory_(dir),file_(file),bytes_(std::move(bytes)){}
OriginalSaveNativeReader50KG::Identity OriginalSaveNativeReader50KG::Receipt::rootIdentity()const noexcept {
    return owner_->rootIdentity();
}
OriginalSaveNativeReader50KG::Status OriginalSaveNativeReader50KG::openExplicitRoot(
    const std::string& path,std::shared_ptr<OriginalSaveNativeReader50KG>& out,Diagnostic& d)noexcept {
    d={};
    try {
        auto state=std::make_unique<State>();
        if(!wideRoot(path,state->path))return fail(d,Status::invalidRoot);
        BY_HANDLE_FILE_INFORMATION info{};
        // Extended path begins with \\?\C:\ (seven UTF-16 code units).
        std::size_t end=7;
        for(;;){
            Handle dir(openDirectory(state->path.substr(0,end)));
            if(dir.value==INVALID_HANDLE_VALUE)return fail(d,Status::rootUnavailable,::GetLastError());
            if(!directoryInfo(dir.value,info))return fail(d,Status::invalidRoot,::GetLastError());
            state->ancestors.push_back(dir.value);dir.value=INVALID_HANDLE_VALUE;
            if(end==state->path.size())break;
            end=state->path.find(L'\\',end==7?7:end+1);
            if(end==std::wstring::npos)end=state->path.size();
        }
        std::shared_ptr<OriginalSaveNativeReader50KG> next(new OriginalSaveNativeReader50KG(std::move(state),identity(info),path));
        out=std::move(next);return Status::ok;
    }catch(...){return fail(d,Status::allocationFailure);}
}
OriginalSaveNativeReader50KG::Status OriginalSaveNativeReader50KG::openConfiguredRoot(
    std::shared_ptr<OriginalSaveNativeReader50KG>& out,Diagnostic& d)noexcept {
    d={};
    try {
        bool present=false;const auto value=native_beta50kg::environment(L"RR_NATIVE_SAVE_ROOT",present);
        if(!present)return fail(d,Status::notConfigured);
        if(value.empty()||value.size()>32760)return fail(d,Status::invalidRoot);
        const int count=::WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,value.data(),static_cast<int>(value.size()),nullptr,0,nullptr,nullptr);
        if(count<=0)return fail(d,Status::invalidRoot,::GetLastError());
        std::string path(count,'\0');
        if(::WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,value.data(),static_cast<int>(value.size()),path.data(),count,nullptr,nullptr)!=count)return fail(d,Status::invalidRoot,::GetLastError());
        return openExplicitRoot(path,out,d);
    }catch(...){return fail(d,Status::allocationFailure);}
}
OriginalSaveNativeReader50KG::Status OriginalSaveNativeReader50KG::read(
    Kind kind,std::unique_ptr<const Receipt>& out,Diagnostic& d)const noexcept {
    d={};
    if(kind!=Kind::header186E50&&kind!=Kind::payload186C70)return fail(d,Status::wrongKind);
    try {
        if(!windows_||windows_->ancestors.empty())return fail(d,Status::rootUnavailable);
        const auto directoryPath=windows_->path+L"\\"+originalName;
        Handle directory(openDirectory(directoryPath));
        if(directory.value==INVALID_HANDLE_VALUE)return fail(d,Status::directoryUnavailable,::GetLastError());
        BY_HANDLE_FILE_INFORMATION dir{};
        if(!directoryInfo(directory.value,dir))return fail(d,Status::directoryUnavailable,::GetLastError());
        const auto filePath=directoryPath+L"\\"+originalName;
        // No write/delete sharing: prevent replacement or ordinary concurrent
        // writes during size validation, reading and identity revalidation.
        Handle file(::CreateFileW(filePath.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,
            OPEN_EXISTING,FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_SEQUENTIAL_SCAN,nullptr));
        if(file.value==INVALID_HANDLE_VALUE)return fail(d,Status::fileUnavailable,::GetLastError());
        if(!native_beta50kg::regularFile(file.value))return fail(d,Status::wrongKind);
        BY_HANDLE_FILE_INFORMATION before{},after{},namedFile{},namedDirectory{};
        if(!::GetFileInformationByHandle(file.value,&before))return fail(d,Status::fileUnavailable,::GetLastError());
        const std::size_t count=kind==Kind::header186E50?8:payloadBytes;
        const auto size=(std::uint64_t(before.nFileSizeHigh)<<32)|before.nFileSizeLow;
        if(size<count||(kind==Kind::payload186C70&&size!=count))return fail(d,Status::wrongSize);
        std::vector<std::uint8_t> bytes(count);
        std::size_t offset=0;
        while(offset<count){DWORD n=0;
            if(!::ReadFile(file.value,bytes.data()+offset,static_cast<DWORD>(count-offset),&n,nullptr))return fail(d,Status::readFailure,::GetLastError());
            if(!n)return fail(d,Status::wrongSize);
            offset+=n;
        }
        if(!::GetFileInformationByHandle(file.value,&after))return fail(d,Status::readFailure,::GetLastError());
        Handle checkFile(::CreateFileW(filePath.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,
            OPEN_EXISTING,FILE_FLAG_OPEN_REPARSE_POINT,nullptr));
        Handle checkDirectory(openDirectory(directoryPath));
        if(checkFile.value==INVALID_HANDLE_VALUE||checkDirectory.value==INVALID_HANDLE_VALUE||
            !native_beta50kg::regularFile(checkFile.value)||!::GetFileInformationByHandle(checkFile.value,&namedFile)||
            !directoryInfo(checkDirectory.value,namedDirectory))return fail(d,Status::changedDuringRead,::GetLastError());
        if(!sameFile(before,after)||!sameFile(after,namedFile)||!(identity(dir)==identity(namedDirectory)))return fail(d,Status::changedDuringRead);
        std::unique_ptr<const Receipt> next(new Receipt(shared_from_this(),kind,identity(dir),identity(after),std::move(bytes)));
        out=std::move(next);return Status::ok;
    }catch(...){return fail(d,Status::allocationFailure);}
}
}

#else
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

#endif
