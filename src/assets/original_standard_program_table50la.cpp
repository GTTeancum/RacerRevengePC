#include "original_standard_program_table50la.h"

namespace rr::assets {
namespace {
struct SourceRow {const char* name;std::uint32_t registrationPC,nameVA;};
// Original1505C0 constructor append order. These are semantic identifiers,
// not original code/data word arrays and not a native executable dispatch.
constexpr SourceRow rows[]={
    {"KickGSCode",0x1505fcu,0x3a53e0u}, // 0
    {"EcoSystemCode2D",0x15061cu,0x3a53f0u}, // 1
    {"TerrainRendererClipCode",0x15063cu,0x3a5400u}, // 2
    {"ParticleCode",0x15065cu,0x3a5418u}, // 3
    {"ProjectedShadow",0x15067cu,0x3a5430u}, // 4
    {"StripPreambleCode",0x15069cu,0x3a5440u}, // 5
    {"ScissorCoreCode",0x1506bcu,0x3a5460u}, // 6
    {"ScissorCoreOrgCode",0x1506dcu,0x3a5470u}, // 7
    {"StripLitCode",0x1506fcu,0x3a5488u}, // 8
    {"StripOptCode",0x15071cu,0x3a5498u}, // 9
    {"StripOptFogCode",0x15073cu,0x3a54b0u}, // 10
    {"StripFogCullCode",0x15075cu,0x3a54c0u}, // 11
    {"StripOptTexXformCode",0x15077cu,0x3a54e0u}, // 12
    {"StripUnlitCode",0x15079cu,0x3a54f8u}, // 13
    {"StripPrelitCode",0x1507bcu,0x3a5510u}, // 14
    {"EnvmapCode",0x1507dcu,0x3a5520u}, // 15
    {"Envmap2Code",0x1507fcu,0x3a5530u}, // 16
    {"SpecularMapCode",0x15081cu,0x3a5540u}, // 17
    {"ProjtexLitCode",0x15083cu,0x3a5550u}, // 18
    {"TerrainScisPreambleCode",0x15085cu,0x3a5560u}, // 19
    {"TerrainScissorCode",0x15087cu,0x3a5580u}, // 20
    {"TerrainShadowScisCode",0x15089cu,0x3a55a0u}, // 21
    {"TerrainRendererHalfResCode",0x1508bcu,0x3a55c0u}, // 22
    {"TerrainScisHalfResCode",0x1508dcu,0x3a55e0u}, // 23
    {"TerrainShadowHalfResCode",0x1508fcu,0x3a5600u}, // 24
    {"ScreenObjectRendererCode",0x15091cu,0x3a5620u}, // 25
    {"Diffuse",0x15093cu,0x3a5640u}, // 26
    {"DiffuseTexTrans",0x15095cu,0x3a5650u}, // 27
    {"Prelit",0x15097cu,0x3a5660u}, // 28
    {"SpecularMap",0x15099cu,0x3a5668u}, // 29
    {"SpecularMapDual",0x1509bcu,0x3a5680u}, // 30
    {"EnvironmentMap",0x1509dcu,0x3a5690u}, // 31
    {"ProjectedTexture",0x1509fcu,0x3a56a0u}, // 32
    {"ProjectedPrelit",0x150a1cu,0x3a56c0u}, // 33
    {"DiffuseAndPrelit",0x150a3cu,0x3a56d0u}, // 34
    {"StripOptFogCodeNoKick",0x150a5cu,0x3a56f0u}, // 35
    {"NopLoop",0x150a7cu,0x3a5708u}, // 36
    {"ParticlePhysics",0x150a9cu,0x3a5710u}, // 37
    {"DiffuseCompressed",0x150abcu,0x3a5720u}, // 38
    {"PrelitCompressed",0x150adcu,0x3a5740u}, // 39
    {"EnvironmentMapCompressed",0x150afcu,0x3a5760u}, // 40
    {"SpecularMapCompressed",0x150b1cu,0x3a5780u}, // 41
    {"SpecularMapDualCompressed",0x150b3cu,0x3a57a0u}, // 42
    {"DiffuseTexTransCompressed",0x150b5cu,0x3a57c0u}, // 43
    {"CubeSubdiv",0x150b7cu,0x3a57e0u}, // 44
    {"Water",0x150b9cu,0x3a57f0u}, // 45
    {"ShadowReceiver",0x150bbcu,0x3a57f8u}, // 46
    {"ScreenObject",0x150bdcu,0x3a5808u}, // 47
    {"TriStripPrologue",0x150bfcu,0x3a5820u}, // 48
    {"TriStripDiffuse",0x150c1cu,0x3a5840u}, // 49
    {"TriStripBackfaceCull",0x150c3cu,0x3a5850u}, // 50
    {"TriStripTexXform",0x150c5cu,0x3a5870u}, // 51
    {"TriStripProjTex",0x150c7cu,0x3a5890u}, // 52
    {"TriStripProjTexPreLit",0x150c9cu,0x3a58a0u}, // 53
    {"TriStripNormEnvMap",0x150cbcu,0x3a58c0u}, // 54
    {"TriStripReflEnvMap",0x150cdcu,0x3a58e0u}, // 55
    {"TriStripPreLit",0x150cfcu,0x3a58f8u}, // 56
    {"TriStripEmboss",0x150d1cu,0x3a5908u}, // 57
    {"TriStripSpecular",0x150d3cu,0x3a5920u}, // 58
    {"TriStripMultiAmbient",0x150d5cu,0x3a5940u}, // 59
    {"TriStripChameleon",0x150d7cu,0x3a5960u}, // 60
    {"TriStripDiffuseCompressed",0x150d9cu,0x3a5980u}, // 61
    {"TriStripBackfaceCullCompressed",0x150dbcu,0x3a59a0u}, // 62
    {"TriStripTexXformCompressed",0x150ddcu,0x3a59c0u}, // 63
    {"TriStripProjTexCompressed",0x150dfcu,0x3a59e0u}, // 64
    {"TriStripProjTexPreLitCompressed",0x150e1cu,0x3a5a00u}, // 65
    {"TriStripNormEnvMapCompressed",0x150e3cu,0x3a5a20u}, // 66
    {"TriStripReflEnvMapCompressed",0x150e5cu,0x3a5a40u}, // 67
    {"TriStripPreLitCompressed",0x150e7cu,0x3a5a60u}, // 68
    {"TriStripEmbossCompressed",0x150e9cu,0x3a5a80u}, // 69
    {"TriStripSpecularCompressed",0x150ebcu,0x3a5aa0u}, // 70
    {"TriStripClipper",0x150edcu,0x3a5ac0u}, // 71
    {"TriStripWireframe",0x150efcu,0x3a5ad0u}, // 72
    {"TriStripWireframeCompressed",0x150f1cu,0x3a5af0u}, // 73
    {"TerrainStrip",0x150f3cu,0x3a5b10u}, // 74
    {"TerrainStripHalfRes",0x150f5cu,0x3a5b20u}, // 75
};
constexpr std::size_t count=sizeof(rows)/sizeof(rows[0]);
struct SourceParameterRow {
    const char* name;std::int32_t value;std::uint32_t registrationPC,nameVA;
};
// Same fresh3C8C10 initialization, in original382B78 append order. Values
// are semantic parameter destinations, not program pointers or payload data.
constexpr SourceParameterRow parameterRows[]={
    {"TexGenMatrixRow1",28,0x3c8c74u,0x3a5b40u},
    {"TexGenMatrixRow2",29,0x3c8c90u,0x3a5b60u},
    {"TexGenMatrixRow3",30,0x3c8cacu,0x3a5b80u},
    {"TexGenMatrixRow4",31,0x3c8cc8u,0x3a5ba0u},
    {"AmbientColor",27,0x3c8ce4u,0x3a5bb8u}
};
constexpr std::size_t parameterCount=sizeof(parameterRows)/sizeof(parameterRows[0]);
char fold(char c) noexcept {return c>='A'&&c<='Z'?char(c+('a'-'A')):c;}
bool same(const std::string& a,const char* b) noexcept {
    std::size_t i=0;
    for(;i<a.size()&&b[i];++i)if(fold(a[i])!=fold(b[i]))return false;
    return i==a.size()&&!b[i];
}
bool fail(OriginalStandardProgramDiagnostic50LA& d,OriginalStandardProgramStatus50LA s,
    std::uint32_t pc) noexcept {d.status=s;d.sourcePC=pc;return false;}
}
bool OriginalStandardProgramTable50LA::prepareFresh(OriginalStandardProgramDiagnostic50LA& d) noexcept {
    d={};
    if(prepared_)return fail(d,OriginalStandardProgramStatus50LA::alreadyPrepared,0x3c8c10);
    prepared_=true;
    return true;
}
std::size_t OriginalStandardProgramTable50LA::rowCount() const noexcept {
    return prepared_?count:0;
}
bool OriginalStandardProgramTable50LA::row(std::int32_t index,
    OriginalStandardProgramRowView50LA& out) const noexcept {
    if(!prepared_||index<0||static_cast<std::size_t>(index)>=count)return false;
    const auto& source=rows[static_cast<std::size_t>(index)];
    out={static_cast<std::int16_t>(index),source.name,source.registrationPC,source.nameVA};
    return true;
}
std::size_t OriginalStandardProgramTable50LA::parameterRowCount382B78() const noexcept {
    return prepared_?parameterCount:0;
}
bool OriginalStandardProgramTable50LA::parameterRow382B78(std::int32_t index,
    OriginalStandardProgramParameterRowView50LA& out) const noexcept {
    if(!prepared_||index<0||static_cast<std::size_t>(index)>=parameterCount)return false;
    const auto& source=parameterRows[static_cast<std::size_t>(index)];
    out={static_cast<std::int16_t>(index),source.name,source.value,
        source.registrationPC,source.nameVA};
    return true;
}
bool OriginalStandardProgramTable50LA::find298C80(const std::string& name,std::int16_t& out,
    OriginalStandardProgramDiagnostic50LA& d) const noexcept {
    d={};
    if(!prepared_)return fail(d,OriginalStandardProgramStatus50LA::unprepared,0x298c80);
    if(name.empty())return fail(d,OriginalStandardProgramStatus50LA::input,0x298c80);
    for(unsigned char c:name)if(c<32||c>=127)
        return fail(d,OriginalStandardProgramStatus50LA::input,0x298c80);
    for(std::size_t i=0;i<count;++i)if(same(name,rows[i].name)){
        out=static_cast<std::int16_t>(i);d.row=out;return true;
    }
    return fail(d,OriginalStandardProgramStatus50LA::missingName,0x298c80);
}
}
