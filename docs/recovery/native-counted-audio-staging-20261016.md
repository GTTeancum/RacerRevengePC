# Same-owner cold audio staging

`live_audio50HF::Owner::beginCountedRaceAudioStorage` initializes only its actual
mixer and stable voice storage. It admits one fresh owner and records the actual
mixer epoch and owner thread. It does not initialize group levels, bind any
provider, open output, or count an Entity.

Cold-only mixer/group borrows let the connected race construct its six groups,
then the actual world and seventh group, using these same objects. Both accessors
reject after preparation begins, native group retirement, owner close, a wrong
thread or changed mixer epoch. Callers must obey this lifetime; cached raw borrows
do not acquire independent ownership or extend the native owner lifetime.

The existing prepare method requires all seven actual counted groups on this
route, then performs the existing authenticated selected-bank/provider work and
single group configuration initialization. It never reinitializes the staged
mixer or changes its epoch. Legacy callers that do not begin cold staging retain
the old preparation order and behavior.

Failures leave counted groups owned by this same object. No failure silently
releases them or pretends original logical-sound callbacks ran. The caller closes
actual providers/output and invokes explicit race/world/group cleanup before
destroying this owner; its existing counted-group destructor guard remains.
An incomplete seven-group sequence rejects prepare without consuming its attempt,
so the caller can finish the real constructor sequence or explicitly unwind it.
A failure after authenticated preparation starts remains terminal as before.
Successful live-audio close is remembered only after both real announcer restoration
and output close succeed. Repeated close then avoids calling the retired group
authority; failed close remains retryable.
No driver call or Pause activation is included. Static review only.
