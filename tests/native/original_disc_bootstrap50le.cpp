// Cold source-input regression. Run with RR_ORIGINAL_ASSET_ROOT set to a genuine
// original disc directory and RR_GENERATED_RUNTIME_ROOT set to an empty directory.
// This executable never opens the game, executes guest code or writes assets.
#include "runtime_startup50fl_fixture.h"
#include "original_disc_query_bootstrap50le.h"
#include <bit>
#include <iostream>

int main(){try{
    namespace f=runtime_startup50fl;
    namespace q=original_disc_query_bootstrap50LE;
    using namespace rr::game;
    f::require(std::fesetround(FE_TONEAREST)==0,"round-to-nearest unavailable");
    f::Assets assets(".",f::AssetInput::OriginalDisc);
    f::require(assets.originalArchive.size()==349110&&assets.phy.size()==6584&&
        assets.podShape.size()==1760,"original source extents differ");
    f::require(assets.hierarchy[0].nodeCount==14&&assets.hierarchy[1].nodeCount==14&&
        assets.hierarchy[2].nodeCount==18,"original source hierarchy differs");
    native_racer_collision50HW::Owner collision;std::string error;
    const auto endpoint=q::endpointPolicy();
    const bool prepared=collision.prepare({assets.originalArchivePath.string(),"Pods\\Pod01\\"},
        assets.originalArchive,endpoint,error);
    f::require(prepared,error.c_str());
    f::require(collision.main().nodeCount==23&&collision.main().rayCount==24&&
        collision.secondary().nodeCount==15&&collision.secondary().rayCount==16,
        "original COL node/ray extents differ");
    f::require(collision.receipt().memberBytes==std::array<std::size_t,4>{1404,956,1760,800},
        "original COL member extents differ");
    CollisionFilter48J mainFilter,secondaryFilter;Type2StaticQuery49X main,secondary;
    q::initialize(collision.main(),mainFilter,main,false);
    q::initialize(collision.secondary(),secondaryFilter,secondary,true);
    f::require(main.resource==&collision.main()&&secondary.resource==&collision.secondary()&&
        main.filter==&mainFilter&&secondary.filter==&secondaryFilter&&
        mainFilter.identity!=secondaryFilter.identity,"distinct owned query bindings required");
    f::require(mainFilter.categoryA4==OriginalNativeCollisionCategories::physicsBody&&
        secondaryFilter.categoryA4==OriginalNativeCollisionCategories::physicsBody&&
        mainFilter.categoryMasks.empty()&&secondaryFilter.categoryMasks==
            std::vector<std::uint32_t>{OriginalNativeCollisionCategories::pod},
        "source category construction differs");
    for(const auto* query:{&main,&secondary}){
        f::require(query->normalMode54Bound&&query->normalMode54==0&&
            query->copied40==std::array<float,4>{0.f,0.f,0.f,1.f}&&query->copied50==query->copied40,
            "source query initialization differs");
        for(std::size_t i=0;i<query->resource->rayCount;++i){
            Type2OpaqueEndpointWords49X words;
            f::require(endpoint.words(nullptr,native_racer_collision50HW::QueryBody::Main,i,words)&&
                std::bit_cast<std::uint32_t>(query->resource->rays[i].first.v[3])==words.first&&
                std::bit_cast<std::uint32_t>(query->resource->rays[i].second.v[3])==words.second,
                "explicit host opaque-W policy differs");
        }
    }
    // A rejected re-prepare cannot replace already-published original storage.
    const auto* published=collision.main().rays;
    f::require(!collision.prepare({assets.originalArchivePath.string(),"Pods\\Pod01\\"},
        assets.originalArchive,endpoint,error)&&collision.main().rays==published,
        "published collision owner was replaced");
    auto damaged=assets.originalArchive;damaged[0]^=1;
    native_racer_collision50HW::Owner rejected;
    f::require(!rejected.prepare({assets.originalArchivePath.string(),"Pods\\Pod01\\"},
        damaged,endpoint,error)&&!rejected.prepared(),"invalid archive was published");
    std::cout<<"original-disc bootstrap: authenticated POD01 geometry/PHY/COL and typed query owners; no generated inputs\n";
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
