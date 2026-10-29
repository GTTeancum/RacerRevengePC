#pragma once
#include "../../../src/assets/original_ra_track_source_file50la.h"
#include "../../../src/game/original_text_config_primitives50la.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <exception>
#include <memory>
#include <optional>
#include <thread>
#include <new>
#include <string_view>
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game::original_world_environment50KG {class Owner;}
namespace rr::platform {
// The actual derived World34 Config. Native wrapper admission is separate from
// malloc(0x4d0)/157320 and cannot certify either 1B3880 or a native SCB reader.
class OriginalWorldSourceConfig50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class rr::game::original_world_environment50KG::Owner;
    using File=assets::OriginalRaTrackSourceFile50LA;
    using D=assets::OriginalStandardTextureDiagnostic50LA;
    enum class Return {bounded,zero,one};
    //Private occurrence family; no caller-selected filename/count relaxation.
    enum class Cohort {sceneScb,objectsTob,audioAub};
    Cohort cohort_=Cohort::sceneScb;
    struct Source final {
        std::int64_t file00;
        std::uint32_t binary08,opened0C,mode10,count14,index18;
        const void* text1C;std::uint32_t selected20;const void* child24;
        std::array<char,1024> name28; // Only byte zero is constructor-written.
        std::uint32_t field428,field42C,field430,field434,cacheCount438;
        std::array<std::uint32_t,32> cache43C;
        std::uint8_t *names4CC,*index4BC;std::uint32_t indexBytes4C0;
        const void* block4C4;std::uint32_t field4C8;
        Source()noexcept {
            file00=-1;binary08=0;opened0C=0;mode10=0;count14=0;index18=0;
            text1C=nullptr;selected20=0;child24=nullptr;name28[0]=0;
            field428=0;field42C=0;field430=0;field434=0;cacheCount438=0;
            cache43C.fill(0);names4CC=nullptr;index4BC=nullptr;
            indexBytes4C0=0;block4C4=nullptr;field4C8=0;
        }
    };
    const std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<File> file_;
    std::unique_ptr<Source> source_;
    std::unique_ptr<std::uint8_t[]> names_,index_,block_,lostBlock_;
    using Text=rr::game::OriginalTextConfigPrimitives50LA;
    Text::Table textTable_;
    File::Line12EE90 textInput_;
    Text::Bytes textBytes_,textLine_,textName_;
    bool textAttempted_=false,textReturned_=false;
    std::uint32_t textLineOrdinal_=0;
    void syncTextFields()noexcept {
        source_->count14=textTable_.count14;
        source_->index18=textTable_.capacity18;
        source_->text1C=textTable_.sourceStorage;
    }
    //Actual157500 call after same occurrence's nonbinary open+seek0. No
    //external callback, supplied reader success or fabricated tell is admitted.
    bool parseText157500(D& d)noexcept {
        if(textAttempted_||!source_||sourcePC_!=0x157ABCu||source_->binary08||
           source_->opened0C!=1||source_->mode10||source_->count14||source_->index18||
           source_->text1C||!file_||!file_->current(*this,source_->file00))return false;
        textAttempted_=true;textLineOrdinal_=0;
        for(;;){
            sourcePC_=0x157760u;
            if(!file_->read12EE90(*this,source_->file00,textInput_,d))return false;
            sourcePC_=0x157768u;
            if(!textInput_.present){
                const auto final=Text::finishLastRow157770(textTable_,textLineOrdinal_);
                syncTextFields();sourcePC_=final.pc;
                if(final.stop!=Text::Stop::complete)return false;
                textReturned_=true;return true;
            }
            //Transfer only genuinely known destination bytes, never copy an
            //indeterminate std::array wholesale. Persistent buffers retain
            //partial source stack state across any bounded return.
            for(std::size_t i=0;i<textInput_.bytes.size();++i){
                textBytes_.known[i]=textInput_.known[i];
                if(textInput_.known[i])textBytes_.value[i]=static_cast<std::uint8_t>(textInput_.bytes[i]);
            }
            auto step=Text::split159440(textBytes_,textLine_);sourcePC_=step.pc;
            if(step.stop!=Text::Stop::complete)return false;
            if(textLine_.value[0]=='['){
                step=Text::sectionName157548(textLine_,textName_);sourcePC_=step.pc;
                if(step.stop!=Text::Stop::complete)return false;
                const auto found=Text::lookupText157C44(textTable_,textName_);sourcePC_=found.pc;
                if(found.stop!=Text::Stop::complete)return false;
                if(found.index!=-1){
                    //Duplicate follows warning/shared assertion state at1576C8;
                    //that diagnostic owner is not joined. Do not merge, skip,
                    //increment line count, or return parser success past it.
                    sourcePC_=0x1576C8u;return false;
                }
                Text::AllocationRequest request;
                step=Text::prepareNewRow15758C(textTable_,request);
                syncTextFields();sourcePC_=step.pc;
                if(step.stop==Text::Stop::needsAllocation){
                    step=Text::continueAllocation(textTable_);
                    syncTextFields();sourcePC_=step.pc;
                }
                if(step.stop!=Text::Stop::complete)return false;
                step=Text::copyRowName15761C(textTable_,textName_);sourcePC_=step.pc;
                if(step.stop!=Text::Stop::complete)return false;
                std::int64_t tell=0;sourcePC_=0x15762Cu;
                if(!file_->tell12F520(*this,source_->file00,tell,d))return false;
                step=Text::finishRow157654(textTable_,static_cast<std::uint64_t>(tell),textLineOrdinal_);
                syncTextFields();sourcePC_=step.pc;
                if(step.stop!=Text::Stop::complete)return false;
            }
            //157544 branch delay for nonheaders,157748 for successful headers.
            ++textLineOrdinal_;
        }
    }

