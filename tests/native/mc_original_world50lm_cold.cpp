// Exact existing non-template cold body; namespace/private ownership unchanged.
#include "mc_original_world50lm.h"

namespace mc_original_world50LM {
bool Owner::load(std::shared_ptr<const mc_cold_sources50LA::Owner> sourceOwner,Diagnostic& d)noexcept{
        try{
            d={};shared::need(!storage_&&std::fegetround()==FE_TONEAREST,"MC direct cold owner already loaded or rounding mode differs");
            shared::need(sourceOwner&&sourceOwner->ready(),"Original MC shared cold source absent");
            auto next=std::make_unique<Storage>();next->source=std::move(sourceOwner);d.stage="archive";
            const auto& archive=next->source->archive();const auto& world=next->source->world();
            shared::need(archive.entries().size()==entryCount&&world.instances.size()==expectedInstances&&
                world.zones.size()==expectedZones&&world.portals.size()==expectedPortals&&
                world.parameters.original_bytes.size()==scbBytesCount&&shared::sha256(world.parameters.original_bytes)==scbSha,
                "Original MC exact shared source/world identity");
            for(std::size_t i=0;i<archive.entries().size();++i)
                shared::need(next->entries.emplace(detail::key(archive.entries()[i].name),i).second,"Ambiguous original MC member");
            std::map<std::string,std::uint32_t> materialIndices,textureIndices;
            std::vector<Bytes> images;
            std::vector<a::MaterialTextureSlotInput50EA> slots;
            constexpr std::uint64_t textureBase=assetIdentity<<48;
            std::size_t rgbaBytes=0,serializedBytes=0;
            auto material=[&](const std::string& materialName)->std::uint32_t{
                const auto normalized=detail::key(materialName);const auto existing=materialIndices.find(normalized);
                if(existing!=materialIndices.end())return existing->second;
                shared::need(shared::name(materialName),"Invalid original MC material basename");
                d.stage="material";const auto* psm=next->entry("textures\\"+materialName+".psm");
                shared::need(psm,"Original MC world material absent: "+materialName);d.member=psm->name;
                const auto source=next->member(*psm,d.decode);a::Material48E authored;
                const bool decoded=a::decodeMaterial48E(source,authored,d.decode);
                shared::need(decoded&&!authored.passes.empty()&&authored.passes.size()<=2,
                    "Original MC material/program pass count");
                const auto materialIndex=static_cast<std::uint32_t>(next->materials.size());
                shared::need(materialIndex<expectedMaterials,"Original MC PSM count bound");
                const auto materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+shared::sha256(source));
                std::vector<std::uint32_t> selectedPasses;
                for(std::size_t pi=0;pi<authored.passes.size();++pi){
                    const auto& pass=authored.passes[pi];const bool projected=pass.microprogram=="ProjectedPrelit";
                    shared::need(pass.pass==pi+1&&pass.textures.size()==1&&pass.textures[0].slot==1&&
                        ((authored.passes.size()==2&&projected)||
                         (authored.passes.size()==1&&!projected&&
                          (detail::key(pass.microprogram)=="PRELITCOMPRESSED"||pass.microprogram=="DiffuseCompressed"||(normalized=="MC_SHIELD"&&pass.microprogram=="diffuseTextransCompressed")))),
                        "Unjoined original MC pass/program/texture subset");
                    const auto& map=pass.textures[0];shared::need(shared::name(map.name),"Invalid original MC texture basename");
                    const auto pstCandidate=map.name+".pst";
                    const auto dot=pstCandidate.find_last_of('.');
                    shared::need(dot!=std::string::npos,"MC source texture candidate lacks extension");
                    const auto stem=pstCandidate.substr(0,dot);
                    const a::ResourceEntry48C* texture=nullptr;
                    for(const char* extension:{".pst",".ipu",".tga",".atm"}){
                        texture=next->entry("textures\\"+stem+extension);if(texture)break;
                    }
                    shared::need(texture,"Original MC required texture absent: "+map.name);
                    const auto textureKey=detail::key(texture->name);const auto* pin=detail::sourceImage(textureKey);
                    shared::need(pin,"Original MC static image lacks exact source pin");
                    const bool animated=textureKey.ends_with(".ATM");
                    if(animated)shared::need(authored.passes.size()==1&&detail::initialFrame(pass.fields)==pin->initialFrame,
                        "Original MC material/ATM initial frame differs");
                    auto found=textureIndices.find(textureKey);
                    if(found==textureIndices.end()){
                        d.stage="texture";d.member=texture->name;const auto bytes=next->member(*texture,d.decode);
                        std::uint32_t mip=0;bool decodedAnimated=false,independent=false;
                        auto image=detail::image(texture->name,bytes,mip,decodedAnimated,d.decode,&independent);
                        shared::need(decodedAnimated==animated,"Original MC texture animation source classification");
                        const auto index=static_cast<std::uint32_t>(images.size());
                        shared::need(index<expectedImages&&image.size()>=16,"MC original texture bank bound");
                        const auto w=shared::u32(image.data()+8),h=shared::u32(image.data()+12);
                        shared::need(w&&h&&w<=1024&&h<=1024&&image.size()==std::size_t(w)*h*4+16,"MC native image extent");
                        shared::add(rgbaBytes,std::size_t(w)*h*4,16u*1024u*1024u,"MC texture RMCA budget");
                        shared::add(serializedBytes,image.size(),16u*1024u*1024u,"MC texture framing budget");
                        tm::TextureSelection selected;selected.identity=textureBase+index+1;selected.bankIndex=index;
                        selected.mipIndex=mip;selected.width=w;selected.height=h;selected.path=texture->name;
                        selected.key="disc:"+textureKey+":"+pin->sourceSha256;
                        selected.sha256=shared::sha256(image); // Actual runtime output identity, not necessarily an independent pin.
                        next->textures.push_back(std::move(selected));images.push_back(std::move(image));
                        next->independentlyPinnedImages.push_back(independent?1:0);
                        if(independent)++d.independentlyPinnedImages;else ++d.runtimeIdentityImages;
                        found=textureIndices.emplace(textureKey,index).first;
                    }
                    const auto index=static_cast<std::uint32_t>(next->selections.size());
                    shared::need(index<expectedSelections,"Original MC authored pass count bound");
                    const auto& image=next->textures[found->second];tm::Selection selected;
                    selected.assetIdentity=assetIdentity;selected.materialIndex=materialIndex;selected.pass=pass.pass;selected.slot=1;
                    selected.bankSlot=static_cast<std::uint32_t>(slots.size());selected.textureIndex=found->second;selected.mipIndex=image.mipIndex;
                    selected.materialName=materialName;selected.materialKey=materialKey;
                    selected.program=pass.microprogram;selected.textureName=map.name;selected.textureKey=image.key;
                    selected.track=selectedTrack;selected.passCount=static_cast<std::uint32_t>(authored.passes.size());
                    selected.clampU=detail::clamp(pass.fields,"wrapModeHorizontal");selected.clampV=detail::clamp(pass.fields,"wrapModeVertical");
                    selected.sourcePassFields=detail::fields(pass.fields);selected.staticAnimationFrame=animated;
                    selected.projectedPrelit=projected;
                    if(projected)selected.texGenMatrix=tm::detail::projection(selected.sourcePassFields);
                    selected.decodedSourceMember=texture->name;selected.decodedSourceSha256=pin->sourceSha256;
                    slots.push_back({{selected.assetIdentity,materialIndex,pass.pass,1},image.identity,a::MaterialTextureRequirement50EA::required});
                    selectedPasses.push_back(index);next->selections.push_back(std::move(selected));
                }
                next->materials.push_back(std::move(authored));next->materialPasses.push_back(std::move(selectedPasses));
                materialIndices.emplace(normalized,materialIndex);return materialIndex;
            };
            constexpr std::array<double,16> identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
            for(std::size_t wi=0;wi<world.instances.size();++wi){
                const auto& placement=world.instances[wi];d.stage="scene";d.member=placement.geometry_member;
                const auto* entry=next->entry(placement.geometry_member);
                if(!entry){shared::need(next->entry(placement.collision_member),"MC placement lacks both original PSG and COL");++d.collisionOnly;continue;}
                const auto name=detail::key(entry->name);auto found=next->scenes.find(name);
                if(found==next->scenes.end()){
                    Scene scene;scene.identity=next->scenes.size()+1;
                    const auto originalScene=next->member(*entry,d.decode);
                    a::SceneLimits48D sceneLimits;sceneLimits.allow_signed32_quantized_positions=true;
                    shared::need(a::decodeScene48D(originalScene,scene.decoded,d.decode,nullptr,sceneLimits),
                        "Original MC PSG decode: "+entry->name+" "+d.decode.code);
                    shared::need(scene.decoded.nodes.size()==1&&scene.decoded.nodes[0].parent==-1&&
                        !scene.decoded.identity_matrix_palette&&!scene.decoded.lods.empty(),"Original MC static hierarchy subset");
                    for(const auto& name:scene.decoded.materials)scene.materials.push_back(material(name));
                    found=next->scenes.emplace(name,std::move(scene)).first;
                }
                const auto& scene=found->second;const auto& matrix=scene.decoded.world_space?identity:placement.matrix;
                a::WorldNormalTransform50HK normalTransform;
                shared::need(a::prepareWorldNormalTransform50HK(matrix,normalTransform),"Original MC visual/normal transform");
                for(std::size_t si=0;si<scene.decoded.lods.front().surfaces.size();++si){
                    const auto& surface=scene.decoded.lods.front().surfaces[si];d.stage="surface";d.member=entry->name;
                    shared::need(surface.material_index<scene.materials.size()&&!surface.vertices.empty()&&!surface.indices.empty()&&
                        !(surface.indices.size()%3)&&surface.vertices.size()<=a::max_mesh_vertices-next->mesh.vertices.size()&&
                        surface.indices.size()<=a::max_mesh_indices-next->mesh.indices.size(),"MC original surface/material/geometry bounds");
                    tm::Span span;span.vertexStart=static_cast<std::uint32_t>(next->mesh.vertices.size());
                    span.vertexCount=static_cast<std::uint32_t>(surface.vertices.size());span.indexStart=static_cast<std::uint32_t>(next->mesh.indices.size());
                    span.indexCount=static_cast<std::uint32_t>(surface.indices.size());span.worldInstanceIndex=static_cast<std::uint32_t>(wi);
                    span.surfaceIndex=static_cast<std::uint32_t>(si);span.hasOriginalColors=surface.has_original_colors;
                    const auto& selectedPasses=next->materialPasses.at(scene.materials[surface.material_index]);
                    shared::need(!selectedPasses.empty()&&selectedPasses.size()<=2,"Original MC surface material pass closure");
                    for(const auto& source:surface.vertices){
                        shared::need(source.bone==0,"MC original static vertex bone");a::Vertex48B vertex{};
                        for(unsigned axis=0;axis<3;++axis){double value=0;
                            for(unsigned k=0;k<3;++k)value+=source.position_local[k]*matrix[k*4+axis];value+=matrix[12+axis];
                            vertex.position[axis]=static_cast<float>(value);shared::need(std::isfinite(value)&&std::isfinite(vertex.position[axis]),"MC nonfinite original vertex");
                        }
                        for(unsigned axis=0;axis<2;++axis){vertex.uv[axis]=static_cast<float>(source.uv[axis]);shared::need(std::isfinite(vertex.uv[axis]),"MC nonfinite original UV");}
                        for(unsigned axis=0;axis<4;++axis)vertex.color[axis]=surface.has_original_colors?source.color[axis]:255;
                        a::WorldNormal50HK normal;shared::need(a::transformPreparedWorldNormal50HK(source.normal_local,normalTransform,normal),"MC original normal transform");
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
                                shared::need(std::isfinite(value)&&std::isfinite(static_cast<float>(value)),"MC projected source UV nonfinite");
                                uv.uv[(selection.pass-1)*2+axis]=static_cast<float>(value);
                            }
                        }
                        next->projectionUvs.push_back(uv);

                    }
                    for(const auto index:surface.indices){shared::need(index<surface.vertices.size(),"MC original surface index");next->mesh.indices.push_back(span.vertexStart+index);}
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
            shared::need(d.scenes==expectedScenes&&d.placements==expectedPlacements&&d.collisionOnly==expectedCollisionOnly&&d.surfaces==expectedSurfaces&&
                d.materials==expectedMaterials&&d.selections==expectedSelections&&d.secondPasses==expectedSecondPasses&&next->spans.size()==expectedSurfaces+expectedSecondPasses&&
                next->mesh.vertices.size()==expectedVertices&&next->mesh.indices.size()==expectedIndices&&images.size()==expectedImages&&
                next->normals.size()==next->mesh.vertices.size()&&
                next->projectionUvs.size()==next->mesh.vertices.size()&&rgbaBytes==expectedRgbaBytes&&d.independentlyPinnedImages==expectedPinnedImages&&d.runtimeIdentityImages==expectedImages-expectedPinnedImages,
                "Original MC full static source closure differs");
            for(unsigned axis=0;axis<3;++axis){next->mesh.bounds.minimum[axis]=std::numeric_limits<float>::infinity();
                next->mesh.bounds.maximum[axis]=-std::numeric_limits<float>::infinity();}
            for(const auto& vertex:next->mesh.vertices)for(unsigned axis=0;axis<3;++axis){
                next->mesh.bounds.minimum[axis]=std::min(next->mesh.bounds.minimum[axis],vertex.position[axis]);
                next->mesh.bounds.maximum[axis]=std::max(next->mesh.bounds.maximum[axis],vertex.position[axis]);}
            std::vector<a::MaterialTextureAssetInput50EA> textureInput;
            for(std::size_t i=0;i<images.size();++i)textureInput.push_back({next->textures[i].identity,&images[i]});
            a::MaterialTextureAssetsInput50EA input;input.textures=textureInput.data();input.textureCount=static_cast<std::uint32_t>(textureInput.size());
            input.slots=slots.data();input.slotCount=static_cast<std::uint32_t>(slots.size());
            input.budgets={expectedImages,expectedSelections,16u*1024u*1024u,16u*1024u*1024u,1024};
            d.stage="pixel_bank";shared::need(next->bank.load(input,d.bank),"Original MC pixel bank rejected: "+d.bank.parserMessage);
            storage_=std::move(next);d.stage="complete";return true;
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Original MC native cold preparation failed";}catch(...){}return false;}
    }
} // namespace mc_original_world50LM
