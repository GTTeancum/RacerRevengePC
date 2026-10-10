#pragma once
// Test-only original-instruction interpreter. No native header/formula is used.
#include "../../evidence/authoredInitialization49N/numeric-source/authored_words49n.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
namespace authored49n_oracle {
using U=std::uint32_t;
inline U bits(float f){U u;std::memcpy(&u,&f,4);return u;}
inline float fp(U u){float f;std::memcpy(&f,&u,4);return f;}
inline float arithmetic(float a,float b,U op){volatile float x=0;switch(op){case 0:x=a+b;break;case 1:x=a-b;break;case 2:x=a*b;break;case 3:x=a/b;break;default:throw std::runtime_error("bad scalar opcode");}return x;}
struct Parsed {std::array<U,4> words{};bool present=true;bool vector=false;};
struct Boundary {U caller,target,destination;std::string section,key;bool present;};
struct Machine {
 static constexpr U vehicle=0x1000,body=0x3000,engine=0x4000,aero=0x5000,reader=0x6000,track=0x7001,application=0x8000,vtable=0x9000,stack=0xf000,manager=0x10000,stop=0xfffffffcu;
 std::array<std::array<U,4>,32> g{};
 std::array<float,32> f{};
 std::array<std::array<float,4>,32> v{};
 std::array<U,0x23000/4> memory{};
 std::array<bool,0x23000/4> initialized{};
 std::array<std::uint64_t,authored49n_words::total_words> hits{},taken{},notTaken{};
 std::map<std::string,Parsed> parsed;
 std::vector<Boundary> boundaries;
 std::string section="Physical Properties";
 bool tuningSection=true,condition=false,annul=false;
 U cursor=stop,pc=0,stepsInRun=0,callbackSalt=0;
 float accumulator=0;std::uint64_t steps=0;
 Machine(){v[0]={0,0,0,1};g[29][0]=stack;g[31][0]=stop;g[20][0]=reader;g[21][0]=vehicle;}
 std::size_t index(U p) const {for(const auto& b:authored49n_words::blocks)if(p>=b.start&&p<b.start+4*b.count&&!(p&3))return b.index+(p-b.start)/4;throw std::runtime_error("unselected PC "+std::to_string(p));}
 U word(U p) const {for(const auto& b:authored49n_words::blocks)if(p>=b.start&&p<b.start+4*b.count&&!(p&3))return b.words[(p-b.start)/4];throw std::runtime_error("unselected PC");}
 U read(U a) const {
  if(a==0x382b80)return application;
  const U lo=authored49n_words::reader_key_strings_start,hi=lo+sizeof(authored49n_words::reader_key_strings);
  if(a>=lo&&a+4<=hi){U result=0;for(unsigned i=0;i<4;++i)result|=U(authored49n_words::reader_key_strings[a-lo+i])<<(8*i);return result;}
  if(a%4||a/4>=memory.size()||!initialized[a/4])throw std::runtime_error("uninitialized/outside supplied numeric memory "+std::to_string(a)+" PC "+std::to_string(pc));
  return memory[a/4];
 }
 unsigned char byte(U a) const {const U lo=authored49n_words::reader_key_strings_start;if(a>=lo&&a<lo+sizeof(authored49n_words::reader_key_strings))return authored49n_words::reader_key_strings[a-lo];return static_cast<unsigned char>(read(a&~3u)>>((a&3u)*8));}
 std::string string(U a) const {std::string s;for(unsigned n=0;n<=255;++n){const auto c=byte(a+n);if(!c)return s;s.push_back(char(c));}throw std::runtime_error("unbounded string");}
 void write(U a,U w){if(a%4||a/4>=memory.size())throw std::runtime_error("outside synthetic store");initialized[a/4]=true;memory[a/4]=w;}
 void store(U a,float f0){write(a,bits(f0));}
 float load(U a) const{return fp(read(a));}
 void text(U a,const std::string& s){for(U p=a&~3u;p<((a+U(s.size())+4)&~3u);p+=4)write(p,0);for(unsigned i=0;i<s.size();++i){const U p=(a+i)&~3u,shift=((a+i)&3u)*8;write(p,(read(p)&~(255u<<shift))|(U(static_cast<unsigned char>(s[i]))<<shift));}}
 void scalar(const std::string& s,const std::string& k,float value,bool present=true){parsed[s+"/"+k]={{{bits(value),0,0,0}},present,false};}
 void integer(const std::string& k,int value,bool present=true){parsed["Tuning/"+k]={{{U(value),0,0,0}},present,false};}
 void vector(const std::string& k,const std::array<float,4>& value,bool present=true){Parsed p;p.present=present;p.vector=true;for(unsigned i=0;i<4;++i)p.words[i]=bits(value[i]);parsed["Engine/"+k]=p;}
 void clobber(){ // Explicit caller-saved state: repeat runs with another salt prove it is dead.
  for(unsigned r:{1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,11u,12u,13u,14u,15u,24u,25u})g[r]={callbackSalt,callbackSalt,callbackSalt,callbackSalt};
  for(unsigned r=0;r<20;++r)f[r]=fp(0x3e800000u|(callbackSalt&0x000fffffu));
  accumulator=fp(0x3e800000u|(callbackSalt&0x000fffffu));condition=false;
 }
 bool boundary(){
  if(cursor==0x187330){if(g[4][0]!=manager)throw std::runtime_error("track accessor target");boundaries.push_back({g[31][0]-8,cursor,track,"context","track",true});clobber();g[2][0]=track;cursor=g[31][0];return true;}
  if(cursor==0x157d00){if(g[4][0]!=reader)throw std::runtime_error("section reader pointer");const auto name=string(g[5][0]);const bool present=name!="Tuning"||tuningSection;boundaries.push_back({g[31][0]-8,cursor,0,section,name,present});if(present)section=name;clobber();g[2][0]=present?1:0;cursor=g[31][0];return true;}
  if(cursor!=0x158a70&&cursor!=0x158920&&cursor!=0x158c00)return false;
  if(g[4][0]!=reader)throw std::runtime_error("value reader pointer");
  const auto key=string(g[5][0]);const auto it=parsed.find(section+"/"+key);if(it==parsed.end())throw std::runtime_error("unsupplied parsed boundary "+section+"/"+key);
  const auto& value=it->second;const U destination=g[cursor==0x158c00?7:6][0];if(value.vector!=(cursor==0x158c00))throw std::runtime_error("wrong reader value type");
  boundaries.push_back({g[31][0]-8,cursor,destination,section,key,value.present});if(value.present)for(unsigned i=0;i<(value.vector?4u:1u);++i)write(destination+4*i,value.words[i]);
  const bool present=value.present;clobber();g[2][0]=present?1:0;cursor=g[31][0];return true;
 }
 bool execute(U p,U& target){
  pc=p;++steps;if(++stepsInRun>50000)throw std::runtime_error("instruction budget");const auto ix=index(p);++hits[ix];annul=false;
  const U w=word(p),op=w>>26,rs=w>>21&31,rt=w>>16&31,rd=w>>11&31,sa=w>>6&31,fn=w&63;
  const auto im=std::int32_t(std::int16_t(w&65535));auto address=[&](){return g[rs][0]+U(im);};
  auto branch=[&](bool yes,bool likely=false){++(yes?taken[ix]:notTaken[ix]);target=yes?p+4+U(im*4):p+8;annul=likely&&!yes;return true;};
  if(!w){}
  else if(op==9)g[rt][0]=g[rs][0]+U(im);
  else if(op==12)g[rt][0]=g[rs][0]&(w&65535);
  else if(op==13)g[rt][0]=g[rs][0]|(w&65535);
  else if(op==15)g[rt][0]=w<<16;
  else if(op==0x23)g[rt][0]=read(address());
  else if(op==0x20)g[rt][0]=U(std::int32_t(std::int8_t(byte(address()))));
  else if(op==0x24)g[rt][0]=byte(address());
  else if(op==0x2b)write(address(),g[rt][0]);
  else if(op==0x31)f[rt]=load(address());
  else if(op==0x39)store(address(),f[rt]);
  else if(op==0x1e){const U a=address();for(unsigned i=0;i<4;++i)g[rt][i]=read(a+4*i);}
  else if(op==0x1f){for(unsigned i=0;i<4;++i)write(address()+4*i,g[rt][i]);}
  else if(op==0x3e){for(unsigned i=0;i<4;++i)store(address()+4*i,v[rt][i]);}
  else if(op==4||op==0x14)return branch(g[rs][0]==g[rt][0],op==0x14);
  else if(op==5||op==0x15)return branch(g[rs][0]!=g[rt][0],op==0x15);
  else if(op==7)return branch(std::int32_t(g[rs][0])>0);
  else if(op==3){g[31][0]=p+8;target=(w&0x3ffffff)*4;return true;}
  else if(op==0){switch(fn){
   case 0:g[rd][0]=g[rt][0]<<sa;break;case 3:g[rd][0]=U(std::int32_t(g[rt][0])>>sa);break;
   case 8:target=g[rs][0];return true;case 9:target=g[rs][0];g[rd][0]=p+8;return true;
   case 0x18:g[rd][0]=U(std::int64_t(std::int32_t(g[rs][0]))*std::int64_t(std::int32_t(g[rt][0])));break;
   case 0x21:g[rd][0]=g[rs][0]+g[rt][0];break;case 0x23:g[rd][0]=g[rs][0]-g[rt][0];break;case 0x25:g[rd][0]=g[rs][0]|g[rt][0];break;
   default:throw std::runtime_error("special opcode "+std::to_string(w));}
  }else if((w&0xfc0007ffu)==0x70000628u){for(unsigned i=0;i<4;++i)g[rd][i]=g[rs][i]|g[rt][i];}
  else if(op==17){
   if(rs==4)f[rd]=fp(g[rt][0]);
   else if(rs==8)return branch(condition==bool(rt&1));
   else if(rs==16){switch(fn){
    case 0:case 1:case 2:case 3:f[sa]=arithmetic(f[rd],f[rt],fn);break;
    case 6:f[sa]=f[rd];break;case 7:f[sa]=-f[rd];break;
    case 0x18:case 0x19:case 0x1a:accumulator=arithmetic(f[rd],f[rt],fn-0x18);break;
    case 0x1c:f[sa]=arithmetic(accumulator,arithmetic(f[rd],f[rt],2),0);break;
    case 0x1d:f[sa]=arithmetic(accumulator,arithmetic(f[rd],f[rt],2),1);break;
    case 0x32:condition=f[rd]==f[rt];break;case 0x34:condition=f[rd]<f[rt];break;case 0x36:condition=f[rd]<=f[rt];break;
    default:throw std::runtime_error("FPU opcode "+std::to_string(w));}
   }else throw std::runtime_error("COP1 opcode");
  }else throw std::runtime_error("unsupported word "+std::to_string(w)+" at "+std::to_string(p));
  g[0]={};v[0]={0,0,0,1};return false;
 }
 void step(){if(boundary())return;U target=0,unused=0;if(execute(cursor,target)){const bool skip=annul;if(!skip&&execute(cursor+4,unused))throw std::runtime_error("branch delay");cursor=target;}else cursor+=4;}
 void until(U end){stepsInRun=0;while(cursor!=end){if(cursor==stop)throw std::runtime_error("unexpected return");step();}}
 void interval(U begin,U end){cursor=begin;until(end);}
 void call(U entry,U a0,U a1=0){g[4][0]=a0;g[5][0]=a1;g[31][0]=stop;interval(entry,stop);}
};
}