    std::uint32_t sectionAttempts_=0;bool sectionReturned_=false;
    std::size_t blockProduced_=0; //Latest actual payload bytes copied, not requested capacity.
    std::optional<std::uint32_t> blockSectionBytes_; //Latest reached source index length.
    std::size_t blockRequestedBytes_=0,blockKnownBytes_=0,lostBlockKnownBytes_=0;
    bool blockPayloadReturned_=false;
    //All source payload reads start at cache0. Reuse/realloc preserves the
    //known old prefix; newly allocated suffix is never implicitly initialized.
    std::size_t sourceSelectedKnownBytes()const noexcept {
        return blockSectionBytes_?std::min<std::size_t>(*blockSectionBytes_,blockKnownBytes_):0;
    }
    bool sourceSectionPayloadCurrent(std::uint32_t selectedBytes)const noexcept {
        return source_&&block_&&source_->block4C4==block_.get()&&blockSectionBytes_&&
            *blockSectionBytes_==selectedBytes&&blockPayloadReturned_&&
            blockRequestedBytes_==source_->field4C8&&blockProduced_<=blockRequestedBytes_&&
            blockKnownBytes_<=blockRequestedBytes_&&sourceSelectedKnownBytes()==selectedBytes;
    }
    bool readSourceSectionPayload157F4C(D& d)noexcept {
        sourcePC_=0x157F4C;blockPayloadReturned_=false;blockProduced_=0;
        if(!source_||!blockSectionBytes_||!block_||source_->block4C4!=block_.get())return false;
        blockRequestedBytes_=source_->field4C8; //Actual a3 load157F48; t0=1 at157F50.
        if(*blockSectionBytes_>blockRequestedBytes_||blockKnownBytes_>blockRequestedBytes_)return false;
        const bool nativeReturned=file_->read12FB30(*this,source_->file00,block_.get(),
            blockRequestedBytes_,blockProduced_,d);
        //Even a native transport refusal may have committed a prefix. Keep
        //those bytes and the genuinely retained old tail; do not clear either.
        if(blockProduced_>blockRequestedBytes_)return false;
        blockKnownBytes_=std::max(blockKnownBytes_,blockProduced_);
        if(!nativeReturned)return false; //Not an original short-count return.
        sourcePC_=0x157F54;blockPayloadReturned_=true;
        //Original branch ignores12FB30's integer and its delay returns1.
        //A short read is NOT exact-read success. Selector return1 is independent
        //of byte knowledge. The separate currentness/getter gates refuse any
        //unproven selected prefix; no unknown suffix is exposed.
        return true;
    }
    std::uint32_t fieldCopyPC_=0;
    bool allocated_=false,sourceClosed_=false,nativeClosed_=false;
    bool attemptRetained_=false;unsigned opens_=0;
    Return returned_=Return::bounded;
    std::uint32_t sourcePC_=0;
    std::size_t namesProduced_=0,indexProduced_=0,currentNameProduced_=0;
    std::uint32_t currentNameLength_=0;
    OriginalWorldSourceConfig50LA()noexcept=default;
    bool bindNative(std::shared_ptr<File> file)noexcept {
        if(thread_!=std::this_thread::get_id()||file_||allocated_||!file)return false;
        file_=std::move(file);return true;
    }
    // True means the allocator returned and publication may proceed, INCLUDING
    // a genuine NULL. Caller must publish World34 before entering 1577D0.
    bool allocate1B3984()noexcept {
        if(thread_!=std::this_thread::get_id()||!file_||allocated_)return false;
        allocated_=true;sourcePC_=0x1B3984;
        try {source_.reset(new Source);}catch(...){source_.reset();}
        sourcePC_=0x1B399C;return true;
    }
    //Separate actual1B43AC allocation/157320 occurrence. Caller owns the
    //unconditional1B43C4 World34 publication, including genuine NULL.
    bool allocate1B43AC()noexcept {
        if(thread_!=std::this_thread::get_id()||!file_||allocated_||nativeClosed_||sourceClosed_)return false;
        cohort_=Cohort::objectsTob;allocated_=true;sourcePC_=0x1B43AC;
        try {source_.reset(new Source);}catch(...){source_.reset();}
        sourcePC_=0x1B43C4;return true;
    }
    //Distinct actual audio Config allocation; the parent publishes World38 at
    //1B47AC even when this genuine allocation returns NULL. No Audio owner is
    //created, and neither closed SCB nor TOB occurrence is reused.
    bool allocate1B4794()noexcept {
        if(thread_!=std::this_thread::get_id()||!file_||allocated_||nativeClosed_||sourceClosed_)return false;
        cohort_=Cohort::audioAub;allocated_=true;sourcePC_=0x1B4794;
        try {source_.reset(new Source);}catch(...){source_.reset();}
        sourcePC_=0x1B47AC;return true;
    }
    static std::uint32_t little(const std::uint8_t* p)noexcept {
        return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|
            (std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);
    }
    bool readExact(void* p,std::size_t n,std::size_t& produced,
                   std::uint32_t call,std::uint32_t next,D& d)noexcept {
        sourcePC_=call;produced=0;
        if(!file_->read12FB30(*this,source_->file00,p,n,produced,d))return false;
        // Real reader return is retained. A short read bounds before the next
        // source consumer, never interpreting native unwritten bytes as zeros.
        sourcePC_=next;return produced==n;
    }
    Return closeFailure(std::uint32_t call,std::uint32_t store,D& d)noexcept {
        sourcePC_=call;
        if(!file_->close131040(*this,source_->file00,false,d))return Return::bounded;
        sourcePC_=store;source_->file00=-1;returned_=Return::zero;
        sourcePC_=0x157ACC;return returned_;
    }
    Return open1577D0(std::string_view path,D& d)noexcept {
        d={};sourcePC_=0x1577E8;
        if(thread_!=std::this_thread::get_id()||!file_||!allocated_||
           sourceClosed_||nativeClosed_||!source_)return Return::bounded;
        //Each private occurrence admits only its own authentic first/fallback
        //names. AUB uses the SAME retained Track root, never a remount or a
        //native audio-authoring reader. The fallback needs actual return zero.
        const bool scene=cohort_==Cohort::sceneScb;
        const bool audio=cohort_==Cohort::audioAub;
        const std::string_view first=scene?"tracks\\ra.scb":audio?"tracks\\ra.aub":"tracks\\ra.tob";
        const std::string_view fallback=scene?"tracks\\ra.scn":audio?"tracks\\ra.aud":"tracks\\ra.toy";
        if((opens_==0&&path!=first)||
           (opens_==1&&(path!=fallback||returned_!=Return::zero))||
           opens_>1||source_->text1C||source_->file00!=-1||source_->index4BC||
           source_->block4C4||source_->names4CC||names_||index_)return Return::bounded;
        ++opens_;returned_=Return::bounded;sourcePC_=0x1578D0;
        // File wrapper owns even a failed-open attempt; native cleanup below
        // withdraws it before releasing this exact occurrence.
        attemptRetained_=true;
        if(!file_->open131230(*this,path,source_->file00,d))return Return::bounded;
        sourcePC_=0x1578D8;
        if(source_->file00==-1){returned_=Return::zero;sourcePC_=0x157ACC;return returned_;}
        source_->field42C=0;source_->opened0C=1;source_->mode10=0;
        std::array<std::uint8_t,7> magic;std::size_t done=0;
        if(!readExact(magic.data(),7,done,0x157914,0x15791C,d))return Return::bounded;
        magic[6]=0; // actual strcmp delay-slot store15792C
        if(std::memcmp(magic.data(),"BINARY",6)!=0){
            source_->binary08=0;sourcePC_=0x157AB0;
            if(!file_->seek12F640(*this,source_->file00,0,d))return Return::bounded;
            sourcePC_=0x157ABC;
            if(!parseText157500(d))return Return::bounded;
            sourcePC_=0x157AC4;source_->field428=0;
            sourcePC_=0x157AC8;returned_=Return::one;
            sourcePC_=0x157ACC;return returned_;
        }
        source_->binary08=1;std::array<std::uint8_t,4> word;
        if(!readExact(word.data(),4,done,0x157950,0x157958,d))return Return::bounded;
        if(little(word.data())!=1)return closeFailure(0x157970,0x15797C,d);
        if(!readExact(word.data(),4,done,0x157998,0x1579A0,d))return Return::bounded;
        const auto count=little(word.data());
        if(!count)return closeFailure(0x1579B4,0x1579C0,d);
        source_->count14=count;
        //Exact authenticated counts remain occurrence-specific. Unknown counts
        //retain the reached count14 store, before allocation-size effects.
        sourcePC_=0x1579DC;if(count!=(scene?1472u:audio?74u:713u))return Return::bounded;
        try {names_.reset(new std::uint8_t[std::size_t(count)*1024]);}catch(...){names_.reset();}
        source_->names4CC=names_.get(); // NULL publication1579E4 is unconditional
        if(!names_)return closeFailure(0x1579FC,0x157A08,d);
        for(std::uint32_t i=0;i<count;++i){
            word.fill(0); // actual stack5C zero, NOT destination-name zeroing
            currentNameProduced_=0;currentNameLength_=0;
            if(!readExact(word.data(),4,done,0x157A30,0x157A38,d))return Return::bounded;
            currentNameLength_=little(word.data());sourcePC_=0x157A4C;
            if(currentNameLength_>1024)return Return::bounded;
            if(!readExact(names_.get()+std::size_t(i)*1024,currentNameLength_,
                currentNameProduced_,0x157A4C,0x157A54,d))return Return::bounded;
            ++namesProduced_;
        }
        source_->indexBytes4C0=count*8;sourcePC_=0x157A6C;
        try {index_.reset(new std::uint8_t[source_->indexBytes4C0]);}catch(...){index_.reset();}
        source_->index4BC=index_.get();sourcePC_=0x157A8C;
        if(!index_)return Return::bounded; // NULL retained before actual read
        if(!readExact(index_.get(),source_->indexBytes4C0,indexProduced_,
            0x157A8C,0x157A94,d))return Return::bounded;
        returned_=Return::one;sourcePC_=0x157ACC;return returned_;
    }
    //Only the real completed TOB binary-open prefix. No selected section,
    //World field, particle owner, or completed vA0 receipt is implied.
    bool sourceObjectConfigCurrent()const noexcept {
        return thread_==std::this_thread::get_id()&&cohort_==Cohort::objectsTob&&
            allocated_&&!sourceClosed_&&!nativeClosed_&&source_&&file_&&opens_==1&&
            attemptRetained_&&returned_==Return::one&&file_->current(*this,source_->file00)&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&
            source_->count14==713&&names_&&index_&&source_->names4CC==names_.get()&&
            source_->index4BC==index_.get()&&source_->indexBytes4C0==713u*8u&&
            namesProduced_==713&&indexProduced_==713u*8u&&!sectionAttempts_&&!sectionReturned_&&
            !source_->text1C&&!source_->child24&&!source_->cacheCount438&&
            !source_->block4C4&&!block_&&!lostBlock_&&source_->field4C8==0;
    }
    //Only completed first AUB binary open: no section, audio Entity, caller log
    //or vE4 return receipt. Fallback/export remains outside this predicate.
    bool sourceAudioConfigCurrent()const noexcept {
        return thread_==std::this_thread::get_id()&&cohort_==Cohort::audioAub&&
            allocated_&&!sourceClosed_&&!nativeClosed_&&source_&&file_&&opens_==1&&
            attemptRetained_&&returned_==Return::one&&file_->current(*this,source_->file00)&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&
            source_->count14==74&&names_&&index_&&source_->names4CC==names_.get()&&
            source_->index4BC==index_.get()&&source_->indexBytes4C0==74u*8u&&
            namesProduced_==74&&indexProduced_==74u*8u&&!sectionAttempts_&&!sectionReturned_&&
            !source_->text1C&&!source_->child24&&!source_->cacheCount438&&
            !source_->block4C4&&!block_&&!lostBlock_&&source_->field4C8==0;
    }
#include "original_world_source_ambient_config50la.inl"
#include "original_world_source_soultreesound_config50la.inl"
#include "original_world_source_materialsound_config50la.inl"
#include "original_world_source_material_assignment_config50la.inl"
    //Private exact TOB PodRacer/optionalRain/321_Anims sequence.
#include "original_world_source_object_config50la.inl"
#include "original_world_source_terrain_config50la.inl"
#include "original_world_source_toy_config50la.inl"
#include "original_world_source_trigger_config50la.inl"
#include "original_world_source_physicsobjects_config50la.inl"
#include "original_world_source_cscamera_config50la.inl"
#include "original_world_source_sunflare_config50la.inl"
#include "original_world_source_particle_config50la.inl"
#include "original_world_source_buoys_config50la.inl"
#include "original_world_source_staticmodelparam_config50la.inl"

