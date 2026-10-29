// Class-scope implementation fragment: include ONLY in the private section of
// OriginalApplicationAural50KG. No includes, namespace, factory, or public API.
// This foundation alone admits no listener or new source continuation.
struct NativeListener290A90;
struct NativeListenerBackend2357A0 final {
private:
    friend class OriginalApplicationAural50KG;
    friend struct NativeListener290A90;
    using VectorWords=std::array<std::uint32_t,4>;
    // Source non-Entity base1048F0. Native source table identity is NOT a
    // dereferenceable guest address, hardware handle, or Entity registration.
    std::uint64_t reference00_;
    std::uint32_t sourceTable08_;
    VectorWords position10_,look20_,up30_,velocity40_;

    NativeListenerBackend2357A0()noexcept {
        sourceTable08_=0x3cc980u; //1048F8.
        reference00_=1u;         //104900 SD, not a32-bit reference.
        sourceTable08_=0x3d1a80u; //2357BC.
        constexpr VectorWords vuZero{{0u,0u,0u,0x3f800000u}};
        position10_=vuZero;      //2357C4.
        look20_=vuZero;          //2357CC.
        up30_=vuZero;            //2357D4.
        velocity40_=vuZero;      //2357DC.
        // Actual startup3CA250..3CA2AC publishes4349D0=(0,0,0,1).
        // Retain both constructor store groups, even though values coincide.
        constexpr VectorWords startup4349D0{{0u,0u,0u,0x3f800000u}};
        position10_=startup4349D0; //2357EC.
        look20_=startup4349D0;     //2357F8.
        up30_=startup4349D0;       //235804.
        velocity40_=startup4349D0; //235810.
    }
    NativeListenerBackend2357A0(const NativeListenerBackend2357A0&)=delete;
    NativeListenerBackend2357A0& operator=(const NativeListenerBackend2357A0&)=delete;
public: // Type itself remains private to Aural; permits its unique_ptr deleter.
    ~NativeListenerBackend2357A0()noexcept {
        sourceTable08_=0x3d1a80u; //235854, before104910(A1=0).
        sourceTable08_=0x3cc980u; //10493C; native delete then frees storage.
    }
private:
    void release104970()noexcept {
        --reference00_; //10497C/80. The private factory issues exactly one ref.
        if(reference00_==0u)delete this; //1049A0 ->235830(A1=1).
    }
    bool current()const noexcept {
        return reference00_==1u&&sourceTable08_==0x3d1a80u;
    }
};

struct NativeListener290A90 final {
private:
    friend class OriginalApplicationAural50KG;
    using Backend=NativeListenerBackend2357A0;
    using XYZWords=std::array<std::uint32_t,3>;
    // Sole owning source pointer. Only enclosing Aural's authentic factory may
    // install it, after232D50's actual NativeInterface28 admission. No public
    // adoption, nonnull-only proof, arbitrary provider, or second mixer exists.
    OriginalApplicationAural50KG* aural00_;
    Backend* backend04_;
    NativeListener290A90()noexcept {
        aural00_=nullptr;  //290A90.
        backend04_=nullptr; //290A94.
    }
    NativeListener290A90(const NativeListener290A90&)=delete;
    NativeListener290A90& operator=(const NativeListener290A90&)=delete;
public: // Type itself remains private to Aural; permits its unique_ptr deleter.
    ~NativeListener290A90()noexcept {releaseAndClear290AA0();}
private:
    void releaseAndClear290AA0()noexcept {
        if(backend04_){
            backend04_->release104970(); //290AD0: virtual+10, NOT direct destructor.
            backend04_=nullptr;         //290AD8 AFTER release/destruction.
        }
        //290AA0 does not clear aural00_; enclosing native delete frees wrapper.
    }
    void bindAural290BAC(OriginalApplicationAural50KG& actual)noexcept {aural00_=&actual;}
    void installBackend290BBC(Backend* actualFactoryReturn)noexcept {
        // Only enclosing Aural may call, once, with its actual232D50 return;
        // includes null. Caller must retain source allocation/flag chronology.
        backend04_=actualFactoryReturn;
    }
    const OriginalApplicationAural50KG* sourceOwner00()const noexcept {return aural00_;}
    bool hasBackend04()const noexcept {return backend04_!=nullptr;}
    bool current(const OriginalApplicationAural50KG& actual)const noexcept {
        return aural00_==&actual&&backend04_&&backend04_->current();
    }
    // Exact backend235890/A0/B0/C0 are JR RA; ADDIU V0,zero,1.
    // Their wrapper null test remains real. Passed scalar/A1 arguments have
    // no stores/effects in these PS2 leaves; never manufacture attenuation.
    bool update290C70()const noexcept {return backend04_!=nullptr;}
    bool initialize290CB0(std::uint32_t /*actualF12Bits*/)const noexcept {return backend04_!=nullptr;}
    bool initialize290CF0(std::uint32_t /*actualF12Bits*/)const noexcept {return backend04_!=nullptr;}
    bool initialize290D30(std::uint32_t /*actualF12Bits*/)const noexcept {return backend04_!=nullptr;}

    bool readPosition290E10(XYZWords& out)const noexcept {
        if(!backend04_)return false; //No output writes on original null backend.
        out[0]=backend04_->position10_[0];out[1]=backend04_->position10_[1];out[2]=backend04_->position10_[2];return true;
    }
    bool writePosition290E50(const XYZWords& in)noexcept {
        if(!backend04_)return false;
        backend04_->position10_[0]=in[0];backend04_->position10_[1]=in[1];backend04_->position10_[2]=in[2];return true;
    }
    bool readOrientation290D70(XYZWords& look,XYZWords& up)const noexcept {
        if(!backend04_)return false;
        look[0]=backend04_->look20_[0];look[1]=backend04_->look20_[1];look[2]=backend04_->look20_[2];
        up[0]=backend04_->up30_[0];up[1]=backend04_->up30_[1];up[2]=backend04_->up30_[2];return true;
    }
    bool writeOrientation290DC0(const XYZWords& look,const XYZWords& up)noexcept {
        if(!backend04_)return false;
        backend04_->look20_[0]=look[0];backend04_->look20_[1]=look[1];backend04_->look20_[2]=look[2];
        backend04_->up30_[0]=up[0];backend04_->up30_[1]=up[1];backend04_->up30_[2]=up[2];return true;
    }
    bool readVelocity290E90(XYZWords& out)const noexcept {
        if(!backend04_)return false;
        out[0]=backend04_->velocity40_[0];out[1]=backend04_->velocity40_[1];out[2]=backend04_->velocity40_[2];return true;
    }
    bool writeVelocity290ED0(const XYZWords& in)noexcept {
        if(!backend04_)return false;
        backend04_->velocity40_[0]=in[0];backend04_->velocity40_[1]=in[1];backend04_->velocity40_[2]=in[2];return true;
    }
};
