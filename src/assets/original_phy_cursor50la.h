#pragma once
#include "pod_physics49n.h"
#include <array>
#include <cstdint>
#include <new>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rr::assets {
class NativePhyLoad50LA;
// Native bounded Config reader, retaining the real selected-section rule:
// missing157D00 returns false without changing current20/24. No atomic whole
// document parse, required-key table, aliases for missing keys, or defaults.
// Source-string grammar is deliberately narrower, as in sourceFloat158A70.
class OriginalPhyCursor50LA final {
    friend class NativePhyLoad50LA;
    // Only NativePhyLoad50LA can supply this owned, actually consumed cache.
    // Values remain views into bytes_, never into replaceable selection/keys.
    enum class NativeFailure { none, inputRejected, hostAllocation, hostLength, hostException };
    struct NativeToken {
        std::string key;
        std::optional<std::string_view> value;
    };
    struct NativeCache {
        std::string selectedName;
        std::string_view selected;
        std::vector<NativeToken> tokens;
        NativeFailure failure=NativeFailure::none;
    };
    const std::vector<std::uint8_t>& bytes_;
    std::string_view& section_;
    NativeCache* native_=nullptr;
    OriginalPhyCursor50LA(const std::vector<std::uint8_t>& bytes,NativeCache& cache)noexcept
        :bytes_(bytes),section_(cache.selected),native_(&cache){}
    void beginRead()const noexcept {if(native_)native_->failure=NativeFailure::none;}
    bool rejectInput()const noexcept {
        if(native_)native_->failure=NativeFailure::inputRejected;
        return false;
    }
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
    bool find(std::string_view selected,std::string_view key,bool sectionOnly,
        bool& sectionFound,std::optional<std::string_view>& token)const noexcept {
        sectionFound=false;token.reset();
        if(bytes_.empty()||bytes_.size()>maxPodPhysicsBytes49N)return false;
        const std::string_view text(reinterpret_cast<const char*>(bytes_.data()),bytes_.size());
        bool active=false;
        for(std::size_t at=0;at<text.size();){
            const auto start=at;auto end=text.find('\n',at);if(end==std::string_view::npos)end=text.size();at=end+1;
            if(end-start>maxPodPhysicsLine49N)return false;
            auto line=trim(text.substr(start,end-start));
            for(char c:line)if((static_cast<unsigned char>(c)<32&&c!='\t')||static_cast<unsigned char>(c)>126)return false;
            if(line.empty()||line.front()==';')continue;
            if(line.front()=='['){
                if(line.size()<3||line.back()!=']')return false;
                if(active)return true; //First matching block only.
                active=equal(trim(line.substr(1,line.size()-2)),selected);
                if(active){sectionFound=true;if(sectionOnly)return true;}
                continue;
            }
            if(!active)continue;
            const auto split=line.find('=');
            if(split==std::string_view::npos||!equal(trim(line.substr(0,split)),key))continue;
            token=trim(line.substr(split+1));return true;
        }
        return true;
    }
    bool token(std::string_view key,std::optional<std::string_view>& out)const noexcept {
        if(!native_){bool found=false;return find(section_,key,false,found,out);}
        beginRead();
        for(const auto& row:native_->tokens)if(equal(row.key,key)){out=row.value;return true;}
        bool found=false;std::optional<std::string_view> value;
        if(!find(section_,key,false,found,value))return rejectInput();
        // Preserve completed rows and caller output on a native host failure.
        // Missing keys are cached too: immutable backing makes this equivalent
        // to the same first-block/first-key bounded scan, not an eager parse.
        try {
            NativeToken row{std::string(key),value};
            native_->tokens.push_back(std::move(row));
        }catch(const std::bad_alloc&){native_->failure=NativeFailure::hostAllocation;return false;}
        catch(const std::length_error&){native_->failure=NativeFailure::hostLength;return false;}
        catch(...){native_->failure=NativeFailure::hostException;return false;}
        out=value;return true;
    }
public:
    OriginalPhyCursor50LA(const std::vector<std::uint8_t>& bytes,std::string_view& selected)noexcept
        :bytes_(bytes),section_(selected){}
    bool select157D00(std::string_view name,bool& present)noexcept {
        beginRead();
        std::optional<std::string_view> unused;
        if(!find(name,{},true,present,unused))return rejectInput();
        if(!present)return true; // Missing selection preserves all old state.
        if(!native_){section_=name;return true;}
        // Admission ends at the matching header exactly as in the old cursor.
        // Copy first because name may alias our previous selectedName. Commit
        // only after allocation succeeds; this is native, not guest heap policy.
        try {
            std::string next(name);
            native_->selectedName.swap(next);
            section_=std::string_view(native_->selectedName);
            native_->tokens.clear();
        }catch(const std::bad_alloc&){native_->failure=NativeFailure::hostAllocation;return false;}
        catch(const std::length_error&){native_->failure=NativeFailure::hostLength;return false;}
        catch(...){native_->failure=NativeFailure::hostException;return false;}
        return true;
    }
    bool float158A70(std::string_view key,std::optional<float>& out)const noexcept {
        std::optional<std::string_view> value;if(!token(key,value))return false;
        if(!value)return true;
        float number;
        if(!parsePodPhysicsNumber49N(value->data(),value->size(),number))return rejectInput();
        out=number;return true;
    }
    // A view of the same immutable PHY backing, safe to retain as Config's
    // selected section after a Wreckage/PartN string lookup. No empty or
    // quoted-string conversion is fabricated by this bounded lexical route.
    bool string158240(std::string_view key,std::optional<std::string_view>& out)const noexcept {
        std::optional<std::string_view> value;if(!token(key,value))return false;
        if(!value)return true;
        if(value->empty()||value->size()>=260u||value->front()=='"'||value->back()=='"')return rejectInput();
        out=*value;return true;
    }
    bool defaultFloat158B80(std::string_view key,float fallback,float& destination)const noexcept {
        std::optional<float> value;if(!float158A70(key,value))return false;
        destination=value?*value:fallback;return true;
    }
    bool integer158920(std::string_view key,std::optional<std::int32_t>& out,bool& written)const noexcept {
        written=false;std::optional<std::string_view> value;if(!token(key,value))return false;
        if(!value)return true;auto s=*value;if(s.empty())return rejectInput();
        const auto first=lower(s.front());
        if(first=='t'||first=='y'||equal(s,"ON")){out=1;written=true;return true;}
        if(first=='f'||first=='n'||equal(s,"OFF")){out=0;written=true;return true;}
        bool negative=false;if(s.front()=='+'||s.front()=='-'){negative=s.front()=='-';s.remove_prefix(1);}
        if(s.empty())return rejectInput();
        unsigned base=10;
        if(s.size()>1&&s.front()=='0'){
            if(lower(s[1])=='x'){base=16;s.remove_prefix(2);if(s.empty())return rejectInput();}
            else base=8;
        }
        // Bounded base-zero strtol subset: reject overflow and trailing text,
        // rather than inventing unsupported guest libc behavior.
        std::uint64_t magnitude=0;const std::uint64_t limit=negative?2147483648ull:2147483647ull;
        for(char c:s){const auto l=lower(c);const unsigned digit=c>='0'&&c<='9'?unsigned(c-'0'):
                l>='a'&&l<='f'?unsigned(l-'a'+10):16u;
            if(digit>=base||magnitude>(limit-digit)/base)return rejectInput();
            magnitude=magnitude*base+digit;
        }
        const auto signedValue=negative?-static_cast<std::int64_t>(magnitude):static_cast<std::int64_t>(magnitude);
        out=static_cast<std::int32_t>(signedValue);written=true;return true;
    }
    bool vector158C00(std::string_view key,std::array<float,4>& destination)const noexcept {
        const std::array<float,4> zero{{0,0,0,1}};
        return vector158C00(key,zero,destination);
    }
    bool vector158C00(std::string_view key,const std::array<float,4>& fallback,
        std::array<float,4>& destination)const noexcept {
        std::optional<std::string_view> value;if(!token(key,value))return false;
        if(!value){destination[0]=fallback[0];destination[1]=fallback[1];destination[2]=fallback[2];return true;}
        // Actual source writes X, then Y, then Z, never W. Retain earlier
        // components if the next bounded numeric conversion is rejected.
        auto remaining=*value;
        for(unsigned axis=0;axis<3;++axis){
            const auto comma=remaining.find(',');
            if(axis==2&&comma!=std::string_view::npos)return rejectInput();
            const auto part=trim(axis<2?remaining.substr(0,comma):remaining);float number;
            if(!parsePodPhysicsNumber49N(part.data(),part.size(),number))return rejectInput();
            destination[axis]=number;
            if(axis<2){if(comma==std::string_view::npos)return rejectInput();remaining.remove_prefix(comma+1);}
        }
        return true;
    }
};
}