    // Actual bounded binary selections. The returned bool describes completion;
    // sourceReturned separately describes v0. No native ParameterDocument.
    bool selectBinarySection157D00(bool environment,bool& sourceReturned,D& d)noexcept {
        d={};sourceReturned=false;sourcePC_=0x157D1C;
        if(thread_!=std::this_thread::get_id()||(environment?sectionAttempts_!=2:sectionAttempts_>=2)||(sectionAttempts_&&!sectionReturned_)||!source_||sourceClosed_||nativeClosed_||
           returned_!=Return::one||!file_||!file_->current(*this,source_->file00)||
           source_->opened0C!=1||source_->binary08!=1||source_->mode10!=0||
           source_->count14!=1472||source_->names4CC!=names_.get()||source_->index4BC!=index_.get()||
           !names_||!index_||namesProduced_!=1472||indexProduced_!=1472*8||
           source_->child24||source_->text1C||source_->cacheCount438||
           source_->block4C4!=block_.get()||lostBlock_||(sectionAttempts_==0&&block_))
            return false;
        ++sectionAttempts_;sectionReturned_=false;
        //157AF0 copies and lowercases the actual section name, then searches the actual
        // loaded sorted 1024-stride source names. No vector/native parsed index.
        const auto compareName=[&](std::uint32_t ordinal,int& result)noexcept {
            if(ordinal>=source_->count14)return false;
            const char* value=reinterpret_cast<const char*>(names_.get()+std::size_t(ordinal)*1024);
            if(!std::memchr(value,0,1024))return false;
            result=std::strcmp(environment?"environment":"sceneinfo",value);return true;
        };
        std::uint32_t low=0,high=source_->count14-1,mid=high/2;int compared=0;bool found=false;
        for(;;){sourcePC_=0x157B8C;if(!compareName(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;
        }
        if(!found){sourcePC_=0x157BE8;if(!compareName(low,compared))return false;
            if(!compared){mid=low;found=true;}
            else {sourcePC_=0x157C1C;if(!compareName(high,compared))return false;if(!compared){mid=high;found=true;}}
        }
        if(!found){sourcePC_=0x15821C;return true;} // Genuine missing-section return0.
        // Native safety aperture for the exact authenticated section.
        // Source search has returned; preserve no fabricated cache publication.
        if(mid!=(environment?1u:200u))return false;
        const char* actualName=environment?"Environment":"SceneInfo";
        const auto nameBytes=environment?11u:9u;
        std::memcpy(source_->name28.data(),actualName,nameBytes); //157E90
        source_->name28[nameBytes]=0;sourcePC_=0x157E9C;
        const auto offset=little(index_.get()+std::size_t(mid)*8);
        const auto bytes=little(index_.get()+std::size_t(mid)*8+4);
        blockSectionBytes_=bytes;blockPayloadReturned_=false; //Actual index length, not4C8 read size.
        if(offset!=(environment?37657u:123073u)||bytes!=(environment?839u:92u)){sourcePC_=0x157EB0;return false;}
        if(!source_->block4C4){
            source_->field4C8=bytes;sourcePC_=0x157EE4; // size BEFORE actual malloc.
            block_.reset(new(std::nothrow) std::uint8_t[bytes]);blockKnownBytes_=0;
            source_->block4C4=block_.get();sourcePC_=0x157EF0; // includes genuine NULL.
        }else if(source_->field4C8<bytes){
            const auto oldBytes=source_->field4C8;source_->field4C8=bytes;sourcePC_=0x157ECC;
            std::unique_ptr<std::uint8_t[]> next(new(std::nothrow) std::uint8_t[bytes]);
            if(next){std::memcpy(next.get(),block_.get(),oldBytes);block_=std::move(next);}
            else {lostBlock_=std::move(block_);lostBlockKnownBytes_=blockKnownBytes_;blockKnownBytes_=0;} // Original reallocNULL loses public pointer; native old backing is retained separately.
            source_->block4C4=block_.get();sourcePC_=0x157EE0; // unconditional, includingNULL.
        }
        sourcePC_=0x157EF4; // NonNULL sufficient cache genuinely skips allocation, not seek/read.
        if(!file_->seek12F640(*this,source_->file00,offset,d)){sourcePC_=0x157F08;return false;}
        std::array<std::uint8_t,4> countBytes{}; // Original stack86C was zeroed157F10.
        std::size_t done=0;
        if(!readExact(countBytes.data(),4,done,0x157F28,0x157F30,d))return false;
        source_->selected20=little(countBytes.data());sourcePC_=0x157F38;
        if(!block_){sourcePC_=0x157F4C;return false;} // preserve seek/read/count prefix before NULL output call.
        if(!readSourceSectionPayload157F4C(d))return false;
        sectionReturned_=true;sourceReturned=true;sourcePC_=0x15821C;return true;
    }
#include "original_world_source_text_section50la.inl"
#include "original_world_source_text_field50la.inl"
#include "original_world_source_text_integer50la.inl"
    bool selectSceneInfo157D00(bool& returned,D& d)noexcept {
        return source_&&source_->binary08==0?selectTextSection157D00(false,returned,d):selectBinarySection157D00(false,returned,d);
    }
    bool selectEnvironment157D00(bool& returned,D& d)noexcept {
        return source_&&source_->binary08==0?selectTextSection157D00(true,returned,d):selectBinarySection157D00(true,returned,d);
    }
    bool environmentCurrent()const noexcept {
        return thread_==std::this_thread::get_id()&&source_&&!sourceClosed_&&!nativeClosed_&&sectionReturned_&&sectionAttempts_==3&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&source_->selected20==18&&
            source_->field4C8==839&&source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(839)&&!lostBlock_&&
            std::memcmp(source_->name28.data(),"Environment",12)==0&&file_&&file_->current(*this,source_->file00);
    }
    bool sceneInfoCurrent()const noexcept {
        return thread_==std::this_thread::get_id()&&source_&&!sourceClosed_&&!nativeClosed_&&sectionReturned_&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&source_->selected20==2&&
            source_->field4C8==92&&source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(92)&&
            std::memcmp(source_->name28.data(),"SceneInfo",10)==0&&file_&&file_->current(*this,source_->file00);
    }
    //158240 uses 43-byte binary rows: name[35], unaligned value offset@35,
    // type@39. Only string lookup is consumed here; it does not allocate438.
    bool findBinaryField158240(bool environment,const char* lower,std::string_view& value,bool& found)noexcept {
        if(source_&&source_->binary08==0)return findTextField158240(environment,lower,value,found);
        found=false;value={};sourcePC_=0x158280;if(environment?!environmentCurrent():!sceneInfoCurrent())return false;
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=source_->selected20)return false;const auto* row=block_.get()+std::size_t(i)*43;
            if(!std::memchr(row,0,35))return false;result=std::strcmp(lower,reinterpret_cast<const char*>(row));return true;
        };
        const auto capture=[&](std::uint32_t i)noexcept {
            const auto offset=little(block_.get()+std::size_t(i)*43+35);
            if(offset>=sourceSelectedKnownBytes())return false;const auto* begin=reinterpret_cast<const char*>(block_.get()+offset);
            const auto* end=static_cast<const char*>(std::memchr(begin,0,sourceSelectedKnownBytes()-offset));
            if(!end)return false;
            fieldCopyPC_=sourcePC_==0x1583F8?0x158474:sourcePC_==0x1584F0?0x158560:0x158620;
            value=std::string_view(begin,std::size_t(end-begin));found=true;return true;
        };
        std::uint32_t low=0,high=source_->selected20-1,mid=(high-low)/2;int compared=0;
        for(;;){sourcePC_=0x1583F8;if(!compare(mid,compared))return false;if(!compared)return capture(mid);
            if(compared>=0)low=mid;else high=mid;mid=low+(high-low)/2;if(high-low<2)break;
        }
        sourcePC_=0x1584F0;if(!compare(low,compared))return false;if(!compared)return capture(low);
        sourcePC_=0x1585B0;if(!compare(high,compared))return false;if(!compared)return capture(high);
        sourcePC_=0x158640;return true; // actual v0=0; destination is NOT written.
    }
    bool readSceneName158240(std::array<std::optional<std::uint8_t>,260>& destination,
        bool& sourceReturned,D& d)noexcept {
        d={};sourceReturned=false;
        std::string_view value;bool found=false;
        if(!findBinaryField158240(false,"scenename",value,found))return false;
        if(source_->binary08==0){
            //2709BC passes the actual World244 destination, max=-1. Text
            //missing DOES strcpy the empty literal; binary missing below does not.
            sourcePC_=found?0x1588B4u:0x1588E4u;
            //Original next field begins348 (270564), so native custody for
            //World244 ends there. Source strcpy itself is unbounded; exceeding
            //this aperture is a native pre-copy stop, NOT source truncation.
            constexpr std::size_t ownedWorld244Bytes=0x348u-0x244u;
            if(value.size()>=ownedWorld244Bytes)return false;
            for(std::size_t i=0;i<value.size();++i)destination[i]=static_cast<std::uint8_t>(value[i]);
            destination[value.size()]=0;
            sourceReturned=found;sourcePC_=0x1588F0u;return true;
        }
        // Exact original SceneInfo has no SceneName. Do not turn constructor
        // backing into an empty-string getter store or admit unexpected values.
        if(found)return false;
        sourcePC_=0x1588F0;return true;
    }
    bool readSceneInfoInteger158B30(std::string_view key,std::int32_t defaultValue,
        std::int32_t& actualOutput,bool& sourceReturned,D& d)noexcept {
        d={};sourceReturned=false;const char* lower=nullptr;
        if(key=="NumPortals"&&defaultValue==0)lower="numportals";
        else if(key=="NumZones"&&defaultValue==1)lower="numzones";
        else if(key=="UseEnvironmentNode"&&defaultValue==0)lower="useenvironmentnode";
        else return false;
        std::string_view value;bool found=false;
        if(!findBinaryField158240(false,lower,value,found))return false;
        if(!found){actualOutput=defaultValue;sourcePC_=0x158B60;return true;}
        if(source_->binary08==0){
            if(!convertInteger158920(value,actualOutput))return false;
        }else {
            // Preserve previously admitted binary values/caller restriction.
            if(value!="10"||key=="UseEnvironmentNode"){sourcePC_=0x158A34;return false;}
            actualOutput=10;
        }
        sourceReturned=true;sourcePC_=0x158B68;return true;
    }

