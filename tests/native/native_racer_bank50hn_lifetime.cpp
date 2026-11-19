// Lifetime emission only: preserves the original native owner and cleanup order.
#include "native_racer_bank50hn_fixture.h"

namespace native_racer50HN {
Bank::~Bank() noexcept {
        // Check BEFORE snapshots/spans and their externally borrowed owners die.
        if(nativeFifoPins_||pendingToken_||sourceIssuer_||animationIssuer_||mcQueryIssuer_||gbQueryIssuer_||mcQueryActive_||gbQueryActive_)std::terminate();
        // Only the RA-bound route adds a retirement obligation. The private
        // pin survives all snapshots/spans and checks its actual Follow owner.
        if(raWorldLifetime_&&(visiting_||readBlocked_))std::terminate();
    }
} // namespace native_racer50HN
