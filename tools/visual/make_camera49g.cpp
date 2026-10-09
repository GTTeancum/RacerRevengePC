// Diagnostic pose -> verified49F core matrices. Not a game camera scheduler.
#include "../../src/game/camera_frame49f.h"
#include "../../third_party/native-json/json.hpp"
#include <array>
#include <charconv>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
using Json=nlohmann::json;
using namespace rr::assets;
using namespace rr::game;
static std::uint32_t bits(float v){std::uint32_t b;std::memcpy(&b,&v,4);return b;}
static Json vectorBits(const TVCameraVector49E& v){return Json::array({bits(v.x),bits(v.y),bits(v.z),bits(v.w)});}
static Json vectorValues(const TVCameraVector49E& v){return Json::array({v.x,v.y,v.z,v.w});}
static float number(const char* text){float value=0;const auto n=std::strlen(text);const auto r=std::from_chars(text,text+n,value);if(r.ec!=std::errc{}||r.ptr!=text+n||!std::isfinite(value))throw std::runtime_error("invalid finite diagnostic scalar");return value;}
int main(int argc,char** argv){try{
    if(argc!=8)throw std::runtime_error("usage: make_camera49g AUTHORED_POS CAMERA_INDEX PLAYER_X PLAYER_Y PLAYER_Z TARGET_Y_OFFSET OUTPUT_JSON");
    if(std::fesetround(FE_TONEAREST))throw std::runtime_error("nearest rounding unavailable");
    unsigned index=0;const auto idx=std::from_chars(argv[2],argv[2]+std::strlen(argv[2]),index);if(idx.ec!=std::errc{}||idx.ptr!=argv[2]+std::strlen(argv[2]))throw std::runtime_error("bad camera index");
    std::ifstream in(argv[1],std::ios::binary);if(!in)throw std::runtime_error("missing authenticated camera fixture");
    const std::string text((std::istreambuf_iterator<char>(in)),{});
    TVCameraRequirements49E needed;TVCameraDiagnostic49E diagnostic;
    if(!inspectTVCamera49E({text.data(),text.size()},needed,diagnostic)||needed.cameras!=19||needed.links!=20||index>=needed.cameras)throw std::runtime_error("expected complete authenticated TA camera domain");
    std::array<TVCameraRecord49E,19> records;std::array<std::uint32_t,20> links{};TVCameraView49E configuration;
    if(!loadTVCamera49E({text.data(),text.size()},{{records.data(),records.size()},{links.data(),links.size()}},configuration,diagnostic))throw std::runtime_error("camera ingestion failed");
    const TVCameraVector49E playerPosition{number(argv[3]),number(argv[4]),number(argv[5]),1};
    TVCameraSelectorState49E selected;TVCameraRandom49E random;TVCameraSelection49E selection;
    // Explicit identity-parent diagnostic fixture: local selection coordinates
    // and resolved view coordinates are supplied equal, not assumed generally.
    const auto localSelectorPosition=playerPosition;
    if(!selectTVCamera49E(configuration,selected,localSelectorPosition,0,random,selection)||selected.current40!=static_cast<std::int32_t>(index))throw std::runtime_error("requested camera not uniquely selected");
    TVCameraUpdateState49F update;
    TVCameraUpdateInput49F input;
    input.resolvedPlayerPosition=playerPosition;
    input.bodyVelocity={0,0,0,1};input.targetYOffset=number(argv[6]);input.elapsed=0.016f;
    TVCameraSceneSample49F stationarySample;stationarySample.localPosition=input.resolvedPlayerPosition;stationarySample.velocity={0,0,0,1};
    input.sceneSamples={&stationarySample,1};
    CameraSceneState49F scene;scene.aspect1F8=0.75f;
    // A4:3 full-frame diagnostic viewport. This supplied field is0.75; no
    // desktop aspect is silently inserted into the source matrix formulas.
    TVCameraUpdateOutput49F numeric;CameraViewOutput49F view;
    const auto randomAtEntry=random.raw;
    if(!updateTVCameraView49F(configuration,selected,update,input,random,scene,numeric,view,{0,0}))throw std::runtime_error("verified49F view domain rejected diagnostic pose");
    if(random.raw!=randomAtEntry)throw std::runtime_error("stationary diagnostic unexpectedly drew shake randomness");
    Json j; j["schema"]="neutral49g.camera.v1";j["viewport"]={{"width",640},{"height",480}};
    j["conventions"]={{"vectors","row_vector"},{"storage","row_major"},{"clip_depth","zero_to_one"}};
    j["view_f32_bits"]=Json::array();j["projection_f32_bits"]=Json::array();j["combined_f32_bits"]=Json::array();
    for(auto x:view.view)j["view_f32_bits"].push_back(bits(x));
    for(auto x:view.projection)j["projection_f32_bits"].push_back(bits(x));
    for(auto x:view.combined)j["combined_f32_bits"].push_back(bits(x));
    j["authored_camera_one_based"]=index+1;j["authored_camera_position"]=vectorValues(records[index].position);
    j["position_f32_bits"]=vectorBits(numeric.position);j["target_f32_bits"]=vectorBits(numeric.target);
    j["position"]=vectorValues(numeric.position);j["target"]=vectorValues(numeric.target);j["fov_degrees"]=numeric.fov;j["fov_f32_bits"]=bits(numeric.fov);
    j["resolved_player_position"]=vectorValues(input.resolvedPlayerPosition);j["selector_local_position"]=vectorValues(localSelectorPosition);j["target_y_offset"]=input.targetYOffset;
    j["projection_inputs"]={{"aspect1F8",scene.aspect1F8},{"near",scene.nearPlane},{"far",scene.farPlane},{"roll_radians",scene.rollRadians}};
    j["context"]={{"rootFlags228",0},{"sceneFlags29",0},{"elapsed",input.elapsed},{"mode",numeric.mode},{"scene_samples",1},{"sample_speed",0},{"rng_at_view_entry",randomAtEntry},{"rng_after_view",random.raw}};
    j["scope"]="Source-authored camera, explicit supplied stationary Pod diagnostic pose. Local selection and resolved view inputs are explicitly equal for this identity-parent fixture; their general contracts remain distinct. TV mode is requested; game camera-type scheduling/intervening draws are not simulated. No race grid, physics, visibility, lighting or rendered parity claim.";
    std::ofstream out(argv[7],std::ios::binary);if(!out)throw std::runtime_error("cannot open camera output");out<<j.dump(2)<<'\n';if(!out)throw std::runtime_error("camera output write failed");
    std::cout<<j.dump()<<'\n';return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
