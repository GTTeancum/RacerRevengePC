// raster_shared_edge::separated(): no accepted pair may share a sample.
// Oracle: for every accepted random/constructed pair, sample many exact
// placements inside the uncertainty boxes (all box corners for the four free
// coordinates of a chosen pair plus random 1/256-grid placements; a shared
// vertex keeps ONE position in both triangles) and test every pixel center
// against both CLOSED triangles. Closed coverage is a superset of any GL tie
// rule, so a center inside or on both boundaries is a failure.
// Also replays the drawCoveragePairGroup run rule on 14-triangle strips to
// count barriers with the shared-edge proof alone versus the extended proof.
#include "../../platform/linux/native/raster_shared_edge.h"
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace rr::platform::raster_shared_edge;
namespace {
std::size_t checks=0;
void need(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAILED: %s\n",why);std::exit(1);}}
Point box(double x,double y,double r){return {{x-r,x+r},{y-r,y+r}};}
struct V {double x,y;};
double orient(V a,V b,V p){return (b.x-a.x)*(p.y-a.y)-(b.y-a.y)*(p.x-a.x);}
bool closedInside(const std::array<V,3>& t,V p){
    const double s0=orient(t[0],t[1],p),s1=orient(t[1],t[2],p),s2=orient(t[2],t[0],p);
    return (s0>=0&&s1>=0&&s2>=0)||(s0<=0&&s1<=0&&s2<=0);
}
double area(const std::array<V,3>& t){return orient(t[0],t[1],t[2]);}
unsigned seed=20261107u;
double unit(){seed=seed*1664525u+1013904223u;return double(seed>>8)/double(1u<<24);}
// Exact 1/256 grid placement inside [low,high] (boxes are wider than 1/128).
double place(Interval i,double t){
    const double lo=std::ceil(i.low*256),hi=std::floor(i.high*256);
    if(hi<lo)return std::nan("");
    return std::round(lo+(hi-lo)*t)/256;
}
// Returns false if some placement puts one pixel center in both closed triangles.
bool noSharedSample(const Point (&a)[3],const Point (&b)[3],const bool (&same)[3][3],unsigned placements){
    for(unsigned trial=0;trial<placements;++trial){
        std::array<V,3> ta{},tb{};
        for(unsigned i=0;i<3;++i){
            // Extremes for the first 64 trials, random grid points afterwards.
            const double tx=trial<64?double((trial>>i)&1):unit(),ty=trial<64?double((trial>>(i+3))&1):unit();
            ta[i]={place(a[i].x,tx),place(a[i].y,ty)};
        }
        for(unsigned j=0;j<3;++j){
            bool shared=false;
            for(unsigned i=0;i<3;++i)if(same[i][j]){tb[j]=ta[i];shared=true;}
            if(!shared){const double tx=trial<64?double((trial>>(j+1))&1):unit(),ty=trial<64?double((trial>>(j+2))&1):unit();
                tb[j]={place(b[j].x,tx),place(b[j].y,ty)};}
        }
        for(const auto& v:ta)if(std::isnan(v.x)||std::isnan(v.y))return true;
        for(const auto& v:tb)if(std::isnan(v.x)||std::isnan(v.y))return true;
        if(area(ta)==0||area(tb)==0)continue; // degenerate placement rasterizes nothing
        double x0=1e9,y0=1e9,x1=-1e9,y1=-1e9;
        for(const auto& v:ta){x0=std::min(x0,v.x);x1=std::max(x1,v.x);y0=std::min(y0,v.y);y1=std::max(y1,v.y);}
        for(int py=int(std::floor(y0))-1;py<=int(std::ceil(y1))+1;++py)
            for(int px=int(std::floor(x0))-1;px<=int(std::ceil(x1))+1;++px){
                const V c{px+.5,py+.5};
                if(closedInside(ta,c)&&closedInside(tb,c))return false;
            }
    }
    return true;
}
// The game's rectangle: floor/ceil of the box extents with two pixels margin.
std::array<int,4> rect(const Point (&t)[3]){
    double x0=1e9,y0=1e9,x1=-1e9,y1=-1e9;
    for(const auto& p:t){x0=std::min(x0,p.x.low);x1=std::max(x1,p.x.high);y0=std::min(y0,p.y.low);y1=std::max(y1,p.y.high);}
    return {int(std::floor(x0))-2,int(std::floor(y0))-2,int(std::ceil(x1))+2,int(std::ceil(y1))+2};
}
bool overlap(const std::array<int,4>& a,const std::array<int,4>& b){return a[0]<b[2]&&b[0]<a[2]&&a[1]<b[3]&&b[1]<a[3];}
}

int main(){
    const bool none[3][3]{};
    // Constructed cases.
    {   // Fan around a shared vertex that is not a pixel center.
        const Point a[3]={box(10.3,10.3,.13),box(20,10.3,.13),box(20,20,.13)};
        const Point b[3]={box(10.3,10.3,.13),box(10.3,20,.13),box(0,20,.13)};
        const bool same[3][3]={{true,false,false},{false,false,false},{false,false,false}};
        need(separated(a,b,same),"separated fan at off-center vertex accepted");
        need(noSharedSample(a,b,same,400),"off-center fan has no shared sample");
    }
    {   // Same fan with the shared vertex able to sit on a pixel center: reject.
        const Point a[3]={box(10.5,10.5,.13),box(20,10.5,.13),box(20,20,.13)};
        const Point b[3]={box(10.5,10.5,.13),box(10.5,20,.13),box(0,20,.13)};
        const bool same[3][3]={{true,false,false},{false,false,false},{false,false,false}};
        need(!separated(a,b,same),"shared vertex on a pixel center rejected");
    }
    {   // Overlapping (folded) triangles sharing a vertex: reject.
        const Point a[3]={box(10.3,10.3,.13),box(30,10.3,.13),box(30,30,.13)};
        const Point b[3]={box(10.3,10.3,.13),box(30,20,.13),box(20,30,.13)};
        const bool same[3][3]={{true,false,false},{false,false,false},{false,false,false}};
        need(!separated(a,b,same),"overlapping shared-vertex pair rejected");
    }
    {   // Disjoint triangles whose conservative rectangles overlap.
        const Point a[3]={box(0,0,.13),box(6,0,.13),box(0,6,.13)};
        const Point b[3]={box(7,1,.13),box(7,7,.13),box(1,7,.13)};
        need(overlap(rect(a),rect(b)),"rectangles overlap");
        need(separated(a,b,none),"nearby disjoint pair accepted");
        need(noSharedSample(a,b,none,400),"nearby disjoint pair has no shared sample");
    }
    {   // Touching without a shared index (same coordinates, distinct vertices): reject.
        const Point a[3]={box(0,0,.13),box(6,0,.13),box(3.2,3.2,.13)};
        const Point b[3]={box(3.2,3.2,.13),box(6,6,.13),box(0,6,.13)};
        need(!separated(a,b,none),"touching distinct vertices rejected");
    }
    {   // Two shared vertices belong to opposite(), never separated().
        const Point a[3]={box(0,0,.13),box(6,0,.13),box(0,6,.13)};
        const Point b[3]={box(6,0,.13),box(0,6,.13),box(6,6,.13)};
        const bool same[3][3]={{false,false,false},{true,false,false},{false,true,false}};
        need(!separated(a,b,same),"shared edge deferred to opposite()");
    }
    {   // Invalid boxes rejected.
        Point a[3]={box(0,0,.13),box(6,0,.13),box(0,6,.13)};
        const Point b[3]={box(7,1,.13),box(7,7,.13),box(1,7,.13)};
        a[1].x.low=std::nan("");need(!separated(a,b,none),"non-finite box rejected");
    }
    // Random pairs near each other, with and without one shared vertex.
    unsigned accepted=0,acceptedShared=0,tested=0;
    for(unsigned trial=0;trial<20000;++trial){
        const double r=.0625+unit()*.25,scale=1+unit()*12;
        Point a[3],b[3];bool same[3][3]{};
        const double cx=50+unit()*4,cy=50+unit()*4;
        for(auto& p:a)p=box(cx+(unit()-.5)*scale,cy+(unit()-.5)*scale,r);
        for(auto& p:b)p=box(cx+(unit()-.5)*scale*1.6,cy+(unit()-.5)*scale*1.6,r);
        const bool share=trial%2;
        if(share){const unsigned i=trial/2%3,j=trial/6%3;b[j]=a[i];same[i][j]=true;}
        ++tested;
        if(!separated(a,b,same))continue;
        ++accepted;if(share)++acceptedShared;
        need(noSharedSample(a,b,same,160),"accepted random pair shares no sample");
    }
    need(accepted>1000&&acceptedShared>300,"random cases exercise acceptance");
    // Benefit on the drawCoveragePairGroup rule: contiguous runs of at most 14,
    // a new member must be rectangle-disjoint or proven against EVERY member.
    unsigned oldBarriers=0,newBarriers=0,strips=0;
    for(unsigned trial=0;trial<2000;++trial){
        const double step=1.5+unit()*10,height=1.5+unit()*10,r=.02+unit()*.05+.125;
        const double x0=100+unit()*3,y0=100+unit()*3,shear=(unit()-.5)*step;
        std::vector<Point> v;for(unsigned k=0;k<16;++k)
            v.push_back(box(x0+(k/2)*step+(k%2)*shear+(unit()-.5)*.2*step,y0+(k%2)*height+(unit()-.5)*.2*height,r));
        // Triangle list from a 16-vertex strip, alternate winding as GL does.
        std::vector<std::array<unsigned,3>> tri;
        for(unsigned k=0;k+2<16;++k)tri.push_back(k%2?std::array<unsigned,3>{k+1,k,k+2}:std::array<unsigned,3>{k,k+1,k+2});
        const auto pair=[&](unsigned i,unsigned j,bool extended){
            const auto& a=tri[i];const auto& b=tri[j];
            unsigned matched=0;bool same[3][3]{};
            for(unsigned p=0;p<3;++p)for(unsigned q=0;q<3;++q)if(a[p]==b[q]){same[p][q]=true;++matched;}
            if(matched==2){
                unsigned s[2],n=0,oa=0,ob=0;
                for(unsigned p=0;p<3;++p){bool f=false;for(unsigned q=0;q<3;++q)f|=same[p][q];if(f)s[n++]=a[p];else oa=a[p];}
                for(unsigned q=0;q<3;++q)if(b[q]!=s[0]&&b[q]!=s[1])ob=b[q];
                return opposite(v[s[0]],v[s[1]],v[oa],v[ob]);
            }
            if(!extended)return false;
            const Point pa[3]={v[a[0]],v[a[1]],v[a[2]]},pb[3]={v[b[0]],v[b[1]],v[b[2]]};
            const bool ok=separated(pa,pb,same);
            if(ok)need(noSharedSample(pa,pb,same,96),"strip pair accepted by the run rule shares no sample");
            return ok;
        };
        for(bool extended:{false,true}){
            unsigned barriers=0;
            for(unsigned first=0;first<tri.size();){
                unsigned run=1;
                while(run<14&&first+run<tri.size()){
                    const unsigned next=first+run;bool ok=true;
                    const Point pn[3]={v[tri[next][0]],v[tri[next][1]],v[tri[next][2]]};
                    for(unsigned m=first;m<next&&ok;++m){
                        const Point pm[3]={v[tri[m][0]],v[tri[m][1]],v[tri[m][2]]};
                        if(overlap(rect(pm),rect(pn))&&!pair(m,next,extended))ok=false;
                    }
                    if(!ok)break;++run;
                }
                ++barriers;first+=run;
            }
            (extended?newBarriers:oldBarriers)+=barriers;
        }
        ++strips;
    }
    std::printf("constructed and random: tested=%u accepted=%u accepted_shared_vertex=%u\n",tested,accepted,acceptedShared);
    std::printf("14-triangle strips=%u barriers shared_edge_only=%u extended=%u (%.1f%% fewer)\n",
        strips,oldBarriers,newBarriers,100.0*(1.0-double(newBarriers)/double(oldBarriers)));
    std::printf("raster_separation_pairs_check passed checks=%zu\n",checks);
}
