//Private isolated source-result facet in the EXISTING typed result owner.
//No semantic Shape/type2 count, loaded_ or prepared query state is adopted.
    enum class SourceResultStatus {owner,bounded,reached};
    struct SourceResultOutcome {SourceResultStatus status;std::uint32_t sourcePC;};
    struct SourceResultArray {
        std::unique_ptr<std::uint8_t[]> raw;
        std::optional<const void*> payload;
        std::size_t bytes=0;
        std::uint32_t vectors=0;
        bool attempted=false,cookieStored=false,constructorsReturned=false,published=false;
    };
    struct SourceResult {
        bool attempted=false,shapeRead=false,shape54Stored=false;
        bool outerAttempted=false,outerReturned=false,countRead=false,countStored=false,admitted=false;
        bool published=false,returned=false,nativeWithdrawn=false;
        const void* shapeIdentity=nullptr;
        const void* outerIdentity=nullptr;
        std::uint32_t rawArgument=0,count=0,pc=0;
        std::uint8_t vectorMask=0;
        std::unique_ptr<std::array<std::uint8_t,0x80>> raw;
        std::array<SourceResultArray,7> arrays;
        std::optional<const void*> returnedPayload; //Actual final2E8990 return; never bool1.
    } sourceResult_;
    static void storeResultWord(std::uint8_t* out,std::uint32_t value)noexcept {
        for(unsigned b=0;b<4;++b)out[b]=std::uint8_t(value>>(8u*b));
    }
    static bool resultWordEquals(const std::uint8_t* p,std::uint32_t value)noexcept {
        for(unsigned b=0;b<4;++b)if(p[b]!=std::uint8_t(value>>(8u*b)))return false;
        return true;
    }
    static void constructResultVector1048E0(std::uint8_t* p)noexcept {
        storeResultWord(p,0);storeResultWord(p+4,0);storeResultWord(p+8,0);storeResultWord(p+12,0x3f800000u);
    }
    static bool resultVectorCurrent(const std::uint8_t* p)noexcept {
        return resultWordEquals(p,0)&&resultWordEquals(p+4,0)&&resultWordEquals(p+8,0)&&resultWordEquals(p+12,0x3f800000u);
    }
    bool allocateSourceResultArray(unsigned index,std::uint32_t callPC,std::uint32_t storePC)noexcept {
        auto& r=sourceResult_;auto& a=r.arrays[index];r.pc=callPC;
        const bool vector=index>=4;
        a.vectors=vector?(index==4?2u*r.count:r.count):0u;
        a.bytes=vector?std::size_t(a.vectors)*16u+16u:std::size_t(r.count)*(index==3?2u:4u);
        a.attempted=true;
        try{a.raw=std::unique_ptr<std::uint8_t[]>(new std::uint8_t[a.bytes]);}catch(...){a.raw.reset();}
        if(vector&&a.raw){
            storeResultWord(a.raw.get(),16u);storeResultWord(a.raw.get()+4,a.vectors);a.cookieStored=true;
            //Cookie8..15 remains unknown;1048E0 writes all16 payload bytes.
            for(std::uint32_t i=0;i<a.vectors;++i)constructResultVector1048E0(a.raw.get()+16u+std::size_t(i)*16u);
            a.constructorsReturned=true;
        }
        a.payload=a.raw?static_cast<const void*>(a.raw.get()+(vector?16u:0u)):nullptr;
        r.pc=storePC;a.published=true;return true;
    }
    template<class StoreShape,class ReadCount,class Publish>
    SourceResultOutcome allocateSourceResult11BBE8(const void* exactShape,std::uint32_t actualArgument,
        StoreShape&& storeShape,ReadCount&& readCount,Publish&& publish)noexcept {
        auto& r=sourceResult_;
        if(r.attempted||attempted_||loaded_||retired_||shape8C_||result90_||!exactShape)
            return {SourceResultStatus::owner,0x11bbe0};
        //COL has installed the exact same occurrence borrower before this call.
        r.attempted=true;r.pc=0x11bbe0;r.rawArgument=actualArgument;
        r.shapeIdentity=exactShape;r.shapeRead=true;
        r.pc=0x11bbec;
        if(!storeShape(r.shapeIdentity,actualArgument))return {SourceResultStatus::bounded,r.pc};
        r.shape54Stored=true;r.pc=0x11bbe8;r.outerAttempted=true;
        try{r.raw=std::unique_ptr<std::array<std::uint8_t,0x80>>(new std::array<std::uint8_t,0x80>);}catch(...){r.raw.reset();}
        r.outerReturned=true;r.outerIdentity=r.raw.get();
        if(r.raw){
            r.pc=0x11bc10;
            for(unsigned i=0;i<2;++i){constructResultVector1048E0(r.raw->data()+16u*i);r.vectorMask|=std::uint8_t(1u<<i);}
            static constexpr std::uint32_t stores[3]={0x11bc1c,0x11bc24,0x11bc2c};
            for(unsigned i=2;i<5;++i){r.pc=stores[i-2];constructResultVector1048E0(r.raw->data()+16u*i);r.vectorMask|=std::uint8_t(1u<<i);}
        }
        r.pc=0x11bc3c;
        if(!readCount(r.shapeIdentity,r.count))return {SourceResultStatus::bounded,r.pc};
        r.countRead=true;r.pc=0x11bc40;
        if(!r.raw)return {SourceResultStatus::bounded,r.pc}; //NULL outer first use AFTER shape48 read.
        storeResultWord(r.raw->data()+0x5c,r.count);r.countStored=true;r.pc=0x11bc48;
        //Existing native collision allocation ceiling65535, NOT source vertex
        //semantics or a source branch. Retain count publication before refusal.
        if(r.count>65535u||r.count>(std::numeric_limits<std::uint32_t>::max()-16u)/32u)
            return {SourceResultStatus::bounded,r.pc};
        r.admitted=true;
        static constexpr std::uint32_t calls[7]={0x11bc48,0x11bc58,0x11bc68,0x11bc78,0x11bc90,0x11bcc0,0x11bcf0};
        static constexpr std::uint32_t stores[7]={0x11bc50,0x11bc60,0x11bc70,0x11bc80,0x11bcb4,0x11bce4,0x11bd14};
        for(unsigned i=0;i<7;++i)allocateSourceResultArray(i,calls[i],stores[i]);
        r.returnedPayload=r.arrays[6].payload;r.pc=0x11bd18;
        if(!publish(r.raw.get()))return {SourceResultStatus::bounded,r.pc};
        r.published=true;r.returned=true;r.pc=0x11bd30;return {SourceResultStatus::reached,r.pc};
    }
    bool preflightSourceResultNative()const noexcept {
        const auto& r=sourceResult_;
        if(!r.attempted)return !r.raw&&!r.shapeIdentity&&!r.outerIdentity&&!r.published&&!r.returned&&!r.nativeWithdrawn;
        if(attempted_||loaded_||retired_||shape8C_||result90_||!r.shapeRead||!r.shapeIdentity||
           (r.returned!=r.published)||(r.published&&!r.returnedPayload))return false;
        if(r.nativeWithdrawn){
            if(r.raw)return false;
            for(const auto& a:r.arrays)if(a.raw)return false;
            return true; //Opaque alias/PC/history only; never inspect retired bytes.
        }
        if(r.outerAttempted!=r.outerReturned||r.outerIdentity!=r.raw.get())return false;
        if(!r.shape54Stored)return !r.outerAttempted&&!r.raw&&!r.vectorMask&&!r.countRead&&!r.countStored&&r.pc==0x11bbec;
        if(!r.outerReturned)return false;
        if(r.raw){
            if(r.vectorMask!=31u)return false;
            for(unsigned i=0;i<5;++i)if(!resultVectorCurrent(r.raw->data()+16u*i))return false;
        }else if(r.vectorMask||r.countStored||r.admitted||r.published)return false;
        if(r.countStored&&(!r.countRead||!r.raw||!resultWordEquals(r.raw->data()+0x5c,r.count)))return false;
        if(r.admitted&&(!r.countStored||r.count>65535u||r.count>(std::numeric_limits<std::uint32_t>::max()-16u)/32u))return false;
        for(unsigned i=0;i<7;++i){const auto& a=r.arrays[i];
            if(!a.attempted){if(a.raw||a.payload||a.bytes||a.vectors||a.cookieStored||a.constructorsReturned||a.published)return false;continue;}
            if(!r.admitted||!a.published||(i&&!r.arrays[i-1].published)||!a.payload)return false;
            const bool vector=i>=4;const auto vectors=vector?(i==4?2u*r.count:r.count):0u;
            const auto bytes=vector?std::size_t(vectors)*16u+16u:std::size_t(r.count)*(i==3?2u:4u);
            if(a.vectors!=vectors||a.bytes!=bytes||*a.payload!=(a.raw?static_cast<const void*>(a.raw.get()+(vector?16u:0u)):nullptr))return false;
            if(a.cookieStored!=(vector&&bool(a.raw))||a.constructorsReturned!=a.cookieStored)return false;
            //Scalar allocation contents and cookie8..15 are source unknown.
            if(vector&&a.raw){
                if(!resultWordEquals(a.raw.get(),16u)||!resultWordEquals(a.raw.get()+4,vectors))return false;
                for(std::uint32_t n=0;n<vectors;++n)if(!resultVectorCurrent(a.raw.get()+16u+std::size_t(n)*16u))return false;
            }
        }
        if(r.published)return r.raw&&r.arrays[6].published&&r.returnedPayload==r.arrays[6].payload&&r.pc==0x11bd30;
        if(r.returnedPayload)return r.raw&&r.arrays[6].published&&r.returnedPayload==r.arrays[6].payload&&r.pc==0x11bd18;
        if(!r.countRead)return r.pc==0x11bc3c;
        if(!r.raw)return r.pc==0x11bc40;
        return r.countStored&&!r.admitted&&r.pc==0x11bc48;
    }
    bool withdrawSourceResultNative()noexcept {
        auto& r=sourceResult_;if(!r.attempted||r.nativeWithdrawn)return true;
        if(!preflightSourceResultNative())return false;
        //Native cancellation, not original11B600 result destruction. All
        //nullable/cookie allocation bases are owned independently and freed once.
        for(auto& a:r.arrays)a.raw.reset();
        r.raw.reset();r.nativeWithdrawn=true;return true;
        //COL then revokes its exact Type0 result token/90 alias; refusal leaves
        //this historical owner intact without touching freed payloads on retry.
    }
