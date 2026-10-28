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
        need(podId.size()==5&&podId.substr(0,3)=="POD"&&podId[3]>='0'&&podId[3]<='9'&&podId[4]>='0'&&podId[4]<='9',"Roster pod identity");
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
        need(prefix==podId.substr(3),"Roster PHY prefix identity differs");
        if(directOriginal){
            const auto* identity=original_player_identity50IK::find(podId);
            need(identity&&pod_materials50ed_fixture::detail::sha256(phy)==identity->phySha256,
                "Original disc PHY identity differs");
            name=value(setup,"pod setup","name");
            // Join the original UI selection comments to this pod's own
            // authenticated speech prefix, preserving POD19's original Event39.
            const auto speech=read("audio/speech/"+lower(podId)+"/taunts.ini");
            need(pod_materials50ed_fixture::detail::sha256(speech)==identity->tauntsSha256,
                "Original disc name speech identity differs");
            const auto speechIni=ini(speech);std::set<std::string> prefixes;
            const auto speechPath="speech/"+lower(podId)+"/";
            for(const auto& section:speechIni)for(const auto& item:section.second)if(item.first=="filename"){
                const auto clip=lower(item.second);
                need(clip.compare(0,speechPath.size(),speechPath)==0&&clip.size()>speechPath.size()+3,
                    "Original disc name speech path differs");
                prefixes.insert(clip.substr(speechPath.size(),3));
            }
            need(prefixes.size()==1,"Original disc name speech prefix is ambiguous");
            const auto speechPrefix=*prefixes.begin();
            const auto disc=resourceFile.parent_path().parent_path().parent_path();
            const auto ui=f::readBytes(disc/"AUDIO/UIACCEPT.INI");
            need(pod_materials50ed_fixture::detail::sha256(ui)=="245b0bd530e6797637ac72bbf2de8f101f3b1da280e00e8fc24e4b4930d79f3b",
                "Original UI selection names differ");
            const std::string text(ui.begin(),ui.end());std::string comment,section;
            for(std::size_t at=0;at<text.size();){auto end=text.find('\n',at);if(end==std::string::npos)end=text.size();
                const auto line=trim(text.substr(at,end-at));at=end+1;
                if(line.empty())continue;
                if(line.front()==';'){comment=trim(line.substr(1));continue;}
                if(line.front()=='['){section=lower(line);continue;}
                const auto equal=line.find('=');if(equal==std::string::npos||lower(trim(line.substr(0,equal)))!="filename")continue;
                const auto clip=lower(trim(line.substr(equal+1)));
                const bool matches=podId=="POD19"?(speechPrefix=="an2"&&section=="[event39]"):
                    clip.compare(0,10+speechPrefix.size(),"speech/ui/"+speechPrefix)==0&&
                    clip.compare(10+speechPrefix.size(),3,"sel")==0;
                if(matches){need(!comment.empty()&&(displayName.empty()||displayName==comment),
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
        if(bottomL){
            need(podId=="POD02"&&value(setup,"pod setup","enginemotion")=="5",
                "Roster optional engine topology requires original POD02 EngineMotion5");
            modelCount=5;
        }
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
                // Authentic POD09 englt1.vut lists damage04..07, while its PSG
                // owns damage00..03. Original153248..15325C logs and skips these
                // four absent names; it never maps them onto the existing four.
                // Other selected models retain the strict historical admission.
                const bool originalMissingDamage=podId=="POD09"&&model==left;
                const bool originalMissingBinder=podId=="POD12"&&model==left;
                const bool originalWrongBinder=podId=="POD20"&&model==right;
                const bool originalWrongFlames=podId=="POD19"&&model==left;
                const bool originalMissingBrakes=podId=="POD22"&&model==chariot;
                const bool originalMissingSpin=podId=="POD25"&&(model==left||model==right);
                if(originalMissingSpin){
                    // Original153248 skips the missing spin LHS, never aliases another node.
                    need(members.at(vutMember).digest==(model==left?
                        "2ba86dd85c18b029ecb5fd0f066ebfa0eafc97d1a1964b9fbbe3e22beee7334a":
                        "0d44b65eebc5244e7cc01487ec2beb4ab90e4bd8c976869e403f8aae873d0c62")&&
                        members.at(psgMember).digest==(model==left?
                        "e06dc530b91da318441f9e3ec89ac9cfcdd1fd957d618f9623de1a0be428bebc":
                        "a89d8932d6ebad3e0b21aae38e15a8387c037d7b0e22e853b23990aeeae6305d"),
                        "POD25 missing-spin projection requires genuine own PSG/VUT");
                }
                if(originalMissingDamage){
                    need(vutMember==base+"englt1.vut"&&psgMember==base+"englt1.psg"&&
                        members.at(vutMember).digest=="af57258b551e3a52223ac2d70bb6b6403c5586242b884088413668525154982f"&&
                        members.at(psgMember).digest=="d6282f412bf96e20e06c57261a3c31ce2073f467e3b95d1716bfc04ac969d12f",
                        "source missing-name admission requires original POD09 left-engine PSG/VUT identities");
                }
                // Authentic POD12 spells its VUT key binderleftt1; the PSG
                // owns binderleft1. Preserve original153248..15325C's skip,
                // rather than repairing the name or manufacturing a node.
                if(originalMissingBinder){
                    need(vutMember==base+"englt1.vut"&&psgMember==base+"englt1.psg"&&
                        members.at(vutMember).digest=="cb7148c2199a65c9a6378f0d3c8a14e646359925700bea4133dc94389db38894"&&
                        members.at(psgMember).digest=="bbd1a0452fecc07503104461c73d60e2d086b728ae04765e5c1cd57d4c1cdcb2",
                        "source missing-name admission requires original POD12 left-engine PSG/VUT identities");
                }
                if(originalWrongBinder){
                    need(vutMember==base+"engrt1.vut"&&psgMember==base+"engrt1.psg"&&
                        members.at(vutMember).digest=="b50ef5b5ac03ca6aac484fd4b67364b7b435701d796fc3053a1a43f7d968da65"&&
                        members.at(psgMember).digest=="be9a0b394caf482167db3fbdb9080476eebdb818b7458dd3c0924e58d25e116f",
                        "source missing-name admission requires original POD20 right-engine PSG/VUT identities");
                }
                // Authentic POD19 left VUT lists engrtflame01/02 at181/208,
                // but its PSG owns engltflame01/02. Original153248..15325C
                // skips these absent lookup keys; it never aliases the real
                // left flame rows or changes their unlisted flags.
                if(originalWrongFlames){
                    need(vutMember==base+"englt1.vut"&&psgMember==base+"englt1.psg"&&
                        members.at(vutMember).digest=="37e4cd4e0fff2472b2a045ef5da8a8e4ca02d1775e7c0d9afbca4affa3ae3524"&&
                        members.at(psgMember).digest=="a5f24cf472e3f52f989cc053236865503c0f6d838a3ce9a18c3b555c0b524b9e",
                        "source missing-name admission requires original POD19 left-engine PSG/VUT identities");
                }
                // Authentic POD22 chariot VUT lists brklt02/brkrt02 at34/87,
                // absent from its18-node PSG. Original153248..15325C diagnoses
                // and skips these lines without manufacturing or renaming nodes.
                if(originalMissingBrakes){
                    need(vutMember==base+"chariot.vut"&&psgMember==base+"chariot.psg"&&
                        members.at(vutMember).digest=="19beab314060e135cf5828a8556e7f656399ae1dc2ba321907f766dd47cac018"&&
                        members.at(psgMember).digest=="b03565147c13202d46cd925134625587e7bd23058ab01499770416a5ef143839",
                        "source missing-name admission requires original POD22 chariot PSG/VUT identities");
                }
                const bool originalMalformedHand=podId=="POD18"&&model==chariot;
                std::size_t bindingBytes=vut[model].size();
                if(originalMalformedHand){
                    need(vutMember==base+"chariot.vut"&&psgMember==base+"chariot.psg",
                        "POD18 original VUT projection role differs");
                    bindingBytes=sebulba18ChariotVutPrefix(psg[model],vut[model]);
                }
                const auto missing=(originalMissingDamage||originalMissingBinder||originalWrongBinder||originalWrongFlames||originalMissingBrakes||originalMissingSpin)?a::MotionBindingMissingNode49O::sourceDiagnosticSkip:
                    a::MotionBindingMissingNode49O::reject;
                if(!a::readMotionNodeBindings49O(hierarchy[model],reinterpret_cast<const char*>(vut[model].data()),
                    bindingBytes,localRows[model],bd,missing))
                    need(false,"native VUT admission: "+vutMember+" error="+bindingErrorName(bd.error)+
                        " byte="+std::to_string(bd.offset)+" skippedUnknown="+std::to_string(bd.skippedUnknownNodes));
                if(originalMalformedHand){
                    need(bd.skippedUnknownNodes==0,"POD18 valid VUT prefix unexpectedly skipped a node");
                    bd.skippedUnknownNodes=1;bd.firstUnknownOffset=352;
                }
                need(bd.skippedUnknownNodes==(originalMissingDamage?4u:(originalWrongFlames||originalMissingBrakes)?2u:(originalMissingBinder||originalWrongBinder||originalMalformedHand||originalMissingSpin)?1u:0u)&&
                    (!originalMalformedHand||bd.firstUnknownOffset==352u)&&
                    (!originalMissingDamage||bd.firstUnknownOffset==57u)&&
                    (!originalWrongFlames||bd.firstUnknownOffset==181u)&&
                    (!originalMissingBrakes||bd.firstUnknownOffset==34u)&&
                    (!(originalMissingBinder||originalWrongBinder)||bd.firstUnknownOffset==15u),
                    "native VUT source missing-name count differs: "+vutMember+" skippedUnknown="+
                    std::to_string(bd.skippedUnknownNodes)+" firstByte="+std::to_string(bd.firstUnknownOffset));
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
