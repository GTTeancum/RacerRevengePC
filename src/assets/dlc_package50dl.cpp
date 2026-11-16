#include "dlc_package50dl.h"
// NOT ORIGINAL BEHAVIOUR (PC enhancement): see dlc_package50dl.h.
#include "../../third_party/native-json/json.hpp"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <set>
#include <sstream>
#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace rr::assets::dlc50DL {
namespace fs=std::filesystem;
using Json=nlohmann::json;

// ---------------------------------------------------------------- SHA-256
namespace {
struct Sha256 {
    std::array<std::uint32_t,8> h{0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
                                   0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u};
    std::array<std::uint8_t,64> block{};std::size_t used=0;std::uint64_t total=0;
    static std::uint32_t rotr(std::uint32_t x,int n){return (x>>n)|(x<<(32-n));}
    void compress(const std::uint8_t* p){
        static constexpr std::uint32_t k[64]={
            0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
            0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
            0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
            0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
            0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
            0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
            0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
            0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u};
        std::uint32_t w[64];
        for(int i=0;i<16;++i)w[i]=std::uint32_t(p[4*i])<<24|std::uint32_t(p[4*i+1])<<16|std::uint32_t(p[4*i+2])<<8|p[4*i+3];
        for(int i=16;i<64;++i){
            const auto s0=rotr(w[i-15],7)^rotr(w[i-15],18)^(w[i-15]>>3);
            const auto s1=rotr(w[i-2],17)^rotr(w[i-2],19)^(w[i-2]>>10);
            w[i]=w[i-16]+s0+w[i-7]+s1;
        }
        auto a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],hh=h[7];
        for(int i=0;i<64;++i){
            const auto t1=hh+(rotr(e,6)^rotr(e,11)^rotr(e,25))+((e&f)^(~e&g))+k[i]+w[i];
            const auto t2=(rotr(a,2)^rotr(a,13)^rotr(a,22))+((a&b)^(a&c)^(b&c));
            hh=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
        }
        h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=hh;
    }
    void update(const std::uint8_t* p,std::size_t n){
        total+=n;
        while(n){
            const std::size_t take=std::min(n,block.size()-used);
            std::memcpy(block.data()+used,p,take);used+=take;p+=take;n-=take;
            if(used==block.size()){compress(block.data());used=0;}
        }
    }
    std::string finish(){
        const std::uint64_t bits=total*8;const std::uint8_t one=0x80,zero=0;
        update(&one,1);while(used!=56)update(&zero,1);
        std::uint8_t length[8];for(int i=0;i<8;++i)length[i]=std::uint8_t(bits>>(56-8*i));
        update(length,8);
        static constexpr char digits[]="0123456789abcdef";std::string out;out.reserve(64);
        for(auto word:h)for(int s=28;s>=0;s-=4)out.push_back(digits[(word>>s)&15u]);
        return out;
    }
};
bool fail(std::string& reason,const std::string& text){
    reason=text.size()>Limits::reasonChars?text.substr(0,Limits::reasonChars):text;return false;
}
bool printable(const std::string& text,std::size_t limit){
    if(text.size()>limit)return false;
    for(unsigned char c:text)if(c<0x20||c==0x7f)return false;
    return true;
}
bool hex64(const std::string& text){
    if(text.size()!=64)return false;
    for(char c:text)if(!((c>='0'&&c<='9')||(c>='a'&&c<='f')))return false;
    return true;
}
bool validId(const std::string& id){
    if(id.size()<3||id.size()>48)return false;
    const auto ok=[](char c){return (c>='a'&&c<='z')||(c>='0'&&c<='9');};
    if(!ok(id[0]))return false;
    for(char c:id)if(!ok(c)&&c!='-')return false;
    return true;
}
std::string lowerHex(std::string text){for(auto& c:text)if(c>='A'&&c<='F')c=char(c-'A'+'a');return text;}
bool reparsePoint(const fs::path& path){
#if defined(_WIN32)
    const DWORD attributes=GetFileAttributesW(path.c_str());
    return attributes!=INVALID_FILE_ATTRIBUTES&&(attributes&FILE_ATTRIBUTE_REPARSE_POINT);
#else
    std::error_code error;return fs::is_symlink(fs::symlink_status(path,error));
#endif
}
bool executableHeader(const fs::path& path){
    std::ifstream in(path,std::ios::binary);
    std::array<unsigned char,64> head{};
    in.read(reinterpret_cast<char*>(head.data()),head.size());
    const auto got=static_cast<std::size_t>(in.gcount());
    if(got>=4&&head[0]==0x7f&&head[1]=='E'&&head[2]=='L'&&head[3]=='F')return true;
    if(got>=64&&head[0]=='M'&&head[1]=='Z'){
        const std::uint32_t at=head[60]|head[61]<<8|head[62]<<16|std::uint32_t(head[63])<<24;
        std::array<char,4> pe{};in.clear();in.seekg(at);in.read(pe.data(),4);
        if(in.gcount()==4&&pe[0]=='P'&&pe[1]=='E'&&pe[2]==0&&pe[3]==0)return true;
    }
    return false;
}
// Typed field readers: a wrong type is a refusal, never a silent default.
bool str(const Json& o,const char* key,std::string& out,bool required,std::string& reason){
    const auto at=o.find(key);
    if(at==o.end()){if(required)return fail(reason,std::string("dlc.json: missing field \"")+key+"\"");return true;}
    if(!at->is_string())return fail(reason,std::string("dlc.json: field \"")+key+"\" must be text");
    out=at->get<std::string>();return true;
}
bool boolean(const Json& o,const char* key,bool& out,bool required,std::string& reason){
    const auto at=o.find(key);
    if(at==o.end()){if(required)return fail(reason,std::string("dlc.json: missing field \"")+key+"\"");return true;}
    if(!at->is_boolean())return fail(reason,std::string("dlc.json: field \"")+key+"\" must be true or false");
    out=at->get<bool>();return true;
}
bool unsignedNumber(const Json& o,const char* key,std::uint64_t& out,std::string& reason){
    const auto at=o.find(key);
    if(at==o.end()||!at->is_number_unsigned())
        return fail(reason,std::string("dlc.json: field \"")+key+"\" must be a whole number");
    out=at->get<std::uint64_t>();return true;
}
bool array(const Json& o,const char* key,const Json*& out,bool required,std::size_t limit,std::string& reason){
    out=nullptr;const auto at=o.find(key);
    if(at==o.end()){if(required)return fail(reason,std::string("dlc.json: missing field \"")+key+"\"");return true;}
    if(!at->is_array())return fail(reason,std::string("dlc.json: field \"")+key+"\" must be a list");
    if(at->size()>limit)return fail(reason,std::string("dlc.json: too many entries in \"")+key+"\"");
    out=&*at;return true;
}
} // namespace

