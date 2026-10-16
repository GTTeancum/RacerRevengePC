#pragma once
// Bounded native MotionLibrary4333C0 / MOT resource lifetime. Source-only.
// No guest object layout, interpreted track objects, resource decoder or VU.
#include "../assets/model_geometry49o.h"
#include <algorithm>
#include <cerrno>
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rr::game {
using OriginalMotionSection50LA=std::map<std::string,std::string>;
using OriginalMotionConfiguration50LA=std::map<std::string,OriginalMotionSection50LA>;
enum class OriginalMotionLibraryStatus50LA : std::uint8_t {
    complete,input,configuration,capacity,admittedMotion,unsupportedVueFallback,
    allocation,foreignResource,reference,alreadyLoaded,releaseState
};
struct OriginalMotionLibraryResult50LA {
    OriginalMotionLibraryStatus50LA status=OriginalMotionLibraryStatus50LA::input;
    std::uint32_t sourcePC=0,motionOrdinal=0;
    // Source153610 returns0 for count0 and1 after its positive-count loop.
    // This is distinct from native admission success.
    std::uint32_t sourceReturn=0;
    bool okay()const noexcept{return status==OriginalMotionLibraryStatus50LA::complete;}
};
struct OriginalMotionDefaults50LA {
    std::string directory190;
    std::int32_t interpolatePosition20=1,interpolateRotation24=1;
    std::uint32_t rotationType28=0,positionType2C=1;
};
struct OriginalAdmittedMotion50LA {
    // The resolver must return the actual authenticated member that the exact
    // requested path resolves to, and its already admitted, unchanged49O view.
    // Missing imported files are not evidence that the original MOT is absent.
    const assets::MotionView49O* motion=nullptr;
    std::string originalMemberName;
};
struct OriginalMotionResourceFields50LA {
    std::string name00,vue51;
    std::uint32_t positionCountAC=0,rotationCountB0=0;
    std::int32_t loopingB4=0,interpolatePositionB8=0,interpolateRotationBC=0;
    std::uint32_t rotationTypeC0=0,positionTypeC4=1;
    std::int32_t fpsC8=0;
    std::uint32_t frameCountCC=0;
    float durationD0=0.f,transitionTimeD4=0.f;
    std::int32_t referenceD8=0;
};
class OriginalMotionLibrary50LA;
class OriginalModelMotionSet50LA;
class OriginalMotionResource50LA final {
public:
    ~OriginalMotionResource50LA()=default;
    const OriginalMotionResourceFields50LA& fields()const noexcept{return fields_;}
    const assets::MotionView49O& motion()const noexcept{return motion_;}
    const std::string& originalMemberName()const noexcept{return originalMemberName_;}
    const std::string& requestedMemberName()const noexcept{return requestedMemberName_;}
    OriginalMotionResource50LA(const OriginalMotionResource50LA&)=delete;
    OriginalMotionResource50LA& operator=(const OriginalMotionResource50LA&)=delete;
private:
    OriginalMotionResource50LA()=default;
    OriginalMotionResourceFields50LA fields_;
    // This resource genuinely owns its CPU-readable immutable track payload.
    // Rebinding the admitted view is not reparsing or fabricating tracks. A
    // library cache hit remains valid after the originating Rig is destroyed.
    std::vector<std::uint8_t> bytes_;
    assets::MotionView49O motion_{};
    std::string originalMemberName_,requestedMemberName_;
    // Native lifetime guard, not a source field or another D8 reference.
    // Counts references represented by ALL currently published model slots.
    // It lets a release preflight detect cross-model aggregate undercounts.
    std::uint32_t publishedSlotReferences_=0;
    friend class OriginalMotionLibrary50LA;
    friend class OriginalModelMotionSet50LA;
};

namespace original_motion_detail50LA {
using S=OriginalMotionLibraryStatus50LA;
inline OriginalMotionLibraryResult50LA result(S s,std::uint32_t pc)noexcept{return {s,pc,0,0};}
inline char lower(char c)noexcept{return c>='A'&&c<='Z'?char(c+32):c;}
//308F00 is strcasecmp, NOT strcmp: its ctype uppercase bit selects+0x20.
inline bool equal308F00(std::string_view a,std::string_view b)noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i)if(lower(a[i])!=lower(b[i]))return false;
    return true;
}
inline bool ascii(std::string_view s,std::size_t max)noexcept {
    if(s.size()>max)return false;
    for(unsigned char c:s)if(c<32||c>126)return false;
    return true;
}
inline const std::string* value(const OriginalMotionSection50LA& section,const char* key)noexcept {
    // Do not construct a temporary std::string inside this noexcept lookup.
    for(const auto& entry:section)if(entry.first==key)return &entry.second;
    return nullptr;
}
inline bool numericEnd(const char* p)noexcept {
    while(*p==' '||*p=='\t'||*p=='\r'||*p=='\n')++p;
    return !*p||*p==';';
}
//158B30->158920 accepts T/F/Y/N first characters, exact ON/OFF, then
//base-0 signed conversion. Native admission rejects overflow/other suffixes
//rather than pretending the source's unchecked conversion was safe.
inline bool integer(const OriginalMotionSection50LA& section,const char* key,
    std::int32_t fallback,std::int32_t& out)noexcept {
    const auto* s=value(section,key);if(!s){out=fallback;return true;}
    if(s->empty()||!ascii(*s,1023))return false;
    const auto first=lower(s->front());
    if(first=='t'||first=='y'||equal308F00(*s,"ON")){out=1;return true;}
    if(first=='f'||first=='n'||equal308F00(*s,"OFF")){out=0;return true;}
    errno=0;char* end=nullptr;const auto n=std::strtoll(s->c_str(),&end,0);
    if(errno==ERANGE||end==s->c_str()||!numericEnd(end)||
       n<std::numeric_limits<std::int32_t>::min()||n>std::numeric_limits<std::int32_t>::max())return false;
    out=static_cast<std::int32_t>(n);return true;
}
inline bool number(const std::string& text,float& out)noexcept {
    if(text.empty()||!ascii(text,1023))return false;
    errno=0;char* end=nullptr;const auto n=std::strtod(text.c_str(),&end);
    if(errno==ERANGE||end==text.c_str()||!numericEnd(end)||!std::isfinite(n))return false;
    const auto value=static_cast<float>(n);if(!std::isfinite(value))return false;
    out=value;return true;
}
}

