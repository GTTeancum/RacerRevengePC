// Implementation-only, included inside original_main_entry50KG after Impl.
// Native lowering of the actual retained F8 ScreenTexture/Text objects. The
// backing types are shared representations, never a Pause instance/receipt.
bool NativeF8BarPrefix50LA::initializeSuffixImages(D& d)noexcept {
    auto& n=*impl_;auto& p=*n.source->impl_;
    if(n.suffix.images||!p.textures40||!p.texts100||!(*p.texts100)[0].font4C)return reject(d,0x1995b8,"F8 actual icon/text storage");
    try {
        std::array<rr::assets::OriginalUiMemberView50KG,3> members{};
        // Always reacquire the actual current FINAL maps and historical first
        // image owners. Cache hits skip decode only, never these source joins.
        for(std::size_t i=0;i<3;++i){
            const auto reference=i<2?4+i:6;
            rr::assets::OriginalStandardMaterialMapBindingView50LA map;
            rr::assets::OriginalStandardMaterialDiagnostic50LA md;
            if(!p.references[reference].retained||
               !n.table->prepareColdScreenPrototypeMap249610(n.receipt,p.references[reference].row,0,0,map,md)||
               map.preparation!=rr::assets::OriginalStandardMaterialMapPreparation50LA::ready||
               !map.texture||!map.representative.material)
                return reject(d,md.sourcePC?md.sourcePC:0x249610,"F8 actual FINAL icon/font map");
            const auto* actual=i<2?(*p.textures40)[i].atlas:(*p.texts100)[0].font4C->texture281F90();
            if(map.texture!=actual||(i==2&&(!(*p.texts100)[1].font4C||
               (*p.texts100)[1].font4C->texture281F90()!=actual)))
                return reject(d,0x281f90,"F8 canonical FINAL and actual source atlas differ");
            auto& member=members[i];
            if(!map.representative.material->borrowF8ScreenFallbackTexture2475C0(*map.texture,n.suffixArchives[i],member,md))
                return reject(d,md.sourcePC?md.sourcePC:0x2475c0,"F8 retained first-construction image provenance");
            std::size_t offset=0,bytes=0;
            if(!map.texture->pstExtent24E880(offset,bytes)||offset||bytes!=member.size)
                return reject(d,0x24e880,"F8 supported actual standalone PST extent");
            n.suffixTextures[i]=actual;
        }
        const auto sameOwner=[](const auto& a,const auto& b)noexcept {
            return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
        };
        if(p.nativeImageCache){
            const auto& cached=*p.nativeImageCache;
            bool same=cached.source==n.source&&cached.generation==p.component.sessionGeneration&&
                sameOwner(cached.race.lock(),n.race)&&sameOwner(cached.domain.lock(),p.domain)&&
                sameOwner(cached.context.lock(),p.context)&&cached.images&&cached.images->loaded();
            for(std::size_t i=0;same&&i<3;++i)same=cached.textures[i]==n.suffixTextures[i]&&
                sameOwner(cached.archives[i],n.suffixArchives[i])&&cached.members[i]==members[i].identity&&
                cached.extents[i]==members[i].size;
            if(same){n.suffix.images=cached.images;return true;}
            // A changed source key is not a stale-pixel hit. Rebuild the native
            // immutable bank below; the renderer separately retires its prior
            // uploaded bank before replacement. No canonical owner is released.
        }
        std::vector<rr::assets::OriginalUiImageSource50KG> images;images.reserve(3);
        for(std::size_t i=0;i<3;++i)images.push_back({i,members[i],0,members[i].size,{}});
        auto bank=std::make_shared<rr::assets::OriginalUiScreenAssets50KG>();
        rr::assets::OriginalUiScreenAssetDiagnostic50KG image;
        if(!bank->load(images,image))return reject(d,0x24e880,"F8 exact retained icon/font PST decode");
        auto cached=std::shared_ptr<NativeF8ImageCache50LA>(new NativeF8ImageCache50LA);
        cached->race=n.race;cached->domain=p.domain;cached->context=p.context;
        cached->source=n.source;cached->generation=p.component.sessionGeneration;
        cached->textures=n.suffixTextures;cached->archives=n.suffixArchives;cached->images=bank;
        for(std::size_t i=0;i<3;++i){cached->members[i]=members[i].identity;cached->extents[i]=members[i].size;}
        p.nativeImageCache=std::move(cached);n.suffix.images=std::move(bank);return true;
    }catch(...){return reject(d,0x24e880,"F8 private immutable image bank allocation");}
}
bool NativeF8BarPrefix50LA::appendSuffixMaterial(std::size_t reference,std::uint32_t image,
    std::uint32_t first,std::uint32_t count,D& d)noexcept {
    auto& n=*impl_;auto& p=*n.source->impl_;
    if(reference<4||reference>=p.references.size()||!p.references[reference].retained||image>=3||
       !count||first>maximumSuffixQuads||count>maximumSuffixQuads-first||
       n.suffix.packetCount>=maximumSuffixPackets||!n.suffix.images||!policiesCurrent())
        return reject(d,0x246760,"F8 bounded actual suffix primitive and registry");
    rr::assets::OriginalStandardMaterialDiagnostic50LA md;
    // Exactly once for this reached primitive, AFTER its CPU geometry. No
    // caller-generated receipt and no source queue/249610 commit is supplied.
    if(!n.table->serviceRetainedScreenPrototype24A740(n.receipt,p.references[reference].row,0,md)){
        d.sourceEffectsCommitted|=md.sourceEffectsCommitted;
        return reject(d,md.sourcePC?md.sourcePC:0x24a740,"F8 originating suffix material service incomplete");
    }
    d.sourceEffectsCommitted|=md.sourceEffectsCommitted;
    const auto ordinal=n.suffix.packetCount;
    auto& map=n.suffixMaps[ordinal];auto& proof=n.suffixProofs[ordinal];
    if(!readProof(n.domain,n.receipt,p.references[reference].row,proof,md,1)||
       !n.table->prepareColdScreenPrototypeMap249610(n.receipt,proof.row,0,0,map,md)||
       map.preparation!=rr::assets::OriginalStandardMaterialMapPreparation50LA::ready||
       map.texture!=n.suffixTextures[image]||!map.representative.material)
        return reject(d,md.sourcePC?md.sourcePC:0x249610,"F8 suffix original/FINAL current one-map identity");
    rr::assets::OriginalUiMaterial50KG parsed;
    auto& packet=n.suffix.packets[ordinal];
    if(!map.representative.material->prepareF8ScreenFallback2475C0(*n.table,n.receipt,map,
        *n.suffix.images,image,parsed,packet.policy,md))
        return reject(d,md.sourcePC?md.sourcePC:0x2475c0,"F8 FINAL effective icon/font native material policy");
    if(packet.policy.blend==rr::assets::OriginalUiMaterialBlend50KG::destinationAlphaAdditive)
        return reject(d,0x249610,"F8 inherited native gameplay alpha is not genuine GS destination alpha");
    packet.first=first;packet.count=count;packet.image=image;++n.suffix.packetCount;return true;
}
bool NativeF8BarPrefix50LA::prepareIcon(std::size_t object,D& d)noexcept {
    auto& n=*impl_;auto& p=*n.source->impl_;d.object=0x40+0x60*object;
    if(object>=2||!p.textures40)return reject(d,0x2df550,"F8 actual ScreenTexture");
    auto& t=(*p.textures40)[object];const auto reference=4+object;
    if(!t.primitive54||!t.buffer58||!t.atlas||t.primitive54->reference!=reference||
       t.primitive54->packet10!=t.buffer58.get()||t.field30!=1.f||t.field3C!=0.f||
       (t.flags5C&2u)||t.field40!=1.f||!std::isfinite(t.x10)||!std::isfinite(t.y14)||
       !std::isfinite(t.width34)||!std::isfinite(t.height38)||n.suffix.quads>=maximumSuffixQuads)
        return reject(d,0x2df674,"F8 bounded unrotated actual texture packet");
    auto packet=*t.buffer58;
    if(t.flags5C&1u){
        volatile float width=t.width34*t.field30,height=t.height38*t.field30;
        const float right=t.x10+width,bottom=t.y14+height;
        if(!std::isfinite(right)||!std::isfinite(bottom))return reject(d,0x2df694,"F8 texture binary32 bounds");
        for(std::size_t v=0;v<4;++v){packet.vertices[v].xyz[0]=(v&1)?right:t.x10;
            packet.vertices[v].xyz[1]=(v&2)?bottom:t.y14;}
    }
    for(const auto& v:packet.vertices){
        for(float f:v.xyz)if(!std::isfinite(f))return reject(d,0x2df6c0,"F8 finite texture position");
        for(float f:v.stq)if(!std::isfinite(f))return reject(d,0x2df6c0,"F8 finite texture coordinates");
        if(v.xyz[2]!=16777214.f||v.xyz[3]!=1.f||v.rgba!=std::array<std::uint32_t,4>{{127,127,127,63}})
            return reject(d,0x2df6c0,"F8 actual texture depth/W/raw RGBA");
    }
    *t.buffer58=packet;t.flags5C=std::uint8_t(t.flags5C&~1u);d.sourceEffectsCommitted=true;
    const auto first=n.suffix.quads;
    if(!appendSuffixMaterial(reference,std::uint32_t(object),first,1,d))return false;
    // Preserve the original strip diagonal using this indexed-quad reorder.
    constexpr std::size_t order[]{1,0,2,3};
    for(std::size_t i=0;i<4;++i){const auto& in=packet.vertices[order[i]];auto& out=n.suffix.vertices[4*first+i];
        for(std::size_t k=0;k<3;++k)out.position[k]=in.xyz[k];
        for(std::size_t k=0;k<4;++k)out.color[k]=std::uint8_t(in.rgba[k]);
        out.uv[0]=in.stq[0];out.uv[1]=in.stq[1];}
    ++n.suffix.quads;return true;
}
bool NativeF8BarPrefix50LA::setCrossText(std::string_view source,D& d)noexcept {
    auto& p=*impl_->source->impl_;d.object=0x100;
    if(!p.texts100)return reject(d,0x199608,"F8 actual cross text");
    auto& t=(*p.texts100)[0];
    if(!t.string8||t.capacity5B!=40||t.length5C>40||t.string8[t.length5C]||
       source.find('\0')!=std::string_view::npos)
        return reject(d,0x199608,"F8 exact bounded selected byte string");
    // Compare the FULL source strlen before capping. A source longer than40
    // re-dirties on every reached199500, exactly as the original byte length.
    if(source.size()==t.length5C&&!std::memcmp(t.string8.get(),source.data(),source.size()))return true;
    t.length5C=std::uint8_t(std::min<std::size_t>(source.size(),t.capacity5B));
    std::memcpy(t.string8.get(),source.data(),t.length5C);t.string8[t.length5C]=0;
    t.flags5A=std::uint8_t(t.flags5A|1u); //199664/1996FC
    t.flags5A=std::uint8_t(t.flags5A|2u); //199678/19970C
    d.sourceEffectsCommitted=true;return true;
}
bool NativeF8BarPrefix50LA::prepareText(std::size_t object,D& d)noexcept {
    auto& n=*impl_;auto& p=*n.source->impl_;d.object=0x100+0x70*object;
    if(object>=2||!p.texts100)return reject(d,0x2e0170,"F8 actual ScreenText");
    auto& t=(*p.texts100)[object];
    if(!t.font4C||!t.string8||t.length5C>40||t.capacity5B!=40||t.string8[t.length5C]||
       !t.glyphsC||!t.packets64||!t.primitives4||t.packetCount5D!=2||t.count0!=2||
       t.justification50||t.field40!=1.f||t.field44!=0.f||t.field48!=1.f||
       !std::isfinite(t.x10)||!std::isfinite(t.y14))return reject(d,0x2e0f50,"F8 actual zero-rotation/left-aligned text backing");
    const auto* atlas=t.font4C->texture281F90();
    if(!atlas||!atlas->sourceWidth14()||!atlas->sourceHeight18()||
       *atlas->sourceWidth14()<=0||*atlas->sourceHeight18()<=0)
        return reject(d,0x281f90,"F8 actual font atlas dimensions");
    for(std::size_t packet=0;packet<2;++packet){
        const auto reference=6+2*object+packet;
        if(!t.primitives4[packet]||t.primitives4[packet]->packet10!=&t.packets64[packet]||
           t.primitives4[packet]->reference!=reference||!t.packets64[packet].vertices||
           t.packets64[packet].count!=(packet?8u:72u))return reject(d,0x2e0278,"F8 exact two font packet aliases");
    }
    if(t.flags5A&3u){
        const float x=(t.flags5A&1u)?t.x10:t.x20,y=(t.flags5A&1u)?t.y14:t.y24;
        rr::game::OriginalUiTextInput50KG input{&t.font4C->metrics(),nullptr,
            reinterpret_cast<const std::uint8_t*>(t.string8.get()),t.length5C,x,y};
        rr::game::OriginalUiTextLayout50KG run;rr::game::OriginalUiTextDiagnostic50KG gd;
        if(!rr::game::layoutOriginalUiTextDimensions50LA(input,std::uint32_t(*atlas->sourceWidth14()),
            std::uint32_t(*atlas->sourceHeight18()),run,gd))return reject(d,gd.sourcePC?gd.sourcePC:0x2e0a30,"F8 actual CEL byte glyph geometry");
        for(std::size_t glyph=0;glyph<t.length5C;++glyph)for(std::size_t v=0;v<2;++v)
            if(t.glyphsC[glyph].vertices[v].packet!=&t.packets64[glyph/36].vertices[(glyph%36)*2+v])
                return reject(d,0x2e0a30,"F8 exact source glyph/packet alias");
        t.active5E=std::uint8_t((t.length5C+35u)/36u);t.x20=x;t.y24=y;
        for(std::size_t glyph=0;glyph<t.length5C;++glyph)for(std::size_t v=0;v<2;++v){
            auto& out=t.glyphsC[glyph].vertices[v];const auto& in=run.quads[glyph].vertices[v?2:0];
            out.authored00[0]=in.x;out.authored00[1]=in.y;out.authored00[2]=16777214.f;
            out.packet->stq[0]=in.u;out.packet->stq[1]=in.v;out.packet->xyz=out.authored00;
        }
        t.flags5A=std::uint8_t(t.flags5A&~7u);d.sourceEffectsCommitted=true;
    }
    if(t.active5E!=(t.length5C+35u)/36u)return reject(d,0x2e0280,"F8 actual active packet count");
    for(std::size_t packet=0;packet<t.active5E;++packet){
        const auto count=std::uint32_t(std::min<std::size_t>(36,t.length5C-36*packet));
        const auto first=n.suffix.quads;
        if(!appendSuffixMaterial(6+2*object+packet,2,first,count,d))return false;
        for(std::size_t glyph=0;glyph<count;++glyph){
            const auto& a=t.packets64[packet].vertices[2*glyph];const auto& b=t.packets64[packet].vertices[2*glyph+1];
            if(a.rgba!=b.rgba||a.xyz[2]!=16777214.f||b.xyz[2]!=16777214.f||a.xyz[3]!=1.f||b.xyz[3]!=1.f)
                return reject(d,0x2e02ac,"F8 actual text opposite-corner depth/W/color");
            for(auto c:a.rgba)if(c>255)return reject(d,0x2e02ac,"F8 raw text RGBA byte");
            for(std::size_t v=0;v<4;++v){auto& out=n.suffix.vertices[4*(first+glyph)+v];const bool right=v==1||v==2,bottom=v>=2;
                out.position[0]=right?b.xyz[0]:a.xyz[0];out.position[1]=bottom?b.xyz[1]:a.xyz[1];out.position[2]=a.xyz[2];
                out.uv[0]=right?b.stq[0]:a.stq[0];out.uv[1]=bottom?b.stq[1]:a.stq[1];
                for(std::size_t c=0;c<4;++c)out.color[c]=std::uint8_t(a.rgba[c]);
                for(float f:out.position)if(!std::isfinite(f))return reject(d,0x2e02ac,"F8 finite text position");
                for(float f:out.uv)if(!std::isfinite(f))return reject(d,0x2e02ac,"F8 finite text coordinates");
            }
        }
        n.suffix.quads+=count;
    }
    return true;
}
