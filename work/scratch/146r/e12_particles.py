from edlib import edit
edit('tests/native/original_single_pass_world50lm.inl', [
('''    bool stockRow=true;                    // stock pins/counts exist for this row
};''', '''    bool stockRow=true;                    // stock pins/counts exist for this row
    // inherit.particles (registry row only; PORT EXTENSION, NOT ORIGINAL): the
    // particle closure members (particle\\demoprt.prt/.pst/.psm) of the stock
    // course archive the row names, mounted beside the course archive in the
    // SAME native namespace before the seal, so the particle owner and the
    // canonical material domain authenticate them like members of the course
    // archive. Name/SHA/bytes are the stock archive's own (read from the install).
    std::string particleContainer,particleSha;
    std::size_t particleBytes=0;
    std::shared_ptr<const a::ResArchive48C> particleArchive;
};'''),
('''            shared::need(provider&&provider->mountAuthenticatedRes(identity_.container,::rr::assets::originalTrackMountDigest50LA(identity_.container,identity_.sha.c_str()),::rr::assets::originalTrackMountBytes50LA(identity_.container,identity_.bytes),storage_->archive,mounted)&&
                provider->sealNativeMounts(mounted)''', '''            shared::need(provider&&provider->mountAuthenticatedRes(identity_.container,::rr::assets::originalTrackMountDigest50LA(identity_.container,identity_.sha.c_str()),::rr::assets::originalTrackMountBytes50LA(identity_.container,identity_.bytes),storage_->archive,mounted)&&
                (!identity_.particleArchive||provider->mountAuthenticatedRes(identity_.particleContainer,
                    ::rr::assets::originalTrackMountDigest50LA(identity_.particleContainer,identity_.particleSha.c_str()),
                    ::rr::assets::originalTrackMountBytes50LA(identity_.particleContainer,identity_.particleBytes),*identity_.particleArchive,mounted))&&
                provider->sealNativeMounts(mounted)'''),
])
edit('tests/native/terrain_interactive_world50du.cpp', [
('''      trackSession->archiveSha256,trackSession->archiveBytes,worldIdentity),"common-route registry course identity rejected");''',
 '''      trackSession->archiveSha256,trackSession->archiveBytes,worldIdentity),"common-route registry course identity rejected");
  // inherit.particles: the stock course's particle closure joins this course's
  // native namespace (see Identity::particleArchive). Members only, in their
  // stock archive's order; the archive identity is a receipt.
  rr::game::dlc50DL::AddonCourseLoad particleRow;
  if(trackSession->registry&&rr::game::dlc50DL::addonCourseLoadByCode(trackSession->world->code,particleRow)&&
     particleRow.particlesInherited){
   const auto stockBytes=runtime_startup50fl::readBytes("work/native/project/disc/"+particleRow.particleArchive);
   rr::assets::ResArchive48C stock;rr::assets::DecodeError48C error;
   require50DU(!stockBytes.empty()&&stockBytes.size()<=course_archive_receipt50CI::maximumArchiveBytes()&&
       rr::assets::decodeResource48C(stockBytes,stock,error,nullptr,course_archive_receipt50CI::limits()),
       "inherited particle archive is not a readable RES");
   auto subset=std::make_shared<rr::assets::ResArchive48C>();
   for(const char* name:{"particle\\\\demoprt.prt","particle\\\\demoprt.psm","particle\\\\demoprt.pst"}){
    std::vector<std::uint8_t> member;
    require50DU(rr::assets::extractResMember48C(stock,name,member,error)&&!member.empty()&&
        member.size()<=std::numeric_limits<std::uint32_t>::max()-subset->decoded.size(),
        "inherited particle member missing from the named stock archive");
    subset->entries.push_back({name,static_cast<std::uint32_t>(subset->decoded.size()),static_cast<std::uint32_t>(member.size())});
    subset->decoded.insert(subset->decoded.end(),member.begin(),member.end());
   }
   subset->blocks.push_back({0u,static_cast<std::uint32_t>(subset->decoded.size())});
   worldIdentity.particleContainer=particleRow.particleArchive;
   worldIdentity.particleSha=pod_materials50ed_fixture::detail::sha256(stockBytes);
   worldIdentity.particleBytes=stockBytes.size();
   worldIdentity.particleArchive=std::move(subset);
   std::cerr<<"course_route_particle_mount code="<<trackSession->world->code<<" container="<<worldIdentity.particleContainer
       <<" members=3 archive_bytes="<<worldIdentity.particleBytes<<'\\n';
  }'''),
])
edit('tests/native/native_ta_particle_cold50la_fixture.h', [
('''        if(inherited)return provider->lookup132820(requested,selected)!=Provider::Lookup::present;''',
 '''        // Inherited closure: present ONLY in the inherited stock-archive mount the
        // common world added to this namespace (Identity::particleArchive), with
        // exactly these bytes.
        if(inherited)return provider->lookup132820(requested,selected)==Provider::Lookup::present&&
            provider->mount(selected.mount,mounted)&&mounted.archiveName&&*mounted.archiveName==particleArchiveName&&
            selected.member&&*selected.member==requested&&selected.data&&selected.revision==providerRevision&&
            selected.size==original.size()&&!std::memcmp(selected.data,original.data(),original.size());'''),
])
