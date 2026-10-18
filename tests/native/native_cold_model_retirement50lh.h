#pragma once
#include <cstdio>
#include <exception>
#include <initializer_list>

namespace native_racer50HN {
// Declare immediately AFTER the actual Bank and before model construction.
// RuntimeOwners and its immutable backing must outlive both. This does not
// move/destroy models, release their backing, or replace Session retirement.
// It covers non-RA cold exceptions before the renderer cleanup scope exists.
template<class RuntimeOwners> class BankBoundModelRetirement50LH final {
    RuntimeOwners& owners_;
    const bool enabled_;
public:
    explicit BankBoundModelRetirement50LH(RuntimeOwners& owners,bool enabled) noexcept
        :owners_(owners),enabled_(enabled){}
    BankBoundModelRetirement50LH(const BankBoundModelRetirement50LH&)=delete;
    BankBoundModelRetirement50LH& operator=(const BankBoundModelRetirement50LH&)=delete;
    bool close() noexcept {
        if(!enabled_)return true;
        for(auto slot=owners_.right.size();slot>0;--slot){
            // Same reverse source order as the existing normal closeWindow.
            for(auto* model:{owners_.chariot[slot-1].get(),owners_.bottomLeft[slot-1].get(),
                owners_.bottomRight[slot-1].get(),owners_.left[slot-1].get(),owners_.right[slot-1].get()})
                if(model&&!model->close())return false;
        }
        return true;
    }
    ~BankBoundModelRetirement50LH() noexcept {
        if(!close()){
            std::fputs("native_cold_model_retirement_rejected before Bank destruction\n",stderr);
            std::terminate();
        }
    }
};
}
