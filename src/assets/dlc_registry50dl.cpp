#include "dlc_registry50dl.h"
#include "dlc_name_font50dl.h"
#include "dlc_package50dl.h"
// NOT ORIGINAL BEHAVIOUR (PC enhancement): see dlc_registry50dl.h.
// Contract: work\scratch\dlc\DLC-JSON.md, "Add-on items" (dlc-contract-2).
// This file owns the one registry store (struct Store below). It writes no
// file, no save byte, no original table and no game memory.
//
// HOOK (DLC save side file, helper 146s, src\assets\dlc_progress50dl.h/.cpp):
// the side file is loaded after publishRegistry() has frozen the rows; it keys
// everything by AddonPod::key / AddonCourse::key (and AddonCourse::recordsKey),
// never by number. Nothing here calls it yet.
#include "import_decode48c.h"
#include "../game/original_course_descriptor50kg.h"
#include "../game/original_ui_course_race_domain50kg.h"
#include "../../third_party/native-json/json.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <map>
#include <set>

namespace rr::assets::dlc50DL {
namespace fs=std::filesystem;
using Json=nlohmann::json;

namespace {
struct Store {
    bool frozen=false;
    std::size_t admittedPackages=0;
    std::vector<AddonPod> pods;          // number order (32, 33, ...)
    std::vector<AddonCourse> courses;    // number order
    std::vector<std::string> podMaterials,podUiMaterials;
    std::map<std::string,std::size_t> materialOwner; // folded name -> pods index
    // package id -> language -> KEY -> text
    std::map<std::string,std::map<std::string,std::map<std::string,std::string>>> strings;
};
Store& store(){static Store s;return s;}

bool fail(std::string& reason,const std::string& text){
    reason=text.size()>Limits::reasonChars?text.substr(0,Limits::reasonChars):text;return false;
}
std::string folded(std::string s){for(auto& c:s)if(c>='A'&&c<='Z')c=char(c-'A'+'a');return s;}
std::string upper(std::string s){for(auto& c:s)if(c>='a'&&c<='z')c=char(c-'a'+'A');return s;}
bool startsWith(const std::string& s,const std::string& p){return s.size()>=p.size()&&s.compare(0,p.size(),p)==0;}
bool endsWith(const std::string& s,const std::string& p){return s.size()>=p.size()&&s.compare(s.size()-p.size(),p.size(),p)==0;}
std::string quoted(std::string s){for(auto& c:s)if(c=='"')c='\'';return "\""+s+"\"";}
std::string dlcPath(const std::string& package,const std::string& path){return std::string(dlcScheme)+package+"/"+path;}
// Text a menu font draws: printable ASCII (0x20..0x7E, quotes included). The stock
// menu fonts (UI/UIFONTS.RES) draw every glyph of that range (checked by the arcade
// converters); characters outside it are refused, never drawn as garbage.
bool fontText(const std::string& s,std::size_t limit){
    if(s.empty()||s.size()>limit)return false;
    for(unsigned char c:s)if(c<0x20||c>0x7e)return false;
    return true;
}
bool keyText(const std::string& s){
    if(s.empty()||s.size()>RegistryLimits::keyChars)return false;
    for(char c:s)if(!((c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'))return false;
    return true;
}
bool itemId(const std::string& s){ // [a-z0-9][a-z0-9-]{1,46}
    if(s.size()<2||s.size()>47)return false;
    const auto ok=[](char c){return (c>='a'&&c<='z')||(c>='0'&&c<='9');};
    if(!ok(s[0]))return false;
    for(char c:s)if(!ok(c)&&c!='-')return false;
    return true;
}
bool recordsKeyText(const std::string& s){
    if(s.empty()||s.size()>47)return false;
    for(char c:s)if(!((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='-'))return false;
    return true;
}
bool stemText(const std::string& s){ // [a-z0-9_]{3,16}, not pod\d\d
    if(s.size()<3||s.size()>16)return false;
    for(char c:s)if(!((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'))return false;
    if(s.size()==5&&s.compare(0,3,"pod")==0&&s[3]>='0'&&s[3]<='9'&&s[4]>='0'&&s[4]<='9')return false;
    return true;
}
bool stockCode(const std::string& code){ // the 13 descriptor codes plus sb
    if(code=="sb")return true;
    for(const auto& d:rr::game::originalFreshCourseDescriptors50KG)
        if(code.size()==2&&folded(std::string{d.first,d.second})==code)return true;
    return false;
}
bool stockRacer(unsigned racer){return rr::game::nativeCourseRacerOwned50KG(racer);}

// ---- typed json readers: a wrong type is a refusal, never a silent default.
const Json* field(const Json& o,const char* key){const auto at=o.find(key);return at==o.end()?nullptr:&*at;}
bool text(const Json& o,const char* key,std::string& out,bool required,const std::string& where,std::string& reason){
    const Json* v=field(o,key);
    if(!v){if(required)return fail(reason,where+": missing field \""+key+"\"");return true;}
    if(!v->is_string())return fail(reason,where+": field \""+key+"\" must be text");
    out=v->get<std::string>();return true;
}
bool textList(const Json& o,const char* key,std::vector<std::string>& out,bool required,const std::string& where,std::string& reason){
    const Json* v=field(o,key);
    if(!v){if(required)return fail(reason,where+": missing field \""+key+"\"");return true;}
    if(!v->is_array())return fail(reason,where+": field \""+key+"\" must be a list");
    if(v->size()>RegistryLimits::uiEntries)return fail(reason,where+": too many entries in \""+key+"\"");
    for(const auto& e:*v){
        if(!e.is_string()||e.get<std::string>().empty())return fail(reason,where+": every entry of \""+key+"\" must be a member name");
        out.push_back(e.get<std::string>());
    }
    return true;
}
bool listedPath(const Package& p,const std::string& raw,std::string& normalised,const std::string& what,std::string& reason){
    if(!normalisePackagePath(raw,normalised))return fail(reason,what+" \""+raw+"\" is not a plain package path");
    for(const auto& f:p.files)if(foldCase(f.path)==foldCase(normalised)){normalised=f.path;return true;}
    return fail(reason,what+" \""+raw+"\" is not listed in files");
}
const FileRow* listedFile(const Package& p,const std::string& normalised){
    for(const auto& f:p.files)if(foldCase(f.path)==foldCase(normalised))return &f;
    return nullptr;
}
std::string packagePathOf(const std::string& dlc){ // dlc:/<id>/<path> -> <path>
    const auto at=dlc.find('/',std::string(dlcScheme).size());
    return at==std::string::npos?std::string():dlc.substr(at+1);
}
fs::path hostOf(const Package& p,const std::string& normalised){
    return p.folder/fs::path(std::u8string(normalised.begin(),normalised.end()));
}

// ---- archives (names only; the game's own loaders decode the content again)
bool readArchive(const fs::path& path,const DecodeLimits48C& limits,ResArchive48C& out,std::string& why){
    std::error_code error;const auto size=fs::file_size(path,error);
    if(error){why="cannot be read";return false;}
    if(!size||size>limits.resource_file_bytes){why="is outside the archive capacity ("+std::to_string(limits.resource_file_bytes)+" bytes)";return false;}
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
    std::ifstream in(path,std::ios::binary);
    in.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
    if(!in||in.gcount()!=static_cast<std::streamsize>(bytes.size())){why="cannot be read";return false;}
    DecodeError48C e;
    if(!decodeResource48C(bytes,out,e,nullptr,limits)){why="does not decode ("+e.code+")";return false;}
    return true;
}
bool readLoose(const fs::path& path,std::size_t cap,std::vector<std::uint8_t>& out){
    std::error_code error;const auto size=fs::file_size(path,error);
    if(error||!size||size>cap)return false;
    out.assign(static_cast<std::size_t>(size),0);
    std::ifstream in(path,std::ios::binary);
    in.read(reinterpret_cast<char*>(out.data()),static_cast<std::streamsize>(out.size()));
    return in&&in.gcount()==static_cast<std::streamsize>(out.size());
}
std::uint32_t word32(const std::vector<std::uint8_t>& b,std::size_t at){
    return at+4>b.size()?0xffffffffu:std::uint32_t(b[at])|std::uint32_t(b[at+1])<<8|std::uint32_t(b[at+2])<<16|std::uint32_t(b[at+3])<<24;
}
// PST shape (kind, levels, smallest-level width/height): the same layout rule as
// racer-arcade\tools\a2r\geo_pst.py parse() and tools\dlc\dlc_pack.py pst_shape():
// u16 1, u16 0, u32 kind 0/1/2, u32 levels 1..7, palette 16/256/0 words, then per
// level h,w,0,0 and its pixels (kind 2: u32 byte count, IPU stream); nothing after.
struct PstShape {unsigned kind=0,levels=0,width=0,height=0;};
bool pstShape(const std::vector<std::uint8_t>& b,PstShape& s){
    if(b.size()<12||(b[0]|b[1]<<8)!=1||(b[2]|b[3]<<8)!=0)return false;
    s.kind=word32(b,4);s.levels=word32(b,8);
    if(s.kind>2||s.levels<1||s.levels>7)return false;
    std::size_t at=12+4u*(s.kind==0?16u:s.kind==1?256u:0u);
    for(unsigned i=0;i<s.levels;++i){
        if(at+16>b.size())return false;
        const auto h=word32(b,at),w=word32(b,at+4);
        if(word32(b,at+8)||word32(b,at+12)||!w||!h||w>4096||h>4096)return false;
        std::size_t bytes;
        if(s.kind==2){if(at+20>b.size())return false;bytes=word32(b,at+16);at+=20;}
        else{bytes=s.kind==0?std::size_t(w)*h/2:std::size_t(w)*h;at+=16;}
        if(bytes>b.size()-at)return false;
        at+=bytes;s.width=w;s.height=h;
    }
    return at==b.size();
}
DecodeLimits48C podLimits(){
    DecodeLimits48C l;l.resource_file_bytes=RegistryLimits::podArchiveBytes;l.resource_decoded_bytes=RegistryLimits::podDecodedBytes;
    l.resource_entries=RegistryLimits::podEntries;l.resource_name_bytes=RegistryLimits::podNameBytes;return l;
}
DecodeLimits48C uiLimits(){
    DecodeLimits48C l;l.resource_file_bytes=RegistryLimits::uiArchiveBytes;l.resource_decoded_bytes=RegistryLimits::uiDecodedBytes;
    l.resource_entries=RegistryLimits::uiEntries;l.resource_name_bytes=RegistryLimits::podNameBytes;return l;
}
// Folded member names, refusing case-insensitive duplicates.
bool memberNames(const ResArchive48C& a,std::map<std::string,std::string>& out,std::string& dup){
    for(const auto& e:a.entries){
        const auto key=folded(e.name);
        if(!out.emplace(key,e.name).second){dup=e.name;return false;}
    }
    return true;
}
// Stock course facts read from the player's install, names only (inherit rule)
// and the hologram textures of UI/UICOURSE/UICOURSE.RES (preview rule).
struct StockArchive {bool ready=false;std::string error;ResArchive48C archive;std::map<std::string,std::string> names;};
const StockArchive& stockArchive(const fs::path& path,const DecodeLimits48C& limits){
    static std::map<std::wstring,StockArchive> cache;
    auto& s=cache[path.wstring()];
    if(s.ready||!s.error.empty())return s;
    std::string why,dup;
    if(!readArchive(path,limits,s.archive,why)){s.error=why;return s;}
    if(!memberNames(s.archive,s.names,dup)){s.error="lists "+dup+" twice";return s;}
    s.ready=true;return s;
}
// Shared original names: member names the stock archives of one kind carry in
// common (present in at least two different stock pod archives, or two stock
// Garage pod archives), so no pod-specific name such as textures\01_1 or
// flame01l can qualify. Read from the player's own install (the 23 stock pods
// of the original racer table), names only, and only when a package brings a
// pod item.
struct SharedNames {bool ready=false;std::string error;std::set<std::string> pod,ui;};
const SharedNames& sharedNames(const fs::path& stock){
    static std::map<std::wstring,SharedNames> cache;
    auto& s=cache[stock.wstring()];
    if(s.ready||!s.error.empty())return s;
    const auto common=[&](bool garage,const DecodeLimits48C& lim,std::set<std::string>& out)->bool{
        std::map<std::string,unsigned> seen;unsigned read=0;
        for(const auto racer:rr::game::nativeCourseRacers50KG){
            const char nn[3]={char('0'+racer/10),char('0'+racer%10),0};
            const auto path=garage?stock/"UI"/"UIGARAGE"/(std::string(nn)+"E.RES"):stock/"PODS"/("POD"+std::string(nn))/("POD"+std::string(nn)+".RES");
            std::error_code e;if(!fs::is_regular_file(path,e))continue;
            ResArchive48C x;std::string why;
            if(!readArchive(path,lim,x,why))continue;
            ++read;std::set<std::string> once;
            for(const auto& m:x.entries)if(once.insert(folded(m.name)).second)++seen[folded(m.name)];
        }
        if(read<2){s.error=garage?"fewer than two stock UI/UIGARAGE pod archives are readable":"fewer than two stock pod archives are readable";return false;}
        for(const auto& [name,count]:seen)if(count>=2)out.insert(name);
        return true;
    };
    if(!common(false,podLimits(),s.pod)||!common(true,uiLimits(),s.ui))return s;
    s.ready=true;return s;
}
std::string leafOf(const std::string& foldedName){ // "ui\\textures\\a2r_ark_1.psm" -> "a2r_ark_1"
    auto leaf=foldedName.substr(foldedName.find_last_of("\\/")==std::string::npos?0:foldedName.find_last_of("\\/")+1);
    const auto dot=leaf.find_last_of('.');
    return dot==std::string::npos?leaf:leaf.substr(0,dot);
}
std::string withoutExtension(const std::string& name){
    const auto slash=name.find_last_of("\\/"),dot=name.find_last_of('.');
    return dot==std::string::npos||(slash!=std::string::npos&&dot<slash)?name:name.substr(0,dot);
}
std::string extensionOf(const std::string& foldedName){
    const auto dot=foldedName.find_last_of('.');
    return dot==std::string::npos?std::string():foldedName.substr(dot+1);
}
// The stem rule (racer-arcade\tools\a2r\pod_stem.py): the stem replaces only the
// pod number of an original pod-specific name.
bool stemLeaf(const std::string& leaf,const std::string& stem){
    if(leaf==stem)return true;
    for(const char* tail:{"_","c","e"})if(startsWith(leaf,stem+tail))return true;
    return startsWith(leaf,"flame"+stem)||startsWith(leaf,"concring"+stem);
}
std::vector<std::string> pathParts(const std::string& foldedName){
    std::vector<std::string> out;std::string part;
    for(char c:foldedName){if(c=='\\'||c=='/'){out.push_back(part);part.clear();}else part.push_back(c);}
    out.push_back(part);return out;
}
// Two-digit pod folder (pods\pod04\, speech\pod04\): an original pod slot.
bool podSlotName(const std::string& part){
    return part.size()==5&&startsWith(part,"pod")&&part[3]>='0'&&part[3]<='9'&&part[4]>='0'&&part[4]<='9';
}

// Small INI reader for PHY and sounds.ini (sections and key = value).
std::map<std::string,std::map<std::string,std::string>> ini(const std::vector<std::uint8_t>& bytes){
    std::map<std::string,std::map<std::string,std::string>> out;std::string section,line;
    const auto flush=[&]{
        auto l=line;line.clear();
        if(const auto c=l.find(';');c!=std::string::npos)l=l.substr(0,c);
        while(!l.empty()&&(l.back()=='\r'||l.back()==' '||l.back()=='\t'))l.pop_back();
        std::size_t b=0;while(b<l.size()&&(l[b]==' '||l[b]=='\t'))++b;l=l.substr(b);
        if(l.empty())return;
        if(l.front()=='['&&l.back()==']'){section=folded(l.substr(1,l.size()-2));return;}
        const auto eq=l.find('=');if(eq==std::string::npos)return;
        auto k=l.substr(0,eq),v=l.substr(eq+1);
        while(!k.empty()&&(k.back()==' '||k.back()=='\t'))k.pop_back();
        std::size_t vb=0;while(vb<v.size()&&(v[vb]==' '||v[vb]=='\t'))++vb;v=v.substr(vb);
        if(v.size()>=2&&v.front()=='"'&&v.back()=='"')v=v.substr(1,v.size()-2);
        out[section].emplace(folded(k),v);
    };
    for(auto b:bytes){if(b=='\n')flush();else line.push_back(static_cast<char>(b));}
    flush();return out;
}
bool member(const ResArchive48C& a,const std::map<std::string,std::string>& names,const std::string& wanted,
            std::vector<std::uint8_t>& out,std::string& why){
    const auto at=names.find(folded(wanted));
    if(at==names.end()){why="lacks member "+wanted;return false;}
    DecodeError48C e;
    if(!extractResMember48C(a,at->second,out,e,RegistryLimits::textMemberBytes)){why="member "+wanted+" is outside the 8 KiB text capacity";return false;}
    return true;
}

// ---------------------------------------------------------------- items (json)
bool readStrings(const Package& p,const Json& root,PackageItems& out,std::string& reason){
    const Json* s=field(root,"strings");
    if(!s)return true;
    if(!s->is_object())return fail(reason,"strings must be an object of languages");
    if(s->size()>RegistryLimits::languages)return fail(reason,"strings: too many languages");
    std::size_t keys=0;
    for(auto lang=s->begin();lang!=s->end();++lang){
        const std::string language=lang.key();
        if(language.empty()||language.size()>16)return fail(reason,"strings: language \""+language+"\" is not a language name");
        for(char c:language)if(c<'a'||c>'z')return fail(reason,"strings: language \""+language+"\" is not a lower-case language name");
        if(!lang->is_object())return fail(reason,"strings: \""+language+"\" must be an object of KEY: text");
        for(auto k=lang->begin();k!=lang->end();++k){
            if(!keyText(k.key()))return fail(reason,"strings: key \""+k.key()+"\" is not [A-Z0-9_]{1,47}");
            if(!k->is_string())return fail(reason,"strings: "+language+"."+k.key()+" must be text");
            const auto t=k->get<std::string>();
            if(!fontText(t,RegistryLimits::stringBytes))
                return fail(reason,"strings: "+language+"."+k.key()+" must be 1-255 characters the menu fonts draw (printable ASCII)");
            // A display string can be a racer name: every font that draws a
            // racer name must draw it (dlc_name_font50dl.h, the one set).
            for(const char c:t)if(!dlcNameCharacter(c)){
                // Named by code too: the log quoting turns '"' into '\''.
                static constexpr char hex[]="0123456789abcdef";
                const std::string code{'0','x',hex[(unsigned char)c>>4],hex[(unsigned char)c&15]};
                const std::string shown=c=='"'?std::string("double quote"):"'"+std::string(1,c)+"'";
                return fail(reason,"strings: "+language+"."+k.key()+" uses "+shown+" ("+code+
                    "), which the race HUD font cannot draw (allowed: "+dlcNameCharacterSetText+")");
            }
            if(++keys>RegistryLimits::stringKeys)return fail(reason,"strings: more than 4096 keys");
            out.strings[language][k.key()]=t;
        }
    }
    (void)p;return true;
}
bool englishKey(const PackageItems& items,const std::string& key,std::string& textOut){
    const auto l=items.strings.find("english");if(l==items.strings.end())return false;
    const auto k=l->second.find(key);if(k==l->second.end())return false;
    textOut=k->second;return true;
}
bool readPod(const Package& p,const Json& o,PackageItems& items,AddonPod& pod,std::string& reason){
    const std::string where="pod item \""+pod.itemId+"\"";
    if(!text(o,"stem",pod.stem,true,where,reason))return false;
    if(!stemText(pod.stem))return fail(reason,where+": stem \""+pod.stem+"\" is not [a-z0-9_]{3,16} (and not pod plus two digits)");
    pod.podId=upper(pod.stem);
    std::string raw,norm;
    if(!text(o,"archive",raw,true,where,reason)||!listedPath(p,raw,norm,where+": archive",reason))return false;
    if(foldCase(norm)!="pods/"+pod.stem+"/"+pod.stem+".res")return fail(reason,where+": archive must be PODS/"+pod.podId+"/"+pod.podId+".RES");
    pod.archive=dlcPath(p.id,norm);
    if(const auto* f=listedFile(p,norm)){pod.archiveBytes=f->bytes;pod.archiveSha256=f->sha256;}
    raw.clear();
    if(!text(o,"camera",raw,true,where,reason)||!listedPath(p,raw,norm,where+": camera",reason))return false;
    if(foldCase(norm)!="pods/"+pod.stem+"/"+pod.stem+".cam")return fail(reason,where+": camera must be PODS/"+pod.podId+"/"+pod.podId+".CAM");
    pod.camera=dlcPath(p.id,norm);
    if(!text(o,"name_key",pod.nameKey,true,where,reason)||!text(o,"short_name_key",pod.shortNameKey,false,where,reason))return false;
    if(pod.shortNameKey.empty())pod.shortNameKey=pod.nameKey;
    if(!englishKey(items,pod.nameKey,pod.name))return fail(reason,where+": name_key \""+pod.nameKey+"\" has no English string");
    if(!englishKey(items,pod.shortNameKey,pod.shortName))return fail(reason,where+": short_name_key \""+pod.shortNameKey+"\" has no English string");
    const Json* ui=field(o,"ui");
    if(!ui||!ui->is_object())return fail(reason,where+": missing object \"ui\"");
    const std::string uw=where+" ui";
    raw.clear();
    if(!text(*ui,"archive",raw,true,uw,reason)||!listedPath(p,raw,norm,uw+": archive",reason))return false;
    if(foldCase(norm)!="ui/"+pod.stem+"_ui.res")return fail(reason,uw+": archive must be UI/"+pod.podId+"_UI.RES");
    pod.ui.archive=dlcPath(p.id,norm);
    if(!text(*ui,"rider_model",pod.ui.riderModel,true,uw,reason)||!text(*ui,"results_icon",pod.ui.resultsIcon,true,uw,reason)||
       !textList(*ui,"character",pod.ui.character,true,uw,reason)||!textList(*ui,"character_pages",pod.ui.characterPages,true,uw,reason)||
       !textList(*ui,"pod_pages",pod.ui.podPages,true,uw,reason)||!textList(*ui,"shadow",pod.ui.shadow,true,uw,reason)||
       !textList(*ui,"portraits",pod.ui.portraits,true,uw,reason)||!textList(*ui,"background",pod.ui.background,true,uw,reason)||
       !textList(*ui,"garage_model",pod.ui.garageModel,false,uw,reason))return false;
    if(pod.ui.riderModel.empty()||pod.ui.resultsIcon.empty())return fail(reason,uw+": rider_model and results_icon must name members");
    if(const Json* slot=field(*ui,"layout_slot")){
        std::uint64_t n=0;
        if(slot->is_number_unsigned())n=slot->get<std::uint64_t>();
        else if(slot->is_string()){const auto s=slot->get<std::string>();
            if(s.empty()||s.size()>2)return fail(reason,uw+": layout_slot must be a stock racer 01..26");
            for(const char c:s){if(c<'0'||c>'9')return fail(reason,uw+": layout_slot must be a stock racer 01..26");n=n*10+unsigned(c-'0');}}
        else return fail(reason,uw+": layout_slot must be a stock racer 01..26");
        if(n<1||n>26||n==5||n==6||n==13)return fail(reason,uw+": layout_slot must be a stock Rider-screen racer (not 5, 6, 13)");
        pod.ui.layoutSlot=static_cast<unsigned>(n);
    }
    // Rider-screen Huttese name (dlc_name_font50dl.h): its own key or name_key.
    if(!text(*ui,"huttese_name_key",pod.ui.hutteseNameKey,false,uw,reason))return false;
    if(pod.ui.hutteseNameKey.empty())pod.ui.hutteseNameKey=pod.nameKey;
    if(!englishKey(items,pod.ui.hutteseNameKey,pod.ui.hutteseName))
        return fail(reason,uw+": huttese_name_key \""+pod.ui.hutteseNameKey+"\" has no English string");
    for(const char c:pod.ui.hutteseName)if(!dlcHutteseNameCharacter(c)){
        static constexpr char hex[]="0123456789abcdef";
        const std::string code{'0','x',hex[(unsigned char)c>>4],hex[(unsigned char)c&15]};
        const std::string shown=c=='"'?std::string("double quote"):"'"+std::string(1,c)+"'";
        return fail(reason,uw+": huttese_name_key \""+pod.ui.hutteseNameKey+"\" uses "+shown+" ("+code+
            "), which the Rider screen's Huttese name font (font\\swhutt44) cannot draw (allowed: "+dlcHutteseNameCharacterSetText+
            "; give ui.huttese_name_key a string in that set)");
    }
    if(const Json* colors=field(*ui,"colors")){
        if(!colors->is_object())return fail(reason,uw+": colors must be an object {\"accent\": [r,g,b], \"secondary\": [r,g,b]}");
        for(auto k=colors->begin();k!=colors->end();++k)
            if(k.key()!="accent"&&k.key()!="secondary")return fail(reason,uw+": colors has an unknown field \""+k.key()+"\" (accent, secondary)");
        const auto rgb=[&](const char* name,std::uint32_t& out){
            const Json* v=field(*colors,name);
            if(!v||!v->is_array()||v->size()!=3)return fail(reason,uw+": colors."+name+" must be [r, g, b]");
            std::uint32_t packed=0;
            for(const auto& c:*v){
                if(!c.is_number_unsigned()||c.get<std::uint64_t>()>255)return fail(reason,uw+": colors."+name+" values must be whole numbers 0-255");
                packed=(packed<<8)|static_cast<std::uint32_t>(c.get<std::uint64_t>());
            }
            out=packed;return true;
        };
        if(!rgb("accent",pod.ui.accentColor)||!rgb("secondary",pod.ui.secondaryColor))return false;
        pod.ui.colorsAuthored=true;
    }
    // character = psg, mcf, vut and the six motions (original_ui_rider_character_assets50kg).
    {
        std::string base;
        for(const auto& m:pod.ui.character)if(extensionOf(folded(m))=="psg")base=withoutExtension(folded(m));
        if(base.empty())return fail(reason,uw+": character lists no .psg");
        std::set<std::string> want{base+".psg",base+".mcf",base+".vut"};
        for(const char* motion:{"_ent","_idl1","_idl2","_idl3","_breathe","_sel"})want.insert(base+motion+".mot");
        std::set<std::string> got;for(const auto& m:pod.ui.character)got.insert(folded(m));
        if(got!=want||pod.ui.character.size()!=want.size())
            return fail(reason,uw+": character must be "+base+".psg/.mcf/.vut and the six motions _ent _idl1 _idl2 _idl3 _breathe _sel");
    }
    if(const Json* v=field(o,"voice")){
        if(!v->is_object())return fail(reason,where+": voice must be an object");
        raw.clear();
        if(!text(*v,"ini",raw,true,where+" voice",reason)||!listedPath(p,raw,norm,where+": voice ini",reason))return false;
        const Json* first=field(*v,"first");const Json* count=field(*v,"count");
        if(!first||!first->is_number_unsigned()||!count||!count->is_number_unsigned()||count->get<std::uint64_t>()==0||
           first->get<std::uint64_t>()>255||count->get<std::uint64_t>()>255)
            return fail(reason,where+": voice needs whole numbers first (0-255) and count (1-255)");
        pod.voice=true;pod.voiceIni=dlcPath(p.id,norm);
        pod.voiceFirst=static_cast<unsigned>(first->get<std::uint64_t>());pod.voiceCount=static_cast<unsigned>(count->get<std::uint64_t>());
    }
    raw.clear();
    if(!text(o,"announcer",raw,false,where,reason))return false;
    if(!raw.empty()){if(!listedPath(p,raw,norm,where+": announcer",reason))return false;pod.announcer=dlcPath(p.id,norm);}
    std::string ai="addon_courses",unlock="always";
    if(!text(o,"ai",ai,false,where,reason)||!text(o,"unlock",unlock,false,where,reason))return false;
    if(ai=="anywhere")return fail(reason,where+": ai \"anywhere\" needs a later version of the port (contract 2: addon_courses or never)");
    if(ai!="addon_courses"&&ai!="never")return fail(reason,where+": ai must be addon_courses or never");
    pod.aiOnAddonCourses=ai=="addon_courses";
    if(unlock!="always")return fail(reason,where+": unlock must be \"always\" (contract 2)");
    return true;
}
bool readHallRows(const Json& o,const char* key,AddonHallRow (&rows)[3],bool count,const std::string& where,std::string& reason){
    const Json* v=field(o,key);
    if(!v)return true;
    if(!v->is_array()||v->size()>3)return fail(reason,where+": hall_defaults."+key+" must be a list of at most 3 rows");
    for(std::size_t i=0;i<v->size();++i){
        const auto& r=(*v)[i];
        if(!r.is_object())return fail(reason,where+": hall_defaults."+key+" rows must be objects");
        std::string name;if(!text(r,"name",name,true,where+" hall row",reason))return false;
        if(!fontText(name,20))return fail(reason,where+": hall_defaults names must be 1-20 printable characters (original 21-byte lane with its end byte)");
        rows[i].name=name;
        if(count){
            const Json* c=field(r,"count");
            if(!c||!c->is_number_unsigned()||c->get<std::uint64_t>()>999)return fail(reason,where+": hall_defaults knockouts need a whole count");
            rows[i].count=static_cast<unsigned>(c->get<std::uint64_t>());
        }else{
            const Json* t=field(r,"time");
            if(!t||!t->is_number()||t->get<double>()<=0||t->get<double>()>35999)return fail(reason,where+": hall_defaults times must be seconds above 0");
            rows[i].time=t->get<double>();
        }
    }
    return true;
}
bool readCourse(const Package& p,const Json& o,PackageItems& items,AddonCourse& c,std::string& reason){
    const std::string where="course item \""+c.itemId+"\"";
    if(!text(o,"code",c.code,true,where,reason))return false;
    if(c.code.size()!=2||c.code[0]<'a'||c.code[0]>'z'||!((c.code[1]>='a'&&c.code[1]<='z')||(c.code[1]>='0'&&c.code[1]<='9')))
        return fail(reason,where+": code \""+c.code+"\" must be 2 characters [a-z][a-z0-9]");
    if(stockCode(c.code))return fail(reason,where+": code \""+c.code+"\" is a stock course code");
    std::string raw,norm;
    if(!text(o,"archive",raw,true,where,reason)||!listedPath(p,raw,norm,where+": archive",reason))return false;
    if(foldCase(norm)!="tracks/"+c.code+".res")return fail(reason,where+": archive must be TRACKS/"+upper(c.code)+".RES");
    c.archive=dlcPath(p.id,norm);
    if(const auto* f=listedFile(p,norm)){c.archiveBytes=f->bytes;c.archiveSha256=f->sha256;}
    if(!text(o,"name_key",c.nameKey,true,where,reason)||!text(o,"planet_name_key",c.planetNameKey,false,where,reason))return false;
    if(!englishKey(items,c.nameKey,c.name))return fail(reason,where+": name_key \""+c.nameKey+"\" has no English string");
    if(!c.planetNameKey.empty()&&!englishKey(items,c.planetNameKey,c.planetName))
        return fail(reason,where+": planet_name_key \""+c.planetNameKey+"\" has no English string");
    std::string phy,ai;
    if(!text(o,"phy_class",phy,true,where,reason)||!text(o,"ai_class",ai,true,where,reason))return false;
    if(phy!="A"&&phy!="B"&&phy!="C")return fail(reason,where+": phy_class must be A, B or C");
    if(ai!="A"&&ai!="B"&&ai!="C")return fail(reason,where+": ai_class must be A, B or C");
    c.phyClass=phy[0];c.aiClass=ai[0];
    if(!text(o,"rival",c.rival,true,where,reason))return false;
    if(c.rival!="none"){
        if(startsWith(c.rival,"stock:")){
            const auto nn=c.rival.substr(6);
            if(nn.size()!=2||nn[0]<'0'||nn[0]>'9'||nn[1]<'0'||nn[1]>'9'||!stockRacer(unsigned((nn[0]-'0')*10+(nn[1]-'0'))))
                return fail(reason,where+": rival \""+c.rival+"\" is not an existing stock racer (stock:01..26)");
        }else{
            const auto slash=c.rival.find('/');
            if(slash==std::string::npos||!itemId(c.rival.substr(slash+1))||c.rival.substr(0,slash).size()<3)
                return fail(reason,where+": rival must be stock:NN, <package-id>/<pod-item-id> or none");
        }
    }
    const Json* ls=field(o,"loading_screen");
    if(!ls||!ls->is_object())return fail(reason,where+": missing object \"loading_screen\"");
    if(ls->contains("file")==ls->contains("stock"))return fail(reason,where+": loading_screen needs exactly one of file or stock");
    if(ls->contains("file")){
        if(!text(*ls,"file",raw,true,where+" loading_screen",reason)||!listedPath(p,raw,norm,where+": loading_screen file",reason))return false;
        if(foldCase(norm)!="ui/textures/ld_"+c.code+".pst")return fail(reason,where+": loading_screen file must be UI/TEXTURES/LD_"+upper(c.code)+".PST");
        c.loadingFile=dlcPath(p.id,norm);
    }else{
        if(!text(*ls,"stock",c.loadingStockCode,true,where+" loading_screen",reason))return false;
        c.loadingStockCode=folded(c.loadingStockCode);
        if(!stockCode(c.loadingStockCode)||c.loadingStockCode=="sb")return fail(reason,where+": loading_screen stock must be a stock course code");
        c.loadingStock=true;
    }
    if(const Json* cs=field(o,"course_screen")){
        if(!cs->is_object())return fail(reason,where+": course_screen must be an object");
        if(cs->contains("archive")||cs->contains("hologram"))return fail(reason,where+": own hologram archives need a later version of the port (contract 2: stock_template)");
        if(!text(*cs,"stock_template",c.courseScreenTemplate,true,where+" course_screen",reason))return false;
        c.courseScreenTemplate=folded(c.courseScreenTemplate);
        if(!stockCode(c.courseScreenTemplate)||c.courseScreenTemplate=="sb")return fail(reason,where+": course_screen stock_template must be a stock course code");
        raw.clear();if(!text(*cs,"preview",raw,false,where+" course_screen",reason))return false;
        if(!raw.empty()){
            if(!listedPath(p,raw,norm,where+": course_screen preview",reason))return false;
            if(extensionOf(foldCase(norm))!="pst")return fail(reason,where+": course_screen preview must be a .PST");
            c.coursePreview=dlcPath(p.id,norm);
        }
    }
    if(const Json* in=field(o,"inherit")){
        if(!in->is_object())return fail(reason,where+": inherit must be an object");
        if(!text(*in,"tuning",c.inheritTuning,false,where+" inherit",reason)||!text(*in,"particles",c.inheritParticles,false,where+" inherit",reason))return false;
        c.inheritTuning=folded(c.inheritTuning);c.inheritParticles=folded(c.inheritParticles);
        for(const auto* code:{&c.inheritTuning,&c.inheritParticles})
            if(!code->empty()&&(!stockCode(*code)||*code=="sb"))return fail(reason,where+": inherit values must be stock course codes");
    }
    if(const Json* m=field(o,"modes")){
        if(!m->is_array()||m->empty())return fail(reason,where+": modes must be a non-empty list");
        c.single=c.timeTrial=c.vs=false;
        for(const auto& e:*m){
            const std::string v=e.is_string()?e.get<std::string>():std::string();
            if(v=="circuit")return fail(reason,where+": mode circuit needs a later version of the port");
            if(v=="single")c.single=true;else if(v=="time_trial")c.timeTrial=true;else if(v=="vs")c.vs=true;
            else return fail(reason,where+": modes may only hold single, time_trial and vs");
        }
    }
    // course_background (146c): {"file": listed PST} or "stock:<code>"; absent = stock:<template>.
    if(const Json* b=field(o,"course_background")){
        if(b->is_string()){
            const auto v=b->get<std::string>();
            if(!startsWith(v,"stock:"))return fail(reason,where+": course_background must be {\"file\": ...} or \"stock:<code>\"");
            c.backgroundStockCode=folded(v.substr(6));
            if(!stockCode(c.backgroundStockCode)||c.backgroundStockCode=="sb")return fail(reason,where+": course_background stock must be a stock course code");
        }else if(b->is_object()&&b->size()==1&&b->contains("file")){
            if(!text(*b,"file",raw,true,where+" course_background",reason)||!listedPath(p,raw,norm,where+": course_background file",reason))return false;
            if(extensionOf(foldCase(norm))!="pst")return fail(reason,where+": course_background file must be a .PST");
            c.backgroundFile=dlcPath(p.id,norm);
        }else return fail(reason,where+": course_background must be {\"file\": ...} or \"stock:<code>\"");
    }else{c.backgroundStockCode=c.courseScreenTemplate;c.backgroundBorrowed=true;}
    c.recordsKey=c.itemId;
    if(!text(o,"records_key",c.recordsKey,false,where,reason))return false;
    if(!recordsKeyText(c.recordsKey))return fail(reason,where+": records_key must be [a-z0-9-]{1,47}");
    if(const Json* h=field(o,"hall_defaults")){
        if(!h->is_object())return fail(reason,where+": hall_defaults must be an object");
        if(!readHallRows(*h,"lap",c.lap,false,where,reason)||!readHallRows(*h,"race",c.race,false,where,reason)||
           !readHallRows(*h,"knockouts",c.knockouts,true,where,reason))return false;
        c.hallDefaults=true;
    }
    if(const Json* e=field(o,"enhancements")){
        if(!e->is_array()||e->size()>16)return fail(reason,where+": enhancements must be a short list");
        for(const auto& x:*e){
            if(!x.is_string()||!fontText(x.get<std::string>(),32))return fail(reason,where+": enhancements must be short words");
            c.enhancements.push_back(x.get<std::string>());
        }
    }
    return true;
}
} // namespace

bool readItems(const Package& p,PackageItems& out,std::string& reason){
    out=PackageItems{};
    Json root;
    try{root=Json::parse(p.manifestText);}catch(const std::exception&){return fail(reason,"dlc.json could not be read again");}
    if(!readStrings(p,root,out,reason))return false;
    const Json* items=field(root,"items");
    if(!items||!items->is_array())return fail(reason,"dlc.json: field \"items\" must be a list");
    std::set<std::string> ids;
    for(const auto& o:*items){
        if(!o.is_object())return fail(reason,"items: every entry must be an object");
        std::string kind,id;
        if(!text(o,"kind",kind,true,"items",reason)||!text(o,"id",id,true,"items",reason))return false;
        if(!itemId(id))return fail(reason,"items: id \""+id+"\" is not [a-z0-9][a-z0-9-]{1,46}");
        if(!ids.insert(id).second)return fail(reason,"items: id \""+id+"\" is used twice");
        if(kind=="pod"){
            AddonPod pod;pod.itemId=id;pod.packageId=p.id;pod.packageName=p.name;pod.key=p.id+"/"+id;
            if(!readPod(p,o,out,pod,reason))return false;
            for(const auto& other:out.pods)if(other.stem==pod.stem)return fail(reason,"items: stem \""+pod.stem+"\" is used twice");
            out.pods.push_back(std::move(pod));
        }else if(kind=="course"){
            AddonCourse c;c.itemId=id;c.packageId=p.id;c.packageName=p.name;c.key=p.id+"/"+id;
            if(!readCourse(p,o,out,c,reason))return false;
            for(const auto& other:out.courses)if(other.code==c.code)return fail(reason,"items: code \""+c.code+"\" is used twice");
            out.courses.push_back(std::move(c));
        }else return fail(reason,"items: kind \""+kind+"\" is not course or pod");
    }
    if(p.kind=="pod"&&(out.pods.empty()||!out.courses.empty()))return fail(reason,"kind \"pod\" must list only pod items (at least one)");
    if(p.kind=="course"&&(out.courses.empty()||!out.pods.empty()))return fail(reason,"kind \"course\" must list only course items (at least one)");
    if(p.kind=="bundle"&&out.pods.empty()&&out.courses.empty()&&p.replacements.empty())return fail(reason,"kind \"bundle\" lists nothing");
    return true;
}

bool checkItems(const Package& p,PackageItems& items,const fs::path& stock,std::string& reason){
    // Pods: archive, camera, menu archive; every member found by name.
    for(auto& pod:items.pods){
        const std::string where="pod "+pod.stem;
        const auto& shared=sharedNames(stock);
        if(!shared.ready)return fail(reason,where+": the shared original pod names cannot be read ("+shared.error+")");
        ResArchive48C a;std::string why;
        if(!readArchive(hostOf(p,packagePathOf(pod.archive)),podLimits(),a,why))return fail(reason,where+": archive "+packagePathOf(pod.archive)+" "+why);
        std::map<std::string,std::string> names;std::string dup;
        if(!memberNames(a,names,dup))return fail(reason,where+": archive member "+dup+" is listed twice");
        pod.podMembers=names.size();
        const std::string own="pods\\"+pod.stem+"\\",speech="audio\\speech\\"+pod.stem+"\\";
        for(const auto& [name,spelling]:names){
            const auto parts=pathParts(name);
            if(parts.size()>=2&&parts[0]=="pods"&&parts[1]!=pod.stem)
                return fail(reason,where+": archive member "+spelling+" is named for another pod (stem must be "+pod.stem+")");
            if(parts.size()>=3&&parts[0]=="audio"&&parts[1]=="speech"&&parts[2]!=pod.stem)
                return fail(reason,where+": archive member "+spelling+" is named for another pod (stem must be "+pod.stem+")");
            for(const auto& part:parts)if(podSlotName(part))return fail(reason,where+": archive member "+spelling+" still names an original pod slot");
            if(startsWith(name,own)||startsWith(name,speech))continue;
            if(stemLeaf(leafOf(name),pod.stem))continue;
            if(shared.pod.count(name))continue;
            return fail(reason,where+": archive member "+spelling+" is neither a shared original name nor named with the stem "+pod.stem);
        }
        std::vector<std::uint8_t> bytes;
        if(!member(a,names,own+pod.stem+".phy",bytes,why))return fail(reason,where+": archive "+why);
        const auto phy=ini(bytes);
        const auto setup=phy.find("pod setup");
        const std::string prefix=setup==phy.end()||!setup->second.count("prefix")?std::string():setup->second.at("prefix");
        if(folded(prefix)!=pod.stem)return fail(reason,where+": PHY [Pod Setup] Prefix is \""+prefix+"\"; it must equal the stem");
        // Stats bars: same keys and range rule as the stock PODPHYS reader
        // (original_ui_rider_profile50kg.cpp 187124..: full keys, finite, >= 0).
        // PORT STAND-IN: number syntax by strtod, not the UI page number reader.
        for(const char* section:{"baseabilities","maxabilities"}){
            const auto s=phy.find(section);
            const bool base=std::string(section)=="baseabilities";
            unsigned lane=0;
            for(const char* k:{"maxthrust","theoreticaltopspeed","spoileryawmoment","defense","baserepairamount","basecoolingtime"}){
                const std::string label=std::string(base?"[BaseAbilities] ":"[MaxAbilities] ")+k;
                if(s==phy.end()||!s->second.count(k))return fail(reason,where+": PHY lacks "+label+" (stats bars)");
                const auto& v=s->second.at(k);char* end=nullptr;
                const double value=std::strtod(v.c_str(),&end);
                if(v.empty()||!end||*end||!std::isfinite(value)||value<0||value>1000)
                    return fail(reason,where+": PHY "+label+" is not a finite number >= 0");
                (base?pod.baseAbilities:pod.maxAbilities)[lane++]=static_cast<float>(value);
            }
        }
        if(!member(a,names,own+pod.stem+".ai",bytes,why))return fail(reason,where+": archive "+why);
        if(!member(a,names,speech+"sounds.ini",bytes,why))return fail(reason,where+": archive "+why);
        const auto sounds=ini(bytes);const auto indices=sounds.find("indices");
        if(indices==sounds.end())return fail(reason,where+": sounds.ini lacks [Indices]");
        for(const auto& [k,v]:indices->second){
            if(!startsWith(k,"numberof"))continue;
            if(v.empty()||v.size()>3||v.find_first_not_of("0123456789")!=std::string::npos||std::stoul(v)>255)
                return fail(reason,where+": sounds.ini "+k+" exceeds its uint8 storage");
        }
        for(const char* table:{"curses.ini","taunts.ini","special.ini"})
            if(!member(a,names,speech+table,bytes,why))return fail(reason,where+": archive "+why);
        // Admitted material names: the pod's own .psm definitions named with the stem.
        for(const auto& [name,spelling]:names)
            if(extensionOf(name)=="psm"&&stemLeaf(leafOf(name),pod.stem))pod.materials.push_back(withoutExtension(name));
        // Camera presets (stock PODnn.CAM format; the race's CAM reader checks the content).
        {
            std::error_code e;const auto size=fs::file_size(hostOf(p,packagePathOf(pod.camera)),e);
            if(e||!size||size>RegistryLimits::cameraBytes)return fail(reason,where+": camera "+packagePathOf(pod.camera)+" is empty or larger than 64 KiB");
        }
        // Menu archive: every named member present, every member named once.
        ResArchive48C u;
        if(!readArchive(hostOf(p,packagePathOf(pod.ui.archive)),uiLimits(),u,why))return fail(reason,where+": menu archive "+packagePathOf(pod.ui.archive)+" "+why);
        std::map<std::string,std::string> uiNames;
        if(!memberNames(u,uiNames,dup))return fail(reason,where+": menu archive member "+dup+" is listed twice");
        pod.uiMembers=uiNames.size();
        std::set<std::string> named;
        std::vector<const std::vector<std::string>*> lists{&pod.ui.character,&pod.ui.characterPages,&pod.ui.podPages,&pod.ui.shadow,
            &pod.ui.portraits,&pod.ui.background,&pod.ui.garageModel};
        std::vector<std::string> all{pod.ui.riderModel,pod.ui.resultsIcon};
        for(const auto* l:lists)all.insert(all.end(),l->begin(),l->end());
        for(const auto& m:all){
            const auto key=folded(m);
            if(!uiNames.count(key))return fail(reason,where+": menu archive lacks member "+m+" named in dlc.json");
            if(!named.insert(key).second)return fail(reason,where+": menu member "+m+" is named twice in dlc.json");
        }
        for(const auto& [name,spelling]:uiNames){
            if(!named.count(name))return fail(reason,where+": menu archive member "+spelling+" is not named by any ui field");
            if(!stemLeaf(leafOf(name),pod.stem)&&!shared.ui.count(name))
                return fail(reason,where+": menu archive member "+spelling+" is neither a shared original name nor named with the stem "+pod.stem);
            if(extensionOf(name)=="psm"&&stemLeaf(leafOf(name),pod.stem))pod.uiMaterials.push_back(withoutExtension(name));
        }
        for(const auto* loose:{&pod.voiceIni,&pod.announcer}){
            if(loose->empty())continue;
            std::error_code e;const auto size=fs::file_size(hostOf(p,packagePathOf(*loose)),e);
            if(e||!size||size>RegistryLimits::textMemberBytes)return fail(reason,where+": "+packagePathOf(*loose)+" is outside the 8 KiB INI capacity");
        }
    }
    // Courses: archive members found by the declared code (names only).
    for(auto& c:items.courses){
        const std::string where="course "+c.code;
        ResArchive48C a;std::string why;
        if(!readArchive(hostOf(p,packagePathOf(c.archive)),DecodeLimits48C{},a,why))return fail(reason,where+": archive "+packagePathOf(c.archive)+" "+why);
        std::map<std::string,std::string> names;std::string dup;
        if(!memberNames(a,names,dup))return fail(reason,where+": archive member "+dup+" is listed twice");
        for(const auto& wanted:{"tracks\\"+c.code+".scb","tracks\\"+c.code+".tob","tracks\\"+c.code+".aub",
                                "tracks\\m"+c.code+".spl","tracks\\a"+c.code+".spl"})
            if(!names.count(wanted))return fail(reason,where+": archive lacks member "+wanted);
        for(const auto& [name,spelling]:names){
            const auto parts=pathParts(name);
            if(parts.size()==2&&parts[0]=="tracks"){
                const auto leaf=leafOf(name);
                if(leaf.size()==2&&leaf!=c.code&&stockCode(leaf))return fail(reason,where+": archive member "+spelling+" is named by stock course code "+leaf);
            }
        }
        c.ownTuning=names.count("tracks\\"+c.code+".tun")!=0;
        c.ownParticles=names.count("particle\\demoprt.prt")&&names.count("particle\\demoprt.pst")&&names.count("particle\\demoprt.psm");
        if(!c.ownTuning&&c.inheritTuning.empty())return fail(reason,where+": archive has no tracks\\"+c.code+".tun and no inherit.tuning");
        if(!c.ownParticles&&c.inheritParticles.empty())return fail(reason,where+": archive has no particle\\demoprt.prt/.pst/.psm and no inherit.particles");
        for(const auto* loose:{&c.loadingFile,&c.coursePreview,&c.backgroundFile}){
            if(loose->empty())continue;
            std::error_code e;const auto size=fs::file_size(hostOf(p,packagePathOf(*loose)),e);
            if(e||!size||size>RegistryLimits::loosePstBytes)return fail(reason,where+": "+packagePathOf(*loose)+" is empty or larger than 4 MiB");
        }
        // 146c: members the race path names by code (optional ones are facts, logged).
        c.cube=names.count("tracks\\"+c.code+".cub")!=0;
        c.tvCamera=names.count("tracks\\"+c.code+"tvcam.pos")!=0;
        c.cutCameras=0;
        for(unsigned n=1;n<=99;++n){
            const char nn[3]={char('0'+n/10),char('0'+n%10),0};
            if(!names.count("tracks\\"+c.code+nn+".vub"))break;
            ++c.cutCameras;
        }
        // Intro (contract 2): introAud.ini NumEvents and scrptLns.ini NumberOfLines,
        // read by key as the race's intro reader does; 0 is accepted, absent = 0.
        std::vector<std::uint8_t> text;
        const auto count=[&](const char* member,const char* section,const char* key,unsigned& value,bool& own)->bool{
            value=0;own=names.count(member)!=0;if(!own)return true;
            if(!rr::assets::dlc50DL::member(a,names,member,text,why))return fail(reason,where+": archive "+why);
            const auto parsed=ini(text);const auto s=parsed.find(section);
            const std::string v=s==parsed.end()||!s->second.count(key)?std::string():s->second.at(key);
            if(v.empty()||v.size()>4||v.find_first_not_of("0123456789")!=std::string::npos)
                return fail(reason,where+": "+member+" ["+section+"] "+key+" is not a whole number");
            value=unsigned(std::stoul(v));return true;
        };
        if(!count("audio\\introaud.ini","responseemitter","numevents",c.introEvents,c.ownIntro)||
           !count("audio\\scrptlns.ini","general","numberoflines",c.scriptLines,c.ownScript))return false;
        // Loading screen: the presenter's layout rule (one 640x448 indexed-8 level).
        // Its stock fingerprint check becomes a receipt (listed SHA-256 kept here).
        if(!c.loadingFile.empty()){
            std::vector<std::uint8_t> b;
            if(!readLoose(hostOf(p,packagePathOf(c.loadingFile)),RegistryLimits::loosePstBytes,b)||
               b.size()!=RegistryLimits::loadingScreenBytes||word32(b,0)!=1||word32(b,4)!=1||word32(b,8)!=1||
               word32(b,1036)!=448||word32(b,1040)!=640||word32(b,1044)||word32(b,1048))
                return fail(reason,where+": loading screen "+packagePathOf(c.loadingFile)+" is not the 640x448 one-level layout the loading presenter reads");
            if(const auto* f=listedFile(p,packagePathOf(c.loadingFile)))c.loadingSha256=f->sha256;
        }
        // Course-screen background: the 640x448 one-level indexed layout every stock
        // *_bkg.pst has and the background decoder reads (2A6490 / 24EA00).
        if(!c.backgroundFile.empty()){
            std::vector<std::uint8_t> b;
            if(!readLoose(hostOf(p,packagePathOf(c.backgroundFile)),RegistryLimits::loosePstBytes,b)||
               b.size()!=RegistryLimits::loadingScreenBytes||word32(b,0)!=1||word32(b,4)!=1||word32(b,8)!=1||
               word32(b,1036)!=448||word32(b,1040)!=640||word32(b,1044)||word32(b,1048))
                return fail(reason,where+": course background "+packagePathOf(c.backgroundFile)+" is not the 640x448 one-level layout of the stock course-screen backgrounds");
        }
        // Preview: same shape as the stock texture it stands in for
        // (ui\textures\holo_<template>.pst in the player's UI/UICOURSE/UICOURSE.RES).
        if(!c.coursePreview.empty()){
            std::vector<std::uint8_t> b;PstShape got,want;
            if(!readLoose(hostOf(p,packagePathOf(c.coursePreview)),RegistryLimits::loosePstBytes,b)||!pstShape(b,got))
                return fail(reason,where+": preview "+packagePathOf(c.coursePreview)+" is not a PST texture");
            const auto& ui=stockArchive(stock/"UI"/"UICOURSE"/"UICOURSE.RES",uiLimits());
            if(!ui.ready)return fail(reason,where+": stock UI/UICOURSE/UICOURSE.RES "+ui.error+" (needed for the preview check)");
            const std::string holo="ui\\textures\\holo_"+c.courseScreenTemplate+".pst";
            const auto at=ui.names.find(holo);DecodeError48C e;b.clear();
            if(at==ui.names.end()||!extractResMember48C(ui.archive,at->second,b,e,RegistryLimits::loosePstBytes)||!pstShape(b,want))
                return fail(reason,where+": stock "+holo+" cannot be read (needed for the preview check)");
            const auto shape=[](const PstShape& s){return "kind "+std::to_string(s.kind)+", "+std::to_string(s.levels)+" level(s), "+
                std::to_string(s.width)+"x"+std::to_string(s.height);};
            if(got.kind!=want.kind||got.levels!=want.levels||got.width!=want.width||got.height!=want.height)
                return fail(reason,where+": preview is "+shape(got)+"; the stock texture it stands in for ("+holo+") is "+shape(want));
            c.previewKind=got.kind;c.previewLevels=got.levels;c.previewWidth=got.width;c.previewHeight=got.height;
        }
        // Inherit: the named stock course's archive in the player's install must
        // carry the member it stands in for (members in the add-on archive win).
        const auto inherited=[&](const std::string& code,const std::vector<std::string>& wanted,const char* what)->bool{
            const auto& s=stockArchive(stock/"TRACKS"/(upper(code)+".RES"),DecodeLimits48C{});
            if(!s.ready)return fail(reason,where+": inherit."+what+" stock TRACKS/"+upper(code)+".RES "+s.error);
            for(const auto& m:wanted)if(!s.names.count(m))return fail(reason,where+": inherit."+what+" stock TRACKS/"+upper(code)+".RES lacks "+m);
            return true;
        };
        if(!c.ownTuning&&!inherited(c.inheritTuning,{"tracks\\"+c.inheritTuning+".tun"},"tuning"))return false;
        if(!c.ownParticles&&!inherited(c.inheritParticles,{"particle\\demoprt.prt","particle\\demoprt.pst","particle\\demoprt.psm"},"particles"))return false;
    }
    return true;
}

bool itemsCollide(const PackageItems& a,const PackageItems& b,std::string& what){
    for(const auto& x:a.pods)for(const auto& y:b.pods){
        if(x.stem==y.stem){what="pod stem "+x.stem;return true;}
        for(const auto& m:x.materials)for(const auto& n:y.materials)if(m==n){what="material name "+m;return true;}
        for(const auto& m:x.uiMaterials)for(const auto& n:y.uiMaterials)if(m==n){what="menu material name "+m;return true;}
    }
    for(const auto& x:a.courses)for(const auto& y:b.courses)if(x.code==y.code){what="course code "+x.code;return true;}
    return false;
}

void publishRegistry(std::vector<AdmittedItems>& admitted,std::size_t admittedPackages,std::vector<std::string>& lines){
    auto& s=store();
    if(s.frozen)return;
    Store next;
    for(auto& entry:admitted){
        auto& items=entry.items;
        for(auto& pod:items.pods){
            if(next.pods.size()>=RegistryLimits::pods){lines.push_back("dlc_item key="+pod.key+" kind=pod status=refused reason=\"more than 68 add-on pods\"");continue;}
            pod.number=RegistryLimits::firstNumber+static_cast<unsigned>(next.pods.size());
            next.pods.push_back(std::move(pod));
        }
        for(auto& c:items.courses){
            if(next.courses.size()>=RegistryLimits::courses){lines.push_back("dlc_item key="+c.key+" kind=course status=refused reason=\"more than 64 add-on courses\"");continue;}
            c.number=RegistryLimits::firstNumber+static_cast<unsigned>(next.courses.size());
            next.courses.push_back(std::move(c));
        }
        next.strings[entry.package->id]=items.strings;
    }
    for(std::size_t i=0;i<next.pods.size();++i){
        for(const auto& m:next.pods[i].materials){next.podMaterials.push_back(m);next.materialOwner.emplace(m,i);}
        for(const auto& m:next.pods[i].uiMaterials){next.podUiMaterials.push_back(m);next.materialOwner.emplace(m,i);}
    }
    // Rivals: stock:NN, an admitted add-on pod, or none (logged).
    for(auto& c:next.courses){
        if(startsWith(c.rival,"stock:"))c.rivalRacer=unsigned(std::stoul(c.rival.substr(6)));
        else if(c.rival!="none"){
            for(const auto& pod:next.pods)if(pod.key==c.rival)c.rivalRacer=pod.number;
            if(!c.rivalRacer)lines.push_back("dlc_rival key="+c.key+" rival="+c.rival+" status=not_admitted (no rival)");
        }
    }
    for(const auto& pod:next.pods)
        lines.push_back("dlc_item key="+pod.key+" kind=pod stem="+pod.stem+" number="+std::to_string(pod.number)+
            " camera=1 ui_members="+std::to_string(pod.uiMembers)+" voice="+(pod.voice?"1":"0")+
            " announcer="+(pod.announcer.empty()?"0":"1")+" ai="+(pod.aiOnAddonCourses?"addon_courses":"never")+
            " pod_members="+std::to_string(pod.podMembers)+" materials="+std::to_string(pod.materials.size())+
            " ui_materials="+std::to_string(pod.uiMaterials.size())+" name="+quoted(pod.name));
    for(const auto& c:next.courses)
        lines.push_back("dlc_item key="+c.key+" kind=course code="+c.code+" number="+std::to_string(c.number)+
            " phy_class="+c.phyClass+" ai_class="+c.aiClass+" rival="+(c.rivalRacer?std::to_string(c.rivalRacer):std::string("none"))+
            " loading="+(c.loadingStock?"stock:"+c.loadingStockCode:std::string("file"))+
            " course_background="+(c.backgroundFile.empty()?"stock:"+c.backgroundStockCode+(c.backgroundBorrowed?" background_borrowed=1":""):std::string("file"))+
            " course_screen=stock:"+c.courseScreenTemplate+
            (c.coursePreview.empty()?" preview=borrowed":" preview=file")+
            " inherit_tuning="+(c.ownTuning?std::string("unused"):c.inheritTuning.empty()?std::string("-"):c.inheritTuning)+
            " inherit_particles="+(c.ownParticles?std::string("unused"):c.inheritParticles.empty()?std::string("-"):c.inheritParticles)+
            " name="+quoted(c.name));
    next.admittedPackages=admittedPackages;
    next.frozen=true;
    s=std::move(next);
}

// ---------------------------------------------------------------- accessors
bool registryFrozen() noexcept {return store().frozen;}
std::size_t admittedPackageCount() noexcept {return store().admittedPackages;}
std::size_t addonPodCount() noexcept {return store().pods.size();}
const AddonPod* addonPodAt(std::size_t index) noexcept {
    const auto& s=store();return index<s.pods.size()?&s.pods[index]:nullptr;
}
const AddonPod* addonPod(unsigned racer) noexcept {
    const auto& s=store();
    if(racer<RegistryLimits::firstNumber)return nullptr;
    const std::size_t index=racer-RegistryLimits::firstNumber;
    return index<s.pods.size()?&s.pods[index]:nullptr;
}
const AddonPod* addonPodByPodId(const std::string& podId) noexcept {
    try{const auto key=folded(podId);for(const auto& p:store().pods)if(p.stem==key)return &p;}catch(...){}
    return nullptr;
}
std::size_t addonCourseCount() noexcept {return store().courses.size();}
const AddonCourse* addonCourseAt(std::size_t index) noexcept {
    const auto& s=store();return index<s.courses.size()?&s.courses[index]:nullptr;
}
const AddonCourse* addonCourse(unsigned number) noexcept {
    const auto& s=store();
    if(number<RegistryLimits::firstNumber)return nullptr;
    const std::size_t index=number-RegistryLimits::firstNumber;
    return index<s.courses.size()?&s.courses[index]:nullptr;
}
const AddonCourse* addonCourseByCode(const std::string& code) noexcept {
    try{const auto key=folded(code);for(const auto& c:store().courses)if(c.code==key)return &c;}catch(...){}
    return nullptr;
}
bool podRow(unsigned racer,PodRow& out){
    out=PodRow{};
    if(stockRacer(racer)){ // original Pod%02d rule (21081C/210820)
        const char digits[3]={char('0'+racer/10),char('0'+racer%10),0};
        out.racer=racer;out.podId=std::string("POD")+digits;out.memberStem=folded(out.podId);
        out.archive="PODS/"+out.podId+"/"+out.podId+".RES";out.camera="PODS/"+out.podId+"/"+out.podId+".CAM";
        return true;
    }
    const auto* row=addonPod(racer);
    if(!row)return false;
    out.racer=racer;out.addon=true;out.podId=row->podId;out.memberStem=row->stem;
    out.archive=row->archive;out.camera=row->camera;out.row=row;return true;
}
bool courseRow(unsigned number,CourseRow& out){
    out=CourseRow{};
    const auto& stock=rr::game::originalFreshCourseDescriptors50KG;
    if(number<stock.size()){
        out.number=number;out.code=folded(std::string{stock[number].first,stock[number].second});
        out.rivalRacer=static_cast<unsigned>(stock[number].rival11280);return true;
    }
    const auto* row=addonCourse(number);
    if(!row)return false;
    out.number=number;out.addon=true;out.code=row->code;out.rivalRacer=row->rivalRacer;out.row=row;return true;
}
const std::vector<std::string>& addonPodMaterialNames() noexcept {return store().podMaterials;}
const std::vector<std::string>& addonPodUiMaterialNames() noexcept {return store().podUiMaterials;}
const AddonPod* addonPodOwningMaterial(const std::string& name) noexcept {
    try{
        const auto& s=store();if(s.materialOwner.empty())return nullptr;
        const auto at=s.materialOwner.find(folded(name));
        return at==s.materialOwner.end()?nullptr:&s.pods[at->second];
    }catch(...){return nullptr;}
}
const std::string* packageString(const std::string& packageId,const std::string& key,const std::string& language) noexcept {
    try{
        const auto& s=store();const auto p=s.strings.find(packageId);if(p==s.strings.end())return nullptr;
        for(const auto* lang:{&language,static_cast<const std::string*>(nullptr)}){
            const auto l=p->second.find(lang?*lang:std::string("english"));
            if(l==p->second.end())continue;
            const auto k=l->second.find(key);if(k!=l->second.end())return &k->second;
        }
    }catch(...){}
    return nullptr;
}

} // namespace rr::assets::dlc50DL
