#pragma once
#include "../../src/assets/dlc_files50dl.h"
#include "../../platform/linux/native/game_host50er.h"
#include "../../src/assets/original_loading_texture_source50la.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/original_ui_localization50kg.h"
#include "../../src/game/original_course_descriptor50kg.h"
#include "../../src/game/dlc_course_entries50dl.h"
#include "../../src/assets/dlc_registry50dl.h"
#include "../../src/game/original_ui_text50kg.h"
#include "../../src/game/original_ui_hall_records50kg.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <sstream>

namespace rr::platform {
// Displays original installed PST/LSI artwork. This native presentation does
// not publish completion of the separately reconstructed LoadingManager330.
class PlayerLoadingPresenter50DU final {
    using U=unsigned;using I=int;
    struct Image {I width=0,height=0;std::vector<unsigned char> rgba;};
    struct Animation {Image image;float x=0,y=0,endX=0,endY=0,width=0,height=0,seconds=0;bool loop=false;};
    GameHost50ER& host_;std::filesystem::path root_;std::string code_;
    std::uint64_t sessionIndex_;
    std::shared_ptr<OriginalUiRiderProfileOwner50KG> profile_;
    Image background_;std::vector<Animation> animations_;std::vector<unsigned char> pixels_;
    // LD_ICON animations 4..6: ProgressBar (no alpha blend), then its
    // BlankBar/BlankBar2 overlays, drawn in animation order after1..3.
    // LD_UI has no ProgressBar, so 1CC900's named lookups find nothing there.
    struct Fixed {Image image;float x=0,y=0,width=0,height=0;bool alpha=true;};
    std::optional<Fixed> bar_;std::vector<Fixed> overlays_;
    float progress3C_=0.f;bool stepped_=false;
    struct Font {assets::OriginalUiFont50KG metrics;Image image;};
    std::array<Font,2> fonts_;
    struct Text {std::string bytes;unsigned font=0,justify=0;float x=0,y=0;};
    std::vector<Text> texts_;
    std::chrono::steady_clock::time_point start_=std::chrono::steady_clock::now();
    bool loaded_=false;unsigned captures_=0;
    struct Api {
        void (*get)(U,I*)=nullptr;
        void (*genTextures)(I,U*)=nullptr;void (*deleteTextures)(I,const U*)=nullptr;
        void (*bindTexture)(U,U)=nullptr;void (*parameter)(U,U,I)=nullptr;
        void (*image)(U,I,I,I,I,I,U,U,const void*)=nullptr;
        void (*genFbos)(I,U*)=nullptr;void (*deleteFbos)(I,const U*)=nullptr;
        void (*bindFbo)(U,U)=nullptr;void (*attach)(U,U,U,U,I)=nullptr;
        U (*checkFbo)(U)=nullptr;void (*readBuffer)(U)=nullptr;
        void (*blit)(I,I,I,I,I,I,I,I,U,U)=nullptr;
        void (*bindBuffer)(U,U)=nullptr;void (*pixelStore)(U,I)=nullptr;
        void (*readPixels)(I,I,I,I,U,U,void*)=nullptr;
        unsigned char (*enabled)(U)=nullptr;void (*enable)(U)=nullptr;void (*disable)(U)=nullptr;
        U (*error)()=nullptr;
    } gl_;
    static std::vector<unsigned char> read(const std::filesystem::path& path,std::size_t cap) {
        std::ifstream f(rr::assets::dlc50DL::host(path),std::ios::binary|std::ios::ate);
        if(!f||f.tellg()<=0||static_cast<std::uint64_t>(f.tellg())>cap)throw std::runtime_error("Original loading asset missing or oversized");
        std::vector<unsigned char> b(static_cast<std::size_t>(f.tellg()));f.seekg(0);
        if(!f.read(reinterpret_cast<char*>(b.data()),b.size()))throw std::runtime_error("Original loading asset read failed");return b;
    }
    static std::string trim(std::string s) {
        const auto begin=s.find_first_not_of(" \t\r\n\"");if(begin==std::string::npos)return {};
        return s.substr(begin,s.find_last_not_of(" \t\r\n\"")-begin+1);
    }
    static bool loadTextRejected(int step)noexcept{std::fprintf(stderr,"original_loading_text_rejected step=%d\n",step);return false;}
    // Registry (add-on) course row of code_ (146r); false for every stock code.
    // PORT EXTENSION, NOT ORIGINAL: the original has 13 courses; an add-on
    // course's loading facts (screen file, name, rival, records) come from its row.
    bool addonRow(game::dlc50DL::AddonCourseLoad& row) const {
        return code_.size()==2&&game::dlc50DL::addonCourseLoadByCode(code_.c_str(),row);
    }
    // HOOK (DLC save side file, helper 146s, not merged here): replace the body by
    //   progress.courseHall(game::dlc50DL::addonCourseRecordsTarget(row.number),view); records=view.rows;
    // Until then: the item's hall_defaults (what courseHall answers when no
    // record was set), else empty rows. Never the stock Hall of profile 0x102F0+c*0xE4.
    static void addonRecords(const game::dlc50DL::AddonCourseLoad& row,game::OriginalUiHallCourseRecords50KG& records) {
        records={};
        const auto* c=assets::dlc50DL::addonCourse(row.number);
        if(!c||!c->hallDefaults)return;
        const auto copyName=[](const std::string& from,std::array<char,21>& to){
            const std::size_t n=std::min<std::size_t>(from.size(),to.size()-1); // bounded: 20 characters + NUL
            std::copy(from.begin(),from.begin()+static_cast<std::ptrdiff_t>(n),to.begin());to[n]='\0';};
        for(unsigned i=0;i<3;++i){
            records.lap102F0[i]=static_cast<float>(c->lap[i].time);copyName(c->lap[i].name,records.lapNames102FC[i]);
            records.race1033C[i]=static_cast<float>(c->race[i].time);copyName(c->race[i].name,records.raceNames10348[i]);
            records.knockouts10388[i]=c->knockouts[i].count;copyName(c->knockouts[i].name,records.knockoutNames10394[i]);
        }
    }
    bool loadText() {
        if(code_=="UI")return true;
        OriginalUiRiderProfileOwner50KG::CourseRaceSelection selected;
        game::OriginalUiHallCourseRecords50KG records;
        game::dlc50DL::AddonCourseLoad addon;
        const bool registryCourse=addonRow(addon);
        std::string title;
        std::optional<std::string> rivalName; // nullopt + rivalId = LOC racer name
        std::uint32_t rivalCareer=0;
        if(registryCourse){
            // Track name from the row; rival = the row's rival racer (0 = none).
            if(!profile_)return loadTextRejected(1);
            addonRecords(addon,records);title=addon.name;rivalCareer=addon.rivalRacer;
            if(const auto* pod=rivalCareer?assets::dlc50DL::addonPod(rivalCareer):nullptr)rivalName=pod->name;
            std::fprintf(stderr,"course_route_loading_text code=%s name=\"%s\" rival=%s records=%s stock=0\n",
                addon.code.c_str(),addon.name.c_str(),rivalCareer?std::to_string(rivalCareer).c_str():"none",
                (assets::dlc50DL::addonCourse(addon.number)&&assets::dlc50DL::addonCourse(addon.number)->hallDefaults)?"hall_defaults":"empty");
        }else{
        if(!profile_||!profile_->courseRaceSnapshot(selected)||selected.course>=13||
           !profile_->hallRecordSnapshot(selected.course,records))return loadTextRejected(1);
        const auto seed=game::originalFreshCourseDescriptors50KG[selected.course];
        if(code_!=std::string{seed.first,seed.second})return loadTextRejected(2);
        rivalCareer=static_cast<unsigned>(seed.rival11280);
        }
        assets::OriginalUiArchive50KG loc,fontArchive;assets::OriginalUiArchiveDiagnostic50KG archiveDiagnostic;
        if(!loc.load(assets::OriginalUiArchiveId50KG::localization,read(root_/"LOC.RES",35925),archiveDiagnostic)||
           !fontArchive.load(assets::OriginalUiArchiveId50KG::fonts,read(root_/"UI"/"UIFONTS.RES",44870),archiveDiagnostic))return loadTextRejected(3);
        assets::OriginalUiEnglish50KG english;assets::OriginalUiLocalizationDiagnostic50KG localizationDiagnostic;
        if(!assets::readOriginalUiEnglish50KG(loc,english,localizationDiagnostic))return loadTextRejected(4);
        const auto localized=[&](unsigned id)->std::string {const auto* entry=english.at(id);if(!entry)throw std::runtime_error("Original loading text ID missing");return entry->text;};
        // Constructor17EA08..17EEDC, actual track-ID order; not course menu order.
        constexpr unsigned titles[]={106,111,115,117,110,114,118,108,107,112,116,109,119};
        if(!registryCourse)title=localized(titles[selected.course]);
        if(rivalCareer&&!rivalName){
            OriginalUiRiderProfileOwner50KG::TournamentTextMetadataSnapshot rival;
            if(!profile_->tournamentTextMetadataSnapshot(rivalCareer,rival)||rival.racerName3C<0)return loadTextRejected(5);
            rivalName=localized(static_cast<unsigned>(rival.racerName3C));
        }
        //1CADE0: title/rival, then best lap, race, KOs and their actual record names.
        texts_.push_back({title,0,3,320,142});
        // An add-on course with rival "none" draws no rival line (PORT EXTENSION:
        // every stock course has a rival).
        if(rivalName)texts_.push_back({localized(129)+" "+*rivalName,1,3,320,186});
        std::array<char,9> lap{},race{};
        if(!game::originalUiHallTime18900050KG(records.lap102F0[0],lap)||
           !game::originalUiHallTime18900050KG(records.race1033C[0],race))return loadTextRejected(6);
        const std::string values[]={lap.data(),race.data(),std::to_string(records.knockouts10388[0])};
        const auto name=[](const std::array<char,21>& bytes){const auto end=std::find(bytes.begin(),bytes.end(),'\0');if(end==bytes.end())throw std::runtime_error("Original loading record name lacks terminator");return std::string(bytes.begin(),end);};
        const std::string names[]={name(records.lapNames102FC[0]),name(records.raceNames10348[0]),name(records.knockoutNames10394[0])};
        for(unsigned row=0;row<3;++row){const float y=227.f+32.f*row;
            texts_.push_back({localized(463+row),1,4,255,y});
            texts_.push_back({values[row],1,1,310,y});texts_.push_back({names[row],1,0,370,y});}
        const char* basenames[]={"font/venus16","font/denmrk16"};
        for(unsigned i=0;i<2;++i){const auto cel=fontArchive.find(std::string(basenames[i])+".cel"),pst=fontArchive.find(std::string(basenames[i])+".pst");
            assets::OriginalUiCelDiagnostic50KG celDiagnostic;assets::DecodeError48C error;assets::Texture48E texture;
            if(!cel.data||!pst.data||!assets::decodeOriginalUiCel50KG(cel.data,cel.size,fonts_[i].metrics,celDiagnostic)||
               !assets::decodeTexture48E(std::vector<unsigned char>(pst.data,pst.data+pst.size),texture,error)||texture.mips.empty())return false;
            auto& mip=texture.mips.back();fonts_[i].image={static_cast<I>(mip.width),static_cast<I>(mip.height),std::move(mip.rgba)};}
        return true;
    }
    bool composeText() {
        for(const auto& text:texts_){const auto& font=fonts_[text.font];game::OriginalUiTextInput50KG input;
            input.font=&font.metrics;input.text=reinterpret_cast<const unsigned char*>(text.bytes.data());input.length=text.bytes.size();
            game::OriginalUiTextLayout50KG layout;game::OriginalUiTextDiagnostic50KG diagnostic;
            if(!game::layoutOriginalUiTextDimensions50LA(input,font.image.width,font.image.height,layout,diagnostic))return false;
            float x=text.x,y=text.y;const I width=static_cast<I>(layout.widthBeforeScaleAndRounding),height=static_cast<I>(layout.firstGlyphHeightBeforeScaleAndRounding);
            if(text.justify==1||text.justify==3)x=static_cast<float>(static_cast<I>(x-width*.5f));
            if(text.justify==4)x-=width;if(text.justify==3)y=static_cast<float>(static_cast<I>(y-height*.5f));
            for(unsigned q=0;q<layout.count;++q){const auto& a=layout.quads[q].vertices[0];const auto& b=layout.quads[q].vertices[2];
                if(b.x<=a.x||b.y<=a.y)continue;
                for(I py=std::max(0,static_cast<I>(std::ceil(a.y+y)));py<std::min(448,static_cast<I>(std::ceil(b.y+y)));++py)
                for(I px=std::max(0,static_cast<I>(std::ceil(a.x+x)));px<std::min(640,static_cast<I>(std::ceil(b.x+x)));++px){
                    const float u=a.u+(b.u-a.u)*(px+.5f-x-a.x)/(b.x-a.x),v=a.v+(b.v-a.v)*(py+.5f-y-a.y)/(b.y-a.y);
                    const I sx=std::clamp(static_cast<I>(u*font.image.width),0,font.image.width-1),sy=std::clamp(static_cast<I>(v*font.image.height),0,font.image.height-1);
                    const auto source=(sy*font.image.width+sx)*4,dest=(py*640+px)*4;const I alpha=std::min<I>(128,font.image.rgba[source+3]);
                    for(I c=0;c<3;++c){const I color=font.image.rgba[source+c]*204/255;pixels_[dest+c]=static_cast<unsigned char>((color*alpha+pixels_[dest+c]*(128-alpha))/128);}
                }
            }
        }
        return true;
    }
    bool load() {
        if(loaded_)return true;
        struct Identity {const char* code;const char* hash;};
        static constexpr Identity identities[]={
#include "original_loading_background_hashes50du.inc"
        };
        // The background file: the stock UI/TEXTURES/LD_<CODE>.PST for a stock
        // code; for a registry course the row names it (a listed package file,
        // dlc:/ path opened through the overlay, or a stock course's LD_<CODE>.PST).
        // The shipped fingerprints are RECEIPTS only (one line), never a gate;
        // the layout checks below still bound what is decoded.
        const auto directory=root_/"UI"/"TEXTURES";
        std::filesystem::path backgroundPath=directory/("LD_"+code_+".PST");
        std::string backgroundName="LD_"+code_+".PST",receiptCode=code_;
        const char* hash=nullptr;
        game::dlc50DL::AddonCourseLoad addon;
        const bool registryCourse=addonRow(addon);
        if(registryCourse){
            if(addon.loadingScreen.empty())return false; // registry rows always name one
            backgroundName=addon.loadingScreen;
            if(addon.loadingFromPackage)backgroundPath=std::filesystem::path(addon.loadingScreen);
            else{
                backgroundPath=root_/std::filesystem::path(addon.loadingScreen); // UI/TEXTURES/LD_<STOCK>.PST
                const auto at=addon.loadingScreen.rfind("LD_");
                if(at!=std::string::npos&&addon.loadingScreen.size()>=at+5)receiptCode=addon.loadingScreen.substr(at+3,2);
            }
        }
        if(!registryCourse||!addon.loadingFromPackage)
            for(const auto& identity:identities)if(receiptCode==identity.code)hash=identity.hash;
        const auto bytes=read(backgroundPath,287772);
        if(bytes.size()!=287772)return false; // the one 640x448 indexed8 layout 24EB20 reads
        const bool stockBytes=hash?assets::OriginalLoadingTextureSource50LA::hashMatches(bytes.data(),bytes.size(),hash):
            (registryCourse&&addon.loadingSha256.size()==64&&
             assets::OriginalLoadingTextureSource50LA::hashMatches(bytes.data(),bytes.size(),addon.loadingSha256.c_str()));
        std::fprintf(stderr,"course_route_loading code=%s source=%s file=%s receipt=%s stock=%d\n",code_.c_str(),
            !registryCourse?"stock":addon.loadingFromPackage?"package":"stock_screen",backgroundName.c_str(),
            stockBytes?"match":"differs",registryCourse?0:1);
        if(!hash)hash=registryCourse&&addon.loadingSha256.size()==64?addon.loadingSha256.c_str():"none";
        //24EB20: height448,width640,zero,zero. Indexed8 payload after1024-byte
        //CSM1 palette. Preserve original linear rows and source RGB bytes.
        const auto word=[&](std::size_t at){U n=0;for(unsigned i=0;i<4;++i)n|=U(bytes[at+i])<<(8*i);return n;};
        if(word(0)!=1||word(4)!=1||word(8)!=1||word(1036)!=448||word(1040)!=640||word(1044)||word(1048))return false;
        background_.width=640;background_.height=448;background_.rgba.resize(640*448*4);
        for(std::size_t p=0;p<640*448;++p){const U index=bytes[1052+p];const U palette=(index&~24u)|((index&8u)<<1)|((index&16u)>>1);
            for(unsigned c=0;c<3;++c)background_.rgba[p*4+c]=bytes[12+palette*4+c];background_.rgba[p*4+3]=255;}
        const bool menu=code_=="UI";
        const auto lsi=read(directory/(menu?"LD_UI.LSI":"LD_ICON.LSI"),4096);
        const char* lsiHash=menu?"c88e112612f1a01798e7ea967eae1805f899e58affbd333ad5a806e7376fc153":"3a6b63bc455c8b5bf88e31688118221340499cb38f5a2c32530e57a4640b15a7";
        if(!assets::OriginalLoadingTextureSource50LA::hashMatches(lsi.data(),lsi.size(),lsiHash))return false;
        assets::ResArchive48C archive;assets::DecodeError48C error;
        if(!assets::decodeResource48C(lsi,archive,error))return false;
        std::vector<unsigned char> ini;
        if(!assets::extractResMember48C(archive,menu?"ld_ui.ini":"ld_icon.ini",ini,error))return false;
        std::map<std::string,std::map<std::string,std::string>> sections;
        std::istringstream input(std::string(ini.begin(),ini.end()));std::string line,section;
        while(std::getline(input,line)){line=trim(line.substr(0,line.find(';')));if(line.empty())continue;
            if(line.front()=='['&&line.back()==']'){section=line.substr(1,line.size()-2);continue;}
            const auto equal=line.find('=');if(equal==std::string::npos)return false;
            if(!sections[section].emplace(trim(line.substr(0,equal)),trim(line.substr(equal+1))).second)return false;}
        // First three source animations are horizontal moving lines.
        for(unsigned i=1;i<=3;++i){const auto& fields=sections.at("Animation"+std::to_string(i));
            if(fields.at("StartFrame")!="1"||fields.at("EndFrame")!="1"||fields.at("AlphaBlendingOn")!="T"||
               fields.at("InputColorA")!="0"||fields.at("InputColorB")!="1"||fields.at("InputAlphaC")!="0"||fields.at("InputColorD")!="1")return false;
            std::vector<unsigned char> encoded;assets::Texture48E texture;
            if(!assets::extractResMember48C(archive,"frame01.pst",encoded,error)||!assets::decodeTexture48E(encoded,texture,error)||texture.mips.empty())return false;
            auto& mip=texture.mips.back();Animation a;a.image={static_cast<I>(mip.width),static_cast<I>(mip.height),std::move(mip.rgba)};
            a.x=std::stof(fields.at("StartPosX"));a.y=std::stof(fields.at("StartPosY"));
            a.endX=std::stof(fields.at("EndPosX"));a.endY=std::stof(fields.at("EndPosY"));
            a.width=std::stof(fields.at("StartSizeX"));a.height=std::stof(fields.at("StartSizeY"));
            a.seconds=std::stof(fields.at("MoveTime"));a.loop=fields.at("LoopMovement")=="T";
            if(a.seconds<=0||a.width<=0||a.height<=0)return false;animations_.push_back(std::move(a));}
        // 2CE104..2CE264: MoveTime/SizeTime 0 leave zero velocity, so the
        // current position/size stay at StartPos/StartSize until 1CC900 sets them.
        const unsigned animationCount=static_cast<unsigned>(std::stoul(sections.at("General").at("NumberOfAnimations")));
        if(animationCount!=(menu?3u:6u))return false;
        for(unsigned i=4;i<=animationCount;++i){const auto& fields=sections.at("Animation"+std::to_string(i));
            const auto& name=fields.at("ReferenceName");const auto& frame=fields.at("StartFrame");
            const bool expected=(i==4&&name=="ProgressBar"&&frame=="4")||(i==5&&name=="BlankBar"&&frame=="2")||(i==6&&name=="BlankBar2"&&frame=="3");
            if(!expected||fields.at("EndFrame")!=frame||fields.at("InputColorA")!="0"||fields.at("InputColorB")!="1"||
               fields.at("InputAlphaC")!="0"||fields.at("InputColorD")!="1"||std::stof(fields.at("MoveTime"))!=0.f||
               std::stof(fields.at("SizeTime"))!=0.f)return false;
            std::vector<unsigned char> encoded;assets::Texture48E texture;
            if(!assets::extractResMember48C(archive,"frame0"+frame+".pst",encoded,error)||!assets::decodeTexture48E(encoded,texture,error)||texture.mips.empty())return false;
            auto& mip=texture.mips.back();Fixed f;f.image={static_cast<I>(mip.width),static_cast<I>(mip.height),std::move(mip.rgba)};
            f.x=std::stof(fields.at("StartPosX"));f.y=std::stof(fields.at("StartPosY"));
            f.width=std::stof(fields.at("StartSizeX"));f.height=std::stof(fields.at("StartSizeY"));
            f.alpha=fields.at("AlphaBlendingOn")=="T";
            if(f.width<=0||f.height<=0||(i==4)==f.alpha)return false;
            if(i==4)bar_=std::move(f);else overlays_.push_back(std::move(f));}
#define RR_LOAD(member,name) gl_.member=reinterpret_cast<decltype(gl_.member)>(host_.procAddress(name));if(!gl_.member)return false
        RR_LOAD(get,"glGetIntegerv");RR_LOAD(genTextures,"glGenTextures");RR_LOAD(deleteTextures,"glDeleteTextures");
        RR_LOAD(bindTexture,"glBindTexture");RR_LOAD(parameter,"glTexParameteri");RR_LOAD(image,"glTexImage2D");
        RR_LOAD(genFbos,"glGenFramebuffers");RR_LOAD(deleteFbos,"glDeleteFramebuffers");RR_LOAD(bindFbo,"glBindFramebuffer");
        RR_LOAD(attach,"glFramebufferTexture2D");RR_LOAD(checkFbo,"glCheckFramebufferStatus");RR_LOAD(readBuffer,"glReadBuffer");
        RR_LOAD(blit,"glBlitFramebuffer");RR_LOAD(bindBuffer,"glBindBuffer");RR_LOAD(pixelStore,"glPixelStorei");
        RR_LOAD(readPixels,"glReadPixels");RR_LOAD(enabled,"glIsEnabled");RR_LOAD(enable,"glEnable");RR_LOAD(disable,"glDisable");RR_LOAD(error,"glGetError");
#undef RR_LOAD
        if(!loadText())return false;
        pixels_.resize(background_.rgba.size());loaded_=true;
        std::fprintf(stderr,"original_loading_assets background=%s layout=%s background_sha256=%s\n",backgroundName.c_str(),menu?"LD_UI.LSI":"LD_ICON.LSI",hash);
        return true;
    }
    // Positions are sprite centres. Blend (Cs-Cd)*As/128+Cd, or a plain copy.
    void blit(const Image& image,double left,double top,double width,double height,bool blend) {
        const I x0=std::max(0,static_cast<I>(std::ceil(left))),y0=std::max(0,static_cast<I>(std::ceil(top)));
        const I x1=std::min(640,static_cast<I>(std::ceil(left+width))),y1=std::min(448,static_cast<I>(std::ceil(top+height)));
        for(I y=y0;y<y1;++y)for(I x=x0;x<x1;++x){
            const I sx=std::clamp(static_cast<I>((x+.5-left)*image.width/width),0,image.width-1);
            const I sy=std::clamp(static_cast<I>((y+.5-top)*image.height/height),0,image.height-1);
            const auto source=(sy*image.width+sx)*4,dest=(y*640+x)*4;
            const I alpha=blend?std::min<I>(128,image.rgba[source+3]):128;
            for(I c=0;c<3;++c)pixels_[dest+c]=static_cast<unsigned char>((image.rgba[source+c]*alpha+pixels_[dest+c]*(128-alpha))/128);
        }
    }
    void compose(double elapsed) {
        pixels_=background_.rgba;
        for(const auto& a:animations_){const double fraction=a.loop?std::fmod(elapsed,a.seconds)/a.seconds:std::min(1.,elapsed/a.seconds);
            blit(a.image,a.x+(a.endX-a.x)*fraction-a.width*.5,a.y+(a.endY-a.y)*fraction-a.height*.5,a.width,a.height,true);}
        if(bar_){float x=bar_->x,y=bar_->y,width=bar_->width,height=bar_->height;
            if(stepped_){
                // 1CC900: below 1.0, width=333*p and x=153.5+width/2; else
                // 333 at x=320. 2CEBC0/2CE9D0 receive truncated integers.
                const float w=progress3C_<1.f?333.f*progress3C_:333.f;
                const float centre=progress3C_<1.f?153.5f+w/2.f:320.f;
                width=static_cast<float>(static_cast<I>(w));height=16.f;
                x=static_cast<float>(static_cast<I>(centre));y=379.f;
            }
            if(width>0)blit(bar_->image,x-width*.5,y-height*.5,width,height,bar_->alpha);}
        for(const auto& f:overlays_)blit(f.image,f.x-f.width*.5,f.y-f.height*.5,f.width,f.height,f.alpha);
    }
public:
    PlayerLoadingPresenter50DU(GameHost50ER& host,const std::filesystem::path& root,const char* code,std::shared_ptr<OriginalUiRiderProfileOwner50KG> profile,std::uint64_t sessionIndex):host_(host),root_(root),code_(code),sessionIndex_(sessionIndex),profile_(std::move(profile)){}
    // Loading-screen +3C after an original 1CC900 call.
    void progress(float total)noexcept {progress3C_=total;stepped_=true;}
    const std::vector<unsigned char>& pixels()const noexcept {return pixels_;}
    bool draw(const char*,bool=false,const std::vector<unsigned char>* frozen=nullptr,
        std::uint64_t overlay=0)noexcept try {
        if(!host_.created()||!host_.hasWindow()||!host_.current())return false;
        const auto info=host_.info();if(info.minimized||info.drawableWidth<=0||info.drawableHeight<=0)return true;
        if(!load()||gl_.error()!=0)return false;
        const auto elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-start_).count();if(frozen){
            if(frozen->size()!=pixels_.size()||frozen->empty())return false;
            const unsigned alpha=std::min(128u,unsigned(overlay>>24)&255u);
            for(std::size_t i=0;i<pixels_.size();i+=4)
                for(unsigned c=0;c<3;++c)
                    pixels_[i+c]=static_cast<unsigned char>(((*frozen)[i+c]*(128-alpha)+((overlay>>(8*c))&255u)*alpha)/128);
        }else{compose(elapsed);if(!composeText())return false;}
        const char* capture=std::getenv("RR_WINDOW_CAPTURE_DIR");
        const char* logsOnly=std::getenv("RR_EVIDENCE_LOGS_ONLY");
        const bool capturing=!(logsOnly&&std::string(logsOnly)=="1")&&capture&&*capture&&captures_<2&&(captures_==0||elapsed>=.5);
        std::vector<unsigned char> rgb;std::ofstream out;
        if(capturing){rgb.resize(static_cast<std::size_t>(info.drawableWidth)*info.drawableHeight*3);
            const auto directory=std::filesystem::path(capture)/("loading-session-"+std::to_string(sessionIndex_+1));
            if(captures_==0&&!std::filesystem::create_directory(directory))return false;
            const auto path=directory/("loading-"+code_+"-"+std::to_string(captures_+1)+".ppm");
            if(std::filesystem::exists(path))return false;
            out.open(path,std::ios::binary);if(!out)return false;}
        I readFbo=0,drawFbo=0,texture=0,unpack=0;gl_.get(0x8CAA,&readFbo);gl_.get(0x8CA6,&drawFbo);gl_.get(0x8069,&texture);gl_.get(0x88EF,&unpack);
        constexpr U fields[]={0x0CF5,0x0CF2,0x0CF3,0x0CF4};I unpackState[4]{};
        for(unsigned i=0;i<4;++i){gl_.get(fields[i],&unpackState[i]);gl_.pixelStore(fields[i],i==0?1:0);}
        const bool scissor=gl_.enabled(0x0C11)!=0,srgb=gl_.enabled(0x8DB9)!=0;
        gl_.disable(0x0C11);gl_.disable(0x8DB9);gl_.bindBuffer(0x88EC,0);
        U image=0,fbo=0;gl_.genTextures(1,&image);gl_.bindTexture(0x0DE1,image);
        gl_.parameter(0x0DE1,0x2801,0x2600);gl_.parameter(0x0DE1,0x2800,0x2600);
        gl_.image(0x0DE1,0,0x8058,640,448,0,0x1908,0x1401,pixels_.data());
        gl_.genFbos(1,&fbo);gl_.bindFbo(0x8CA8,fbo);gl_.attach(0x8CA8,0x8CE0,0x0DE1,image,0);gl_.readBuffer(0x8CE0);
        bool okay=gl_.checkFbo(0x8CA8)==0x8CD5;
        if(okay){gl_.bindFbo(0x8CA9,0);gl_.blit(0,448,640,0,0,0,info.drawableWidth,info.drawableHeight,0x4000,0x2600);}
        // Read only this process's default framebuffer, immediately before swap.
        if(okay&&capturing){I pack=0,alignment=0,row=0,skipRows=0,skipPixels=0,readBuffer=0;
            gl_.get(0x88ED,&pack);gl_.get(0x0D05,&alignment);gl_.get(0x0D02,&row);gl_.get(0x0D03,&skipRows);gl_.get(0x0D04,&skipPixels);
            gl_.bindFbo(0x8CA8,0);gl_.get(0x0C02,&readBuffer);gl_.readBuffer(0x0405);gl_.bindBuffer(0x88EB,0);
            gl_.pixelStore(0x0D05,1);gl_.pixelStore(0x0D02,0);gl_.pixelStore(0x0D03,0);gl_.pixelStore(0x0D04,0);
            gl_.readPixels(0,0,info.drawableWidth,info.drawableHeight,0x1907,0x1401,rgb.data());
            gl_.readBuffer(static_cast<U>(readBuffer));gl_.bindBuffer(0x88EB,static_cast<U>(pack));
            gl_.pixelStore(0x0D05,alignment);gl_.pixelStore(0x0D02,row);gl_.pixelStore(0x0D03,skipRows);gl_.pixelStore(0x0D04,skipPixels);
        }
        gl_.bindFbo(0x8CA8,static_cast<U>(readFbo));gl_.bindFbo(0x8CA9,static_cast<U>(drawFbo));
        gl_.bindTexture(0x0DE1,static_cast<U>(texture));gl_.bindBuffer(0x88EC,static_cast<U>(unpack));
        for(unsigned i=0;i<4;++i)gl_.pixelStore(fields[i],unpackState[i]);
        if(scissor)gl_.enable(0x0C11);if(srgb)gl_.enable(0x8DB9);
        gl_.deleteFbos(1,&fbo);gl_.deleteTextures(1,&image);
        GameHostDiagnostic50ER diagnostic;okay=okay&&gl_.error()==0&&host_.swapWindow(diagnostic);
        if(okay&&capturing){out<<"P6\n"<<info.drawableWidth<<' '<<info.drawableHeight<<"\n255\n";
            for(I y=info.drawableHeight;y>0;--y)out.write(reinterpret_cast<const char*>(rgb.data()+(y-1)*info.drawableWidth*3),info.drawableWidth*3);
            out.close();okay=bool(out);if(okay)++captures_;}
        return okay;
    }catch(const std::exception& e){std::fprintf(stderr,"original_loading_failure=%s\n",e.what());return false;}
};
}
