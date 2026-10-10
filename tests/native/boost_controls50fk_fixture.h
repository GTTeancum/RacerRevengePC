#pragma once
// Cold-only extension of the existing actual PHY binding, with no guest
// instruction execution. The native49N parser validates the complete text;
// this scan adds the seven cooling keys it deliberately leaves unconsumed.
#include "../../src/game/boost_controls50fk.h"
#include "../../src/assets/impact_authoring49y.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <string_view>

namespace boost50fk_fixture {
namespace detail {
inline std::string_view trim(std::string_view s){
    const auto first=s.find_first_not_of(" \t\r");
    if(first==s.npos)return {};
    const auto last=s.find_last_not_of(" \t\r");return s.substr(first,last-first+1);
}
inline bool equal(std::string_view a,std::string_view b){
    if(a.size()!=b.size())return false;
    for(std::size_t k=0;k<a.size();++k){
        const auto lower=[](char c){return c>='A'&&c<='Z'?char(c+'a'-'A'):c;};
        if(lower(a[k])!=lower(b[k]))return false;
    }return true;
}
inline float fp(std::uint32_t bits){float value;std::memcpy(&value,&bits,4);return value;}
}
inline bool bind(const char* text,std::size_t bytes,
    const rr::game::AuthoredSelectionContext49N& selection,
    const rr::game::AuthoredBinding49N& authored,
    rr::game::BoostParameters50FK& output,float& coolingCF8){
    using namespace rr::game;
    if(selection.applyProfileOverwrite||std::fegetround()!=FE_TONEAREST)return false;
    AuthoredParameters49N raw{};rr::assets::PodPhysicsDiagnostic49N diagnostic{};
    if(!rr::assets::readPodPhysics49N(text,bytes,raw,diagnostic))return false;
    const char* sections[]={"DamageRepair","DamageRepair","DamageRepair",
        "DamageRepair","DamageRepair","BaseAbilities","MaxAbilities"};
    const char* keys[]={"BaseCoolingTimeA","BaseCoolingTimeB","BaseCoolingTimeC",
        "MinBaseCoolingTime","MaxBaseCoolingTime","BaseCoolingTime","BaseCoolingTime"};
    float values[7]{};bool present[7]{};std::string_view section;
    std::string_view remaining(text,bytes);
    while(!remaining.empty()){
        const auto end=remaining.find('\n');
        const auto line=detail::trim(remaining.substr(0,end));
        remaining=end==remaining.npos?std::string_view{}:remaining.substr(end+1);
        if(line.empty()||line.front()==';')continue;
        if(line.front()=='['){section=line.substr(1,line.size()-2);continue;}
        const auto split=line.find('=');
        const auto key=detail::trim(line.substr(0,split));
        const auto token=detail::trim(line.substr(split+1));
        for(unsigned k=0;k<7;++k)if(detail::equal(section,sections[k])&&detail::equal(key,keys[k])){
            if(present[k]||!rr::assets::parseImpactNumber49Y(token.data(),token.size(),values[k]))return false;
            present[k]=true;
        }
    }
    float selected=0.f;
    if(authored.usedAuthoredClassValues){
        const auto klass=static_cast<unsigned>(authored.selectedClass);
        if(klass<1u||klass>3u||!present[klass-1u])return false;
        selected=values[klass-1u];
    }else{
        for(unsigned k=3;k<7;++k)if(!present[k])return false;
        const float upgrade=raw.tuning.upgradePercent.present?raw.tuning.upgradePercent.value:0.f;
        volatile float difference=values[6]-values[5];
        volatile float delta=upgrade*difference;
        volatile float ability=values[5]+delta;
        volatile float range=values[4]-values[3];
        volatile float scaled=ability*range;
        selected=values[3]+scaled;
    }
    volatile float speedCooling=detail::fp(0x37a7c5acu)*selected;
    if(!std::isfinite(selected)||!std::isfinite(speedCooling)||
       !std::isfinite(authored.engineDifference5C))return false;
    output={selected,speedCooling,detail::fp(0x3e2e147bu),authored.engineDifference5C};
    coolingCF8=selected;return true;
}
}
