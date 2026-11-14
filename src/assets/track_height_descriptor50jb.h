#pragma once
#include "import_world48f.h"
#include <array>
#include <cstring>

namespace rr::assets {
// The consumed string subset of the actual276C00 track descriptor. This is
// not a replacement for its unrelated sky/ecosystem/material construction.
struct TrackHeightString50JB {
    std::array<char,260> value{};
    bool defaulted=false;
    std::size_t entryOffset=0,valueOffset=0;
    std::uint32_t valueBytes=0;
};
struct TrackHeightDescriptor50JB {
    TrackHeightString50JB terrainFile0C{},waterFile110{};
    std::size_t environmentBlock=0;
    bool bound=false;
};
enum class TrackHeightReadStatus50JB : std::uint8_t {
    Complete,MissingEnvironment,Duplicate,InvalidProvenance,UnsupportedString
};
namespace track_height50jb_detail {
inline bool extent(std::size_t at,std::size_t size,std::size_t total) noexcept {
    return at<=total&&size<=total-at;
}
inline std::uint32_t u32(const std::vector<std::uint8_t>& bytes,std::size_t at) noexcept {
    return std::uint32_t(bytes[at])|(std::uint32_t(bytes[at+1])<<8)|
        (std::uint32_t(bytes[at+2])<<16)|(std::uint32_t(bytes[at+3])<<24);
}
inline bool rawString(const std::vector<std::uint8_t>& raw,std::size_t at,
    std::size_t size,const std::string& value) noexcept {
    if(!extent(at,size,raw.size())||value.size()>=size)return false;
    if(std::memcmp(raw.data()+at,value.data(),value.size()))return false;
    for(std::size_t i=value.size();i<size;++i)if(raw[at+i])return false;
    return true;
}
inline TrackHeightReadStatus50JB readString(const ParameterDocument48F& doc,
    const ParameterBlock48F& block,const char* label,TrackHeightString50JB& output) noexcept {
    const ParameterField48F* found=nullptr;
    for(const auto& field:block.fields)if(field.name==label){
        if(found)return TrackHeightReadStatus50JB::Duplicate;found=&field;
    }
    TrackHeightString50JB value{};
    if(!found){
        //158AD0 copies this exact default after158240 reports not found.
        std::memcpy(value.value.data(),"none",5);value.defaulted=true;
        output=value;return TrackHeightReadStatus50JB::Complete;
    }
    if(found->value.size()>=value.value.size())return TrackHeightReadStatus50JB::UnsupportedString;
    // The admitted compiled-parameter values are plain strings. Quoted/escape
    // variants are a boundary until their original158240 treatment is joined.
    for(unsigned char c:found->value)
        if(c<32||c>=127||c=='"')return TrackHeightReadStatus50JB::UnsupportedString;
    const auto& raw=doc.original_bytes;
    if(!extent(found->entry_offset,43,raw.size())||
       !rawString(raw,found->entry_offset,35,found->name)||
       !rawString(raw,found->value_offset,found->value_bytes,found->value)||
       found->value_bytes!=found->value.size()+1u||block.offset>raw.size()||
       raw.size()-block.offset<4u)return TrackHeightReadStatus50JB::InvalidProvenance;
    const auto base=block.offset+4u;
    if(found->value_offset<base||u32(raw,found->entry_offset+35u)!=found->value_offset-base||
       u32(raw,found->entry_offset+39u)!=found->value_bytes)
        return TrackHeightReadStatus50JB::InvalidProvenance;
    std::memcpy(value.value.data(),found->value.data(),found->value.size());
    value.entryOffset=found->entry_offset;value.valueOffset=found->value_offset;
    value.valueBytes=found->value_bytes;output=value;
    return TrackHeightReadStatus50JB::Complete;
}
}
// Cold source subset over the already-authenticated, decoded original SCB.
// Environment must exist, as required276C34. A missing field is a genuine
// source default, not inferred absence of a height provider. The full decoded
// document's raw provenance is retained; no source bytes are embedded here.
// Distinct input/output; transactional result. No files, loading or allocation.
inline TrackHeightReadStatus50JB readTrackHeightDescriptor50JB(
    const ParameterDocument48F& doc,TrackHeightDescriptor50JB& out) noexcept {
    using Status=TrackHeightReadStatus50JB;using namespace track_height50jb_detail;
    // A document decoded with duplicate labels retained (MC's SCB) is admitted:
    // the Environment block and every label read here are still checked one by
    // one below (a duplicated Environment or WaterFile is refused as Duplicate),
    // so a duplicate elsewhere in the document cannot change what is read.
    if(doc.version!=1u)return Status::InvalidProvenance;
    const ParameterBlock48F* environment=nullptr;std::size_t index=0;
    for(std::size_t i=0;i<doc.blocks.size();++i)if(doc.blocks[i].name=="environment"){
        if(environment)return Status::Duplicate;environment=&doc.blocks[i];index=i;
    }
    if(!environment)return Status::MissingEnvironment;
    const auto& raw=doc.original_bytes;const auto& block=*environment;
    if(!extent(block.table_entry_offset,8u,raw.size())||
       block.table_entry_offset!=doc.header_table_offset+8u*index||
       u32(raw,block.table_entry_offset)!=block.offset||
       u32(raw,block.table_entry_offset+4u)!=block.size_without_count||
       !extent(block.offset,4u,raw.size())||
       !extent(block.offset+4u,block.size_without_count,raw.size())||
       !rawString(raw,block.name_offset,block.name.size()+1u,block.name)||
       u32(raw,block.offset)!=block.fields.size())return Status::InvalidProvenance;
    // Checking every source field prevents an omitted native field from being
    // mistaken for158240's authentic missing-field result.
    if(block.fields.size()>block.size_without_count/43u)return Status::InvalidProvenance;
    for(std::size_t i=0;i<block.fields.size();++i){const auto& f=block.fields[i];
        if(f.entry_offset!=block.offset+4u+43u*i||
           !rawString(raw,f.entry_offset,35u,f.name))return Status::InvalidProvenance;
    }
    TrackHeightDescriptor50JB staged{};staged.environmentBlock=index;
    auto status=readString(doc,block,"terrainfile",staged.terrainFile0C);
    if(status!=Status::Complete)return status;
    status=readString(doc,block,"waterfile",staged.waterFile110);
    if(status!=Status::Complete)return status;
    staged.bound=true;out=staged;return Status::Complete;
}
} // namespace rr::assets
