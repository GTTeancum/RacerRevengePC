#pragma once
// ONE static-world texture rule for the course world loaders that include it
// (TA, GC, TB, TC, MC, RC; MA/SA/GA keep their own copy of the same unpinned
// branch from 2ec225a). Repo CLAUDE.md: courses differ only in the data they load.
//
// The original course load reads each texture member a material names and
// takes its kind, frame count, mip chain and extent from the member's own
// header (PST/ATM container, IPUM mip chain, TGA header); nothing compares a
// checksum or a member name against a table. So here:
//   - decode() derives kind / frame count / extent / mip from the bytes alone;
//   - the ATM frame is the frame the material authors (animatedMapInitialFrame);
//   - every stock table (source pin, decoded-output pin, native regression
//     output pin) is a RECEIPT: it is looked up by member name and source
//     SHA, compared with what the data produced, logged, and never used to
//     choose or refuse anything (course_archive_receipt50ci.h model).
// One line per loader load: "course_static_textures site=XX images=.. stock=1|0".
// A differing comparison logs "course_static_texture_receipt ... stock=0".
//
// PORT STAND-IN (flagged): the native decoders below are the port's (IPU
// intra decode, PST indexed decode, TGA 29EA90/29EE40 readers); their bounds
// (16..1024 power-of-two mips, <=5 IPUM mips, square 16..256 type-2 TGA) are
// port capability limits, refused by capability, never by name or checksum.
#include "pod_materials50ed_fixture.h"
#include "course_archive_receipt50ci.h"
#include "original_world_material_admission50cm.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/original_ipu_decoder.h"
#include "../../src/assets/original_standard_ipum_framing50la.h"
#include "../../src/assets/original_tga_conversion50la.h"
#include "../../src/assets/original_ui_atm50kg.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace course_world_texture50ST {
namespace a=rr::assets;
namespace shared=pod_materials50ed_fixture::detail;
using Bytes=std::vector<std::uint8_t>;

// image() callers without a material (dynamic-material callbacks) pass this:
// the frame is then the stock pin's frame for stock bytes, else frame 0.
inline constexpr std::uint32_t noAuthoredFrame=~0u;

// What the image's own bytes say. kind: 0/1 indexed PST, 2 IPUM, 3 TGA.
struct Shape {
    std::uint32_t kind=0,frameCount=1,frameIndex=0,width=0,height=0,mipIndex=0;
    bool animated=false;
};

// Frame one IPUM PST (mip chain smallest first); returns the largest mip.
inline a::detail::OriginalIpumMipExtent50LA framePst(const char* site,const Bytes& source,std::uint32_t& mipIndex){
    const std::string s=std::string("Original ")+(site?site:"track")+" ";
    const auto range=[&](std::size_t at,std::size_t n){return at<=source.size()&&n<=source.size()-at;};
    const auto word=[&](std::size_t at){shared::need(range(at,4),s+"truncated IPUM PST");return shared::u32(source.data()+at);};
    const auto dimension=[](std::uint32_t n){return n>=16&&n<=1024&&!(n&(n-1));};
    shared::need(source.size()<=8u*1024u*1024u&&word(0)==1&&word(4)==2&&word(8)>=1&&word(8)<=5,
        s+"IPUM PST header rejected");
    const auto count=word(8);std::size_t at=12;std::uint32_t priorW=0,priorH=0;
    a::detail::OriginalIpumMipExtent50LA largest;
    for(std::uint32_t i=0;i<count;++i){
        shared::need(range(at,20),s+"IPUM mip header extent");
        const auto h=word(at),w=word(at+4),bytes=word(at+16);const auto p=at+20;
        shared::need(dimension(w)&&dimension(h)&&!word(at+8)&&!word(at+12)&&
            (!i||(w==priorW*2&&h==priorH*2))&&bytes>=25&&range(p,bytes),s+"IPUM mip extent/order");
        shared::need(!std::memcmp(source.data()+p,"ipum",4)&&word(p+4)==bytes-8&&
            (std::uint32_t(source[p+8])|(std::uint32_t(source[p+9])<<8))==w&&
            (std::uint32_t(source[p+10])|(std::uint32_t(source[p+11])<<8))==h&&
            word(p+12)==1&&source[p+16]==0,s+"IPUM frame header");
        constexpr std::uint8_t end[]{0,0,1,0xb0,0,0,1,0xb1};
        shared::need(!std::memcmp(source.data()+p+bytes-8,end,8),s+"IPUM terminators");
        largest={w,h,at,p,bytes};at=p+bytes;priorW=w;priorH=h;
    }
    shared::need(at==source.size(),s+"IPUM PST trailing bytes");mipIndex=count-1;return largest;
}

