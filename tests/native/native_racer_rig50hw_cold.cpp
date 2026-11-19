// Exact existing cold body outlined to its own translation unit.
// No ownership, allocation, source arithmetic, or exception-policy change.
#include "native_racer_rig50hw_fixture.h"

namespace native_racer50HW {
Rig::Rig(const std::filesystem::path& repository,const std::filesystem::path& podJson) {
        using namespace rig_detail;
        const auto resolved=runtime_asset_paths50LB::resolve(podJson);
        const auto file=resolved.is_absolute()?resolved:repository/resolved;
        directory=file.parent_path();J pod;
        directOriginal=lower(file.extension().string())==".res";
        if(directOriginal){
            resourceFile=file;podId=file.stem().string();
            for(auto& c:podId)if(c>='a'&&c<='z')c=char(c-'a'+'A');
            const auto* identity=original_player_identity50IK::find(podId);
            need(identity,"Original disc pod has no admitted identity");
            const auto raw=f::readBytes(resourceFile);
            const auto digest=pod_materials50ed_fixture::detail::sha256(raw);
            need(raw.size()==identity->archiveBytes&&digest==identity->archiveSha256,
                "Original disc pod archive identity differs: "+podId);
            a::DecodeError48C error;
            need(a::decodeResource48C(raw,originalArchive,error),"Original pod archive rejected: "+error.code);
            manifest={{"schema","RR_ORIGINAL_DISC_POD_OWNER_V1"},{"source_container",resourceFile.filename().string()},
                {"source_container_bytes",raw.size()},{"source_container_sha256",digest},{"members",J::array()}};
            std::set<std::string> names;
            for(std::size_t i=0;i<originalArchive.entries.size();++i){const auto& entry=originalArchive.entries[i];
                need(names.insert(lower(entry.name)).second,"Original pod archive names are ambiguous");
                std::vector<std::uint8_t> member;
                need(a::extractResMember48C(originalArchive,entry.name,member,error),
                    "Original pod member rejected: "+entry.name+" "+error.code);
                manifest["members"].push_back({{"source_member",entry.name},{"source_index",i},
                    {"logical_offset",entry.logical_offset},{"source_bytes",member.size()},
                    {"source_sha256",pod_materials50ed_fixture::detail::sha256(member)}});
            }
            pod={{"phy",{{"source_member","pods/"+lower(podId)+"/"+lower(podId)+".phy"}}}};
        }else{
            pod=json(file);manifest=json(directory/"import.json");
            podId=pod.at("pod_id").get<std::string>();name=pod.at("name").get<std::string>();
            resourceFile=repository/runtime_asset_paths50LB::resolve("work/native/project/disc/PODS")/podId/(podId+".RES");
        }
        // Stock Pod%02d spelling, or an admitted add-on row's id (upper-case
        // stem; PORT EXTENSION, NOT ORIGINAL). Same pod load either way.
        need(original_player_identity50IK::admittedPodId(podId),"Roster pod identity");
        struct Member {std::filesystem::path path;std::string digest;std::size_t bytes;std::string sourceName;};
        std::map<std::string,Member> members;
        for(const auto& entry:manifest.at("members")){
            const auto path=directOriginal?std::filesystem::path{}:std::filesystem::path(entry.at("source_file").get<std::string>());
            need(!path.is_absolute()&&path==path.filename(),"Roster imported member path escapes owner");
            need(members.emplace(lower(entry.at("source_member").get<std::string>()),Member{directory/path,entry.at("source_sha256").get<std::string>(),entry.at("source_bytes").get<std::size_t>(),entry.at("source_member").get<std::string>()}).second,
                "Roster duplicate source member");
        }
        auto read=[&](const std::string& member){const auto found=members.find(lower(member));
            need(found!=members.end(),"Roster "+podId+" source member missing: "+member);
            auto bytes=directOriginal?originalMember(member):f::readBytes(found->second.path);
            need(bytes.size()==found->second.bytes&&pod_materials50ed_fixture::detail::sha256(bytes)==found->second.digest,
                "Roster "+podId+" retained original member identity differs: "+member);return bytes;};
        phy=read(pod.at("phy").at("source_member").get<std::string>());
        const auto setup=ini(phy);const auto prefix=value(setup,"pod setup","prefix");
        need(prefix==original_player_identity50IK::phyPrefix(podId),"Roster PHY prefix identity differs");
        if(directOriginal){
            const auto* identity=original_player_identity50IK::find(podId);
            need(identity&&pod_materials50ed_fixture::detail::sha256(phy)==identity->phySha256,
                "Original disc PHY identity differs");
            name=value(setup,"pod setup","name");
        }
        if(directOriginal&&original_player_identity50IK::addonPodOf(podId)){
            // PORT EXTENSION, NOT ORIGINAL: an add-on pod has no row in the
            // original UIACCEPT first-event table; its display name is the
            // package string (AddonPod::name, dlc-contract-2 name_key).
            displayName=original_player_identity50IK::addonPodOf(podId)->name;
            need(!displayName.empty(),"Add-on pod display name unavailable");
        }else if(directOriginal){
            const auto* identity=original_player_identity50IK::find(podId);
            need(identity,"Original disc pod has no admitted identity");
            // Original1E7C74..1E7E80 fills the UIACCEPT first-event table at
            // +5428+4*racer60 (counts at +53BC+4*racer60); 1EA764..1EA780 plays
            // Event(first+random). The racer-keyed table is the original's own
            // data; the name is the UIACCEPT comment that heads that event.
            static constexpr unsigned firstUiAcceptEvent[27]{0,1,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39,44,46,50,52,54,56,60};
            need(identity->racer60<27&&firstUiAcceptEvent[identity->racer60]!=0,
                "Original UI first-event table has no row for this racer");
            const auto disc=resourceFile.parent_path().parent_path().parent_path();
            const auto ui=f::readBytes(disc/"AUDIO/UIACCEPT.INI");
            need(pod_materials50ed_fixture::detail::sha256(ui)=="245b0bd530e6797637ac72bbf2de8f101f3b1da280e00e8fc24e4b4930d79f3b",
                "Original UI selection names differ");
            const auto eventSection="[event"+std::to_string(firstUiAcceptEvent[identity->racer60])+"]";
            const std::string text(ui.begin(),ui.end());std::string comment;
            for(std::size_t at=0;at<text.size();){auto end=text.find('\n',at);if(end==std::string::npos)end=text.size();
                const auto line=trim(text.substr(at,end-at));at=end+1;
                if(line.empty())continue;
                if(line.front()==';'){comment=trim(line.substr(1));continue;}
                if(line.front()=='['&&lower(line)==eventSection){need(!comment.empty()&&displayName.empty(),
                    "Original UI speech/name association ambiguous");displayName=comment;}
            }
            need(!displayName.empty(),"Original UI name unavailable for this pod");
        }
        const std::string base="pods/"+lower(podId)+"/";
        const char* roles[]={"leftengine","rightengine","chariot","bottomleftengine","bottomrightengine"};
        const auto& authoredSetup=setup.at("pod setup");
        const bool bottomL=authoredSetup.count("bottomleftengine")!=0;
        const bool bottomR=authoredSetup.count("bottomrightengine")!=0;
        need(bottomL==bottomR,"Roster partial bottom-engine topology unavailable");
        // Original211470..211488 loads the bottom pair only when this pod's
        // [Pod Setup] BottomLeftEngine key is present (EngineMotion is a
        // separate PHY value read at214700, authored 5 for every pod).
        if(bottomL)modelCount=5;
        const char* suffixes[]={"engineleft","engineright","brakeleft","brakeright","forward","back","mid","enginerightmid","engineleftmid"};
        const unsigned motionModels[]={left,right,chariot,chariot,chariot,chariot,chariot,right,left};
        std::uint32_t next=0;
        for(unsigned model=0;model<modelCount;++model){
            const auto selected=base+lower(value(setup,"pod setup",roles[model]));
            try {
            const auto psgMember=withExtension(selected,".psg");
            psg[model]=read(psgMember);psgMembers[model]=members.at(lower(psgMember)).sourceName;
            a::GeometryDiagnostic49O gd{};
            if(!a::readPsgHierarchy49O(psg[model].data(),psg[model].size(),hierarchy[model],gd))
                need(false,"native PSG admission: "+psgMember+" error="+
                    std::to_string(static_cast<unsigned>(gd.code))+" byte="+std::to_string(gd.byteOffset));
            roots[model]=static_cast<Index>(next++);
            need(hierarchy[model].nodeCount<=a::maxModelNodes49O&&next+hierarchy[model].nodeCount+4<=g::vehicleFrameNodeCount49W,
                "Roster forest exceeds native scene capacity");
            for(unsigned n=0;n<hierarchy[model].nodeCount;++n)sourceNodes[model][n]=static_cast<Index>(next+n);
            next+=hierarchy[model].nodeCount;
            const auto mcfMember=withExtension(selected,".mcf");
            mcf[model]=read(mcfMember);mcfMembers[model]=members.at(lower(mcfMember)).sourceName;
            const auto config=ini(mcf[model]);
            std::size_t consumed=0;const auto count=std::stoul(value(config,"general info","numberofmotions"),&consumed);
            need(consumed==value(config,"general info","numberofmotions").size()&&count<=MotionCount,"Roster MCF motion count");
            const auto content=lower(value(config,"general info","contentdirectory"));
            need(content==base,"Roster MCF content directory differs from selected pod");
            if(count){
                const auto vutMember=content+lower(value(config,"general info","vutfilename"));
                vut[model]=read(vutMember);
                auto& bd=bindingDiagnostics[model];
                // Original153238..15325C: every VUT LHS absent from this model's
                // PSG is diagnosed and skipped, for every pod and model. It never
                // renames, aliases or manufactures a node; text and hierarchy
                // admission stay strict. bd records the skipped count/offset.
                if(!a::readMotionNodeBindings49O(hierarchy[model],reinterpret_cast<const char*>(vut[model].data()),
                    vut[model].size(),localRows[model],bd,a::MotionBindingMissingNode49O::sourceDiagnosticSkip))
                    need(false,"native VUT admission: "+vutMember+" error="+bindingErrorName(bd.error)+
                        " byte="+std::to_string(bd.offset)+" skippedUnknown="+std::to_string(bd.skippedUnknownNodes));
                rows[model]=localRows[model];
                for(unsigned n=0;n<rows[model].count;++n)
                    rows[model].rows[n].sceneNode=static_cast<std::uint8_t>(sourceNodes[model][localRows[model].rows[n].sceneNode]);
            }
            if(!count){
                // Original152EA0 enumerates the PSG even with no MCF motions.
                // Keep absent VUT/MOT absent, but retain the real static nodes
                // for fresh bounds and bind-pose geometry (not a dummy motion).
                a::MotionBindingDiagnostic49O enumeration;
                need(a::readMotionNodeEnumeration49O(hierarchy[model],localRows[model],enumeration),
                    "Static original model PSG enumeration failed");
                rows[model]=localRows[model];
                for(unsigned n=0;n<rows[model].count;++n)
                    rows[model].rows[n].sceneNode=static_cast<std::uint8_t>(sourceNodes[model][localRows[model].rows[n].sceneNode]);
            }
            // NumberOfMotions=0 is authentic. Its absent VUT/MOT stays absent;
            // the procedural engine response still owns its real PSG subtree.
            std::string selectedMotionSection;
            for(unsigned n=1;n<=count;++n){
                const auto requestedSection="motion "+std::to_string(n);
                const bool retainedSection=config.find(requestedSection)==config.end();
                if(!retainedSection)selectedMotionSection=requestedSection;
                need(!selectedMotionSection.empty(),"Roster missing first MCF motion section");
                const auto& section=selectedMotionSection;
                const auto identity=lower(value(config,section,"name"));unsigned slot=MotionCount;
                for(unsigned i=0;i<MotionCount;++i)if(identity==prefix+"_"+suffixes[i])slot=i;
                need(slot<MotionCount&&motionModels[slot]==model,"Roster unexpected MCF motion identity");
                //157D00 retains the prior section on a missing ordinal;
                //151270 then resolves its existing NAME through1523F0. Rig
                //owns unique decoded tracks, not153610's retained slot array.
                //Keep the original count in MCF; canonical lifetime owns and
                //balances both references to this same authenticated resource.
                if(retainedSection){
                    need(motions[slot].data&&motionNames[slot]==value(config,section,"name"),
                        "Roster retained MCF section requires admitted motion identity");
                    continue;
                }
                need(!motions[slot].data,"Roster duplicate MCF motion identity");
                const auto motMember=content+withExtension(lower(value(config,section,"vue")),".mot");
                mot[slot]=read(motMember);
                motMembers[slot]=members.at(lower(motMember)).sourceName;
                motionNames[slot]=value(config,section,"name");
                if(!a::readMot49O(mot[slot].data(),mot[slot].size(),motions[slot],gd))
                    need(false,"native MOT admission: "+motMember+" identity="+identity+" error="+
                        std::to_string(static_cast<unsigned>(gd.code))+" byte="+std::to_string(gd.byteOffset));
                for(unsigned channel=0;channel<2;++channel){const auto tracks=channel?motions[slot].positionCount:motions[slot].rotationCount;
                    for(unsigned t=0;t<tracks;++t)need((channel?motions[slot].position[t]:motions[slot].rotation[t]).rowIndex<rows[model].count,"Roster MOT/VUT row binding");}
            }
            } catch(const std::exception& error) {
                throw std::runtime_error("Roster "+podId+" "+roles[model]+" ["+selected+"]: "+error.what());
            }
        }
        physical=static_cast<Index>(next++);main=static_cast<Index>(next++);
        container=static_cast<Index>(next++);hidden=static_cast<Index>(next++);nodeCount=next;
        //21380C/213818 skip the pair if either actual MCF lookup is absent.
        const unsigned order[]={chariot,right,left,bottomRight,bottomLeft};
        for(unsigned i=0;i<modelCount;++i){const auto model=order[i];rebaseModels[i]={roots[model],hierarchy[model],sourceNodes[model],hierarchy[model].nodeCount};}
        const auto sources=geometrySources();g::FreshPodGeometryWorkspace49O work{};g::FreshPodGeometryDiagnostic49O diagnostic{};
        if(!g::buildFreshPodGeometry49O(sources,work,geometry,diagnostic))
            need(false,"Roster "+podId+" chariot fresh geometry error="+
                std::to_string(static_cast<unsigned>(diagnostic.error))+" motionError="+
                std::to_string(static_cast<unsigned>(diagnostic.motion.error))+" track="+
                std::to_string(diagnostic.motion.track));
    }
} // namespace native_racer50HW
