#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rr::assets::detail {
// Pure CPU translation of the recovered TGA conversion paths. This is not
// archive/member authentication or resource admission: the provider still owns
// identity and each consumer owns its exact supported source shape.
// Bounded square power-of-two16..256 images cover the reviewed callers.
// Readers own their scratch allocation; converters/mips require already-sized
// distinct output buffers and never allocate. Invalid arguments do not mutate.
struct OriginalTgaConversion50LA final {
    static bool dimensions(std::size_t width,std::size_t height,std::size_t minimum=16) noexcept {
        return width==height&&width>=minimum&&width<=256&&!(width&(width-1));
    }
    // 29EA90/29ECD8 (and 16C0D0) read any square power-of-two TGA; 251040 starts
    // one level and 251074's minimum-16 comparison only stops the mip chain, so
    // an 8x8 source is read and converted unchanged (no mips). Readers default to
    // the reviewed 16 minimum; the static-world reader passes readerMinimum.
    static constexpr std::size_t readerMinimum=8;
    static bool sourceShape(const std::vector<std::uint8_t>& source,
        std::size_t width,std::size_t height,std::uint8_t depth,std::size_t minimum=16) noexcept {
        if(!dimensions(width,height,minimum)||(depth!=24&&depth!=32))return false;
        const auto bytesPerPixel=std::size_t(depth/8);
        // Exact type2, no ID/palette, origin(0,0), bottom-origin subset.
        // The retained26-byte footer is not converted or interpreted.
        if(source.size()!=18+width*height*bytesPerPixel+26)return false;
        if(source[0]||source[1]||source[2]!=2)return false;
        for(std::size_t i=3;i<12;++i)if(source[i])return false;
        const auto sourceWidth=std::size_t(source[12])|(std::size_t(source[13])<<8);
        const auto sourceHeight=std::size_t(source[14])|(std::size_t(source[15])<<8);
        return sourceWidth==width&&sourceHeight==height&&source[16]==depth&&
            source[17]==(depth==24?0:8);
    }
    //29EA90 swaps B/R;29ECD8 reverses bottom-origin rows. Scratch still holds
    // reader RGB until the later24E0E0 step in convert1555_16C0D0.
    static bool read24_29EA90(const std::vector<std::uint8_t>& source,
        std::size_t width,std::size_t height,std::vector<std::uint8_t>& rgb,std::size_t minimum=16) {
        if(&source==&rgb||!sourceShape(source,width,height,24,minimum))return false;
        rgb.resize(width*height*3);
        for(std::size_t y=0;y<height;++y)for(std::size_t x=0;x<width;++x){
            const auto from=18+((height-1-y)*width+x)*3,to=(y*width+x)*3;
            rgb[to]=source[from+2];rgb[to+1]=source[from+1];rgb[to+2]=source[from];
        }
        return true;
    }
    //29EE40/29EF14/29F060: BGRA -> top-origin RGBA, alpha byte unchanged.
    //24E0E0 is a no-op for8888;16A530 reduction0 copies these bytes.
    static bool read32_29EE40(const std::vector<std::uint8_t>& source,
        std::size_t width,std::size_t height,std::vector<std::uint8_t>& rgba,std::size_t minimum=16) {
        if(&source==&rgba||!sourceShape(source,width,height,32,minimum))return false;
        rgba.resize(width*height*4);
        for(std::size_t y=0;y<height;++y)for(std::size_t x=0;x<width;++x){
            const auto from=18+((height-1-y)*width+x)*4,to=(y*width+x)*4;
            rgba[to]=source[from+2];rgba[to+1]=source[from+1];
            rgba[to+2]=source[from];rgba[to+3]=source[from+3];
        }
        return true;
    }
    static bool convert1555_16C0D0(std::vector<std::uint8_t>& rgb,
        std::size_t width,std::size_t height,std::vector<std::uint16_t>& out,std::size_t minimum=16) noexcept {
        if(!dimensions(width,height,minimum)||rgb.size()!=width*height*3||
           out.size()!=width*height)return false;
        for(std::size_t i=0;i<width*height;++i){
            //2511B0->24E0E0 mutates retained reader scratch back to BGR.
            std::swap(rgb[3*i],rgb[3*i+2]);
            const auto b=rgb[3*i],g=rgb[3*i+1],r=rgb[3*i+2];
            const auto rgb5=static_cast<std::uint16_t>((std::uint16_t(b>>3)<<10)|
                (std::uint16_t(g>>3)<<5)|std::uint16_t(r>>3));
            //16C128..16C150 compares FULL bytes before quantization.
            //29E3B4's00FF00FF key becomes7C1F with coverage clear.
            out[i]=b==255&&g==0&&r==255?std::uint16_t(0x7c1f):
                static_cast<std::uint16_t>(rgb5|0x8000u);
        }
        return true;
    }
    static bool mip1555_168980(const std::vector<std::uint16_t>& source,
        std::size_t width,std::size_t height,std::vector<std::uint16_t>& out) noexcept {
        if(&source==&out||!dimensions(width,height)||width<32||
           source.size()!=width*height||out.size()!=(width/2)*(height/2))return false;
        const auto nextWidth=width/2;
        for(std::size_t y=0;y<height/2;++y)for(std::size_t x=0;x<nextWidth;++x){
            std::uint32_t red=0,green=0,blue=0,count=0;
            for(std::size_t dy=0;dy<2;++dy)for(std::size_t dx=0;dx<2;++dx){
                const auto word=source[(2*y+dy)*width+2*x+dx];
                if(!(word&0x8000u))continue;
                red+=(word&31u)<<3;green+=((word>>5)&31u)<<3;
                blue+=((word>>10)&31u)<<3;++count;
            }
            //168AD4 discards fewer than two covered inputs. Original positive
            // integer DIV/DIV1 uses bias4*count before reducing to5 bits.
            out[y*nextWidth+x]=count<2?std::uint16_t(0):static_cast<std::uint16_t>(
                0x8000u|(((blue+4*count)/count>>3)<<10)|
                (((green+4*count)/count>>3)<<5)|((red+4*count)/count>>3));
        }
        return true;
    }
    static bool mip8888_168470(const std::vector<std::uint8_t>& source,
        std::size_t width,std::size_t height,std::vector<std::uint8_t>& out) noexcept {
        if(&source==&out||!dimensions(width,height)||width<32||
           source.size()!=width*height*4||out.size()!=(width/2)*(height/2)*4)return false;
        const auto nextWidth=width/2;
        for(std::size_t y=0;y<height/2;++y)for(std::size_t x=0;x<nextWidth;++x){
            const auto top=(2*y*width+2*x)*4,bottom=top+width*4;
            const std::size_t offsets[4]={top,top+4,bottom,bottom+4};
            std::uint32_t red=0,green=0,blue=0,alpha=0,count=0;
            for(std::size_t i=0;i<4;++i){
                const auto from=offsets[i];
                if(!source[from+3])continue;
                red+=source[from];green+=source[from+1];blue+=source[from+2];
                // Original quirk, NOT a corrected box filter:
                //168568 tests bottom-right alpha;168574 reuses s4 containing
                // bottom-left alpha from168534. RGB/count use bottom-right.
                alpha+=source[(i==3?bottom:from)+3];++count;
            }
            const auto to=(y*nextWidth+x)*4;
            //1685EC..16869C truncates each positive sum/count, without bias.
            out[to]=static_cast<std::uint8_t>(count?red/count:0);
            out[to+1]=static_cast<std::uint8_t>(count?green/count:0);
            out[to+2]=static_cast<std::uint8_t>(count?blue/count:0);
            out[to+3]=static_cast<std::uint8_t>(count?alpha/count:0);
        }
        return true;
    }
};
}
