#include "../../platform/linux/native/raster_shared_edge.h"
#include <cstdio>
#include <cstdlib>
using namespace rr::platform::raster_shared_edge;
static void require(bool v){if(!v)std::abort();}
static Point point(double x,double y,double radius=0){return {{x-radius,x+radius},{y-radius,y+radius}};}
int main(){
    const auto a=point(0,0),b=point(100,0);
    require(opposite(a,b,point(50,10),point(50,-10)));
    require(!opposite(a,b,point(50,10),point(20,20)));
    require(!opposite(a,b,point(50,0),point(50,-10)));
    require(!opposite(a,a,point(50,10),point(50,-10)));
    require(!opposite(point(0,0,1),point(100,0,1),point(50,1,1),point(50,-1,1)));
    require(opposite(point(0,0,1),point(100,0,1),point(50,50,1),point(50,-50,1)));
    require(!opposite(a,b,point(50,10),point(0,std::numeric_limits<double>::quiet_NaN())));
    unsigned cases=0;
    for(int y=-100;y<=100;++y)for(int z=-100;z<=100;++z){
        const bool expected=(y<0&&z>0)||(y>0&&z<0);
        require(opposite(a,b,point(20,y),point(80,z))==expected);
        require(opposite(b,a,point(80,z),point(20,y))==expected);++cases;
    }
    // Independent endpoint oracle for accepted uncertainty boxes. The signed
    // area is multi-affine in vertex coordinates, so extrema occur at corners.
    unsigned seed=173,accepted=0;
    const auto random=[&](){seed=seed*1664525u+1013904223u;return double(seed%2000u)/10.;};
    for(unsigned trial=0;trial<1000;++trial){
        Point p[4];for(auto& v:p)v=point(random(),random(),.125);
        if(!opposite(p[0],p[1],p[2],p[3]))continue;
        ++accepted;
        for(unsigned mask=0;mask<256;++mask){
            double v[4][2];
            for(unsigned i=0;i<4;++i){v[i][0]=(mask&(1u<<(2*i)))?p[i].x.high:p[i].x.low;
                v[i][1]=(mask&(1u<<(2*i+1)))?p[i].y.high:p[i].y.low;}
            const auto area=[&](unsigned i){return (v[1][0]-v[0][0])*(v[i][1]-v[0][1])-
                (v[1][1]-v[0][1])*(v[i][0]-v[0][0]);};
            require(area(2)*area(3)<0);
        }
    }
    require(accepted>100);
    std::printf("raster_shared_edge_check passed cases=%u accepted_uncertain=%u endpoint_cases=%u\n",cases,accepted,accepted*256);
}
