#include "model_bindings49o.h"
#include <cstring>

namespace rr::assets {
namespace {
char upper(char c) noexcept {return c>='a'&&c<='z'?char(c-'a'+'A'):c;}
bool identifier(const char* p,std::size_t n) noexcept {
    if(!n||n>49)return false;
    for(std::size_t i=0;i<n;++i){const auto c=static_cast<unsigned char>(p[i]);
        if(c<=32||c>=127||c=='='||c==',')return false;}
    return true;
}
bool equal(const char* a,const char* b,std::size_t n) noexcept {
    std::size_t i=0;for(;i<n&&a[i];++i)if(a[i]!=upper(b[i]))return false;
    return i==n&&a[i]==0;
}
}
bool readMotionNodeBindings49O(const PsgHierarchyView49O& view,
    const char* text,std::size_t bytes,MotionNodeBindings49O& output,
    MotionBindingDiagnostic49O& diagnostic) noexcept {
    diagnostic={MotionBindingError49O::none,0};
    const auto fail=[&](MotionBindingError49O e,std::size_t p){diagnostic={e,p};return false;};
    if(!text||!bytes||bytes>16384)return fail(MotionBindingError49O::limit,0);
    if(!view.nodeCount||view.nodeCount>64)return fail(MotionBindingError49O::hierarchy,0);
    std::int32_t parents[64]{};
    char names[64][50]{};
    for(std::uint32_t i=0;i<view.nodeCount;++i){
        PsgNode49O node{};
        if(!readPsgNode49O(view,i,node))return fail(MotionBindingError49O::hierarchy,i);
        parents[i]=node.parent;
        if((i==0&&node.parent!=-1)||(i&& (node.parent<0||std::uint32_t(node.parent)>=i)))
            return fail(MotionBindingError49O::hierarchy,i);
        std::size_t n=0;while(n<128&&node.name[n])++n;
        if(!identifier(node.name,n))return fail(MotionBindingError49O::name,i);
        for(std::size_t j=0;j<n;++j)names[i][j]=upper(node.name[j]);
    }
    MotionNodeBindings49O result{};
    std::int32_t pending[65]{};std::uint32_t pendingCount=1;pending[0]=-1;
    while(pendingCount){
        const auto parent=pending[--pendingCount];
        for(std::uint32_t i=0;i<view.nodeCount;++i)if(parents[i]==parent){
            if(result.count>=64)return fail(MotionBindingError49O::hierarchy,i);
            auto& row=result.rows[result.count++];row.sceneNode=static_cast<std::uint8_t>(i);
            std::memcpy(row.name,names[i],50);
        }
        for(std::uint32_t i=view.nodeCount;i>0;--i)if(parents[i-1]==parent){
            if(pendingCount>=65)return fail(MotionBindingError49O::hierarchy,i-1);
            pending[pendingCount++]=static_cast<std::int32_t>(i-1);
        }
    }
    if(result.count!=view.nodeCount)return fail(MotionBindingError49O::hierarchy,0);
    std::size_t p=0;
    while(p<bytes){
        const auto start=p;while(p<bytes&&text[p]!='\n')++p;
        auto end=p;if(end>start&&text[end-1]=='\r')--end;
        if(end==start||end-start>255)return fail(MotionBindingError49O::line,start);
        std::size_t sep=start;while(sep<end&&text[sep]!='=')++sep;
        if(sep==end||!identifier(text+start,sep-start))return fail(MotionBindingError49O::line,start);
        std::size_t comma=sep+1;while(comma<end&&text[comma]!=',')++comma;
        if(!identifier(text+sep+1,comma-sep-1))return fail(MotionBindingError49O::line,sep+1);
        if(comma<end&&!identifier(text+comma+1,end-comma-1))return fail(MotionBindingError49O::line,comma+1);
        std::uint32_t row=0;for(;row<result.count;++row)if(equal(result.rows[row].name,text+start,sep-start))break;
        if(row==result.count)return fail(MotionBindingError49O::unknownNode,start);
        auto& target=result.rows[row];std::memset(target.name,0,sizeof target.name);
        for(std::size_t j=sep+1;j<comma;++j)target.name[j-sep-1]=upper(text[j]);
        target.translationFlag38=comma<end&&upper(text[comma+1])=='T';target.listedFlag3c=true;
        if(p<bytes)++p;
    }
    output=result;return true;
}
}
