// Included privately in the actual aggregate-owned world. CPU inventory only.
public:
    enum class StaticProgram : std::uint8_t {prelitCompressed,diffuseCompressed};
    struct RasterInventory {
        std::size_t family=0,firstImage=0,imageCount=0;
        std::int32_t materialRow=-1;
        std::int16_t programToken=-1,group=-1;
        std::uint8_t passFlags=0;
        std::array<std::uint8_t,4> fillRgba{};
        StaticProgram program=StaticProgram::prelitCompressed;
        StaticFragmentDemands fragmentDemands;
        a::OriginalStandardMaterialCollectorEntryView50LA collector;
        // Exact final canonical fields with original provenance. No lossy
        // replacement by a blend enum and no inferred host raster defaults.
        a::OriginalStandardMaterialStateContext50LA finalContext;
        bool dynamicCrowdMap=false;
        bool requiresNativeStaticLighting=false;
        bool requiresNativeCoverage=false;
        std::size_t surfaceCount=0;
    };
    struct SurfaceDrawPlan {
        std::size_t surface=0,placement=0,worldInstance=0,raster=0;
        std::uint32_t sourceLod=0,sourceSurface=0,materialSlot=0;
        std::uint32_t vertexStart=0,vertexCount=0,indexStart=0,indexCount=0;
        bool hasOriginalColors=false,worldSpaceRootBypassed=false,authoredUseLighting=false;
        // Actual vertex/normal/UV streams and transforms remain in the same
        // owned source Surface/Placement. No duplicate or fabricated stream.
    };
    struct DrawPlanRequirements {
        std::size_t surfaces=0,placements=0,staticLightingSurfaces=0,coverageSurfaces=0,dynamicMapSurfaces=0;
        std::size_t staticLightingVertices=0;
        // These are unresolved implementation requirements, not caller-set
        // readiness. They deliberately do not authorize renderer publication.
    };