bool sha256File(const fs::path& path,std::uint64_t& bytes,std::string& hex){
    std::ifstream in(path,std::ios::binary);
    if(!in)return false;
    Sha256 sha;std::vector<char> buffer(1u<<20);bytes=0;
    while(in){
        in.read(buffer.data(),static_cast<std::streamsize>(buffer.size()));
        const auto got=static_cast<std::size_t>(in.gcount());
        if(got){sha.update(reinterpret_cast<const std::uint8_t*>(buffer.data()),got);bytes+=got;}
    }
    if(in.bad()||!in.eof())return false;
    hex=sha.finish();return true;
}

bool parseVersion(const std::string& text,Version& out){
    int parts[3]{};std::size_t at=0;
    for(int i=0;i<3;++i){
        if(at>=text.size()||!std::isdigit(static_cast<unsigned char>(text[at])))return false;
        long value=0;std::size_t digits=0;
        while(at<text.size()&&std::isdigit(static_cast<unsigned char>(text[at]))){
            value=value*10+(text[at]-'0');++at;if(++digits>6)return false;
        }
        parts[i]=static_cast<int>(value);
        if(i<2){if(at>=text.size()||text[at]!='.')return false;++at;}
    }
    if(at!=text.size())return false;
    out={parts[0],parts[1],parts[2]};return true;
}
int compareVersions(const Version& a,const Version& b){
    if(a.major!=b.major)return a.major<b.major?-1:1;
    if(a.minor!=b.minor)return a.minor<b.minor?-1:1;
    if(a.patch!=b.patch)return a.patch<b.patch?-1:1;
    return 0;
}
namespace {
bool comparator(const std::string& term,std::string& op,Version& v){
    std::size_t n=0;
    while(n<term.size()&&(term[n]=='<'||term[n]=='>'||term[n]=='='))++n;
    op=term.substr(0,n);if(op.empty())op="=";
    if(op!="="&&op!="<"&&op!="<="&&op!=">"&&op!=">=")return false;
    return parseVersion(term.substr(n),v);
}
}
bool versionRangeValid(const std::string& range){
    std::istringstream terms(range);std::string term,op;Version v;std::size_t count=0;
    while(terms>>term){if(!comparator(term,op,v)||++count>4)return false;}
    return true;
}
bool versionSatisfies(const Version& version,const std::string& range){
    std::istringstream terms(range);std::string term,op;Version v;
    while(terms>>term){
        if(!comparator(term,op,v))return false;
        const int c=compareVersions(version,v);
        const bool ok=op=="="?c==0:op=="<"?c<0:op=="<="?c<=0:op==">"?c>0:c>=0;
        if(!ok)return false;
    }
    return true;
}