// Decode supplied original bytes from their own data. name: normalized member
// name (TEXTURES/X.PST). frameIndex: the material's authored ATM frame.
inline Bytes decode(const char* site,const std::string& name,const Bytes& source,std::uint32_t frameIndex,
    Shape& shape,a::DecodeError48C& error){
    const std::string s=std::string("Original ")+(site?site:"track")+" ";
    shared::need(source.size()>=12&&source.size()<=8u*1024u*1024u,s+"texture source extent");
    Shape next;Bytes output;
    if(name.ends_with(".TGA")){
        // Extent and depth from the TGA's own header (bytes 12..16).
        using Convert=a::detail::OriginalTgaConversion50LA;
        shared::need(source.size()>=18,s+"TGA header extent");
        const std::size_t w=std::size_t(source[12])|(std::size_t(source[13])<<8);
        const std::size_t h=std::size_t(source[14])|(std::size_t(source[15])<<8);
        const auto depth=source[16];
        shared::need(Convert::sourceShape(source,w,h,depth),
            s+"TGA source shape (port reader: type 2 BGR24/BGRA32, square 16..256)");
        a::TextureMip48E image;image.width=static_cast<std::uint32_t>(w);image.height=static_cast<std::uint32_t>(h);
        if(depth==24){
            Bytes rgb;std::vector<std::uint16_t> words(w*h);
            shared::need(Convert::read24_29EA90(source,w,h,rgb)&&Convert::convert1555_16C0D0(rgb,w,h,words),
                s+"BGR24 conversion");
            image.rgba.resize(words.size()*4);
            for(std::size_t i=0;i<words.size();++i){const auto word=words[i];
                image.rgba[4*i]=static_cast<std::uint8_t>((word&31u)<<3);
                image.rgba[4*i+1]=static_cast<std::uint8_t>(((word>>5)&31u)<<3);
                image.rgba[4*i+2]=static_cast<std::uint8_t>(((word>>10)&31u)<<3);
                // PORT STAND-IN: inherited TEXA 255/255, AEM0 (the stock RC
                // consumers'); one rule for every 24-bit TGA, not keyed on a name.
                image.rgba[4*i+3]=255;
            }
        }else shared::need(Convert::read32_29EE40(source,w,h,image.rgba),s+"BGRA32 source-alpha conversion");
        shared::need(a::encodeTextureLevel48E(image,output,error),s+"TGA native framing");
        next.kind=3;next.frameCount=1;next.frameIndex=0;next.mipIndex=0;next.animated=false;
    }else{
        next.animated=name.ends_with(".ATM");
        shared::need(next.animated||name.ends_with(".PST"),s+"image extension unsupported: "+name);
        shared::need(!next.animated||source.size()>=24,s+"ATM header extent");
        next.kind=shared::u32(source.data()+(next.animated?16:4));
        if(next.kind==2){
            a::detail::OriginalIpumMipExtent50LA mip;
            if(next.animated){
                std::vector<a::detail::OriginalIpumFrameExtent50LA> frames;std::size_t bad=0;
                shared::need(a::detail::frameOriginalIpumAtm50LA(source,frames,bad)&&frameIndex<frames.size(),
                    s+"IPUM ATM framing/frame failed at "+std::to_string(bad));
                const auto& frame=frames[frameIndex];next.mipIndex=frame.count-1;mip=frame.mips[next.mipIndex];
                next.frameCount=static_cast<std::uint32_t>(frames.size());next.frameIndex=frameIndex;
            }else mip=framePst(site,source,next.mipIndex);
            a::TextureMip48E decoded;decoded.width=mip.width;decoded.height=mip.height;std::string why;
            shared::need(a::decodeOriginalIpuRgba(source.data()+mip.payload,mip.bytes,mip.width,mip.height,decoded.rgba,why),
                s+"IPUM decode: "+why);
            shared::need(a::encodeTextureLevel48E(decoded,output,error),s+"IPUM output framing: "+error.code);
        }else{
            shared::need(next.kind==0||next.kind==1,s+"indexed texture kind");Bytes pst;
            if(next.animated){
                a::OriginalUiAtm50KG frames;
                shared::need(a::splitOriginalUiAtm50KG(source.data(),source.size(),frames,error)&&frameIndex<frames.frames.size(),
                    s+"indexed ATM framing/frame: "+error.code);
                const auto& frame=frames.frames[frameIndex];
                pst.assign(source.begin()+frame.pstOffset,source.begin()+frame.pstOffset+frame.pstBytes);
                next.frameCount=static_cast<std::uint32_t>(frames.frames.size());next.frameIndex=frameIndex;
            }
            a::Texture48E decoded;
            shared::need(a::decodeTexture48E(next.animated?pst:source,decoded,error)&&!decoded.mips.empty(),
                s+"indexed PST decode: "+error.code);
            next.mipIndex=static_cast<std::uint32_t>(decoded.mips.size()-1);output=std::move(decoded.base_rrtex);
        }
    }
    // Extent from the decoded envelope, bounded before any size arithmetic.
    shared::need(wma50CM::decodedExtent(output,next.width,next.height),s+"decoded image framing");
    shape=next;return output;
}

