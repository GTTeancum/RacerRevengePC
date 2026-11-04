// Included inside rr::game beside metadata, after the complete manager Impl.
// Only the retained actual2C0C44 occurrence can enter. No host sprintf, source
// printf result, duplicate process State, or independent readiness token.
bool OriginalRaceMusicManager50LA::formatSourceSong2C0C44(std::uint32_t ordinal,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    if(!impl_||!impl_->sourceOnly||!impl_->sourceChild)
        return Impl::stop(d,0x2c0c44,"Actual original music child absent");
    auto& p=*impl_;auto& child=*p.sourceChild;
    const auto stop=[&](std::uint32_t pc,const char* why)noexcept {
        p.sourcePC=child.pc=pc;return Impl::stop(d,pc,why);};
    if(!p.componentCurrent()||!child.metadataAttempted||child.pc!=0x2c0c44||
       !ordinal||ordinal>47u)
        return stop(0x2c0c44,"Original fixed Music formatter occurrence unavailable");
    auto& frame=child.songFormats[ordinal-1u];
    if(frame.attempted)return stop(0x308c90,"Original Music formatter occurrence already retained");
    frame.attempted=true;
    char* destination=child.songSection860.data();const auto extent=child.songSection860.size();
    using G=assets::OriginalConfigProcessGlobals50LA;
    using Identity=original_race_music_source50LA::Child::SongFormat::ReentIdentity;
    const G::MusicSprintfAccess access;
    G::StdioIdentity captured;std::uint32_t pc=0x308c98;
    if(!G::readMusicReent308C98(access,captured,pc))
        return stop(pc,"Original Music formatter reentrancy read remains bounded");
    frame.capturedReent=Identity::source385160; //Only after same-process read succeeds.
    frame.flags0C=0x208u; //308CD0. Other temporary FILE fields remain unknown.
    frame.buffer10=destination; //308CD4.
    frame.bufferSize14=0x7fffffffu; //308CD8.
    frame.fileReent54=Identity::source385160; //308CDC, retained captured identity.
    frame.cursor00=destination; //308CE0.
    frame.remaining08=0x7fffffffu; //308CE8 call delay slot BEFORE30B4F0.
    //The actual call uses the captured private identity, not a fabricated enum
    //conversion or a fresh global read in place of the FILE's non-NULL+54.
    if(!G::initializeMusicSprintf30B528(access,captured,pc))
        return stop(pc,"Original Music formatter stdio initialization remains bounded");
    frame.initializerReturned=true;
    char unusedDecimal;
    if(!G::musicLocale305328(access,unusedDecimal,pc))
        return stop(pc,"Original Music fixed locale pointer read remains bounded");
    frame.localeReturned=true; //No decimal byte is consumed by fixed Song %d.
    const auto scan=[&](std::size_t offset,unsigned char byte)noexcept {
        if(frame.scanCount==frame.scans.size())return stop(0x30b658,"Native Music format scan receipt capacity exceeded");
        auto& receipt=frame.scans[frame.scanCount++];receipt.offset=offset;
        std::uint32_t wide=0;std::int32_t length=0;
        if(!G::musicByte306108(access,byte,wide,length,pc))
            return stop(pc,"Original Music format byte scan remains bounded");
        receipt.length=length;if(length>=0)receipt.wide=wide;
        if(length!=(byte?1:0))return stop(0x30b674,"Original Music formatter scan outside admitted single-byte branch");
        return true;
    };
    const auto copySegment=[&](const char* text,std::size_t bytes)noexcept {
        if(frame.segmentCount==frame.copiedSegments.size()||!frame.remaining08||
           !frame.cursor00||!frame.flags0C||*frame.flags0C!=0x208u||
           bytes>*frame.remaining08||frame.copied>=extent||bytes>=extent-frame.copied)
            return stop(0x304f40,"Original Music string-FILE copy exceeds retained destination");
        //304F28 string flag bypasses FILE callbacks and buffering.306370 copies
        //this known segment; selected count/cursor stores follow in source order.
        std::memcpy(*frame.cursor00,text,bytes);
        *frame.remaining08-=static_cast<std::uint32_t>(bytes); //304F58.
        *frame.cursor00+=bytes; //304F5C.
        frame.copiedSegments[frame.segmentCount++]=bytes;frame.copied+=bytes;return true;
    };
    constexpr char format[]="Song %d"; //Immutable original bytes at3C16A0.
    for(std::size_t n=0;n<=5u;++n)if(!scan(n,static_cast<unsigned char>(format[n])))return false;
    //30B718 reads 'd' directly, not through306108. Positive signed1..47
    //takes the decimal quotient/remainder path without flags, width, or sign.
    std::array<char,2> reversed,digits;std::size_t count=0;auto value=ordinal;
    do{reversed[count++]=char('0'+value%10u);value/=10u;}while(value);
    for(std::size_t n=0;n<count;++n)digits[n]=reversed[count-1u-n];
    //30C9FC flushes literal and decimal segments BEFORE the final NUL scan.
    if(!copySegment(format,5u)||!copySegment(digits.data(),count))return false;
    if(!scan(7u,0))return false;
    frame.formatterReturn=static_cast<std::uint32_t>(frame.copied); //30CA40/4C; flag0x40 remains clear.
    **frame.cursor00=0;frame.terminated=true; //308CF4; no cursor/count update.
    frame.sourceReturn=frame.formatterReturn; //308CF8 return, ignored by2C0C4C.
    child.songSectionKnown=frame.copied+1u;p.sourcePC=child.pc=0x2c0c4c;
    d={child.pc,nullptr};return true;
}
