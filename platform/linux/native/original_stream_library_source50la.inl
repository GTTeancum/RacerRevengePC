// Included by the complete Aural TU. The body has no current issuer: only
// the future private17CCF0 owner may issue232EA0, then268690 only after its return.
#include "../../../src/assets/original_stream_library_file50la.h"
#include "../../../src/assets/original_config_process_globals50la.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <optional>
#include <string_view>
namespace rr::platform {
struct OriginalStreamLibrarySource50LA::Catalog final {
    std::uint64_t reference00;std::uint32_t dispatch08;
    std::optional<std::uint32_t> padding0C;
    std::uint32_t count10,capacity14,retainedCount24,extraCount2C;
    const void *retained20,*extra28;
    std::unique_ptr<std::uint8_t[]> text18,textKnown;
    struct Row {
        const char* name00=nullptr;const void* payload04=nullptr;std::uint32_t references08=0;
        bool nameKnown=false,payloadKnown=false,referencesKnown=false;
        std::optional<std::size_t> nameOffset; //Native extent custody at actual2988A0, not a guest address.
        std::unique_ptr<std::array<std::uint8_t,8>> payloadBacking; // Native ownership of associated source+4.
    };
    std::unique_ptr<Row[]> rows1C;
    std::size_t textExtent=0,rowsExtent=0,rowsWritten=0;
    Catalog()noexcept {
        dispatch08=0x3cc980u;reference00=1; //1048F0,64-bit reference store104900.
        dispatch08=0x3d3690u;count10=0;capacity14=0; //2984AC..B4.
        text18.reset();rows1C.reset();retained20=nullptr;retainedCount24=0;
        extra28=nullptr;extraCount2C=0; //2984B8..CC.0C remains UNKNOWN.
    }
};
struct OriginalStreamLibrarySource50LA::CatalogLoad {
    std::shared_ptr<assets::OriginalStandardTextureResolver50LA> resolver;
    std::weak_ptr<assets::OriginalStandardTextureResolver50LA> resolverIdentity;
    std::unique_ptr<assets::OriginalStreamLibraryFile50LA> configFile;
    assets::OriginalStandardTextureDiagnostic50LA configFileDiagnostic{};
    std::uint32_t pc=0;
    std::array<char,0x120> catalogPath30;
    std::size_t catalogPathKnown=0;
    bool catalogPublished=false,catalogDispatchEntered=false,statusReturned=false;
    bool catalogReadReturned=false,catalogCloseReturned=false;
    std::optional<std::int64_t> catalogHandle;
    std::uint64_t modified148,size140;
    std::size_t catalogCopied=0,loweredBytes=0,countScanBytes=0,rowScanBytes=0;
    std::optional<std::uint32_t> catalogReturn,source232Return;
    bool nativeFileRetired=false;
    CatalogLoad()noexcept {} // Source output locals remain uninitialized.
};
struct OriginalStreamLibrarySource50LA::Storage final : CatalogLoad {
    struct Config final {
        std::int64_t file00;
        std::uint32_t binary08,opened0C,mode10,count14,index18;
        const void* text1C;std::uint32_t selected20;const void* child24;
        std::array<char,1024> name28;
        std::uint32_t field428,field42C,field430,field434,cacheCount438;
        std::array<std::uint32_t,32> cache43C;
        const void *names4CC,*index4BC;
        std::uint32_t indexBytes4C0;const void* block4C4;std::uint32_t field4C8;
        Config()noexcept {
            file00=-1;binary08=0;opened0C=0;mode10=0;count14=0;index18=0;
            text1C=nullptr;selected20=0;child24=nullptr;name28[0]=0;
            field428=0;field42C=0;field430=0;field434=0;cacheCount438=0;
            cache43C.fill(0);names4CC=nullptr;index4BC=nullptr;
            indexBytes4C0=0;block4C4=nullptr;field4C8=0;
        }
    };
    std::optional<Config> config190;
    struct Format {
        using Identity=assets::OriginalConfigProcessGlobals50LA::StdioIdentity;
        std::optional<Identity> capturedReent,fileReent54;
        std::optional<char*> buffer10,cursor00;
        std::optional<std::uint32_t> remaining08,bufferSize14;
        std::optional<std::uint16_t> flags0C;
        struct Scan {std::size_t offset=0;std::optional<std::uint32_t> wide;std::optional<std::int32_t> length;};
        std::array<Scan,18> scans;std::size_t scanCount=0,copied=0;
        std::array<std::size_t,2> copiedSegments{};std::size_t segmentCount=0;
        bool attempted=false,initializerReturned=false,localeReturned=false,terminated=false;
        std::optional<std::uint32_t> formatterReturn,sourceReturn;
    };
    Format filenameFormat;std::array<Format,1743> songFormats;
    std::array<char,0x110> filename80;
    std::size_t filenameKnown=0;
    struct Read {std::uint32_t callPC=0;std::size_t requested=0,copied=0;bool returned=false;};
    std::array<Read,6980> configReads{};std::size_t configReadCount=0;
    std::array<std::uint8_t,7> magic48;
    std::array<std::uint8_t,4> version54,sections58,length5C;
    std::size_t magicKnown=0,versionKnown=0,sectionsKnown=0;
    bool magicTerminatorWritten=false,lengthZeroed=false;
    std::uint32_t nameOrdinal=0;
    std::array<std::size_t,1744> nameKnown{};
    std::unique_ptr<std::uint8_t[]> names4CC,index4BC;std::size_t indexKnown=0;
    std::optional<std::uint32_t> configOpenReturn;
    struct Lookup {
        std::array<char,1024> lowerKey;std::size_t lowerKnown=0;
        std::array<std::int32_t,32> visited{};std::size_t visitedCount=0;
        std::optional<std::int32_t> result;
        std::uint32_t comparisonPC=0;std::uint8_t matchArm=0;
    };
    struct Selection {Lookup lookup;std::optional<bool> returned;
        std::optional<std::uint32_t> offset,length;std::array<std::uint8_t,4> fieldCount86C;
        bool countZeroed=false,seekReturned=false;};
    struct Getter {Lookup lookup;std::optional<bool> returned;std::size_t produced=0;};
    std::array<Selection,1744> selections;Getter numberGetter;
    struct Song {Getter duration,pitch;std::optional<std::uint64_t> durationDouble;
        std::optional<std::uint32_t> durationBits,pitchValue;
        std::optional<bool> durationReturned,pitchReturned;
        std::uint8_t payloadKnownMask=0;bool formatted=false,associated=false;};
    std::array<Song,1743> songs;
    std::unique_ptr<std::uint8_t[]> block4C4,blockKnown;
    std::size_t blockCapacity=0,configNameKnown=1;
    std::array<char,1024> numberStack20,durationStack30;
    std::optional<std::uint32_t> count664,durationBits66C,pitch668;
    std::optional<bool> numberOfSongsReturned;
    std::array<char,0x40> songSection40;std::size_t songSectionKnown=0;
    std::unique_ptr<std::array<std::uint8_t,8>> pendingPayload8; // No source row association yet.
    std::optional<std::uint32_t> songOrdinal;std::size_t associatedSongs=0;
    struct Free {const void* identity=nullptr;std::uint32_t callPC=0;
        bool entered=false,returned=false;};
    struct Destruction {
        bool entered=false,loadedStoreReached=false,flushEntered=false,closeEntered=false;
        std::optional<std::int32_t> flushReturn;
        bool closeReturned=false,selectedClearReturned=false;
        Free names,index,block;
        bool returned=false,automaticLifetimeEnded=false,nativeFileRetired=false;
        const void* returnedThis=nullptr;std::int32_t flag=-1;
    } destruction;
    std::optional<std::uint32_t> libraryReturn;
    Storage()noexcept {pc=0x268690;} // Unknown automatic bytes are not value-initialized.
};
OriginalStreamLibrarySource50LA::OriginalStreamLibrarySource50LA(
    const Aural& aural,const void* interface)noexcept
    :issuer_(aural.issuer_),aural_(&aural),interfaceA0_(interface) {
    loadedDFC_=0;catalog98A0_.reset();audioDirectory98A4_.reset(); //266670,2668A4/B4 at actual interface construction.
}
OriginalStreamLibrarySource50LA::~OriginalStreamLibrarySource50LA()noexcept {
    if((load_||catalog98A0_||audioDirectoryLoad_||audioDirectory98A4_)&&!nativeClosed_)std::terminate();
}
bool OriginalApplicationAural50KG::sourceStreamLibraryOwnerCurrent(
    const OriginalStreamLibrarySource50LA& library,const assets::OriginalStandardTextureResolver50LA* resolver)const noexcept {
    if(!current()||!nativeInitialized_||!backendCurrent()||
       !interfaceA0_||!*interfaceA0_||library.aural_!=this||library.issuer_!=issuer_||
       library.thread_!=thread_||library.nativeClosed_||!issuer_->sourceAural00Owner_||
       issuer_->sourceAural00Owner_.get()!=this||!issuer_->standardMaterialDomainOwner_||
       !issuer_->standardMaterialDomainOwner_->prepared())return false;
    const auto& i=**interfaceA0_;
    return library.interfaceA0_==&i&&&i.sourceStreamLibrary==&library&&resolver&&
        issuer_->standardMaterialDomainOwner_->resolver3E1A70().get()==resolver;
}
bool OriginalApplicationAural50KG::sourceStreamLibraryIdentityCurrent(
    const OriginalStreamLibrarySource50LA& library,const assets::OriginalStandardTextureResolver50LA* resolver)const noexcept {
    //Startup-only admission. Later genuine catalog calls use the same-owner
    //predicate plus their retained completed268690, not this unattached state.
    return sourceStreamLibraryOwnerCurrent(library,resolver)&&!attached_&&!entity_.parent1C&&
        !entities_->pendingWaves98A4_&&!entities_->pendingWavesCreated_;
}
bool OriginalStreamLibrarySource50LA::closeNative()noexcept {
    if(thread_!=std::this_thread::get_id())return false;
    if(nativeClosed_)return true;
    // Explicit Aural native retirement only. No1573B0/26DB20 invocation,
    // loaded-flag write, source call return or replay permission is implied.
    if(load_&&load_->configFile){assets::OriginalStandardTextureDiagnostic50LA d;
        if(!load_->configFile->closeNative(d))return false;}
    if(audioDirectoryLoad_&&audioDirectoryLoad_->configFile){assets::OriginalStandardTextureDiagnostic50LA d;
        if(!audioDirectoryLoad_->configFile->closeNative(d))return false;}
    //Native release preserves original backend teardown order: Stream first,
    //then AudioStr. It does not claim26DB20 or either source destructor.
    load_.reset();catalog98A0_.reset();audioDirectoryLoad_.reset();audioDirectory98A4_.reset();
    nativeClosed_=true;return true;
}
bool OriginalStreamLibrarySource50LA::begin232EA0(std::shared_ptr<Resolver> resolver,Diagnostic& d)noexcept {
    d={};
    //Only the future genuine17CCF0 issuer can enter. Existing native pending
    //waves are explicitly rejected; metadata is not an earlier source return.
    if(thread_!=std::this_thread::get_id()||nativeClosed_||audioDirectoryAttempted_||attempted_||
       !aural_||!issuer_||!aural_->sourceStreamLibraryIdentityCurrent(*this,resolver.get()))
        return stop(d,0x232ea0,"Authentic same-Aural AudioStr occurrence unavailable");
    audioDirectoryAttempted_=true;
    try {audioDirectoryLoad_.reset(new CatalogLoad);}
    catch(...){return stop(d,0x26d8c0,"Native AudioStr call storage unavailable");}
    auto& child=*audioDirectoryLoad_;child.pc=0x26d8c0;
    child.resolverIdentity=resolver;child.resolver=std::move(resolver);
    const auto fail=[&](std::uint32_t pc,const char* why)noexcept {child.pc=pc;return stop(d,pc,why);};
    //232EA0 tail jump uses this SAME interface+40, not a second backend.
    if(audioDirectory98A4_)return fail(0x26d8f8,"Original AudioStr replacement destructor is not joined");
    std::unique_ptr<Catalog> next;
    try {next.reset(new Catalog);}
    catch(...){return fail(0x26d904,"Native AudioStr catalog backing unavailable; source allocation return unknown");}
    audioDirectory98A4_=std::move(next);child.catalogPublished=true; //26D924 BEFORE slot18.
    constexpr char input[]="AppStart\\AudioStr.txt";
    if(!loadCatalog2985B0(*audioDirectory98A4_,child,true,std::string_view(input,sizeof(input)-1u),d))return false;
    //The source directory reader has already closed at298714 and returned
    //its count. Retire native pin only; catalog text/rows remain owned98A4.
    child.pc=0x26d944;
    if(!child.configFile->closeNative(child.configFileDiagnostic))return fail(0,"Native AudioStr file-custody release remains bounded");
    child.nativeFileRetired=true;child.configFile.reset();child.resolver.reset();
    child.source232Return=child.catalogReturn;child.pc=0x26d950;
    d={child.pc,nullptr};return true; //Local232EA0 return only, no17CCF0 advancement.
}
bool OriginalStreamLibrarySource50LA::loadCatalog2985B0(Catalog& catalog,CatalogLoad& child,
        bool audioDirectory,std::string_view filename,Diagnostic& d)noexcept {
    const auto fail=[&](std::uint32_t pc,const char* why)noexcept {child.pc=pc;return stop(d,pc,why);};
    child.catalogDispatchEntered=true;
    // Fresh298490 makes old rows/text branches skip. No replay or adopted table.
    if(catalog.dispatch08!=0x3d3690u||catalog.rows1C||catalog.text18||catalog.retainedCount24||catalog.retained20)
        return fail(0x2985c0,"Original fresh catalog owner changed");
    if(filename.size()>=child.catalogPath30.size())return fail(0x298640,"Original catalog filename exceeds native extent");
    for(std::size_t n=0;n<filename.size();++n){const auto c=filename[n];
        if(static_cast<unsigned char>(c)>127u)return fail(0x298658,"Original filename lowercase is outside ASCII cohort");
        child.catalogPath30[n]=c>='A'&&c<='Z'?char(c+32):c;}
    child.catalogPath30[filename.size()]=0;child.catalogPathKnown=filename.size()+1u; //298640 strcpy then30B158.
    try {child.configFile.reset(new assets::OriginalStreamLibraryFile50LA(issuer_,this));}
    catch(...){return fail(0x298680,"Native file occurrence backing unavailable");}
    auto& fd=child.configFileDiagnostic;std::int64_t returned=-1;
    const auto kind=audioDirectory?assets::OriginalStreamLibraryFile50LA::Kind::audioDirectory:
        assets::OriginalStreamLibraryFile50LA::Kind::streamNames;
    if(!child.configFile->begin131230(child.resolver,&catalog,kind,returned,fd))
        return fail(fd.sourcePC,"Original catalog open remains bounded");
    child.catalogHandle=returned;
    if(returned==-1)return fail(0x2986a0,"Original catalog-open failure diagnostic is not joined");
    if(!child.configFile->status12EC90(&catalog,returned,child.modified148,child.size140,fd))
        return fail(fd.sourcePC,"Original catalog status remains bounded");
    child.statusReturned=true;
    if(child.size140!=(audioDirectory?7542u:55535u))return fail(0x2986d4,"Original catalog size outside authenticated cohort");
    const auto size=std::size_t(child.size140);
    std::unique_ptr<std::uint8_t[]> nextText,nextKnown;
    try {nextText.reset(new std::uint8_t[size+1]);nextKnown.reset(new std::uint8_t[size+1]);}
    catch(...){return fail(0x2986d4,"Native catalog text backing unavailable; source malloc return unknown");}
    catalog.text18=std::move(nextText);catalog.textKnown=std::move(nextKnown); //2986DC.
    catalog.textExtent=size+1;std::fill_n(catalog.textKnown.get(),size+1,std::uint8_t(0));
    catalog.text18[size]=0;catalog.textKnown[size]=1; //2986DC publication then2986F4 terminator.
    child.catalogReadReturned=child.configFile->read12FB30(&catalog,returned,catalog.text18.get(),size,child.catalogCopied,fd);
    for(std::size_t n=0;n<child.catalogCopied;++n)catalog.textKnown[n]=1;
    if(!child.catalogReadReturned)return fail(fd.sourcePC,"Original catalog read remains bounded");
    if(!child.configFile->close131040(&catalog,returned,false,fd))return fail(fd.sourcePC,"Original catalog close remains bounded");
    child.catalogCloseReturned=true; // No Config-style flush here.
    for(std::size_t n=0;;++n){
        if(n>=catalog.textExtent||!catalog.textKnown[n])return fail(0x29871c,"Original lowercase would consume unknown catalog text");
        const auto c=catalog.text18[n];if(!c)break;
        if(c>127u)return fail(0x29871c,"Original non-ASCII lowercase is not joined");
        if(c>='A'&&c<='Z')catalog.text18[n]=std::uint8_t(c+32);child.loweredBytes=n+1;
    }
    catalog.count10=0;bool inToken=false; //298730.
    for(std::size_t n=0;n<size;++n){
        if(!catalog.textKnown[n])return fail(0x298748,"Original token scan would consume unknown text");
        const auto c=catalog.text18[n];
        if(c==' '||c=='\r'||c=='\n'){catalog.text18[n]=0;inToken=false;}
        else if(!inToken){++catalog.count10;inToken=true;}
        child.countScanBytes=n+1;
    }
    catalog.count10+=catalog.retainedCount24; //2987CC; fresh source constructor supplied0.
    if(catalog.count10>(audioDirectory?267u:1743u))return fail(0x2987dc,"Original catalog row allocation outside cohort");
    try {catalog.rows1C.reset(new Catalog::Row[catalog.count10]);}
    catch(...){return fail(0x2987dc,"Native catalog rows unavailable; source array-new return unknown");}
    catalog.rowsExtent=catalog.count10; //2987E4; native row size is not a guest address/stride.
    inToken=false;
    for(std::size_t n=0;n<size;++n){
        const auto c=catalog.text18[n];
        if(!c)inToken=false;
        else if(!inToken){
            if(catalog.rowsWritten>=catalog.rowsExtent)return fail(0x29888c,"Original emitted row exceeds native extent");
            auto& row=catalog.rows1C[catalog.rowsWritten++];inToken=true;
            row.payload04=nullptr;row.payloadKnown=true; //29888C.
            row.name00=reinterpret_cast<const char*>(catalog.text18.get()+n);row.nameKnown=true;row.nameOffset=n; //2988A0.
            row.references08=0;row.referencesKnown=true; //2988B0.
        }
        child.rowScanBytes=n+1;
    }
    catalog.capacity14=catalog.count10;child.catalogReturn=catalog.count10; //2988DC/E0; returns to the retained caller.
    child.pc=0x2988e0;return true;
}
bool OriginalStreamLibrarySource50LA::begin268690(std::shared_ptr<Resolver> resolver,Diagnostic& d)noexcept {
    d={};
    // No current caller exists. Merely owning an initialized native interface
    // cannot enter this private body; the future17CCF0 owner must issue it.
    if(thread_!=std::this_thread::get_id()||nativeClosed_||attempted_||!aural_||!issuer_||
       !aural_->sourceStreamLibraryIdentityCurrent(*this,resolver.get()))
        return stop(d,0x268690,"Authentic same-Aural startup library occurrence unavailable");
    if(!audioDirectoryLoad_||!audioDirectory98A4_||!audioDirectoryLoad_->source232Return.has_value()||
       !audioDirectoryLoad_->nativeFileRetired||audioDirectoryLoad_->configFile)
        return stop(d,0x268690,"Actual earlier232EA0 catalog return unavailable");
    const auto previousResolver=audioDirectoryLoad_->resolverIdentity.lock();
    if(!previousResolver||previousResolver.get()!=resolver.get()||previousResolver.owner_before(resolver)||
       resolver.owner_before(previousResolver))return stop(d,0x268690,"Earlier AudioStr resolver identity changed");
    attempted_=true;
    try {load_.reset(new Storage);}
    catch(...){return stop(d,0x2686ac,"Native automatic Config backing unavailable");}
    auto& child=*load_;child.resolverIdentity=resolver;child.resolver=std::move(resolver);
    const auto fail=[&](std::uint32_t pc,const char* why)noexcept {child.pc=pc;return stop(d,pc,why);};
    child.config190.emplace(); //2686AC actual automatic157320.
    child.pc=0x2686b8;
    if(!format308C90(false,0,d))return false; // Earlier AudioStr status warning owns the first genuine stdio initialization.
    constexpr char extension[]=".txt";
    std::memcpy(child.filename80.data()+13,extension,sizeof(extension)); //2686DC, actual final dot.
    if(catalog98A0_)return fail(0x268710,"Original replacement catalog destruction is not joined");
    std::unique_ptr<Catalog> next;
    try {next.reset(new Catalog);}
    catch(...){return fail(0x26871c,"Native catalog backing unavailable; source allocation return unknown");}
    catalog98A0_=std::move(next);child.catalogPublished=true; //26873C BEFORE virtual18 load.
    if(!loadCatalog2985B0(*catalog98A0_,child,false,
            std::string_view(child.filename80.data(),child.filenameKnown-1u),d))return false;
    child.pc=0x268768;
    return openConfig1577D0(d);
}

class OriginalStreamLibrarySource50LA::Lookup final {
    friend class OriginalStreamLibrarySource50LA;
    const OriginalStreamLibrarySource50LA* owner=nullptr;
    const Catalog* catalog=nullptr;char* filename=nullptr;std::uint8_t* output=nullptr;
    std::uint32_t pc=0x2684e0;
    std::size_t strlenResult=0;bool strlenReturned=false,trimmed=false;
    struct Visit {std::int32_t index=0;bool nameReturned=false;
        std::size_t comparedBytes=0;std::optional<std::int32_t> comparison;};
    std::array<Visit,1743> visits;std::size_t visited=0;
    std::optional<std::int32_t> findReturn;
    bool outputWritten=false,assertionReturned=false;
    std::optional<std::uint32_t> sourceReturn;
    Lookup()noexcept {} //Only the concrete source method creates a call frame.
public:
    ~Lookup()noexcept=default;
};
bool OriginalStreamLibrarySource50LA::lookup2684E0(std::shared_ptr<Lookup>& retained,
    char* filename,std::size_t known,std::size_t extent,std::uint8_t* output,
    std::optional<std::int32_t>& outputKnown,Diagnostic& d)noexcept {
    d={};
    //DFC=1 is stored BEFORE Config destruction. It is not sufficient evidence
    //that268690 returned. No current startup issuer can produce this predicate.
    if(thread_!=std::this_thread::get_id()||nativeClosed_||retained||!aural_||!load_||
       !load_->libraryReturn||*load_->libraryReturn!=1u||!load_->destruction.loadedStoreReached||
       !audioDirectoryLoad_||!audioDirectoryLoad_->source232Return||!audioDirectory98A4_||
       !load_->destruction.returned||
       !load_->destruction.automaticLifetimeEnded||!load_->destruction.nativeFileRetired||
       load_->configFile||load_->resolver||loadedDFC_!=1u||!catalog98A0_)
        return stop(d,0x2684e0,"Actual same-backend returned268690 occurrence unavailable");
    const auto resolver=load_->resolverIdentity.lock();
    if(!resolver||!aural_->sourceStreamLibraryOwnerCurrent(*this,resolver.get()))
        return stop(d,0x2684e0,"Original Stream Library owner lifetime changed");
    const auto currentResolver=issuer_->standardMaterialDomainOwner_->resolver3E1A70();
    if(currentResolver.owner_before(resolver)||resolver.owner_before(currentResolver))
        return stop(d,0x2684e0,"Original Stream Library resolver shared custody changed");
    try {retained=std::shared_ptr<Lookup>(new Lookup);}
    catch(...){return stop(d,0x2684e0,"Native lookup receipt unavailable before source call");}
    auto& call=*retained;call.owner=this;call.catalog=catalog98A0_.get();
    call.filename=filename;call.output=output;
    const auto fail=[&](std::uint32_t pc,const char* why)noexcept {call.pc=pc;return stop(d,pc,why);};
    call.pc=0x2684f0; //Actual owned loaded flag; never a native readiness query.
    if(!output)return fail(0x268594,"Original null-output assertion branch is not joined");
    //The sole current caller supplies a freshly allocated264-byte playlist
    //entry. Its260-byte name and final4-byte index never alias each other or
    //this backend/catalog/process state. Caller separately proves its live row.
    if(!filename||extent!=0x104u||!known||known>extent||outputKnown||
       output!=reinterpret_cast<std::uint8_t*>(filename)+extent)
        return fail(0x2684e0,"Original music entry name/index custody is not the admitted disjoint layout");
    call.pc=0x266c6c;
    if(!filename||!known||known>extent)return fail(call.pc,"Original strlen input ownership unavailable");
    std::size_t length=0;
    while(length<known&&filename[length])++length;
    if(length==known)return fail(0x3093a8,"Original strlen would consume unknown filename bytes");
    call.strlenResult=length;call.strlenReturned=true;
    if(!length)return fail(0x266c78,"Original empty filename would read before its owned extent");
    //Only ONE terminal LF or CR is stripped, on the actual playlist string.
    if(filename[length-1u]=='\n'||filename[length-1u]=='\r'){
        filename[length-1u]=0;call.trimmed=true; //266CA0, before the secondDFC read.
    }
    call.pc=0x266ca4;
    auto& catalog=*catalog98A0_;
    if(loadedDFC_!=1u||catalog.dispatch08!=0x3d3690u||!catalog.rows1C||
       !catalog.text18||!catalog.textKnown||catalog.count10>1743u||
       catalog.count10!=catalog.rowsExtent||catalog.capacity14!=catalog.count10)
        return fail(0x266cf8,"Actual Stream catalog extent or source loaded state changed");
    using G=assets::OriginalConfigProcessGlobals50LA;
    const G::StreamLookupAccess key;
    std::int32_t found=-1;
    for(std::int32_t index=0;index<static_cast<std::int32_t>(catalog.count10);++index){
        auto& visit=call.visits[call.visited++];visit.index=index;
        //298D90 asserts only index<0 OR count<index, not index==count.
        //This caller's own signed loop condition keeps index strictly belowcount.
        const auto& row=catalog.rows1C[std::size_t(index)];
        if(!row.nameKnown||!row.nameOffset||*row.nameOffset>=catalog.textExtent||
           row.name00!=reinterpret_cast<const char*>(catalog.text18.get()+*row.nameOffset))
            return fail(0x298e4c,"Original row-name pointer is not owned known text");
        visit.nameReturned=true;call.pc=0x266cd0;
        std::size_t n=0;std::int32_t compared=0;
        for(;;){
            const auto offset=*row.nameOffset;
            if(n>=catalog.textExtent-offset||!catalog.textKnown[offset+n])
                return fail(n?0x308f60:0x308f08,"Original catalog comparison would consume unknown text");
            const auto a=catalog.text18[offset+n];
            if(n>=known)return fail(a?0x308f18:0x308f80,"Original comparison would consume unknown filename bytes");
            const auto b=static_cast<unsigned char>(filename[n]);
            std::int32_t foldedA=0,foldedB=0;std::uint32_t pc=a?0x308f34:0x308f90;
            if(!G::streamLookupFold308F00(key,a,foldedA,pc))return fail(pc,"Original ASCII ctype ownership unavailable");
            pc=a?0x308f3c:0x308f94;
            if(!G::streamLookupFold308F00(key,b,foldedB,pc))return fail(pc,"Original ASCII ctype ownership unavailable");
            visit.comparedBytes=n+1u;
            if(!a||foldedA!=foldedB){compared=foldedA-foldedB;break;}
            ++n;
        }
        visit.comparison=compared;
        if(!compared){found=index;break;} //First matching row only.
    }
    call.findReturn=found; //266CE4 or266D10, restored frame returns266D98.
    const auto bits=static_cast<std::uint32_t>(found);
    for(std::size_t i=0;i<4;++i)output[i]=std::uint8_t(bits>>(i*8u)); //268510.
    outputKnown=found;call.outputWritten=true;
    std::uint32_t pc=0x268514;
    if(!G::streamLookupIndexAssertion268514(key,found,pc))
        return fail(pc,"Original Stream index assertion remains bounded after output store");
    call.assertionReturned=true;call.sourceReturn=1u;call.pc=0x268680;
    d={call.pc,nullptr};return true; //Original caller ignores this1; output is independently retained.
}

bool OriginalStreamLibrarySource50LA::format308C90(bool song,std::uint32_t ordinal,Diagnostic& d)noexcept {
    auto& child=*load_;using G=assets::OriginalConfigProcessGlobals50LA;
    const auto stop=[&](std::uint32_t pc,const char* why)noexcept {
        child.pc=pc;return OriginalStreamLibrarySource50LA::stop(d,pc,why);};
    if(song?(child.pc!=0x268860||!ordinal||ordinal>1743u):(child.pc!=0x2686b8||ordinal))
        return stop(child.pc,"Original fixed formatter occurrence unavailable");
    auto& frame=song?child.songFormats[ordinal-1u]:child.filenameFormat;
    if(frame.attempted)return stop(0x308c90,"Original formatter occurrence already retained");
    frame.attempted=true;
    char* destination=song?child.songSection40.data():child.filename80.data();
    const auto extent=song?child.songSection40.size():child.filename80.size();
    const G::StreamSprintfAccess access;
    G::StdioIdentity captured;std::uint32_t pc=0x308c98;
    if(!G::readStreamReent308C98(access,captured,pc))return stop(pc,"Original formatter reentrancy read remains bounded");
    frame.capturedReent=captured;
    frame.flags0C=0x208u; //308CD0. Other temporary FILE fields remain unknown.
    frame.buffer10=destination; //308CD4.
    frame.bufferSize14=0x7fffffffu; //308CD8.
    frame.fileReent54=captured; //308CDC.
    frame.cursor00=destination; //308CE0.
    frame.remaining08=0x7fffffffu; //308CE8 call delay slot BEFORE30B4F0.
    if(!G::initializeStreamSprintf30B528(access,*frame.fileReent54,pc))
        return stop(pc,"Original formatter stdio initialization remains bounded");
    frame.initializerReturned=true;
    char unusedDecimal;
    if(!G::streamLocale305328(access,unusedDecimal,pc))return stop(pc,"Original fixed locale pointer read remains bounded");
    frame.localeReturned=true; // No decimal byte is consumed by either admitted format.
    const auto scan=[&](std::size_t offset,unsigned char byte)noexcept {
        if(frame.scanCount==frame.scans.size())return stop(0x30b658,"Native format scan receipt capacity exceeded");
        auto& receipt=frame.scans[frame.scanCount++];receipt.offset=offset;
        std::uint32_t wide=0;std::int32_t length=0;
        if(!G::streamByte306108(access,byte,wide,length,pc))return stop(pc,"Original format byte scan remains bounded");
        receipt.length=length;if(length>=0)receipt.wide=wide;
        if(length!=(byte?1:0))return stop(0x30b674,"Original formatter scan outside admitted single-byte branch");
        return true;
    };
    const auto copySegment=[&](const char* text,std::size_t bytes)noexcept {
        if(frame.segmentCount==frame.copiedSegments.size()||!frame.remaining08||
           !frame.cursor00||!frame.flags0C||*frame.flags0C!=0x208u||
           bytes>*frame.remaining08||frame.copied>=extent||bytes>=extent-frame.copied)
            return stop(0x304f40,"Original string-FILE copy exceeds retained destination");
        //304F28 string flag bypasses all FILE callbacks and buffering.306370
        //copies this known segment; source count/cursor stores follow in order.
        std::memcpy(*frame.cursor00,text,bytes);
        *frame.remaining08-=static_cast<std::uint32_t>(bytes); //304F58.
        *frame.cursor00+=bytes; //304F5C.
        frame.copiedSegments[frame.segmentCount++]=bytes;frame.copied+=bytes;return true;
    };
    if(!song){
        constexpr char literal[]="audio\\streams.juk";
        for(std::size_t n=0;n<sizeof(literal);++n)if(!scan(n,static_cast<unsigned char>(literal[n])))return false;
        //No '%' in the actual literal. Final30CA24 flush contains one segment.
        if(!copySegment(literal,sizeof(literal)-1u))return false;
    }else{
        constexpr char format[]="Song %d";
        for(std::size_t n=0;n<=5u;++n)if(!scan(n,static_cast<unsigned char>(format[n])))return false;
        //30B718 reads immutable 'd' directly (not306108); fixed positive signed
        //argument1..1743 selects decimal quotient/remainder, no flags or width.
        std::array<char,4> reversed,digits;std::size_t count=0;auto value=ordinal;
        do{reversed[count++]=char('0'+value%10u);value/=10u;}while(value);
        for(std::size_t n=0;n<count;++n)digits[n]=reversed[count-1u-n];
        //30C9FC flushes literal and numeric segments BEFORE scanning next NUL.
        if(!copySegment(format,5u)||!copySegment(digits.data(),count))return false;
        if(!scan(7u,0))return false;
    }
    frame.formatterReturn=static_cast<std::uint32_t>(frame.copied); //30CA40/4C; flag0x40 remains clear.
    **frame.cursor00=0;frame.terminated=true; //308CF4, no cursor/count update.
    frame.sourceReturn=frame.formatterReturn;
    if(song){child.songSectionKnown=frame.copied+1u;child.pc=0x268868;}
    else {child.filenameKnown=frame.copied+1u;child.pc=0x2686c0;}
    return true;
}

bool OriginalStreamLibrarySource50LA::openConfig1577D0(Diagnostic& d)noexcept {
    auto& child=*load_;auto& config=*child.config190;auto& fd=child.configFileDiagnostic;
    const auto stop=[&](std::uint32_t pc,const char* why)noexcept {
        child.pc=pc;return OriginalStreamLibrarySource50LA::stop(d,pc,why);};
    if(child.pc!=0x268768||!child.catalogReturn||!child.catalogCloseReturned||
       !child.configFile||!aural_->sourceStreamLibraryIdentityCurrent(*this,child.resolver.get()))
        return stop(0x268768,"Returned catalog occurrence unavailable");
    if(config.text1C||config.file00!=-1||config.index4BC||config.block4C4||
       config.names4CC||config.count14||config.opened0C||config.binary08||config.mode10)
        return stop(0x1577e8,"Automatic Config is not its reached fresh constructor");
    // Same root pin, genuine second131230, original unmodified audio\streams.juk.
    std::int64_t returned=-1;
    if(!child.configFile->begin131230(child.resolver,&config,assets::OriginalStreamLibraryFile50LA::Kind::streamParameters,returned,fd))
        return stop(fd.sourcePC,"Original JUK FileManager open remains bounded");
    config.file00=returned; //1578D8, including a genuine131230 signed-1 result.
    const auto failedOpenReturn=[&]()noexcept {
        child.configOpenReturn=0; //157ACC epilogue then268774 fallthrough.
        child.pc=0x268780;return finish268690(0x268780,0,d);};
    if(config.file00==-1)return failedOpenReturn();
    if(!child.configFile->current(&config,config.file00))
        return stop(0x1578f8,"Returned original handle occurrence no longer current");
    config.field42C=0;config.opened0C=1;config.mode10=0; //1578F8/FC/157900.
    const auto read=[&](std::uint32_t call,void* out,std::size_t bytes,std::size_t& known)noexcept {
        if(child.configReadCount==child.configReads.size())
            return stop(call,"Native read receipt capacity exceeded; no source read result");
        auto& receipt=child.configReads[child.configReadCount++];
        receipt.callPC=call;receipt.requested=bytes;
        receipt.returned=child.configFile->read12FB30(&config,config.file00,out,bytes,receipt.copied,fd);
        known=receipt.copied; // Partial bytes remain known even on a bounded native stop.
        if(!receipt.returned)return stop(fd.sourcePC,"Original Stream Library Config read remains bounded");
        return true;
    };
    const auto word=[](const std::array<std::uint8_t,4>& b)noexcept {
        return std::uint32_t(b[0])|(std::uint32_t(b[1])<<8u)|
            (std::uint32_t(b[2])<<16u)|(std::uint32_t(b[3])<<24u);};
    if(!read(0x157914,child.magic48.data(),7,child.magicKnown))return false;
    child.magic48[6]=0;child.magicTerminatorWritten=true; //15792C delay store.
    if(child.magicKnown<6u)return stop(0x157928,"Original strcmp would consume unknown magic bytes");
    if(std::memcmp(child.magic48.data(),"BINARY",6)!=0){
        config.binary08=0; //157A9C, before unimplemented seek/text parser.
        return stop(0x157ab0,"Original nonbinary Config seek/parser branch is not implemented");
    }
    config.binary08=1; //15793C, before reading uninitialized stack version54.
    if(!read(0x157950,child.version54.data(),4,child.versionKnown))return false;
    if(child.versionKnown!=4u)return stop(0x157958,"Original version load would consume unknown bytes");
    const auto closeFailed=[&](std::uint32_t call)noexcept {
        if(!child.configFile->close131040(&config,config.file00,false,fd))
            return stop(fd.sourcePC?fd.sourcePC:call,"Original Config failure close remains bounded");
        config.file00=-1; //15797C or1579C0; opened/binary fields stay reached.
        return failedOpenReturn();
    };
    if(word(child.version54)!=1u)return closeFailed(0x157970);
    if(!read(0x157998,child.sections58.data(),4,child.sectionsKnown))return false;
    if(child.sectionsKnown!=4u)return stop(0x1579a0,"Original section count load would consume unknown bytes");
    const auto sections=word(child.sections58);
    if(!sections)return closeFailed(0x1579b4);
    config.count14=sections; //1579CC. Authenticated corpus has1744, not a substitute read.
    if(sections!=child.nameKnown.size())
        return stop(0x1579dc,"Source section count exceeds this authenticated Config cohort");
    // Typed host allocation adapts original305700 backing. Host failure is a
    // bounded allocation, NOT invented guest NULL and its source close/return0.
    try {child.names4CC.reset(new std::uint8_t[std::size_t(sections)*1024u]);}
    catch(...){return stop(0x1579dc,"Native name storage unavailable; original malloc return unknown");}
    config.names4CC=child.names4CC.get(); //1579E4; no initialization of name tails.
    for(;child.nameOrdinal<sections;++child.nameOrdinal){
        child.length5C.fill(0);child.lengthZeroed=true; //157A18, unlike version/count.
        std::size_t supplied=0;
        if(!read(0x157a30,child.length5C.data(),4,supplied))return false;
        const auto length=word(child.length5C); // Unwritten suffix was explicitly zeroed.
        if(length>1024u)return stop(0x157a4c,"Original name length exceeds safe native slot extent");
        if(!read(0x157a4c,child.names4CC.get()+std::size_t(child.nameOrdinal)*1024u,
                length,child.nameKnown[child.nameOrdinal]))return false;
        //157A58 and157A60 advance ordinal and1024-byte destination even on a
        //returned short read. No invented terminator or initialized tail.
    }
    config.indexBytes4C0=sections*8u; //157A68.
    try {child.index4BC.reset(new std::uint8_t[config.indexBytes4C0]);}
    catch(...){return stop(0x157a6c,"Native index storage unavailable; original malloc return unknown");}
    config.index4BC=child.index4BC.get(); //157A74. No guest NULL-result claim.
    if(!read(0x157a8c,child.index4BC.get(),config.indexBytes4C0,child.indexKnown))return false;
    // Original157A94/98 returns1 regardless of the returned read byte count.
    // Keep that source result separate from which index bytes are actually known.
    child.configOpenReturn=1;
    child.pc=0x268790;
    return metadata268790(d);
}

bool OriginalStreamLibrarySource50LA::metadata268790(Diagnostic& d)noexcept {
    auto& child=*load_;auto& config=*child.config190;auto& fd=child.configFileDiagnostic;
    const auto stop=[&](std::uint32_t pc,const char* why)noexcept {
        child.pc=pc;return OriginalStreamLibrarySource50LA::stop(d,pc,why);};
    if(child.pc!=0x268790||child.configOpenReturn!=1u||!child.catalogReturn||
       !child.configFile||!child.configFile->current(&config,config.file00))
        return stop(0x268790,"Returned original JUK Config occurrence unavailable");
    const auto le=[](const std::uint8_t* b)noexcept {
        return std::uint32_t(b[0])|(std::uint32_t(b[1])<<8u)|
            (std::uint32_t(b[2])<<16u)|(std::uint32_t(b[3])<<24u);};
    const auto lower=[](char c)noexcept {return c>='A'&&c<='Z'?char(c+('a'-'A')):c;};
    const auto key=[&](auto& lookup,std::string_view requested,std::uint32_t pc)noexcept {
        if(requested.size()>=lookup.lowerKey.size())return stop(pc,"Original lookup strcpy exceeds native extent");
        for(std::size_t i=0;i<requested.size();++i){
            if(static_cast<unsigned char>(requested[i])>127u)return stop(pc,"Non-ASCII original lowercase path is not joined");
            lookup.lowerKey[i]=lower(requested[i]);
        }
        lookup.lowerKey[requested.size()]=0;lookup.lowerKnown=requested.size()+1;return true;
    };
    // Actual midpoint/low/high algorithm, including the terminal duplicate
    // low comparison. Each comparator consumes only reached known bytes.
    const auto find=[&](auto& lookup,std::uint32_t count,auto&& compare,const std::array<std::uint32_t,3>& pcs,bool single)noexcept {
        const auto pc=pcs[0];
        if(!count||count>1744u)return stop(pc,"Original binary search index is outside this retained cohort");
        const auto visit=[&](std::int32_t index,int& cmp,std::uint8_t arm)noexcept {
            lookup.comparisonPC=pcs[arm?arm-1u:0u];
            if(lookup.visitedCount==lookup.visited.size())return stop(lookup.comparisonPC,"Native comparison receipt capacity exceeded");
            lookup.visited[lookup.visitedCount++]=index;return compare(index,cmp);
        };
        std::int32_t low=0,high=static_cast<std::int32_t>(count-1u),mid=high>>1;int cmp=0;
        if(single){if(!visit(0,cmp,0))return false;lookup.result=cmp? -1:0;lookup.matchArm=0;return true;}
        for(;;){
            if(!visit(mid,cmp,1))return false;
            if(!cmp){lookup.result=mid;lookup.matchArm=1;return true;}
            if(cmp<0)high=mid;else low=mid;
            const auto gap=high-low;mid=low+(gap>>1);if(gap<2)break;
        }
        if(!visit(low,cmp,2))return false;
        if(!cmp){lookup.result=low;lookup.matchArm=2;return true;}
        if(!visit(high,cmp,3))return false;
        lookup.result=cmp? -1:high;if(!cmp)lookup.matchArm=3;return true;
    };
    const auto read=[&](std::uint32_t pc,void* target,std::size_t bytes,std::size_t& copied)noexcept {
        if(child.configReadCount==child.configReads.size())return stop(pc,"Native Config read receipt capacity exceeded");
        auto& receipt=child.configReads[child.configReadCount++];receipt.callPC=pc;receipt.requested=bytes;
        receipt.returned=child.configFile->read12FB30(&config,config.file00,target,bytes,receipt.copied,fd);
        copied=receipt.copied;
        if(!receipt.returned)return stop(fd.sourcePC,"Original selected Config read remains bounded");
        return true;
    };
    const auto select=[&](std::size_t ordinal,std::string_view requested)noexcept {
        auto& state=child.selections[ordinal];auto& lookup=state.lookup;
        if(!config.opened0C)return stop(0x157d2c,"Original unopened Config assertion path is not joined");
        if(config.mode10)return stop(0x157db4,"Original writable Config flush path is not joined");
        if(config.binary08!=1u)return stop(0x157b10,"Original nonbinary selection is not joined");
        if(!config.names4CC){lookup.result=-1;state.returned=false;return true;} //157B24/157DD8.
        if(config.names4CC!=child.names4CC.get()||config.count14!=1744u)
            return stop(0x157b60,"Original section-name ownership changed");
        if(!key(lookup,requested,0x157b30))return false;
        const auto compare=[&](std::int32_t index,int& cmp)noexcept {
            if(index<0||std::size_t(index)>=child.nameKnown.size())return stop(lookup.comparisonPC,"Original section index is not represented");
            const auto* name=child.names4CC.get()+std::size_t(index)*1024u;
            for(std::size_t at=0;at<lookup.lowerKnown;++at){
                if(at>=child.nameKnown[index])return stop(lookup.comparisonPC,"Original section strcmp would consume unknown bytes");
                const auto a=static_cast<unsigned char>(lookup.lowerKey[at]);const auto b=name[at];
                if(a!=b){cmp=a<b?-1:1;return true;}if(!a){cmp=0;return true;}
            }
            return stop(lookup.comparisonPC,"Original section strcmp extent unavailable");
        };
        if(!find(lookup,config.count14,compare,{0x157b8c,0x157be8,0x157c1c},false))return false;
        if(*lookup.result<0){state.returned=false;return true;} // Existing selection is not cleared.
        const auto length=std::min<std::size_t>(requested.size(),1023u);
        std::memcpy(config.name28.data(),requested.data(),length);config.name28[length]=0;child.configNameKnown=length+1;
        const auto index=std::size_t(*lookup.result)*8u;
        if(config.index4BC!=child.index4BC.get()||index+4u>child.indexKnown)
            return stop(0x157eb0,"Original section offset would consume unknown bytes");
        state.offset=le(child.index4BC.get()+index);
        if(index+8u>child.indexKnown)return stop(0x157eb8,"Original section length would consume unknown bytes");
        state.length=le(child.index4BC.get()+index+4u);
        if(!*state.length||*state.length>1024u)return stop(config.block4C4?0x157ebc:0x157ee4,
            "Section payload exceeds native cohort; no capacity store reached");
        if(!config.block4C4||config.field4C8<*state.length){
            const bool replacing=config.block4C4!=nullptr;
            if(replacing&&(config.block4C4!=child.block4C4.get()||!child.blockKnown))
                return stop(0x157ecc,"Original selected-block ownership changed");
            config.field4C8=*state.length; //157ECC or157EE4, BEFORE allocator call.
            std::unique_ptr<std::uint8_t[]> next,known;
            try {next.reset(new std::uint8_t[config.field4C8]);known.reset(new std::uint8_t[config.field4C8]);}
            catch(...){return stop(replacing?0x157ed4:0x157ee8,"Native block backing unavailable; source allocation return unknown");}
            std::fill_n(known.get(),config.field4C8,std::uint8_t(0)); // Native knownness only; payload is NOT cleared.
            if(replacing){const auto kept=std::min<std::size_t>(child.blockCapacity,config.field4C8);
                // Only copy initialized payload bytes; source unknown bytes stay
                // unknown instead of being read as C++ indeterminate values.
                for(std::size_t n=0;n<kept;++n)if(child.blockKnown[n]){next[n]=child.block4C4[n];known[n]=1;}}
            child.block4C4=std::move(next);child.blockKnown=std::move(known);
            child.blockCapacity=config.field4C8;config.block4C4=child.block4C4.get(); //157EE0/157EF0.
        }
        if(config.block4C4!=child.block4C4.get()||!child.blockKnown||child.blockCapacity<config.field4C8)
            return stop(0x157ef4,"Original selected block native extent unavailable");
        if(!child.configFile->seekSet12F640(&config,config.file00,*state.offset,fd))
            return stop(fd.sourcePC,"Original Config section seek remains bounded");
        state.seekReturned=true;state.fieldCount86C.fill(0);state.countZeroed=true; //157F10.
        std::size_t copied=0;if(!read(0x157f28,state.fieldCount86C.data(),4,copied))return false;
        config.selected20=le(state.fieldCount86C.data()); //157F38, including genuine zero suffix on short read.
        const bool returned=read(0x157f4c,child.block4C4.get(),config.field4C8,copied);
        for(std::size_t n=0;n<copied;++n)child.blockKnown[n]=1; // Keep old known suffix if read was short.
        if(!returned)return false;
        state.returned=true;return true; //157F58 ignores the actual read byte count.
    };
    const auto getString=[&](auto& state,std::string_view requested,char* output,std::size_t extent,std::int32_t limit)noexcept {
        if(config.binary08!=1u)return stop(0x158278,"Original text getter path is not joined");
        if(!config.selected20){output[0]=0;state.produced=1;state.returned=false;return true;} //15828C.
        const bool single=config.selected20==1u;
        if(!key(state.lookup,requested,single?0x1582ac:0x15837c))return false;
        if(!config.block4C4||config.block4C4!=child.block4C4.get()||!child.blockKnown)
            return stop(single?0x1582dc:0x1583e8,"Original field block unavailable");
        const auto compare=[&](std::int32_t index,int& cmp)noexcept {
            if(index<0)return stop(state.lookup.comparisonPC,"Original field index unavailable");
            const auto base=std::size_t(index)*43u;
            for(std::size_t n=0;n<state.lookup.lowerKnown;++n){
                if(base+n>=child.blockCapacity||!child.blockKnown[base+n])
                    return stop(state.lookup.comparisonPC,"Original field strcmp would consume unknown bytes");
                const auto a=static_cast<unsigned char>(state.lookup.lowerKey[n]),b=child.block4C4[base+n];
                if(a!=b){cmp=a<b?-1:1;return true;}if(!a){cmp=0;return true;}
            }
            return stop(state.lookup.comparisonPC,"Original field strcmp extent unavailable");
        };
        if(!find(state.lookup,config.selected20,compare,{single?0x1582e4u:0x1583f8u,0x1584f0,0x1585b0},single))return false;
        if(*state.lookup.result<0){state.returned=false;return true;} // A nonempty-section miss leaves output untouched.
        constexpr std::array<std::uint32_t,4> offsetPC{0x158300,0x158410,0x15850c,0x1585cc};
        constexpr std::array<std::uint32_t,4> strlenPC{0x158314,0x158424,0x158520,0x1585e0};
        constexpr std::array<std::uint32_t,4> limitPC{0x15832c,0x15843c,0x158538,0x1585f8};
        constexpr std::array<std::uint32_t,4> copyPC{0x158354,0x158474,0x158560,0x158620};
        const auto arm=state.lookup.matchArm;
        const auto offsetAt=std::size_t(*state.lookup.result)*43u+35u;
        if(offsetAt+4u>child.blockCapacity)return stop(offsetPC[arm],"Original unaligned value offset exceeds block extent");
        for(std::size_t n=0;n<4;++n)if(!child.blockKnown[offsetAt+n])
            return stop(offsetPC[arm],"Original value offset would consume unknown bytes");
        const auto valueAt=std::size_t(le(child.block4C4.get()+offsetAt));std::size_t bytes=0;
        for(;;++bytes){
            if(valueAt>child.blockCapacity||bytes>=child.blockCapacity-valueAt||!child.blockKnown[valueAt+bytes])
                return stop(strlenPC[arm],"Original value strlen would consume unknown/out-of-extent bytes");
            if(!child.block4C4[valueAt+bytes])break;
        }
        if(limit!=-1){if(limit<=0)return stop(limitPC[arm],"Original getter limit is outside the admitted positive cohort");
            bytes=std::min<std::size_t>(bytes,std::size_t(limit-1));}
        if(bytes>=extent)return stop(copyPC[arm],"Original getter copy exceeds native destination extent");
        std::memcpy(output,child.block4C4.get()+valueAt,bytes);output[bytes]=0;
        state.produced=bytes+1;state.returned=true;return true;
    };

    // Integer wrapper158B30 calls158920's stack20 getter. Reuse that
    // exact scratch across count/Pitch calls; misses still write default0.
    const auto getInteger=[&](auto& getter,std::string_view requested,
            std::optional<std::uint32_t>& output,std::optional<bool>& returned)noexcept {
        if(!getString(getter,requested,child.numberStack20.data(),child.numberStack20.size(),-1))return false;
        if(!*getter.returned){output=0;returned=false;return true;} //158B60.
        const auto number=std::string_view(child.numberStack20.data(),getter.produced-1u);
        std::uint32_t converted=0;
        if(!number.empty()&&lower(number[0])=='t')converted=1;
        else if(!number.empty()&&lower(number[0])=='f')converted=0;
        else if(number.size()==2u&&lower(number[0])=='o'&&lower(number[1])=='n')converted=1;
        else if(number.size()==3u&&lower(number[0])=='o'&&lower(number[1])=='f'&&lower(number[2])=='f')converted=0;
        else if(!number.empty()&&lower(number[0])=='y')converted=1;
        else if(!number.empty()&&lower(number[0])=='n')converted=0;
        else{
            if(!assets::OriginalConfigProcessGlobals50LA::reentrancyCurrentNative())
                return stop(0x30ae84,"Original integer process reentrancy custody unavailable");
            if(number.empty()||number.size()>10u||number[0]=='0')return stop(0x158a34,"Original integer conversion outside admitted decimal subset");
            std::uint64_t value=0;
            for(const char c:number){if(c<'0'||c>'9')return stop(0x158a34,"Original integer conversion outside admitted decimal subset");value=value*10u+std::uint32_t(c-'0');}
            converted=static_cast<std::uint32_t>(value);
        }
        output=converted;returned=true;return true; //158A44/158B58.
    };
    const auto getDuration=[&](auto& song)noexcept {
        if(!getString(song.duration,"Duration",child.durationStack30.data(),child.durationStack30.size(),-1))return false; //158B9C.
        if(!*song.duration.returned){child.durationBits66C=0;song.durationBits=0;song.durationReturned=false;return true;} //158BDC.
        //302E10->30AB18 reads actual38544C before309C18. Do not reset or
        // substitute the shared process owner when this library is created.
        if(!assets::OriginalConfigProcessGlobals50LA::reentrancyCurrentNative())
            return stop(0x30ab28,"Original decimal process reentrancy custody unavailable");
        const auto text=std::string_view(child.durationStack30.data(),song.duration.produced-1u);
        // Authenticated Duration cohort: positive unsigned fixed decimals,
        // <=9 digits, exactly6 fractional digits. No locale/ctype/callback or
        // Bigint allocation occurs on this309C18 fast path. Unsupported syntax
        // stops before a conversion return, never supplies guessed zero.
        std::uint64_t n=0,den=1;std::size_t digits=0,fractional=0;bool dot=false;
        if(text.empty()||text.size()>10u)return stop(0x309c18,"Original Duration outside admitted decimal cohort");
        for(const char c:text){
            if(c=='.'&&!dot&&digits){dot=true;continue;}
            if(c<'0'||c>'9'||++digits>9u)return stop(0x309c18,"Original Duration outside admitted decimal cohort");
            n=n*10u+std::uint32_t(c-'0');if(dot){++fractional;den*=10u;}
        }
        if(!dot||fractional!=6u||!n)return stop(0x309c18,"Original Duration outside admitted positive cohort");
        //3101E8 produces an exact binary64 integer.30A1A4 divides by exact
        //3C6558 table10^k using30FF18. Integer long division below preserves
        //normal binary64 nearest-even, without host FP/locale or direct float.
        int exponent=0;
        while(n<den){n*=2u;--exponent;}
        while(n>=den*2u){den*=2u;++exponent;}
        std::uint64_t remainder=n-den,significand=1;
        for(unsigned i=0;i<52u;++i){remainder*=2u;significand*=2u;
            if(remainder>=den){++significand;remainder-=den;}}
        if(remainder*2u>den||(remainder*2u==den&&(significand&1u)))++significand;
        if(significand==(std::uint64_t(1)<<53u)){significand>>=1u;++exponent;}
        song.durationDouble=(std::uint64_t(exponent+1023)<<52u)|
            (significand&((std::uint64_t(1)<<52u)-1u)); //302E10 returned soft-double bits.
        //310440->30F8D0->311008->310498 sticky compression and exact normal
        //nearest-even rounding. All admitted positive numbers remain normal.
        std::uint32_t m=static_cast<std::uint32_t>(significand>>22u);
        if(significand&((std::uint64_t(1)<<22u)-1u))m|=1u;
        if((m&127u)==64u){if(m&128u)m+=64u;}else m+=63u;
        if(m&0x80000000u){m>>=1u;++exponent;}
        child.durationBits66C=(std::uint32_t(exponent+127)<<23u)|((m>>7u)&0x7fffffu); //158BC4.
        song.durationBits=child.durationBits66C;song.durationReturned=true;return true;
    };
    if(!select(0,"Songs"))return false;
    if(!*child.selections[0].returned){child.pc=0x2688f0;return finish268690(0x2688f0,0,d);}
    if(!getInteger(child.numberGetter,"NumberOfSongs",child.count664,child.numberOfSongsReturned))return false;
    child.songOrdinal=1; //2687C8 delay slot, including the mismatch arm.
    if(*child.count664!=*child.catalogReturn)
        return stop(0x2687cc,"Original catalog/config count assertion path is not joined");
    if(*child.count664!=1743u)
        return stop(0x2688d4,"Original per-song loop outside authenticated catalog cohort");
    auto& catalog=*catalog98A0_;
    for(;*child.songOrdinal<=*child.count664;++*child.songOrdinal){
        const auto ordinal=*child.songOrdinal;auto& song=child.songs[ordinal-1u];
        try {child.pendingPayload8.reset(new std::array<std::uint8_t,8>);}
        catch(...){return stop(0x268848,"Native song payload backing unavailable; source allocation return unknown");}
        child.pc=0x268860;
        if(!format308C90(true,ordinal,d))return false;
        song.formatted=true;
        if(!select(ordinal,std::string_view(child.songSection40.data(),child.songSectionKnown-1u)))return false;
        //26886C ignores selection boolean, retaining previous selection on miss.
        if(!getDuration(song))return false;
        auto& payload=*child.pendingPayload8;
        const auto put=[&](std::size_t at,std::uint32_t word)noexcept {
            for(unsigned n=0;n<4u;++n)payload[at+n]=std::uint8_t(word>>(8u*n));};
        put(4u,*child.durationBits66C);song.payloadKnownMask=0xf0u; //2688A8 Pitch call delay slot BEFORE getter.
        if(!getInteger(song.pitch,"Pitch",child.pitch668,song.pitchReturned))return false;
        song.pitchValue=child.pitch668;put(0u,*child.pitch668);song.payloadKnownMask=0xffu; //2688BC.
        const auto index=ordinal-1u;
        //298F80's actual diagnostic predicate is signed index<0 OR count<index,
        //not index>=count. Valid current ordinals skip its lazy assertion.
        if(index>catalog.count10)return stop(0x298fb4,"Original catalog association assertion is not joined");
        if(index>=catalog.rowsExtent||!catalog.rows1C)return stop(0x299038,"Original catalog association exceeds retained native rows");
        auto& row=catalog.rows1C[index];
        if(!row.payloadKnown||row.payload04||row.payloadBacking)
            return stop(0x299038,"Original payload overwrite requires separate retained ownership");
        row.payload04=child.pendingPayload8.get();row.payloadKnown=true; //299038; no name search/refcount change.
        row.payloadBacking=std::move(child.pendingPayload8);song.associated=true;++child.associatedSongs;
    }
    child.songOrdinal=1; //2688E4 branch delay slot resets source s1, not a loader return.
    child.pc=0x26890c;return finish268690(0x26890c,1,d);
}
bool OriginalStreamLibrarySource50LA::finish268690(std::uint32_t callPC,std::uint32_t sourceResult,Diagnostic& d)noexcept {
    auto& child=*load_;auto& config=*child.config190;auto& retired=child.destruction;
    const auto stop=[&](std::uint32_t pc,const char* why)noexcept {
        child.pc=pc;return OriginalStreamLibrarySource50LA::stop(d,pc,why);};
    if(retired.entered||child.libraryReturn||child.pc!=callPC||!child.configFile||
       (sourceResult==1u?(callPC!=0x26890cu||child.associatedSongs!=1743u||child.songOrdinal!=1u):
        (sourceResult!=0u||(callPC!=0x268780u&&callPC!=0x2688f0u))))
        return stop(callPC,"Original Config destruction occurrence unavailable");
    if((callPC==0x268780u&&child.configOpenReturn!=0u)||
       (callPC==0x2688f0u&&(!child.selections[0].returned||*child.selections[0].returned)))
        return stop(callPC,"Original failure-return branch custody unavailable");
    retired.flag=-1;
    //The success call's delay slot executes BEFORE the destructor body. A
    //later bound must retain this store, not turn it into successful return.
    if(sourceResult==1u){loadedDFC_=1;retired.loadedStoreReached=true;child.pc=0x268910;}
    retired.entered=true;child.pc=0x1573b0;
    auto& fd=child.configFileDiagnostic;
    if(config.file00!=-1){
        retired.flushEntered=true;std::int32_t returned=0;
        if(!child.configFile->flush12F290(&config,config.file00,returned,fd))
            return stop(fd.sourcePC,"Original Config destructor flush remains bounded");
        retired.flushReturn=returned; //Separate call receipt retained even if close later bounds.
        retired.closeEntered=true;
        if(!child.configFile->close131040(&config,config.file00,false,fd))
            return stop(fd.sourcePC,"Original Config destructor close remains bounded");
        retired.closeReturned=true;config.file00=-1; //157404 ONLY after both calls returned.
    }
    //15740C->157CB0: binary fresh Config owns no selected-text child.
    if(config.child24)return stop(0x157cc8,"Original selected-text child destruction is outside this binary cohort");
    config.selected20=0;config.child24=nullptr;retired.selectedClearReturned=true; //157CE4/E8.
    if(config.text1C)return stop(0x157420,"Original text allocation destruction is outside this binary cohort");
    //A null text1C deliberately leaves count14 unchanged. Binary lookups never
    //populate the text cache; do not clear guessed cache pointer/type fields.
    if(config.cacheCount438)return stop(0x15743c,"Original populated text-cache destruction is not joined");
    config.cacheCount438=0; //157478.
    const auto release=[&](const void* pointer,std::unique_ptr<std::uint8_t[]>& backing,
            auto& receipt,std::uint32_t pc)noexcept {
        receipt.identity=pointer;receipt.callPC=pc;receipt.entered=true;
        if(!pointer||pointer!=backing.get())return stop(pc,"Original free lacks its exact native allocation owner");
        if(!assets::OriginalConfigProcessGlobals50LA::reentrancyCurrentNative())
            return stop(0x305734,"Original free process reentrancy custody unavailable");
        //Paired native allocation/free adaptation only. This is not execution
        //of304900's guest heap metadata/free-list or lock implementation.
        backing.reset();receipt.returned=true;return true;
    };
    if(config.names4CC){
        if(!release(config.names4CC,child.names4CC,retired.names,0x157488))return false;
        config.names4CC=nullptr; //157490 after free returned.
    }
    if(config.index4BC){
        if(!release(config.index4BC,child.index4BC,retired.index,0x1574a0))return false;
        config.index4BC=nullptr; //1574A8; indexBytes4C0 intentionally unchanged.
    }
    if(config.block4C4){
        if(!release(config.block4C4,child.block4C4,retired.block,0x1574b8))return false;
        child.blockKnown.reset(); //Native knownness storage is not a source free/store.
        //No original null store to block4C4 or capacity reset follows. Retain
        //the historical dead identity, never dereference it after this point.
    }
    //Signed low16(-1)<=0 skips2E8B00(this).1573B0 returns the same this.
    retired.returnedThis=&config;retired.returned=true;retired.automaticLifetimeEnded=true;
    //Numeric file ownership is now gone. Retire only the native root pin;
    //catalog text/rows/payloads are independent and remain live. No archive,
    //device, decoder window, bank registration or source DFC state is reset.
    if(!child.configFile->closeNative(fd))return stop(0,"Native terminal file-custody release remains bounded");
    retired.nativeFileRetired=true;
    child.configFile.reset();child.resolver.reset(); //Terminal native adapter, not another source close.
    child.libraryReturn=sourceResult;
    child.pc=sourceResult?0x268914u:(callPC==0x268780u?0x26878cu:0x2688fcu);
    d={child.pc,nullptr};return true; //Only this local268690 return; never caller17CCF0 advancement.
}
} //namespace rr::platform