// A stock source pin applies only when the bytes under its name are its stock
// bytes; otherwise it is a member receipt (logged once when logReceipt) and
// the image is admitted from its own data.
template<class Pin> const Pin* matchedPin(const Pin* pin,const std::string& name,const Bytes& source,
    const std::string& sourceSha,bool logReceipt){
    if(pin&&!(source.size()==pin->sourceBytes&&sourceSha==pin->sourceSha256)){
        if(logReceipt)course_archive_receipt50CI::memberReceipt("static_image_pin",name,source,0,0,
            pin->sourceBytes,pin->sourceSha256);
        pin=nullptr;
    }
    return pin;
}

// Per image: what the stock tables said about the bytes actually read.
struct Receipt {
    bool listed=false;      // a stock source pin exists under this member name
    bool stockBytes=false;  // ...and these are its stock bytes
    bool shapeMatch=true;   // the stock pin's kind/frames/extent/mip/frame equal the data's
    std::uint32_t outputPins=0,outputMatches=0; // decoded-output pins naming these bytes, and matches
};
// Stock source pin vs. the data (receipt only).
template<class Pin> bool compareShape(const char* site,const std::string& name,const Pin& pin,const Shape& data){
    const bool same=pin.kind==data.kind&&pin.frameCount==data.frameCount&&pin.width==data.width&&
        pin.height==data.height&&pin.mipIndex==data.mipIndex&&(!data.animated||pin.initialFrame==data.frameIndex);
    if(!same)std::fprintf(stderr,"course_static_texture_receipt site=%s member=%s check=shape pin=%u/%u/%ux%u/mip%u/frame%u "
        "data=%u/%u/%ux%u/mip%u/frame%u stock=0\n",site?site:"?",name.c_str(),pin.kind,pin.frameCount,pin.width,pin.height,
        pin.mipIndex,pin.initialFrame,data.kind,data.frameCount,data.width,data.height,data.mipIndex,data.frameIndex);
    return same;
}
// A decoded-output pin for these exact source bytes vs. the decoded output (receipt only).
inline bool compareOutput(const char* site,const std::string& name,const char* check,const char* outputSha,
    const Bytes& output,bool extentKnown=false,std::uint32_t w=0,std::uint32_t h=0,std::uint32_t mip=0,const Shape* data=nullptr){
    const bool same=outputSha&&shared::sha256(output)==outputSha&&
        (!extentKnown||(data&&data->width==w&&data->height==h&&data->mipIndex==mip));
    if(!same)std::fprintf(stderr,"course_static_texture_receipt site=%s member=%s check=%s stock=0\n",
        site?site:"?",name.c_str(),check?check:"output");
    return same;
}
// One line per load.
struct Tally {
    std::size_t images=0,listed=0,stockBytes=0,shapeMatches=0,outputPins=0,outputMatches=0;
    void add(const Receipt& r)noexcept{
        ++images;listed+=r.listed;stockBytes+=r.stockBytes;shapeMatches+=r.stockBytes&&r.shapeMatch;
        outputPins+=r.outputPins;outputMatches+=r.outputMatches;
    }
    bool stock()const noexcept{
        return images&&listed==images&&stockBytes==images&&shapeMatches==images&&outputMatches==outputPins;
    }
    void log(const char* site)const noexcept{
        std::fprintf(stderr,"course_static_textures site=%s images=%llu stock_pins=%llu stock_bytes=%llu shape_match=%llu "
            "output_pins=%llu output_match=%llu new=%llu stock=%d\n",site?site:"?",
            static_cast<unsigned long long>(images),static_cast<unsigned long long>(listed),
            static_cast<unsigned long long>(stockBytes),static_cast<unsigned long long>(shapeMatches),
            static_cast<unsigned long long>(outputPins),static_cast<unsigned long long>(outputMatches),
            static_cast<unsigned long long>(images-stockBytes),stock()?1:0);
    }
};
// Material/ATM initial frame vs. the stock pin of the same stock bytes (receipt only).
inline void frameReceipt(const char* site,const std::string& name,std::uint32_t authored,std::uint32_t pinned){
    if(authored!=pinned)std::fprintf(stderr,"course_static_texture_receipt site=%s member=%s check=atm_initial_frame "
        "authored=%u pin=%u stock=0\n",site?site:"?",name.c_str(),authored,pinned);
}

// One pass rule: native AA1 coverage when the pass authors
// onePassAntialiasingOnOff=1 (any material, any texture). valid=false when the
// authored value is neither empty, "0" nor "1".
inline bool authoredOnePassAa1(const nlohmann::json& passFields,bool& valid)noexcept{
    valid=true;
    try{
        bool found=false,on=false;
        for(const auto& field:passFields.at("authored")){
            if(shared::normalized(field.at("name").get<std::string>())!="ONEPASSANTIALIASINGONOFF")continue;
            if(found||!field.at("value").is_string()){valid=false;return false;}
            const auto value=field.at("value").get<std::string>();
            if(value!="0"&&value!="1"&&!value.empty()){valid=false;return false;}
            found=true;on=value=="1";
        }
        return on;
    }catch(...){valid=false;return false;}
}
} // namespace course_world_texture50ST
