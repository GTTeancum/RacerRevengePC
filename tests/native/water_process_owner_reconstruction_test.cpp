#include "../../src/game/original_application_entities50la.h"
#include <cstdio>
#include <thread>

// Standalone test executable: exercise the existing private World-owner seam,
// without exporting a mutable process-state API from production Application.
namespace ra_cold_aggregate50LA {
class Owner {
public:
    static int test() {
        rr::game::OriginalApplicationEntities50LA app;
        int failures=0;
        const auto check=[&](bool ok,const char* name){
            if(!ok){std::fprintf(stderr,"FAIL: %s\n",name);++failures;}
        };
        auto& state=app.sourceWaterProcessScalars_;
        std::uint32_t word=123;std::uint64_t version=456;
        check(app.waterScalarReadNative(24,word,version)&&word==32&&version==1,"startup integer alpha");
        check(!state.words[0]&&!state.words[1]&&!state.words[30],"unpublished lanes stay unknown");
        std::array<std::uint32_t,4> words{1,2,3,4};
        std::array<std::uint64_t,4> versions{5,6,7,8};
        check(!app.waterVectorReadNative(0,words,versions)&&words[0]==1&&words[3]==4&&versions[0]==5,
              "partial vector refuses atomically");
        state.words[0]=0x40000000u;state.publications[0]=1;
        check(app.waterVectorReadNative(0,words,versions)&&
              words==std::array<std::uint32_t,4>{0x40000000u,0,0x448ae000u,0xc0000000u},"current vector mapping");
        state.words[0]=0x40400000u;++state.publications[0];
        check(app.waterVectorReadNative(0,words,versions)&&words[0]==0x40400000u&&versions[0]==2,
              "later publication is current");
        state.words[4]=0;state.publications[4]=std::numeric_limits<std::uint64_t>::max();
        check(app.waterScalarReadNative(4,word,version)&&word==0&&!app.waterLoadScalarAdmittedNative(4)&&
              !app.waterAdaptedLoadScalarAdmittedNative(4),"saturated counter readable but unwritable");
        state.words[31]=32;state.publications[31]=1;
        check(!app.waterAdaptedScalarReadNative(31,word,version),"Wake read requires adapted provenance");
        state.nativeAdapted[31]=true;
        check(app.waterAdaptedScalarReadNative(31,word,version)&&word==32&&
              !app.waterScalarReadNative(31,word,version),"adapted Wake cannot become source-exact");
        state.nativeAdapted[4]=true;
        check(app.waterAdaptedScalarReadNative(4,word,version)&&
              !app.waterScalarReadNative(4,word,version),"adapted range consumer is explicit");
        bool wrongThreadAdmitted=true;
        std::thread other([&]{std::uint32_t w=0;std::uint64_t v=0;
            wrongThreadAdmitted=app.waterScalarReadNative(24,w,v)||app.waterLoadScalarAdmittedNative(2);
        });other.join();
        check(!wrongThreadAdmitted,"wrong thread rejects reads and writes");
        check(!app.waterLoadScalarAdmittedNative(35)&&!app.waterAdaptedLoadScalarAdmittedNative(35)&&
              !app.waterVectorReadNative(2,words,versions),"bounds reject");
        app.invalidateWaterProcessScalarsNative();
        word=123;version=456;
        check(!app.waterScalarReadNative(24,word,version)&&word==123&&version==456&&
              !app.waterConstructorScalarsAdmittedNative()&&!app.waterAdaptedLoadScalarAdmittedNative(30),
              "poison refuses without restoring defaults");
        check(app.releaseApplication1411AC(),"native Application release");
        check(!app.waterProcessScalarsCurrentNative(),"released owner unavailable");
        if(!failures)std::puts("Water process-owner reconstruction checks passed");
        return failures?1:0;
    }
};
}
int main(){return ra_cold_aggregate50LA::Owner::test();}