class OriginalMotionLibrary50LA final {
public:
    // Native capacity, not an alleged original constant.
    static constexpr std::size_t resourceLimit=1024;
    OriginalMotionLibrary50LA()=default;
    OriginalMotionLibrary50LA(const OriginalMotionLibrary50LA&)=delete;
    OriginalMotionLibrary50LA& operator=(const OriginalMotionLibrary50LA&)=delete;
    OriginalMotionLibrary50LA(OriginalMotionLibrary50LA&&)=delete;
    OriginalMotionLibrary50LA& operator=(OriginalMotionLibrary50LA&&)=delete;
    ~OriginalMotionLibrary50LA(){
        // Application must outlive all slot owners. Resources retained by the
        // source's explicit no-array branch remain owned by this library.
        if(slotOwners_)std::terminate();
    }
    //3C8D9C..3C8DBC initializes00/04/08/0C/10/18=0 and14=1.
    //list18 is the ordered native owner vector. It is not an Entity list.
    std::uint32_t count00()const noexcept{return static_cast<std::uint32_t>(list18_.size());}
    std::uint32_t allocationCounter04()const noexcept{return allocationCounter04_;}
    std::uint32_t field08()const noexcept{return field08_;}
    std::uint32_t field0C()const noexcept{return field0C_;}
    std::uint32_t releaseLock10()const noexcept{return releaseLock10_;}
    std::uint32_t field14()const noexcept{return field14_;}
    // For an actual enclosing source owner that writes these source fields.
    // Unlocking alone does NOT sweep zero-reference resources in151180.
    void writeReleaseLock10(std::uint32_t value)noexcept{releaseLock10_=value;}
    void writeField14(std::uint32_t value)noexcept{field14_=value;}
    const OriginalMotionResource50LA* at(std::size_t i)const noexcept {
        return i<list18_.size()?list18_[i].get():nullptr;
    }
    const OriginalMotionResource50LA* find1523F0(std::string_view name)const noexcept {
        for(const auto& entry:list18_)
            if(original_motion_detail50LA::equal308F00(entry->fields_.name00,name))return entry.get();
        return nullptr;
    }
    bool owns(const OriginalMotionResource50LA* resource)const noexcept {
        for(const auto& entry:list18_)if(entry.get()==resource)return true;
        return false;
    }