std::string foldCase(std::string text){
    for(auto& c:text)if(c>='A'&&c<='Z')c=char(c-'A'+'a');
    return text;
}
bool normalisePackagePath(const std::string& input,std::string& out){
    out.clear();
    if(input.empty()||input.size()>Limits::pathChars)return false;
    std::string part;std::size_t depth=0;
    const auto flush=[&]()->bool{
        if(part.empty()||part=="."||part=="..")return false;
        if(part.back()==' '||part.back()=='.')return false; // Windows would silently trim these
        if(++depth>Limits::pathDepth)return false;
        if(!out.empty())out.push_back('/');
        out+=part;part.clear();return true;
    };
    for(std::size_t i=0;i<input.size();++i){
        const unsigned char c=static_cast<unsigned char>(input[i]);
        if(c<0x20||c==0x7f||c==':'||c=='*'||c=='?'||c=='"'||c=='<'||c=='>'||c=='|')return false;
        if(c=='/'||c=='\\'){if(!flush())return false;continue;}
        part.push_back(static_cast<char>(c));
    }
    return flush();
}
bool refusedFileType(const std::string& path){
    static const char* const refused[]={".exe",".dll",".com",".bat",".cmd",".ps1",".vbs",".js",
        ".py",".sh",".lua",".so",".dylib",".scr",".msi",".jar",".wsf",".hta"};
    const auto folded=foldCase(path);
    for(const char* ext:refused){
        const std::size_t n=std::strlen(ext);
        if(folded.size()>=n&&folded.compare(folded.size()-n,n,ext)==0)return true;
    }
    return false;
}

