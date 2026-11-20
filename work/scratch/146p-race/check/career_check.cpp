// Host check (no game run) of the DLC progress careers, schema rrpc-dlc-progress/2.
// Usage: RR_ORIGINAL_ASSET_ROOT=<stock> RR_DLC_ROOT=<root> career_check.exe <scratch dir>
#include "../../../../src/assets/dlc_files50dl.h"
#include "../../../../src/assets/dlc_registry50dl.h"
#include "../../../../src/assets/dlc_progress50dl.h"
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace d=rr::assets::dlc50DL;
static int failures=0,passes=0;
static void check(bool ok,const char* what){std::printf("%s %s\n",ok?"PASS":"FAIL",what);ok?++passes:++failures;}
static bool sameCareer(const d::ProgressCareer& a,const d::ProgressCareer& b){
    return a.earned2190==b.earned2190&&a.unspent2194==b.unspent2194&&a.kills2198==b.kills2198&&
        a.progress219C==b.progress219C&&a.pod.abilities1920==b.pod.abilities1920&&
        a.pod.medals1938==b.pod.medals1938&&a.pod.remainingPrizes1946==b.pod.remainingPrizes1946;
}
static std::string careerJson(const char* key,const char* body){
    return std::string("{\"schema\":\"rrpc-dlc-progress/2\",\"courses\":{},\"stock_course_addon_pod_records\":{},\"pods\":{},\"careers\":{\"")+key+"\":"+body+"}}";
}
static const char* good=R"({"earned":12000,"unspent":3400,"kills":7,"progress":4,"pod":{"abilities":[1.5,2,0,3.25,4,5],"medals":[1,2,3,8,0,0,0,0,0,0,0,0,0],"remaining_prizes":[2200,1200,-5,0,0,0,0,0,0,0,0,0,6000]}})";
static bool parses(const std::string& text,d::DlcProgress50DL::State& out,std::string& reason){
    bool newer=false;return d::DlcProgress50DL::parse(text,out,newer,reason);
}
static void refused(const char* body,const char* what){
    d::DlcProgress50DL::State s;std::string reason;
    const bool ok=parses(careerJson("arcade-ark-roose/a2r-ark",body),s,reason);
    check(!ok&&s.careers.empty(),what);if(!ok)std::printf("     reason: %s\n",reason.c_str());
}
int main(int argc,char** argv){
    if(argc!=2){std::printf("usage: career_check <scratch dir>\n");return 2;}
    // 1. Round trip: serialise -> parse gives the same career (exact floats).
    {d::DlcProgress50DL::State s;std::string reason;
     check(parses(careerJson("arcade-ark-roose/a2r-ark",good),s,reason)&&s.careers.size()==1,"parse /2 career");
     std::string text;check(d::DlcProgress50DL::serialise(s,text,reason),"serialise /2 career");
     check(text.find("\"schema\": \"rrpc-dlc-progress/2\"")!=std::string::npos,"written schema is /2");
     d::DlcProgress50DL::State back;check(parses(text,back,reason)&&back.careers.size()==1&&
        sameCareer(back.careers.begin()->second,s.careers.begin()->second),"round trip keeps every career value");
     d::ProgressCareer odd=s.careers.begin()->second;odd.pod.abilities1920[0]=0.1f;odd.pod.abilities1920[1]=1234.5678f;
     s.careers.begin()->second=odd;text.clear();
     check(d::DlcProgress50DL::serialise(s,text,reason)&&parses(text,back,reason)&&
        sameCareer(back.careers.begin()->second,odd),"round trip keeps float bits (0.1, 1234.5678)");}
    // 2. Bounds (each refused, nothing kept).
    refused(R"({"earned":1,"unspent":0,"kills":0,"progress":14,"pod":{"abilities":[0,0,0,0,0,0],"medals":[0,0,0,0,0,0,0,0,0,0,0,0,0],"remaining_prizes":[0,0,0,0,0,0,0,0,0,0,0,0,0]}})","progress 14 refused");
    refused(R"({"earned":1,"unspent":0,"kills":0,"progress":0,"pod":{"abilities":[0,0,0,0,0,0],"medals":[9,0,0,0,0,0,0,0,0,0,0,0,0],"remaining_prizes":[0,0,0,0,0,0,0,0,0,0,0,0,0]}})","medal 9 refused");
    refused(R"({"earned":1,"unspent":0,"kills":0,"progress":0,"pod":{"abilities":[-1,0,0,0,0,0],"medals":[0,0,0,0,0,0,0,0,0,0,0,0,0],"remaining_prizes":[0,0,0,0,0,0,0,0,0,0,0,0,0]}})","negative ability refused");
    refused(R"({"earned":1,"unspent":0,"kills":0,"progress":0,"pod":{"abilities":[0,0,0,0,0,0],"medals":[0,0,0,0,0,0,0,0,0,0,0,0,0],"remaining_prizes":[40000,0,0,0,0,0,0,0,0,0,0,0,0]}})","prize outside 16 bits refused");
    refused(R"({"earned":4294967296,"unspent":0,"kills":0,"progress":0,"pod":{"abilities":[0,0,0,0,0,0],"medals":[0,0,0,0,0,0,0,0,0,0,0,0,0],"remaining_prizes":[0,0,0,0,0,0,0,0,0,0,0,0,0]}})","earned above u32 refused");
    refused(R"({"earned":-1,"unspent":0,"kills":0,"progress":0,"pod":{"abilities":[0,0,0,0,0,0],"medals":[0,0,0,0,0,0,0,0,0,0,0,0,0],"remaining_prizes":[0,0,0,0,0,0,0,0,0,0,0,0,0]}})","negative earned refused");
    refused(R"({"earned":1.5,"unspent":0,"kills":0,"progress":0,"pod":{"abilities":[0,0,0,0,0,0],"medals":[0,0,0,0,0,0,0,0,0,0,0,0,0],"remaining_prizes":[0,0,0,0,0,0,0,0,0,0,0,0,0]}})","fractional earned refused");
    refused(R"({"earned":1,"unspent":0,"kills":0,"progress":0,"pod":{"abilities":[0,0,0,0,0],"medals":[0,0,0,0,0,0,0,0,0,0,0,0,0],"remaining_prizes":[0,0,0,0,0,0,0,0,0,0,0,0,0]}})","5 abilities refused");
    refused(R"({"earned":1,"unspent":0,"kills":0,"progress":0,"pod":{"abilities":[0,0,0,0,0,0],"medals":[0,0,0,0,0,0,0,0,0,0,0,0],"remaining_prizes":[0,0,0,0,0,0,0,0,0,0,0,0,0]}})","12 medals refused");
    refused(R"({"earned":1,"unspent":0,"kills":0,"progress":0,"extra":1,"pod":{"abilities":[0,0,0,0,0,0],"medals":[0,0,0,0,0,0,0,0,0,0,0,0,0],"remaining_prizes":[0,0,0,0,0,0,0,0,0,0,0,0,0]}})","unknown career key refused");
    refused(R"({"unspent":0,"kills":0,"progress":0,"pod":{"abilities":[0,0,0,0,0,0],"medals":[0,0,0,0,0,0,0,0,0,0,0,0,0],"remaining_prizes":[0,0,0,0,0,0,0,0,0,0,0,0,0]}})","missing earned refused");
    {d::DlcProgress50DL::State s;std::string reason;
     check(!parses(careerJson("Not-A-Key",good),s,reason),"career key that is not an item key refused");}
    // 3. All or nothing: one good and one bad career -> nothing kept.
    {const std::string text=std::string("{\"schema\":\"rrpc-dlc-progress/2\",\"careers\":{\"arcade-ark-roose/a2r-ark\":")+good+
        ",\"arcade-elan-mak/a2r-elan\":{\"earned\":1}}}";
     d::DlcProgress50DL::State s;s.pods["keep/me"]=true;std::string reason;
     check(!parses(text,s,reason)&&s.pods.size()==1&&s.careers.empty(),"one bad career refuses the whole file (out untouched)");}
    // 4. /1 still loads (careers empty); a /1 file with careers is refused; /3 is newer.
    {d::DlcProgress50DL::State s;std::string reason;bool newer=false;
     check(d::DlcProgress50DL::parse(R"({"schema":"rrpc-dlc-progress/1","courses":{},"stock_course_addon_pod_records":{"TA":{"lap":[{"time":1.0,"name":"EYECHECK","racer":"arcade-ark-roose/a2r-ark"},null,null],"race":[null,null,null],"knockouts":[null,null,null]}},"pods":{"arcade-ark-roose/a2r-ark":{"unlocked":true}},"careers":{}})",s,newer,reason)&&
        s.stockCourseAddonPod.size()==1&&s.careers.empty(),"/1 file loads");
     check(!d::DlcProgress50DL::parse([]{auto t=careerJson("arcade-ark-roose/a2r-ark",good);t.replace(t.find("progress/2"),10,"progress/1");return t;}(),s,newer,reason),"/1 file with careers refused");
     check(!d::DlcProgress50DL::parse(R"({"schema":"rrpc-dlc-progress/3"})",s,newer,reason)&&newer,"/3 reported as newer schema");}
    // 5. Seeding and the stock prize table.
    {const float base[6]{1.f,2.f,3.f,4.f,5.f,6.f};const auto c=d::seededCareer(base);
     bool same=true;for(int i=0;i<6;++i)same=same&&c.pod.abilities1920[i]==base[i];
     check(same,"seed abilities = PHY base abilities");
     check(c.pod.remainingPrizes1946==rr::game::originalTournamentPrizes50KG&&c.pod.remainingPrizes1946[0]==2200&&
        c.pod.remainingPrizes1946[12]==6000,"seed remaining prizes = stock prize table (2200..6000)");
     check(c.earned2190==0&&c.unspent2194==0&&c.kills2198==0&&c.progress219C==0&&
        c.pod.medals1938==std::array<std::uint8_t,13>{},"seed scalars and medals zero");}
    // 6. Owner API over the real registry (startup pass over RR_DLC_ROOT).
    d::startup();
    const std::filesystem::path dir=argv[1];std::filesystem::create_directories(dir);
    std::filesystem::remove(dir/d::progressFileName);
    d::loadProgressFromRegistry(dir);
    auto& owner=d::progressOwner();
    const auto* ark=d::addonPodCount()?d::addonPodAt(0):nullptr;
    check(ark!=nullptr,"registry has an add-on pod");
    if(ark){
        d::ProgressCareer snap;
        check(owner.careerSnapshot(ark->key,snap),"careerSnapshot of an admitted pod");
        bool seeded=true;for(int i=0;i<6;++i)seeded=seeded&&snap.pod.abilities1920[i]==ark->baseAbilities[i];
        check(seeded&&snap.pod.remainingPrizes1946==rr::game::originalTournamentPrizes50KG,"fresh snapshot seeded from AddonPod::baseAbilities and the stock prizes");
        std::printf("     base=%g,%g,%g,%g,%g,%g\n",ark->baseAbilities[0],ark->baseAbilities[1],ark->baseAbilities[2],
            ark->baseAbilities[3],ark->baseAbilities[4],ark->baseAbilities[5]);
        check(!owner.careerSnapshot("no-such/pod",snap),"careerSnapshot of an unknown pod refused");
        auto bad=snap;bad.pod.medals1938[3]=9;
        check(!owner.commitCareer(ark->key,bad)&&!owner.changed(),"commitCareer refuses a bad value (nothing changes)");
        check(!owner.commitCareer("no-such/pod",snap)&&!owner.changed(),"commitCareer refuses an unknown pod");
        auto next=snap;next.earned2190=2200;next.unspent2194=2200;next.pod.medals1938[0]=1;next.progress219C=1;
        next.pod.remainingPrizes1946[0]=0;
        check(owner.commitCareer(ark->key,next)&&owner.changed(),"commitCareer stores a good career and marks changed");
        d::ProgressCareer after;check(owner.careerSnapshot(ark->key,after)&&sameCareer(after,next),"snapshot returns the committed career");
        check(owner.saveIfChanged()==d::DlcProgress50DL::SaveStatus::saved,"saveIfChanged writes the side file");
        std::ifstream in(dir/d::progressFileName,std::ios::binary);std::stringstream ss;ss<<in.rdbuf();
        d::DlcProgress50DL::State back;std::string reason;
        check(parses(ss.str(),back,reason)&&back.careers.count(ark->key)&&sameCareer(back.careers[ark->key],next),
            "written file reads back the same career");
    }
    std::printf("career_check passes=%d failures=%d\n",passes,failures);
    return failures?1:0;
}
