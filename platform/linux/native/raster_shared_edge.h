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
}
