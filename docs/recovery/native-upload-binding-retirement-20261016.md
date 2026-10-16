# Native direct-upload binding retirement

Prerequisite: original-native-frontend-upload-lifetime-20261016.patch on38b733e.
This is a concrete native capability-retirement operation, not a source13C heap
emulator or a claim that a source cached-transfer backend was constructed.

The SAME Application texture manager owns a private native binding generation.
Every genuine race/HUD/frontend binding captures it when minted by the actual
owner. The actual MaterialTextureGl transfer admission checks it before any
client-memory upload. Old copied requests fail after retirement; persistent
images and decoded data remain in their existing checked owners.

RaceDisplayLifetime::retireDirectUploadBindings2543F0 preflights owner thread,
active registered transfers and generation exhaustion before any drain. It then
calls its actual resetRenderer2450A0 exactly once. Failure preserves binding
generation; the reset helper preserves D8 if its drain fails. After success,
the nonthrowing generation increment retires all prior sealed capabilities.
New cold upload needs a newly minted actual-owner binding. This never silently
revalidates a stored frontend request or resurrects a previous display.

## Admitted native route and remaining limits

The implemented direct driver route binds its original request, copied primary/
opponent/track/cable/sky/specular/dynamic/ATM/Binder/particle requests, six HUD
local requests, and actual frontend coordinator requests. Client image transfers
all use MaterialTextureGl with PIXEL_UNPACK_BUFFER zero and actual decoded
client memory. Other direct renderer texImage2D calls allocate targets using
nullptr, so they own no client transfer borrow. Persistent target/image names
retain their separate retirement protocol.

No runtime destructor call is installed by this patch. Future source-order race
cleanup may use this concrete native direct-transfer operation only with the
above admitted route. Generic unbound MaterialTextureGl callers are not silently
certified; a new cached-transfer backend requires actual backing/alias retirement.
There is no all-covered flag, caller-supplied completion token, guessed null13C,
or252EA0 success stub. This operation does not close a renderer, free fonts or
materials, dispose race children, or complete the whole race destructor.

No builds, tests, syntax checks, gameplay or decoder execution were performed.
