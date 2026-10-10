#include "original_ui_static_text_page_owner50kg.h"
#include <limits>
#include <utility>
#include <vector>

namespace rr::game {
namespace {
using Status=OriginalUiStaticTextPageStatus50KG;
using Diagnostic=OriginalUiStaticTextPageDiagnostic50KG;
using Input=OriginalUiStaticTextInput50KG;
using Op=OriginalUiControlOperation50KG;
using NavStatus=OriginalUiNavigationStatus50KG;
using TextStatus=OriginalUiTextWidgetStatus50KG;
using Section=assets::OriginalUiDatSection50KG;

char lower(char c) noexcept { return c>='A' && c<='Z' ? char(c+'a'-'A') : c; }
bool same(const std::string& a,const char* b) noexcept {
    std::size_t i=0;
    for (;i<a.size() && b[i];++i) if (lower(a[i])!=lower(b[i])) return false;
    return i==a.size() && !b[i];
}
template<std::size_t N> bool oneOf(const std::string& key,const char* const (&keys)[N]) noexcept {
    for (const auto* candidate:keys) if (same(key,candidate)) return true;
    return false;
}
void label(std::array<char,128>& output,const std::string& source) noexcept {
    output={};
    for (std::size_t i=0;i<source.size() && i+1<output.size();++i) output[i]=source[i];
}
bool indexed(const std::string& text,const char* prefix,std::size_t minimum,
    std::size_t maximum,std::size_t& output) noexcept {
    std::size_t at=0;
    for (;prefix[at];++at)
        if (at>=text.size() || lower(text[at])!=lower(prefix[at])) return false;
    if (at==text.size() || (text[at]=='0' && at+1!=text.size())) return false;
    std::size_t n=0;
    for (;at<text.size();++at) {
        if (text[at]<'0' || text[at]>'9') return false;
        const auto digit=std::size_t(text[at]-'0');
        if (n>maximum/10 || n*10+digit>maximum) return false;
        n=n*10+digit;
    }
    if (n<minimum) return false;
    output=n;return true;
}
Status result(Diagnostic& d,Status status) noexcept { d.status=status;return status; }
Status admissionFailure(Diagnostic& d,Status status,Input input,
    const Section& section,const std::string& key,std::uint16_t widget,
    std::size_t offset,std::uint32_t pc=0) noexcept {
    d={};d.status=status;d.input=input;d.widgetId=widget;
    label(d.section,section.name);label(d.key,key);
    d.parse.byteOffset=offset;d.parse.sourcePC=pc;d.sourcePC=pc;return status;
}
Status fieldFailure(Diagnostic& d,Status status,Input input,const Section& section,
    const char* key,std::uint16_t widget,std::uint32_t pc) {
    const std::string requestedKey=key;
    const auto* value=section.find(requestedKey);
    return admissionFailure(d,status,input,section,value?value->key:requestedKey,widget,
        value?value->valueOffset:section.sourceOffset,pc);
}
bool admitPageSchema(const assets::OriginalUiDat50KG& dat,
    const assets::OriginalUiPage50KG& page,Diagnostic& d) noexcept {
    static constexpr const char* infoKeys[]={"NumberOfControls","NumberOfGroups",
        "NumberOfStaticWidgets","PageNo","OpenSize","Time","UseVideoAsTexture",
        "Background","Start","BackgroundColor","HasPopup","PopupConfirmWidget",
        "PopupFormatWidget","PopupFormatSuccessWidget","PopupFormatFailWidget",
        "PopupYesWidget","PopupNoWidget","PopupAskFormatWidget","PopupNoMemcardWidget",
        "PopupFullMemcardWidget"};
    static constexpr const char* groupKeys[]={"NumberOfControls","NumberOfAssociations","KeepEnabled"};
    for (const auto& section:dat.sections()) {
        std::size_t index=0;
        const bool info=same(section.name,"PageInfo");
        const bool control=indexed(section.name,"Control ",0,page.controls.size()-1,index);
        const bool group=indexed(section.name,"Group ",0,page.groups.size()-1,index);
        if (!info && !control && !group) {
            admissionFailure(d,Status::unsupportedSection,Input::page,section,{},0,section.sourceOffset);
            return false;
        }
        for (const auto& value:section.values) {
            if (!value.sourceValuePresent) continue; // Source Find does not see Key=.
            const bool known=info ? oneOf(value.key,infoKeys) : control ?
                (same(value.key,"NumberOfWidgets") || indexed(value.key,"WidgetID",1,
                    assets::originalUiMembershipLimit50KG,index)) :
                (oneOf(value.key,groupKeys) || indexed(value.key,"ControlID",1,
                    assets::originalUiMembershipLimit50KG,index) || indexed(value.key,"AssociationID",1,
                    assets::originalUiMembershipLimit50KG,index));
            if (!known) {
                admissionFailure(d,Status::unsupportedField,Input::page,section,value.key,0,value.keyOffset);
                return false;
            }
        }
    }
    return true;
}
bool admitWidgetSections(const assets::OriginalUiDat50KG& dat,std::uint16_t count,
    Diagnostic& d) noexcept {
    for (const auto& section:dat.sections()) {
        std::size_t id=0;
        if (same(section.name,"General")) {
            for (const auto& value:section.values) if (value.sourceValuePresent && !same(value.key,"NumberOfWidgets")) {
                admissionFailure(d,Status::unsupportedField,Input::widgets,section,value.key,0,value.keyOffset);
                return false;
            }
        } else if (!indexed(section.name,"Widget ",1,count,id)) {
            admissionFailure(d,Status::unsupportedSection,Input::widgets,section,{},0,section.sourceOffset);
            return false;
        }
    }
    return true;
}
bool admitTextSchema(const Section& section,std::uint16_t id,Diagnostic& d) {
    const auto* type=section.find("Type");
    if (!type || type->text!="Overlay_Text") {
        fieldFailure(d,Status::unsupportedWidget,Input::widgets,section,"Type",id,0x2A4748);
        return false;
    }
    static constexpr const char* keys[]={"Type","FontName","MaterialName","XPosition","YPosition",
        "Height","Alpha","LeadingFactor","Depth","Color","HighlightColor","DisabledColor",
        "EnableOnSelect","DisableOnSelect","EnableOnDeSelect","DisableOnDeSelect",
        "WantsJoystickInput1","WantsJoystickInput2","Justification","TextID","NumberOfLines","Text"};
    for (const auto& value:section.values) {
        if (!value.sourceValuePresent) continue;
        if (same(value.key,"TextDynamicLineLength") || same(value.key,"TextDynamicLineCount")) {
            admissionFailure(d,Status::unsupportedTextSource,Input::widgets,section,value.key,id,
                value.valueOffset,0x2A9568);return false;
        }
        std::size_t line=0;
        if (!oneOf(value.key,keys) && !indexed(value.key,"Line",1,originalUiTextWidgetLineLimit50KG,line)) {
            // The whitelist is native admission. It deliberately rejects new
            // motion/fade/child/widget fields instead of quietly ignoring them.
            admissionFailure(d,Status::unsupportedField,Input::widgets,section,value.key,id,value.keyOffset);
            return false;
        }
    }
    return true;
}
bool operationAdmitted(Op op) noexcept {
    switch (op) {
    case Op::show: case Op::hide: case Op::select: case Op::deselect:
    case Op::clearSelection: case Op::setEnabled: case Op::reset: return true;
    }
    return false;
}
Status textResult(Diagnostic& d,TextStatus status) noexcept {
    d.text=status;
    switch (status) {
    case TextStatus::ok: return result(d,Status::ok);
    case TextStatus::hidden: return result(d,Status::hidden);
    case TextStatus::lockedOut: return result(d,Status::lockedOut);
    case TextStatus::unsupportedAnimation: return result(d,Status::unsupportedAnimation);
    case TextStatus::invalidInput: return result(d,Status::invalidInput);
    case TextStatus::invalidLine: return result(d,Status::invalidLine);
    default: return result(d,Status::textFailure);
    }
}
bool navigationProducedBatch(NavStatus status) noexcept {
    return status==NavStatus::ok || status==NavStatus::noChange || status==NavStatus::unavailable;
}
}

struct OriginalUiStaticTextPageOwner50KG::Storage {
    // Declaration order also preserves descriptor/archive lifetime at teardown.
    std::shared_ptr<const assets::OriginalUiArchive50KG> pageArchive,widgetsArchive;
    assets::OriginalUiDat50KG pageDat,widgetsDat;
    assets::OriginalUiPage50KG page;
    std::vector<assets::OriginalUiOverlayText50KG> descriptors;
    std::unique_ptr<OriginalUiTextWidget50KG[]> widgets;
    OriginalUiNavigation50KG navigation;
};
OriginalUiStaticTextPageOwner50KG::OriginalUiStaticTextPageOwner50KG() noexcept=default;
OriginalUiStaticTextPageOwner50KG::~OriginalUiStaticTextPageOwner50KG()=default;

Status OriginalUiStaticTextPageOwner50KG::bind(
    std::shared_ptr<const assets::OriginalUiArchive50KG> pageArchive,const std::string& pageMember,
    std::shared_ptr<const assets::OriginalUiArchive50KG> widgetsArchive,const std::string& widgetsMember,
    const assets::OriginalUiEnglish50KG* english,Diagnostic& d) noexcept {
    d={};
    if (storage_) return result(d,Status::alreadyBound);
    if (!pageArchive || !pageArchive->loaded()) {
        d.input=Input::page;return result(d,Status::invalidArchive);
    }
    if (!widgetsArchive || !widgetsArchive->loaded()) {
        d.input=Input::widgets;return result(d,Status::invalidArchive);
    }
    try {
        auto next=std::make_unique<Storage>();
        next->pageArchive=std::move(pageArchive);next->widgetsArchive=std::move(widgetsArchive);
        d.input=Input::page;
        if (!assets::readOriginalUiDat50KG(*next->pageArchive,pageMember,next->pageDat,d.parse))
            return result(d,Status::parseFailure);
        d.input=Input::widgets;
        if (!assets::readOriginalUiDat50KG(*next->widgetsArchive,widgetsMember,next->widgetsDat,d.parse))
            return result(d,Status::parseFailure);
        // This leaf reads NumberOfWidgets first. Parse diagnostics retain the
        // exact source offset/PC; other page-schema fields belong to pageDat.
        d.input=Input::page;
        if (!assets::readOriginalUiPage50KG(next->pageDat,next->widgetsDat,next->page,d.parse)) {
            if (d.parse.sourcePC==0x2A4404 ||
                (next->pageDat.section("PageInfo") && !next->widgetsDat.section("General")))
                d.input=Input::widgets;
            return result(d,Status::parseFailure);
        }
        if (!admitPageSchema(next->pageDat,next->page,d) ||
            !admitWidgetSections(next->widgetsDat,next->page.widgetCount,d)) return d.status;
        const auto* info=next->pageDat.section("PageInfo"); // Proven by successful reader.
        if (next->page.staticWidgetCount!=0)
            return fieldFailure(d,Status::unsupportedPageFeature,Input::page,*info,
                "NumberOfStaticWidgets",0,0x1D6DEC);
        if (next->page.hasPopup)
            return fieldFailure(d,Status::unsupportedPageFeature,Input::page,*info,"HasPopup",0,0x1D6570);
        if (next->page.useVideoAsTexture)
            return fieldFailure(d,Status::unsupportedPageFeature,Input::page,*info,"UseVideoAsTexture",0,0x1D6D74);
        // Complete allocation/descriptor population before publishing pointers.
        const auto count=std::size_t(next->page.widgetCount);
        next->descriptors.resize(count+1);
        for (std::size_t i=1;i<=count;++i) {
            const auto id=static_cast<std::uint16_t>(i);
            const auto* section=next->widgetsDat.section("Widget "+std::to_string(i));
            d={};d.input=Input::widgets;d.widgetId=id;
            if (!section) {
                d.parse={assets::OriginalUiPageError50KG::missingSection,0,0x2A9000};
                return result(d,Status::parseFailure);
            }
            if (!admitTextSchema(*section,id,d)) return d.status;
            label(d.section,section->name);
            if (!assets::readOriginalUiOverlayText50KG(next->widgetsDat,id,english,next->descriptors[i],d.parse))
                return result(d,Status::parseFailure);
            const auto& descriptor=next->descriptors[i];
            if (descriptor.textSource==assets::OriginalUiTextSource50KG::dynamic)
                return fieldFailure(d,Status::unsupportedTextSource,Input::widgets,*section,
                    "TextDynamicLineLength",id,0x2A9568);
            if (descriptor.alpha<std::numeric_limits<std::int16_t>::min() ||
                descriptor.alpha>std::numeric_limits<std::int16_t>::max()) {
                fieldFailure(d,Status::textFailure,Input::widgets,*section,"Alpha",id,0x2A91D0);
                d.text=TextStatus::invalidDescriptor;return d.status;
            }
            // Manager input callbacks have no admitted static-text implementation.
            if (descriptor.wantsJoystickInput[0] || descriptor.wantsJoystickInput[1])
                return fieldFailure(d,Status::unsupportedField,Input::widgets,*section,
                    descriptor.wantsJoystickInput[0]?"WantsJoystickInput1":"WantsJoystickInput2",id,0x2A44C0);
        }
        next->widgets=std::make_unique<OriginalUiTextWidget50KG[]>(count+1);
        for (std::size_t i=1;i<=count;++i) {
            const auto status=next->widgets[i].bind(next->descriptors[i]);
            if (status!=TextStatus::ok) {
                d={};d.input=Input::widgets;d.widgetId=static_cast<std::uint16_t>(i);
                d.text=status;return result(d,Status::textFailure);
            }
        }
        if (!bindOriginalUiNavigation50KG(next->page,next->navigation)) {
            d={};d.navigation=NavStatus::invalidPage;return result(d,Status::navigationFailure);
        }
        // Moving the heap handle does not move Storage, page, descriptors,
        // strings, text owners or line arrays. No post-bind resizing is possible.
        storage_=std::move(next);d={};return Status::ok;
    } catch (...) {
        d={};return result(d,Status::allocation);
    }
}

bool OriginalUiStaticTextPageOwner50KG::bound() const noexcept { return bool(storage_); }
const assets::OriginalUiDat50KG* OriginalUiStaticTextPageOwner50KG::pageDat() const noexcept {
    return storage_?&storage_->pageDat:nullptr;
}
const assets::OriginalUiDat50KG* OriginalUiStaticTextPageOwner50KG::widgetsDat() const noexcept {
    return storage_?&storage_->widgetsDat:nullptr;
}
const assets::OriginalUiPage50KG* OriginalUiStaticTextPageOwner50KG::page() const noexcept {
    return storage_?&storage_->page:nullptr;
}
const OriginalUiNavigation50KG* OriginalUiStaticTextPageOwner50KG::navigation() const noexcept {
    return storage_?&storage_->navigation:nullptr;
}
const assets::OriginalUiOverlayText50KG* OriginalUiStaticTextPageOwner50KG::descriptor(std::uint16_t id) const noexcept {
    return storage_ && id && id<=storage_->page.widgetCount ? &storage_->descriptors[id] : nullptr;
}
const OriginalUiTextWidget50KG* OriginalUiStaticTextPageOwner50KG::textWidget(std::uint16_t id) const noexcept {
    return storage_ && id && id<=storage_->page.widgetCount ? &storage_->widgets[id] : nullptr;
}

Status OriginalUiStaticTextPageOwner50KG::preflight(const OriginalUiCommands50KG& commands,
    Diagnostic& d) const noexcept {
    d={};
    if (!storage_) return result(d,Status::notBound);
    if (commands.count>commands.values.size()) return result(d,Status::invalidCommand);
    std::size_t callbacks=0;
    for (std::size_t i=0;i<commands.count;++i) {
        const auto& command=commands.values[i];
        d.commandIndex=i;d.controlId=command.controlId;d.sourcePC=command.sourcePC;
        d.membershipIndex=originalUiStaticTextNoIndex50KG;d.widgetId=0;
        if (command.controlId>=storage_->page.controls.size() || !operationAdmitted(command.operation))
            return result(d,Status::invalidCommand);
        const auto& membership=storage_->page.controls[command.controlId].widgets;
        for (std::size_t j=0;j<membership.size();++j) {
            const auto id=membership[j];d.membershipIndex=j;d.widgetId=id;
            if (!id || id>storage_->page.widgetCount || !storage_->widgets[id].descriptor())
                return result(d,Status::invalidWidget);
            ++callbacks;
        }
    }
    d={};
    d.commandCount=commands.count;d.callbackCount=callbacks;return Status::ok;
}
void OriginalUiStaticTextPageOwner50KG::commitCommands(const OriginalUiCommands50KG& commands) noexcept {
    for (std::size_t i=0;i<commands.count;++i) {
        const auto& command=commands.values[i];
        // Text apply is total for bound descriptors and these seven admitted
        // operations. Preflight plus private immutable ownership makes this
        // second pass nonfailing. No external callback/reentry point is exposed.
        for (const auto id:storage_->page.controls[command.controlId].widgets)
            (void)storage_->widgets[id].apply(command.operation,command.value);
        auto& state=storage_->navigation.controls[command.controlId];
        switch (command.operation) {
        case Op::select: state.selected=true;break;
        case Op::deselect: case Op::clearSelection: state.selected=false;break;
        case Op::show: state.shown=true;break;
        case Op::hide: state.shown=false;break;
        case Op::setEnabled: state.enabled=command.value;break;
        case Op::reset: break; // Navigation's command helper makes no reset write.
        }
    }
}
Status OriginalUiStaticTextPageOwner50KG::applyCommands(const OriginalUiCommands50KG& commands,
    Diagnostic& d) noexcept {
    const auto status=preflight(commands,d);
    if (status!=Status::ok) return status;
    commitCommands(commands);return Status::ok;
}
Status OriginalUiStaticTextPageOwner50KG::finishNavigation(OriginalUiNavigation50KG& next,
    const OriginalUiCommands50KG& commands,NavStatus navigationStatus,Diagnostic& d) noexcept {
    if (!navigationProducedBatch(navigationStatus)) {
        d={};d.navigation=navigationStatus;return result(d,Status::navigationFailure);
    }
    const auto status=preflight(commands,d);d.navigation=navigationStatus;
    if (status!=Status::ok) return status;
    commitCommands(commands);storage_->navigation=next;
    return result(d,navigationStatus==NavStatus::noChange ? Status::noChange :
        navigationStatus==NavStatus::unavailable ? Status::unavailable : Status::ok);
}
Status OriginalUiStaticTextPageOwner50KG::enterGroup(std::uint16_t group,std::uint16_t slot,
    std::uint8_t controller,Diagnostic& d) noexcept {
    d={};if (!storage_) return result(d,Status::notBound);
    auto next=storage_->navigation;OriginalUiCommands50KG commands;
    const auto status=enterOriginalUiGroup50KG(next,group,slot,controller,commands);
    return finishNavigation(next,commands,status,d);
}
Status OriginalUiStaticTextPageOwner50KG::moveControl(std::uint16_t group,bool forward,Diagnostic& d) noexcept {
    d={};if (!storage_) return result(d,Status::notBound);
    auto next=storage_->navigation;OriginalUiCommands50KG commands;
    const auto status=moveOriginalUiControl50KG(next,group,forward,commands);
    return finishNavigation(next,commands,status,d);
}
Status OriginalUiStaticTextPageOwner50KG::enableControl(std::uint16_t control,bool enabled,Diagnostic& d) noexcept {
    d={};if (!storage_) return result(d,Status::notBound);
    auto next=storage_->navigation;OriginalUiCommands50KG commands;
    const auto status=enableOriginalUiControl50KG(next,control,enabled,commands);
    return finishNavigation(next,commands,status,d);
}
Status OriginalUiStaticTextPageOwner50KG::dispatchMain(std::uint8_t controller,OriginalUiMainInput50KG input,
    const OriginalUiMainContext50KG& context,OriginalUiMainDecision50KG& output,Diagnostic& d) noexcept {
    d={};if (!storage_) return result(d,Status::notBound);
    auto next=storage_->navigation;OriginalUiCommands50KG commands;OriginalUiMainDecision50KG decision;
    const auto navigationStatus=dispatchOriginalUiMain50KG(next,controller,input,context,commands,decision);
    const auto status=finishNavigation(next,commands,navigationStatus,d);
    if (status==Status::ok || status==Status::noChange || status==Status::unavailable) output=decision;
    return status;
}

OriginalUiTextWidget50KG* OriginalUiStaticTextPageOwner50KG::mutableWidget(std::uint16_t id,Diagnostic& d) noexcept {
    d={};d.widgetId=id;
    if (!storage_) { d.status=Status::notBound;return nullptr; }
    if (!id || id>storage_->page.widgetCount) { d.status=Status::invalidWidget;return nullptr; }
    return &storage_->widgets[id];
}
Status OriginalUiStaticTextPageOwner50KG::setupLine(std::uint16_t id,std::size_t index,
    const OriginalUiTextLineSetup50KG& setup,Diagnostic& d) noexcept {
    auto* widget=mutableWidget(id,d);d.lineIndex=index;
    return widget?textResult(d,widget->setupLine(index,setup)):d.status;
}
Status OriginalUiStaticTextPageOwner50KG::setLineColor(std::uint16_t id,std::size_t index,
    std::uint32_t color,Diagnostic& d) noexcept {
    auto* widget=mutableWidget(id,d);d.lineIndex=index;
    return widget?textResult(d,widget->setLineColor(index,color)):d.status;
}
Status OriginalUiStaticTextPageOwner50KG::setLineAlpha(std::uint16_t id,std::size_t index,
    std::int32_t alpha,float duration,Diagnostic& d) noexcept {
    auto* widget=mutableWidget(id,d);d.lineIndex=index;
    return widget?textResult(d,widget->setLineAlpha(index,alpha,duration)):d.status;
}
Status OriginalUiStaticTextPageOwner50KG::stopLine(std::uint16_t id,std::size_t index,Diagnostic& d) noexcept {
    auto* widget=mutableWidget(id,d);d.lineIndex=index;
    return widget?textResult(d,widget->stopLine(index)):d.status;
}
Status OriginalUiStaticTextPageOwner50KG::updateStatic(std::uint16_t id,bool ancestorsShown,Diagnostic& d) noexcept {
    auto* widget=mutableWidget(id,d);
    return widget?textResult(d,widget->updateStatic(ancestorsShown)):d.status;
}
Status OriginalUiStaticTextPageOwner50KG::positionLine(std::uint16_t id,std::size_t index,
    const OriginalUiStaticTextMetrics50KG& metrics,OriginalUiTextPosition50KG& output,Diagnostic& d) const noexcept {
    d={};d.widgetId=id;d.lineIndex=index;
    if (!storage_) return result(d,Status::notBound);
    const auto* desc=descriptor(id);
    if (!desc) return result(d,Status::invalidWidget);
    if (index>=desc->lines.size()) return result(d,Status::invalidLine);
    OriginalUiTextPositionInput50KG input;
    input.widget=desc;input.line=index;input.fontLineHeight=metrics.fontLineHeight;
    input.measuredWidth=metrics.measuredWidth;input.measuredHeight=metrics.measuredHeight;
    if (!positionOriginalUiText50KG(input,output)) return result(d,Status::invalidMetrics);
    return Status::ok;
}
Status OriginalUiStaticTextPageOwner50KG::projectLine(std::uint16_t id,std::size_t index,
    const OriginalUiStaticTextMetrics50KG& metrics,bool ancestorsShown,
    OriginalUiStaticTextProjection50KG& output,Diagnostic& d) noexcept {
    OriginalUiStaticTextProjection50KG projected;
    const auto status=positionLine(id,index,metrics,projected.position,d);
    if (status!=Status::ok) return status;
    const auto textStatus=storage_->widgets[id].projectLine(index,projected.line,ancestorsShown);
    if (textStatus!=TextStatus::ok) return textResult(d,textStatus);
    projected.descriptor=&storage_->descriptors[id];projected.widgetId=id;projected.lineIndex=index;
    output=projected;return textResult(d,TextStatus::ok);
}
}
