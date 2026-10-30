#pragma once
#include "original_standard_texture_resource50la.h"
#include "original_standard_archive_provider50la.h"
#include "../game/original_text_config_primitives50la.h"
#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <cstring>
#include <thread>
#include <exception>
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::assets {
// Fresh stack Config at1617A0. No World-derived allocation, shared numeric
// handle, predecoded PRT, or public path/root receipt. Cold World alone lends
// its genuine same-App source invocation; no issuer is installed here.
class OriginalParticleSourceConfig50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    using D=OriginalStandardTextureDiagnostic50LA;
    using Text=rr::game::OriginalTextConfigPrimitives50LA;
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
    Source storage_;Source* source_=&storage_;
    const std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider_;
    const void* selectedRoot_=nullptr;
    std::int32_t handle_=-1;
    std::string filename_;
    bool attempted_=false,borrowed_=false,nativeClosed_=false;
    bool sourceDestructorAttempted_=false,sourceClosed_=false;
    std::optional<bool> sourceOpenReturned_;
    std::uint32_t sourcePC_=0;
    struct Line {std::array<char,1024> bytes;std::array<bool,1024> known{};bool present=false;std::size_t position=0;};
    Text::Table textTable_;Line textInput_;
    Text::Bytes textBytes_,textLine_,textName_;
    bool textAttempted_=false,textReturned_=false;
    std::uint32_t textLineOrdinal_=0;
    OriginalParticleSourceConfig50LA()noexcept=default;
    bool begin1617A0(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalStandardArchiveProvider50LA>,const std::string&,D&)noexcept;
    bool current()const noexcept;
    bool readLine(Line&,D&)noexcept;
    bool tell(std::int64_t&,D&)noexcept;
    bool seek(std::size_t,D&)noexcept;
    bool abortNative(D&)noexcept;
    bool closeStack1573B0(D&)noexcept;
    bool missingOpenCurrent()const noexcept;
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
           source_->text1C||!current())return false;
        textAttempted_=true;textLineOrdinal_=0;
        for(;;){
            sourcePC_=0x157760u;
            if(!readLine(textInput_,d))return false;
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
                std::int64_t tellValue=0;sourcePC_=0x15762Cu;
                if(!tell(tellValue,d))return false;
                step=Text::finishRow157654(textTable_,static_cast<std::uint64_t>(tellValue),textLineOrdinal_);
                syncTextFields();sourcePC_=step.pc;
                if(step.stop!=Text::Stop::complete)return false;
            }
            //157544 branch delay for nonheaders,157748 for successful headers.
            ++textLineOrdinal_;
        }
    }

#include "original_particle_source_section50la.inl"
#include "original_particle_source_field50la.inl"
#include "original_particle_source_integer50la.inl"

public:
    ~OriginalParticleSourceConfig50LA(){if(borrowed_)std::terminate();}
    OriginalParticleSourceConfig50LA(const OriginalParticleSourceConfig50LA&)=delete;
    OriginalParticleSourceConfig50LA& operator=(const OriginalParticleSourceConfig50LA&)=delete;
};
}