    bool readMissingVector158C00(std::string_view key,const std::array<std::uint32_t,4>& actualDefault,
        std::array<std::uint32_t,4>& actualDestination,bool& sourceReturned,D& d)noexcept {
        d={};sourceReturned=false;const char* lower=nullptr;
        if(key=="DefaultStartPosition")lower="defaultstartposition";
        else if(key=="DefaultStartDirection")lower="defaultstartdirection";
        else return false;
        if(sectionAttempts_!=2)return false;
        std::string_view value;bool found=false;
        if(!findBinaryField158240(false,lower,value,found))return false;
        if(found){sourcePC_=0x158C54;return false;} // No unjoined decimal/vector arithmetic.
        // Missing158240 sets local text[0] to the real empty literal;158C4C
        // branches to bit-preserving scalar loads/stores. No W store occurs.
        actualDestination[0]=actualDefault[0];sourcePC_=0x158CD0;
        actualDestination[1]=actualDefault[1];sourcePC_=0x158CD8;
        actualDestination[2]=actualDefault[2];sourcePC_=0x158CE0;
        return true; // Genuine getter v0=0, outputXYZ nevertheless written.
    }
    bool readEnvironmentInteger158B30(std::string_view key,std::int32_t defaultValue,
        std::int32_t& actualOutput,bool& sourceReturned,D& d)noexcept {
        d={};sourceReturned=false;if(defaultValue!=1)return false;
        const char* lower=nullptr;
        static constexpr std::array<const char*,10> keys{{"CubeFileForZone1","CubeFileForZone2","CubeFileForZone3","CubeFileForZone4","CubeFileForZone5","CubeFileForZone6","CubeFileForZone7","CubeFileForZone8","CubeFileForZone9","CubeFileForZone10"}};
        static constexpr std::array<const char*,10> lowered{{"cubefileforzone1","cubefileforzone2","cubefileforzone3","cubefileforzone4","cubefileforzone5","cubefileforzone6","cubefileforzone7","cubefileforzone8","cubefileforzone9","cubefileforzone10"}};
        if(key=="NumberOfSkyCubes")lower="numberofskycubes";
        else for(std::size_t i=0;i<keys.size();++i)if(key==keys[i]){lower=lowered[i];break;}
        if(!lower)return false;std::string_view value;bool found=false;
        if(!findBinaryField158240(true,lower,value,found))return false;
        if(!found){actualOutput=defaultValue;sourcePC_=0x158B60;return true;}
        if(source_->binary08==0){
            if(!convertInteger158920(value,actualOutput))return false;
        }else {
            // Preserve binary authored0/1 admission and caller-owned subtraction.
            if(value!="0"&&value!="1"){sourcePC_=0x158A34;return false;}
            actualOutput=value=="0"?0:1;
        }
        sourceReturned=true;sourcePC_=0x158B68;return true;
    }
    bool readEnvironmentString158AD0(std::string_view key,std::string_view defaultValue,
        char* actualDestination,std::size_t capacity,bool& sourceReturned,D& d)noexcept {
        d={};sourceReturned=false;if(defaultValue!="none")return false;
        const char* lower=nullptr;std::string_view expected;
        if(key=="CubeFile1"){lower="cubefile1";expected="ra.cub";}
        else if(key=="CubeFile"){lower="cubefile";}
        else if(key=="EcosystemFile"){lower="ecosystemfile";}
        else if(key=="TerrainFile"){lower="terrainfile";expected="nat13.trn";}
        else if(key=="WaterFile"){lower="waterfile";}
        else if(key=="ParticleTextureMapName"){lower="particletexturemapname";expected="demoprt.tga";}
        else return false;
        std::string_view value;bool found=false;
        if(!findBinaryField158240(true,lower,value,found))return false;
        if(source_->binary08==0&&!found){
            //Text missing158240 writes empty to the REAL destination before
            //158AD0's subsequent default strcpy. Preserve that partial store
            //even if native capacity later bounds the default copy.
            sourcePC_=0x1588E4u;
            if(!actualDestination||!capacity)return false;
            actualDestination[0]=0;
        }
        if(source_->binary08!=0){
            if(found&&value!=expected)return false;
            if(!found&&!expected.empty())return false; //Binary authored-positive aperture.
        } //Text uses actual extracted string or the caller's real "none" default.
        if(!found){value=defaultValue;sourcePC_=0x158B08;}else sourcePC_=fieldCopyPC_;
        // The native capacity is custody only; actual max=-1 means no source
        // truncation. A NULL destination reaches the lookup and then bounds
        // before the actual copy, retaining the caller's allocation prefix.
        if(!actualDestination||capacity<=value.size())return false;
        std::memcpy(actualDestination,value.data(),value.size());actualDestination[value.size()]=0;
        sourceReturned=found;sourcePC_=0x158B14;return true;
    }
    bool readEnvironmentFloat158B80(std::string_view key,std::uint32_t defaultBits,
        std::uint32_t& actualOutputBits,bool& sourceReturned,D& d)noexcept {
        d={};sourceReturned=false;const char* lower=nullptr;std::string_view expected;std::uint32_t exactBits=0;
        if(key=="TerrainWidth"){lower="terrainwidth";expected="5";exactBits=0x40a00000;}
        else if(key=="TerrainBreadth"){lower="terrainbreadth";expected="5";exactBits=0x40a00000;}
        else if(key=="TerrainWidthScale"){lower="terrainwidthscale";expected="24.0";exactBits=0x41c00000;}
        else if(key=="TerrainBreadthScale"){lower="terrainbreadthscale";expected="24.0";exactBits=0x41c00000;}
        else if(key=="TerrainBorderWidth")lower="terrainborderwidth";
        else if(key=="Xmin")lower="xmin";
        else if(key=="Xmax")lower="xmax";
        else if(key=="Zmin")lower="zmin";
        else if(key=="Zmax")lower="zmax";
        else if(key=="EcoDistance2D"&&defaultBits==0x43340000)lower="ecodistance2d";
        else if(key=="EcoDistance3D"&&defaultBits==0x42f00000)lower="ecodistance3d";
        else return false;
        if(key!="EcoDistance2D"&&key!="EcoDistance3D"&&defaultBits!=0)return false;
        std::string_view value;bool found=false;
        if(!findBinaryField158240(true,lower,value,found))return false;
        if(!found){actualOutputBits=defaultBits;sourcePC_=0x158BDC;return true;}
        if(expected.empty()||value!=expected){sourcePC_=0x158BAC;return false;}
        // Authenticated decimal5 and24.0 are exact integers in both original
        // double parse and final float conversion. These bit patterns need no
        // rounding approximation, host atof, or general EE arithmetic policy.
        actualOutputBits=exactBits;sourcePC_=0x158BC4;sourceReturned=true;sourcePC_=0x158BE4;return true;
    }

