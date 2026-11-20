from edlib import edit
# world50du: replace the shared-helper call by a local fold (no header edit).
edit('tests/native/terrain_interactive_world50du.cpp', [
('''    for(const auto& e:stock.entries)if(track_session_assets50IG::lowerEquals(e.name,wanted)){entry=&e;break;}''',
 '''    const auto folded=[](std::string s){for(auto& c:s)if(c>='A'&&c<='Z')c=static_cast<char>(c-'A'+'a');return s;};
    for(const auto& e:stock.entries)if(folded(e.name)==wanted){entry=&e;break;}'''),
])
edit('tests/native/native_ta_particle_cold50la_fixture.h', [
('''            provider->mount(selected.mount,mounted)&&mounted.archiveName&&*mounted.archiveName==particleArchiveName&&
            selected.member&&*selected.member==requested&&selected.data&&selected.revision==providerRevision&&''',
 '''            provider->mount(selected.mount,mounted)&&mounted.archiveName&&*mounted.archiveName==particleArchiveName&&
            selected.member&&sameName(*selected.member,requested.c_str())&&selected.data&&selected.revision==providerRevision&&'''),
('''    require(matches(member,prt)&&matches(psmMember,psm)&&matches(pstMember,pst)&&
        provider->sealed()&&provider->revision()==providerRevision,
        "Track particle closure differs from the same selected mounted archive");
    auto materialSource=std::make_shared<rr::assets::OriginalStandardMaterialSource50LA>();
    require(materialSource->prepare(psmMember,psm,diagnostic.material.document),
        "Original selected-track particle PSM parsing rejected");
    // Same domain owns the exact PSM/PST beyond these temporary source vectors.
    // The actual particle owner acquires its canonical reference before the
    // caller's14D370/29E340 population. Failure retains real partial effects.
    require(domain->appendOriginalSources({materialSource},{{pstMember,pst}},diagnostic.material),
        "Original selected-track particle canonical source append rejected");
    const bool prepared=application.withParticleMaterialDirectory160F88([&](const std::string& actualDirectory){''',
 '''    const bool prtMatch=matches(member,prt),psmMatch=matches(psmMember,psm),pstMatch=matches(pstMember,pst);
    if(!(prtMatch&&psmMatch&&pstMatch))std::cerr<<"course_route_particle_closure code="<<canonical->world->code
        <<" inherited="<<(inherited?1:0)<<" prt="<<prtMatch<<" psm="<<psmMatch<<" pst="<<pstMatch
        <<" mount="<<(inherited?particleArchiveName:std::string(archiveName))<<'\\n';
    require(prtMatch&&psmMatch&&pstMatch&&
        provider->sealed()&&provider->revision()==providerRevision,
        "Track particle closure differs from the same selected mounted archive");
    auto materialSource=std::make_shared<rr::assets::OriginalStandardMaterialSource50LA>();
    require(materialSource->prepare(psmMember,psm,diagnostic.material.document),
        "Original selected-track particle PSM parsing rejected");
    // Same domain owns the exact PSM/PST beyond these temporary source vectors.
    // The actual particle owner acquires its canonical reference before the
    // caller's14D370/29E340 population. Failure retains real partial effects.
    const bool appended=domain->appendOriginalSources({materialSource},{{pstMember,pst}},diagnostic.material);
    if(!appended)std::cerr<<"course_route_particle_append code="<<canonical->world->code<<" inherited="<<(inherited?1:0)
        <<" status="<<unsigned(diagnostic.material.status)<<" pc="<<std::hex<<diagnostic.material.sourcePC<<std::dec<<'\\n';
    require(appended,"Original selected-track particle canonical source append rejected");
    const bool prepared=application.withParticleMaterialDirectory160F88([&](const std::string& actualDirectory){'''),
])