    // Only key names are already normalized by the MCF reader; values retain
    // authored spelling. Resolver(path,out) must authenticate the actual MOT
    // and return its existing49O view. It runs only on a genuine cache miss.
    // No resource reference is added here; source153690..698 is separate.
    template<class ResolveAdmittedMotion>
    OriginalMotionLibraryResult50LA load151270(const OriginalMotionSection50LA& section,
        const OriginalMotionDefaults50LA& defaults,ResolveAdmittedMotion&& resolve,
        const OriginalMotionResource50LA*& output)noexcept {
        using namespace original_motion_detail50LA;
        try {
            const auto* authoredName=value(section,"name");
            const std::string_view name=authoredName?std::string_view(*authoredName):std::string_view("NONE");
            if(!ascii(name,271))return result(S::configuration,0x1512b8u);
            if(const auto* found=find1523F0(name)){
                output=found;return result(S::complete,0x1512d4u);
            }
            if(list18_.size()>=resourceLimit||allocationCounter04_>std::numeric_limits<std::uint32_t>::max()-0xdcu)
                return result(S::capacity,0x1512e8u);
#if defined(__FAST_MATH__) || defined(_M_FP_FAST) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
            return result(S::input,0x151818u);
#endif
            if(std::fegetround()!=FE_TONEAREST||!ascii(defaults.directory190,79))
                return result(S::input,0x1514fcu);
            auto next=std::unique_ptr<OriginalMotionResource50LA>(new OriginalMotionResource50LA);
            auto& f=next->fields_;
            //151300..350 ctor, then1513E0..414 and1514B0..4E8 bounded80.
            f.name00=std::string(name.substr(0,80));
            const auto* vue=value(section,"vue");
            if(vue&&!ascii(*vue,271))return result(S::configuration,0x151430u);
            f.vue51=vue?vue->substr(0,80):"NONE";
            std::int32_t authoredFrames=0;
            if(!integer(section,"framecount",0,authoredFrames)||
               !integer(section,"looping",0,f.loopingB4)||
               !integer(section,"fps",15,f.fpsC8)||
               !integer(section,"interpolateposition",defaults.interpolatePosition20,f.interpolatePositionB8)||
               !integer(section,"interpolaterotation",defaults.interpolateRotation24,f.interpolateRotationBC))
                return result(S::configuration,0x151524u);
            f.frameCountCC=static_cast<std::uint32_t>(authoredFrames);
            f.rotationTypeC0=defaults.rotationType28;f.positionTypeC4=defaults.positionType2C;
            if(const auto* type=value(section,"rotationtype")){
                if(!ascii(*type,63))return result(S::configuration,0x15159cu);
                if(equal308F00(*type,"BYTEAXIS"))f.rotationTypeC0=0;
                else if(equal308F00(*type,"SHORTAXIS"))f.rotationTypeC0=1;
                //1515F8 diagnostic falls back to actual animation+28.
            }
            if(const auto* type=value(section,"positiontype")){
                if(!ascii(*type,63))return result(S::configuration,0x151624u);
                if(equal308F00(*type,"LINEAR"))f.positionTypeC4=0;
                else if(equal308F00(*type,"BEZIER"))f.positionTypeC4=1;
                //151680 diagnostic falls back to actual animation+2C.
            }
            //1514F8/500/50C concatenates exact directory and truncated VUE;
            //1169D0 finds the last dot anywhere in that concatenated string.
            auto requested=defaults.directory190+f.vue51;
            const auto dot=requested.find_last_of('.');
            if(dot!=std::string::npos)requested.resize(dot);
            requested+=".MOT";
            if(!ascii(requested,271))return result(S::capacity,0x1516d0u);
            OriginalAdmittedMotion50LA admitted;
            if(!resolve(requested,admitted))return result(S::admittedMotion,0x152540u);
            if(!admitted.motion)return result(S::unsupportedVueFallback,0x151738u);
            const auto& view=*admitted.motion;
            if(!view.data||view.bytes<28||view.bytes>assets::maxMotBytes49O||
               !view.frameCount||view.frameCount>assets::maxMotKeysPerTrack49O||
               !std::isfinite(view.duration)||view.duration<=0.f||view.rotationType>1||
               view.positionCount>assets::maxMotTracks49O||view.rotationCount>assets::maxMotTracks49O||
               admitted.originalMemberName.empty())return result(S::admittedMotion,0x152570u);
            //152500 successful MOT path replaces CC/D0/C0/C4/AC/B0. Its
            // original position/rotation records are these same admitted bytes;
            // no guest polymorphic track allocations are imitated.
            next->bytes_.assign(view.data,view.data+view.bytes);
            next->motion_=view;next->motion_.data=next->bytes_.data();
            next->originalMemberName_=std::move(admitted.originalMemberName);
            next->requestedMemberName_=std::move(requested);
            f.frameCountCC=view.frameCount;f.durationD0=view.duration;
            f.rotationTypeC0=view.rotationType;f.positionTypeC4=1;
            f.positionCountAC=view.positionCount;f.rotationCountB0=view.rotationCount;
            if(const auto* transition=value(section,"transitiontime")){
                if(!number(*transition,f.transitionTimeD4))return result(S::configuration,0x151820u);
            }else{
                volatile float quarter=0.25f*f.durationD0;
                f.transitionTimeD4=quarter<=0.5f?quarter:0.5f;
            }
            const auto* resource=next.get();
            list18_.push_back(std::move(next)); //1518B0 append; genuine first-match order.
            allocationCounter04_+=0xdcu; //151368..370;151180 never subtracts.
            output=resource;return result(S::complete,0x151884u);
        }catch(const std::bad_alloc&){return result(S::allocation,0x1512e8u);}
         catch(...){return result(S::configuration,0x151270u);}
    }