    // Private exact RA Lights/Light1..185 source selections and raw lookup.
#include "original_world_source_light_config50la.inl"
#include "original_world_source_fog_config50la.inl"
#include "original_world_source_zone_config50la.inl"
#include "original_world_source_portal_config50la.inl"
#include "original_world_source_instance_config50la.inl"
#include "original_world_source_static_config50la.inl"
#include "original_world_source_collision_config50la.inl"
#include "original_world_source_tail_config50la.inl"

    bool close1573B0(D& d)noexcept {
        d={};if(thread_!=std::this_thread::get_id()||!allocated_||nativeClosed_)return false;
        if(sourceClosed_)return true;
        if(!source_){sourceClosed_=true;sourcePC_=0x1574D8;return true;}
        auto& s=*source_;
        // Verify every owned pointer BEFORE handle withdrawal. Text tables
        // belong only to this actual parser; foreign text/child storage is refused.
        if(s.child24!=textChildTable_.get()||s.text1C!=textTable_.sourceStorage||s.cacheCount438||s.block4C4!=block_.get()||
           s.names4CC!=names_.get()||s.index4BC!=index_.get()||
           (textAttempted_&&(s.count14!=textTable_.count14||s.index18!=textTable_.capacity18)))return false;
        if(s.file00!=-1){sourcePC_=0x1573E8;
            if(!file_->close131040(*this,s.file00,true,d))return false;
            sourcePC_=0x157404;s.file00=-1;
        }
        if(textAttempted_){if(!retireTextChild157F90())return false;}
        s.selected20=0;s.child24=nullptr; //157CB0 destructor always clears selected20.
        if(textAttempted_){
            const auto cleanup=Text::cleanupText157414(textTable_);sourcePC_=cleanup.pc;
            syncTextFields();if(cleanup.stop!=Text::Stop::complete)return false;
        }
        s.cacheCount438=0;
        if(s.names4CC){names_.reset();s.names4CC=nullptr;}
        if(s.index4BC){index_.reset();s.index4BC=nullptr;}
        if(s.block4C4){block_.reset();} //1574B8 frees4C4; source does NOT NULL-store it.
        // Actual +1 destructor frees Config backing only after all fields above.
        source_.reset();sourceClosed_=true;sourcePC_=0x1574D8;return true;
    }
    bool abortNative(D& d)noexcept {
        d={};if(thread_!=std::this_thread::get_id())return false;
        if(nativeClosed_)return true;
        if(attemptRetained_&&!file_->abortOccurrenceNative(*this,d))return false;
        attemptRetained_=false;lostBlock_.reset();block_.reset();index_.reset();names_.reset();source_.reset();
        nativeClosed_=true;file_.reset();return true; // no retail destructor receipt
    }
public:
    ~OriginalWorldSourceConfig50LA(){
        // Parent must finish exact native attempt withdrawal before destroying
        // this address, including after a successful retail destructor.
        if(attemptRetained_)std::terminate();
    }
    OriginalWorldSourceConfig50LA(const OriginalWorldSourceConfig50LA&)=delete;
    OriginalWorldSourceConfig50LA& operator=(const OriginalWorldSourceConfig50LA&)=delete;
};
}
