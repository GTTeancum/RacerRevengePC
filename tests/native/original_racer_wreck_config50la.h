#pragma once
#include "../../src/assets/pod_physics49n.h"
#include <cerrno>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
namespace ra_cold_roster50LA {class WreckBacking;}
namespace rr::game {
//Explicit native retained-PHY text projection of the source stack Config.
//No guest FileManager handle/cache/stream or assertion side effects claimed.
//The selected first block is live reader state; authoring counts are not input.
class NativeRacerWreckConfigReader50LA final {
    friend class ::ra_cold_roster50LA::WreckBacking;
    const std::vector<std::uint8_t>* phy_=nullptr;
    std::optional<std::size_t> blockBegin_,blockEnd_;
    bool withdrawn_=false;
    static std::string_view trim(std::string_view s)noexcept {
        while(!s.empty()&&(s.front()==' '||s.front()=='\t'||s.front()=='\r'))s.remove_prefix(1);
        while(!s.empty()&&(s.back()==' '||s.back()=='\t'||s.back()=='\r'))s.remove_suffix(1);
        return s;
    }
    static char lower(char c)noexcept{return c>='A'&&c<='Z'?char(c+32):c;}
    static bool equal(std::string_view a,std::string_view b)noexcept {
        if(a.size()!=b.size())return false;
        for(std::size_t i=0;i<a.size();++i)if(lower(a[i])!=lower(b[i]))return false;
        return true;
    }
    bool bindNative(const std::vector<std::uint8_t>& actualPhy)noexcept {
        if(phy_||withdrawn_||actualPhy.empty()||actualPhy.size()>rr::assets::maxPodPhysicsBytes49N)return false;
        phy_=&actualPhy;return true;
    }
    bool select157D00(std::string_view section)noexcept {
        if(!phy_||withdrawn_)return false;
        const std::string_view text(reinterpret_cast<const char*>(phy_->data()),phy_->size());
        std::optional<std::size_t> begin,end;
        for(std::size_t at=0;at<text.size();){
            const auto start=at;auto stop=text.find('\n',at);if(stop==std::string_view::npos)stop=text.size();at=stop<text.size()?stop+1:stop;
            if(stop-start>rr::assets::maxPodPhysicsLine49N)return false;
            const auto line=trim(text.substr(start,stop-start));
            for(char c:line)if((static_cast<unsigned char>(c)<32&&c!='\t')||static_cast<unsigned char>(c)>126)return false;
            if(line.empty()||line.front()==';')continue;
            if(line.front()!='[')continue;
            if(line.size()<3||line.back()!=']')return false;
            if(begin){end=start;break;}
            if(equal(trim(line.substr(1,line.size()-2)),section))begin=at;
        }
        //Missing selected block is outside the admitted retained inventory;
        //never substitute an unrelated previous block or source assertion.
        if(!begin)return false;
        blockBegin_=*begin;blockEnd_=end?*end:text.size();return true;
    }
    bool read158240(std::string_view key,std::optional<std::string>& destination,std::size_t capacity)const {
        if(!phy_||withdrawn_||!blockBegin_||!blockEnd_||*blockBegin_>*blockEnd_||*blockEnd_>phy_->size())return false;
        const std::string_view text(reinterpret_cast<const char*>(phy_->data()),phy_->size());
        for(std::size_t at=*blockBegin_;at<*blockEnd_;){
            const auto start=at;auto stop=text.find('\n',at);if(stop==std::string_view::npos||stop>*blockEnd_)stop=*blockEnd_;at=stop<text.size()?stop+1:stop;
            const auto line=trim(text.substr(start,stop-start));
            if(line.empty()||line.front()==';')continue;
            const auto split=line.find('=');if(split==std::string_view::npos||!equal(trim(line.substr(0,split)),key))continue;
            auto token=trim(line.substr(split+1));
            //Only already-authenticated unquoted PHY values. Source capacity
            //includes NUL; copied bytes truncate at the original bound.
            if(token.find('"')!=std::string_view::npos)return false;
            if(capacity!=static_cast<std::size_t>(-1)){
                if(!capacity)return false;if(token.size()>=capacity)token=token.substr(0,capacity-1u);
            }
            destination=std::string(token);return true;
        }
        return true; //Missing key skips destination store, preserving unknown/history.
    }
    bool readInteger158920(std::string_view key,std::optional<std::uint32_t>& actualWord)const {
        std::optional<std::string> token;
        if(!read158240(key,token,static_cast<std::size_t>(-1)))return false;
        if(!token)return true;
        const auto first=token->empty()?'\0':lower((*token)[0]);
        if(first=='t'||first=='y'||equal(*token,"ON")){actualWord=1u;return true;}
        if(first=='f'||first=='n'||equal(*token,"OFF")){actualWord=0u;return true;}
        //158A34 base0 ignores its end pointer. Bounded signed32 host policy,
        //same as existing source Config reader: unsupported overflow refuses
        //before source158A44; empty/nonnumeric present tokens write zero.
        errno=0;char* end=nullptr;const auto value=std::strtoll(token->c_str(),&end,0);(void)end;
        if(errno==ERANGE||value<std::numeric_limits<std::int32_t>::min()||value>std::numeric_limits<std::int32_t>::max())return false;
        actualWord=static_cast<std::uint32_t>(static_cast<std::int32_t>(value));return true;
    }
    bool readInteger158B30(std::string_view key,std::uint32_t originalDefault,
        std::optional<std::uint32_t>& actualWord)const {
        std::optional<std::uint32_t> read;
        if(!readInteger158920(key,read))return false;
        //158920 writes only a present value;158B60 writes the default only
        //when missing. No authored count/default is used as live authority.
        actualWord=read?*read:originalDefault;return true;
    }
    bool readInteger158B30(std::string_view key,std::uint32_t originalDefault,
        std::uint32_t& actualWord)const {
        std::optional<std::uint32_t> read;
        if(!readInteger158920(key,read))return false;
        actualWord=read?*read:originalDefault;return true;
    }
    void withdrawNative()noexcept {withdrawn_=true;phy_=nullptr;} //Selected offsets remain history.
};
}
