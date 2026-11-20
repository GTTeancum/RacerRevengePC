// Common rewrite (R3): the ONE static world owner body (former single-pass world / SA body),
// for every course. identity_ (built from the race's Track) is the only source of member
// names, archive identity and asset identity. No stock row: every count is the data's own;
// the stock values are receipts (stock_course_receipts50cr.h) logged by the Track.
#include "single_pass_world50lm.h"
#include "original_world_material_admission50cm.h"
#include "course_world_texture50st.h"   // the one texture budget (R7, 382B38/298900/2993A8) and texture reader
#include "../../src/assets/original_texture_candidate50tc.h"   // 24DADC texture request spelling
namespace wma=original_world_material_admission50CM;

namespace single_pass_world50LM {
// Raw archive bytes: decoded here, then the one body below.
bool Owner::load(const Bytes& original,const a::World48F& world,Diagnostic& d)noexcept{
        try{
            d={};shared::need(!storage_&&wma::archiveFits(original.size()),"single-pass world already loaded or archive exceeds the resource file limit");
            a::ResArchive48C decoded;d.stage="archive";
            a::DecodeLimits48C limits=wma::archiveLimits(original.size()); // file-sized; decoded/entry caps are the shared resource limits
            shared::need(a::decodeResource48C(original,decoded,d.decode,nullptr,limits),
                "Single-pass world archive decode: "+d.decode.code);
            return loadDecoded(decoded,world,d);
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Single-pass world archive decode failed";}catch(...){}return false;}
}
// The Track's already decoded and authenticated archive (common_track50CR::Archive, step 04).
// identity_.sha / identity_.bytes are the Track's actual archive identity.
bool Owner::loadDecoded(const a::ResArchive48C& archive,const a::World48F& world,Diagnostic& d)noexcept{
        try{
            d={};shared::need(!storage_&&std::fegetround()==FE_TONEAREST,"single-pass world direct cold owner already loaded or rounding mode differs");
            const auto& id=identity_;
            auto next=std::make_unique<Storage>();d.stage="archive";
            next->archive=archive;
            for(std::size_t i=0;i<next->archive.entries.size();++i)
                shared::need(next->entries.emplace(detail::key(next->archive.entries[i].name),i).second,"Ambiguous original single-pass world member");
            d.stage="world";const auto* scb=next->entry(id.scb);
            shared::need(scb&&scb->bytes,"Single-pass world SCB extent differs");
            const auto scbBytes=next->member(*scb,d.decode);
            const auto actualScbSha=shared::sha256(scbBytes);
            wma::identityReceipt(id.code.c_str(),false,id.sha,id.bytes,next->archive.decoded.size(),next->archive.entries.size(),actualScbSha,scbBytes.size());
            detail::validateBorrowedWorld(scbBytes,world,d.decode);
            std::map<std::string,std::uint32_t> materialIndices,textureIndices;
            // Stock pin per admitted texture, only when its bytes are the stock bytes
            // (course_archive_receipt50ci.h: a differing pin is a receipt, logged by detail::image).
            std::map<std::string,const detail::SourceImage*> texturePins;
            std::vector<Bytes> images;
            std::vector<a::MaterialTextureSlotInput50EA> slots;
            const std::uint64_t textureBase=id.asset<<48;
            // One texture budget for every course (R7: course_world_texture50ST), no per-course 16/32/64 MiB.
            const auto budgets=course_world_texture50ST::textureBudgets50ST();
            std::size_t rgbaBytes=0,serializedBytes=0;
            auto material=[&](const std::string& materialName)->std::uint32_t{
                const auto normalized=detail::key(materialName);const auto existing=materialIndices.find(normalized);
                if(existing!=materialIndices.end())return existing->second;
                shared::need(shared::name(materialName),"Invalid original single-pass world material basename");
                d.stage="material";const auto* psm=next->entry("textures\\"+materialName+".psm");
                shared::need(psm,"Single-pass world world material absent: "+materialName);d.member=psm->name;
                const auto source=next->member(*psm,d.decode);a::Material48E authored;
                shared::need(a::decodeMaterial48E(source,authored,d.decode)&&wma::materialPassCount(authored),
                    "Single-pass world world material subset: "+d.member+" "+d.decode.code);
                const auto materialIndex=static_cast<std::uint32_t>(next->materials.size());
                shared::need(materialIndex<wma::maxWorldMaterials,"Single-pass world PSM count bound");
                const auto materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+shared::sha256(source));
                std::vector<std::uint32_t> selectedPasses;
                for(std::size_t pi=0;pi<authored.passes.size();++pi){
                    const auto& pass=authored.passes[pi];const bool projected=wma::projected(pass);
                    shared::need(wma::pass(authored,pi), // common admission, no name/course key
                        "Unjoined original single-pass world pass/program/texture subset");
                    const auto& map=pass.textures[0];
                    // 1169D0/24DA98..24DADC: the request's last dot (if any) is replaced by
                    // .pst, so an authored map "x.tga" or "x.psm" names the same stem as "x".
                    // One rule for every material (retires the exact-bytes TC admission).
                    const auto candidate=a::detail::originalPstCandidate24DADC(map.name);
                    const auto stem=candidate.substr(0,candidate.find_last_of('.'));
                    shared::need(shared::name(stem),"Invalid original single-pass world texture basename");
                    const a::ResourceEntry48C* texture=nullptr;
                    for(const char* extension:{".pst",".ipu",".tga",".atm"}){
                        texture=next->entry("textures\\"+stem+extension);if(texture)break;
                    }
                    shared::need(texture,"Single-pass world required texture absent: "+map.name);
                    const auto textureKey=detail::key(texture->name);
                    const bool animated=textureKey.ends_with(".ATM");
                    auto found=textureIndices.find(textureKey);
                    if(found==textureIndices.end()){
                        d.stage="texture";d.member=texture->name;const auto bytes=next->member(*texture,d.decode);
                        const detail::SourceImage* pin=nullptr;
                        texturePins.emplace(textureKey,pin);
                        // The one static-world texture reader (course_world_texture50ST: PST/ATM/IPUM and
                        // TGA 29EA90/29EE40), from the bytes alone. No pin tables on this owner.
                        course_world_texture50ST::Shape shape;
                        auto image=course_world_texture50ST::decode(id.code.c_str(),textureKey,bytes,
                            detail::initialFrame(pass.fields),shape,d.decode);
                        const std::uint32_t mip=shape.mipIndex;const bool decodedAnimated=shape.animated,independent=false;
                        shared::need(decodedAnimated==animated,"Single-pass world texture animation source classification");
                        const auto index=static_cast<std::uint32_t>(images.size());
                        shared::need(index<budgets.textureCapacity&&image.size()>=16,"single-pass world original texture bank bound");
                        const auto w=shared::u32(image.data()+8),h=shared::u32(image.data()+12);
                        shared::need(w&&h&&w<=1024&&h<=1024&&image.size()==std::size_t(w)*h*4+16,"single-pass world native image extent");
                        shared::add(rgbaBytes,std::size_t(w)*h*4,budgets.rgbaByteCapacity,"single-pass world texture RGBA budget");
                        shared::add(serializedBytes,image.size(),budgets.sourceByteCapacity,"single-pass world texture framing budget");
                        tm::TextureSelection selected;selected.identity=textureBase+index+1;selected.bankIndex=index;
                        selected.mipIndex=mip;selected.width=w;selected.height=h;selected.path=texture->name;
                        selected.key="disc:"+textureKey+":"+(pin?std::string(pin->sourceSha256):shared::sha256(bytes)); // unpinned: runtime source identity
                        selected.sha256=shared::sha256(image); // Actual runtime output identity, not necessarily an independent pin.
                        next->textures.push_back(std::move(selected));images.push_back(std::move(image));
                        next->independentlyPinnedImages.push_back(independent?1:0);
                        if(independent)++d.independentlyPinnedImages;else ++d.runtimeIdentityImages;
                        found=textureIndices.emplace(textureKey,index).first;
                    }
                    {
                        const auto pinned=texturePins.find(textureKey);
                        const auto* pin=pinned==texturePins.end()?nullptr:pinned->second;
                        if(animated)shared::need(authored.passes.size()==1&&(!pin||detail::initialFrame(pass.fields)==pin->initialFrame),
                            "Single-pass world material/ATM initial frame differs");
                    }
                    const auto index=static_cast<std::uint32_t>(next->selections.size());
                    shared::need(index<budgets.slotCapacity,"Single-pass world authored pass count bound");
                    const auto& image=next->textures[found->second];tm::Selection selected;
                    selected.assetIdentity=id.asset;selected.materialIndex=materialIndex;selected.pass=pass.pass;selected.slot=1;
                    selected.bankSlot=static_cast<std::uint32_t>(slots.size());selected.textureIndex=found->second;selected.mipIndex=image.mipIndex;
                    selected.materialName=materialName;selected.materialKey=materialKey;
                    selected.program=pass.microprogram;selected.textureName=map.name;selected.textureKey=image.key;
                    selected.passCount=static_cast<std::uint32_t>(authored.passes.size());
                    selected.clampU=detail::clamp(pass.fields,"wrapModeHorizontal");selected.clampV=detail::clamp(pass.fields,"wrapModeVertical");
                    selected.sourcePassFields=detail::fields(pass.fields);selected.staticAnimationFrame=animated;
                    selected.projectedPrelit=projected;
                    if(projected)selected.texGenMatrix=detail::projection(selected.sourcePassFields);
                    selected.decodedSourceMember=texture->name;selected.decodedSourceSha256=wma::keySha(image.key);
                    slots.push_back({{selected.assetIdentity,materialIndex,pass.pass,1},image.identity,a::MaterialTextureRequirement50EA::required});
                    selectedPasses.push_back(index);next->selections.push_back(std::move(selected));
                }
                next->materials.push_back(std::move(authored));next->materialPasses.push_back(std::move(selectedPasses));
                materialIndices.emplace(normalized,materialIndex);return materialIndex;
            };
            constexpr std::array<double,16> identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
            for(std::size_t wi=0;wi<world.instances.size();++wi){
                rr::platform::loading_progress50DU::pulse();
                const auto& placement=world.instances[wi];d.stage="scene";d.member=placement.geometry_member;
                const auto* entry=next->entry(placement.geometry_member);
                if(!entry){shared::need(next->entry(placement.collision_member),"single-pass world placement lacks both original PSG and COL");++d.collisionOnly;continue;}
                const auto name=detail::key(entry->name);auto found=next->scenes.find(name);
                if(found==next->scenes.end()){
                    Scene scene;scene.identity=next->scenes.size()+1;
                    const auto originalScene=next->member(*entry,d.decode);
                    a::SceneLimits48D sceneLimits;sceneLimits.allow_signed32_quantized_positions=true;
                    shared::need(a::decodeScene48D(originalScene,scene.decoded,d.decode,nullptr,sceneLimits),
                        "Single-pass world PSG decode: "+entry->name+" "+d.decode.code);
                    shared::need(scene.decoded.nodes.size()==1&&scene.decoded.nodes[0].parent==-1&&
                        !scene.decoded.identity_matrix_palette&&!scene.decoded.lods.empty(),"Single-pass world static hierarchy subset");
                    for(const auto& name:scene.decoded.materials)scene.materials.push_back(material(name));
                    found=next->scenes.emplace(name,std::move(scene)).first;
                }
                const auto& scene=found->second;const auto& matrix=scene.decoded.world_space?identity:placement.matrix;
                a::WorldNormalTransform50HK normalTransform;
                shared::need(a::prepareWorldNormalTransform50HK(matrix,normalTransform),"Single-pass world visual/normal transform");
                for(std::size_t si=0;si<scene.decoded.lods.front().surfaces.size();++si){
                rr::platform::loading_progress50DU::pulse();
                    const auto& surface=scene.decoded.lods.front().surfaces[si];d.stage="surface";d.member=entry->name;
                    shared::need(surface.material_index<scene.materials.size()&&!surface.vertices.empty()&&!surface.indices.empty()&&
                        !(surface.indices.size()%3)&&surface.vertices.size()<=a::max_mesh_vertices-next->mesh.vertices.size()&&
                        surface.indices.size()<=a::max_mesh_indices-next->mesh.indices.size(),"single-pass world original surface/material/geometry bounds");
                    tm::Span span;span.vertexStart=static_cast<std::uint32_t>(next->mesh.vertices.size());
                    span.vertexCount=static_cast<std::uint32_t>(surface.vertices.size());span.indexStart=static_cast<std::uint32_t>(next->mesh.indices.size());
                    span.indexCount=static_cast<std::uint32_t>(surface.indices.size());span.worldInstanceIndex=static_cast<std::uint32_t>(wi);
                    span.surfaceIndex=static_cast<std::uint32_t>(si);span.hasOriginalColors=surface.has_original_colors;
                    const auto& selectedPasses=next->materialPasses.at(scene.materials[surface.material_index]);
                    shared::need(!selectedPasses.empty()&&selectedPasses.size()<=wma::maxWorldPasses,"Single-pass world surface material pass closure");
                    for(const auto& source:surface.vertices){
                        shared::need(source.bone==0,"single-pass world original static vertex bone");a::Vertex48B vertex{};
                        for(unsigned axis=0;axis<3;++axis){double value=0;
                            for(unsigned k=0;k<3;++k)value+=source.position_local[k]*matrix[k*4+axis];value+=matrix[12+axis];
                            vertex.position[axis]=static_cast<float>(value);shared::need(std::isfinite(value)&&std::isfinite(vertex.position[axis]),"single-pass world nonfinite original vertex");
                        }
                        for(unsigned axis=0;axis<2;++axis){vertex.uv[axis]=static_cast<float>(source.uv[axis]);shared::need(std::isfinite(vertex.uv[axis]),"single-pass world nonfinite original UV");}
                        for(unsigned axis=0;axis<4;++axis)vertex.color[axis]=surface.has_original_colors?source.color[axis]:255;
                        a::WorldNormal50HK normal;shared::need(a::transformPreparedWorldNormal50HK(source.normal_local,normalTransform,normal),"single-pass world original normal transform");
                        if(!normal.xyz[0]&&!normal.xyz[1]&&!normal.xyz[2])++d.zeroNormals;
                        next->mesh.vertices.push_back(vertex);next->normals.push_back(normal);
                        // Original ProjectedPrelit consumes quantized source-space XYZ,
                        // before the SCB/world transform, for each authored pass.
                        a::TrackProjectedUv uv{{vertex.uv[0],vertex.uv[1],vertex.uv[0],vertex.uv[1]}};
                        for(const auto selectedPass:selectedPasses){
                            const auto& selection=next->selections[selectedPass];
                            if(!selection.projectedPrelit)continue;
                            for(unsigned axis=0;axis<2;++axis){double value=0;
                                for(unsigned k=0;k<3;++k)value+=(source.position_local[k]-scene.decoded.quantization[4+k])*selection.texGenMatrix[k*4+axis];
                                value+=selection.texGenMatrix[12+axis];
                                shared::need(std::isfinite(value)&&std::isfinite(static_cast<float>(value)),"single-pass world projected source UV nonfinite");
                                uv.uv[(selection.pass-1)*2+axis]=static_cast<float>(value);
                            }
                        }
                        next->projectionUvs.push_back(uv);
                    }
                    for(const auto index:surface.indices){shared::need(index<surface.vertices.size(),"single-pass world original surface index");next->mesh.indices.push_back(span.vertexStart+index);}
                    // Duplicate draw spans only. Vertex/index storage stays shared.
                    for(const auto selectedPass:selectedPasses){
                        const auto& selection=next->selections[selectedPass];
                        span.materialSelection=selectedPass;span.textureIndex=selection.textureIndex;
                        span.sourcePass=selection.pass;span.sourcePassCount=selection.passCount;
                        span.projectionPass=selection.projectedPrelit?selection.pass:0;
                        next->spans.push_back(span);if(selection.pass==2)++d.secondPasses;
                    }
                    ++d.surfaces;
                }
                ++d.placements;
            }
            d.scenes=next->scenes.size();d.stage="closure";d.member.clear();
            d.materials=next->materials.size();d.selections=next->selections.size();
            // Counts are the data's own; the stock closure counts are receipts of the Track.
            const bool stockClosure=false;
            wma::closureReceipt(id.code.c_str(),stockClosure,d.materials,d.selections,next->spans.size(),images.size(),next->mesh.vertices.size());
            shared::need(wma::closureBounds(d.scenes,d.placements,d.collisionOnly,d.surfaces,world.instances.size(),
                d.materials,d.selections,d.secondPasses,next->spans.size(),next->mesh.vertices.size(),next->mesh.indices.size(),
                images.size(),d.independentlyPinnedImages,d.runtimeIdentityImages)&&next->normals.size()==next->mesh.vertices.size()&&next->projectionUvs.size()==next->mesh.vertices.size(),
                "Single-pass world full static source closure differs");
            for(unsigned axis=0;axis<3;++axis){next->mesh.bounds.minimum[axis]=std::numeric_limits<float>::infinity();
                next->mesh.bounds.maximum[axis]=-std::numeric_limits<float>::infinity();}
            for(const auto& vertex:next->mesh.vertices)for(unsigned axis=0;axis<3;++axis){
                next->mesh.bounds.minimum[axis]=std::min(next->mesh.bounds.minimum[axis],vertex.position[axis]);
                next->mesh.bounds.maximum[axis]=std::max(next->mesh.bounds.maximum[axis],vertex.position[axis]);}
            std::vector<a::MaterialTextureAssetInput50EA> textureInput;
            for(std::size_t i=0;i<images.size();++i)textureInput.push_back({next->textures[i].identity,&images[i]});
            a::MaterialTextureAssetsInput50EA input;input.textures=textureInput.data();input.textureCount=static_cast<std::uint32_t>(textureInput.size());
            input.slots=slots.data();input.slotCount=static_cast<std::uint32_t>(slots.size());
            input.budgets=budgets;
            d.stage="pixel_bank";shared::need(next->bank.load(input,d.bank),"Single-pass world pixel bank rejected: "+d.bank.parserMessage);
            storage_=std::move(next);d.stage="complete";return true;
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Single-pass world native cold preparation failed";}catch(...){}return false;}
    }
} // namespace single_pass_world50LM
