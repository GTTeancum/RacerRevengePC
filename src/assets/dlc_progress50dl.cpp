#include "dlc_progress50dl.h"
// PORT EXTENSION, NOT ORIGINAL (PC enhancement). Design: work\scratch\dlc\DESIGN.md 6.
// Owns the DLC progress side file only. Never reads or writes the stock save,
// the stock Hall rows, any original table or game memory.
#include "../game/original_course_descriptor50kg.h"
#include "dlc_registry50dl.h"
#include "../../platform/linux/native/native_beta_file50kg.h"
#include "../../third_party/native-json/json.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <system_error>

namespace rr::assets::dlc50DL {
namespace fs=std::filesystem;
using Json=nlohmann::json;
using Hall=game::OriginalUiHallCourseRecords50KG;
using Name=game::race_record_rule50kg::Name;
namespace rule=game::race_record_rule50kg;
namespace beta=rr::platform::native_beta50kg;

struct DlcProgress50DL::Lock {beta::ProcessLock lock;};
DlcProgress50DL::DlcProgress50DL()=default;
DlcProgress50DL::~DlcProgress50DL()=default;

namespace {
void line(const std::string& text){std::fprintf(stderr,"%s\n",text.c_str());std::fflush(stderr);}
std::string utf8(const fs::path& path){const auto u=path.u8string();return std::string(u.begin(),u.end());}
std::string inQuotes(const std::string& text){
    std::string out="\"";for(char c:text)out.push_back(c=='"'?'\'':c);return out+"\"";
}
bool lowerDigit(char c){return (c>='a'&&c<='z')||(c>='0'&&c<='9');}
// [a-z0-9][a-z0-9-]{min-1,max-1}
bool idText(const std::string& s,std::size_t min,std::size_t max){
    if(s.size()<min||s.size()>max||!lowerDigit(s[0]))return false;
    for(char c:s)if(!lowerDigit(c)&&c!='-')return false;
    return true;
}
bool shaText(const std::string& s){
    if(s.size()!=64)return false;
    for(char c:s)if(!((c>='0'&&c<='9')||(c>='a'&&c<='f')))return false;
    return true;
}
bool nameText(const std::string& s){
    if(s.size()>ProgressLimits::nameChars)return false;
    for(char c:s)if(static_cast<unsigned char>(c)<0x20||static_cast<unsigned char>(c)>0x7e)return false;
    return true;
}
Name toName(const std::string& s){Name n{};std::memcpy(n.data(),s.data(),s.size());return n;}
std::string fromName(const Name& n){
    const void* end=std::memchr(n.data(),0,n.size());
    return std::string(n.data(),end?static_cast<const char*>(end)-n.data():n.size());
}
bool sameBits(float a,float b){std::uint32_t x,y;std::memcpy(&x,&a,4);std::memcpy(&y,&b,4);return x==y;}
// Shortest decimal that reads back as the same binary32 (stored values stay exact).
double floatJson(float f){
    char text[32];
    for(int digits=6;digits<=9;++digits){
        std::snprintf(text,sizeof text,"%.*g",digits,static_cast<double>(f));
        if(sameBits(std::strtof(text,nullptr),f))break;
    }
    return std::strtod(text,nullptr);
}
DlcProgress50DL::Table emptyTable(const std::optional<Hall>& defaults){
    DlcProgress50DL::Table t;t.rows=defaults?*defaults:emptyHallRows();return t;
}

// ---- parse ----
struct Parse {
    std::string& reason;
    bool fail(const std::string& why){reason=why;return false;}
    bool onlyKeys(const Json& o,std::initializer_list<const char*> keys,const std::string& where){
        for(auto it=o.begin();it!=o.end();++it){
            bool known=false;for(const char* k:keys)if(it.key()==k)known=true;
            if(!known)return fail(where+": unknown field \""+it.key()+"\"");
        }
        return true;
    }
    bool entryCommon(const Json& e,const std::string& where,Name& name,std::string& racer){
        const auto n=e.find("name");
        if(n==e.end()||!n->is_string()||!nameText(n->get<std::string>()))
            return fail(where+": name must be text of at most 20 printable characters");
        name=toName(n->get<std::string>());
        racer.clear();
        if(const auto r=e.find("racer");r!=e.end()){
            if(!r->is_string()||!validRacerReference(r->get<std::string>()))
                return fail(where+": racer must be \"stock:NN\" or an item key");
            racer=r->get<std::string>();
        }
        return true;
    }
    bool timeTable(const Json& hall,const char* key,std::array<float,3>& values,
        std::array<Name,3>& names,std::array<std::string,3>& racers,const std::string& where){
        const auto at=hall.find(key);if(at==hall.end())return true;
        const auto w=where+"."+key;
        if(!at->is_array()||at->size()>ProgressLimits::entries)return fail(w+": must be a list of at most 3 entries");
        for(std::size_t i=0;i<at->size();++i){
            const auto& e=(*at)[i];const auto ew=w+"["+std::to_string(i)+"]";
            if(e.is_null())continue;
            if(!e.is_object()||!onlyKeys(e,{"time","name","racer"},ew))return reason.empty()?fail(ew+": must be an entry or null"):false;
            const auto t=e.find("time");
            if(t==e.end()||!t->is_number())return fail(ew+": time must be a number");
            const float f=static_cast<float>(t->get<double>());
            if(!rule::ordinary(f)||!(f<=emptyRecordTime))return fail(ew+": time out of range");
            if(!entryCommon(e,ew,names[i],racers[i]))return false;
            values[i]=f;
        }
        return true;
    }
    bool countTable(const Json& hall,std::array<std::uint32_t,3>& values,
        std::array<Name,3>& names,std::array<std::string,3>& racers,const std::string& where){
        const auto at=hall.find("knockouts");if(at==hall.end())return true;
        const auto w=where+".knockouts";
        if(!at->is_array()||at->size()>ProgressLimits::entries)return fail(w+": must be a list of at most 3 entries");
        for(std::size_t i=0;i<at->size();++i){
            const auto& e=(*at)[i];const auto ew=w+"["+std::to_string(i)+"]";
            if(e.is_null())continue;
            if(!e.is_object()||!onlyKeys(e,{"count","name","racer"},ew))return reason.empty()?fail(ew+": must be an entry or null"):false;
            const auto c=e.find("count");
            if(c==e.end()||!c->is_number_unsigned()||c->get<std::uint64_t>()>0xffffffffull)
                return fail(ew+": count must be a whole number 0..4294967295");
            if(!entryCommon(e,ew,names[i],racers[i]))return false;
            values[i]=static_cast<std::uint32_t>(c->get<std::uint64_t>());
        }
        return true;
    }
    // Unsigned integer in [0,max]; anything else (fraction, sign, type) refused.
    bool unsignedIn(const Json& o,const char* key,std::uint64_t max,std::uint64_t& out,const std::string& where){
        const auto at=o.find(key);
        if(at==o.end()||!at->is_number_integer()||(!at->is_number_unsigned()&&at->get<std::int64_t>()<0))
            return fail(where+"."+key+": must be a whole number 0.."+std::to_string(max));
        const auto v=at->get<std::uint64_t>();if(v>max)return fail(where+"."+key+": must be 0.."+std::to_string(max));
        out=v;return true;
    }
    bool career(const Json& c,ProgressCareer& out,const std::string& where,bool v3){
        if(!c.is_object())return fail(where+": must be a career object");
        const bool keys=v3?onlyKeys(c,{"earned","unspent","kills","progress","pod","ai","name","boundary"},where):
            onlyKeys(c,{"earned","unspent","kills","progress","pod"},where);
        if(!keys)return reason.empty()?fail(where+": must be a career object"):false;
        std::uint64_t v=0;ProgressCareer next;
        if(!unsignedIn(c,"earned",0xffffffffull,v,where))return false;next.earned2190=std::uint32_t(v);
        if(!unsignedIn(c,"unspent",0xffffffffull,v,where))return false;next.unspent2194=std::uint32_t(v);
        if(!unsignedIn(c,"kills",0xffffffffull,v,where))return false;next.kills2198=std::uint32_t(v);
        if(!unsignedIn(c,"progress",ProgressLimits::progress,v,where))return false;next.progress219C=std::uint8_t(v);
        const auto pod=c.find("pod");
        if(pod==c.end())return fail(where+".pod: must be an object");
        if(!podRow(*pod,next.pod,where+".pod"))return false;
        if(v3){
            if(const auto ai=c.find("ai");ai!=c.end()){
                if(!ai->is_array()||ai->size()!=27)return fail(where+".ai: must be a list of 27 pod rows");
                for(std::size_t i=0;i<27;++i)if(!podRow((*ai)[i],next.ai[i],where+".ai["+std::to_string(i)+"]"))return false;
                next.aiStored=true;
            }
            if(const auto n=c.find("name");n!=c.end()){
                if(!n->is_string())return fail(where+".name: must be a string");
                next.name=n->get<std::string>();
            }
            if(c.find("boundary")!=c.end()){
                if(!unsignedIn(c,"boundary",1,v,where))return false;
                next.boundary102CC=std::uint32_t(v);
            }
        }
        if(!validCareer(next))return fail(where+": career values out of range");
        out=next;return true;
    }
    bool podRow(const Json& row,game::OriginalTournamentPodCareer50KG& out,const std::string& pw){
        const Json* pod=&row;game::OriginalTournamentPodCareer50KG next;
        if(!pod->is_object()||!onlyKeys(*pod,{"abilities","medals","remaining_prizes"},pw))
            return reason.empty()?fail(pw+": must be an object"):false;
        const auto list=[&](const char* key,std::size_t n)->const Json*{
            const auto at=pod->find(key);
            if(at==pod->end()||!at->is_array()||at->size()!=n){fail(pw+"."+key+": must be a list of "+std::to_string(n));return nullptr;}
            return &*at;};
        const auto* abilities=list("abilities",6);if(!abilities)return false;
        for(std::size_t i=0;i<6;++i){const auto& e=(*abilities)[i];
            if(!e.is_number())return fail(pw+".abilities: numbers only");
            const float f=static_cast<float>(e.get<double>());
            if(!(f==0.f||rule::ordinary(f)))return fail(pw+".abilities: finite values only");
            if(!(f>=0.f))return fail(pw+".abilities: values must be >= 0");
            next.abilities1920[i]=f;}
        const auto* medals=list("medals",13);if(!medals)return false;
        for(std::size_t i=0;i<13;++i){const auto& e=(*medals)[i];
            if(!e.is_number_integer()||(!e.is_number_unsigned()&&e.get<std::int64_t>()<0)||e.get<std::uint64_t>()>ProgressLimits::medal)
                return fail(pw+".medals: whole numbers 0..8");
            next.medals1938[i]=std::uint8_t(e.get<std::uint64_t>());}
        const auto* prizes=list("remaining_prizes",13);if(!prizes)return false;
        for(std::size_t i=0;i<13;++i){const auto& e=(*prizes)[i];
            if(!e.is_number_integer())return fail(pw+".remaining_prizes: whole numbers");
            const auto x=e.get<std::int64_t>();
            if(x<-32768||x>32767)return fail(pw+".remaining_prizes: must fit 16 bits");
            next.remainingPrizes1946[i]=std::int16_t(x);}
        out=next;return true;
    }
    bool table(const Json& hall,DlcProgress50DL::Table& out,const std::string& where){
        if(!hall.is_object())return fail(where+": must be an object");
        if(!onlyKeys(hall,{"lap","race","knockouts"},where))return false;
        out=emptyTable(std::nullopt);
        return timeTable(hall,"lap",out.rows.lap102F0,out.rows.lapNames102FC,out.lapRacers,where)&&
            timeTable(hall,"race",out.rows.race1033C,out.rows.raceNames10348,out.raceRacers,where)&&
            countTable(hall,out.rows.knockouts10388,out.rows.knockoutNames10394,out.knockoutRacers,where);
    }
    bool recordSet(const Json& o,DlcProgress50DL::RecordSet& out,const std::string& where,bool withKey){
        if(!o.is_object())return fail(where+": must be an object");
        if(withKey){if(!onlyKeys(o,{"records_key","archive_sha256","hall","previous"},where))return false;}
        else if(!onlyKeys(o,{"archive_sha256","hall"},where))return false;
        const auto s=o.find("archive_sha256");
        if(s==o.end()||!s->is_string()||!shaText(s->get<std::string>()))
            return fail(where+": archive_sha256 must be 64 lower-case hex digits");
        out.archiveSha256=s->get<std::string>();
        const auto h=o.find("hall");if(h==o.end())return fail(where+": missing hall");
        return table(*h,out.table,where+".hall");
    }
};

// ---- serialise ----
Json timeEntries(const std::array<float,3>& values,const std::array<Name,3>& names,
    const std::array<std::string,3>& racers){
    Json list=Json::array();
    for(std::size_t i=0;i<3;++i){
        const auto name=fromName(names[i]);
        if(sameBits(values[i],emptyRecordTime)&&name.empty()&&racers[i].empty()){list.push_back(nullptr);continue;}
        Json e=Json::object();e["time"]=floatJson(values[i]);e["name"]=name;
        if(!racers[i].empty())e["racer"]=racers[i];
        list.push_back(e);
    }
    return list;
}
Json tableJson(const DlcProgress50DL::Table& t){
    Json hall=Json::object();
    hall["lap"]=timeEntries(t.rows.lap102F0,t.rows.lapNames102FC,t.lapRacers);
    hall["race"]=timeEntries(t.rows.race1033C,t.rows.raceNames10348,t.raceRacers);
    Json ko=Json::array();
    for(std::size_t i=0;i<3;++i){
        const auto name=fromName(t.rows.knockoutNames10394[i]);
        if(t.rows.knockouts10388[i]==0&&name.empty()&&t.knockoutRacers[i].empty()){ko.push_back(nullptr);continue;}
        Json e=Json::object();e["count"]=t.rows.knockouts10388[i];e["name"]=name;
        if(!t.knockoutRacers[i].empty())e["racer"]=t.knockoutRacers[i];
        ko.push_back(e);
    }
    hall["knockouts"]=ko;
    return hall;
}
const char* loadName(DlcProgress50DL::LoadStatus s){
    using L=DlcProgress50DL::LoadStatus;
    switch(s){
    case L::notLoaded:return "not_loaded";case L::noPackages:return "no_packages";
    case L::fresh:return "fresh";case L::loaded:return "loaded";case L::setAside:return "set_aside";
    case L::newerSchema:return "newer_schema";case L::unavailable:return "unavailable";
    }
    return "?";
}
} // namespace

game::OriginalUiHallCourseRecords50KG emptyHallRows() noexcept {
    Hall h{};
    h.lap102F0.fill(emptyRecordTime);h.race1033C.fill(emptyRecordTime);
    return h;
}
bool validItemKey(const std::string& key) noexcept {
    const auto slash=key.find('/');
    if(slash==std::string::npos||key.find('/',slash+1)!=std::string::npos)return false;
    return idText(key.substr(0,slash),3,48)&&idText(key.substr(slash+1),2,47);
}
bool validRecordsKey(const std::string& key) noexcept {
    if(key.empty()||key.size()>47)return false;
    for(char c:key)if(!lowerDigit(c)&&c!='-')return false;
    return true;
}
bool validRacerReference(const std::string& ref) noexcept {
    if(ref.size()==8&&ref.compare(0,6,"stock:")==0){
        if(ref[6]<'0'||ref[6]>'9'||ref[7]<'0'||ref[7]>'9')return false;
        const int n=(ref[6]-'0')*10+(ref[7]-'0');return n>=1&&n<=26;
    }
    return validItemKey(ref);
}
bool stockCourseCode(const std::string& code,std::string& upper) noexcept {
    if(code.size()!=2)return false;
    std::string u=code;for(auto& c:u)if(c>='a'&&c<='z')c=char(c-'a'+'A');
    for(const auto& d:game::originalFreshCourseDescriptors50KG)
        if(u[0]==d.first&&u[1]==d.second){upper=u;return true;}
    return false;
}

bool DlcProgress50DL::parse(const std::string& text,State& out,bool& newerSchema,std::string& reason){
    newerSchema=false;reason.clear();
    Parse p{reason};State next;
    if(text.size()>ProgressLimits::fileBytes)return p.fail("file larger than 1 MiB");
    if(text.size()>=3&&static_cast<unsigned char>(text[0])==0xEF)return p.fail("file starts with a byte-order mark");
    Json root;
    try{root=Json::parse(text);}catch(const std::exception& e){return p.fail(std::string("not valid JSON: ")+e.what());}
    if(!root.is_object())return p.fail("must hold one JSON object");
    const auto schema=root.find("schema");
    if(schema==root.end()||!schema->is_string())return p.fail("missing schema");
    const auto name=schema->get<std::string>();
    // /3 is written; /2 (careers without ai/name/boundary, no unlocks) and /1
    // (no careers) are still read and rewritten as /3.
    const bool v1=name==progressSchemaV1,v2=name==progressSchemaV2,v3=name==progressSchema;
    if(!v3&&!v2&&!v1){
        if(name.rfind("rrpc-dlc-progress/",0)==0){newerSchema=true;return p.fail("schema \""+name+"\" is not "+progressSchema);}
        return p.fail("schema \""+name+"\" is not "+progressSchema);
    }
    if(!(v3?p.onlyKeys(root,{"schema","courses","stock_course_addon_pod_records","pods","careers","unlocks"},"file"):
            p.onlyKeys(root,{"schema","courses","stock_course_addon_pod_records","pods","careers"},"file")))return false;
    if(const auto c=root.find("courses");c!=root.end()){
        if(!c->is_object()||c->size()>ProgressLimits::courses)return p.fail("courses: must be an object of at most 1024 entries");
        for(auto it=c->begin();it!=c->end();++it){
            const auto where="courses."+it.key();
            if(!validItemKey(it.key()))return p.fail(where+": not an item key");
            CourseEntry entry;
            if(!p.recordSet(*it,entry.active,where,true))return false;
            const auto k=it->find("records_key");
            if(k==it->end()||!k->is_string()||!validRecordsKey(k->get<std::string>()))
                return p.fail(where+": records_key must be [a-z0-9-]{1,47}");
            entry.recordsKey=k->get<std::string>();
            if(const auto prev=it->find("previous");prev!=it->end()){
                if(!prev->is_object()||prev->size()>ProgressLimits::previousSets)
                    return p.fail(where+".previous: must be an object of at most 16 entries");
                for(auto pi=prev->begin();pi!=prev->end();++pi){
                    const auto pw=where+".previous."+pi.key();
                    if(!validRecordsKey(pi.key())||pi.key()==entry.recordsKey)
                        return p.fail(pw+": not a retired records_key");
                    RecordSet set;if(!p.recordSet(*pi,set,pw,false))return false;
                    entry.previous.emplace(pi.key(),std::move(set));
                }
            }
            next.courses.emplace(it.key(),std::move(entry));
        }
    }
    if(const auto s=root.find("stock_course_addon_pod_records");s!=root.end()){
        if(!s->is_object())return p.fail("stock_course_addon_pod_records: must be an object");
        for(auto it=s->begin();it!=s->end();++it){
            std::string upper;
            if(!stockCourseCode(it.key(),upper)||upper!=it.key())
                return p.fail("stock_course_addon_pod_records."+it.key()+": not a stock course code");
            Table t;if(!p.table(*it,t,"stock_course_addon_pod_records."+it.key()))return false;
            next.stockCourseAddonPod.emplace(upper,std::move(t));
        }
    }
    if(const auto pods=root.find("pods");pods!=root.end()){
        if(!pods->is_object()||pods->size()>ProgressLimits::pods)return p.fail("pods: must be an object of at most 1024 entries");
        for(auto it=pods->begin();it!=pods->end();++it){
            const auto where="pods."+it.key();
            if(!validItemKey(it.key()))return p.fail(where+": not an item key");
            if(!it->is_object()||!p.onlyKeys(*it,{"unlocked"},where))return reason.empty()?p.fail(where+": must be an object"):false;
            const auto u=it->find("unlocked");
            if(u==it->end()||!u->is_boolean())return p.fail(where+": unlocked must be true or false");
            next.pods.emplace(it.key(),u->get<bool>());
        }
    }
    if(const auto careers=root.find("careers");careers!=root.end()){
        if(!careers->is_object())return p.fail("careers: must be an object");
        if(v1&&!careers->empty())return p.fail("careers: must be empty in rrpc-dlc-progress/1");
        if(careers->size()>ProgressLimits::careers)return p.fail("careers: must hold at most 1024 entries");
        for(auto it=careers->begin();it!=careers->end();++it){
            const auto where="careers."+it.key();
            if(!validItemKey(it.key()))return p.fail(where+": not an item key");
            ProgressCareer c;if(!p.career(*it,c,where,v3))return false;
            next.careers.emplace(it.key(),c);
        }
    }
    if(const auto u=root.find("unlocks");v3&&u!=root.end()){
        if(!u->is_object())return p.fail("unlocks: must be an object");
        if(!p.onlyKeys(*u,{"stock_pods","gallery_tier"},"unlocks"))return false;
        if(const auto pods=u->find("stock_pods");pods!=u->end()){
            if(!pods->is_array()||pods->size()>27)return p.fail("unlocks.stock_pods: must be a list of at most 27 pods");
            for(const auto& e:*pods){
                if(!e.is_number_unsigned()||e.get<std::uint64_t>()>26)return p.fail("unlocks.stock_pods: pods 0..26 only");
                next.unlocks.stockPods[e.get<std::size_t>()]=true;
            }
        }
        if(u->find("gallery_tier")!=u->end()){
            std::uint64_t t=0;if(!p.unsignedIn(*u,"gallery_tier",3,t,"unlocks"))return false;
            next.unlocks.galleryTier=std::uint8_t(t);
        }
    }
    out=std::move(next);return true;
}

bool DlcProgress50DL::serialise(const State& state,std::string& text,std::string& reason){
    try {
        if(state.courses.size()>ProgressLimits::courses||state.pods.size()>ProgressLimits::pods||
           state.careers.size()>ProgressLimits::careers){
            reason="more entries than the file limits";return false;
        }
        Json root=Json::object();
        root["schema"]=progressSchema;
        Json courses=Json::object();
        for(const auto& [key,entry]:state.courses){
            if(entry.previous.size()>ProgressLimits::previousSets){reason="too many retired records keys";return false;}
            Json c=Json::object();
            c["records_key"]=entry.recordsKey;c["archive_sha256"]=entry.active.archiveSha256;
            c["hall"]=tableJson(entry.active.table);
            if(!entry.previous.empty()){
                Json prev=Json::object();
                for(const auto& [rk,set]:entry.previous){
                    Json s=Json::object();s["archive_sha256"]=set.archiveSha256;s["hall"]=tableJson(set.table);
                    prev[rk]=s;
                }
                c["previous"]=prev;
            }
            courses[key]=c;
        }
        root["courses"]=courses;
        Json stock=Json::object();
        for(const auto& [code,t]:state.stockCourseAddonPod)stock[code]=tableJson(t);
        root["stock_course_addon_pod_records"]=stock;
        Json pods=Json::object();
        for(const auto& [key,unlocked]:state.pods){Json p=Json::object();p["unlocked"]=unlocked;pods[key]=p;}
        root["pods"]=pods;
        Json careers=Json::object();
        const auto rowJson=[](const game::OriginalTournamentPodCareer50KG& r){
            Json pod=Json::object();Json abilities=Json::array(),medals=Json::array(),prizes=Json::array();
            for(const auto f:r.abilities1920)abilities.push_back(floatJson(f));
            for(const auto m:r.medals1938)medals.push_back(unsigned(m));
            for(const auto x:r.remainingPrizes1946)prizes.push_back(int(x));
            pod["abilities"]=abilities;pod["medals"]=medals;pod["remaining_prizes"]=prizes;return pod;};
        for(const auto& [key,c]:state.careers){
            if(!validCareer(c)){reason="career "+key+" out of range";return false;}
            Json e=Json::object();e["earned"]=c.earned2190;e["unspent"]=c.unspent2194;e["kills"]=c.kills2198;
            e["progress"]=unsigned(c.progress219C);e["pod"]=rowJson(c.pod);
            if(c.aiStored){Json ai=Json::array();for(const auto& r:c.ai)ai.push_back(rowJson(r));e["ai"]=ai;}
            if(!c.name.empty())e["name"]=c.name;
            if(c.boundary102CC)e["boundary"]=c.boundary102CC;
            careers[key]=e;
        }
        root["careers"]=careers;
        if(state.unlocks.galleryTier>3u){reason="gallery tier out of range";return false;}
        Json unlocks=Json::object();Json stockPods=Json::array();
        for(std::size_t i=0;i<state.unlocks.stockPods.size();++i)if(state.unlocks.stockPods[i])stockPods.push_back(i);
        unlocks["stock_pods"]=stockPods;unlocks["gallery_tier"]=unsigned(state.unlocks.galleryTier);
        root["unlocks"]=unlocks;
        text=root.dump(2)+"\n";
        if(text.size()>ProgressLimits::fileBytes){reason="file would exceed 1 MiB";return false;}
        return true;
    }catch(const std::exception& e){reason=e.what();return false;}
}

DlcProgress50DL::LoadStatus DlcProgress50DL::load(const fs::path& saveDirectory,const ProgressCatalog& catalog) noexcept {
    if(status_!=LoadStatus::notLoaded)return status_;
    try {
        if(catalog.admittedPackages==0){
            status_=LoadStatus::noPackages;
            line("dlc_progress status=no_packages (side file not read, not created)");
            return status_;
        }
        // Catalog rows the registry hands over; a bad row is skipped (logged).
        for(const auto& item:catalog.courses){
            bool dup=false;for(const auto& c:catalog_.courses)if(c.key==item.key)dup=true;
            if(dup||!validItemKey(item.key)||!validRecordsKey(item.recordsKey)||!shaText(item.archiveSha256)){
                line("dlc_progress catalog_row_refused kind=course key="+inQuotes(item.key));continue;
            }
            catalog_.courses.push_back(item);
        }
        for(const auto& item:catalog.pods){
            bool dup=false;for(const auto& c:catalog_.pods)if(c.key==item.key)dup=true;
            if(dup||!validItemKey(item.key)){line("dlc_progress catalog_row_refused kind=pod key="+inQuotes(item.key));continue;}
            catalog_.pods.push_back(item);
        }
        catalog_.admittedPackages=catalog.admittedPackages;
        directory_=saveDirectory;
        if(directory_.empty()){
            status_=LoadStatus::unavailable;line("dlc_progress status=unavailable reason=\"no save folder\"");return status_;
        }
        path_=directory_/progressFileName;
        std::error_code ec;fs::create_directories(directory_,ec);
        if(ec){status_=LoadStatus::unavailable;line("dlc_progress path="+utf8(path_)+" status=unavailable reason=\"save folder cannot be created\"");return status_;}
        lock_=std::make_unique<Lock>();
        if(lock_->lock.acquire(directory_/progressLockName)!=beta::Result::ok){
            status_=LoadStatus::unavailable;line("dlc_progress path="+utf8(path_)+" status=unavailable reason=\"side file locked by another session\"");return status_;
        }
        const auto st=fs::symlink_status(path_,ec);
        bool readable=true;
        if(!ec&&fs::exists(st)&&!fs::is_regular_file(st)){
            status_=LoadStatus::unavailable;line("dlc_progress path="+utf8(path_)+" status=unavailable reason=\"not a regular file\"");return status_;
        }
        beta::Bytes bytes;const auto read=beta::readFile(path_,bytes);
        std::string reason;State loaded;bool newer=false;
        if(read==beta::Result::missing){status_=LoadStatus::fresh;readable=false;}
        else if(read==beta::Result::ioError||read==beta::Result::unavailable){
            status_=LoadStatus::unavailable;line("dlc_progress path="+utf8(path_)+" status=unavailable reason=\"read error\"");return status_;
        }else if(read==beta::Result::invalid){reason="file larger than 1 MiB";}
        else if(parse(std::string(bytes.begin(),bytes.end()),loaded,newer,reason)){state_=std::move(loaded);status_=LoadStatus::loaded;}
        if(readable&&status_!=LoadStatus::loaded){
            if(newer){
                status_=LoadStatus::newerSchema;
                line("dlc_progress path="+utf8(path_)+" status=newer_schema reason="+inQuotes(reason)+" (file left untouched; DLC progress not saved this session)");
                return status_;
            }
            // Malformed: reported once, set aside, never half-loaded.
            const auto bad=directory_/progressBadFileName;
            fs::rename(path_,bad,ec);
            if(ec){std::error_code removed;fs::remove(bad,removed);ec.clear();fs::rename(path_,bad,ec);}
            if(ec){
                status_=LoadStatus::unavailable;
                line("dlc_progress malformed path="+utf8(path_)+" reason="+inQuotes(reason)+" set_aside=failed (file left in place; DLC progress not saved this session)");
                return status_;
            }
            status_=LoadStatus::setAside;state_={};
            line("dlc_progress malformed path="+utf8(path_)+" reason="+inQuotes(reason)+" set_aside="+utf8(bad));
        }
        writable_=true;
        // 6.3 against the admitted catalog.
        std::size_t shown=0,kept=0;
        for(const auto& item:catalog_.courses){
            const auto it=state_.courses.find(item.key);
            if(it==state_.courses.end())continue;
            auto& entry=it->second;
            if(entry.recordsKey!=item.recordsKey){
                // D10: the old set stays under the old key (not shown); the new key
                // starts from the package defaults, or from its own retired set.
                const bool restore=entry.previous.count(item.recordsKey)!=0;
                if(!restore&&entry.previous.size()>=ProgressLimits::previousSets){
                    blocked_.insert(item.key);
                    line("dlc_progress records_key_change_refused key="+item.key+" old="+entry.recordsKey+" new="+item.recordsKey+
                        " reason=\"16 retired records keys already kept\" (course records not shown)");
                    continue;
                }
                RecordSet next;
                if(restore){next=std::move(entry.previous[item.recordsKey]);entry.previous.erase(item.recordsKey);}
                else next.table=emptyTable(item.hallDefaults);
                entry.previous[entry.recordsKey]=std::move(entry.active);
                line("dlc_progress records_key_changed key="+item.key+" old="+entry.recordsKey+" new="+item.recordsKey+
                    " restored="+(restore?"1":"0"));
                entry.recordsKey=item.recordsKey;entry.active=std::move(next);
                if(entry.active.archiveSha256.empty())entry.active.archiveSha256=item.archiveSha256;
                dirty_=true;
            }
            if(entry.active.archiveSha256!=item.archiveSha256){
                line("dlc_progress archive_changed key="+item.key+" records_key="+entry.recordsKey+
                    " old_sha="+entry.active.archiveSha256.substr(0,16)+" new_sha="+item.archiveSha256.substr(0,16)+" records=kept");
                entry.active.archiveSha256=item.archiveSha256;dirty_=true;
            }
            ++shown;
        }
        for(const auto& [key,entry]:state_.courses){
            bool admitted=false;for(const auto& c:catalog_.courses)if(c.key==key)admitted=true;
            if(!admitted){++kept;line("dlc_progress kept_unloaded key="+key+" kind=course");}
        }
        for(const auto& [key,unlocked]:state_.pods){
            bool admitted=false;for(const auto& c:catalog_.pods)if(c.key==key)admitted=true;
            if(!admitted){++kept;line("dlc_progress kept_unloaded key="+key+" kind=pod");}
        }
        line("dlc_progress path="+utf8(path_)+" status="+loadName(status_)+" courses="+std::to_string(state_.courses.size())+
            " stock_tables="+std::to_string(state_.stockCourseAddonPod.size())+" pods="+std::to_string(state_.pods.size())+
            " shown="+std::to_string(shown)+" kept_unloaded="+std::to_string(kept)+" changed="+(dirty_?"1":"0"));
        return status_;
    }catch(...){
        status_=LoadStatus::unavailable;writable_=false;state_={};
        line("dlc_progress status=unavailable reason=\"internal error while loading\"");
        return status_;
    }
}

bool DlcProgress50DL::courseHall(const std::string& key,ProgressHallView& out) const noexcept {
    if(status_==LoadStatus::notLoaded||status_==LoadStatus::noPackages||blocked_.count(key))return false;
    for(const auto& item:catalog_.courses){
        if(item.key!=key)continue;
        const auto it=state_.courses.find(key);
        const Table t=it!=state_.courses.end()?it->second.active.table:emptyTable(item.hallDefaults);
        out={t.rows,t.lapRacers,t.raceRacers,t.knockoutRacers};
        return true;
    }
    return false;
}
bool DlcProgress50DL::stockCourseAddonPodHall(const std::string& code,ProgressHallView& out) const noexcept {
    std::string upper;
    if(status_==LoadStatus::notLoaded||status_==LoadStatus::noPackages||!stockCourseCode(code,upper))return false;
    const auto it=state_.stockCourseAddonPod.find(upper);
    const Table t=it!=state_.stockCourseAddonPod.end()?it->second:emptyTable(std::nullopt);
    out={t.rows,t.lapRacers,t.raceRacers,t.knockoutRacers};
    return true;
}
bool DlcProgress50DL::podUnlocked(const std::string& key) const noexcept {
    if(status_==LoadStatus::notLoaded||status_==LoadStatus::noPackages)return false;
    for(const auto& p:catalog_.pods)if(p.key==key)return true; // contract 2: "unlock": "always"
    return false;
}

ProgressSubmitOutcome DlcProgress50DL::submit(const ProgressRaceResult& r) noexcept {
    using S=ProgressSubmitOutcome::Status;
    ProgressSubmitOutcome out;
    const auto refuse=[&](const std::string& why){
        out.status=S::refused;out.reason=why;
        line("dlc_progress submit status=refused reason="+inQuotes(why));return out;
    };
    try {
        if(status_==LoadStatus::notLoaded||status_==LoadStatus::noPackages)return refuse("no DLC progress this session");
        const std::size_t humans=r.race1B0Low2==1u?1u:r.race1B0Low2==2u?2u:0u;
        if(!humans)return refuse("1AC440 handles one or two humans only");
        std::array<Name,2> names{};
        for(std::size_t i=0;i<humans;++i){
            const auto& racer=r.racers[i].racer;
            if(!validRacerReference(racer))return refuse("racer reference "+inQuotes(racer)+" is not stock:NN or an item key");
            if(racer.rfind("stock:",0)!=0&&!podUnlocked(racer))return refuse("racer "+racer+" is not an admitted add-on pod");
            if(!nameText(r.racers[i].name))return refuse("name must be at most 20 printable characters");
            names[i]=toName(r.racers[i].name);
        }
        // Target rows (a copy; published only after both rule bodies succeeded).
        Table current;std::string target;
        const bool addonCourse=!r.courseKey.empty();
        if(addonCourse==!r.stockCode.empty())return refuse("give exactly one of course key or stock code");
        std::string code;
        if(addonCourse){
            const ProgressCourseItem* item=nullptr;
            for(const auto& c:catalog_.courses)if(c.key==r.courseKey)item=&c;
            if(!item||blocked_.count(r.courseKey))return refuse("course "+r.courseKey+" is not an admitted add-on course");
            const auto it=state_.courses.find(r.courseKey);
            if(it==state_.courses.end()&&state_.courses.size()>=ProgressLimits::courses)return refuse("course entry limit reached");
            current=it!=state_.courses.end()?it->second.active.table:emptyTable(item->hallDefaults);
            target="course:"+r.courseKey;
        }else{
            if(!stockCourseCode(r.stockCode,code))return refuse("not a stock course code: "+r.stockCode);
            // D7 (b): only add-on pod records go to the side file; stock pods on a
            // stock course belong to the stock Hall (one owner per record).
            for(std::size_t i=0;i<humans;++i)
                if(r.racers[i].racer.rfind("stock:",0)==0)
                    return refuse("stock pod on stock course "+code+" belongs to the stock Hall");
            const auto it=state_.stockCourseAddonPod.find(code);
            current=it!=state_.stockCourseAddonPod.end()?it->second:emptyTable(std::nullopt);
            target="stock:"+code;
        }
        // 1ABFC0, selector 0 (race end: either player), on this one row.
        const game::LapRecordConfig50ID config{r.subtype11258,r.lapCount1125C,r.packed1125C};
        const std::array<game::OriginalRaceRecordQualificationRacerObservation50KG,2> observed{
            r.racers[0].values,r.racers[1].values};
        rule::QualificationResult q;rule::QualificationDiagnostic qd;
        if(!rule::qualify1ABFC0(config,rule::HallRowTable{&current.rows,1},r.unlock10FAC,0,
            r.race1B0Low2,0,observed.data(),humans,q,qd))
            return refuse("1ABFC0 refused at "+std::to_string(qd.sourcePC));
        if(!q.originalV0){
            out.status=S::notQualified;
            line("dlc_progress submit target="+target+" racer0="+r.racers[0].racer+" status=not_qualified");
            return out;
        }
        // 1AC440 on the real row and on a tag row with the same values: the tag
        // names follow the original shifts, so racer references move with names.
        Table next=current;Hall tags=current.rows;
        for(std::size_t i=0;i<3;++i){
            const Name tag=toName(std::string(1,char('a'+i)));
            tags.lapNames102FC[i]=tag;tags.raceNames10348[i]=tag;tags.knockoutNames10394[i]=tag;
        }
        const std::array<Name,2> playerTags{toName("P"),toName("Q")};
        rule::InsertionRanks first,second,tagFirst,tagSecond;std::uint32_t failPC=0;
        bool ok;
        if(humans==1){
            ok=rule::insertSingleHuman1AC440(next.rows,r.subtype11258,r.lapCount1125C,r.packed1125C,
                observed[0].records,names[0],first,failPC)&&
               rule::insertSingleHuman1AC440(tags,r.subtype11258,r.lapCount1125C,r.packed1125C,
                observed[0].records,playerTags[0],tagFirst,failPC);
        }else{
            const std::array<game::OriginalRaceRecordRacerObservation50KG,2> values{
                observed[0].records,observed[1].records};
            ok=rule::insertTwoHumans1AC440(next.rows,r.subtype11258,r.lapCount1125C,r.packed1125C,
                values,names,first,second,failPC)&&
               rule::insertTwoHumans1AC440(tags,r.subtype11258,r.lapCount1125C,r.packed1125C,
                values,playerTags,tagFirst,tagSecond,failPC);
        }
        if(!ok)return refuse("1AC440 refused at "+std::to_string(failPC));
        if(std::memcmp(next.rows.lap102F0.data(),tags.lap102F0.data(),sizeof tags.lap102F0)||
           std::memcmp(next.rows.race1033C.data(),tags.race1033C.data(),sizeof tags.race1033C)||
           next.rows.knockouts10388!=tags.knockouts10388)return refuse("internal: tag row diverged");
        const auto mapRacers=[&](const std::array<Name,3>& tagNames,const std::array<std::string,3>& before,
            std::array<std::string,3>& after){
            for(std::size_t i=0;i<3;++i){
                const char t=tagNames[i][0];
                after[i]=t>='a'&&t<='c'?before[std::size_t(t-'a')]:t=='P'?r.racers[0].racer:
                    t=='Q'?r.racers[1].racer:std::string();
            }
        };
        mapRacers(tags.lapNames102FC,current.lapRacers,next.lapRacers);
        mapRacers(tags.raceNames10348,current.raceRacers,next.raceRacers);
        mapRacers(tags.knockoutNames10394,current.knockoutRacers,next.knockoutRacers);
        const bool any=first.lap||first.race||first.knockouts||second.lap||second.race||second.knockouts;
        if(any){
            if(addonCourse){
                auto it=state_.courses.find(r.courseKey);
                if(it==state_.courses.end()){
                    const ProgressCourseItem* item=nullptr;
                    for(const auto& c:catalog_.courses)if(c.key==r.courseKey)item=&c;
                    CourseEntry entry;entry.recordsKey=item->recordsKey;entry.active.archiveSha256=item->archiveSha256;
                    it=state_.courses.emplace(r.courseKey,std::move(entry)).first;
                }
                it->second.active.table=std::move(next);
            }else state_.stockCourseAddonPod[code]=std::move(next);
            dirty_=true;
        }
        out.status=S::recorded;out.first=first;out.second=second;
        line("dlc_progress submit target="+target+" racer0="+r.racers[0].racer+
            (humans==2?" racer1="+r.racers[1].racer:std::string())+" status=recorded lap="+std::to_string(first.lap)+
            " race="+std::to_string(first.race)+" knockouts="+std::to_string(first.knockouts)+
            (humans==2?" lap1="+std::to_string(second.lap)+" race1="+std::to_string(second.race)+
                " knockouts1="+std::to_string(second.knockouts):std::string())+" changed="+(any?"1":"0"));
        return out;
    }catch(...){return refuse("internal error");}
}

DlcProgress50DL::SaveStatus DlcProgress50DL::saveIfChanged() noexcept {
    if(!dirty_)return SaveStatus::unchanged;
    if(!writable_||!lock_||!lock_->lock.held()){
        if(!notWritableLogged_){notWritableLogged_=true;
            line(std::string("dlc_progress save_skipped status=")+loadName(status_)+" reason=\"side file not writable this session\"");}
        return SaveStatus::notWritable;
    }
    try {
        std::string text,reason;
        if(!serialise(state_,text,reason)){line("dlc_progress save_refused reason="+inQuotes(reason));return SaveStatus::tooLarge;}
        const beta::Bytes bytes(text.begin(),text.end());
        const auto result=beta::writeFile(path_,bytes);
        if(result==beta::Result::ok||result==beta::Result::durabilityUnknown){
            dirty_=false;
            line("dlc_progress saved path="+utf8(path_)+" bytes="+std::to_string(bytes.size())+
                (result==beta::Result::ok?"":" durability=unknown"));
            return SaveStatus::saved;
        }
        line("dlc_progress save_failed path="+utf8(path_));return SaveStatus::writeError;
    }catch(...){line("dlc_progress save_failed reason=\"internal error\"");return SaveStatus::writeError;}
}

// ---- Add-on pod careers (schema /2). PORT EXTENSION, NOT ORIGINAL.
namespace {
bool validPodRow(const game::OriginalTournamentPodCareer50KG& r) noexcept {
    for(const auto f:r.abilities1920)if(!(f==0.f||rule::ordinary(f))||!(f>=0.f))return false;
    for(const auto m:r.medals1938)if(m>ProgressLimits::medal)return false;
    return true;
}
}
bool validCareer(const ProgressCareer& c) noexcept {
    if(c.progress219C>ProgressLimits::progress||c.boundary102CC>1u||!validPodRow(c.pod))return false;
    if(c.aiStored)for(const auto& r:c.ai)if(!validPodRow(r))return false;
    if(c.name.size()>ProgressLimits::nameChars)return false;
    for(const char ch:c.name)if(static_cast<unsigned char>(ch)<0x20u||static_cast<unsigned char>(ch)>0x7Eu)return false;
    return true;
}
bool DlcProgress50DL::grantStockPod(unsigned pod) noexcept {
    if(pod>=state_.unlocks.stockPods.size())return false;   // bounds: stock pods 0..26
    if(!state_.unlocks.stockPods[pod]){state_.unlocks.stockPods[pod]=true;dirty_=true;}
    return true;
}
bool DlcProgress50DL::grantGalleryTier(std::uint8_t tier) noexcept {
    if(tier>3u)return false;                                 // bounds: 11018 bits 5..6
    if(tier>state_.unlocks.galleryTier){state_.unlocks.galleryTier=tier;dirty_=true;}
    return true;
}
ProgressCareer seededCareer(const float (&baseAbilities)[6]) noexcept {
    ProgressCareer c;
    for(std::size_t i=0;i<6;++i)c.pod.abilities1920[i]=baseAbilities[i];
    c.pod.remainingPrizes1946=game::originalTournamentPrizes50KG;
    return c;
}
bool DlcProgress50DL::careerSnapshot(const std::string& podKey,ProgressCareer& out) const noexcept {
    try{
        if(!podUnlocked(podKey))return false;
        const auto it=state_.careers.find(podKey);
        if(it!=state_.careers.end()){out=it->second;return true;}
        for(std::size_t i=0;i<addonPodCount();++i)if(const auto* p=addonPodAt(i);p&&p->key==podKey){
            out=seededCareer(p->baseAbilities);return validCareer(out);}
        return false;
    }catch(...){return false;}
}
bool DlcProgress50DL::commitCareer(const std::string& podKey,const ProgressCareer& career) noexcept {
    try{
        if(!podUnlocked(podKey)||!validCareer(career))return false;
        const auto it=state_.careers.find(podKey);
        if(it==state_.careers.end()&&state_.careers.size()>=ProgressLimits::careers)return false;
        state_.careers[podKey]=career;dirty_=true;return true;
    }catch(...){return false;}
}

// ---- Process hook-up (146p round 3). PORT EXTENSION, NOT ORIGINAL.
DlcProgress50DL& progressOwner() noexcept {static DlcProgress50DL owner;return owner;}
ProgressCatalog progressCatalogFromRegistry(){
    ProgressCatalog catalog;
    catalog.admittedPackages=admittedPackageCount();
    for(std::size_t i=0;i<addonCourseCount();++i){
        const auto* c=addonCourseAt(i);if(!c)continue;
        ProgressCourseItem item;item.key=c->key;item.recordsKey=c->recordsKey;item.archiveSha256=c->archiveSha256;
        if(c->hallDefaults){
            // hall_defaults -> original rows (start from empty slots). Names were
            // checked by the registry (1..20 characters); copy is bounded to 20.
            auto rows=emptyHallRows();
            const auto put=[](std::array<char,21>& lane,const std::string& name){
                lane.fill(0);for(std::size_t k=0;k<name.size()&&k<ProgressLimits::nameChars;++k)lane[k]=name[k];};
            for(std::size_t k=0;k<ProgressLimits::entries;++k){
                rows.lap102F0[k]=static_cast<float>(c->lap[k].time);put(rows.lapNames102FC[k],c->lap[k].name);
                rows.race1033C[k]=static_cast<float>(c->race[k].time);put(rows.raceNames10348[k],c->race[k].name);
                rows.knockouts10388[k]=c->knockouts[k].count;put(rows.knockoutNames10394[k],c->knockouts[k].name);
            }
            item.hallDefaults=rows;
        }
        catalog.courses.push_back(std::move(item));
    }
    for(std::size_t i=0;i<addonPodCount();++i)if(const auto* p=addonPodAt(i))catalog.pods.push_back({p->key});
    return catalog;
}
void loadProgressFromRegistry(const fs::path& saveDirectory) noexcept {
    try{
        if(!registryFrozen()||admittedPackageCount()==0)return; // stock: nothing read, created or logged
        (void)progressOwner().load(saveDirectory,progressCatalogFromRegistry());
    }catch(...){line("dlc_progress status=unavailable reason=\"catalog build failed\"");}
}

} // namespace rr::assets::dlc50DL
