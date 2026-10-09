#include "candidate_handles50cb.h"
#include <cstdio>
using namespace rr::game;
int main(){
 CandidateHandle50CB owners[]={{10,7},{20,1},{30,4}};
 std::uint32_t ids[]={30,10,30,20};std::size_t scratch[4]{},out[4]{};
 if(resolveCandidateHandles50CB(ids,4,owners,3,scratch,4,out,4)!=CandidateHandleStatus50CB::Complete||out[0]!=4||out[1]!=7||out[2]!=4||out[3]!=1)return 1;
 ids[2]=999;
 if(resolveCandidateHandles50CB(ids,4,owners,3,scratch,4,out,4)!=CandidateHandleStatus50CB::UnknownIdentity||out[0]!=4||out[1]!=7||out[2]!=4||out[3]!=1)return 1;
 std::puts("50CB smoke passed: stable order/repeats, missing owner preserves output");
}
