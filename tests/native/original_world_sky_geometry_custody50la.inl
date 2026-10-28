namespace ra_pod_scene50LA {
bool Owner::continueNativeSkyGeometry12A8A0(const sky50gw_fixture::Owner& images,
    std::uint32_t& pc)noexcept {
    using C=original_world_sky50LA::Commands;
    pc=0x12991c;
    if(!nativeSky3F8_||!nativeSky3F8_->owner||!worldApplication_||
       worldApplication_->attachmentLock423F58()||!worldApplication_->skyPolicy36C0F0_)return false;
    auto& cube=*nativeSky3F8_->owner;
    if(cube.images!=&images||!cube.commands||!cube.commands->complete||cube.commands->terminal||
       !cube.resourceComplete||cube.resourceTerminal||!canRetireNativeSkyCommands1298AC(cube)||
       !images.storage_||!images.storage_->nativeRaProof)return false;
    auto& c=*cube.commands;const auto policy=*worldApplication_->skyPolicy36C0F0_;
    if(policy>1||worldApplication_->skySubdivisionsX36C0D8_!=3||
       worldApplication_->skySubdivisionsY36C0E0_!=2||c.geometryTerminal)return false;
    if(c.geometryEntered){
        if(!c.geometryComplete||c.geometryPolicy!=policy||c.geometryCells!=96||c.geometryFaces!=6)return false;
        for(const auto& object:c.objects7E8)
            if(!object||!object->geometryComplete||!C::current(*object))return false;
        pc=0x129924;return true;
    }
    const auto grid=[](std::size_t face,std::size_t column,std::size_t row)noexcept {
        const int right=(int(column)*128-256)*100,up=(256-int(row)*128)*100;
        switch(face){
        case 0:return std::array<float,3>{{float(right),float(up),25600.f}};
        case 1:return std::array<float,3>{{-25600.f,float(up),float(right)}};
        case 2:return std::array<float,3>{{float(-right),float(up),-25600.f}};
        case 3:return std::array<float,3>{{25600.f,float(up),float(-right)}};
        case 4:return std::array<float,3>{{float(right),25600.f,float(-up)}};
        default:return std::array<float,3>{{float(right),-25600.f,float(up)}};
        }
    };
    // Authenticate unchanged native corner/texture identities before source CPU
    // mutation. This is NOT procedural12ACD0 or tessellated12B0D0 equivalence:
    // native renderer retains its existing48 flat quads and largest-mip backend.
    const auto& proof=*images.storage_->nativeRaProof;
    const auto* mesh=images.mesh();const auto* spans=images.spans();
    if(!mesh||!spans||proof.mesh!=mesh||proof.spans!=spans||proof.bank!=images.bank()||
       spans->size()!=48||mesh->vertices.size()!=192||mesh->indices.size()!=288)return false;
    constexpr unsigned order[6]={0,1,2,2,1,3};
    for(std::size_t i=0;i<48;++i){const auto& span=(*spans)[i];const auto& tile=proof.tiles[i];
        if(span.face!=tile.face||span.tile!=tile.tile||span.face>=6||span.tile>=16||
           span.vertexStart!=4*i||span.vertexCount!=4||span.indexStart!=6*i||
           span.indexCount!=6||span.textureIndex!=i||
           tile.image!=images.bank()->texture(static_cast<std::uint32_t>(i)))return false;
        for(unsigned v=0;v<4;++v){
            const auto expected=grid(span.face,span.tile%4+(v>=2),span.tile/4+(v&1));
            const auto& actual=mesh->vertices[span.vertexStart+v];
            for(unsigned a=0;a<3;++a)
                if(!std::isfinite(actual.position[a])||actual.position[a]!=expected[a]+(a==1?1024.f:0.f))return false;
            // Numeric equality admits harmless signed-zero differences from
            // existing float-native face2/3 construction; not bitwise identity.
        }
        for(unsigned j=0;j<6;++j)if(mesh->indices[span.indexStart+j]!=span.vertexStart+order[j])return false;
    }
    c.geometryEntered=true;c.geometryTerminal=true;c.geometryPolicy=policy;
    const auto bits=[](float f)noexcept{std::uint32_t u;std::memcpy(&u,&f,4);return u;};
    const std::array<std::array<std::uint32_t,3>,6> normals{{
        {{0,0,0xbf800000u}},{{0x3f800000u,0,0}},{{0,0,0x3f800000u}},
        {{0xbf800000u,0,0}},{{0,0xbf800000u,0}},{{0,0x3f800000u,0}}}};
    //12ACC0 is a no-op: do not invent writes to temporary offsets0C/14.
    struct Vertex {std::array<float,3> xyz;std::uint32_t field10;
        std::array<float,2> uv;};
    for(std::size_t face=0;face<6;++face){
        std::array<Vertex,25> vertices;
        for(std::size_t row=0;row<5;++row)for(std::size_t col=0;col<5;++col){
            auto& vertex=vertices[row*5+col];vertex.xyz=grid(face,col,row);
            vertex.field10=0xffffffffu;vertex.uv={{float(col)*.25f,float(row)*.25f}};
            //Exact reached XYZ stores; normal W remains unrepresented/unknown.
            worldApplication_->skyNormals3E0E20_[face]=normals[face];
        }
        for(std::size_t row=0;row<4;++row)for(std::size_t col=0;col<4;++col){
            const auto cell=face*16+row*4+col;
            C::CellInput input;input.face=face;input.row=row;input.column=col;
            input.topLeft=vertices[row*5+col].xyz;input.right=vertices[row*5+col+1].xyz;
            input.below=vertices[(row+1)*5+col].xyz;input.diagonal=vertices[(row+1)*5+col+1].xyz;
            input.policy36C0F0=policy;input.subdivisionsX=worldApplication_->skySubdivisionsX36C0D8_;
            input.subdivisionsY=worldApplication_->skySubdivisionsY36C0E0_;
            if(!c.objects7E8[cell]||!C::buildGeometry12ACD0Or12B0D0(*c.objects7E8[cell],input,pc))return false;
            ++c.geometryCells;
        }
        //12AC18..44 follows all16 cells. Source temporarily copies unknown W
        //then overwrites it with1; no observer intervenes in this native domain.
        for(std::size_t i=0;i<25;++i){const auto& xyz=vertices[i].xyz;
            worldApplication_->skyPositions3E0E80_[face*25+i]=
                std::array<std::uint32_t,4>{{bits(xyz[0]),bits(xyz[1]),bits(xyz[2]),0x3f800000u}};
        }
        ++c.geometryFaces;
    }
    c.geometryComplete=true;c.geometryTerminal=false;pc=0x129924;return true;
}
}
