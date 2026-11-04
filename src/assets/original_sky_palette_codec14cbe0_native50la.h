#pragma once
#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <new>
#include <optional>
#include <thread>
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::assets {
//Private, bounded native custody of SLUS_202.68 14CBE0/1051F0/105510/1052B0.
//ELF SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
//This is NOT a general LZW decoder or a source heap allocation receipt.
class OriginalSkyPaletteCodec14CBE0Native50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    //The initialized dictionary interval and distance to the next mapped
    //global are deliberately NARROW admissions, not recovered symbol sizes.
    static constexpr std::size_t dictionaryLimit_=0x88cf,reverseLimit_=0x88d0;
    static constexpr std::size_t inputLimit_=65536,outputLimit_=65536,readLimit_=65536;
    static constexpr std::uint64_t effectLimit_=4u*1024u*1024u;
    struct Entry {
        std::uint32_t marker,prefix;std::uint8_t suffix;
        bool markerKnown=false,prefixKnown=false,suffixKnown=false;
        std::uint64_t prefixWrite=0,suffixWrite=0;
    };
    //No native address is a guest address. A process pointer retains this
    //typed identity and byte position after its native borrow is retired.
    struct InputAlias {
        const void* const owner;const std::uint64_t generation;
        std::atomic<const std::uint8_t*> native;
        std::atomic<bool> pending{false}; //Native source-call custody, independent of the short lock.
        InputAlias(const void* o,std::uint64_t g,const std::uint8_t* p)noexcept:owner(o),generation(g),native(p){}
    };
    struct ReaderGlobals {
        std::optional<std::size_t> wordOffset;
        std::optional<std::uint32_t> cache,consumed;
    };
    struct Process {
        std::atomic_flag busy=ATOMIC_FLAG_INIT;
        std::optional<std::thread::id> thread;
        std::uint64_t generation=0;
        std::shared_ptr<InputAlias> input;
        std::shared_ptr<InputAlias> pendingCall; //Retained after refusal until END or native cancellation.
        ReaderGlobals reader;
        std::optional<std::uint32_t> next42F990,width42F998,mask42F9A0;
        std::array<std::uint8_t,reverseLimit_> reverse4270C0; //Unknown initially.
        std::size_t reverseKnownPrefix=0;
    };
    static Process& process()noexcept {static Process p;return p;}
    struct Claim {
        Process& p;bool held;
        explicit Claim(Process& q)noexcept:p(q),held(!p.busy.test_and_set(std::memory_order_acquire)){}
        ~Claim(){if(held)p.busy.clear(std::memory_order_release);}
    };
    enum class ReadStage:std::uint8_t {begun,added,reduced,maskRead,pointerAdvanced,wordLoaded,cacheStored,returned};
    struct Read {
        std::uint32_t width=0,beforeConsumed=0,beforeCache=0,result=0,loaded=0;
        std::size_t beforePointer=0;
        ReadStage stage=ReadStage::begun;bool longPath=false;
        ReaderGlobals after;
    };
    struct Local {
        std::array<Entry,dictionaryLimit_> dictionary;
        std::array<std::uint64_t,dictionaryLimit_> visit{}; //Native cycle metadata only.
        std::array<Read,readLimit_> reads;
        std::array<std::uint8_t,inputLimit_> inputHistory;
        std::array<std::uint8_t,outputLimit_> outputHistory;
        std::array<std::uint8_t,reverseLimit_> reverseHistory;
    };
    std::unique_ptr<Local> local_;
    std::shared_ptr<InputAlias> alias_;
    const void* inputOwner_=nullptr;
    const std::uint8_t* input_=nullptr;
    std::uint8_t* output_=nullptr;
    const std::thread::id thread_=std::this_thread::get_id();
    std::size_t inputExtent_=0,inputKnownPrefix_=0,outputExtent_=0,initialOutputKnownPrefix_=0;
    std::uint32_t outputBytes_=0,declaredBits_=0;
    bool entered_=false,returned_=false,runAttempted_=false,aliasesRetired_=false;
    bool constructorReturned_=false,destructorEntered_=false,destructorReturned_=false;
    bool staleStackReaderReturn_=false,borrowedInputStored_=false,processInputPublished_=false;
    std::optional<std::uint32_t> readerVtable_,readerFlag20_,readerBorrowed24_,readerByte18_,readerWord1C_;
    std::optional<std::uint64_t> readerDeclared00_;
    bool readerInput10_=false,readerInput14_=false;
    std::uint32_t pc_=0x14cbe0;
    std::uint64_t generation_=0,effects_=0,dictionaryWriteSequence_=0,expansion_=0;
    std::size_t outputKnownPrefix_=0,outputBytesWritten_=0,readCount_=0,reverseKnownPrefix_=0;
    std::uint64_t markerStores_=0,resetAttempts_=0,resetCompletions_=0;
    std::size_t resetIndex_=0;
    std::optional<std::uint32_t> next_,width_,unusedMask_,previous_,current_,firstByte_;
    std::size_t expansionLength_=0,copyRemaining_=0;
    ReaderGlobals reader_;
    bool guardStopped_=false;
    OriginalSkyPaletteCodec14CBE0Native50LA()noexcept=default;
    OriginalSkyPaletteCodec14CBE0Native50LA(const OriginalSkyPaletteCodec14CBE0Native50LA&)=delete;
    OriginalSkyPaletteCodec14CBE0Native50LA& operator=(const OriginalSkyPaletteCodec14CBE0Native50LA&)=delete;
    static bool sameReader(const ReaderGlobals& a,const ReaderGlobals& b)noexcept {
        return a.wordOffset==b.wordOffset&&a.cache==b.cache&&a.consumed==b.consumed;
    }
    static bool disjoint(const void* a,std::size_t an,const void* b,std::size_t bn)noexcept {
        const auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);
        const auto max=std::numeric_limits<std::uintptr_t>::max();
        return an<=max-x&&bn<=max-y&&(x+an<=y||y+bn<=x);
    }
    static std::unique_ptr<OriginalSkyPaletteCodec14CBE0Native50LA> create(
        const void* inputOwner,const std::uint8_t* input,std::size_t inputExtent,std::size_t inputKnownPrefix,
        std::uint8_t* output,std::size_t outputExtent,std::size_t initialOutputKnownPrefix,std::uint32_t outputBytes)noexcept {
        if(!inputOwner||!input||!output||inputExtent!=inputLimit_||inputKnownPrefix>inputExtent||
           outputExtent!=outputBytes||(outputBytes!=0x300u&&outputBytes!=0x8000u&&outputBytes!=0x10000u)||
           initialOutputKnownPrefix>outputExtent||!disjoint(input,inputExtent,output,outputExtent))return {};
        std::unique_ptr<OriginalSkyPaletteCodec14CBE0Native50LA> r(new(std::nothrow) OriginalSkyPaletteCodec14CBE0Native50LA);
        if(!r)return {};r->local_.reset(new(std::nothrow) Local);if(!r->local_)return {};
        r->inputOwner_=inputOwner;r->input_=input;r->output_=output;
        r->inputExtent_=inputExtent;r->inputKnownPrefix_=inputKnownPrefix;r->outputExtent_=outputExtent;
        r->initialOutputKnownPrefix_=r->outputKnownPrefix_=initialOutputKnownPrefix;
        r->outputBytes_=outputBytes;r->declaredBits_=std::uint32_t(outputBytes<<3);
        if(inputKnownPrefix)std::memcpy(r->local_->inputHistory.data(),input,inputKnownPrefix);
        if(initialOutputKnownPrefix)std::memcpy(r->local_->outputHistory.data(),output,initialOutputKnownPrefix);
        return r;
    }
    //Fuel is native refusal at the NEXT source effect, never END or success.
    bool at(std::uint32_t pc)noexcept {
        pc_=pc;if(effects_==effectLimit_){guardStopped_=true;return false;}++effects_;return true;
    }
    bool word(std::size_t offset,std::uint32_t& out)const noexcept {
        if(!input_||!alias_||alias_->native.load(std::memory_order_acquire)!=input_||offset>inputKnownPrefix_||
           inputKnownPrefix_-offset<4||offset>inputExtent_||inputExtent_-offset<4)return false;
        //LW little-endian then the source byte swap gives this big-endian word.
        const auto* p=input_+offset;
        if(std::memcmp(p,local_->inputHistory.data()+offset,4)!=0)return false;
        out=(std::uint32_t(p[0])<<24)|(std::uint32_t(p[1])<<16)|(std::uint32_t(p[2])<<8)|p[3];return true;
    }
    std::uint32_t historyWord(std::size_t offset)const noexcept {
        const auto* p=local_->inputHistory.data()+offset;
        return (std::uint32_t(p[0])<<24)|(std::uint32_t(p[1])<<16)|(std::uint32_t(p[2])<<8)|p[3];
    }
    static std::uint32_t mask(std::uint32_t width)noexcept {
        //Exact33 words at36AD20; callers prove width<=32 before this access.
        return width==32u?0xffffffffu:width?((std::uint32_t(1)<<width)-1u):0u;
    }
    bool constructReader(Process& p)noexcept {
        if(!at(0x1051f8))return false;readerVtable_=0x3cc9c0;
        if(!at(0x105208))return false;readerVtable_=0x3cc9a0;
        if(!at(0x105210))return false;readerDeclared00_=std::uint64_t(declaredBits_);
        if(!at(0x105228))return false;readerInput10_=true;
        if(!at(0x105230))return false;readerFlag20_=0u; //A3=0 in caller delay slot14CC18.
        if(!at(0x105238))return false;readerBorrowed24_=1u;borrowedInputStored_=true;
        if(!at(0x105240))return false;readerInput14_=true;
        if(!at(0x105244))return false;readerByte18_=0x80u;
        if(!at(0x105248))return false;readerWord1C_=0u;
        if(!at(0x105250))return false;p.input=alias_;p.reader.wordOffset=0;reader_.wordOffset=0;processInputPublished_=true;
        if(!at(0x105258))return false;p.reader.consumed=0;reader_.consumed=0;
        std::uint32_t loaded;
        if(!at(0x105260)||!word(0,loaded))return false;
        if(!at(0x1052a0))return false;p.reader.cache=loaded;reader_.cache=loaded;
        constructorReturned_=true;return true;
    }
    bool readBits(Process& p,std::uint32_t width,std::uint32_t& result)noexcept {
        pc_=0x105510;
        if(!reader_.wordOffset||!reader_.cache||!reader_.consumed||readCount_==readLimit_)return false;
        auto& r=local_->reads[readCount_++];r.width=width;r.beforePointer=*reader_.wordOffset;
        r.beforeConsumed=*reader_.consumed;r.beforeCache=*reader_.cache;r.after=reader_;
        const auto save=[&]()noexcept{r.after=reader_;};
        if(!at(0x105520))return false;
        const auto sum=std::uint32_t(r.beforeConsumed+width);p.reader.consumed=sum;reader_.consumed=sum;
        r.stage=ReadStage::added;r.longPath=!(sum<32u);save();
        if(r.longPath){
            //Only reachable admitted widths are9..32 (header32). Preserve
            //the offset reduction BEFORE refusing the out-of-table width33.
            if(!at(0x10554c))return false;p.reader.consumed=sum-32u;reader_.consumed=sum-32u;
            r.stage=ReadStage::reduced;save();
            if(!at(0x10556c)||width>32u)return false;r.stage=ReadStage::maskRead;
            const auto oldPart=(r.beforeCache<<(*reader_.consumed&31u))&mask(width);
            if(!at(0x105580)||r.beforePointer>std::numeric_limits<std::size_t>::max()-4)return false;
            p.reader.wordOffset=r.beforePointer+4;reader_.wordOffset=r.beforePointer+4;
            r.stage=ReadStage::pointerAdvanced;save();
            if(!at(0x105594)||!word(*reader_.wordOffset,r.loaded))return false;r.stage=ReadStage::wordLoaded;
            //The delay-slot SW at1055CC runs even when consumed becomes0.
            if(!at(0x1055cc))return false;p.reader.cache=r.loaded;reader_.cache=r.loaded;
            r.stage=ReadStage::cacheStored;save();
            result=oldPart;if(*reader_.consumed)result|=r.loaded>>((32u-*reader_.consumed)&31u);
        }else {
            if(!at(0x10560c)||width>32u)return false;r.stage=ReadStage::maskRead;
            result=(r.beforeCache>>((32u-sum)&31u))&mask(width);
        }
        r.result=result;r.stage=ReadStage::returned;save();pc_=0x10561c;return true;
    }
    bool resetDictionary(Process& p)noexcept {
        ++resetAttempts_;resetIndex_=0;
        for(;resetIndex_<dictionaryLimit_;++resetIndex_){
            if(!at(0x14cc6c))return false;auto& e=local_->dictionary[resetIndex_];
            e.marker=0xffffffffu;e.markerKnown=true;++markerStores_;
            //Deliberately do not initialize or clear prefix+4/suffix+8.
        }
        if(!at(0x14cc8c))return false;p.next42F990=0x103u;next_=0x103u;
        if(!at(0x14cc98))return false;p.width42F998=9u;width_=9u;
        if(!at(0x14cca4))return false;p.mask42F9A0=0x1ffu;unusedMask_=0x1ffu;
        ++resetCompletions_;return true;
    }
    bool reverseStore(Process& p,std::size_t index,std::uint8_t value,std::uint32_t pc)noexcept {
        if(!at(pc)||index>=reverseLimit_)return false;
        p.reverse4270C0[index]=value;local_->reverseHistory[index]=value;
        p.reverseKnownPrefix=std::max(p.reverseKnownPrefix,index+1);
        reverseKnownPrefix_=std::max(reverseKnownPrefix_,index+1);return true;
    }
    bool outputStore(std::uint8_t value,std::uint32_t pc)noexcept {
        if(!at(pc)||!output_||outputBytesWritten_>=outputExtent_)return false;
        output_[outputBytesWritten_]=value;local_->outputHistory[outputBytesWritten_]=value;
        ++outputBytesWritten_;outputKnownPrefix_=std::max(outputKnownPrefix_,outputBytesWritten_);return true;
    }
    bool expand(Process& p,std::uint32_t code,bool special)noexcept {
        ++expansion_;expansionLength_=0;
        if(special&&(!firstByte_||!reverseStore(p,0,static_cast<std::uint8_t>(*firstByte_),0x14cd80)))return false;
        if(special)expansionLength_=1; //14CD84 follows the possibly-refused14CD80 SB.
        const auto suffixPC=special?0x14cdccu:0x14ce44u;
        const auto storePC=special?0x14cdd4u:0x14ce4cu;
        const auto prefixPC=special?0x14cdd8u:0x14ce50u;
        while(code>=256u){
            if(!at(suffixPC)||code>=dictionaryLimit_||local_->visit[code]==expansion_)return false;
            local_->visit[code]=expansion_;const auto& e=local_->dictionary[code];if(!e.suffixKnown)return false;
            const auto suffix=e.suffix;
            //Source increments length before SB, then reads the prefix only
            //AFTER that scratch store. Unknown prefix preserves that byte.
            const auto index=expansionLength_++;
            if(!reverseStore(p,index,suffix,storePC))return false;
            if(!at(prefixPC)||!e.prefixKnown)return false;code=e.prefix;
        }
        const auto index=expansionLength_++; //ADDIU A1,A2,1 precedes terminal SB.
        if(!reverseStore(p,index,static_cast<std::uint8_t>(code),special?0x14cdfcu:0x14ce70u))return false;
        if(!at(0x14ce80)||!expansionLength_||expansionLength_>reverseKnownPrefix_)return false;
        const auto first=p.reverse4270C0[expansionLength_-1];
        firstByte_=first<128u?std::uint32_t(first):(0xffffff00u|first); //14CE80 signed LB, low32 of S2.
        copyRemaining_=expansionLength_;
        while(copyRemaining_){
            --copyRemaining_; //14CE90, before the source scratch LB and output SB.
            if(!at(0x14ce98)||copyRemaining_>=reverseKnownPrefix_)return false;
            const auto value=p.reverse4270C0[copyRemaining_];
            if(!outputStore(value,0x14ce9c))return false;
        }
        return true;
    }
    bool finishReader()noexcept {
        destructorEntered_=true;
        if(!at(0x1052d4))return false;readerVtable_=0x3cc9a0;
        if(!at(0x1052d8)||readerBorrowed24_!=1u)return false;
        //1052B0(reader,-1): borrowed input is not freed; signed-negative
        //destruction flag does not free the stack receiver. V0 is the now
        //stale stack-reader identity, NOT output length or a success code.
        destructorReturned_=true;staleStackReaderReturn_=true;pc_=0x14cf28;
        returned_=true;return true;
    }
    bool execute(Process& p)noexcept {
        if(!constructReader(p))return false;
        std::uint32_t token;if(!readBits(p,32u,token))return false; //Discarded header eagerly readsinput+4.
        for(;;){
            if(!resetDictionary(p)||!readBits(p,*width_,token))return false;
            previous_=token;current_=token; //14CCB8 precedes the initial END test.
            if(token==0x100u)return finishReader();
            //EVERY initial token other than END emits its low byte, including101/102.
            if(!outputStore(static_cast<std::uint8_t>(token),0x14cce4))return false;firstByte_=token; //14CCF4 whole9-bit S2.
            for(;;){
                if(!readBits(p,*width_,token))return false;current_=token;
                if(token==0x100u)return finishReader();
                if(token==0x102u)break; //Reset retains prefix/suffix and input/output positions.
                if(token==0x101u){
                    if(!at(0x14cd64))return false;p.width42F998=std::uint32_t(*width_+1u);width_=p.width42F998;continue;
                }
                const bool special=token>=*next_; //Source SLTU branch: >=, not canonical equality.
                if(!expand(p,special?*previous_:token,special))return false;
                if(!at(0x14cee4)||*next_>=dictionaryLimit_)return false;
                auto& e=local_->dictionary[*next_];e.prefix=*previous_;e.prefixKnown=true;e.prefixWrite=++dictionaryWriteSequence_;
                if(!at(0x14cee8))return false;e.suffix=static_cast<std::uint8_t>(*firstByte_);e.suffixKnown=true;e.suffixWrite=++dictionaryWriteSequence_;
                previous_=token; //14CEEC, before the next-code increment.
                if(!at(0x14cf04))return false;p.next42F990=std::uint32_t(*next_+1u);next_=p.next42F990;
            }
        }
    }
    bool runOnce(std::uint32_t& pendingPC)noexcept {
        pendingPC=pc_;
        if(runAttempted_||aliasesRetired_||thread_!=std::this_thread::get_id()||!local_||!input_||!output_)return false;
        runAttempted_=true;auto& p=process();Claim claim(p);
        if(!claim.held||(p.thread&&*p.thread!=thread_)||p.generation==std::numeric_limits<std::uint64_t>::max()||
           (p.pendingCall&&p.pendingCall->pending.load(std::memory_order_acquire)))return false;
        if(std::memcmp(input_,local_->inputHistory.data(),inputKnownPrefix_)!=0||
           std::memcmp(output_,local_->outputHistory.data(),initialOutputKnownPrefix_)!=0)return false;
        try{alias_=std::make_shared<InputAlias>(inputOwner_,p.generation+1,input_);}catch(...){return false;}
        generation_=++p.generation;p.thread=thread_;p.pendingCall=alias_;
        alias_->pending.store(true,std::memory_order_release);entered_=true;
        //Snapshot only already-known shared state. No BSS or source-zero fiction.
        reader_=p.reader;next_=p.next42F990;width_=p.width42F998;unusedMask_=p.mask42F9A0;
        reverseKnownPrefix_=p.reverseKnownPrefix;
        if(reverseKnownPrefix_)std::memcpy(local_->reverseHistory.data(),p.reverse4270C0.data(),reverseKnownPrefix_);
        const bool result=execute(p);
        if(result)alias_->pending.store(false,std::memory_order_release); //Only genuine END releases a returned call.
        pendingPC=pc_;return result;
    }
    bool validateReadHistory()const noexcept {
        if(!constructorReturned_)return readCount_==0;
        if(inputKnownPrefix_<4)return false;
        ReaderGlobals expected;expected.wordOffset=0;expected.cache=historyWord(0);expected.consumed=0;
        for(std::size_t i=0;i<readCount_;++i){const auto& r=local_->reads[i];
            if(!expected.wordOffset||!expected.cache||!expected.consumed||r.beforePointer!=*expected.wordOffset||
               r.beforeCache!=*expected.cache||r.beforeConsumed!=*expected.consumed||r.width<9u||r.width>33u||
               (i==0&&r.width!=32u)||(i+1<readCount_&&r.stage!=ReadStage::returned))return false;
            const auto sum=std::uint32_t(r.beforeConsumed+r.width);
            if(r.stage!=ReadStage::begun){expected.consumed=sum;if(r.longPath!=(sum>=32u))return false;}
            if(r.longPath){
                if(r.stage>=ReadStage::reduced)expected.consumed=sum-32u;
                if(r.stage>=ReadStage::maskRead&&r.width>32u)return false;
                if(r.stage>=ReadStage::pointerAdvanced)expected.wordOffset=r.beforePointer+4;
                if(r.stage>=ReadStage::wordLoaded){
                    if(*expected.wordOffset>inputKnownPrefix_||inputKnownPrefix_-*expected.wordOffset<4||
                       r.loaded!=historyWord(*expected.wordOffset))return false;
                }
                if(r.stage>=ReadStage::cacheStored)expected.cache=r.loaded;
                if(r.stage==ReadStage::returned){
                    auto value=(r.beforeCache<<(*expected.consumed&31u))&mask(r.width);
                    if(*expected.consumed)value|=r.loaded>>((32u-*expected.consumed)&31u);
                    if(r.result!=value)return false;
                }
            }else {
                if(r.stage!=ReadStage::begun&&r.stage!=ReadStage::added&&r.stage!=ReadStage::maskRead&&r.stage!=ReadStage::returned)return false;
                if(r.stage==ReadStage::returned&&(r.width>32u||r.result!=((r.beforeCache>>((32u-sum)&31u))&mask(r.width))))return false;
            }
            if(!sameReader(expected,r.after))return false;
        }
        return sameReader(expected,reader_);
    }
    bool validateHistory()const noexcept {
        if(!local_||inputExtent_!=inputLimit_||inputKnownPrefix_>inputExtent_||outputExtent_!=outputBytes_||
           initialOutputKnownPrefix_>outputExtent_||outputBytesWritten_>outputExtent_||
           outputKnownPrefix_!=std::max(initialOutputKnownPrefix_,outputBytesWritten_)||
           declaredBits_!=std::uint32_t(outputBytes_<<3)||readCount_>readLimit_||reverseKnownPrefix_>reverseLimit_||
           effects_>effectLimit_||(guardStopped_&&effects_!=effectLimit_)||
           resetCompletions_>resetAttempts_||resetAttempts_-resetCompletions_>1||resetIndex_>dictionaryLimit_||
           markerStores_!=(resetAttempts_?((resetAttempts_-1)*dictionaryLimit_+resetIndex_):0))return false;
        if(!entered_)return !returned_&&!generation_&&!constructorReturned_&&!readCount_&&!effects_;
        if(!runAttempted_||!generation_||!alias_||alias_->generation!=generation_||alias_->owner!=inputOwner_||
           alias_->pending.load(std::memory_order_acquire)!=(!returned_&&!aliasesRetired_)||
           (readerDeclared00_&&*readerDeclared00_!=declaredBits_)||
           (borrowedInputStored_!=(readerBorrowed24_==1u))||
           (constructorReturned_&&(!readerInput10_||!readerInput14_||readerVtable_!=0x3cc9a0u||
             readerFlag20_!=0u||readerBorrowed24_!=1u||readerByte18_!=0x80u||readerWord1C_!=0u))||
           (returned_&&(!destructorEntered_||!destructorReturned_||!staleStackReaderReturn_||pc_!=0x14cf28u))||
           (!returned_&&(destructorReturned_||staleStackReaderReturn_))||!validateReadHistory())return false;
        std::uint64_t maxWrite=0;
        for(const auto& e:local_->dictionary){
            if((e.markerKnown&&e.marker!=0xffffffffu)||(e.prefixKnown!=(e.prefixWrite!=0))||
               (e.suffixKnown!=(e.suffixWrite!=0))||(e.suffixKnown&&!e.prefixKnown)||
               e.prefixWrite>dictionaryWriteSequence_||e.suffixWrite>dictionaryWriteSequence_)return false;
            maxWrite=std::max(maxWrite,std::max(e.prefixWrite,e.suffixWrite));
        }
        return maxWrite==dictionaryWriteSequence_;
    }
    bool validateCurrent()const noexcept {
        if(!validateHistory())return false;
        if(aliasesRetired_)return !input_&&!output_&&(!alias_||!alias_->native.load(std::memory_order_acquire));
        if(thread_!=std::this_thread::get_id()||!input_||!output_||
           std::memcmp(input_,local_->inputHistory.data(),inputKnownPrefix_)!=0||
           std::memcmp(output_,local_->outputHistory.data(),outputKnownPrefix_)!=0)return false;
        if(!entered_)return true;
        if(alias_->native.load(std::memory_order_acquire)!=input_)return false;
        auto& p=process();Claim claim(p);
        if(!claim.held||!p.thread||*p.thread!=thread_||generation_>p.generation)return false;
        if(generation_<p.generation)return returned_; //A pending source call cannot be superseded.
        if(p.pendingCall!=alias_)return false;
        if(!sameReader(reader_,p.reader)||next_!=p.next42F990||width_!=p.width42F998||unusedMask_!=p.mask42F9A0||
           reverseKnownPrefix_!=p.reverseKnownPrefix||
           (processInputPublished_&&p.input!=alias_))return false;
        return !reverseKnownPrefix_||std::memcmp(local_->reverseHistory.data(),p.reverse4270C0.data(),reverseKnownPrefix_)==0;
    }
    void retireNativeAliases()noexcept {
        if(alias_)alias_->native.store(nullptr,std::memory_order_release);
        input_=nullptr;output_=nullptr;aliasesRetired_=true;
        if(alias_)alias_->pending.store(false,std::memory_order_release); //Native cancellation, never source return.
        //Do not change source pointer identity/offset, cache, consumed bits,
        //controls or reversal bytes. Retirement is NOT source memset/free.
    }
public:
    ~OriginalSkyPaletteCodec14CBE0Native50LA(){retireNativeAliases();}
};
}
