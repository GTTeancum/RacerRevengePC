#include "native_cold_query_role50lh.h"
#include "native_cold_model_retirement50lh.h"
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

using undertrack_owners50ew_fixture::ColdQueryRole50LH;
using undertrack_owners50ew_fixture::coldQueryRoleMatches50LH;
static_assert(coldQueryRoleMatches50LH(ColdQueryRole50LH::originalAi,1,true));
static_assert(coldQueryRoleMatches50LH(ColdQueryRole50LH::humanPlayer,0,false));
static_assert(!coldQueryRoleMatches50LH(ColdQueryRole50LH::originalAi,0,false));
static_assert(!coldQueryRoleMatches50LH(ColdQueryRole50LH::humanPlayer,1,true));
static_assert(!coldQueryRoleMatches50LH(ColdQueryRole50LH::originalAi,1,false));
static_assert(!coldQueryRoleMatches50LH(ColdQueryRole50LH::humanPlayer,0,true));
static_assert(!coldQueryRoleMatches50LH(ColdQueryRole50LH::originalAi,2,true));
static_assert(!coldQueryRoleMatches50LH(ColdQueryRole50LH::humanPlayer,2,false));
static_assert(!coldQueryRoleMatches50LH(static_cast<ColdQueryRole50LH>(255),0,false));
namespace {
struct Bank {bool& alive;explicit Bank(bool& v):alive(v){alive=true;}~Bank(){alive=false;}};
struct Model {
    bool& bankAlive;std::vector<int>& releases;int id;bool closed=false;
    Model(bool& alive,std::vector<int>& out,int value):bankAlive(alive),releases(out),id(value){}
    bool close() noexcept {if(closed)return true;if(!bankAlive)return false;
        releases.push_back(id);closed=true;return true;}
    ~Model(){if(!close())std::abort();}
};
struct Owners {
    std::array<std::unique_ptr<Model>,2> right,left,bottomRight,bottomLeft,chariot;
};
void populate(Owners& owners,bool& alive,std::vector<int>& releases){
    for(int slot=0;slot<2;++slot){int role=0;
        for(auto* list:{&owners.right,&owners.left,&owners.bottomRight,&owners.bottomLeft,&owners.chariot})
            (*list)[slot]=std::make_unique<Model>(alive,releases,slot*10+role++);
    }
}
void require(bool v,const char* message){if(!v)throw std::runtime_error(message);}
}
int main(){
    // Owners are deliberately declared before Bank, matching the failure.
    bool alive=false;std::vector<int> releases;releases.reserve(10);Owners owners;
    try {
        Bank bank(alive);
        native_racer50HN::BankBoundModelRetirement50LH<Owners> guard(owners,true);
        populate(owners,alive,releases);
        throw std::runtime_error("cold construction failure before renderer scope");
    }catch(const std::runtime_error&){}
    require(!alive,"Bank must have finished unwinding");
    require(releases==std::vector<int>({14,13,12,11,10,4,3,2,1,0}),"models must retire before Bank in reverse source order");
    for(auto* list:{&owners.right,&owners.left,&owners.bottomRight,&owners.bottomLeft,&owners.chariot})
        for(auto& model:*list){require(model&&model->closed,"model remains closed after Bank destruction");model.reset();}
    require(releases.size()==10,"later owner destructors must be idempotent");
    bool otherAlive=false;std::vector<int> otherReleases;otherReleases.reserve(10);Owners other;
    {
        Bank bank(otherAlive);
        populate(other,otherAlive,otherReleases);
        {native_racer50HN::BankBoundModelRetirement50LH<Owners> guard(other,false);}
        require(otherReleases.empty(),"disabled guard preserves separate RA ownership");
        for(auto* list:{&other.right,&other.left,&other.bottomRight,&other.bottomLeft,&other.chariot})
            for(auto& model:*list)require(model->close(),"explicit existing close must succeed");
        native_racer50HN::BankBoundModelRetirement50LH<Owners> guard(other,true);
        require(guard.close()&&otherReleases.size()==10,"normal explicit close remains idempotent");
    }
    std::cout<<"PASS: nine role assertions and cold exception/normal/disabled retirement checks\n";
}
