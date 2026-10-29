#pragma once
#include <memory>
#include <utility>
#include <thread>
#include <exception>
#include <cstddef>
namespace original_main_entry50KG {struct Application;}
namespace rr::game {
class OriginalRaceConstructor50LA;class OriginalRaceMusicManager50LA;
class OriginalPlayerConstructorPrefix50LA;
// Read-only identity retention of the ONE already-published Race10C owner.
// Not a new Music manager, backend readiness, or outer Race admission receipt.
class OriginalPlayerRaceMusicBorrow50LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalPlayerConstructorPrefix50LA;
    std::shared_ptr<const OriginalRaceConstructor50LA> race_;
    const OriginalRaceMusicManager50LA* music_=nullptr;
    std::size_t* borrowers_=nullptr;
    const std::thread::id thread_=std::this_thread::get_id();
    OriginalPlayerRaceMusicBorrow50LA(std::shared_ptr<const OriginalRaceConstructor50LA> race,
        const OriginalRaceMusicManager50LA* music,std::size_t& borrowers)
        :race_(std::move(race)),music_(music),borrowers_(&borrowers){++*borrowers_;}
public:
    ~OriginalPlayerRaceMusicBorrow50LA(){
        if(thread_!=std::this_thread::get_id()||!borrowers_||!*borrowers_)std::terminate();
        --*borrowers_; // Race keepalive remains until after this body.
    }
    OriginalPlayerRaceMusicBorrow50LA(const OriginalPlayerRaceMusicBorrow50LA&)=delete;
    OriginalPlayerRaceMusicBorrow50LA& operator=(const OriginalPlayerRaceMusicBorrow50LA&)=delete;
};
}