private:
    std::vector<RasterInventory> rasterInventory_;
    std::vector<SurfaceDrawPlan> surfaceDrawPlan_;
    DrawPlanRequirements drawRequirements_{};
    bool drawPlanAttempted_=false,drawPlanComplete_=false,drawPlanTerminal_=false;
    bool prepareDrawPlan(Diagnostic& d)noexcept {
        d={};
        if(drawPlanAttempted_||closed_||imagePublication_||rendererPublication_||modelServiceTerminal_||!coldImagesCurrent())
            return reject(d,"RA draw inventory requires unpublished current cold world");
        drawPlanAttempted_=true;
        auto failed=[&](const char* why){drawPlanTerminal_=true;return reject(d,why);};
        try {
            if(placements_.size()!=1085||imageFamilies_.size()!=44||coldImages_.size()!=49||surfaces_.empty())
                return failed("complete RA source extent required for draw inventory");
            const auto& table=domain_->materials382B38();
            rasterInventory_.reserve(44);surfaceDrawPlan_.reserve(surfaces_.size());
            for(std::size_t fi=0;fi<44;++fi){const auto& family=imageFamilies_[fi];
                RasterInventory row;row.family=fi;row.materialRow=family.materialRow;
                row.firstImage=family.firstImage;row.imageCount=family.imageCount;row.dynamicCrowdMap=family.animatedCurrentMapPending;
                const a::OriginalStandardMaterialObject50LA* material=nullptr;
                a::OriginalStandardMaterialPassView50LA pass;
                if(!table->resolve298E60(row.materialRow,material,d.material)||!material||
                   !material->source()||!material->source()->originalBytes()||
                   *material->source()->originalBytes()!=*psms_.at(normalized(family.materialMember))->originalBytes()||
                   !material->passCount48()||*material->passCount48()!=1||!material->pass4C(0,pass)||
                   !pass.program18||!pass.group22||!pass.flags21||!pass.fillRgba2C||
                   !pass.mapCount20||*pass.mapCount20!=1||!pass.texture1A||(*pass.texture1A)[0]!=family.textureToken||
                   pass.constructionTemplate!=context_->materialApplication()->templates().get(a::OriginalUiTemplate50KG::defaultMt)||
                   !table->coldCollectorPass(imageCollector_,row.materialRow,0,row.collector)||
                   row.collector.registration!=family.collector.registration||row.collector.finalLeaf!=family.collector.finalLeaf)
                    return failed("actual raster source/pass/collector differs");
                if(family.authored.passes.size()!=1)return failed("actual static pass extent differs");
                const auto program=normalized(family.authored.passes[0].microprogram);
                std::int16_t expected=-1;
                if(!domain_->programs382B70()->find298C80(family.authored.passes[0].microprogram,expected,d.material.program)||expected!=*pass.program18)
                    return failed("actual static program token differs");
                if(program=="prelitcompressed")row.program=StaticProgram::prelitCompressed;
                else if(program=="diffusecompressed"&&normalized(family.materialMember)=="textures/black.psm"){
                    row.program=StaticProgram::diffuseCompressed;row.requiresNativeStaticLighting=true;
                }else return failed("unsupported actual static program retained; full draw plan rejected");
                row.programToken=*pass.program18;row.group=*pass.group22;row.passFlags=*pass.flags21;row.fillRgba=*pass.fillRgba2C;
                a::OriginalStandardMaterialMapBindingView50LA map;
                if(row.dynamicCrowdMap){bool found=false;
                    for(std::size_t si=0;si<surfaces_.size();++si)if(surfaces_[si].materialRow==row.materialRow){
                        ModelCrowdMap current;
                        if(!currentCrowdSurfaceMap24B26C(si,current,d))return failed("actual crowd map proof rejected");
                        map=current.binding;found=true;break;
                    }
                    if(!found||row.imageCount!=6)return failed("actual crowd placement/frame extent missing");
                }else if(row.imageCount!=1||!table->prepareColdRepresentativeMap249610(imageCollector_,row.materialRow,0,0,map,d.material)||
                         map.preparation!=a::OriginalStandardMaterialMapPreparation50LA::ready||
                         family.firstImage>=coldImages_.size()||map.texture!=coldImages_[family.firstImage].canonical)
                    return failed("actual ordinary static map rejected");
                if(!decodeStaticMaterialProfile(fi,map,row.fragmentDemands,d))
                    return failed("actual RA static profile rejected");
                row.finalContext=map.contextAfter249610;
                using F=a::OriginalUiGsField50KG;
                const auto coverage=row.finalContext.fields[static_cast<std::size_t>(F::onePassAntiAliasingOnOff)].value;
                if(coverage!=0&&coverage!=1)return failed("unsupported actual AA field value");
                row.requiresNativeCoverage=coverage!=0;
                rasterInventory_.push_back(std::move(row));
            }
            std::vector<bool> seenPlacement(placements_.size(),false);
            for(std::size_t si=0;si<surfaces_.size();++si){const auto& span=surfaces_[si];
                if(span.placement>=placements_.size()||!placements_[span.placement])return failed("draw surface has no owned placement");
                const auto& placement=*placements_[span.placement];const auto* scene=placement.payload?placement.payload->scene():nullptr;
                if(!scene||span.sourceLod>=scene->lods.size()||placement.lodRanges.size()!=scene->lods.size()||
                   span.sourceSurface>=scene->lods[span.sourceLod].surfaces.size()||
                   span.worldInstance!=placement.worldInstance||si<placement.firstSurface||si-placement.firstSurface>=placement.surfaceCount||
                   !placement.materials.complete()||placement.materials.table382B38()!=table.get())return failed("draw source surface/placement proof differs");
                const auto& range=placement.lodRanges[span.sourceLod];
                if(range.firstSurface<placement.firstSurface||range.firstSurface>surfaces_.size()||
                   range.surfaceCount>surfaces_.size()-range.firstSurface||
                   range.surfaceCount!=scene->lods[span.sourceLod].surfaces.size()||
                   si<range.firstSurface||si-range.firstSurface!=span.sourceSurface)
                    return failed("draw authentic LOD range differs");
                const auto& original=scene->lods[span.sourceLod].surfaces[span.sourceSurface];const auto* token=placement.materials.handle1C4(original.material_index);
                if(original.material_index!=span.materialSlot||!token||*token!=span.materialRow||
                   original.vertices.size()!=span.vertexCount||original.indices.size()!=span.indexCount||
                   span.vertexStart>vertices_.size()||span.vertexCount>vertices_.size()-span.vertexStart||
                   span.indexStart>indices_.size()||span.indexCount>indices_.size()-span.indexStart||!span.indexCount||span.indexCount%3)
                    return failed("draw source slot/range differs");
                std::size_t raster=rasterInventory_.size();
                for(std::size_t i=0;i<rasterInventory_.size();++i)if(rasterInventory_[i].materialRow==*token){
                    if(raster!=rasterInventory_.size())return failed("ambiguous canonical raster row");raster=i;
                }
                if(raster==rasterInventory_.size())return failed("surface omitted from actual full raster inventory");
                SurfaceDrawPlan record;record.surface=si;record.placement=span.placement;record.worldInstance=span.worldInstance;record.raster=raster;
                record.sourceLod=span.sourceLod;record.sourceSurface=span.sourceSurface;record.materialSlot=span.materialSlot;
                record.vertexStart=span.vertexStart;record.vertexCount=span.vertexCount;record.indexStart=span.indexStart;record.indexCount=span.indexCount;
                record.hasOriginalColors=span.hasOriginalColors;record.worldSpaceRootBypassed=span.worldSpaceRootBypassed;record.authoredUseLighting=placement.sourceUseLighting;
                surfaceDrawPlan_.push_back(record);seenPlacement[span.placement]=true;
                auto& policy=rasterInventory_[raster];++policy.surfaceCount;
                if(policy.requiresNativeStaticLighting){
                    if(drawRequirements_.staticLightingVertices>vertices_.size()||
                       record.vertexCount>vertices_.size()-drawRequirements_.staticLightingVertices)
                        return failed("lit source vertex total exceeds retained world geometry");
                    drawRequirements_.staticLightingVertices+=record.vertexCount;
                    ++drawRequirements_.staticLightingSurfaces;
                }
                drawRequirements_.coverageSurfaces+=policy.requiresNativeCoverage?1:0;
                drawRequirements_.dynamicMapSurfaces+=policy.dynamicCrowdMap?1:0;
            }
            for(bool seen:seenPlacement)if(!seen)return failed("actual visual placement omitted from draw plan");
            for(const auto& row:rasterInventory_)if(!row.surfaceCount)return failed("actual raster family omitted from draw plan");
            if(!coldImagesCurrent())return failed("canonical collector changed during draw inventory");
            drawRequirements_.surfaces=surfaceDrawPlan_.size();drawRequirements_.placements=placements_.size();
            drawPlanComplete_=true;return true;
        }catch(...){return failed("terminal draw inventory allocation failure; partial records retained");}
    }
public:
    bool drawPlanCurrent()const noexcept{return drawPlanComplete_&&!drawPlanTerminal_&&!modelServiceTerminal_&&coldImagesCurrent();}
    const std::vector<RasterInventory>* rasterInventory()const noexcept{return drawPlanCurrent()?&rasterInventory_:nullptr;}
    const std::vector<SurfaceDrawPlan>* surfaceDrawPlan()const noexcept{return drawPlanCurrent()?&surfaceDrawPlan_:nullptr;}
    const DrawPlanRequirements* drawRequirements()const noexcept{return drawPlanCurrent()?&drawRequirements_:nullptr;}
private:
