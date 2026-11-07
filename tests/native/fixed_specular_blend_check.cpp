// Fixed-function map-2 blend versus the byte-exact destination-feedback
// kernel, over every source byte, destination alpha byte and destination byte.
// Exact: rgb = clamp(floor(Cs * Ad / 128) + Cd, 0, 255).
// Fixed: the base pass stores Ad as UNORM8 round(min(Ad / 128, 1) * 255); the
// blender computes Cs/255 * storedAlpha + Cd/255 and rounds to UNORM8.
// Paired source alpha is bounded by vertex alpha 64 (texture alpha <= 255
// gives Ad <= 127), so the admitted range is Ad in [0,127]; [128,255] is
// reported separately because those values saturate in the stored scale.
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
int main(){
    int worstAdmitted=0,worstSaturated=0;long long differing=0,total=0;
    for(int ad=0;ad<=255;++ad){
        const double stored=std::round(std::min(ad/128.0,1.0)*255.0)/255.0;
        for(int cs=0;cs<=255;++cs)for(int cd=0;cd<=255;++cd){
            const int exact=std::clamp(int(std::floor(cs*ad/128.0))+cd,0,255);
            const int fixed=int(std::round(std::clamp(cs/255.0*stored+cd/255.0,0.0,1.0)*255.0));
            const int error=std::abs(exact-fixed);
            if(ad<=127){worstAdmitted=std::max(worstAdmitted,error);++total;if(error)++differing;}
            else worstSaturated=std::max(worstSaturated,error);
        }
    }
    std::printf("admitted Ad 0..127: max difference %d of 255, %.2f%% of combinations differ\n",
        worstAdmitted,100.0*double(differing)/double(total));
    std::printf("outside admitted range Ad 128..255: max difference %d of 255\n",worstSaturated);
    if(worstAdmitted>2){std::fprintf(stderr,"FAILED: admitted difference above two steps\n");return 1;}
    std::printf("fixed_specular_blend_check passed\n");
}
