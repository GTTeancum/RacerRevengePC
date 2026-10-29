#pragma once
#include "../../../src/assets/original_standard_archive_provider50la.h"
#include "../../../src/assets/original_standard_texture_resource50la.h"
#include "../../../src/assets/original_ui_pages50kg.h"
#include <algorithm>
#include <array>
#include <new>
#include <cstring>
#include <exception>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
namespace rr::platform {
class OriginalUiCourseOwnedLifecycle50KG;
// Actual Course manager298 Config157320 native file/buffer/parser custody.
// This is not an Entity and cannot issue a manager/page/loader receipt.
class OriginalUiCourseManagerConfig50KG final {
    friend class OriginalUiCourseOwnedLifecycle50KG;
    using Provider=assets::OriginalStandardArchiveProvider50LA;
    using Archive=assets::OriginalUiArchive50KG;
    using Resolver=assets::OriginalStandardTextureResolver50LA;
    std::shared_ptr<Resolver> resolver_;
    const OriginalUiCourseManagerConfig50KG* bankRoot_=nullptr;
    bool bankBorrowed_=false,bankOwned_=false,automaticStorage_=false;
    assets::OriginalStandardTextureDiagnostic50LA fileDiagnostic_{};
    std::uint32_t lastSourceFilePC()const noexcept{return fileDiagnostic_.sourcePC;}
    enum class Status {empty,mounted,constructed,opened,parsed,sourceClosed,closed};
    enum class OpenResult {opened,absent,unsupported,stale,allocation};
    const std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<Provider> provider_;
    std::shared_ptr<const Archive> archive_;
    // Native mount custody exists before retail Config allocation. Only this
    // separate allocation at1CCC70 supplies the actual source Config object.
    struct Config157320 final {
        Provider::MemberView member_{};
        assets::OriginalUiMemberView50KG authenticated_{};
        std::string requested_;
        std::vector<std::uint8_t> buffer_;
        assets::OriginalUiDat50KG records_;
        std::size_t cursor_=0;
        bool fileOpened_=false;
        std::int32_t sourceFile00=-1;
    };
    std::unique_ptr<Config157320> config_;
    std::size_t mount_=static_cast<std::size_t>(-1);
    std::uint64_t mountedRevision_=0;
    bool mountWasAlreadyActive_=false,archiveOnlyScope170_=false;
    Status status_=Status::empty;
    OriginalUiCourseManagerConfig50KG()noexcept=default;
    bool fileCurrent()const noexcept {
        if(thread_!=std::this_thread::get_id()||!config_||!config_->fileOpened_||!provider_||!archive_||!resolver_||
           !resolver_->courseConfigFileCurrent(bankRoot_,this,config_->sourceFile00)||
           !archive_->loaded()||!config_->member_.member||!config_->member_.data||config_->cursor_>config_->member_.size||
           !config_->authenticated_.identity||config_->authenticated_.size!=config_->member_.size)return false;
        Provider::MemberView now;
        return provider_->lookup132820(config_->requested_,now)==Provider::Lookup::present&&
            now.member==config_->member_.member&&now.data==config_->member_.data&&now.size==config_->member_.size&&
            now.mount==config_->member_.mount&&now.ordinal==config_->member_.ordinal&&now.bankGeneration==config_->member_.bankGeneration;
    }
    bool read(void* output,std::size_t count,std::size_t& done)noexcept {
        done=0;if(!fileCurrent())return false;
        if(!resolver_->readCourseConfig12FB30(bankRoot_,this,config_->sourceFile00,output,count,done,fileDiagnostic_))return false;
        config_->cursor_+=done;return true;
    }
    bool seek(std::size_t position)noexcept {
        if(!fileCurrent()||!resolver_->seekCourseConfig12F640(bankRoot_,this,config_->sourceFile00,position,fileDiagnostic_))return false;
        config_->cursor_=position;return true;
    }
    // The reached native archive call validates and mounts the WHOLE archive.
    // An earlier active namespace entry is borrowed, never claimed as a new
    // source12CEA0 acquisition or authority to remove other users' bank.
    bool mountCourse12CEA0(std::shared_ptr<Provider> provider,std::shared_ptr<Resolver> resolver,
        std::shared_ptr<const Archive> archive,Provider::Diagnostic& d,assets::OriginalStandardTextureDiagnostic50LA& file)noexcept {
        d={};file={};if(thread_!=std::this_thread::get_id()||status_!=Status::empty||!provider||!resolver||!provider->sealed()||
           !archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course))return false;
        // Native wrapper capacity is admitted before any source12CEA0 effect.
        try {widgetsText_=std::make_unique<WidgetsTextSource>();}catch(...){return false;}
        const auto* identity=archive->identity();
        for(std::size_t i=0;i<provider->mountCount();++i){Provider::MountView view;
            if(provider->mount(i,view)&&view.archiveName&&Provider::sameName(*view.archiveName,identity->discPath)){
                if(mount_!=static_cast<std::size_t>(-1))return false;
                mount_=i;mountWasAlreadyActive_=true;
            }
        }
        provider_=std::move(provider);archive_=std::move(archive);resolver_=std::move(resolver);bankRoot_=this;
        status_=Status::mounted;
        if(!resolver_->registerCourseBank12CEA0(archive_,provider_.get(),this,bankOwned_,fileDiagnostic_)){file=fileDiagnostic_;return false;}
        if(!provider_->mountAuthenticatedUi(archive_,d))return false;
        if(!resolver_->adoptCourseNativeMount(this,!mountWasAlreadyActive_,fileDiagnostic_)){file=fileDiagnostic_;return false;}
        status_=Status::mounted;mountedRevision_=provider_->revision();
        // Re-seal exactly this previously complete native namespace plus the
        // authenticated complete Course directory. Not retail mount priority.
        if(!provider_->sealNativeMounts(d))return false;
        std::size_t matched=static_cast<std::size_t>(-1);
        for(std::size_t i=0;i<provider_->mountCount();++i){Provider::MountView view;
            if(provider_->mount(i,view)&&view.archiveName&&Provider::sameName(*view.archiveName,identity->discPath)){
                if(matched!=static_cast<std::size_t>(-1)||!view.archiveSha256||
                   *view.archiveSha256!=identity->sha256||view.archiveBytes!=identity->bytes||
                   view.memberCount!=identity->memberCount)return false;
                matched=i;
            }
        }
        if(matched==static_cast<std::size_t>(-1)||(mountWasAlreadyActive_&&matched!=mount_))return false;
        mount_=matched;
        if(!resolver_->joinCourseBankCanonical(this,!mountWasAlreadyActive_,fileDiagnostic_)){file=fileDiagnostic_;return false;}
        file=fileDiagnostic_;return true;
    }
    bool enterArchiveOnly132A20()noexcept {
        if(thread_!=std::this_thread::get_id()||status_!=Status::mounted||archiveOnlyScope170_||
           !provider_||!provider_->sealed()||!archive_)return false;
        //132A20(1) stores source170=0. This owner's concrete Config below
        // consequently admits archive-provider rows only; no host-file fallback
        // or generic lookup-mode waiver is installed by this scoped authority.
        if(!resolver_||!resolver_->enterCourseArchiveOnly132A20(bankRoot_,fileDiagnostic_))return false;
        archiveOnlyScope170_=true;return true;
    }
    bool admitBorrowedHeapTransportNative(const OriginalUiCourseManagerConfig50KG& parent)noexcept {
        if(thread_!=std::this_thread::get_id()||status_!=Status::empty||config_||
           !parent.parsedCurrent()||!parent.archiveOnlyScope170_||!parent.provider_->sealed())return false;
        // Native lease of an ALREADY owned Course mount, not another12CEA0 or
        // an actual Config constructor. The reached malloc4D0 still occurs only
        // in allocateAndConstruct157320; NULL then remains sourceNULL.
        if(!widgetsText_){try{widgetsText_=std::make_unique<WidgetsTextSource>();}catch(...){return false;}}
        resolver_=parent.resolver_;bankRoot_=parent.bankRoot_;
        if(!resolver_||!resolver_->borrowCourseConfigNative(bankRoot_,true,fileDiagnostic_))return false;
        bankBorrowed_=true;provider_=parent.provider_;archive_=parent.archive_;mount_=parent.mount_;
        mountedRevision_=parent.mountedRevision_;archiveOnlyScope170_=true;
        mountWasAlreadyActive_=true;status_=Status::mounted;return true;
    }
    bool admitStackConfigNative(const OriginalUiCourseManagerConfig50KG& parent)noexcept {
        if(thread_!=std::this_thread::get_id()||status_!=Status::empty||config_||
           !parent.parsedCurrent()||!parent.archiveOnlyScope170_||!parent.provider_->sealed())return false;
        // Storage for an actual automatic Config157320 is native admission,
        // not a source malloc. Existing source bank/archive-only mode borrowed.
        try {config_=std::make_unique<Config157320>();widgetsText_=std::make_unique<WidgetsTextSource>();}
        catch(...){config_.reset();widgetsText_.reset();return false;}
        if(!widgetsText_){try{widgetsText_=std::make_unique<WidgetsTextSource>();}catch(...){return false;}}
        resolver_=parent.resolver_;bankRoot_=parent.bankRoot_;
        if(!resolver_||!resolver_->borrowCourseConfigNative(bankRoot_,true,fileDiagnostic_))return false;
        bankBorrowed_=true;provider_=parent.provider_;archive_=parent.archive_;mount_=parent.mount_;
        mountedRevision_=parent.mountedRevision_;archiveOnlyScope170_=true;automaticStorage_=true;
        return true;
    }
    bool constructAdmittedStack157320()noexcept {
        if(thread_!=std::this_thread::get_id()||status_!=Status::empty||!config_||!provider_||!archive_||
           !archiveOnlyScope170_)return false;
        status_=Status::constructed;return true; // Actual1CD7DC automatic Config constructor.
    }
    bool allocateAndConstruct157320()noexcept {
        if(status_!=Status::mounted||!archiveOnlyScope170_||config_)std::terminate();
        try {config_=std::make_unique<Config157320>();}
        catch(...){return false;} //2E8D90 NULL: skip157320, still publish298.
        status_=Status::constructed;return true;
    }
    OpenResult open1577D0(const std::string& name,assets::OriginalUiPageDiagnostic50KG& d)noexcept {
        d={};if(thread_!=std::this_thread::get_id()||status_!=Status::constructed||!archiveOnlyScope170_||!provider_||!archive_||!resolver_||!config_||!widgetsText_)
            return OpenResult::stale;
        Provider::MemberView found;const auto result=provider_->lookup132820(name,found);
        if(result!=Provider::Lookup::present&&result!=Provider::Lookup::absent)return OpenResult::stale;
        // Read-only native parsing/capacity admission precedes source131230.
        // It neither advances the FileManager cursor nor replaces157500 reads.
        if(result==Provider::Lookup::present){
            const auto authenticated=archive_->find(name);
            if(found.mount!=mount_||!authenticated.identity||!authenticated.data||!found.data||authenticated.size!=found.size||
               found.size>assets::originalUiDatByteLimit50KG||std::memcmp(authenticated.data,found.data,found.size))return OpenResult::unsupported;
            try {config_->requested_=name;config_->member_=found;config_->authenticated_=authenticated;
                config_->buffer_.assign(found.data,found.data+found.size);
                const assets::OriginalUiMemberView50KG owned{authenticated.identity,config_->buffer_.data(),config_->buffer_.size()};
                if(!assets::OriginalUiDat50KG::parseRetainedMember(owned,config_->records_,d,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)){d.sourcePC=0;return OpenResult::unsupported;}
            }catch(...){return OpenResult::allocation;}
        }
        if(!resolver_->openCourseConfig131230(bankRoot_,this,name,config_->sourceFile00,fileDiagnostic_)){
            d.sourcePC=fileDiagnostic_.sourcePC;
            return result==Provider::Lookup::absent&&config_->sourceFile00==-1&&fileDiagnostic_.sourcePC==0x1315B0&&fileDiagnostic_.status==assets::OriginalStandardTextureStatus50LA::unresolved?OpenResult::absent:OpenResult::stale;
        }
        config_->fileOpened_=true;config_->cursor_=0;status_=Status::opened;widgetsText_->sourceFileMinusOne=false;
        if(result!=Provider::Lookup::present)return OpenResult::stale;
        std::uint8_t signature[7]{};std::size_t count=0;
        if(!read(signature,sizeof(signature),count)){d.sourcePC=lastSourceFilePC();return OpenResult::stale;}
        signature[6]=0; //1577D0 overwrites the seventh probe byte before comparison.
        constexpr std::uint8_t binary[7]={'B','I','N','A','R','Y',0};
        if(count==sizeof(binary)&&!std::memcmp(signature,binary,sizeof(binary)))return OpenResult::unsupported;
        if(!seek(0)){d.sourcePC=lastSourceFilePC();return OpenResult::stale;}
        if(!scanWidgetsText157500(d))return d.error==assets::OriginalUiPageError50KG::allocation?OpenResult::allocation:OpenResult::unsupported;
        status_=Status::parsed;return OpenResult::opened;
    }
    bool parsedCurrent()const noexcept {return status_==Status::parsed&&fileCurrent();}
    bool readSourceLine12EE90(std::array<char,1024>& line,bool& present,std::size_t& position)noexcept {
        return resolver_->readCourseConfigLine12EE90(bankRoot_,this,config_->sourceFile00,line,present,position,fileDiagnostic_);
    }
    bool tellSource12F520(std::size_t& position)noexcept {
        return resolver_->tellCourseConfig12F520(bankRoot_,this,config_->sourceFile00,position,fileDiagnostic_);
    }
    bool closeSource131040(bool flush)noexcept {
        return resolver_->closeCourseConfig131040(bankRoot_,this,config_->sourceFile00,flush,fileDiagnostic_);
    }
#include "original_source_config_text50la.inl"
    bool closeNative()noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(status_==Status::closed)return true;
        // Native abort of an authentic partial Config, never a1573B0 receipt.
        // Close the actual member before dropping any backing or bank borrow.
        if(config_&&config_->sourceFile00!=-1){
            if(!resolver_||!resolver_->closeCourseConfig131040(bankRoot_,this,config_->sourceFile00,false,fileDiagnostic_))return false;
            config_->sourceFile00=-1;config_->fileOpened_=false;config_->cursor_=0;
        }
        if(bankOwned_&&resolver_&&!resolver_->retireCourseBankNative(this,fileDiagnostic_))return false;
        if(bankBorrowed_&&resolver_&&!resolver_->borrowCourseConfigNative(bankRoot_,false,fileDiagnostic_))return false;
        bankOwned_=bankBorrowed_=false;
        config_.reset();widgetsText_.reset(); // Includes unpublished old backing after source reallocNULL.
        archive_.reset();provider_.reset();resolver_.reset();bankRoot_=nullptr;
        archiveOnlyScope170_=false;status_=Status::closed;return true;
    }

public:
    ~OriginalUiCourseManagerConfig50KG()noexcept {if(bankOwned_||bankBorrowed_||(status_!=Status::closed&&status_!=Status::empty))std::terminate();}
    OriginalUiCourseManagerConfig50KG(const OriginalUiCourseManagerConfig50KG&)=delete;
    OriginalUiCourseManagerConfig50KG& operator=(const OriginalUiCourseManagerConfig50KG&)=delete;
};
}
