#include "dlc_files50dl.h"
#include "dlc_package50dl.h"
#include "dlc_registry50dl.h"
// NOT ORIGINAL BEHAVIOUR (PC enhancement). Design: work\scratch\dlc\DESIGN.md 2.5, 3, 5.
// This file owns the one session layout of the overlay (struct Session below).
// It writes no file, no save byte and no game memory; it only answers host().
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

namespace rr::assets::dlc50DL {
namespace fs=std::filesystem;
namespace {

struct Active {                 // one admitted whole-file replacement
    std::string target;         // stock path as the package spells it
    std::string package;        // package id
    fs::path file;              // the replacing package file (absolute)
    std::uintmax_t bytes=0;fs::file_time_type written{};
};
struct AddonFile {             // one listed file of an admitted add-on package
    fs::path file;std::uintmax_t bytes=0;fs::file_time_type written{};
};
struct Session {
    bool started=false;
    std::map<std::wstring,Active> replacements; // key: folded absolute stock path
    // "dlc:/<id>/<path>": key = "<id>/<folded path>". Only packages with add-on items.
    std::map<std::string,AddonFile> addonFiles;
};
Session& session(){static Session s;return s;}
std::once_flag once;

std::wstring key(const fs::path& path){
    std::error_code error;
    auto absolute=fs::absolute(path,error);
    if(error)absolute=path;
    auto text=absolute.lexically_normal().generic_wstring();
    for(auto& c:text)if(c>=L'A'&&c<=L'Z')c=wchar_t(c-L'A'+L'a');
    while(text.size()>1&&text.back()==L'/')text.pop_back();
    return text;
}
std::string utf8(const fs::path& path){
    const auto u=path.u8string();return std::string(u.begin(),u.end());
}
fs::path fromUtf8(const std::string& text){return fs::path(std::u8string(text.begin(),text.end()));}
std::string inQuotes(const std::string& text){
    std::string out="\"";
    for(char c:text)out.push_back(c=='"'?'\'':c);
    return out+"\"";
}
void line(const std::string& text){std::fprintf(stderr,"%s\n",text.c_str());std::fflush(stderr);}
const char* env(const char* name){const char* v=std::getenv(name);return v&&*v?v:nullptr;}

enum class Status{pending,admitted,refused,disabled,off};
struct Entry {
    Package package;Status status=Status::pending;std::string reason;std::string folderName;
    PackageItems items;             // add-on items (dlc-contract-2), dlc_registry50dl.cpp
};
bool before(const Entry* a,const Entry* b){
    if(a->package.priority!=b->package.priority)return a->package.priority>b->package.priority;
    return a->folderName<b->folderName;
}
void refuse(Entry& e,const std::string& reason){e.status=Status::refused;e.reason=reason;}

void run(){
    auto& s=session();s.started=true;
    const char* stockSetting=env("RR_ORIGINAL_ASSET_ROOT");
    if(!stockSetting){line("dlc_root unavailable reason=no_original_asset_root packages=0");return;}
    std::error_code error;
    const fs::path stock=fs::absolute(fs::path(stockSetting),error).lexically_normal();
    if(error){line("dlc_root unavailable reason=original_asset_root packages=0");return;}
    const char* rootSetting=env("RR_DLC_ROOT");
    const fs::path root=rootSetting?fs::absolute(fs::path(rootSetting),error).lexically_normal():stock/"DLC";
    const char* enabled=env("RR_DLC");
    if(enabled&&std::string(enabled)!="0"&&std::string(enabled)!="1"){
        line("dlc_root path="+inQuotes(utf8(root))+" disabled=1 reason=\"RR_DLC must be 0 or 1\" packages=0");return;
    }
    if(enabled&&std::string(enabled)=="0"){line("dlc_root path="+inQuotes(utf8(root))+" disabled=1 packages=0");return;}
    const char* switchSetting=env("RR_DLC_REPLACEMENTS");
    bool replacementsOn=true;
    if(switchSetting){
        const std::string v(switchSetting);
        if(v=="0")replacementsOn=false;
        else if(v!="1"){line("dlc_switch replacements=invalid value="+inQuotes(v)+" (replacements off)");replacementsOn=false;}
    }
    if(!fs::is_directory(fs::symlink_status(root,error))){line("dlc_root path="+inQuotes(utf8(root))+" packages=0");return;}

    std::vector<Entry> entries;
    for(fs::directory_iterator walk(root,error),end;!error&&walk!=end;walk.increment(error)){
        if(!walk->is_directory(error))continue; // loose files beside packages are ignored
        if(entries.size()>=256){line("dlc_root more than 256 package folders; the rest are ignored");break;}
        Entry e;e.folderName=utf8(walk->path().filename());e.package.folder=walk->path();
        entries.push_back(std::move(e));
    }
    if(error)line("dlc_root listing_error="+inQuotes(error.message()));
    std::sort(entries.begin(),entries.end(),[](const Entry& a,const Entry& b){return a.folderName<b.folderName;});
    line("dlc_root path="+inQuotes(utf8(root))+" packages="+std::to_string(entries.size())+
         " replacements_switch="+(replacementsOn?"on":"off"));
    if(entries.empty())return;

    // 1-2. Each package on its own: json, then every file.
    for(auto& e:entries){
        const fs::path folder=e.package.folder;std::string reason;
        if(!readManifest(folder,e.package,reason)){e.package.folder=folder;refuse(e,reason);continue;}
        if(!e.package.enabled){e.status=Status::disabled;e.reason="enabled is false in dlc.json";continue;}
        if(e.package.itemCount&&!readItems(e.package,e.items,reason)){refuse(e,reason);continue;}
        if(!replacementsOn&&!e.package.replacements.empty()){e.status=Status::off;e.reason="replacements are switched off";continue;}
        if(!checkFiles(e.package,reason)){refuse(e,reason);continue;}
        if(e.package.itemCount&&!checkItems(e.package,e.items,stock,reason)){refuse(e,reason);continue;}
        // 5. Replacements must be built for exactly the installed stock file.
        for(const auto& r:e.package.replacements){
            const auto target=stock/fromUtf8(r.target);
            std::uint64_t bytes=0;std::string hex;
            if(!fs::is_regular_file(target,error)||!sha256File(target,bytes,hex)){
                refuse(e,"replacement target "+r.target+" is not a stock file of this install");break;
            }
            if(bytes!=r.expectBytes||hex!=r.expectSha256){
                refuse(e,"built for a different stock file: "+r.target+" is "+std::to_string(bytes)+" bytes sha256 "+
                    hex.substr(0,12)+"..., package expects "+std::to_string(r.expectBytes)+" bytes sha256 "+
                    r.expectSha256.substr(0,12)+"...");break;
            }
        }
        if(e.status==Status::pending)e.status=Status::admitted;
    }
    std::map<std::string,Entry*> byId;
    for(auto& e:entries)if(!e.package.id.empty())byId[e.package.id]=&e;
    std::vector<Entry*> order;for(auto& e:entries)order.push_back(&e);
    std::sort(order.begin(),order.end(),before);

    // 3-5 repeated until nothing changes: a refusal can cascade to dependants.
    for(int pass=0;pass<int(entries.size())+2;++pass){
        bool changed=false;
        // Dependencies (missing, refused, disabled or wrong version).
        for(auto* e:order){
            if(e->status!=Status::admitted)continue;
            for(const auto& d:e->package.dependencies){
                const auto at=byId.find(d.id);
                const Entry* dep=at==byId.end()?nullptr:at->second;
                const bool ok=dep&&dep->status==Status::admitted&&versionSatisfies(dep->package.version,d.range);
                if(ok||d.optional)continue;
                std::string why=!dep?"is not installed":dep->status!=Status::admitted?"is not loaded":
                    "is version "+dep->package.versionText+", needs "+d.range;
                refuse(*e,"needs package "+d.id+", which "+why);changed=true;break;
            }
        }
        // Dependency cycles (required edges only): refuse every package on the cycle.
        for(auto* start:order){
            if(start->status!=Status::admitted)continue;
            std::vector<Entry*> stack{start};std::vector<std::size_t> next{0};
            while(!stack.empty()){
                auto* top=stack.back();auto& i=next.back();
                if(i>=top->package.dependencies.size()){stack.pop_back();next.pop_back();continue;}
                const auto& d=top->package.dependencies[i++];
                if(d.optional)continue;
                const auto at=byId.find(d.id);if(at==byId.end())continue;
                auto* dep=at->second;if(dep->status!=Status::admitted)continue;
                const auto loop=std::find(stack.begin(),stack.end(),dep);
                if(loop!=stack.end()){
                    for(auto it=loop;it!=stack.end();++it)refuse(**it,"dependency cycle through package "+dep->package.id);
                    changed=true;break;
                }
                if(stack.size()<=entries.size()){stack.push_back(dep);next.push_back(0);}
            }
        }
        // Declared conflicts: refuse the package that declares one with an admitted package.
        for(auto* e:order){
            if(e->status!=Status::admitted)continue;
            for(const auto& c:e->package.conflicts){
                const auto at=byId.find(c.id);
                if(at!=byId.end()&&at->second->status==Status::admitted){
                    refuse(*e,"conflicts with package "+c.id+(c.reason.empty()?"":" ("+c.reason+")"));changed=true;break;
                }
            }
        }
        // Replacement target claimed twice: highest priority wins; a tie refuses all of them.
        std::map<std::string,std::vector<Entry*>> claims;
        for(auto* e:order)if(e->status==Status::admitted)
            for(const auto& r:e->package.replacements)claims[foldCase(r.target)].push_back(e);
        for(auto& [target,who]:claims){
            if(who.size()<2)continue;
            const int top=who.front()->package.priority; // order is priority-first
            const bool tie=who[1]->package.priority==top;
            for(std::size_t i=tie?0:1;i<who.size();++i){
                if(who[i]->status!=Status::admitted)continue;
                std::string others;
                for(auto* o:who)if(o!=who[i])others+=(others.empty()?"":",")+o->package.id;
                refuse(*who[i],"replaces "+target+(tie?" at the same priority as ":" with lower priority than ")+others);
                changed=true;
            }
        }
        // Pod stem, course code or material name claimed twice: the first in
        // package order keeps it; the later package is refused as a whole.
        for(std::size_t i=0;i<order.size();++i){
            if(order[i]->status!=Status::admitted||!order[i]->package.itemCount)continue;
            for(std::size_t j=0;j<i;++j){
                std::string what;
                if(order[j]->status!=Status::admitted||!itemsCollide(order[j]->items,order[i]->items,what))continue;
                refuse(*order[i],what+" is already used by package "+order[j]->package.id);changed=true;break;
            }
        }
        if(!changed)break;
    }

    // Log one line per package, then the active replacements, then a summary.
    std::size_t admitted=0,refused=0,replacements=0;
    std::vector<AdmittedItems> addons;
    for(auto* e:order){
        const auto& p=e->package;
        const char* status=e->status==Status::admitted?"admitted":e->status==Status::disabled?"disabled":
            e->status==Status::off?"switched_off":"refused";
        std::string text="dlc_package id="+(p.id.empty()?e->folderName:p.id)+" version="+(p.versionText.empty()?"?":p.versionText)+
            " status="+status+" priority="+std::to_string(p.priority)+" items="+std::to_string(p.itemCount)+
            " replacements="+std::to_string(p.replacements.size());
        if(e->status!=Status::admitted)text+=" reason="+inQuotes(e->reason);
        line(text);
        if(e->status==Status::refused)++refused;
        if(p.personal&&e->status!=Status::refused)line("dlc_personal id="+p.id+" note="+inQuotes(p.personalNote));
        if(e->status!=Status::admitted)continue;
        ++admitted;
        if(p.itemCount){
            AdmittedItems a;a.package=&p;a.items=std::move(e->items);addons.push_back(std::move(a));
            for(const auto& f:p.files){
                const auto file=fs::absolute(p.folder/fromUtf8(f.path),error).lexically_normal();
                AddonFile row;row.file=file;row.bytes=fs::file_size(file,error);row.written=fs::last_write_time(file,error);
                s.addonFiles[p.id+"/"+foldCase(f.path)]=row;
            }
        }
        for(const auto& r:p.replacements){
            const auto file=fs::absolute(p.folder/fromUtf8(r.with),error).lexically_normal();
            Active a;a.target=r.target;a.package=p.id;a.file=file;
            a.bytes=fs::file_size(file,error);a.written=fs::last_write_time(file,error);
            std::string newSha;
            for(const auto& f:p.files)if(foldCase(f.path)==foldCase(r.with))newSha=f.sha256;
            s.replacements[key(stock/fromUtf8(r.target))]=a;++replacements;
            line("dlc_replace target="+r.target+" by="+p.id+" stock_sha="+r.expectSha256.substr(0,16)+
                 " new_sha="+newSha.substr(0,16)+" file="+inQuotes(utf8(file)));
        }
    }
    std::vector<std::string> itemLines;
    publishRegistry(addons,admitted,itemLines); // numbers in package order; freezes the registry
    for(const auto& l:itemLines)line(l);
    line("dlc_registry packages="+std::to_string(admitted)+" refused="+std::to_string(refused)+
         " courses="+std::to_string(addonCourseCount())+" pods="+std::to_string(addonPodCount())+
         " replacements="+std::to_string(replacements));
}
} // namespace

void startup() noexcept {
    try{std::call_once(once,[]{
        try{run();}catch(const std::exception& e){
            session().replacements.clear();session().addonFiles.clear();
            line(std::string("dlc_root failed reason=")+inQuotes(e.what())+" (stock files only this session)");
        }catch(...){session().replacements.clear();session().addonFiles.clear();line("dlc_root failed (stock files only this session)");}
    });}catch(...){}
}

bool replacing() noexcept {return !session().replacements.empty();}

fs::path host(const fs::path& path){
    const auto& s=session();
    if(!s.addonFiles.empty()){
        // Add-on file: "dlc:/<id>/<path>", a listed file of an admitted package.
        // Never falls back to stock and never leaves the package folder.
        const auto text=path.generic_u8string();
        const std::u8string scheme(u8"dlc:/");
        if(text.size()>scheme.size()&&text.compare(0,scheme.size(),scheme)==0){
            const std::string rest(text.begin()+static_cast<std::ptrdiff_t>(scheme.size()),text.end());
            const auto slash=rest.find('/');
            std::string normalised;
            const bool plain=slash!=std::string::npos&&normalisePackagePath(rest.substr(slash+1),normalised);
            const auto at=plain?s.addonFiles.find(rest.substr(0,slash)+"/"+foldCase(normalised)):s.addonFiles.end();
            if(at==s.addonFiles.end()){
                line("dlc_open_refused target="+inQuotes(rest)+" reason=\"not a listed file of an admitted add-on package\"");
                throw std::runtime_error("DLC: "+rest+" is not a listed file of an admitted add-on package");
            }
            std::error_code error;
            const auto bytes=fs::file_size(at->second.file,error);
            const auto written=error?fs::file_time_type{}:fs::last_write_time(at->second.file,error);
            if(error||bytes!=at->second.bytes||written!=at->second.written){
                line("dlc_open_refused target=dlc:/"+rest+" reason=\"the package file changed after startup\" file="+inQuotes(utf8(at->second.file)));
                throw std::runtime_error("DLC: dlc:/"+rest+" changed on disk after startup; restart the game");
            }
            line("dlc_open target=dlc:/"+rest+" by="+rest.substr(0,slash)+" file="+inQuotes(utf8(at->second.file)));
            return at->second.file;
        }
    }
    if(s.replacements.empty())return path; // stock: exactly the path the caller built
    const auto at=s.replacements.find(key(path));
    if(at==s.replacements.end())return path;
    const auto& a=at->second;
    std::error_code error;
    const auto bytes=fs::file_size(a.file,error);
    const auto written=error?fs::file_time_type{}:fs::last_write_time(a.file,error);
    if(error||bytes!=a.bytes||written!=a.written){
        line("dlc_open_refused target="+a.target+" by="+a.package+" reason=\"the package file changed after startup\" file="+inQuotes(utf8(a.file)));
        throw std::runtime_error("DLC package "+a.package+": "+a.target+" changed on disk after startup; restart the game");
    }
    line("dlc_open target="+a.target+" by="+a.package+" file="+inQuotes(utf8(a.file)));
    return a.file;
}

} // namespace rr::assets::dlc50DL
