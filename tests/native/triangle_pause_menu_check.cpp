// Controller Triangle/Y in a race: preset0 action6A4 = descriptor0 = Triangle
// is the original camera view change, so the native host Pause menu must not
// take it while no menu is open. Start still opens Pause; inside an open menu
// Triangle keeps its Back position (resumes from Pause).
#include "../../platform/linux/native/host_session_menu50if.h"
#include <cstdio>
#include <stdexcept>
#include <string>

namespace {
unsigned checks=0;
void need(bool value,const char* what){++checks;if(!value)throw std::runtime_error(std::string("check failed: ")+what);}
using Menu=rr::platform::HostSessionMenu50IF;
rr::platform::GameHostInput50ER frame(unsigned pressedButton){
    rr::platform::GameHostInput50ER in{};
    in.keys.focused=true;in.keys.gameControllersEnabled=true;
    auto& pad=in.keys.controllers[0];pad.connected=true;pad.instanceId=7;
    if(pressedButton<pad.buttons.size()){pad.buttons[pressedButton]=1;pad.pressed[pressedButton]=1;}
    return in;
}
void step(Menu& menu,unsigned pressedButton){
    const auto in=frame(pressedButton);
    menu.observeController(in.keys,true);
    (void)menu.poll(in,true);
}
}

int main(){
    try{
        constexpr unsigned triangleY=3,start=6,none=99;
        // In-race menu (not at Main), human race on TA.
        Menu menu(false,true,19);
        need(!menu.open(),"race starts with no menu open");
        step(menu,none);                        // Arm the controller on a neutral frame.
        step(menu,triangleY);step(menu,none);   // Triangle in gameplay.
        need(!menu.open(),"Triangle in a race must not open Pause");
        step(menu,start);step(menu,none);       // Start opens Pause.
        need(menu.open()&&menu.page()==Menu::Page::Pause,"Start opens Pause");
        step(menu,triangleY);step(menu,none);   // Triangle = Back inside the menu.
        need(!menu.open(),"Triangle inside Pause resumes (Back)");
        std::printf("triangle pause menu checks=%u passed\n",checks);
        return 0;
    }catch(const std::exception& e){std::fprintf(stderr,"%s\n",e.what());return 1;}
}
