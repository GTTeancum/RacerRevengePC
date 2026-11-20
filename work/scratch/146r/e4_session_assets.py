from edlib import edit
edit('tests/native/track_session_assets50ig_fixture.h', [
('''        auto& s=registrySessionStorage();s=RegistrySessionStorage{};''',
 '''        auto& s=registrySessionStorage();
        // Built once per code: rows already handed out (ResolvedSessionAssets copies,
        // borrowed const char*) stay valid while the same course is asked again.
        if(s.assets.registry&&s.code==load.codeUpper){error=nullptr;return &s.assets;}
        s=RegistrySessionStorage{};'''),
])
