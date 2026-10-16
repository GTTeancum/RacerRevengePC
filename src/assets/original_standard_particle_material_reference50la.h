#pragma once
#include "original_standard_material_table50la.h"
#include <exception>
#include <new>
#include <utility>

namespace rr::assets {
// The concrete2461E0 primitive at particle-manager150. Its2465B0->299BE0
// handle owns one numeric14D590 reference in the SAME application382B38
// table. The host shared_ptr only pins that canonical owner. This is the
// cold constructor/reference/destructor domain; no246300 draw, packet,
// texture demand, renderer allocation or material-object readiness is implied.
class OriginalStandardParticleMaterialReference50LA final {
public:
    struct ColdFields2461E0 {
        //299AA0 and246200..248. No mutable buffer/draw entry point is exposed,
        // so the optional299B50 pair04 and246260 buffers10/14 remain absent.
        const void* optional04=nullptr;
        std::uint8_t byte0C=0,byte0D=0,byte0E=0xffu,flags0F=0;
        const void* allocation10=nullptr;
        const void* allocation14=nullptr;
        float scale18=1.f,scale1C=1.f;
        std::uint16_t value20=0,value22=0;
    };
    OriginalStandardParticleMaterialReference50LA() noexcept=default;
    ~OriginalStandardParticleMaterialReference50LA(){release246260();}
    OriginalStandardParticleMaterialReference50LA(const OriginalStandardParticleMaterialReference50LA&)=delete;
    OriginalStandardParticleMaterialReference50LA& operator=(const OriginalStandardParticleMaterialReference50LA&)=delete;

    // Caller lends the ACTUAL current36B970 after160F84 selected36B530,
    // plus the original SCB ParticleTextureMapName passed to160E00. PRT's
    // TextureMap field is never the material-name input to this constructor.
    // Admission bounds cover source's128-byte160F90 temporary and256-byte
    //299BE0 join. They do not normalize separators, case or source spelling.
    static bool acceptsTrackInput160F90(const std::string& directory,
        const std::string& scbName) noexcept {
        if(directory.size()>=128u||scbName.empty()||scbName.size()>=128u||
           directory.find('\0')!=std::string::npos||scbName.find('\0')!=std::string::npos)return false;
        const auto dot=scbName.find_last_of('.');
        const auto stemSize=dot==std::string::npos?scbName.size():dot;
        const auto separator=directory.empty()||directory.back()=='\\'?0u:1u;
        return directory.size()+separator+stemSize<256u;
    }
    bool acquireTrack299BE0(std::shared_ptr<OriginalStandardMaterialTable50LA> table,
        const std::string& actualDirectory36B970,const std::string& actualScbParticleTextureMapName,
        OriginalStandardMaterialDiagnostic50LA& d) noexcept {
        using S=OriginalStandardMaterialStatus50LA;
        d={};
        if(started_||released_)return fail(d,S::alreadyPrepared,0x299be0u);
        if(!table||!table->prepared()||
           !acceptsTrackInput160F90(actualDirectory36B970,actualScbParticleTextureMapName))
            return fail(d,S::input,0x160f90u);
        if(table->populationTerminal())return fail(d,S::phase,0x14d370u);
        if(table->demandTerminal())return fail(d,S::phase,0x24a430u);
        try {
            //160F90..FCC:1169D0 searches backwards for the final dot;
            //306478 zeroes that suffix. This is not a filesystem stem rule.
            std::string stem=actualScbParticleTextureMapName;
            const auto extension=stem.find_last_of('.');
            if(extension!=std::string::npos)stem.resize(extension);
            std::string requested=actualDirectory36B970;
            if(!requested.empty()&&requested.back()!='\\')requested+='\\';
            requested+=stem;
            //299C74 repeats the same suffix operation on the joined path.
            // Preserve it even for multiple dots or dots in the directory.
            const auto joinedExtension=requested.find_last_of('.');
            if(joinedExtension!=std::string::npos)requested.resize(joinedExtension);
            table_=std::move(table);started_=true;
            std::int32_t found=-1;
            const bool lookedUp=table_->find298C80(requested,found,d);
            material00_=found; //299CB4 precedes the source assertion/retain.
            d.row=material00_;d.storedSlots=1;d.sourceEffectsCommitted=true;
            if(!lookedUp)return false;
            //161004 supplies the actual application DefaultMT3833E8.
            // The canonical table selects its retained real template owner.
            if(!table_->retainDefault14D590(material00_,d))return false;
            retained_=true;d.retainedSlots=1;
            // Actual3D20C0 slot0C is2465B0. After299BE0 returns,2465CC
            // stores the caller's a2 byte;161008 supplied exactly zero.
            fields_.byte0C=0;return true;
        }catch(const std::bad_alloc&){return fail(d,S::allocation,0x299be0u);}
        catch(...){return fail(d,S::input,0x299be0u);}
    }
    bool started() const noexcept{return started_;}
    bool retained() const noexcept{return retained_;}
    const std::int32_t& material00() const noexcept{return material00_;}
    const ColdFields2461E0& coldFields() const noexcept{return fields_;}
    const OriginalStandardMaterialTable50LA* table382B38() const noexcept{return table_.get();}
    // Bounded246260->299AC0: constructor-only buffers are genuinely null.
    // Native idempotence prevents a second source release. Keep the handle
    // and table pin for inspection;14D500 leaves associated30 untouched.
    void release246260() noexcept {
        if(released_)return;
        if(fields_.optional04||fields_.allocation10||fields_.allocation14||
           (fields_.flags0F&1u))std::terminate();
        if(retained_){
            if(!table_||!table_->release14D500(material00_))std::terminate();
            retained_=false;
        }
        released_=true;
    }
private:
    static bool fail(OriginalStandardMaterialDiagnostic50LA& d,
        OriginalStandardMaterialStatus50LA status,std::uint32_t pc) noexcept {
        d.status=status;d.sourcePC=pc;return false;
    }
    std::shared_ptr<OriginalStandardMaterialTable50LA> table_;
    std::int32_t material00_=-1; //299AB0's actual constructor sentinel.
    ColdFields2461E0 fields_{};
    bool started_=false,retained_=false,released_=false;
};
}
