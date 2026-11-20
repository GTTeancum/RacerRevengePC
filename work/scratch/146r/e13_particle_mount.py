from edlib import edit
edit('tests/native/terrain_interactive_publication50du.cpp', [
('''   const auto provider=trackMaterialRequest.canonicalDomain->archiveProvider3E1A70();
   rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic mounted;
   require50DU(provider&&archive&&
       (taOriginalWorld?taOriginalWorld->mountCanonicalArchive(*provider,mounted,native_course_archive_release50PD::holder().begin(provider,
            trackMaterialRequest.canonicalDomain->resolver3E1A70())):
           // Label = the archive actually admitted (course_cut_identity50ci.h).
           provider->mountAuthenticatedRes(trackSession->world->container,
               course_cut_identity50CI::mountDigest(trackSession->world->code,trackSession->archiveSha256),
               course_cut_identity50CI::mountBytes(trackSession->world->code,trackSession->archiveBytes),
               *archive,mounted,native_course_archive_release50PD::holder().begin(provider,
                   trackMaterialRequest.canonicalDomain->resolver3E1A70())))&&provider->sealNativeMounts(mounted)&&''',
 '''   const auto provider=trackMaterialRequest.canonicalDomain->archiveProvider3E1A70();
   rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic mounted;
   // One race lease for every mount of this course's namespace (released at race end).
   auto* const raceLease=provider?native_course_archive_release50PD::holder().begin(provider,
       trackMaterialRequest.canonicalDomain->resolver3E1A70()):nullptr;
   // inherit.particles (146r, PORT EXTENSION, NOT ORIGINAL): the common world
   // carries the stock archive's particle closure members (Identity::particleArchive);
   // they join THIS namespace beside the course archive, under the stock
   // archive's own name/SHA/bytes, before the seal, so the particle owner and the
   // canonical domain authenticate them like members of a mounted archive.
   const auto* inheritedParticles=commonRouteWorld?&commonRouteWorld->identity():nullptr;
   const bool mountInheritedParticles=inheritedParticles&&inheritedParticles->particleArchive;
   require50DU(provider&&archive&&
       (taOriginalWorld?taOriginalWorld->mountCanonicalArchive(*provider,mounted,raceLease):
           // Label = the archive actually admitted (course_cut_identity50ci.h).
           provider->mountAuthenticatedRes(trackSession->world->container,
               course_cut_identity50CI::mountDigest(trackSession->world->code,trackSession->archiveSha256),
               course_cut_identity50CI::mountBytes(trackSession->world->code,trackSession->archiveBytes),
               *archive,mounted,raceLease))&&
       (!mountInheritedParticles||provider->mountAuthenticatedRes(inheritedParticles->particleContainer,
           ::rr::assets::originalTrackMountDigest50LA(inheritedParticles->particleContainer,inheritedParticles->particleSha.c_str()),
           ::rr::assets::originalTrackMountBytes50LA(inheritedParticles->particleContainer,inheritedParticles->particleBytes),
           *inheritedParticles->particleArchive,mounted,raceLease))&&provider->sealNativeMounts(mounted)&&'''),
('''       "canonical selected-track archive mount rejected");
   require50DU(native_ta_particle_cold50LA::prepare(''',
 '''       "canonical selected-track archive mount rejected");
   if(mountInheritedParticles)std::cerr<<"course_route_particle_namespace code="<<trackSession->world->code
       <<" container="<<inheritedParticles->particleContainer<<" mounted=1 lease="<<(raceLease?1:0)<<'\\n';
   require50DU(native_ta_particle_cold50LA::prepare('''),
])