    OriginalMotionLibraryResult50LA retain153610(const OriginalMotionResource50LA* resource)noexcept {
        using namespace original_motion_detail50LA;
        auto* found=mutableResource(resource);
        if(!found)return result(S::foreignResource,0x153690u);
        if(found->fields_.referenceD8==std::numeric_limits<std::int32_t>::max())
            return result(S::reference,0x153694u);
        ++found->fields_.referenceD8;return result(S::complete,0x153698u);
    }
    OriginalMotionLibraryResult50LA release151180(const OriginalMotionResource50LA* resource)noexcept {
        using namespace original_motion_detail50LA;
        std::size_t slot=list18_.size();
        //1511A8..1D0 scans the whole list and records the last pointer match.
        //Native owned pointers are unique, so exactly one match is admissible.
        for(std::size_t i=0;i<list18_.size();++i)if(list18_[i].get()==resource)slot=i;
        if(slot==list18_.size())return result(S::foreignResource,0x1511d4u);
        auto& ref=list18_[slot]->fields_.referenceD8;
        if(ref==std::numeric_limits<std::int32_t>::min())return result(S::reference,0x1511d8u);
        // A caller cannot consume another live model's published reference.
        //153F50 removes its own native guard immediately before its source
        //decrement; load rollback has not published guards in the first place.
        const auto guarded=list18_[slot]->publishedSlotReferences_;
        if(guarded&&(ref<=0||static_cast<std::uint32_t>(ref)<=guarded))
            return result(S::reference,0x1511d8u);
        --ref;
        if(ref>0||releaseLock10_)return result(S::complete,0x15125cu);
        //151040 destroys real native track-byte ownership here;151210..244
        //compacts in order and decrements00. No Entity counter is touched.
        list18_.erase(list18_.begin()+static_cast<std::ptrdiff_t>(slot));
        return result(S::complete,0x151258u);
    }
private:
    OriginalMotionResource50LA* mutableResource(const OriginalMotionResource50LA* resource)noexcept {
        for(auto& entry:list18_)if(entry.get()==resource)return entry.get();
        return nullptr;
    }
    std::vector<std::unique_ptr<OriginalMotionResource50LA>> list18_;
    std::uint32_t allocationCounter04_=0,field08_=0,field0C_=0,releaseLock10_=0,field14_=1;
    std::size_t slotOwners_=0;
    friend class OriginalModelMotionSet50LA;
};

