// Native-width backing for original281FE0 eight-byte {Font*,next*}.
// Its allocation/null/link/count order is retained, not a guest heap address.
struct LoadingRegistryNode {const void* font;LoadingRegistryNode* next;};
// Included privately in OriginalApplicationFonts50LA. Source values and native
// custody are distinct; no member1C cache or GPU readiness is inferred here.
struct LoadingGlyph281BE4 {
    std::uint32_t index00;
    float left04,right08,top0C,bottom10;
    std::array<std::int16_t,4> adjustment14;
    std::uint32_t adjusted1C;
};
static_assert(sizeof(LoadingGlyph281BE4)==0x20,"source glyph payload");
struct LoadingFontPartial {
    std::unique_ptr<char[]> name04;
    std::array<std::unique_ptr<LoadingGlyph281BE4>,256> glyphs08;
    std::array<bool,256> glyphPublished{};
    // Native fixed input staging, not a source read or guest allocation.
    std::array<std::uint8_t,maxOriginalUiCelBytes50KG> readBytes{};
    std::size_t bytesRead=0;
    std::uint32_t count298=0,index29C=0,adjustmentCount2B0=0,adjustmentIndex2B4=0;
    std::array<float,4> coordinates2A0{};
    std::array<std::int16_t,4> adjustment2B8{};
    std::uint32_t glyphRows=0,adjustmentRows=0;
    std::array<char,256> materialName190{};
    bool materialNameWritten=false,materialLookupPending=false,material408Stored=false;
    bool populationEntered=false,populationReturned=false,directoryRestored=false,celClosed=false;
    bool vtable00Constructed=false,name04Constructed=false,glyphSlotsConstructed=false,material408Constructed=false;
    bool namePublished=false,outerAllocated=false,sourceReadsComplete=false;
};
