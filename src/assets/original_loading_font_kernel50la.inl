// Private Loading-only source kernel. Existing generic CEL decoder remains a
// native admission envelope; it does not manufacture this source history.
struct FontSourceAssertionHistory {
    bool unjoinedGenericProducer=false; // native contamination; never cleared
    bool index8C0C=false,bounds8C14=false,adjustment80BC=false;
    std::uint32_t index4431A0=0,bounds4431B0=0,adjustment3DF418=0;
};
static FontSourceAssertionHistory& fontSourceAssertions()noexcept {
    static FontSourceAssertionHistory state;
    return state;
}
static bool loadLoadingCel2817D0(Entry& e,const std::string& path,
    const char* basename,OriginalApplicationFontDiagnostic50LA& d)noexcept {
    using S=OriginalApplicationFontStatus50LA;
    // Entry is native admission/custody, distinct from the nullable original
    // Font allocation. Typed backing is native-sized, not a guest address.
    e.loadingOuterAttempted_=true;
    e.loadingPartial_.reset(new(std::nothrow) LoadingFontPartial);
    e.loadingOuterKnownNull_=!e.loadingPartial_;
    // Actual mallocNULL skips constructor, then calls2817D0 withNULL. Bound
    // before that call, retaining the known-null argument, not a loader return.
    if(!e.loadingPartial_)return fail(d,S::allocation,0x2823E0);
    auto& p=*e.loadingPartial_;p.outerAllocated=true;
    p.vtable00Constructed=true; //282380:3D2E48 (native typed vtable, no guest address)
    p.name04.reset();p.name04Constructed=true;
    for(auto& glyph:p.glyphs08)glyph.reset();
    p.glyphSlotsConstructed=true; //all256 source pointer stores reached
    e.texture408_=-1;p.material408Constructed=true; //2823B0 before name allocation
    const auto nameBytes=std::strlen(basename)+1;
    p.name04.reset(new(std::nothrow) char[nameBytes]);
    p.namePublished=true; //2823C4, including genuine allocation-null
    if(!p.name04)return fail(d,S::allocation,0x2823CC); //before strcpy
    std::memcpy(p.name04.get(),basename,nameBytes);
    if(!e.loadingSource_||!e.loadingSource_->beginCel281890(&e,path,e.cel_,d.texture))
        return fail(d,S::source,d.texture.sourcePC?d.texture.sourcePC:0x281890);
    // A refusal stops at the next original consumer, retaining actual bytes,
    // source locals, cursor, scratch/window history and all prior glyphs. It
    // is not an invented false return from2817D0 or an automatic file close.
    const auto read=[&](void* out,std::size_t size,std::uint32_t pc,std::uint32_t next)noexcept {
        std::size_t done=0;
        const bool okay=e.loadingSource_->readCel12FB30(&e,out,size,done,d.texture);
        if(done>size||done>p.readBytes.size()-p.bytesRead)
            return fail(d,S::source,pc);
        if(done){std::memcpy(p.readBytes.data()+p.bytesRead,out,done);p.bytesRead+=done;}
        if(!okay)return fail(d,S::source,d.texture.sourcePC?d.texture.sourcePC:pc);
        if(done!=size)return fail(d,S::source,next);
        return true;
    };
    if(!read(&p.count298,4,0x281934,0x28193C))return false;
    p.index29C=0;p.coordinates2A0[0]=0;p.coordinates2A0[1]=0;
    p.coordinates2A0[2]=0;p.coordinates2A0[3]=0; //28193C..28194C
    if(p.count298>256)return fail(d,S::font,0x281950); //native cohort bound
    while(p.glyphRows<p.count298){
        if(!read(&p.index29C,4,0x28196C,0x281974))return false;
        auto& guards=fontSourceAssertions();
        if(!guards.index8C0C){guards.index4431A0=1;guards.index8C0C=true;}
        if(p.index29C>=256)return fail(d,S::font,0x2819CC);
        constexpr std::array<std::uint32_t,4> readPC{{0x281A14,0x281A38,0x281A5C,0x281A80}};
        for(std::size_t c=0;c<4;++c)
            if(!read(&p.coordinates2A0[c],4,readPC[c],readPC[c]+8))return false;
        if(!guards.bounds8C14){guards.bounds4431B0=1;guards.bounds8C14=true;}
        const auto& v=p.coordinates2A0;
        if(!(v[0]<=v[2]&&v[1]<=v[3]))return fail(d,S::font,0x281B74);
        // Duplicate overwrite/leak is outside the authenticated strict CEL
        // cohort, not silently replaced by a unique_ptr deletion.
        if(p.glyphPublished[p.index29C])return fail(d,S::font,0x281BE4);
        std::unique_ptr<LoadingGlyph281BE4> g(new(std::nothrow) LoadingGlyph281BE4);
        if(g){
            g->adjustment14[0]=0;g->adjustment14[1]=0;
            g->adjustment14[2]=0;g->adjustment14[3]=0;g->adjusted1C=0;
            g->index00=p.index29C;
            g->left04=v[0];g->right08=v[2];g->top0C=v[1];g->bottom10=v[3];
        }
        ++p.glyphRows; //281C34 precedes nullable pointer publication
        p.glyphs08[p.index29C]=std::move(g);p.glyphPublished[p.index29C]=true; //281C40
        if(!p.glyphs08[p.index29C])return fail(d,S::allocation,0x281C48);
    }
    p.adjustmentCount2B0=0;p.adjustmentIndex2B4=0;
    p.adjustment2B8[0]=0;p.adjustment2B8[1]=0;p.adjustment2B8[2]=0;p.adjustment2B8[3]=0;
    bool eof=false;
    if(!e.loadingSource_->eofCel12F3E0(&e,eof,d.texture))
        return fail(d,S::source,d.texture.sourcePC?d.texture.sourcePC:0x281C78);
    if(!eof){
        if(!read(&p.adjustmentCount2B0,4,0x281C9C,0x281CA4))return false;
        if(p.adjustmentCount2B0>256)return fail(d,S::font,0x281CA4);
        for(;p.adjustmentRows<p.adjustmentCount2B0;++p.adjustmentRows){
            if(!read(&p.adjustmentIndex2B4,4,0x281CCC,0x281CD4))return false;
            constexpr std::array<std::uint32_t,4> readPC{{0x281CF0,0x281D14,0x281D38,0x281D5C}};
            for(std::size_t c=0;c<4;++c)
                if(!read(&p.adjustment2B8[c],2,readPC[c],readPC[c]+8))return false;
            auto& guards=fontSourceAssertions();
            if(!guards.adjustment80BC){guards.adjustment3DF418=1;guards.adjustment80BC=true;}
            if(p.adjustmentIndex2B4>=256)return fail(d,S::font,0x281E34);
            auto* g=p.glyphs08[p.adjustmentIndex2B4].get();
            if(g){g->adjustment14[0]=p.adjustment2B8[0];g->adjustment14[1]=p.adjustment2B8[1];
                g->adjustment14[2]=p.adjustment2B8[2];g->adjustment14[3]=p.adjustment2B8[3];g->adjusted1C=1;}
        }
    }
    p.sourceReadsComplete=true;
    // Immutable consumer metrics are derived from bytes returned by the SAME
    // handle, only after all real source allocations/publications above.
    // This is a native mirror; source glyph ownership stays alive separately.
    if(p.bytesRead!=e.cel_.size||std::memcmp(p.readBytes.data(),e.cel_.data,p.bytesRead)||
       !decodeOriginalUiCel50KG(p.readBytes.data(),p.bytesRead,e.metrics_,d.cel))
        return fail(d,S::font,0x281EC0);
    return true;
}
