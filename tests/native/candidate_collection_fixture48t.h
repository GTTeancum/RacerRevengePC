#pragma once
#include "candidate_collection_fixture48s.h"
#include "group_shape_fixture48t.h"
#include <algorithm>
namespace collection_test48t {
namespace old=collection_test48s;
using namespace shape_test48t;
struct Fixture:old::Fixture {
 std::vector<GroupShapeSource48T> shapeSources;
 std::vector<GroupShapeGeometry48T> geometries;
 std::vector<GroupShapeState48T> sourceShapes,scratchShapes,finalShapes;
 std::map<std::uint32_t,CollectionRef48S> shapeRefs,geometryRefs;
 std::vector<CollectionRef48S> identityOrder,blockOrder;
 explicit Fixture(const Json& row):old::Fixture(row){
  for(const auto& c:row.at("candidates"))if(c.value("shape_tag",1u)==0){const auto sid=c.value("shape_alias_id",c.at("id").get<std::uint32_t>()),gid=c.value("geometry_alias_id",c.at("id").get<std::uint32_t>());if(!shapeRefs.count(sid))shapeRefs.emplace(sid,static_cast<CollectionRef48S>(shapeRefs.size()+1));if(!geometryRefs.count(gid))geometryRefs.emplace(gid,static_cast<CollectionRef48S>(geometryRefs.size()+1));}
  shapeSources.resize(shapeRefs.size());geometries.resize(geometryRefs.size());sourceShapes.resize(shapeRefs.size()+4);scratchShapes.resize(shapeRefs.size()+4);finalShapes.resize(shapeRefs.size()+4);shape_test48t::fill(sourceShapes,0x35);shape_test48t::fill(scratchShapes,0x6d);shape_test48t::fill(finalShapes,0xa5);
  std::vector<bool> gotShape(shapeSources.size()),gotGeometry(geometries.size());
  for(std::size_t k=0;k<row.at("candidates").size();++k){const auto& c=row.at("candidates")[k];if(c.value("shape_tag",1u)!=0)continue;const auto sid=c.value("shape_alias_id",c.at("id").get<std::uint32_t>()),gid=c.value("geometry_alias_id",c.at("id").get<std::uint32_t>());const auto s=shapeRefs.at(sid)-1,g=geometryRefs.at(gid)-1;objects[object(c.at("id"))-1].shape0Ref=s+1;
   if(!gotGeometry[g]){geometries[g]=shape_test48t::geometry(row.at("initial_state").at("candidates")[k].at("shape_geometry_words"));gotGeometry[g]=true;}
   if(!gotShape[s]){const auto& words=row.at("initial_state").at("candidates")[k].at("shape_words");shape_test48t::matrix(shapeSources[s].local40,words,16);shapeSources[s].geometry=c.value("geometry_present",true)?&geometries[g]:nullptr;sourceShapes[s]=shape_test48t::state(words);gotShape[s]=true;}
  }
  bindShapes();
 }
 void bindShapes(){in.shape0Sources=shapeSources.data();in.shape0Count=shapeSources.size();initial.shape0States=sourceShapes.data();initial.shape0Capacity=sourceShapes.size();work.buffers.shape0States=scratchShapes.data();work.buffers.shape0Capacity=scratchShapes.size();destination.shape0States=finalShapes.data();destination.shape0Capacity=finalShapes.size();}
 void hints(unsigned mode){
  if(mode&1u){identityOrder.resize(in.objectCount);for(std::size_t i=0;i<identityOrder.size();++i)identityOrder[i]=static_cast<CollectionRef48S>(i+1);std::sort(identityOrder.begin(),identityOrder.end(),[&](auto a,auto b){return objects[a-1].identity<objects[b-1].identity;});work.identityOrder=identityOrder.data();work.identityOrderCount=identityOrder.size();}
  if(mode&2u){blockOrder.resize(in.poolBlockCount);for(std::size_t i=0;i<blockOrder.size();++i)blockOrder[i]=static_cast<CollectionRef48S>(i+1);std::sort(blockOrder.begin(),blockOrder.end(),[&](auto a,auto b){const auto& x=poolBlocks[a-1];const auto& y=poolBlocks[b-1];const auto ka=x.entryCount?x.firstEntry:0,kb=y.entryCount?y.firstEntry:0;return ka<kb||(ka==kb&&a<b);});work.blockOrder=blockOrder.data();work.blockOrderCount=blockOrder.size();}
 }
};
}
