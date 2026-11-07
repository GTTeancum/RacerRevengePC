#pragma once
#include <algorithm>
#include <cmath>
#include <limits>

namespace rr::platform::raster_shared_edge {
struct Interval {double low=0,high=0;};
struct Point {Interval x,y;};
inline Interval subtract(Interval a,Interval b) noexcept {return {a.low-b.high,a.high-b.low};}
inline Interval multiply(Interval a,Interval b) noexcept {
    const double p[]={a.low*b.low,a.low*b.high,a.high*b.low,a.high*b.high};
    return {*std::min_element(p,p+4),*std::max_element(p,p+4)};
}
inline bool valid(Point p) noexcept {
    return std::isfinite(p.x.low)&&std::isfinite(p.x.high)&&p.x.low<=p.x.high&&
           std::isfinite(p.y.low)&&std::isfinite(p.y.high)&&p.y.low<=p.y.high;
}
// Caller proves identical indexed edge endpoints, fully unclipped triangles,
// and single-sample filled rasterization. Boxes include transform uncertainty
// and pixel rounding. No geometric epsilon substitutes for opposite sides.
inline bool opposite(Point a,Point b,Point c,Point d) noexcept {
    if(!valid(a)||!valid(b)||!valid(c)||!valid(d))return false;
    const auto x=subtract(b.x,a.x),y=subtract(b.y,a.y);
    const auto side=[&](Point p) {
        const auto u=multiply(x,subtract(p.y,a.y)),v=multiply(y,subtract(p.x,a.x));
        auto r=subtract(u,v);
        const double scale=std::max({std::abs(u.low),std::abs(u.high),std::abs(v.low),std::abs(v.high),1.});
        const double error=64*std::numeric_limits<double>::epsilon()*scale;
        r.low-=error;r.high+=error;return r;
    };
    const auto p=side(c),q=side(d);
    if(!std::isfinite(p.low)||!std::isfinite(p.high)||!std::isfinite(q.low)||!std::isfinite(q.high))return false;
    return (p.low>0&&q.high<0)||(p.high<0&&q.low>0);
}
// Robust side of p relative to the directed line a->b: +1/-1 only when every
// placement of all three boxes agrees, else 0. Same bound as opposite().
inline int side(Point a,Point b,Point p) noexcept {
    const auto x=subtract(b.x,a.x),y=subtract(b.y,a.y);
    const auto u=multiply(x,subtract(p.y,a.y)),v=multiply(y,subtract(p.x,a.x));
    auto r=subtract(u,v);
    const double scale=std::max({std::abs(u.low),std::abs(u.high),std::abs(v.low),std::abs(v.high),1.});
    const double error=64*std::numeric_limits<double>::epsilon()*scale;
    r.low-=error;r.high+=error;
    if(!std::isfinite(r.low)||!std::isfinite(r.high))return 0;
    return r.low>0?1:r.high<0?-1:0;
}
// True when no pixel center (k+0.5) can equal any placement of p: one axis
// interval already excludes every half-integer, including rounding slack.
inline bool excludesPixelCenter(Point p) noexcept {
    const auto clear=[](Interval i){return std::floor(i.high-.5)<std::ceil(i.low-.5);};
    return clear(p.x)||clear(p.y);
}
// Two filled, fully unclipped, single-sample triangles A and B that share AT
// MOST ONE identical indexed vertex (same[i][j]: A vertex i is B vertex j;
// identical vertices have one exact transformed position). Accept only a
// separating edge line of A or B: its own third vertex strictly on one side,
// every other-triangle vertex strictly on the other, except a vertex identical
// to that edge's endpoint, which lies exactly on the line. The closed snapped
// triangles then meet at most at that one shared vertex, and a shared vertex
// must exclude every pixel center, so no sample is covered by both. Two shared
// vertices are the shared-edge case: use opposite() and its edge ownership.
inline bool separated(const Point (&a)[3],const Point (&b)[3],const bool (&same)[3][3]) noexcept {
    for(const auto& p:a)if(!valid(p))return false;
    for(const auto& p:b)if(!valid(p))return false;
    unsigned shared=0,sharedA=3;
    for(unsigned i=0;i<3;++i){
        unsigned matches=0;
        for(unsigned j=0;j<3;++j)if(same[i][j]){++matches;++shared;sharedA=i;}
        if(matches>1)return false;
    }
    for(unsigned j=0;j<3;++j){unsigned matches=0;for(unsigned i=0;i<3;++i)matches+=same[i][j];if(matches>1)return false;}
    if(shared>1)return false;
    if(shared==1&&!excludesPixelCenter(a[sharedA]))return false;
    // owner: 0 tests A's edges against B, 1 tests B's edges against A.
    for(unsigned owner=0;owner<2;++owner){
        const auto& t=owner?b:a;const auto& u=owner?a:b;
        const auto identical=[&](unsigned edgeVertex,unsigned otherVertex){
            return owner?same[otherVertex][edgeVertex]:same[edgeVertex][otherVertex];
        };
        for(unsigned i=0;i<3;++i){
            const unsigned j=(i+1)%3,k=(i+2)%3;
            const int own=side(t[i],t[j],t[k]);
            if(!own)continue;
            bool accepted=true;
            for(unsigned m=0;m<3&&accepted;++m){
                if(identical(i,m)||identical(j,m))continue;
                accepted=side(t[i],t[j],u[m])==-own;
            }
            if(accepted)return true;
        }
    }
    return false;
}
}
