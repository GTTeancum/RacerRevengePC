#pragma once
#include "original_water_topology_numeric50la.h"
#include <bit>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <optional>

namespace rr::game {
// Native buffers for one24-byte row of2998D0. Independent original allocation
// and read histories belong to the caller. This class never admits a source
// return, cursor, pointer word, or ownership relationship on its own.
class OriginalWaterRowStorage50LA final {
    struct Free {void operator()(void* p)const noexcept {std::free(p);}};
    std::unique_ptr<void,Free> normals_,vertices_,output_;
    std::unique_ptr<std::uint8_t[]> normalCopied_;
    std::uint32_t count_=0,nextNormal_=0;
    std::size_t vertexCopied_=0;
    bool normalsAttempted_=false,verticesAttempted_=false,outputAttempted_=false;
    bool vertexRead_=false,calculationAttempted_=false,calculated_=false;
    static float load(const void* storage,std::size_t offset)noexcept {
        const auto* p=static_cast<const std::uint8_t*>(storage)+offset;
        std::uint32_t word=0;for(unsigned i=0;i<4u;++i)word|=std::uint32_t(p[i])<<(8u*i);
        return std::bit_cast<float>(word);
    }
public:
    OriginalWaterRowStorage50LA()noexcept=default;
    OriginalWaterRowStorage50LA(const OriginalWaterRowStorage50LA&)=delete;
    OriginalWaterRowStorage50LA& operator=(const OriginalWaterRowStorage50LA&)=delete;
    bool allocateNormals(std::uint32_t count)noexcept {
        if(normalsAttempted_)return false;normalsAttempted_=true;
        if(count>16384u)return false;
        count_=count;
        if(count){normalCopied_.reset(new(std::nothrow) std::uint8_t[count]);if(!normalCopied_)return false;
            std::memset(normalCopied_.get(),255,count);}
        normals_.reset(std::malloc(std::size_t(count)*16u));return bool(normals_);
    }
    bool allocateVertices()noexcept {
        if(!normals_||verticesAttempted_)return false;verticesAttempted_=true;
        vertices_.reset(std::malloc(std::size_t(count_)*32u));return bool(vertices_);
    }
    bool copyVertexPrefix(std::span<const std::uint8_t> copied)noexcept {
        if(!vertices_||vertexRead_||copied.size()>std::size_t(count_)*32u)return false;
        if(!copied.empty())std::memcpy(vertices_.get(),copied.data(),copied.size());
        vertexCopied_=copied.size();vertexRead_=true;return true;
    }
    bool copyNormalPrefix(std::uint32_t ordinal,std::span<const std::uint8_t> copied)noexcept {
        if(!normals_||!vertexRead_||ordinal!=nextNormal_||ordinal>=count_||copied.size()>12u)return false;
        if(!copied.empty())std::memcpy(static_cast<std::uint8_t*>(normals_.get())+std::size_t(ordinal)*16u,copied.data(),copied.size());
        normalCopied_[ordinal]=std::uint8_t(copied.size());++nextNormal_;return true;
    }
    bool allocateOutput()noexcept {
        if(!vertexRead_||nextNormal_!=count_||outputAttempted_)return false;outputAttempted_=true;
        output_.reset(std::malloc(std::size_t(count_)*16u));
        if(!output_)return false;
        std::memset(output_.get(),0,std::size_t(count_)*16u);return true;
    }
    water_topology_numeric50LA::Result calculateXYZ()noexcept {
        namespace w=water_topology_numeric50LA;
        w::Result result;
        if(!output_||calculationAttempted_)return result;calculationAttempted_=true;
        if(count_<2u)return result;
        std::vector<w::Vertex> vertices;std::vector<w::Vec3> normals,output;
        try {vertices.resize(count_);normals.resize(count_);output.resize(count_);}catch(...){return result;}
        for(std::uint32_t i=0;i<count_;++i){
            if(normalCopied_[i]<12u||normalCopied_[i]==255u)return result;
            for(unsigned lane=0;lane<3u;++lane)normals[i][lane]=load(normals_.get(),std::size_t(i)*16u+lane*4u);
            if(count_>2u){
                //The overlapping triples consume XYZ and UV of every vertex.
                //Unknown padding/unused words do not make those lanes known.
                const auto at=std::size_t(i)*32u;
                if(vertexCopied_<at+32u)return result;
                for(unsigned lane=0;lane<3u;++lane)vertices[i].position[lane]=load(vertices_.get(),at+lane*4u);
                vertices[i].uv={load(vertices_.get(),at+24u),load(vertices_.get(),at+28u)};
            } //For N=2 the source executes only tails; vertex fields are unused.
        }
        result=w::prepare(vertices,normals,output);if(!result.prepared)return result;
        auto* bytes=static_cast<std::uint8_t*>(output_.get());
        for(std::uint32_t i=0;i<count_;++i)for(unsigned lane=0;lane<3u;++lane){
            const auto word=std::bit_cast<std::uint32_t>(output[i][lane]);
            for(unsigned j=0;j<4u;++j)bytes[std::size_t(i)*16u+lane*4u+j]=std::uint8_t(word>>(8u*j));
        }
        calculated_=true;return result;
    }
    std::optional<std::uint8_t> normalByte(std::uint32_t ordinal,unsigned offset)const noexcept {
        if(!normals_||ordinal>=nextNormal_||offset>=12u||offset>=normalCopied_[ordinal])return std::nullopt;
        return static_cast<const std::uint8_t*>(normals_.get())[std::size_t(ordinal)*16u+offset];
    }
    std::optional<std::uint8_t> vertexByte(std::size_t offset)const noexcept {
        if(!vertices_||!vertexRead_||offset>=vertexCopied_)return std::nullopt;
        return static_cast<const std::uint8_t*>(vertices_.get())[offset];
    }
    std::optional<water_topology_numeric50LA::Vec3> outputXYZ(std::uint32_t ordinal)const noexcept {
        if(!calculated_||ordinal>=count_)return std::nullopt;
        const auto at=std::size_t(ordinal)*16u;
        return water_topology_numeric50LA::Vec3{load(output_.get(),at),load(output_.get(),at+4u),load(output_.get(),at+8u)};
    }
    const void* normalsIdentity()const noexcept{return normals_.get();}
    const void* verticesIdentity()const noexcept{return vertices_.get();}
    const void* outputIdentity()const noexcept{return output_.get();}
    //Output W is deliberately not exposed as a known source result. Its native
    //allocation bytes do not prove the original full-vector store's W value.
};
} //namespace rr::game
