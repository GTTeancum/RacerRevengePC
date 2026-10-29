//Included privately in the same OriginalApplicationEntities50LA process domain.
//This retains the exact3C81A0 startup prefix and its produced globals. It is NOT
//the live mutable36ADB8 namespace after later initializers/runtime appends.
    struct SourceCollisionCategoryStartup3C81A0 {
        std::uint32_t prefixCount00;
        std::array<std::array<char,32>,32> prefixNames04; //Only copied logical names/NUL are known.
        std::array<std::uint32_t,5> globals3E0708;
        std::array<std::array<std::uint32_t,4>,4> vectors3E02C0;
        SourceCollisionCategoryStartup3C81A0()noexcept {
            prefixCount00=0; //Actual3C81C0, not a BSS/fresh-cache assumption.
            //Actual3C8220/28/30/40 full vector stores from constructed stack40..70.
            vectors3E02C0[0]={{0,0,0,0x3f800000}};
            vectors3E02C0[1]={{0x3f800000,0,0,0x3f800000}};
            vectors3E02C0[2]={{0,0x3f800000,0,0x3f800000}};
            vectors3E02C0[3]={{0,0,0x3f800000,0x3f800000}};
            const char* const names[]={"SOFTFOLIAGE","SOLIDFOLIAGE","PHYSICSBODY","STATICOBJECT","TRIGGER"};
            for(std::size_t g=0;g<5;++g){
                std::uint32_t row=0;
                //308F00 is case insensitive. This closed producer's actual
                //literal/copy alphabet is uppercase ASCII, so equality is exact.
                for(;row<prefixCount00;++row)if(std::strcmp(names[g],prefixNames04[row].data())==0)break;
                const auto mask=std::uint32_t(1)<<row;
                if(row==prefixCount00){
                    std::memcpy(prefixNames04[row].data(),names[g],std::strlen(names[g])+1);
                    ++prefixCount00; //After genuine309230 logical copy.
                }
                //Actual3C82B0,3C8338,3C83C0,3C8448,3C84D0, in order.
                globals3E0708[g]=mask;
            }
        }
    };
    const SourceCollisionCategoryStartup3C81A0 collisionStartup3C81A0_;