bool readManifest(const fs::path& folder,Package& out,std::string& reason){
    out=Package{};out.folder=folder;
    const auto manifest=folder/"dlc.json";
    std::error_code error;
    if(reparsePoint(folder))return fail(reason,"package folder is a link or junction");
    if(!fs::is_regular_file(fs::symlink_status(manifest,error))||reparsePoint(manifest))
        return fail(reason,"no dlc.json in the package folder");
    const auto size=fs::file_size(manifest,error);
    if(error||size==0||size>Limits::manifestBytes)return fail(reason,"dlc.json is empty or larger than 1 MiB");
    std::string text(static_cast<std::size_t>(size),'\0');
    {std::ifstream in(manifest,std::ios::binary);in.read(text.data(),static_cast<std::streamsize>(text.size()));
     if(!in||in.gcount()!=static_cast<std::streamsize>(text.size()))return fail(reason,"dlc.json could not be read");}
    if(text.size()>=3&&static_cast<unsigned char>(text[0])==0xEF&&static_cast<unsigned char>(text[1])==0xBB&&
       static_cast<unsigned char>(text[2])==0xBF)return fail(reason,"dlc.json starts with a byte-order mark (save it as UTF-8 without BOM)");
    Json root;
    try{root=Json::parse(text);}catch(const std::exception& e){
        return fail(reason,std::string("dlc.json is not valid UTF-8 JSON: ")+e.what());
    }
    if(!root.is_object())return fail(reason,"dlc.json must hold one JSON object");
    std::string schema;
    if(!str(root,"schema",schema,true,reason))return false;
    if(schema!=schemaName)return fail(reason,"dlc.json schema \""+schema+"\" is not supported by this build (needs \""+schemaName+"\")");
    if(!str(root,"id",out.id,true,reason))return false;
    if(!validId(out.id))return fail(reason,"package id \""+out.id+"\" is not 3-48 characters of a-z, 0-9 and '-' starting with a letter or digit");
    if(folder.filename().u8string()!=std::u8string(out.id.begin(),out.id.end()))
        return fail(reason,"folder name differs from the package id \""+out.id+"\"");
    if(!str(root,"name",out.name,true,reason)||!str(root,"author",out.author,true,reason)||
       !str(root,"version",out.versionText,true,reason)||!str(root,"description",out.description,false,reason)||
       !str(root,"kind",out.kind,true,reason))return false;
    if(out.name.empty()||!printable(out.name,Limits::nameChars))return fail(reason,"name must be 1-63 printable characters");
    if(!printable(out.author,Limits::nameChars))return fail(reason,"author must be at most 63 printable characters");
    if(!printable(out.description,Limits::descriptionChars))return fail(reason,"description must be at most 1024 printable characters");
    if(!parseVersion(out.versionText,out.version))return fail(reason,"version \""+out.versionText+"\" is not MAJOR.MINOR.PATCH");
    if(!boolean(root,"enabled",out.enabled,true,reason))return false;
    if(!boolean(root,"replace",out.replace,false,reason))return false;
    if(!boolean(root,"personal",out.personal,false,reason))return false;
    if(!str(root,"personal_note",out.personalNote,false,reason))return false;
    if(!printable(out.personalNote,Limits::noteChars))return fail(reason,"personal_note must be at most 255 printable characters");
    if(out.personal&&out.personalNote.empty())out.personalNote=personalNoteDefault;
    if(const auto at=root.find("priority");at!=root.end()){
        if(!at->is_number_integer())return fail(reason,"priority must be a whole number");
        const auto value=at->get<long long>();
        if(value<Limits::priorityLow||value>Limits::priorityHigh)return fail(reason,"priority must be -1000..1000");
        out.priority=static_cast<int>(value);
    }
    std::string requiredPort;
    if(!str(root,"requires_port",requiredPort,false,reason))return false;
    if(!requiredPort.empty()&&requiredPort!=portContract)
        return fail(reason,"package needs port data contract \""+requiredPort+"\"; this build has \""+portContract+"\"");
    static const std::set<std::string> kinds{"course","pod","menu_assets","replacement","bundle"};
    if(!kinds.count(out.kind))return fail(reason,"kind \""+out.kind+"\" is not course, pod, menu_assets, replacement or bundle");
    // Phase 1: replacements only. Add-on items load through later phases.
    const Json* items=nullptr;
    if(!array(root,"items",items,true,Limits::items,reason))return false;
    if(!items->empty()||out.kind!="replacement")
        return fail(reason,"add-on items (kind \""+out.kind+"\") need a later version of the port; this build loads replacement packages only");

    const Json* files=nullptr;
    if(!array(root,"files",files,true,Limits::files,reason))return false;
    std::set<std::string> seen;
    for(const auto& row:*files){
        if(!row.is_object())return fail(reason,"files: every entry must be an object");
        FileRow file;std::string raw;
        if(!str(row,"path",raw,true,reason)||!unsignedNumber(row,"bytes",file.bytes,reason)||
           !str(row,"sha256",file.sha256,true,reason))return false;
        if(!normalisePackagePath(raw,file.path))return fail(reason,"files: path \""+raw+"\" is not a plain relative path");
        if(foldCase(file.path)=="dlc.json")return fail(reason,"files: dlc.json itself must not be listed");
        if(refusedFileType(file.path))return fail(reason,"files: \""+file.path+"\" is a program or script file; packages carry data only");
        file.sha256=lowerHex(file.sha256);
        if(!hex64(file.sha256))return fail(reason,"files: sha256 of \""+file.path+"\" is not 64 hex digits");
        if(!seen.insert(foldCase(file.path)).second)return fail(reason,"files: \""+file.path+"\" is listed twice");
        out.totalBytes+=file.bytes;
        if(file.bytes>Limits::packageBytes||out.totalBytes>Limits::packageBytes)return fail(reason,"package is larger than 512 MiB");
        out.files.push_back(std::move(file));
    }

    const Json* replacements=nullptr;
    if(!array(root,"replacements",replacements,false,Limits::replacements,reason))return false;
    if(replacements&&!replacements->empty()&&!out.replace)
        return fail(reason,"replacements are listed but \"replace\" is not true");
    std::set<std::string> targets;
    if(replacements)for(const auto& row:*replacements){
        if(!row.is_object())return fail(reason,"replacements: every entry must be an object");
        if(row.contains("member"))return fail(reason,"replacements: member-level replacement needs a later version of the port");
        Replacement item;std::string target,with;
        if(!str(row,"target",target,true,reason)||!str(row,"with",with,true,reason))return false;
        if(!normalisePackagePath(target,item.target))return fail(reason,"replacements: target \""+target+"\" is not a plain stock path");
        if(!normalisePackagePath(with,item.with))return fail(reason,"replacements: with \""+with+"\" is not a plain package path");
        const auto folded=foldCase(item.target);
        if(folded.rfind("dlc/",0)==0||folded=="dlc")return fail(reason,"replacements: target \""+item.target+"\" is inside the DLC folder");
        if(refusedFileType(item.target))return fail(reason,"replacements: target \""+item.target+"\" is a program file");
        if(!targets.insert(folded).second)return fail(reason,"replacements: target \""+item.target+"\" is replaced twice");
        const auto expects=row.find("expects");
        if(expects==row.end()||!expects->is_object())return fail(reason,"replacements: \""+item.target+"\" needs \"expects\" (the stock file it was built for)");
        if(!unsignedNumber(*expects,"bytes",item.expectBytes,reason)||!str(*expects,"sha256",item.expectSha256,true,reason))return false;
        item.expectSha256=lowerHex(item.expectSha256);
        if(!hex64(item.expectSha256))return fail(reason,"replacements: expects.sha256 of \""+item.target+"\" is not 64 hex digits");
        if(!seen.count(foldCase(item.with)))return fail(reason,"replacements: \""+item.with+"\" is not listed in files");
        out.replacements.push_back(std::move(item));
    }
    if(out.kind=="replacement"&&out.replacements.empty())return fail(reason,"kind \"replacement\" lists no replacements");

    const Json* dependencies=nullptr;
    if(!array(root,"dependencies",dependencies,false,Limits::dependencies,reason))return false;
    if(dependencies)for(const auto& row:*dependencies){
        if(!row.is_object())return fail(reason,"dependencies: every entry must be an object");
        Dependency item;
        if(!str(row,"id",item.id,true,reason)||!str(row,"version",item.range,false,reason)||
           !boolean(row,"optional",item.optional,false,reason))return false;
        if(!validId(item.id)||item.id==out.id)return fail(reason,"dependencies: \""+item.id+"\" is not another package id");
        if(!versionRangeValid(item.range))return fail(reason,"dependencies: version range \""+item.range+"\" is not understood");
        out.dependencies.push_back(std::move(item));
    }
    const Json* conflicts=nullptr;
    if(!array(root,"conflicts",conflicts,false,Limits::conflicts,reason))return false;
    if(conflicts)for(const auto& row:*conflicts){
        if(!row.is_object())return fail(reason,"conflicts: every entry must be an object");
        Conflict item;
        if(!str(row,"id",item.id,true,reason)||!str(row,"reason",item.reason,false,reason))return false;
        if(!validId(item.id)||item.id==out.id)return fail(reason,"conflicts: \""+item.id+"\" is not another package id");
        if(!printable(item.reason,Limits::noteChars))return fail(reason,"conflicts: reason is too long");
        out.conflicts.push_back(std::move(item));
    }
    return true;
}