// One real animation's153610 retain set and153FF4..154044 release set. The
// enclosing model owns this at a stable address and explicitly releases it
// during153F50, before its Application-owned shared library is destroyed.
class OriginalModelMotionSet50LA final {
public:
    static constexpr std::int32_t motionLimit=256;
    OriginalModelMotionSet50LA()=default;
    OriginalModelMotionSet50LA(const OriginalModelMotionSet50LA&)=delete;
    OriginalModelMotionSet50LA& operator=(const OriginalModelMotionSet50LA&)=delete;
    OriginalModelMotionSet50LA(OriginalModelMotionSet50LA&&)=delete;
    OriginalModelMotionSet50LA& operator=(OriginalModelMotionSet50LA&&)=delete;
    ~OriginalModelMotionSet50LA(){if(loaded_&&!released_)std::terminate();}
    template<class ResolveAdmittedMotion>
    OriginalMotionLibraryResult50LA load153610(OriginalMotionLibrary50LA& library,
        const OriginalMotionConfiguration50LA& configuration,std::int32_t count14,
        std::uint32_t keepArray1E0,const OriginalMotionDefaults50LA& defaults,
        ResolveAdmittedMotion&& resolve)noexcept {
        using namespace original_motion_detail50LA;
        if(loaded_||released_)return result(S::alreadyLoaded,0x153610u);
        if(count14<0||count14>motionLimit)return result(S::capacity,0x153648u);
        try {
            std::vector<const OriginalMotionResource50LA*> acquired;
            acquired.reserve(static_cast<std::size_t>(count14));
            struct Rollback {
                OriginalMotionLibrary50LA& library;
                std::vector<const OriginalMotionResource50LA*>& acquired;
                bool committed=false;
                ~Rollback(){if(!committed)for(const auto* p:acquired)library.release151180(p);}
            } rollback{library,acquired};
            std::vector<const OriginalMotionResource50LA*> slots;
            if(count14&&keepArray1E0)slots.resize(static_cast<std::size_t>(count14));
            // Native preflight/failure cleanup is deliberately stronger than
            // source's unsafe failed-load/unwritten-slot branch. Only fully
            // produced positive-count sets publish an actual1E4 pointer.
            for(std::int32_t i=0;i<count14;++i){
                const auto section=configuration.find("motion "+std::to_string(i+1));
                OriginalMotionLibraryResult50LA r;
                const OriginalMotionResource50LA* resource=nullptr;
                if(section==configuration.end())r=result(S::configuration,0x151298u);
                else r=library.load151270(section->second,defaults,resolve,resource);
                if(r.okay())r=library.retain153610(resource);
                if(!r.okay()){
                    // A failed retain cannot leave a newly constructed D8=0
                    // resource behind under the normal unlocked source state.
                    if(resource&&library.owns(resource)&&resource->fields().referenceD8==0)
                        library.release151180(resource);
                    r.motionOrdinal=static_cast<std::uint32_t>(i+1);return r;
                }
                acquired.push_back(resource);
                if(keepArray1E0)slots[static_cast<std::size_t>(i)]=resource;
            }
            // Prove the whole retain set before publishing guards. In
            // particular, a locked resource with negative sourceD8 cannot
            // masquerade as enough references for a new live model set.
            if(keepArray1E0)for(const auto* resource:acquired){
                std::size_t repeats=0;for(const auto* p:acquired)if(p==resource)++repeats;
                const auto required=static_cast<std::uint64_t>(resource->publishedSlotReferences_)+repeats;
                if(resource->fields_.referenceD8<0||
                   static_cast<std::uint64_t>(resource->fields_.referenceD8)<required)
                    return result(S::reference,0x153698u);
            }
            slots_=std::move(slots);library_=&library;count14_=count14;
            for(const auto* resource:slots_)++library.mutableResource(resource)->publishedSlotReferences_;
            keepArray1E0_=keepArray1E0;loaded_=true;++library.slotOwners_;
            rollback.committed=true;
            auto r=result(S::complete,count14?0x1536d0u:0x153638u);
            r.sourceReturn=count14?1u:0u;return r;
        }catch(const std::bad_alloc&){return result(S::allocation,0x153648u);}
         catch(...){return result(S::configuration,0x153610u);}
    }
    // Non-mutating, allocation-free admission for the ENTIRE release set.
    // Call this before an enclosing animation/Entity reference decrement.
    // The application library must still be alive and no caller may mutate
    // it or this set between admission and release. This is not a lock or a
    // concurrent-lifetime mechanism. release153F50 repeats the same admission.
    OriginalMotionLibraryResult50LA preflightRelease153F50()const noexcept {
        using namespace original_motion_detail50LA;
        if(released_){
            // A released set may outlive its former library. Do not dereference
            //that borrow merely to admit an idempotent second release.
            return result(slots_.empty()?S::complete:S::releaseState,0x154048u);
        }
        if(!loaded_){
            if(library_||count14_!=0||!slots_.empty())return result(S::releaseState,0x153ff8u);
            return result(S::complete,0x153ff8u);
        }
        if(!library_||!library_->slotOwners_||count14_<0||count14_>motionLimit)
            return result(S::releaseState,0x15401cu);
        const auto expected=keepArray1E0_?static_cast<std::size_t>(count14_):std::size_t(0);
        if(slots_.size()!=expected)return result(S::releaseState,0x153ff4u);
        // Check every pointer before the first release; repeated slots are
        // valid and have each acquired one sourceD8 reference.
        for(std::size_t i=0;i<slots_.size();++i){
            if(!library_->owns(slots_[i])){
                auto r=result(S::foreignResource,0x154018u);
                r.motionOrdinal=static_cast<std::uint32_t>(i+1);return r;
            }
            std::size_t repeats=0;for(const auto* p:slots_)if(p==slots_[i])++repeats;
            const auto guarded=slots_[i]->publishedSlotReferences_;
            const auto references=slots_[i]->fields_.referenceD8;
            // Validate against all published models, not merely this set's
            //multiplicity. Otherwise two sets could each pass against oneD8.
            if(guarded<repeats||references<0||static_cast<std::uint32_t>(references)<guarded){
                auto r=result(S::reference,0x15401cu);
                r.motionOrdinal=static_cast<std::uint32_t>(i+1);return r;
            }
        }
        return result(S::complete,0x15401cu);
    }
    OriginalMotionLibraryResult50LA release153F50()noexcept {
        using namespace original_motion_detail50LA;
        const auto admission=preflightRelease153F50();if(!admission.okay())return admission;
        if(released_)return result(S::complete,0x154048u);
        if(!loaded_){released_=true;return result(S::complete,0x153ff8u);}
        for(const auto* resource:slots_){
            auto* owned=library_->mutableResource(resource);
            --owned->publishedSlotReferences_;
            const auto r=library_->release151180(resource);
            // With the admitted immutable borrow, every decrement is safe:
            //D8>=all remaining guards and duplicate pointers survive until
            //their last occurrence. No callback/allocation intervenes here.
            if(!r.okay()){++owned->publishedSlotReferences_;return r;}
        }
        // keepArray1E0==0 intentionally has no151180 loop, matching source.
        // Those D8 references persist in the Application library; do not invent
        // a hidden source slot array or an unmatched decrement to erase them.
        std::vector<const OriginalMotionResource50LA*>().swap(slots_);
        --library_->slotOwners_;released_=true;return result(S::complete,0x154048u);
    }
    bool loaded153610()const noexcept{return loaded_&&!released_;}
    bool released()const noexcept{return released_;}
    std::int32_t count14()const noexcept{return count14_;}
    std::uint32_t keepArray1E0()const noexcept{return keepArray1E0_;}
    //153624 clears current18;153610 never selects a resource for playback.
    const OriginalMotionResource50LA* current18()const noexcept{return nullptr;}
    const OriginalMotionResource50LA* const* motionArray1E4()const noexcept {
        return loaded153610()&&!slots_.empty()?slots_.data():nullptr;
    }
    const OriginalMotionResource50LA* at(std::size_t i)const noexcept {
        return loaded153610()&&i<slots_.size()?slots_[i]:nullptr;
    }
private:
    OriginalMotionLibrary50LA* library_=nullptr;
    std::vector<const OriginalMotionResource50LA*> slots_;
    std::int32_t count14_=0;
    std::uint32_t keepArray1E0_=1;
    bool loaded_=false,released_=false;
};
} //namespace rr::game
