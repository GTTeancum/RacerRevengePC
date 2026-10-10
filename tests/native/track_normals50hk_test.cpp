#include "track_normals50hk_fixture.h"
#include <iostream>
#include <limits>
#include <stdexcept>
static void check(bool ok,const char* what) {if(!ok)throw std::runtime_error(what);}
int main() {
 try {
  using rr::assets::WorldNormal50HK; using rr::assets::transformWorldNormal50HK;
  std::array<double,16> matrix{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
  WorldNormal50HK result;
  check(transformWorldNormal50HK({{0,3,0}},matrix,result) && result.xyz[1]==1,"identity normal normalization");
  matrix[0]=2;matrix[5]=3;matrix[10]=4;
  check(transformWorldNormal50HK({{1,1,0}},matrix,result),"nonuniform normal accepted");
  check(std::abs(result.xyz[0]*2-result.xyz[1]*3)<1e-6,"inverse transpose scale");
  matrix={{1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,1}};
  check(transformWorldNormal50HK({{1,0,0}},matrix,result) &&
      std::abs(result.xyz[0]+result.xyz[1])<1e-6,"inverse transpose shear perpendicularity");
  matrix={{-1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
  check(transformWorldNormal50HK({{1,0,0}},matrix,result) && result.xyz[0]==-1,"reflected normal sign");
  check(transformWorldNormal50HK({{0,0,0}},matrix,result) && result.xyz[0]==0 && result.xyz[1]==0,"zero normal fallback");
  result.xyz[0]=7;matrix[0]=0;
  check(!transformWorldNormal50HK({{1,0,0}},matrix,result) && result.xyz[0]==7,"singular rejection transactional");
  matrix[0]=1;matrix[2]=std::numeric_limits<double>::quiet_NaN();
  check(!transformWorldNormal50HK({{1,0,0}},matrix,result),"nonfinite rejection");
  rr::assets::Mesh48B mesh;std::string why;
  check(rr::assets::loadMesh48B("evidence/visual49G/export-placed-normal/ta-world.rrmesh",mesh,why),why.c_str());
  const auto vertices=mesh.vertices;const auto indices=mesh.indices;
  std::vector<WorldNormal50HK> normals;track_normals50hk_fixture::Diagnostic d;
  check(track_normals50hk_fixture::load("evidence/visual49G","evidence/visual49G/export-placed-normal/export-report.json",mesh,normals,d),d.message.c_str());
  check(normals.size()==mesh.vertices.size() && d.scenes==228 && d.instances==284 && d.surfaces==353,"full source normal coverage");
  for(const auto& n:normals) {
   const double length=std::hypot(n.xyz[0],n.xyz[1],n.xyz[2]);
   check(length==0 || std::abs(length-1)<2e-6,"all source normals unit or explicit zero");
  }
  check(indices==mesh.indices && std::memcmp(vertices.data(),mesh.vertices.data(),vertices.size()*sizeof(vertices[0]))==0,"source mesh not changed");
  std::cout<<"PASS source scenes="<<d.scenes<<" instances="<<d.instances<<" surfaces="<<d.surfaces
      <<" normals="<<d.normals<<" zero="<<d.zeroNormals<<" source_bytes="<<d.sourceBytes<<'\n';
  std::map<std::array<float,3>,WorldNormal50HK> atPosition;
  std::uint32_t retainedDiscontinuities=0, geometricAligned=0, geometricOpposed=0;
  for(std::size_t i=0;i<mesh.vertices.size();++i) {
   const auto& p=mesh.vertices[i].position;
   const auto [it,inserted]=atPosition.emplace(std::array<float,3>{{p[0],p[1],p[2]}},normals[i]);
   if(!inserted) {
    double dot=0;for(unsigned c=0;c<3;++c)dot+=it->second.xyz[c]*normals[i].xyz[c];
    if(dot<.98)++retainedDiscontinuities;
   }
  }
  for(std::size_t i=0;i<mesh.indices.size();i+=3) {
   const auto ia=mesh.indices[i],ib=mesh.indices[i+1],ic=mesh.indices[i+2];
   const auto& a=mesh.vertices[ia].position;const auto& b=mesh.vertices[ib].position;const auto& c=mesh.vertices[ic].position;
   double u[3],v[3],n[3];for(unsigned k=0;k<3;++k){u[k]=b[k]-a[k];v[k]=c[k]-a[k];n[k]=normals[ia].xyz[k]+normals[ib].xyz[k]+normals[ic].xyz[k];}
   const double dot=(u[1]*v[2]-u[2]*v[1])*n[0]+(u[2]*v[0]-u[0]*v[2])*n[1]+(u[0]*v[1]-u[1]*v[0])*n[2];
   if(dot>1e-9)++geometricAligned;else if(dot< -1e-9)++geometricOpposed;
  }
  check(retainedDiscontinuities>0,"authored normal discontinuities retained");
  std::cout<<"PASS retained_normal_discontinuities="<<retainedDiscontinuities<<" geometric_aligned="<<geometricAligned<<" geometric_opposed="<<geometricOpposed<<'\n';
  const auto originalNormal=normals[0];mesh.vertices[0].position[0]+=1.f;
  check(!track_normals50hk_fixture::load("evidence/visual49G","evidence/visual49G/export-placed-normal/export-report.json",mesh,normals,d),"mismatched source position rejected");
  check(normals.size()==vertices.size() && normals[0].xyz[0]==originalNormal.xyz[0],"failed load leaves stream unchanged");
  std::cout<<"PASS identity rejection, transactional output, inverse transpose, reflection, zero and finite checks\n";
 } catch(const std::exception& e) {std::cerr<<e.what()<<'\n';return 1;}
}
