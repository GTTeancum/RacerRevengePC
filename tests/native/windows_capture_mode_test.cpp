#include "windows_framebuffer_capture50ld.h"
#include <cstdio>

int main(int argc,char** argv) {
    if(argc!=2||!std::filesystem::path(argv[1]).is_absolute()||
       !std::filesystem::is_directory(argv[1]))return 2;
    const auto set=[](const char* name,const char* value){
        if(_putenv_s(name,value))throw std::runtime_error("test environment failed");
    };
    set("RR_WINDOW_CAPTURE_DIR",argv[1]);set("RR_WINDOW_CAPTURE_AUTO","1");
    set("RR_UNATTENDED_CAPTURE","1");set("RR_TRACK_FPS_BASELINE","1");
    set("RR_ORIGINAL_TEST_PROFILE","");set("RR_WINDOW_VIDEO_HOLD_SECONDS","");
    set("RR_UNATTENDED_SECONDS","30");
    unsigned checked=0;
    const auto check=[&](const char* mode,const char* route,bool expected){
        set("RR_ORIGINAL_AI_ALL_RACERS",mode);set("RR_UNATTENDED_UI_ROUTE",route);
        bool accepted=false;
        try {
            windows_capture50LD::Run run;
            accepted=true;
            // Configuration alone must never manufacture capture completion.
            if(run.evidenceComplete())throw std::logic_error("empty run reported complete");
        }catch(const std::logic_error&){throw;}
        catch(const std::runtime_error&){}
        if(accepted!=expected)throw std::runtime_error(std::string("unexpected mode admission: ")+mode+" "+route);
        ++checked;
    };
    for(const char* route:{"main-rider-course-ta","main-dud04-course-ga","main-ben02-course-ga",
                          "main-alder10-course-ma","main-ody11-course-ma"})check("0",route,true);
    check("0","main-rider-course-sa",false);check("0","main-occo20-course-sa",false);
    check("0","",false);check("invalid","main-dud04-course-ga",false);
    check("1","main-rider-course-sa",true);check("1","",true);
    std::printf("capture mode admission: %u cases passed\n",checked);
}
