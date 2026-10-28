// Private implementation included once by ra_particle_authoring50la.h.
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__)
#error "RA emitter loader requires strict binary32 semantics"
#endif
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
    std::numeric_limits<float>::digits==24,"RA emitter loader requires binary32");
namespace ra_particle_authoring50LA {
// Actual native file/cursor and selected-cache custody. NOT a guest131230
// physical-cache history claim, generic archive service, or Entity/ref owner.
class Owner::NativeEmitterConfig157320 final {
    using Provider=a::OriginalStandardArchiveProvider50LA;
    std::shared_ptr<Provider> provider_;
    Provider::MemberView member_{};
    std::string requested_;
    std::size_t cursor_=0;
    bool opened_=false,closed_=false;
    std::vector<std::uint8_t> readBuffer_;
    // Source section index and selected cache are independent lifetime owners.
    std::vector<a::OriginalUiDatSection50KG> sections_;
    std::optional<a::OriginalUiDatSection50KG> selected_;
    bool current()const noexcept {
        if(!opened_||closed_||!provider_||!member_.member||!member_.data||
           !member_.bankGeneration||cursor_>member_.size)return false;
        Provider::MemberView now;
        return provider_->lookup132820(requested_,now)==Provider::Lookup::present&&
            now.member==member_.member&&now.data==member_.data&&now.size==member_.size&&
            now.mount==member_.mount&&now.ordinal==member_.ordinal&&
            now.revision==member_.revision&&now.bankGeneration==member_.bankGeneration;
    }
    bool read(void* destination,std::size_t size)noexcept {
        if(!current()||(!destination&&size)||size>member_.size-cursor_)return false;
        if(size)std::memcpy(destination,member_.data+cursor_,size);
        cursor_+=size;return true;
    }
public:
    NativeEmitterConfig157320()noexcept=default;
    NativeEmitterConfig157320(const NativeEmitterConfig157320&)=delete;
    NativeEmitterConfig157320& operator=(const NativeEmitterConfig157320&)=delete;
    ~NativeEmitterConfig157320(){close1573B0();}
    bool open1577D0(std::shared_ptr<Provider> provider,const std::string& name,
        const std::vector<std::uint8_t>& authenticated,std::uint32_t& pc) {
        pc=0x1577d0;
        if(opened_||closed_||!provider||authenticated.size()!=801u)return false;
        Provider::MemberView found;
        if(provider->lookup132820(name,found)!=Provider::Lookup::present||
           !found.member||!found.data||!found.bankGeneration||found.size!=authenticated.size()||
           std::memcmp(found.data,authenticated.data(),authenticated.size()))return false;
        // Actual first-provider identity is retained, not pointer-equality to
        // an independently decoded equal-content RA archive allocation.
        requested_=name;provider_=std::move(provider);member_=found;opened_=true;
        std::array<char,7> signature{};pc=0x157914;
        if(!read(signature.data(),signature.size()))return false;
        signature[6]=0;
        if(!std::strcmp(signature.data(),"BINARY"))return false;
        pc=0x12f640;if(!current())return false;cursor_=0;
        // Explicit native transport adaptation: bounded whole-member read,
        // then the existing159440/158648 text parser builds a real owned index.
        // This is not source compressed-device/cache activation chronology.
        pc=0x159440;readBuffer_.resize(found.size);
        if(!read(readBuffer_.data(),readBuffer_.size()))return false;
        pc=0x157500;
        if(!g::readOriginalGcParticleText50KG(readBuffer_,801,
            "839be25d68132969c3e7b5772ba1a5a5e66f280d48d188d76bc04ab8c3689d41",sections_))return false;
        return current();
    }
    const a::OriginalUiDatSection50KG* select157D00(const char* name,std::uint32_t& pc) {
        pc=0x157d00;if(!current())return nullptr;
        const a::OriginalUiDatSection50KG* found=nullptr;
        for(const auto& section:sections_)if(Provider::sameName(section.name,name)){
            if(found)return nullptr;found=&section;
        }
        // Source failed selection retains previous cache. Exact authenticated
        // RA has all three sections; absence is a rejected admission, not a
        // made-up empty section/default path.
        if(!found)return nullptr;
        selected_.reset();selected_.emplace(*found); // Actual owned selected cache.
        return &*selected_;
    }
    bool actualOpenCurrent()const noexcept {return current();}
    void close1573B0()noexcept {
        if(closed_)return;
        // Native close of the actual opened cursor/member first; no invented
        // provider.open/close API or physical131040 cache receipt.
        opened_=false;cursor_=0;member_={};requested_.clear();provider_.reset();
        selected_.reset(); //157CB0 selected payload/table before section index.
        std::vector<a::OriginalUiDatSection50KG>{}.swap(sections_);
        std::vector<std::uint8_t>{}.swap(readBuffer_);closed_=true;
    }
};
inline bool Owner::readLiveEmitter15E6D0(Emitter& owner,NativeEmitterConfig157320& config,
    Diagnostic& d,std::uint32_t& pc)noexcept {
    auto& f=owner.liveDefinition;auto& e=f.emission;
    const auto at=[&](std::uint32_t value)noexcept{pc=value;owner.loaderPC=value;};
    try {
        const auto* general=config.select157D00("General Info",pc);if(!general)return false;
        at(0x15e710);if(!a::originalUiTriple50KG(*general,"MinRange",{},e.minRange,d.text,pc))return false;
        at(0x15e72c);if(!a::originalUiTriple50KG(*general,"MaxRange",{},e.maxRange,d.text,pc))return false;
        at(0x15e744);if(!a::originalUiBoolean50KG(*general,"ApplySpreadToVelocity",true,e.applySpreadToVelocity,d.text,pc))return false;
        owner.field228=e.applySpreadToVelocity?1u:0u;
        // Derived reader then invokes common15AEA0, which selects General Info
        // again. Do not retain a reference to the released selected cache.
        general=config.select157D00("General Info",pc);if(!general)return false;
        at(0x15aee0);if(!a::originalUiInteger50KG(*general,"ParticleGroup",-1,e.particleGroup,d.text,pc)||e.particleGroup<0)return false;
        at(0x15af70);if(!a::originalUiNumber50KG(*general,"ParticleLife",1000,e.particleLife,d.text,pc))return false;
        at(0x15af8c);if(!a::originalUiNumber50KG(*general,"InitialSize",1,e.initialSize,d.text,pc))return false;
        at(0x15afa4);if(!a::originalUiNumber50KG(*general,"Growth",0,e.growth,d.text,pc))return false;
        at(0x15afc0);if(!a::originalUiNumber50KG(*general,"EmitRate",500,e.emitRate,d.text,pc))return false;
        at(0x15afd8);if(!a::originalUiNumber50KG(*general,"AgeAtBirth",0,f.ageAtBirth,d.text,pc))return false;
        at(0x15aff8);volatile float age=f.ageAtBirth*e.particleLife;
        if(!std::isfinite(age))return false;f.ageAtBirth=age;
        at(0x15b004);if(!a::originalUiTriple50KG(*general,"Velocity",{},f.authoredVelocity,d.text,pc))return false;
        for(std::size_t axis=0;axis<3u;++axis)
            std::memcpy(&owner.controls.velocity60[axis],&f.authoredVelocity[axis],sizeof(float));
        // XYZ only: actual constructor velocity60.W remains unchanged.
        std::int32_t mode=0;
        at(0x15b01c);if(!a::originalUiInteger50KG(*general,"EmitMode",0,mode,d.text,pc)||mode!=0)return false;
        f.timing.mode1D8=static_cast<std::uint32_t>(mode);
        at(0x15b03c);if(!a::originalUiNumber50KG(*general,"DelayTime",0,f.timing.delayTime1E0,d.text,pc))return false;
        at(0x15b068);owner.delay1DC=f.timing.delayTime1E0; // Delay slot executes BEFORE15B064 GroundLevel callee.
        at(0x15b064);if(!a::originalUiNumber50KG(*general,"GroundLevel",std::numeric_limits<float>::max(),f.groundLevel,d.text,pc))return false;
        at(0x15b080);if(!a::originalUiNumber50KG(*general,"GravityMultiplier",3,f.gravityMultiplier,d.text,pc))return false;
        at(0x15b0a0);if(!a::originalUiNumber50KG(*general,"Restitution",.55f,f.restitution,d.text,pc))return false;
        const auto* properties=config.select157D00("Properties",pc);if(!properties)return false;
        bool fades=false,additive=false,velocity=false,grows=false,nonlinear=false,glows=false,gravity=false,lit=true;
        const auto property=[&](const char* key,bool defaultValue,bool& value,std::uint32_t callPC){
            at(callPC);return a::originalUiBoolean50KG(*properties,key,defaultValue,value,d.text,pc);
        };
        if(!property("Fades",false,fades,0x15b1e8)||
           !property("HasVelocity",false,velocity,0x15b200)||
           !property("Grows",false,grows,0x15b218)||
           !property("FadeNonLinear",false,nonlinear,0x15b230)||
           !property("Glows",false,glows,0x15b248)||
           !property("Additive",false,additive,0x15b260)||
           !property("Gravity",false,gravity,0x15b278)||
           !property("Lit",true,lit,0x15b290)||
           !property("RandomMirrorAboutX",false,e.randomMirrorX,0x15b2a8)||
           !property("RandomMirrorAboutY",false,e.randomMirrorY,0x15b2c0))return false;
        // Source first reads all eight booleans into stack locals and two
        // mirrors into actual1CC/1D0, THEN performs these ordered198 writes.
        // Preserve all untouched source constructor bits (known198 zero).
        const auto bit=[&](bool on,std::uint32_t mask,std::uint32_t setPC,std::uint32_t clearPC)noexcept{
            at(on?setPC:clearPC);e.particleFlags=(e.particleFlags&~mask)|(on?mask:0u);
        };
        bit(fades,2u,0x15b2e0,0x15b2f0);
        bit(velocity,4u,0x15b30c,0x15b31c);
        bit(grows,8u,0x15b338,0x15b348);
        bit(nonlinear,0x1000u,0x15b364,0x15b374);
        bit(additive,1u,0x15b390,0x15b3a0);
        bit(glows,0x2000u,0x15b3bc,0x15b3cc);
        bit(gravity,0x20u,0x15b3e8,0x15b3f8);
        bit(!lit,0x800u,0x15b414,0x15b424);
        bit(f.groundLevel!=std::numeric_limits<float>::max(),0x200u,0x15b458,0x15b468);
        const auto* animation=config.select157D00("Animation",pc);if(!animation)return false;
        at(0x15b490);if(!a::originalUiNumber50KG(*animation,"FramesPerSet",-1,e.framesPerSet,d.text,pc))return false;
        at(0x15b4b0);if(!std::isfinite(e.framesPerSet)||e.framesPerSet==0)return false;
        volatile float inverse=1.f/e.framesPerSet;if(!std::isfinite(inverse))return false;
        owner.inverseFrames1C0=inverse;
        at(0x15b4c0);if(!a::originalUiInteger50KG(*animation,"NumberOfForcedSets",0,f.forcedSets,d.text,pc))return false;
        bit(e.framesPerSet>1.f,0x400u,0x15b4f4,0x15b504);
        at(0x15b508);if(f.forcedSets!=0)return false; // No fabricated1C4 allocation.
        at(0x15e758);e.authoredVelocityY220=f.authoredVelocity[1];
        at(0x161ea0);const auto* group=owner.numericManager70->group(e.particleGroup);
        if(!group||group->type!=8||group->first!=52||group->last!=75)return false;
        e.lastFrame=group->last;e.firstFrame=group->first;owner.currentFrame190=group->first;
        // Actual known RA bytes plus reached group producer, not preflight
        // endpoints substituted for a missing source manager/group.
        if(e.particleGroup!=4||e.particleLife!=1000||e.initialSize!=10||e.growth!=-6.5f||
           e.emitRate!=250||e.framesPerSet!=8||e.particleFlags!=0x1405u||
           !e.randomMirrorX||!e.randomMirrorY||!e.applySpreadToVelocity||
           e.minRange!=definition_.emission.minRange||e.maxRange!=definition_.emission.maxRange||
           f.authoredVelocity!=definition_.authoredVelocity||f.ageAtBirth!=0||
           f.groundLevel!=definition_.groundLevel||f.gravityMultiplier!=definition_.gravityMultiplier||
           f.restitution!=definition_.restitution||f.timing.delayTime1E0!=0)return false;
        at(0x15e7ac);return true;
    }catch(...){return false;}
}
inline bool Owner::loadEmitter15DEF0(std::size_t index,g::OriginalGcParticleManager50KG& manager,
    g::OriginalWreckComponent50LA& managerEntity,const g::OriginalRacerSceneObject50LA& attachment,
    std::shared_ptr<a::OriginalStandardArchiveProvider50LA> provider,const std::string& directory,
    const std::string& filename,const std::array<float,3>& offset,std::optional<std::uint32_t> sourceW,
    Diagnostic& d,std::uint32_t& pc)noexcept {
    d={};pc=0x15def0;
    if(!decoded_||closed_||index>=constructed_||!application_||!source_||!provider||
       application_->attachmentLock423F58()||!manager.prepared()||
       managerEntity.sessionGeneration!=generation_||!managerEntity.reference00||
       managerEntity.kind!=g::OriginalWreckComponentKind50LA::particleManager160B10||
       !attachment.constructed||attachment.released||!attachment.entity().reference00||
       attachment.entity().sessionGeneration!=generation_||
       !a::OriginalStandardArchiveProvider50LA::sameName(attachment.name38,attachments_[index].soulTree)||directory!="particle\\"||filename!="rafire.emt"||
       offset!=std::array<float,3>{{0,15,0}}||bytes_[1].size()!=801u)return false;
    auto& owner=emitters_[index];
    if(owner.loaderAttempted||owner.loaderComplete||owner.manager70||owner.attachment74||
       owner.entity.reference00!=1||owner.entity.sessionGeneration!=generation_||
       owner.entity.objectKey!=key(&owner.entity)||
       owner.entity.kind!=g::OriginalWreckComponentKind50LA::effect15DE00)return false;
    owner.loaderAttempted=true;owner.loaderPC=pc;
    const auto fail=[&]()noexcept{owner.loaderPC=pc;d.failure="RA reached emitter loader retained prefix";return false;};
    // Existing counted native constructor adopted; no recount, reset, global
    // type lookup, registration, source control activation or topology replay.
    pc=0x15df28;owner.manager70=&managerEntity;owner.numericManager70=&manager;
    pc=0x15df30;owner.attachment74=&attachment;
    pc=0x15df38;owner.offset50XYZ=offset;owner.sourceOffsetW=sourceW;
    NativeEmitterConfig157320 config;
    try {
        pc=0x15dfd8;owner.filename78=directory; // Prefix retained if append fails.
        pc=0x15e018;if(owner.filename78.size()+filename.size()>=260u)return fail();
        owner.filename78+=filename;
        if(!config.open1577D0(std::move(provider),owner.filename78,bytes_[1],pc))return fail();
        pc=0x15e6d0;
        if(std::fegetround()!=FE_TONEAREST)return fail();
        if(!readLiveEmitter15E6D0(owner,config,d,pc))return fail();
        pc=0x15ae54;if(!config.actualOpenCurrent())return fail();config.close1573B0();
        owner.loaderPC=0x15e0ac;owner.loaderComplete=true;pc=0x27da20;return true;
    }catch(...){return fail();} // Config RAII closes actual member/cache on every path.
}
} // namespace ra_particle_authoring50LA