bool checkFiles(const Package& package,std::string& reason){
    std::set<std::string> listed;for(const auto& f:package.files)listed.insert(foldCase(f.path));
    std::error_code error;std::size_t entries=0;
    fs::recursive_directory_iterator walk(package.folder,fs::directory_options::none,error),end;
    if(error)return fail(reason,"package folder could not be listed");
    for(;walk!=end;walk.increment(error)){
        if(error)return fail(reason,"package folder could not be listed");
        if(++entries>Limits::files*4)return fail(reason,"package folder holds too many entries");
        const auto& path=walk->path();
        if(reparsePoint(path)||walk->is_symlink(error))return fail(reason,"package contains a link or junction: "+path.filename().string());
        if(walk->is_directory(error)){
            if(walk.depth()+1>=static_cast<int>(Limits::pathDepth))return fail(reason,"package folders are nested too deeply");
            continue;
        }
        const auto relative=path.lexically_relative(package.folder).generic_u8string();
        const std::string rel(relative.begin(),relative.end());
        if(foldCase(rel)=="dlc.json")continue;
        if(!walk->is_regular_file(error))return fail(reason,"package contains something that is not a file: "+rel);
        if(!listed.count(foldCase(rel)))return fail(reason,"file "+rel+" is in the package folder but not listed in dlc.json");
    }
    for(const auto& file:package.files){
        const auto path=package.folder/fs::path(std::u8string(file.path.begin(),file.path.end()));
        if(!fs::is_regular_file(fs::symlink_status(path,error))||reparsePoint(path))
            return fail(reason,"file "+file.path+" listed in dlc.json is missing");
        const auto size=fs::file_size(path,error);
        if(error||size!=file.bytes)return fail(reason,"file "+file.path+" is "+std::to_string(error?0:size)+
            " bytes; dlc.json says "+std::to_string(file.bytes));
        if(executableHeader(path))return fail(reason,"file "+file.path+" is a program; packages carry data only");
        std::uint64_t bytes=0;std::string hex;
        if(!sha256File(path,bytes,hex)||bytes!=file.bytes)return fail(reason,"file "+file.path+" could not be read");
        if(hex!=file.sha256)return fail(reason,"file "+file.path+" sha256 differs from dlc.json (expected "+
            file.sha256.substr(0,12)+"..., found "+hex.substr(0,12)+"...)");
    }
    return true;
}

} // namespace rr::assets::dlc50DL
