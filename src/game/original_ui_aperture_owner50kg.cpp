// New reconstruction after rollback; source-only, unverified.
#include "original_ui_aperture_owner50kg.h"
#include <cfenv>
#include <cmath>
#include <limits>
#include <new>
#include <string>
#include <utility>

namespace rr::game {
namespace {
using Status=OriginalUiApertureStatus50KG;
using State=OriginalUiApertureState50KG;
using Phase=OriginalUiTransitionPhase50KG;
using Life=OriginalUiTransitionLifecycle50KG;
using Completion=OriginalUiTransitionCompletion50KG;
using Hook=OriginalUiApertureHookKind50KG;
struct OperationScope {
    bool& active;
    explicit OperationScope(bool& flag) noexcept:active(flag) { active=true; }
    ~OperationScope() { active=false; }
};
Status fail(OriginalUiApertureDiagnostic50KG& d,Status status,
    std::uint32_t pc=0) noexcept { d.status=status;d.sourcePC=pc;return status; }
bool dependencies(const OriginalUiApertureCallbacks50KG& c) noexcept {
    return c.available && c.hook && c.submitOverlay;
}
bool dependenciesAvailable(const OriginalUiApertureCallbacks50KG& c) noexcept {
    return dependencies(c) && c.available(c.context);
}
bool ordinary(float value) noexcept {
    return std::isfinite(value) && (value==0.0f || std::isnormal(value));
}
// Native admitted float arithmetic: individual binary32 operations, no fused
// multiply/add or extended-precision accumulator. Host mode is admitted at
// both bind and render. This is the explicit native bounded arithmetic policy;
// settled CVT.W.S semantics do not imply universal EE multiplication parity.
float div32(float a,float b) noexcept { volatile float r=a/b;return r; }
float mul32(float a,float b) noexcept { volatile float r=a*b;return r; }
float intToFloat32(std::int32_t value) noexcept {
    volatile float r=static_cast<float>(value);return r;
}
bool safeEdges(std::int64_t top,std::int64_t bottom) noexcept {
    return top>=std::numeric_limits<std::int32_t>::min() &&
        top<=std::numeric_limits<std::int32_t>::max() &&
        bottom>=std::numeric_limits<std::int32_t>::min() &&
        bottom<=std::numeric_limits<std::int32_t>::max();
}
OriginalUiApertureRectangle50KG rectangle(std::int32_t top,
    std::int32_t bottom,std::uint32_t objectOffset) noexcept {
    OriginalUiApertureRectangle50KG r;
    r.left=0;r.top=top;r.right=640;r.bottom=bottom;
    r.sourceObjectOffset=objectOffset;
    const float t=intToFloat32(top),b=intToFloat32(bottom);
    r.vertices={{{0.0f,t},{0.0f,b},{640.0f,t},{640.0f,b}}};
    return r;
}
}

bool convertOriginalUiApertureCvtWS50KG(float value,std::int32_t& output) noexcept {
    static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559,
        "Original aperture conversion requires IEEE binary32 input");
    // float(INT32_MAX) would round to +2^31 and make a <= check unsafe.
    // Use the strict upper bound and cast only inside the representable range.
    if (!std::isfinite(value) || value < -2147483648.0f || value >= 2147483648.0f)
        return false;
    output=static_cast<std::int32_t>(value); // Signed truncation; zero stays zero.
    return true;
}

