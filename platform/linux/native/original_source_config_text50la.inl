    // Shared actual157500/157D00/1573B0 text ownership. The enclosing private
    // owner supplies exact typed FileManager read/tell/close adapters.
    // These source allocations are distinct from the native parser admission cache.
    struct WidgetsSection1048 {
        std::uint64_t fileOffset00;std::int32_t physicalLines08;
        std::array<char,1024> name0C;
        std::array<std::uint8_t,12> unwritten40C; // Original stride0x418; no fabricated tail initialization.
    };
    struct WidgetsTextSource {
        std::unique_ptr<WidgetsSection1048[]> sections1C,failedReallocSections;
        std::size_t sectionCount14=0,sectionCapacity18=0,retainedOldCapacity=0;
        std::unique_ptr<char*[]> selected24;
        std::unique_ptr<char[]> selectedStorage;
        std::size_t selectedCount20=0,selectedSlotsWritten=0,selectedLinesWritten=0;
        std::array<char,1024> currentName28{};
        bool tableScanComplete=false,selectionComplete=false,selectionAttempted=false,selectedFirstSlotKnown=false;
        bool sourceFileMinusOne=true,closed1573B0=false;
        //157320 writes these. The exact text157500/157D00/getter callgraph
        // never enters binary cache or159090 scratch producers.
        bool binary4CCNull=true,binary4BCNull=true,binary4C4Null=true;
        std::uint32_t scratchCount438=0;
    };
    std::unique_ptr<WidgetsTextSource> widgetsText_;
    static bool widgetsFail(assets::OriginalUiPageDiagnostic50KG& d,std::uint32_t pc,
        assets::OriginalUiPageError50KG error=assets::OriginalUiPageError50KG::unsupportedValue)noexcept {
        d.sourcePC=pc;d.error=error;return false;
    }
    bool widgetsReadLine12EE90(std::array<char,1024>& line,bool& present)noexcept {
        present=false;fileDiagnostic_={};if(!fileCurrent())return false;std::size_t position=0;
        if(!readSourceLine12EE90(line,present,position))return false;
        config_->cursor_=position;return true;
    }
    static bool widgetsClean159440(const std::array<char,1024>& input,std::array<char,1024>& output)noexcept {
        std::size_t out=0;bool quoted=false;std::int32_t brackets=0;
        for(std::size_t n=0;n<input.size()&&input[n];++n){const char c=input[n];
            if(c=='[')++brackets;else if(c==']')--brackets;else if(c=='"')quoted=!quoted;
            const bool whitespace=c==' '||c=='\t'||c=='\r'||c=='\n';
            if(whitespace&&brackets<=0&&!quoted)continue;
            if(out+1>=output.size())return false;output[out++]=c;
        }
        output[out]=0;return true;
    }
    bool scanWidgetsText157500(assets::OriginalUiPageDiagnostic50KG& d)noexcept {
        if(!widgetsText_||!config_||!fileCurrent())return widgetsFail(d,0x157500);
        auto& s=*widgetsText_;if(s.tableScanComplete||s.sections1C||s.sectionCount14)return widgetsFail(d,0x157500);
        // Caller1577D0 already performed actual12F640(0) before text157500.
        std::size_t physicalLine=0;std::array<char,1024> raw{},clean{};
        for(;;){bool present=false;if(!widgetsReadLine12EE90(raw,present))return widgetsFail(d,lastSourceFilePC()?lastSourceFilePC():0x157760);
            if(!present)break;
            if(!widgetsClean159440(raw,clean))return widgetsFail(d,0x157530);
            if(clean[0]=='['){
                std::size_t close=1;while(close<clean.size()&&clean[close]&&clean[close]!=']')++close;
                if(close>=clean.size()||clean[close]!=']')return widgetsFail(d,0x157550);
                clean[close]=0;const char* name=clean.data()+1;
                for(std::size_t i=0;i<s.sectionCount14;++i)
                    if(Provider::sameName(s.sections1C[i].name0C.data(),name))return widgetsFail(d,0x1576C8);
                if(!s.sections1C||s.sectionCount14==s.sectionCapacity18){
                    const bool growing=bool(s.sections1C);
                    if(growing)s.sectionCapacity18+=10; //1575B0 before realloc.
                    const auto capacity=growing?s.sectionCapacity18:10;
                    std::unique_ptr<WidgetsSection1048[]> allocation(new(std::nothrow) WidgetsSection1048[capacity]);
                    if(allocation){if(s.sectionCount14)std::memcpy(allocation.get(),s.sections1C.get(),s.sectionCount14*sizeof(WidgetsSection1048));s.sections1C=std::move(allocation);}
                    else {s.failedReallocSections=std::move(s.sections1C);s.retainedOldCapacity=growing?capacity-10:0;}
                    if(!growing)s.sectionCapacity18=10; //1575F0 after initial NULL publication too.
                    if(!s.sections1C)return widgetsFail(d,0x15761C,assets::OriginalUiPageError50KG::allocation);
                }
                auto& entry=s.sections1C[s.sectionCount14];
                std::memcpy(entry.name0C.data(),name,close); // Original strcpy0C including NUL.
                std::size_t actualPosition=0;
                if(!tellSource12F520(actualPosition))return widgetsFail(d,lastSourceFilePC());
                entry.fileOffset00=actualPosition; //12F520 actual position after the header read.
                entry.physicalLines08=static_cast<std::int32_t>(physicalLine);
                if(s.sectionCount14){auto& previous=s.sections1C[s.sectionCount14-1];
                    previous.physicalLines08=static_cast<std::int32_t>(physicalLine)-previous.physicalLines08-1;}
                ++s.sectionCount14; //1576C4 after actual fields.
            }
            ++physicalLine;
        }
        if(s.sectionCount14){auto& last=s.sections1C[s.sectionCount14-1];
            last.physicalLines08=static_cast<std::int32_t>(physicalLine)-last.physicalLines08-1;}
        s.tableScanComplete=true;return true;
    }
    bool selectedCurrent(std::string_view name)const noexcept {
        if(!widgetsText_||!parsedCurrent()||widgetsText_->closed1573B0)return false;
        const auto& s=*widgetsText_;
        return s.tableScanComplete&&s.selectionComplete&&Provider::sameName(s.currentName28.data(),name)&&
            s.selected24&&s.selectedStorage&&s.selectedFirstSlotKnown&&s.selectedSlotsWritten==s.selectedCount20&&
            s.selectedLinesWritten==s.selectedCount20;
    }
    bool selectWidgetsSection157D00(std::string_view name,assets::OriginalUiPageDiagnostic50KG& d)noexcept {
        d={};if(!widgetsText_||!parsedCurrent()||!widgetsText_->tableScanComplete||widgetsText_->closed1573B0)
            return widgetsFail(d,0x157D00);
        auto& s=*widgetsText_;if(s.selectionAttempted&&!s.selectionComplete)return widgetsFail(d,0x157D00);
        const WidgetsSection1048* entry=nullptr;
        for(std::size_t n=0;n<s.sectionCount14;++n)if(Provider::sameName(s.sections1C[n].name0C.data(),name)){entry=&s.sections1C[n];break;}
        if(!entry)return widgetsFail(d,0x157DD8);
        if(entry->fileOffset00>config_->member_.size||entry->physicalLines08<0||name.size()>=1024)return widgetsFail(d,0x157F88);
        if(!seek(std::size_t(entry->fileOffset00)))return widgetsFail(d,lastSourceFilePC()); // Actual seek before cache release.
        s.selectionComplete=false;s.selectionAttempted=true;
        if(s.selected24){s.selectedStorage.reset();s.selected24.reset();s.selectedCount20=0;}
        s.selectedSlotsWritten=s.selectedLinesWritten=0;s.selectedFirstSlotKnown=false;
        std::memcpy(s.currentName28.data(),name.data(),name.size());s.currentName28[name.size()]=0;
        std::array<char,1024> raw{},clean{};
        for(std::int32_t n=0;n<entry->physicalLines08;++n){bool present=false;
            if(!widgetsReadLine12EE90(raw,present)||!present)return widgetsFail(d,lastSourceFilePC()?lastSourceFilePC():0x1580B4);
            if(!widgetsClean159440(raw,clean))return widgetsFail(d,0x1580C4);
            if(clean[0]&&clean[0]!=';')++s.selectedCount20; //1580EC per reached nonempty/noncomment line.
        }
        if(!seek(std::size_t(entry->fileOffset00)))return widgetsFail(d,lastSourceFilePC()); //158130 actual second seek.
        s.selected24.reset(new(std::nothrow) char*[s.selectedCount20]); //15814C NULL included.
        s.selectedStorage.reset(new(std::nothrow) char[s.selectedCount20*1024]); //158154 even after24NULL.
        if(!s.selected24)return widgetsFail(d,0x158168,assets::OriginalUiPageError50KG::allocation);
        if(!s.selectedCount20)return widgetsFail(d,0x158168); // malloc0 pointer store is deliberately not admitted.
        s.selected24[0]=s.selectedStorage.get();s.selectedFirstSlotKnown=true;s.selectedSlotsWritten=1;
        if(!s.selectedStorage&&s.selectedCount20>1)return widgetsFail(d,0x158180,assets::OriginalUiPageError50KG::allocation);
        for(std::size_t n=1;n<s.selectedCount20;++n){s.selected24[n]=s.selected24[n-1]+1024;++s.selectedSlotsWritten;}
        for(std::int32_t n=0;n<entry->physicalLines08;++n){bool present=false;
            if(!widgetsReadLine12EE90(raw,present)||!present)return widgetsFail(d,lastSourceFilePC()?lastSourceFilePC():0x1581B4);
            if(!widgetsClean159440(raw,clean))return widgetsFail(d,0x1581C4);
            if(clean[0]&&clean[0]!=';'){
                if(s.selectedLinesWritten>=s.selectedCount20||!s.selected24[s.selectedLinesWritten])
                    return widgetsFail(d,0x1581F4,assets::OriginalUiPageError50KG::allocation);
                std::memcpy(s.selected24[s.selectedLinesWritten],clean.data(),std::strlen(clean.data())+1);++s.selectedLinesWritten;
            }
        }
        s.selectionComplete=true;return true;
    }
    bool retireConfig1573B0(std::int32_t deletingArgument,assets::OriginalUiPageDiagnostic50KG& d)noexcept {
        d={};if(thread_!=std::this_thread::get_id()||!widgetsText_||
           (automaticStorage_?deletingArgument!=-1:deletingArgument!=1))return widgetsFail(d,0x1573B0);
        auto& s=*widgetsText_;if(s.closed1573B0)return true;
        if(status_==Status::empty||status_==Status::mounted)return widgetsFail(d,0x1573B0);
        if(!config_)return widgetsFail(d,0x1573B0);
        if(config_->sourceFile00!=-1){
            if(!resolver_||!closeSource131040(true))
                return widgetsFail(d,lastSourceFilePC());
            config_->sourceFile00=-1;config_->fileOpened_=false;config_->cursor_=0;s.sourceFileMinusOne=true;
        }
        //157CB0 dereferences the first pointer only when24 is non-NULL. A
        // failed pointer-array construction cannot invent that first slot.
        if(s.selected24){if(!s.selectedFirstSlotKnown)return widgetsFail(d,0x157CB0);
            if(s.selected24[0])s.selectedStorage.reset();s.selected24.reset();}
        s.selectedCount20=0;s.selectionComplete=false;s.selectedSlotsWritten=s.selectedLinesWritten=0;s.selectedFirstSlotKnown=false;
        if(s.sections1C){s.sections1C.reset();s.sectionCount14=0;}
        // Every reached getter in this exact text cohort uses local parsing
        // buffers.159090 scratch allocation and binary branches were not called.
        if(s.scratchCount438||!s.binary4CCNull||!s.binary4BCNull||!s.binary4C4Null)return widgetsFail(d,0x157418);
        s.scratchCount438=0; //157478 reached even when loop count was zero.
        s.closed1573B0=true;status_=Status::sourceClosed;
        if(deletingArgument>0)config_.reset(); //1574D0→2E8B00 actual heap object, after its owned children.
        return true; // Signed-1 leaves actual automatic storage.
    }
    bool closeWidgetsStack1573B0(std::int32_t arg,assets::OriginalUiPageDiagnostic50KG& d)noexcept {
        if(!automaticStorage_)return widgetsFail(d,0x1573B0);return retireConfig1573B0(arg,d);
    }
    bool closeHeap1573B0(std::int32_t arg,assets::OriginalUiPageDiagnostic50KG& d)noexcept {
        if(automaticStorage_)return widgetsFail(d,0x1573B0);return retireConfig1573B0(arg,d);
    }
    bool closedWidgetsStack1573B0()const noexcept {
        return automaticStorage_&&widgetsText_&&widgetsText_->closed1573B0&&config_&&config_->sourceFile00==-1;
    }
