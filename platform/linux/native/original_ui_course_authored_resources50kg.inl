// Private native input admission for the actual retained Course Quad occurrence.
// Parsing/gathering is not14D370,29B550,29E340 or a renderer completion receipt.
    bool prepareCourseAuthoredWidgetSources(CourseWidgetQuad& state,std::size_t ordinal,std::size_t slot,
        const std::string& requested,std::int32_t row,
        const assets::OriginalStandardArchiveProvider50LA::MemberView& psm,
        std::vector<std::shared_ptr<const assets::OriginalStandardMaterialSource50LA>>& sources,
        std::vector<assets::OriginalStandardTextureSourceInput50LA>& textures,Diagnostic& d)noexcept {
        using Provider=assets::OriginalStandardArchiveProvider50LA;
        const auto fail=[&](assets::OriginalStandardMaterialStatus50LA status)noexcept {
            d.page.quad.material.status=status;
            d.sourcePC=0x2DF144; // Native corpus admission before actual whole-set population.
            return false;
        };
        if(thread_!=std::this_thread::get_id()||courseWidgetQuadOccurrence(ordinal,slot)!=&state||
           !courseQuadTexture(ordinal,slot)||!state.occurrence||!state.backing||!state.constructed||
           !state.loadAttempted||state.retired||!courseWidgetQuadInputsCurrent())
            return fail(assets::OriginalStandardMaterialStatus50LA::phase);
        auto& q=*state.backing;
        const auto domain=issuer_->standardMaterialDomainOwner_;
        const auto provider=domain->archiveProvider3E1A70();
        const auto table=domain->materials382B38();
        const auto archive=loaderConfig_->archive_;
        if(!q.material54||q.material54!=q.primitiveOwner_.get()||!q.material54->materialRetained||
           !q.material54->materialStored||q.material54->material00!=row||q.domain_!=domain||
           q.context_!=issuer_->materialContextOwner_||!provider||!provider->sealed()||
           !archive||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course)||
           !sources.empty()||!textures.empty())return fail(assets::OriginalStandardMaterialStatus50LA::source);
        assets::OriginalStandardMaterialRowView50LA retained;
        if(!table->row(row,retained)||!retained.name00||!retained.references08||*retained.references08<=0||
           retained.object04||!Provider::sameName(*retained.name00,requested))
            return fail(assets::OriginalStandardMaterialStatus50LA::phase);
        try {
            const auto revision=provider->revision();
            const std::string expected=std::string("ui\\textures\\")+courseQuadTexture(ordinal,slot);
            if(!Provider::sameName(requested,expected))return fail(assets::OriginalStandardMaterialStatus50LA::source);
            const auto authored=archive->find(expected+".psm");
            Provider::MemberView current;
            if(!authored.identity||!authored.data||!psm.member||!psm.data||
               !Provider::sameName(*psm.member,expected+".psm")||psm.revision!=revision||
               psm.size!=authored.size||std::memcmp(psm.data,authored.data,authored.size)||
               provider->lookup132820(expected+".psm",current)!=Provider::Lookup::present||
               current.mount!=psm.mount||current.ordinal!=psm.ordinal||current.bankGeneration!=psm.bankGeneration||
               current.data!=psm.data||current.size!=psm.size)return fail(assets::OriginalStandardMaterialStatus50LA::source);
            auto source=std::make_shared<assets::OriginalStandardMaterialSource50LA>();
            if(!source->prepare(*psm.member,std::vector<std::uint8_t>(psm.data,psm.data+psm.size),d.page.quad.material.document))
                return fail(assets::OriginalStandardMaterialStatus50LA::source);
            const auto* general=source->section("General");std::int32_t passes=0;
            if(!general||!assets::originalUiInteger50KG(*general,"TotalShaderPasses",0,passes,d.page.quad.material.document,0x29B718)||
               passes!=1)return fail(assets::OriginalStandardMaterialStatus50LA::source);
            const auto* pass=source->section("ShaderPass_1");std::int32_t maps=1;
            if(!pass||!assets::originalUiInteger50KG(*pass,"totalTextureMaps",1,maps,d.page.quad.material.document,0x247214)||
               maps!=1)return fail(assets::OriginalStandardMaterialStatus50LA::source);
            const auto* map=pass->find("TextureMap_1");
            std::string image=map?map->text:std::string{};
            if(image.empty())image=courseQuadTexture(ordinal,slot); // Source13DA50 material basename fallback.
            if(image.empty()||image.size()>255||image.find_first_of("\\/.:\"")!=std::string::npos)
                return fail(assets::OriginalStandardMaterialStatus50LA::source);
            const auto slash=source->member()->find_last_of('\\');
            const std::string prefix=slash==std::string::npos?std::string{}:source->member()->substr(0,slash+1);
            const auto textureName=prefix+image;
            if(textureName.size()>255)return fail(assets::OriginalStandardMaterialStatus50LA::source);
            // Same positive resolver priority24DADC, not TextureAnimation's mode
            // and not an assumption that every unique controller owns an ATM.
            std::vector<assets::OriginalStandardTextureSourceInput50LA> selectedInputs;
            for(const char* extension:{".pst",".ipu",".tga",".atm"}){
                Provider::MemberView selected;
                const auto lookup=provider->lookup132820(textureName+extension,selected);
                if(lookup==Provider::Lookup::unknown)return fail(assets::OriginalStandardMaterialStatus50LA::source);
                if(lookup==Provider::Lookup::absent)continue;
                // Keep actualIPU as the winning unsupported source. Do not seed
                // a lower-priority substitute. Actual29E340 will stop24DB60,
                // AFTER reached material population, with its normal custody.
                if(!std::strcmp(extension,".ipu"))break;
                assets::OriginalUiEncodedArchiveView50KG encoded;Provider::MountView mount;
                if(!selected.member||!selected.data||!selected.size||selected.revision!=revision||
                   !provider->encodedUiInput(selected.mount,encoded)||encoded.providerIdentity()!=provider.get()||
                   !encoded.identity()||!provider->mount(selected.mount,mount)||!mount.archiveSha256||
                   *mount.archiveSha256!=encoded.identity()->sha256||mount.archiveBytes!=encoded.identity()->bytes)
                    return fail(assets::OriginalStandardMaterialStatus50LA::source);
                const auto* identity=assets::originalUiMemberIdentity50KG(encoded.identity()->id,selected.ordinal);
                if(!identity||identity->bytes!=selected.size||!Provider::sameName(identity->name,*selected.member))
                    return fail(assets::OriginalStandardMaterialStatus50LA::source);
                selectedInputs.push_back({*selected.member,std::vector<std::uint8_t>(selected.data,selected.data+selected.size)});
                break; // Positive winner chosen; never append competing extensions.
            }
            // All-absent remains actual resolver failure during29E340; it does
            // not become an absent-PSM fallback or a fabricated texture owner.
            if(provider->revision()!=revision||!provider->sealed())return fail(assets::OriginalStandardMaterialStatus50LA::phase);
            std::shared_ptr<assets::OriginalCourseScreenAuthored50KG> proof(new assets::OriginalCourseScreenAuthored50KG);
            proof->occurrence_=state.occurrence;proof->row_=row;proof->ordinal_=ordinal;proof->slot_=slot;
            proof->source_=source;proof->course_=archive;proof->provider_=provider;
            proof->context_=q.context_;proof->textures_=domain->textures383400();
            sources.push_back(source);textures=std::move(selectedInputs);
            if(!table->bindCourseScreenAuthoredAfterRetain(row,proof,d.page.quad.material)){
                d.sourcePC=d.page.quad.material.sourcePC?d.page.quad.material.sourcePC:0x2DF144;return false;
            }
            return true; // Only native input admission; caller performs actual population.
        }catch(...){return fail(assets::OriginalStandardMaterialStatus50LA::allocation);}
    }