Status OriginalUiApertureOwner50KG::bindMain(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const OriginalUiApertureCallbacks50KG& supplied,
    OriginalUiApertureDiagnostic50KG& d) noexcept {
    return bindPageImpl(std::move(archive),supplied,false,false,1,d);
}
Status OriginalUiApertureOwner50KG::bindMainEntryOnly(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const OriginalUiApertureCallbacks50KG& supplied,
    OriginalUiApertureDiagnostic50KG& d) noexcept {
    return bindPageImpl(std::move(archive),supplied,true,false,1,d);
}
Status OriginalUiApertureOwner50KG::bindOptionsMenu(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const OriginalUiApertureCallbacks50KG& supplied,
    OriginalUiApertureDiagnostic50KG& d) noexcept {
    return bindPageImpl(std::move(archive),supplied,false,true,1,d);
}
Status OriginalUiApertureOwner50KG::bindGameOptions(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const OriginalUiApertureCallbacks50KG& supplied,OriginalUiApertureDiagnostic50KG& d) noexcept {
    return bindPageImpl(std::move(archive),supplied,false,true,4,d);
}
Status OriginalUiApertureOwner50KG::bindControls(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,std::uint8_t page,
    const OriginalUiApertureCallbacks50KG& supplied,OriginalUiApertureDiagnostic50KG& d) noexcept {
    if(page!=2&&page!=3)return fail(d,Status::missingPage,0x1CEC8C);
    return bindPageImpl(std::move(archive),supplied,false,true,page,d);
}
Status OriginalUiApertureOwner50KG::bindOptionsMenuEntryOnly(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const OriginalUiApertureCallbacks50KG& supplied,
    OriginalUiApertureDiagnostic50KG& d) noexcept {
    return bindPageImpl(std::move(archive),supplied,true,true,1,d);
}
Status OriginalUiApertureOwner50KG::bindRider(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,std::uint8_t page,
    const OriginalUiApertureCallbacks50KG& supplied,OriginalUiApertureDiagnostic50KG& d) noexcept {
    if(page!=1&&page!=2)return fail(d,Status::missingPage,0x1CEC8C);
    return bindPageImpl(std::move(archive),supplied,false,false,page,d,true);
}
Status OriginalUiApertureOwner50KG::bindCourse(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const OriginalUiApertureCallbacks50KG& callbacks,OriginalUiApertureDiagnostic50KG& d)noexcept{
    return bindPageImpl(std::move(archive),callbacks,false,false,1,d,false,true);
}
Status OriginalUiApertureOwner50KG::bindCourseEntryOnly(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const OriginalUiApertureCallbacks50KG& callbacks,OriginalUiApertureDiagnostic50KG& d)noexcept{
    return bindPageImpl(std::move(archive),callbacks,true,false,1,d,false,true);
}
Status OriginalUiApertureOwner50KG::bindTournament(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const OriginalUiApertureCallbacks50KG& callbacks,OriginalUiApertureDiagnostic50KG& d) noexcept {
    return bindPageImpl(std::move(archive),callbacks,false,false,1,d,false,false,true);
}
Status OriginalUiApertureOwner50KG::saveTournamentUpdateDelta38(float delta) noexcept {
    if(!state_.bound)return Status::notBound;
    if(!tournament_)return Status::invalidPhase;
    //1F2498 stores298 before2A6BB0. Session owns preceding timer4064 subtraction.
    return saveUpdateDelta38(delta);
}
Status OriginalUiApertureOwner50KG::resetTournamentExitRequestedAtEntry74() noexcept {
    if(inOperation_)return Status::busy;
    if(!state_.bound)return Status::notBound;
    if(!tournament_)return Status::invalidPhase;
    OperationScope operation(inOperation_);
    if(!available())return Status::unavailable;
    // Both popup entry paths clear4060, at1EEAB8 or1EEAD0.
    tournamentExitRequested4060_=false;
    return Status::ok;
}
Status OriginalUiApertureOwner50KG::beginTournamentEntryAtRender48(
    Phase phase,std::uint8_t popupFlags29) noexcept {
    if(inOperation_)return Status::busy;
    if(!state_.bound)return Status::notBound;
    if(!tournament_)return Status::invalidPhase;
    OperationScope operation(inOperation_);
    if(!available())return Status::unavailable;
    if((popupFlags29&1u)&&!tournamentExitRequested4060_)return Status::unavailable;
    // Actual source render point1F2DB8..1F2DF8, never manager entry.
    if(state_.state!=State::closed||phase!=Phase::entry)return Status::noChange;
    state_.state=State::opening;
    emit(Hook::openingAC,0x1F2DF8,0x1EEB50);
    return Status::ok;
}
Status OriginalUiApertureOwner50KG::bindPageImpl(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const OriginalUiApertureCallbacks50KG& supplied,bool entryOnly,bool optionsMenu,std::uint8_t page,
    OriginalUiApertureDiagnostic50KG& d,bool rider,bool course,bool tournament) noexcept {
    static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559,
        "Original aperture requires IEEE binary32");
    d={};
    if (inOperation_) return fail(d,Status::busy);
    if (state_.bound) return fail(d,Status::alreadyBound);
    OperationScope operation(inOperation_);
    const auto callbacks=supplied;
    if (!archive || !archive->loaded() || !archive->identity() ||
        archive->identity()->id!=(tournament?assets::OriginalUiArchiveId50KG::tournament:course?assets::OriginalUiArchiveId50KG::course:rider?assets::OriginalUiArchiveId50KG::rider:optionsMenu?assets::OriginalUiArchiveId50KG::options:assets::OriginalUiArchiveId50KG::mainMenu))
        return fail(d,Status::invalidArchive);
    if (!dependencies(callbacks)) return fail(d,Status::missingDependency);
    if (!dependenciesAvailable(callbacks)) return fail(d,Status::unavailable);
    if (std::fegetround()!=FE_TONEAREST) return fail(d,Status::unsupportedArithmetic,0x1D6804);
    try {
        assets::OriginalUiDat50KG indexDat,pageDat;
        assets::OriginalUiPageIndex50KG index;
        const std::string directory=tournament?"UI/UITourn/":course?"UI/UICourse/":rider?"UI/UIRider/":optionsMenu?"UI/UIOption/":"UI/UIMain/";
        if (!assets::readOriginalUiDat50KG(*archive,directory+"ui.dat",indexDat,d.parse) ||
            !assets::readOriginalUiPageIndex50KG(indexDat,index,d.parse))
            return fail(d,Status::parseFailure,d.parse.sourcePC);
        if (!page || page>index.pageFiles.size()) return fail(d,Status::missingPage,0x1CEC8C);
        // Pod page loader1CECA4..BC prefixes the actual manager directory
        // before its PageFile. This is source path construction, not a search
        // or basename fallback; Main's authored value is simply main.dat.
        const auto pageMember=directory+index.pageFiles[page-1];
        if (!assets::readOriginalUiDat50KG(*archive,pageMember,pageDat,d.parse))
            return fail(d,Status::parseFailure,d.parse.sourcePC);
        const auto* info=pageDat.section("PageInfo");
        if (!info) return fail(d,Status::missingPage,0x1D64CC);
        OriginalUiApertureConfig50KG config;
        config.indexIdentity=indexDat.identity();config.pageIdentity=pageDat.identity();
        if (!assets::originalUiInteger50KG(*info,"OpenSize",224,config.openSize274,
                d.parse,0x1D64DC) ||
            !assets::originalUiNumber50KG(*info,"Time",1.0f,config.time4B0,
                d.parse,0x1D64F4))
            return fail(d,Status::parseFailure,d.parse.sourcePC);
        config.openSizeAuthored=info->find("OpenSize")!=nullptr;
        config.timeAuthored=info->find("Time")!=nullptr;
        const auto* start=info->find("Start");
        config.startAuthored=start!=nullptr;
        // Literal source strcmp against "Closed". Every other authored string
        // takes the open path; do not silently normalize its case/spelling.
        config.startClosed=!start || start->text=="Closed";
        // Same bounded native OpenSize/Time domain as original_ui_pages50kg.
        // Denormal/overflow behavior is additionally unavailable, not emulated.
        if (config.openSize274<0 || config.openSize274>224 ||
            !ordinary(config.time4B0) || config.time4B0<=0.0f)
            return fail(d,Status::invalidConfig,0x1D67E4);
        const float numerator=intToFloat32(224-config.openSize274);
        const float speed=div32(numerator,config.time4B0);
        if (!ordinary(speed) || speed<0.0f || (numerator!=0.0f && speed==0.0f))
            return fail(d,Status::unsupportedArithmetic,0x1D6804);
        OriginalUiApertureSnapshot50KG next;
        next.bound=true;next.speed4B8=speed;
        next.state=config.startClosed?State::closed:State::open;
        next.top284=config.startClosed?224:config.openSize274;
        next.bottom28C=config.startClosed?224:448-config.openSize274;
        // Shared constructor 1D5F70 clears input268 (overriding generic ctor
        // 2A5CCC's 1). Only the non-Closed config branch 1D654C writes it to 1.
        next.inputEnabled268=!config.startClosed;
        // No source update sample or rendered primitive is fabricated at bind.
        archive_=std::move(archive);config_=config;callbacks_=callbacks;state_=next;entryOnly_=entryOnly;optionsMenu_=optionsMenu;optionsPage_=page;rider_=rider;course_=course;tournament_=tournament;
        return fail(d,Status::ok);
    } catch (const std::bad_alloc&) { return fail(d,Status::allocation); }
    catch (...) { return fail(d,Status::parseFailure); }
}
bool OriginalUiApertureOwner50KG::available() const noexcept {
    return state_.bound && dependenciesAvailable(callbacks_);
}
void OriginalUiApertureOwner50KG::emit(Hook kind,std::uint32_t callPC,
    std::uint32_t targetPC) noexcept {
    const OriginalUiApertureHook50KG event{kind,callPC,targetPC,state_};
    callbacks_.hook(callbacks_.context,event);
}
Status OriginalUiApertureOwner50KG::beginTransition7C(Phase phase) noexcept {
    if (inOperation_) return Status::busy;
    if (!state_.bound) return Status::notBound;
    OperationScope operation(inOperation_);
    if (!available()) return Status::unavailable;
    if (phase==Phase::entry) {
        if(course_||tournament_)return Status::noChange; // actual1E0460 / 1F2E30 entry is no-op
        if (state_.state==State::open || state_.state==State::opening) return Status::noChange;
        state_.state=State::opening;
        emit(Hook::openingAC,0x1D750C,rider_?0x1E9E70:optionsMenu_?(optionsPage_==4?0x1F5BF0:optionsPage_==2||optionsPage_==3?0x1F44A0:0x1F9D20):0x1E66C0);
        return Status::ok;
    }
    if (phase==Phase::exit) {
        if (entryOnly_) return Status::unavailable;
        if (state_.state==State::closed || state_.state==State::closing) return Status::noChange;
        state_.state=State::closing;state_.inputEnabled268=false;
        emit(Hook::closingB4,tournament_?0x1F2E7C:course_?0x1E04AC:rider_?0x1ED130:0x1D74B4,tournament_?0x1EEBE0:course_?0x1DAA70:rider_?0x1EA360:optionsMenu_?(optionsPage_==4?0x1F5DD0:optionsPage_==2||optionsPage_==3?0x1F45C0:0x1F9ED0):0x1E6960);
        return Status::ok;
    }
    // Native rejection instead of executing the source bad-transition assert.
    return Status::invalidPhase;
}
Status OriginalUiApertureOwner50KG::beginCourseEntryAtRender48(Phase phase)noexcept{
    if(inOperation_)return Status::busy;if(!state_.bound)return Status::notBound;if(!course_)return Status::invalidPhase;
    OperationScope operation(inOperation_);if(!available())return Status::unavailable;
    if(state_.state!=State::closed||phase!=Phase::entry)return Status::noChange;
    state_.state=State::opening;emit(Hook::openingAC,0x1DEB60,0x1DA7A0);return Status::ok;
}
Status OriginalUiApertureOwner50KG::saveCourseUpdateDelta38(float delta)noexcept{
    if(!state_.bound)return Status::notBound;if(!course_)return Status::invalidPhase;return saveUpdateDelta38(delta);
}
Status OriginalUiApertureOwner50KG::saveRiderUpdateDelta38(float actualDelta) noexcept {
    if(!rider_)return Status::invalidPhase;
    return saveUpdateDelta38(actualDelta);
}
Status OriginalUiApertureOwner50KG::saveMainUpdateDelta38(float actualDelta) noexcept {
    if(optionsMenu_||rider_||course_||tournament_)return Status::invalidPhase;
    return saveUpdateDelta38(actualDelta);
}
Status OriginalUiApertureOwner50KG::saveOptionsMenuUpdateDelta38(float actualDelta) noexcept {
    if(!state_.bound)return Status::notBound;
    if(!optionsMenu_||optionsPage_!=1)return Status::invalidPhase;
    return saveUpdateDelta38(actualDelta);
}
Status OriginalUiApertureOwner50KG::saveGameOptionsUpdateDelta38(float actualDelta) noexcept {
    if(!state_.bound)return Status::notBound;
    if(!optionsMenu_||optionsPage_!=4)return Status::invalidPhase;
    return saveUpdateDelta38(actualDelta);
}
Status OriginalUiApertureOwner50KG::saveControlsUpdateDelta38(float actualDelta) noexcept {
    if(!state_.bound)return Status::notBound;
    if(!optionsMenu_||(optionsPage_!=2&&optionsPage_!=3))return Status::invalidPhase;
    return saveUpdateDelta38(actualDelta); //1F3CD0/1F3CD4, before2A6BB0.
}
Status OriginalUiApertureOwner50KG::saveUpdateDelta38(float actualDelta) noexcept {
    if (inOperation_) return Status::busy;
    if (!state_.bound) return Status::notBound;
    OperationScope operation(inOperation_);
    if (!available()) return Status::unavailable;
    if (!ordinary(actualDelta) || actualDelta<0.0f) return Status::invalidDelta;
    state_.savedDelta298=actualDelta;state_.hasUpdateDelta=true;
    return Status::ok;
}
Status OriginalUiApertureOwner50KG::render48(Life lifecycle) noexcept {
    if (inOperation_) return Status::busy;
    if (!state_.bound) return Status::notBound;
    OperationScope operation(inOperation_);
    if (!available()) return Status::unavailable;
    // Native admission: no invented initial savedDelta, even if this particular
    // state could draw without consulting it. The real update precedes render.
    if (!state_.hasUpdateDelta) return Status::missingUpdateDelta;
    if (std::fegetround()!=FE_TONEAREST) return Status::unsupportedArithmetic;
    const bool closing=state_.state==State::closing;
    const bool opening=state_.state==State::opening;
    const bool closedEndpoint=state_.bottom28C<=224 && state_.top284>=224;
    const bool openEndpoint=state_.bottom28C>=448-config_.openSize274 &&
        state_.top284<=config_.openSize274;
    // Preflight only arithmetic, with no writes/hooks/primitive submission.
    std::int32_t top=state_.top284,bottom=state_.bottom28C;
    if ((closing && !closedEndpoint) || (opening && !openEndpoint)) {
        const float product=mul32(state_.speed4B8,state_.savedDelta298);
        if (!ordinary(product) || product<0.0f ||
            (product==0.0f && state_.speed4B8!=0.0f && state_.savedDelta298!=0.0f))
            return Status::unsupportedArithmetic;
        std::int32_t step=0;
        if (!convertOriginalUiApertureCvtWS50KG(product,step) || step<0)
            return Status::unsupportedArithmetic;
        const std::int64_t nextTop=std::int64_t(top)+(closing?std::int64_t(step):-std::int64_t(step));
        const std::int64_t nextBottom=std::int64_t(bottom)+(closing?-std::int64_t(step):std::int64_t(step));
        // Source ADDU/SUBU wraps outside signed32. Such enormous deltas are
        // explicitly outside this native admission instead of invoking C++ UB.
        if (!safeEdges(nextTop,nextBottom)) return Status::unsupportedArithmetic;
        top=static_cast<std::int32_t>(nextTop);bottom=static_cast<std::int32_t>(nextBottom);
    }
    if (closing && closedEndpoint) {
        // This effect sees the pre-clamp closing state and original overshoot.
        if (lifecycle==Life::waitScreenExit)
            emit(Hook::screenExitObject440,0x1D75A4,0x2E0170);
        state_.top284=224;state_.bottom28C=224;state_.state=State::closed;
        emit(Hook::closedBC,0x1D75C8,0x1D7A00);
    } else if (closing) {
        state_.bottom28C=bottom;state_.top284=top;
        if (state_.bottom28C<=224 && state_.top284>=224)
            emit(Hook::closingThresholdB8,0x1D7630,tournament_?0x1EEC30:course_?0x1DAAC0:rider_?0x1EA3E0:optionsMenu_?(optionsPage_==4?0x1F5F30:optionsPage_==2||optionsPage_==3?0x1F45E0:0x1F9FE0):0x1E6A50);
        // Deliberately retain closing here, including a crossed endpoint.
    } else if (opening && openEndpoint) {
        state_.top284=config_.openSize274;state_.bottom28C=448-config_.openSize274;
        state_.state=State::open;state_.inputEnabled268=true;
        emit(Hook::openedB0,0x1D7698,tournament_?0x1EEBA0:course_?0x1DA950:rider_?0x1EA090:optionsMenu_?(optionsPage_==4?0x1F5D20:optionsPage_==2||optionsPage_==3?0x1F44E0:0x1F9E20):0x1E68F0);
    } else if (opening) {
        state_.bottom28C=bottom;state_.top284=top;
        // Source has no opening post-step threshold check or callback.
    }
    // Preserve overshoot geometry until the next real render's endpoint clamp.
    geometry_.rectangles[0]=rectangle(0,state_.top284,0x424);
    geometry_.rectangles[1]=rectangle(state_.bottom28C,448,0x428);
    state_.geometryPrepared=true;
    for (std::uint8_t i=0;i<2;++i) {
        geometry_.rectangles[i].dirty3C=true;
        callbacks_.submitOverlay(callbacks_.context,i,geometry_.rectangles[i]);
    }
    // Further source page drawing 1D7A10 and child render 141D70 are caller-owned.
    return Status::ok;
}
Completion OriginalUiApertureOwner50KG::pollCompletion(
    OriginalUiTransitionRoute50KG route,Phase phase) const noexcept {
    if (inOperation_ || !state_.bound || route.screen!=(tournament_?9:course_?6:rider_?5:optionsMenu_?12:4) || route.page!=((rider_||optionsMenu_)?optionsPage_:1))
        return Completion::unavailable;
    OperationScope operation(inOperation_);
    if (!available()) return Completion::unavailable;
    if(tournament_){
        //1F2F00 returns1 for entry even while closed; render opens later.
        // Pending exit writes4060 to suppress popup/save at1F28CC.
        if(phase==Phase::entry)return Completion::complete;
        if(phase==Phase::exit&&state_.state==State::closed)return Completion::complete;
        tournamentExitRequested4060_=true;
        return Completion::pending;
    }
    if (phase==Phase::entry) return state_.state==State::open?Completion::complete:Completion::pending;
    if (phase==Phase::exit) return entryOnly_?Completion::unavailable:
        (state_.state==State::closed?Completion::complete:Completion::pending);
    return Completion::pending;
}
} // namespace rr::game

